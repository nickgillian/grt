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

#define GRT_DLL_EXPORTS
#include "LDA.h"

GRT_BEGIN_NAMESPACE

LDA::LDA(bool useScaling,bool useNullRejection,Float nullRejectionCoeff)
{
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    classType = "LDA";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG LDA]");
    errorLog.setProceedingText("[ERROR LDA]");
    trainingLog.setProceedingText("[TRAINING LDA]");
    warningLog.setProceedingText("[WARNING LDA]");
}

LDA::~LDA(void)
{
} 
    
bool LDA::train(ClassificationData trainingData){
    
    errorLog << "SORRY - this module is still under development and can't be used yet!" << std::endl;
    return false;
    
    //Reset any previous model
    numInputDimensions = 0;
    numClasses = 0;
    models.clear();
    classLabels.clear();
    trained = false;
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(LabelledClassificationData trainingData) - There is no training data to train the model!" << std::endl;
        return false;
    }
    
    numInputDimensions = trainingData.getNumDimensions();
    numClasses = trainingData.getNumClasses();

	//Calculate the between scatter matrix
	MatrixFloat SB = computeBetweenClassScatterMatrix( trainingData );
	
	//Calculate the within scatter matrix
	MatrixFloat SW = computeWithinClassScatterMatrix( trainingData );


   /*

    
    //Counters and stat containers
    vector< UINT > groupLabels(numClasses);
    VectorDouble groupCounters(numClasses);
    VectorDouble priorProb(numClasses);
    MatrixFloat groupMeans(numClasses,numFeatures);
    MatrixFloat pCov(numFeatures,numFeatures);
    MatrixFloat pCovInv(numFeatures,numFeatures);
    MatrixFloat modelCoeff(numClasses,numFeatures+1);
    
    pCov.setAllValues(0);
    modelCoeff.setAllValues(0);
    
    //Set the class labels and counters
    for(UINT k=0; k<numClasses; k++){
        groupLabels[k] = trainingData.getClassTracker()[k].classLabel;
        groupCounters[k] = trainingData.getClassTracker()[k].counter;
    }
    
    //Loop over the classes to compute the group stats
    for(UINT k=0; k<numClasses; k++){
        LabelledClassificationData classData = trainingData.getClassData( groupLabels[k] );
        MatrixFloat cov(numFeatures,numFeatures);
        
        //Compute class mu
        for(UINT j=0; j<numFeatures; j++){
            groupMeans[k][j] = 0;
            for(UINT i=0; i<classData.getNumSamples(); i++){
                groupMeans[k][j] += classData[i][j];
            }
            groupMeans[k][j] /= Float(classData.getNumSamples());
        }
        
        //Compute the class covariance
        for(UINT m=0; m<numFeatures; m++){
            for(UINT n=0; n<numFeatures; n++){
                cov[m][n] = 0;
                for(UINT i=0; i<classData.getNumSamples(); i++){
                    cov[m][n] += (classData[i][m]-groupMeans[k][m]) * (classData[i][n]-groupMeans[k][n]);
                }
                cov[m][n] /= Float(classData.getNumSamples()-1);
            }
        }
        
        debugLog << "Group Cov:\n";
        for(UINT m=0; m<numFeatures; m++){
            for(UINT n=0; n<numFeatures; n++){
                debugLog << cov[m][n] << "\t";
            }debugLog << "\n";
        }debugLog << std::endl;
        
        //Set the prior probability for this class (which is just 1/numClasses)
        priorProb[k] = 1.0/Float(numClasses);
        
        //Update the main covariance matrix
        Float weight = ((classData.getNumSamples() - 1) / Float(trainingData.getNumSamples() - numClasses) );
        debugLog << "Weight: " << weight << std::endl;
        for(UINT m=0; m<numFeatures; m++){
            for(UINT n=0; n<numFeatures; n++){
                pCov[m][n] += weight * cov[m][n];
            }
        }
    }
    
    for(UINT k=0; k<numClasses; k++){
        debugLog << "GroupMu: " << groupLabels[k] << "\t";
        for(UINT j=0; j<numFeatures; j++){
            debugLog << groupMeans[k][j] << "\t";
        }debugLog << std::endl;
    }
    
    debugLog << "pCov:\n";
    for(UINT m=0; m<numFeatures; m++){
        for(UINT n=0; n<numFeatures; n++){
            debugLog << pCov[m][n] << "\t";
        }debugLog << "\n";
    }debugLog << std::endl;
    
    //Invert the pCov matrix
    LUDecomposition matrixInverter(pCov);
    if( !matrixInverter.inverse(pCovInv) ){
        errorLog << "Failed to invert pCov Matrix!" << std::endl;
        return false;
    }
    
    //Loop over classes to calculate linear discriminant coefficients
    Float sum = 0;
    vector< Float > temp(numFeatures);
    for(UINT k=0; k<numClasses; k++){
        //Compute the temporary vector
        for(UINT j=0; j<numFeatures; j++){
            temp[j] = 0;
            for(UINT m=0; m<numFeatures; m++){
                    temp[j] += groupMeans[k][m] * pCovInv[m][j];
            }
        }
        
        //Compute the model coefficients
        sum = 0;
        for(UINT j=0; j<numFeatures; j++){
            sum += temp[j]*groupMeans[k][j];
        }
        modelCoeff[k][0] = -0.5 * sum + log( priorProb[k] );
        
        for(UINT j=0; j<numFeatures; j++){
            modelCoeff[k][j+1] = temp[j];
        }
    }
    
    //Setup the models for realtime prediction
    models.resize(numClasses);
    classLabels.resize(numClasses);
    
    for(UINT k=0; k<numClasses; k++){
        classLabels[k] = groupLabels[k];
        models[k].classLabel = groupLabels[k];
        models[k].priorProb = priorProb[k];
        models[k].weights = modelCoeff.getRowVector(k);
    }
    
    //Flag that the models were successfully trained
    trained = true;
    */
    
    return true;
}

