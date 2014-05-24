/*
 GRT MIT License
 Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial
 portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "DTW.h"

namespace GRT{
    
//Register the DTW module with the Classifier base class
RegisterClassifierModule< DTW > DTW::registerModule("DTW");

DTW::DTW(bool useScaling,bool useNullRejection,double nullRejectionCoeff,UINT rejectionMode,bool constrainWarpingPath,double radius,bool offsetUsingFirstSample,bool useSmoothing,UINT smoothingFactor)
{
    this->useScaling=useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    this->rejectionMode = rejectionMode;
    this->constrainWarpingPath = constrainWarpingPath;
    this->radius = radius;
    this->offsetUsingFirstSample = offsetUsingFirstSample;
    this->useSmoothing = useSmoothing;
    this->smoothingFactor = smoothingFactor;

	trained=false;
	useZNormalisation=false;
	constrainZNorm=false;
    trimTrainingData = false;

	zNormConstrainThreshold=0.2;
	trimThreshold = 0.1;
	maximumTrimPercentage = 90;

	numTemplates=0;
	distanceMethod=EUCLIDEAN_DIST;

	averageTemplateLength =0;

    classType = "DTW";
    classifierType = classType;
    classifierMode = TIMESERIES_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG NDDTW]");
    errorLog.setProceedingText("[ERROR NDDTW]");
    trainingLog.setProceedingText("[TRAINING NDDTW]");
    warningLog.setProceedingText("[WARNING NDDTW]");
}

DTW::DTW(const DTW &rhs){
    *this = rhs;
}

DTW::~DTW(void)
{
}

DTW& DTW::operator=(const DTW &rhs){
	
	if( this != &rhs ){
		
		this->templatesBuffer = rhs.templatesBuffer;
        this->distanceMatrices = rhs.distanceMatrices;
        this->warpPaths = rhs.warpPaths;
        this->continuousInputDataBuffer = rhs.continuousInputDataBuffer;
        this->numTemplates = rhs.numTemplates;
        this->rejectionMode = rhs.rejectionMode;
        this->useSmoothing = rhs.useSmoothing;
        this->useZNormalisation = rhs.useZNormalisation;
        this->constrainZNorm = rhs.constrainZNorm;
        this->constrainWarpingPath = rhs.constrainWarpingPath;
        this->trimTrainingData = rhs.trimTrainingData;
        this->zNormConstrainThreshold = rhs.zNormConstrainThreshold;
        this->radius = rhs.radius;
        this->offsetUsingFirstSample = rhs.offsetUsingFirstSample;
        this->trimThreshold = rhs.trimThreshold;
        this->maximumTrimPercentage = rhs.maximumTrimPercentage;
        this->smoothingFactor = rhs.smoothingFactor;
        this->distanceMethod = rhs.distanceMethod;
        this->rejectionMode = rhs.rejectionMode;
        this->averageTemplateLength = rhs.averageTemplateLength;

	    //Copy the classifier variables
		copyBaseVariables( (Classifier*)&rhs );
	}
	return *this;
}

bool DTW::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        
        DTW *ptr = (DTW*)classifier;
        this->templatesBuffer = ptr->templatesBuffer;
        this->distanceMatrices = ptr->distanceMatrices;
        this->warpPaths = ptr->warpPaths;
        this->continuousInputDataBuffer = ptr->continuousInputDataBuffer;
        this->numTemplates = ptr->numTemplates;
        this->rejectionMode = ptr->rejectionMode;
        this->useSmoothing = ptr->useSmoothing;
        this->useZNormalisation = ptr->useZNormalisation;
        this->constrainZNorm = ptr->constrainZNorm;
        this->constrainWarpingPath = ptr->constrainWarpingPath;
        this->trimTrainingData = ptr->trimTrainingData;
        this->zNormConstrainThreshold = ptr->zNormConstrainThreshold;
        this->radius = ptr->radius;
        this->offsetUsingFirstSample = ptr->offsetUsingFirstSample;
        this->trimThreshold = ptr->trimThreshold;
        this->maximumTrimPercentage = ptr->maximumTrimPercentage;
        this->smoothingFactor = ptr->smoothingFactor;
        this->distanceMethod = ptr->distanceMethod;
        this->rejectionMode = ptr->rejectionMode;
        this->averageTemplateLength = ptr->averageTemplateLength;
        
	    //Copy the classifier variables
		return copyBaseVariables( classifier );
    }
    return false;
}

////////////////////////// TRAINING FUNCTIONS //////////////////////////
bool DTW::train_(TimeSeriesClassificationData &labelledTrainingData){

	UINT bestIndex = 0;

	//Cleanup Memory
	templatesBuffer.clear();
    classLabels.clear();
	trained = false;
    continuousInputDataBuffer.clear();

    if( trimTrainingData ){
        TimeSeriesClassificationSampleTrimmer timeSeriesTrimmer(trimThreshold,maximumTrimPercentage);
        TimeSeriesClassificationData tempData;
        tempData.setNumDimensions( labelledTrainingData.getNumDimensions() );
        
        for(UINT i=0; i<labelledTrainingData.getNumSamples(); i++){
            if( timeSeriesTrimmer.trimTimeSeries( labelledTrainingData[i] ) ){
                tempData.addSample(labelledTrainingData[i].getClassLabel(), labelledTrainingData[i].getData());
            }else{
                trainingLog << "Removing training sample " << i << " from the dataset as it could not be trimmed!" << endl;
            }
        }
        //Overwrite the original training data with the trimmed dataset
        labelledTrainingData = tempData;
    }
    
    if( labelledTrainingData.getNumSamples() == 0 ){
        errorLog << "train_(TimeSeriesClassificationData &labelledTrainingData) - Can't train model as there are no samples in training data!" << endl;
        return false;
    }

	//Assign
    numClasses = labelledTrainingData.getNumClasses();
	numTemplates = labelledTrainingData.getNumClasses();
    numInputDimensions = labelledTrainingData.getNumDimensions();
	templatesBuffer.resize( numClasses );
    classLabels.resize( numClasses );
	nullRejectionThresholds.resize( numClasses );
	averageTemplateLength = 0;

	//Need to copy the labelled training data incase we need to scale it or znorm it
	TimeSeriesClassificationData trainingData( labelledTrainingData );

	//Perform any scaling or normalisation
    ranges = trainingData.getRanges();
	if( useScaling ) scaleData( trainingData );
	if( useZNormalisation ) znormData( trainingData );

	//For each class, run a one-to-one DTW and find the template the best describes the data
	for(UINT k=0; k<numTemplates; k++){
        //Get the class label for the cth class
        UINT classLabel = trainingData.getClassTracker()[k].classLabel;
        TimeSeriesClassificationData classData = trainingData.getClassData( classLabel );
		UINT numExamples = classData.getNumSamples();
		bestIndex = 0;

        //Set the class label of this template
        templatesBuffer[k].classLabel = classLabel;

        //Set the kth class label
        classLabels[k] = classLabel;
        
        trainingLog << "Training Template: " << k << " Class: " << classLabel << endl;

		//Check to make sure we actually have some training examples
		if( numExamples < 1 ){
            errorLog << "train_(TimeSeriesClassificationData &labelledTrainingData) - Can not train model: Num of Example is < 1! Class: " << classLabel << ". Turn off null rejection if you want to use DTW with only 1 training sample per class." << endl;
			return false;
		}
        
        if( numExamples == 1 && useNullRejection ){
            errorLog << "train_(TimeSeriesClassificationData &labelledTrainingData) - Can not train model as there is only 1 example in class: " << classLabel << ". Turn off null rejection if you want to use DTW with only 1 training sample per class." << endl;
			return false;
		}

		if( numExamples == 1 ){//If we have just one training example then we have to use it as the template
            bestIndex = 0;
            nullRejectionThresholds[k] = 0.0;//TODO-We need a better way of calculating this!
		}else{
            //Search for the best training example for this class
			if( !train_NDDTW(classData,templatesBuffer[k],bestIndex) ){
                errorLog << "train_(LabelledTimeSeriesClassificationData &labelledTrainingData) - Failed to train template for class with label: " << classLabel << endl;
                return false;
            }
		}

		//Add the template with the best index to the buffer
		int trainingMethod = 0;
		if(useSmoothing) trainingMethod = 1;

		switch (trainingMethod) {
			case(0)://Standard Training
				templatesBuffer[k].timeSeries = classData[bestIndex].getData();
				break;
			case(1)://Training using Smoothing
				//Smooth the data, reducing its size by a factor set by smoothFactor
				smoothData(classData[ bestIndex ].getData(),smoothingFactor,templatesBuffer[k].timeSeries);
				break;
			default:
				cout<<"Can not train model: Unknown training method \n";
				return false;
				break;
		}
        
        if( offsetUsingFirstSample ){
            offsetTimeseries( templatesBuffer[k].timeSeries );
        }

		//Add the average length of the training examples for this template to the overall averageTemplateLength
		averageTemplateLength += templatesBuffer[k].averageTemplateLength;
	}

    //Flag that the models have been trained
	trained = true;
	averageTemplateLength = (UINT) averageTemplateLength/double(numTemplates);

    //Recompute the null rejection thresholds
    recomputeNullRejectionThresholds();

    //Resize the prediction results to make sure it is setup for realtime prediction
    continuousInputDataBuffer.clear();
    continuousInputDataBuffer.resize(averageTemplateLength,vector<double>(numInputDimensions,0));
    classLikelihoods.resize(numTemplates,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numTemplates,0);
    predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;

	//Training complete
	return true;
}

bool DTW::train_NDDTW(TimeSeriesClassificationData &trainingData,DTWTemplate &dtwTemplate,UINT &bestIndex){

   UINT numExamples = trainingData.getNumSamples();
   VectorDouble results(numExamples,0.0);
   MatrixDouble distanceResults(numExamples,numExamples);
   dtwTemplate.averageTemplateLength = 0;
    
   for(UINT m=0; m<numExamples; m++){
       
	   MatrixDouble templateA; //The m'th template
	   MatrixDouble templateB; //The n'th template
	   dtwTemplate.averageTemplateLength += trainingData[m].getLength();

	   //Smooth the data if required
	   if( useSmoothing ) smoothData(trainingData[m].getData(),smoothingFactor,templateA);
	   else templateA = trainingData[m].getData();
       
       if( offsetUsingFirstSample ){
           offsetTimeseries(templateA);
       }

	   for(UINT n=0; n<numExamples; n++){
		if(m!=n){
		    //Smooth the data if required
		    if( useSmoothing ) smoothData(trainingData[n].getData(),smoothingFactor,templateB);
		    else templateB = trainingData[n].getData();
            
            if( offsetUsingFirstSample ){
                offsetTimeseries(templateB);
            }

			//Compute the distance between the two time series
            MatrixDouble distanceMatrix(templateA.getNumRows(),templateB.getNumRows());
            vector< IndexDist > warpPath;
			double dist = computeDistance(templateA,templateB,distanceMatrix,warpPath);
            
            trainingLog << "Template: " << m << " Timeseries: " << n << " Dist: " << dist << endl;

			//Update the results values
			distanceResults[m][n] = dist;
			results[m] += dist;
		}else distanceResults[m][n] = 0; //The distance is zero because the two timeseries are the same
	   }
   }

	for(UINT m=0; m<numExamples; m++) results[m]/=(numExamples-1);
	//Find the best average result, this is the result with the minimum value
	bestIndex = 0;
	double bestAverage = results[0];
	for(UINT m=1; m<numExamples; m++){
		if( results[m] < bestAverage ){
			bestAverage = results[m];
			bestIndex = m;
		}
	}

    if( numExamples > 2 ){

        //Work out the threshold value for the best template
        dtwTemplate.trainingMu = results[bestIndex];
        dtwTemplate.trainingSigma = 0.0;

        for(UINT n=0; n<numExamples; n++){
            if(n!=bestIndex){
                dtwTemplate.trainingSigma += SQR( distanceResults[ bestIndex ][n] - dtwTemplate.trainingMu );
            }
        }
        dtwTemplate.trainingSigma = sqrt( dtwTemplate.trainingSigma / double(numExamples-2) );
    }else{
        warningLog << "_train_NDDTW(TimeSeriesClassificationData &trainingData,DTWTemplate &dtwTemplate,UINT &bestIndex - There are not enough examples to compute the trainingMu and trainingSigma for the template for class " << dtwTemplate.classLabel << endl;
        dtwTemplate.trainingMu = 0.0;
        dtwTemplate.trainingSigma = 0.0;
    }

	//Set the average length of the training examples
	dtwTemplate.averageTemplateLength = (UINT) (dtwTemplate.averageTemplateLength/double(numExamples));
    
    trainingLog << "AverageTemplateLength: " << dtwTemplate.averageTemplateLength << endl;

    //Flag that the training was successfull
	return true;
}


bool DTW::predict_(MatrixDouble &inputTimeSeries){

    if( !trained ){
        errorLog << "predict_(MatrixDouble &inputTimeSeries) - The DTW templates have not been trained!" << endl;
        return false;
    }

    if( classLikelihoods.size() != numTemplates ) classLikelihoods.resize(numTemplates);
    if( classDistances.size() != numTemplates ) classDistances.resize(numTemplates);

    predictedClassLabel = 0;
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    for(UINT k=0; k<classLikelihoods.size(); k++){
        classLikelihoods[k] = 0;
        classDistances[k] = DEFAULT_NULL_LIKELIHOOD_VALUE;
    }

	if( numInputDimensions != inputTimeSeries.getNumCols() ){
        errorLog << "predict_(MatrixDouble &inputTimeSeries) - The number of features in the model (" << numInputDimensions << ") do not match that of the input time series (" << inputTimeSeries.getNumCols() << ")" << endl;
        return false;
    }

	//Perform any preprocessing if requried
    MatrixDouble *timeSeriesPtr = &inputTimeSeries;
    MatrixDouble processedTimeSeries;
    MatrixDouble tempMatrix;
	if(useScaling){
        scaleData(*timeSeriesPtr,processedTimeSeries);
        timeSeriesPtr = &processedTimeSeries;
    }
    
    //Normalize the data if needed
	if( useZNormalisation ){
        znormData(*timeSeriesPtr,processedTimeSeries);
        timeSeriesPtr = &processedTimeSeries;
    }

	//Smooth the data if required
	if( useSmoothing ){
		smoothData(*timeSeriesPtr,smoothingFactor,tempMatrix);
		timeSeriesPtr = &tempMatrix;
	}
    
    //Offset the timeseries if required
    if( offsetUsingFirstSample ){
        offsetTimeseries( *timeSeriesPtr );
    }

	//Make the prediction by finding the closest template
    double sum = 0;
    if( distanceMatrices.size() != numTemplates ) distanceMatrices.resize( numTemplates );
    if( warpPaths.size() != numTemplates ) warpPaths.resize( numTemplates );
    
	//Test the timeSeries against all the templates in the timeSeries buffer
	for(UINT k=0; k<numTemplates; k++){
		//Perform DTW
		classDistances[k] = computeDistance(templatesBuffer[k].timeSeries,*timeSeriesPtr,distanceMatrices[k],warpPaths[k]);
        classLikelihoods[k] = classDistances[k];
        sum += classLikelihoods[k];
	}

	//See which gave the min distance
	UINT closestTemplateIndex = 0;
	bestDistance = classDistances[0];
	for(UINT k=1; k<numTemplates; k++){
		if( classDistances[k] < bestDistance ){
			bestDistance = classDistances[k];
			closestTemplateIndex = k;
		}
	}
    
    //Normalize the class likelihoods and check which class has the maximum likelihood
    UINT maxLikelihoodIndex = 0;
    maxLikelihood = 0;
    for(UINT k=0; k<numTemplates; k++){
        classLikelihoods[k] = (sum-classLikelihoods[k])/sum;
        if( classLikelihoods[k] > maxLikelihood ){
            maxLikelihood = classLikelihoods[k];
            maxLikelihoodIndex = k;
        }
    }

    if( useNullRejection ){

        switch( rejectionMode ){
            case TEMPLATE_THRESHOLDS:
                if( bestDistance <= nullRejectionThresholds[ closestTemplateIndex ] ) predictedClassLabel = templatesBuffer[ closestTemplateIndex ].classLabel;
                else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
                break;
            case CLASS_LIKELIHOODS:
                if( maxLikelihood >= 0.99 )  predictedClassLabel = templatesBuffer[ maxLikelihoodIndex ].classLabel;
                else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
                break;
            case THRESHOLDS_AND_LIKELIHOODS:
                if( bestDistance <= nullRejectionThresholds[ closestTemplateIndex ] && maxLikelihood >= 0.99 )
                    predictedClassLabel = templatesBuffer[ closestTemplateIndex ].classLabel;
                else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
                break;
            default:
                errorLog << "predict_(MatrixDouble &timeSeries) - Unknown RejectionMode!" << endl;
                return false;
                break;
        }

	}else predictedClassLabel = templatesBuffer[ closestTemplateIndex ].classLabel;

    return true;
}

bool DTW::predict_(VectorDouble &inputVector){

    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - The model has not been trained!" << endl;
        return false;
    }
    predictedClassLabel = 0;
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    for(UINT c=0; c<classLikelihoods.size(); c++){
        classLikelihoods[c] = DEFAULT_NULL_LIKELIHOOD_VALUE;
    }

	if( numInputDimensions != inputVector.size() ){
        errorLog << "predict_(VectorDouble &inputVector) - The number of features in the model " << numInputDimensions << " does not match that of the input vector " << inputVector.size() << endl;
        return false;
    }

    //Add the new input to the circular buffer
    continuousInputDataBuffer.push_back( inputVector );

    if( continuousInputDataBuffer.getNumValuesInBuffer() < averageTemplateLength ){
        //We haven't got enough samples yet so can't do the prediction
        return true;
    }

    //Copy the data into a temporary matrix
    MatrixDouble predictionTimeSeries(continuousInputDataBuffer.getSize(),numInputDimensions);
    for(UINT i=0; i<predictionTimeSeries.getNumRows(); i++){
        for(UINT j=0; j<predictionTimeSeries.getNumCols(); j++){
            predictionTimeSeries[i][j] = continuousInputDataBuffer[i][j];
        }
    }

    //Run the prediction
    return predict( predictionTimeSeries );

}

bool DTW::reset(){
    continuousInputDataBuffer.clear();
    if( trained ){
        continuousInputDataBuffer.resize(averageTemplateLength,vector<double>(numInputDimensions,0));
        recomputeNullRejectionThresholds();
    }
    return true;
}
    
bool DTW::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the DTW model
    templatesBuffer.clear();
    distanceMatrices.clear();
    warpPaths.clear();
    continuousInputDataBuffer.clear();
    
    return true;
}

bool DTW::recomputeNullRejectionThresholds(){
	if(!trained) return false;

    //Copy the null rejection thresholds into one buffer so they can easily be accessed from the base class
    nullRejectionThresholds.resize(numTemplates);

	for(UINT k=0; k<numTemplates; k++){
		//The threshold is set as the mean distance plus gamma standard deviations
		nullRejectionThresholds[k] = templatesBuffer[k].trainingMu + (templatesBuffer[k].trainingSigma * nullRejectionCoeff);
	}

	return true;
}

bool DTW::setModels( vector< DTWTemplate > newTemplates ){
	
	if( newTemplates.size() == templatesBuffer.size() ){
		templatesBuffer = newTemplates;
		//Make sure the class labels have not changed
		classLabels.resize( templatesBuffer.size() );
		for(UINT i=0; i<templatesBuffer.size(); i++){
			classLabels[i] = templatesBuffer[i].classLabel;
		}
		return true;
	}
	return false;
}

////////////////////////// computeDistance ///////////////////////////////////////////

double DTW::computeDistance(MatrixDouble &timeSeriesA,MatrixDouble &timeSeriesB,MatrixDouble &distanceMatrix,vector< IndexDist > &warpPath){

	const int M = timeSeriesA.getNumRows();
	const int N = timeSeriesB.getNumRows();
	const int C = timeSeriesA.getNumCols();
	int i,j,k,index = 0;
	double totalDist,v,normFactor = 0.;
    
    warpPath.clear();
    if( int(distanceMatrix.getNumRows()) != M || int(distanceMatrix.getNumCols()) != N ){
        distanceMatrix.resize(M, N);
    }

	switch (distanceMethod) {
		case (ABSOLUTE_DIST):
			for(i=0; i<M; i++){
				for(j=0; j<N; j++){
					distanceMatrix[i][j] = 0.0;
					for(k=0; k< C; k++){
					   distanceMatrix[i][j] += fabs(timeSeriesA[i][k]-timeSeriesB[j][k]);
					}
				}
			}
			break;
		case (EUCLIDEAN_DIST):
			//Calculate Euclidean Distance for all possible values
			for(i=0; i<M; i++){
				for(j=0; j<N; j++){
					distanceMatrix[i][j] = 0.0;
					for(k=0; k< C; k++){
						distanceMatrix[i][j] += SQR( timeSeriesA[i][k]-timeSeriesB[j][k] );
					}
					distanceMatrix[i][j] = sqrt( distanceMatrix[i][j] );
				}
			}
			break;
		case (NORM_ABSOLUTE_DIST):
			for(i=0; i<M; i++){
				for(j=0; j<N; j++){
					distanceMatrix[i][j] = 0.0;
					for(k=0; k< C; k++){
					   distanceMatrix[i][j] += fabs(timeSeriesA[i][k]-timeSeriesB[j][k]);
					}
					distanceMatrix[i][j]/=N;
				}
			}
			break;
		default:
			errorLog<<"ERROR: Unknown distance method: "<<distanceMethod<<endl;
			return -1;
			break;
	}

    //Run the recursive search function to build the cost matrix
    double distance = sqrt( d(M-1,N-1,distanceMatrix,M,N) );

    if( std::isinf(distance) || std::isnan(distance) ){
        warningLog << "DTW computeDistance(...) - Distance Matrix Values are INF!" << endl;
        return INFINITY;
    }
    
    //cout << "DIST: " << distance << endl;

    //The distMatrix values are negative so make them positive
    for(i=0; i<M; i++){
        for(j=0; j<N; j++){
            distanceMatrix[i][j] = fabs( distanceMatrix[i][j] );
        }
    }

	//Now Create the Warp Path through the cost matrix, starting at the end
    i=M-1;
	j=N-1;
	totalDist = distanceMatrix[i][j];
    warpPath.push_back( IndexDist(i,j,distanceMatrix[i][j]) );
    
	//Use dynamic programming to navigate through the cost matrix until [0][0] has been reached
    normFactor = 1;
	while( true ) {
        if( i==0 && j==0 ) break;
		if( i==0 ){ j--; }
        else{ 
            if( j==0 ) i--;
            else{
                //Find the minimum cell to move to
                v = numeric_limits<double>::max();
                index = 0;
                if( distanceMatrix[i-1][j] < v ){ v = distanceMatrix[i-1][j]; index = 1; }
                if( distanceMatrix[i][j-1] < v ){ v = distanceMatrix[i][j-1]; index = 2; }
                if( distanceMatrix[i-1][j-1] <= v ){ index = 3; }
                switch(index){
                    case(1):
                        i--;
                        break;
                    case(2):
                        j--;
                        break;
                    case(3):
                        i--;
                        j--;
                        break;
                    default:
                        warningLog << "DTW computeDistance(...) - Could not compute a warping path for the input matrix! Dist: " << distanceMatrix[i-1][j] << " i: " << i << " j: " << j << endl;
                        return INFINITY;
                        break;
                }
            }
        }
        normFactor++;
		totalDist += distanceMatrix[i][j];
		warpPath.push_back( IndexDist(i,j,distanceMatrix[i][j]) );
	}

	return totalDist/normFactor;
}

double DTW::d(int m,int n,MatrixDouble &distanceMatrix,const int M,const int N){
    double dist = 0;
    //The following is based on Matlab code by Eamonn Keogh and Michael Pazzani
    
    //If this cell is NAN then it has already been flagged as unreachable
    if( std::isnan( distanceMatrix[m][n] ) ){
        return NAN;
    }

    if( constrainWarpingPath ){
        double r = ceil( min(M,N)*radius );
        //Test to see if the current cell is outside of the warping window
        if( fabs( n-((N-1)/((M-1)/double(m))) ) > r ){
            if( n-((N-1)/((M-1)/double(m))) > 0 ){
                for(int i=0; i<m; i++){
                    for(int j=n; j<N; j++){
                        distanceMatrix[i][j] = NAN;
                    }
                }
            }else{
                for(int i=m; i<M; i++){
                    for(int j=0; j<n; j++){
                        distanceMatrix[i][j] = NAN;
                    }
                }
            }
            return NAN;
        }
    }

    //If this cell contains a negative value then it has already been searched
    //The cost is therefore the absolute value of the negative value so return it
    if( distanceMatrix[m][n] < 0 ){
        dist = fabs( distanceMatrix[m][n] );
        return dist;
    }

    //Case 1: A warping path has reached the end
    //Return the contribution of distance
    //Negate the value, to record the fact that this cell has been visited
    //End of recursion
    if( m == 0 && n == 0 ){
        dist = distanceMatrix[0][0];
        distanceMatrix[0][0] = -distanceMatrix[0][0];
        return dist;
    }

    //Case 2: we are somewhere in the top row of the matrix
    //Only need to consider moving left
    if( m == 0 ){
        double contribDist = d(m,n-1,distanceMatrix,M,N);

        dist = distanceMatrix[m][n] + contribDist;

        distanceMatrix[m][n] = -dist;
        return dist;
    }else{
        //Case 3: we are somewhere in the left column of the matrix
        //Only need to consider moving down
        if ( n == 0) {
            double contribDist = d(m-1,n,distanceMatrix,M,N);

            dist = distanceMatrix[m][n] + contribDist;

            distanceMatrix[m][n] = -dist;
            return dist;
        }else{
            //Case 4: We are somewhere away from the edges so consider moving in the three main directions
            double contribDist1 = d(m-1,n-1,distanceMatrix,M,N);
            double contribDist2 = d(m-1,n,distanceMatrix,M,N);
            double contribDist3 = d(m,n-1,distanceMatrix,M,N);
            double minValue = numeric_limits<double>::max();
            int index = 0;
            if( contribDist1 < minValue ){ minValue = contribDist1; index = 1; }
			if( contribDist2 < minValue ){ minValue = contribDist2; index = 2; }
			if( contribDist3 < minValue ){ minValue = contribDist3; index = 3; }

            switch ( index ) {
                case 1:
                    dist = distanceMatrix[m][n] + minValue;
                    break;
                case 2:
                    dist = distanceMatrix[m][n] + minValue;
                    break;
                case 3:
                    dist = distanceMatrix[m][n] + minValue;
                    break;

                default:
                    break;
            }

            distanceMatrix[m][n] = -dist; //Negate the value to record that it has been visited
            return dist;
        }
    }

    //This should not happen!
    return dist;
}

inline double DTW::MIN_(double a,double b, double c){
	double v = a;
	if(b<v) v = b;
	if(c<v) v = c;
	return v;
}


////////////////////////// SCALING AND NORMALISATION FUNCTIONS //////////////////////////

void DTW::scaleData(TimeSeriesClassificationData &trainingData){

	//Scale the data using the min and max values
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        scaleData( trainingData[i].getData(), trainingData[i].getData() );
    }

}

void DTW::scaleData(MatrixDouble &data,MatrixDouble &scaledData){

	const UINT R = data.getNumRows();
	const UINT C = data.getNumCols();

    if( scaledData.getNumRows() != R || scaledData.getNumCols() != C ){
        scaledData.resize(R, C);
    }

	//Scale the data using the min and max values
	for(UINT i=0; i<R; i++)
		for(UINT j=0; j<C; j++)
			scaledData[i][j] = scale(data[i][j],ranges[j].minValue,ranges[j].maxValue,0.0,1.0);

}

void DTW::znormData(TimeSeriesClassificationData &trainingData){

    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        znormData( trainingData[i].getData(), trainingData[i].getData() );
    }

}

void DTW::znormData(MatrixDouble &data,MatrixDouble &normData){

	const UINT R = data.getNumRows();
	const UINT C = data.getNumCols();

    if( normData.getNumRows() != R || normData.getNumCols() != C ){
        normData.resize(R,C);
    }

	for(UINT j=0; j<C; j++){
		double mean = 0.0;
		double stdDev = 0.0;

		//Calculate Mean
		for(UINT i=0; i<R; i++) mean += data[i][j];
		mean /= double(R);

		//Calculate Std Dev
		for(UINT i=0; i<R; i++)
			stdDev += SQR(data[i][j]-mean);
		stdDev = sqrt( stdDev / (R - 1.0) );

		if(constrainZNorm && stdDev < 0.01){
			//Normalize the data to 0 mean
		    for(UINT i=0; i<R; i++)
			normData[i][j] = (data[i][j] - mean);
		}else{
			//Normalize the data to 0 mean and standard deviation of 1
		    for(UINT i=0; i<R; i++)
			normData[i][j] = (data[i][j] - mean) / stdDev;
		}
	}
}

void DTW::smoothData(VectorDouble &data,UINT smoothFactor,VectorDouble &resultsData){

	const UINT M = (UINT)data.size();
	const UINT N = (UINT) floor(double(M)/double(smoothFactor));
	resultsData.resize(N,0);
	for(UINT i=0; i<N; i++) resultsData[i]=0.0;

	if(smoothFactor==1 || M<smoothFactor){
		resultsData = data;
		return;
	}

	for(UINT i=0; i<N; i++){
	    double mean = 0.0;
		UINT index = i*smoothFactor;
		for(UINT x=0; x<smoothFactor; x++){
			mean += data[index+x];
		}
		resultsData[i] = mean/smoothFactor;
	}
	//Add on the data that does not fit into the window
	if(M%smoothFactor!=0.0){
		double mean = 0.0;
			for(UINT i=N*smoothFactor; i<M; i++) mean += data[i];
        mean/=M-(N*smoothFactor);
		//Add one to the end of the vector
		VectorDouble tempVector(N+1);
		for(UINT i=0; i<N; i++) tempVector[i] = resultsData[i];
		tempVector[N] = mean;
		resultsData = tempVector;
	}

}

void DTW::smoothData(MatrixDouble &data,UINT smoothFactor,MatrixDouble &resultsData){

	const UINT M = data.getNumRows();
	const UINT C = data.getNumCols();
	const UINT N = (UINT) floor(double(M)/double(smoothFactor));
	resultsData.resize(N,C);

	if(smoothFactor==1 || M<smoothFactor){
		resultsData = data;
		return;
	}

	for(UINT i=0; i<N; i++){
		for(UINT j=0; j<C; j++){
	     double mean = 0.0;
		 int index = i*smoothFactor;
		 for(UINT x=0; x<smoothFactor; x++){
			mean += data[index+x][j];
		 }
		 resultsData[i][j] = mean/smoothFactor;
		}
	}

	//Add on the data that does not fit into the window
	if(M%smoothFactor!=0.0){
		VectorDouble mean(C,0.0);
		for(UINT j=0; j<C; j++){
		 for(UINT i=N*smoothFactor; i<M; i++) mean[j] += data[i][j];
		 mean[j]/=M-(N*smoothFactor);
		}

		//Add one row to the end of the Matrix
		MatrixDouble tempMatrix(N+1,C);

		for(UINT i=0; i<N; i++)
			for(UINT j=0; j<C; j++)
				tempMatrix[i][j] = resultsData[i][j];

        for(UINT j=0; j<C; j++) tempMatrix[N][j] = mean[j];
		resultsData = tempMatrix;
	}

}

////////////////////////////// SAVE & LOAD FUNCTIONS ////////////////////////////////

bool DTW::saveModelToFile( string fileName ) const{

    std::fstream file;

    if(!trained){
       errorLog << "saveModelToFile( string fileName ) - Model not trained yet, can not save to file" << endl;
     return false;
    }

    file.open(fileName.c_str(), std::ios::out);

    if( !saveModelToFile( file ) ){
        return false;
    }

    file.close();
    return true;
}
    
bool DTW::saveModelToFile( fstream &file ) const{
    
    if(!file.is_open()){
        errorLog << "saveModelToFile( string fileName ) - Could not open file to save data" << endl;
        return false;
    }
    
    file << "GRT_DTW_Model_File_V2.0" <<endl;
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    file << "DistanceMethod: ";
    switch(distanceMethod){
        case(ABSOLUTE_DIST):
            file <<ABSOLUTE_DIST<<endl;
            break;
        case(EUCLIDEAN_DIST):
            file <<EUCLIDEAN_DIST<<endl;
            break;
        default:
            file <<ABSOLUTE_DIST<<endl;
            break;
    }
    file << "UseSmoothing: "<<useSmoothing<<endl;
    file << "SmoothingFactor: "<<smoothingFactor<<endl;
    file << "UseZNormalisation: "<<useZNormalisation<<endl;
    file << "OffsetUsingFirstSample: " << offsetUsingFirstSample << endl;
    file << "ConstrainWarpingPath: " << constrainWarpingPath << endl;
    file << "Radius: " << radius << endl;
    file << "RejectionMode: " << rejectionMode<< endl;
    
    if( trained ){
        file << "NumberOfTemplates: " << numTemplates << endl;
        file << "OverallAverageTemplateLength: " << averageTemplateLength << endl;
        //Save each template
        for(UINT i=0; i<numTemplates; i++){
            file << "***************TEMPLATE***************" << endl;
            file << "Template: " << i+1 << endl;
            file << "ClassLabel: " << templatesBuffer[i].classLabel << endl;
            file << "TimeSeriesLength: " << templatesBuffer[i].timeSeries.getNumRows() << endl;
            file << "TemplateThreshold: " << nullRejectionThresholds[i] << endl;
            file << "TrainingMu: " << templatesBuffer[i].trainingMu << endl;
            file << "TrainingSigma: " << templatesBuffer[i].trainingSigma << endl;
            file << "AverageTemplateLength: " << templatesBuffer[i].averageTemplateLength << endl;
            file << "TimeSeries: " << endl;
            for(UINT k=0; k<templatesBuffer[i].timeSeries.getNumRows(); k++){
                for(UINT j=0; j<templatesBuffer[i].timeSeries.getNumCols(); j++){
                    file << templatesBuffer[i].timeSeries[k][j] << "\t";
                }
                file << endl;
            }
        }
    }
    
    return true;
}


bool DTW::loadModelFromFile( string fileName ){

   std::fstream file;
   file.open(fileName.c_str(), std::ios::in);

    if( !loadModelFromFile( file ) ){
        return false;
    }
    
	file.close();
	

    return trained;
}

bool DTW::loadModelFromFile( fstream &file ){
    
    std::string word;
    UINT timeSeriesLength;
    UINT ts;
    
    if(!file.is_open())
    {
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to open file!" << endl;
        return false;
    }
    
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_DTW_Model_File_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Check to make sure this is a file with the DTW File Format
    if(word != "GRT_DTW_Model_File_V2.0"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Unknown file header!" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    //Check and load the Distance Method
    file >> word;
    if(word != "DistanceMethod:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find DistanceMethod!" << endl;
        return false;
    }
    file >> distanceMethod;
    
    //Check and load if Smoothing is used
    file >> word;
    if(word != "UseSmoothing:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find UseSmoothing!" << endl;
        return false;
    }
    file >> useSmoothing;
    
    //Check and load what the smoothing factor is
    file >> word;
    if(word != "SmoothingFactor:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find SmoothingFactor!" << endl;
        return false;
    }
    file >> smoothingFactor;
    
    //Check and load if ZNormalization is used
    file >> word;
    if(word != "UseZNormalisation:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find UseZNormalisation!" << endl;
        return false;
    }
    file >> useZNormalisation;
    
    //Check and load if OffsetUsingFirstSample is used
    file >> word;
    if(word != "OffsetUsingFirstSample:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find OffsetUsingFirstSample!" << endl;
        return false;
    }
    file >> offsetUsingFirstSample;
    
    //Check and load if ConstrainWarpingPath is used
    file >> word;
    if(word != "ConstrainWarpingPath:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find ConstrainWarpingPath!" << endl;
        return false;
    }
    file >> constrainWarpingPath;
    
    //Check and load if ZNormalization is used
    file >> word;
    if(word != "Radius:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find Radius!" << endl;
        return false;
    }
    file >> radius;
    
    //Check and load if Scaling is used
    file >> word;
    if(word != "RejectionMode:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find RejectionMode!" << endl;
        return false;
    }
    file >> rejectionMode;
    
    if( trained ){
        
        //Check and load the Number of Templates
        file >> word;
        if(word != "NumberOfTemplates:"){
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find NumberOfTemplates!" << endl;
            return false;
        }
        file >> numTemplates;
        
        //Check and load the overall average template length
        file >> word;
        if(word != "OverallAverageTemplateLength:"){
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find OverallAverageTemplateLength!" << endl;
            return false;
        }
        file >> averageTemplateLength;
        
        //Clean and reset the memory
        templatesBuffer.resize(numTemplates);
        classLabels.resize(numTemplates);
        nullRejectionThresholds.resize(numTemplates);
        
        //Load each template
        for(UINT i=0; i<numTemplates; i++){
            //Check we have the correct template
            file >> word;
            if( word != "***************TEMPLATE***************" ){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find template header!" << endl;
                return false;
            }
            
            //Load the template number
            file >> word;
            if(word != "Template:"){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find Template Number!" << endl;
                return false;
            }
            
            //Check the template number
            file >> ts;
            if(ts!=i+1){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Invalid Template Number: " << ts << endl;
                return false;
            }
            
            //Get the class label of this template
            file >> word;
            if(word != "ClassLabel:"){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find ClassLabel!" << endl;
                return false;
            }
            file >> templatesBuffer[i].classLabel;
            classLabels[i] = templatesBuffer[i].classLabel;
            
            //Get the time series length
            file >> word;
            if(word != "TimeSeriesLength:"){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find TimeSeriesLength!" << endl;
                return false;
            }
            file >> timeSeriesLength;
            
            //Resize the buffers
            templatesBuffer[i].timeSeries.resize(timeSeriesLength,numInputDimensions);
            
            //Get the template threshold
            file >> word;
            if(word != "TemplateThreshold:"){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find TemplateThreshold!" << endl;
                return false;
            }
            file >> nullRejectionThresholds[i];
            
            //Get the mu values
            file >> word;
            if(word != "TrainingMu:"){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find TrainingMu!" << endl;
                return false;
            }
            file >> templatesBuffer[i].trainingMu;
            
            //Get the sigma values
            file >> word;
            if(word != "TrainingSigma:"){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find TrainingSigma!" << endl;
                return false;
            }
            file >> templatesBuffer[i].trainingSigma;
            
            //Get the AverageTemplateLength value
            file >> word;
            if(word != "AverageTemplateLength:"){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find AverageTemplateLength!" << endl;
                return false;
            }
            file >> templatesBuffer[i].averageTemplateLength;
            
            //Get the data
            file >> word;
            if(word != "TimeSeries:"){
                clear();
                errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find template timeseries!" << endl;
                return false;
            }
            for(UINT k=0; k<timeSeriesLength; k++)
                for(UINT j=0; j<numInputDimensions; j++)
                    file >> templatesBuffer[i].timeSeries[k][j];
        }
        
        //Resize the prediction results to make sure it is setup for realtime prediction
        continuousInputDataBuffer.clear();
        continuousInputDataBuffer.resize(averageTemplateLength,vector<double>(numInputDimensions,0));
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}
bool DTW::setRejectionMode(UINT rejectionMode){
    if( rejectionMode == TEMPLATE_THRESHOLDS || rejectionMode == CLASS_LIKELIHOODS || rejectionMode == THRESHOLDS_AND_LIKELIHOODS ){
        this->rejectionMode = rejectionMode;
        return true;
    }
    return false;
}
    
bool DTW::setOffsetTimeseriesUsingFirstSample(bool offsetUsingFirstSample){
    this->offsetUsingFirstSample = offsetUsingFirstSample;
    return true;
}
    
bool DTW::setContrainWarpingPath(bool constrain){
    this->constrainWarpingPath = constrain;
    return true;
}
    
bool DTW::setWarpingRadius(double radius){
    this->radius = radius;
    return true;
}

bool DTW::enableZNormalization(bool useZNormalisation,bool constrainZNorm){ 
	this->useZNormalisation = useZNormalisation; 
	this->constrainZNorm = constrainZNorm;
	return true; 
}

bool DTW::enableTrimTrainingData(bool trimTrainingData,double trimThreshold,double maximumTrimPercentage){
	
	if( trimThreshold < 0 || trimThreshold > 1 ){
		warningLog << "Failed to set trimTrainingData.  The trimThreshold must be in the range of [0 1]" << endl;
		return false;
	}
	if( maximumTrimPercentage < 0 || maximumTrimPercentage > 100 ){
		warningLog << "Failed to set trimTrainingData.  The maximumTrimPercentage must be a valid percentage in the range of [0 100]" << endl;
		return false;
	}
	
	this->trimTrainingData = trimTrainingData;
	this->trimThreshold = trimThreshold;
	this->maximumTrimPercentage = maximumTrimPercentage;
	return true;
}
    
void DTW::offsetTimeseries(MatrixDouble &timeseries){
    VectorDouble firstRow = timeseries.getRowVector(0);
    for(UINT i=0; i<timeseries.getNumRows(); i++){
        for(UINT j=0; j<timeseries.getNumCols(); j++){
            timeseries[i][j] -= firstRow[j];
        }
    }
}
    
bool DTW::loadLegacyModelFromFile( fstream &file ){
    
    string word;
    UINT timeSeriesLength;
    UINT ts;
    
    //Check and load the Number of Dimensions
    file >> word;
    if(word != "NumberOfDimensions:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find NumberOfDimensions!" << endl;
        return false;
    }
    file >> numInputDimensions;
    
    //Check and load the Number of Classes
    file >> word;
    if(word != "NumberOfClasses:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find NumberOfClasses!" << endl;
        return false;
    }
    file >> numClasses;
    
    //Check and load the Number of Templates
    file >> word;
    if(word != "NumberOfTemplates:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find NumberOfTemplates!" << endl;
        return false;
    }
    file >> numTemplates;
    
    //Check and load the Distance Method
    file >> word;
    if(word != "DistanceMethod:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find DistanceMethod!" << endl;
        return false;
    }
    file >> distanceMethod;
    
    //Check and load if UseNullRejection is used
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find UseNullRejection!" << endl;
        return false;
    }
    file >> useNullRejection;
    
    //Check and load if Smoothing is used
    file >> word;
    if(word != "UseSmoothing:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find UseSmoothing!" << endl;
        return false;
    }
    file >> useSmoothing;
    
    //Check and load what the smoothing factor is
    file >> word;
    if(word != "SmoothingFactor:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find SmoothingFactor!" << endl;
        return false;
    }
    file >> smoothingFactor;
    
    //Check and load if Scaling is used
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find UseScaling!" << endl;
        return false;
    }
    file >> useScaling;
    
    //Check and load if ZNormalization is used
    file >> word;
    if(word != "UseZNormalisation:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find UseZNormalisation!" << endl;
        return false;
    }
    file >> useZNormalisation;
    
    //Check and load if OffsetUsingFirstSample is used
    file >> word;
    if(word != "OffsetUsingFirstSample:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find OffsetUsingFirstSample!" << endl;
        return false;
    }
    file >> offsetUsingFirstSample;
    
    //Check and load if ConstrainWarpingPath is used
    file >> word;
    if(word != "ConstrainWarpingPath:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find ConstrainWarpingPath!" << endl;
        return false;
    }
    file >> constrainWarpingPath;
    
    //Check and load if ZNormalization is used
    file >> word;
    if(word != "Radius:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find Radius!" << endl;
        return false;
    }
    file >> radius;
    
    //Check and load if Scaling is used
    file >> word;
    if(word != "RejectionMode:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find RejectionMode!" << endl;
        return false;
    }
    file >> rejectionMode;
    
    //Check and load gamma
    file >> word;
    if(word != "NullRejectionCoeff:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find NullRejectionCoeff!" << endl;
        return false;
    }
    file >> nullRejectionCoeff;
    
    //Check and load the overall average template length
    file >> word;
    if(word != "OverallAverageTemplateLength:"){
        errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find OverallAverageTemplateLength!" << endl;
        return false;
    }
    file >> averageTemplateLength;
    
    //Clean and reset the memory
    templatesBuffer.resize(numTemplates);
    classLabels.resize(numTemplates);
    nullRejectionThresholds.resize(numTemplates);
    
    //Load each template
    for(UINT i=0; i<numTemplates; i++){
        //Check we have the correct template
        file >> word;
        while(word != "Template:"){
            file >> word;
        }
        file >> ts;
        
        //Check the template number
        if(ts!=i+1){
            numTemplates=0;
            trained = false;
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find Invalid Template Number!" << endl;
            return false;
        }
        
        //Get the class label of this template
        file >> word;
        if(word != "ClassLabel:"){
            numTemplates=0;
            trained = false;
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find ClassLabel!" << endl;
            return false;
        }
        file >> templatesBuffer[i].classLabel;
        classLabels[i] = templatesBuffer[i].classLabel;
        
        //Get the time series length
        file >> word;
        if(word != "TimeSeriesLength:"){
            numTemplates=0;
            trained = false;
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find TimeSeriesLength!" << endl;
            return false;
        }
        file >> timeSeriesLength;
        
        //Resize the buffers
        templatesBuffer[i].timeSeries.resize(timeSeriesLength,numInputDimensions);
        
        //Get the template threshold
        file >> word;
        if(word != "TemplateThreshold:"){
            numTemplates=0;
            trained = false;
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find TemplateThreshold!" << endl;
            return false;
        }
        file >> nullRejectionThresholds[i];
        
        //Get the mu values
        file >> word;
        if(word != "TrainingMu:"){
            numTemplates=0;
            trained = false;
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find TrainingMu!" << endl;
            return false;
        }
        file >> templatesBuffer[i].trainingMu;
        
        //Get the sigma values
        file >> word;
        if(word != "TrainingSigma:"){
            numTemplates=0;
            trained = false;
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find TrainingSigma!" << endl;
            return false;
        }
        file >> templatesBuffer[i].trainingSigma;
        
        //Get the AverageTemplateLength value
        file >> word;
        if(word != "AverageTemplateLength:"){
            numTemplates=0;
            trained = false;
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find AverageTemplateLength!" << endl;
            return false;
        }
        file >> templatesBuffer[i].averageTemplateLength;
        
        //Get the data
        file >> word;
        if(word != "TimeSeries:"){
            numTemplates=0;
            trained = false;
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find template timeseries!" << endl;
            return false;
        }
        for(UINT k=0; k<timeSeriesLength; k++)
            for(UINT j=0; j<numInputDimensions; j++)
                file >> templatesBuffer[i].timeSeries[k][j];
        
        //Check for the footer
        file >> word;
        if(word != "***************************"){
            numTemplates=0;
            numClasses = 0;
            numInputDimensions=0;
            trained = false;
            errorLog << "loadDTWModelFromFile( string fileName ) - Failed to find template footer!" << endl;
            return false;
        }
    }
    
    //Resize the prediction results to make sure it is setup for realtime prediction
    continuousInputDataBuffer.clear();
    continuousInputDataBuffer.resize(averageTemplateLength,vector<double>(numInputDimensions,0));
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
    classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    
    trained = true;
    
    return true;
}

} //End of namespace GRT