bool LDA::predict(VectorDouble inputVector){
    
    if( !trained ){
        errorLog << "predict(vector< Float > inputVector) - LDA Model Not Trained!" << std::endl;
        return false;
    }
    
    predictedClassLabel = 0;
	maxLikelihood = -10000;
    
    if( !trained ) return false;
    
	if( inputVector.getSize() != numInputDimensions ){
        errorLog << "predict(vector< Float > inputVector) - The size of the input vector (" << inputVector.getSize() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
		return false;
	}
    
    //Make sure the likelihoods and distances vectors have been assigned
    if( classLikelihoods.getSize() != numClasses || classDistances.getSize() != numClasses ){
        classLikelihoods.resize(numClasses);
        classDistances.resize(numClasses);
    }
    
    //Compute the linear scores for each class
    bestDistance = 0;
    maxLikelihood = 0;
    UINT bestIndex = 0;
    Float sum = 0;
    for(UINT k=0; k<numClasses; k++){
        
        for(UINT j=0; j<numInputDimensions+1; j++){
            if( j==0 ) classDistances[k] = models[k].weights[j];
            else classDistances[k] += inputVector[j-1] * models[k].weights[j];
        }
        classLikelihoods[k] = exp( classDistances[k] );
        sum += classLikelihoods[k];
        
        if( classLikelihoods[k] > maxLikelihood ){
            bestIndex = k;
            maxLikelihood = classLikelihoods[k];
        }
    }
    
    //Normalize the likelihoods
    for(UINT k=0; k<numClasses; k++){
        classLikelihoods[k] /= sum;
    }
    
    maxLikelihood = classLikelihoods[ bestIndex ];
    
    predictedClassLabel = models[ bestIndex ].classLabel;
    
    return true;
}
    
bool LDA::saveModelToFile( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"saveModelToFile(fstream &file) - Could not open file to save model" << std::endl;
        return false;
    }
    
    //Write the header info
    file<<"GRT_LDA_MODEL_FILE_V1.0\n";
    file<<"NumFeatures: "<<numInputDimensions<< std::endl;
    file<<"NumClasses: "<<numClasses<< std::endl;
    file <<"UseScaling: " << useScaling << std::endl;
    file<<"UseNullRejection: " << useNullRejection << std::endl;
    
    ///Write the ranges if needed
    if( useScaling ){
        file << "Ranges: \n";
        for(UINT n=0; n<ranges.size(); n++){
            file << ranges[n].minValue << "\t" << ranges[n].maxValue << std::endl;
        }
    }
    
    //Write each of the models
    for(UINT k=0; k<numClasses; k++){
        file<<"ClassLabel: "<<models[k].classLabel<< std::endl;
        file<<"PriorProbability: "<<models[k].priorProb<< std::endl;
        file<<"Weights: ";
        
        for(UINT j=0; j<models[k].getNumDimensions(); j++){
            file << "\t" << models[k].weights[j];
        }file<< std::endl;
    }
    
    return true;
}
    
bool LDA::loadModelFromFile( std::fstream &file ){
    
    trained = false;
    numInputDimensions = 0;
    numClasses = 0;
    models.clear();
    classLabels.clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Find the file type header
    file >> word;
    if(word != "GRT_LDA_MODEL_FILE_V1.0"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find Model File Header" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NumFeatures " << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NumClasses" << std::endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find UseScaling" << std::endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find UseNullRejection" << std::endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numInputDimensions);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find the Ranges" << std::endl;
            return false;
        }
        for(UINT n=0; n<ranges.size(); n++){
            file >> ranges[n].minValue;
            file >> ranges[n].maxValue;
        }
    }
    
    //Resize the buffer
    models.resize(numClasses);
    classLabels.resize(numClasses);
    
    //Load each of the K models
    for(UINT k=0; k<numClasses; k++){
        file >> word;
        if(word != "ClassLabel:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find ClassLabel for the "<<k+1<<"th model" << std::endl;
            return false;
        }
        file >> models[k].classLabel;
        classLabels[k] = models[k].classLabel;
        
        file >> word;
        if(word != "PriorProbability:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find the PriorProbability for the "<<k+1<<"th model" << std::endl;
            return false;
        }
        file >> models[k].priorProb;
        
        models[k].weights.resize(numInputDimensions+1);
        
        //Load the weights
        file >> word;
        if(word != "Weights:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find the Weights vector for the "<<k+1<<"th model" << std::endl;
            return false;
        }
        
        //Load Weights
        for(UINT j=0; j<numInputDimensions+1; j++){
            Float value;
            file >> value;
            models[k].weights[j] = value;
        }
    }
    
    //Resize the prediction results to make sure it is setup for realtime prediction
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
    classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    
    trained = true;
    
    return true;
}

MatrixFloat LDA::computeBetweenClassScatterMatrix( ClassificationData &data ){
	
	MatrixFloat sb(numInputDimensions,numInputDimensions);
	MatrixFloat classMean = data.getClassMean();
	VectorDouble totalMean = data.getMean();
	sb.setAllValues( 0 );
	
	for(UINT k=0; k<numClasses; k++){
		
		UINT numSamplesInClass = data.getClassTracker()[k].counter;
	
		for(UINT m=0; m<numInputDimensions; m++){
			for(UINT n=0; n<numInputDimensions; n++){
				sb[m][n] += (classMean[k][m]-totalMean[m]) * (classMean[k][n]-totalMean[n]) * Float(numSamplesInClass);
			}
		}
	}
	
	return sb;
}

MatrixFloat LDA::computeWithinClassScatterMatrix( ClassificationData &data ){
	
	MatrixFloat sw(numInputDimensions,numInputDimensions);
	sw.setAllValues( 0 );
	
	for(UINT k=0; k<numClasses; k++){
		
		//Compute the scatter matrix for class k
		ClassificationData classData = data.getClassData( data.getClassTracker()[k].classLabel );
		MatrixFloat scatterMatrix = classData.getCovarianceMatrix();
		
		//Add this to the main scatter matrix
		for(UINT m=0; m<numInputDimensions; m++){
			for(UINT n=0; n<numInputDimensions; n++){
				sw[m][n] += scatterMatrix[m][n];
			}
		}
	}
	
	return sw;
}

GRT_END_NAMESPACE

