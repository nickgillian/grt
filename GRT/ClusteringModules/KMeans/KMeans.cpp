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

#include "KMeans.h"

namespace GRT{
    
//Register the KMeans class with the Clusterer base class
RegisterClustererModule< KMeans > KMeans::registerModule("KMeans");

//Constructor,destructor
KMeans::KMeans(const UINT numClusters,const UINT minNumEpochs,const UINT maxNumEpochs,const double minChange,const bool computeTheta){
    
    this->numClusters = numClusters;
    this->minNumEpochs = minNumEpochs;
    this->maxNumEpochs = maxNumEpochs;
    this->minChange = minChange;
    this->computeTheta = computeTheta;
    
    numTrainingSamples = 0;
    nchg = 0;
    finalTheta = 0;
    numTrainingIterationsToConverge = 0;
    trained = false;
    
    classType = "KMeans";
    clustererType = classType;
    debugLog.setProceedingText("[DEBUG KMeans]");
    errorLog.setProceedingText("[ERROR KMeans]");
    trainingLog.setProceedingText("[TRAINING KMeans]");
    warningLog.setProceedingText("[WARNING KMeans]");
}
    
KMeans::KMeans(const KMeans &rhs){
    
    classType = "KMeans";
    clustererType = classType;
    debugLog.setProceedingText("[DEBUG KMeans]");
    errorLog.setProceedingText("[ERROR KMeans]");
    trainingLog.setProceedingText("[TRAINING KMeans]");
    warningLog.setProceedingText("[WARNING KMeans]");
    
    if( this != &rhs ){
        
        this->numTrainingSamples = rhs.numTrainingSamples;
        this->nchg = rhs.nchg;
        this->computeTheta = rhs.computeTheta;
        this->finalTheta = rhs.finalTheta;
        this->clusters = rhs.clusters;
        this->assign = rhs.assign;
        this->count = rhs.count;
        this->thetaTracker = rhs.thetaTracker;
        
        //Clone the Clusterer variables
        copyBaseVariables( (Clusterer*)&rhs );
    }
    
}

KMeans::~KMeans(){
}
    
KMeans& KMeans::operator=(const KMeans &rhs){
    
    if( this != &rhs ){
        
        this->numTrainingSamples = rhs.numTrainingSamples;
        this->nchg = rhs.nchg;
        this->computeTheta = rhs.computeTheta;
        this->finalTheta = rhs.finalTheta;
        this->clusters = rhs.clusters;
        this->assign = rhs.assign;
        this->count = rhs.count;
        this->thetaTracker = rhs.thetaTracker;
        
        //Clone the Clusterer variables
        copyBaseVariables( (Clusterer*)&rhs );
    }
    
    return *this;
}

bool KMeans::deepCopyFrom(const Clusterer *clusterer){
    
    if( clusterer == NULL ) return false;
    
    if( this->getClustererType() == clusterer->getClustererType() ){
        //Clone the KMeans values
        KMeans *ptr = (KMeans*)clusterer;
        
        this->numTrainingSamples = ptr->numTrainingSamples;
        this->nchg = ptr->nchg;
        this->computeTheta = ptr->computeTheta;
        this->finalTheta = ptr->finalTheta;
        this->clusters = ptr->clusters;
        this->assign = ptr->assign;
        this->count = ptr->count;
        this->thetaTracker = ptr->thetaTracker;
        
        //Clone the Clusterer variables
        return copyBaseVariables( clusterer );
    }
    return false;
}

    /*
bool KMeans::predict(VectorDouble inputVector,UINT &predictedClusterLabel,double &maxLikelihood,VectorDouble &clusterLikelihoods){
	
	if( !trained ){
		 return false;
	}
	
	if( inputVector.size() != numInputDimensions ){
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
	
	double minDist = numeric_limits<double>::max();
	double sum = 0;
	UINT minIndex = 0;
	predictedClusterLabel = 0;
	maxLikelihood = 0;
	if( clusterLikelihoods.size() != numClusters )
        clusterLikelihoods.resize( numClusters );
	
	for(UINT i=0; i<numClusters; i++){
		
		double dist = 0;
		for(UINT j=0; j<numInputDimensions; j++){
			dist += SQR( inputVector[j]-clusters[i][j] );
		}
        
        //We don't need to compute the sqrt as it works without it and is faster
		clusterLikelihoods[i] = dist;
		sum += dist;
				
		if( dist < minDist ){
			minDist = dist;
			minIndex = i;
		}
	}
	
	//Normalize the likelihood
	for(UINT i=0; i<numClusters; i++){
		clusterLikelihoods[i] /= sum;
	}
	
	predictedClusterLabel = clusterLabels[ minIndex ];
	maxLikelihood = 1.0 - clusterLikelihoods[ minIndex ];
	
	return true;
}
*/
bool KMeans::train_(ClassificationData &trainingData){
	
	if( trainingData.getNumSamples() == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - The training data is empty!" << endl;
		return false;
	}
	
	//Set the numClusters as the number of classes in the training data
	numClusters = trainingData.getNumClasses();

    //Convert the labelled training data to a training matrix
	UINT M = trainingData.getNumSamples();
    UINT N = trainingData.getNumDimensions();
    MatrixDouble data(M,N);
    for(UINT i=0; i<M; i++){
        for(UINT j=0; j<N; j++){
            data[i][j] = trainingData[i][j];
        }
    }

    //Run the K-Means algorithm
    return train_( data );
}

bool KMeans::train_(UnlabelledData &trainingData){

    //Convert the training data into one matrix
	UINT M = trainingData.getNumSamples();
    UINT N = trainingData.getNumDimensions();
    MatrixDouble data(M,N);
    for(UINT i=0; i<M; i++){
        for(UINT j=0; j<N; j++){
            data[i][j] = trainingData[i][j];
        }
    }
	
	return train_(data);
}

bool KMeans::train_(MatrixDouble &data){
	
	trained = false;
	
	if( numClusters == 0 ){
        errorLog << "train_(MatrixDouble &data) - Failed to train model. NumClusters is zero!" << endl;
		return false;
	}
    
    if( data.getNumRows() == 0 || data.getNumCols() == 0 ){
        errorLog << "train_(MatrixDouble &data) - The number of rows or columns in the data is zero!" << endl;
		return false;
	}
    
	numTrainingSamples = data.getNumRows();
	numInputDimensions = data.getNumCols();

	clusters.resize(numClusters,numInputDimensions);
	assign.resize(numTrainingSamples);
	count.resize(numClusters);

	//Randomly pick k data points as the starting clusters
	Random random;
	vector< UINT > randIndexs(numTrainingSamples);
	for(UINT i=0; i<numTrainingSamples; i++) randIndexs[i] = i;
    std::random_shuffle(randIndexs.begin(), randIndexs.end());

    //Copy the clusters
	for(UINT k=0; k<numClusters; k++){
		for(UINT j=0; j<numInputDimensions; j++){
            clusters[k][j] = data[ randIndexs[k] ][j];
		}
	}

	return trainModel( data );
}

bool KMeans::trainModel(MatrixDouble &data){
    
    if( numClusters == 0 ){
        errorLog << "trainModel(MatrixDouble &data) - Failed to train model. NumClusters is zero!" << endl;
		return false;
	}
    
    if( clusters.getNumRows() != numClusters ){
        errorLog << "trainModel(MatrixDouble &data) - Failed to train model. The number of rows in the cluster matrix does not match the number of clusters! You should need to initalize the clusters matrix first before calling this function!" << endl;
		return false;
	}
    
    if( clusters.getNumCols() != numInputDimensions ){
        errorLog << "trainModel(MatrixDouble &data) - Failed to train model. The number of columns in the cluster matrix does not match the number of input dimensions! You should need to initalize the clusters matrix first before calling this function!" << endl;
		return false;
	}

    Timer timer;
	UINT currentIter = 0;
    UINT numChanged = 0;
	bool keepTraining = true;
    double theta = 0;
    double lastTheta = 0;
    double delta = 0;
    double startTime = 0;
    thetaTracker.clear();
    finalTheta = 0;
    numTrainingIterationsToConverge = 0;
    trained = false;
    converged = false;
    
    //Scale the data if needed
    ranges = data.getRanges();
    if( useScaling ){
        data.scale(0,1);
    }

    //Init the assign and count vectors
    //Assign is set to K+1 so that the nChanged values in the eStep at the first iteration will be updated correctly
    for(UINT m=0; m<numTrainingSamples; m++) assign[m] = numClusters+1;
	for(UINT k=0; k<numClusters; k++) count[k] = 0;

    //Run the training loop
    timer.start();
	while( keepTraining ){
        startTime = timer.getMilliSeconds();

		//Compute the E step
		numChanged = estep( data );

        //Compute the M step
        mstep( data );

        //Update the iteration counter
		currentIter++;

		//Compute theta if needed
		if( computeTheta ){
            theta = calculateTheta(data);
            delta = lastTheta - theta;
            lastTheta = theta;
        }else theta = delta = 0;
        
        //Check convergance
		if( numChanged == 0 && currentIter > minNumEpochs ){ converged = true; keepTraining = false; }
		if( currentIter >= maxNumEpochs ){ keepTraining = false; }
		if( fabs( delta ) < minChange && computeTheta && currentIter > minNumEpochs ){ converged = true; keepTraining = false; }
        if( computeTheta )  thetaTracker.push_back( theta );
        
        trainingLog << "Epoch: " << currentIter << "/" << maxNumEpochs;
        trainingLog << " Epoch time: " << (timer.getMilliSeconds()-startTime)/1000.0 << " seconds";
        trainingLog << " Theta: " << theta << " Delta: " << delta << endl;
	}
    trainingLog << "Model Trained at epoch: " << currentIter << " with a theta value of: " << theta << endl;

    finalTheta = theta;
    numTrainingIterationsToConverge = currentIter;
	trained = true;
	
	return true;
}

UINT KMeans::estep(const MatrixDouble &data) {
		UINT k,m,n,kmin;
		double dmin,d;
		nchg = 0;
		kmin = 0;

		//Reset Count
		for (k=0; k < numClusters; k++) count[k] = 0;

		//Search for the closest center and reasign if needed
		for (m=0; m < numTrainingSamples; m++) {
			dmin = 9.99e+99; //Set dmin to a really big value
			for (k=0; k < numClusters; k++) {
				d = 0.0;
				for (n=0; n < numInputDimensions; n++)
					d += SQR( data[m][n]-clusters[k][n] );
				if (d <= dmin){ dmin = d; kmin = k; }
			}
			if ( kmin != assign[m] ){
                nchg++;
                assign[m] = kmin;
            }
			count[kmin]++;
		}
		return nchg;
}

void KMeans::mstep(const MatrixDouble &data) {
    UINT n,k,m;

    //Reset means to zero
    for (k=0; k<numClusters; k++)
        for (n=0;n<numInputDimensions;n++)
            clusters[k][n] = 0.;

    //Get new mean by adding assigned data points and dividing by the number of values in each cluster
    for(m=0; m < numTrainingSamples; m++)
        for(n=0; n < numInputDimensions; n++)
            clusters[ assign[m] ][n] += data[m][n];

    for (k=0; k < numClusters; k++) {
        if (count[k] > 0){
            for (n=0; n < numInputDimensions; n++){
                clusters[k][n] /= double(count[k]);
            }
        }
    }
}

double KMeans::calculateTheta(const MatrixDouble &data){

	double theta = 0;
    double sum = 0;
    UINT m,n,k = 0;
	for(m=0; m < numTrainingSamples; m++){
		k = assign[m];
        sum = 0;
		for(n=0; n < numInputDimensions; n++){
				sum += SQR(clusters[k][n] - data[m][n]);
		}
		theta += sqrt(sum);
	}
    theta /= numTrainingSamples;

	return theta;

}
    
bool KMeans::saveModelToFile(string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(string filename) - Failed to open file!" << endl;
        return false;
    }
    
    if( !saveModelToFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
}

bool KMeans::saveModelToFile(fstream &file) const{

    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - Failed to save model, file is not open!" << endl;
        return false;
    }

    file << "GRT_KMEANS_MODEL_FILE_V1.0\n";
    
    if( !saveClustererSettingsToFile( file ) ){
        errorLog << "saveModelToFile(fstream &file) - Failed to save clusterer settings to file!" << endl;
        return false;
    }
    
    if( trained ){
        file << "Clusters:\n";

        for(UINT k=0; k<numClusters; k++){
           for(UINT n=0; n<numInputDimensions; n++){
                file << clusters[k][n] << "\t";
           }file << endl;
        }
    }

   return true;

}
    
bool KMeans::loadModelFromFile(string fileName){
    
    std::fstream file;
    string word;
    file.open(fileName.c_str(), std::ios::in);
    
    if(!file.is_open()){
        errorLog << "loadModelFromFile(string filename) - Failed to open file!" << endl;
        return false;
    }
    
    if( !loadModelFromFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
    
}

bool KMeans::loadModelFromFile(fstream &file){

    //Clear any previous model
    clear();
    
    if(!file.is_open()){
        errorLog << "loadModelFromFile(string filename) - Failed to open file!" << endl;
        return false;
    }

    string word;
    file >> word;
    if( word != "GRT_KMEANS_MODEL_FILE_V1.0" ){
	   return false;
    }
    
    if( !loadClustererSettingsFromFile( file ) ){
        errorLog << "loadModelFromFile(string filename) - Failed to open file!" << endl;
        return false;
    }

    if( trained ){
        file >> word;
        if( word != "Clusters:" ){
            return false;
        }
        
        //Resize the buffers
        clusters.resize(numClusters,numInputDimensions);
        
        //Load the data
        for(UINT k=0; k<numClusters; k++){
            for(UINT n=0; n<numInputDimensions; n++){
                file >> clusters[k][n];
            }
        }
    }

    return true;
}
    
bool KMeans::reset(){
    Clusterer::reset();
    
    numTrainingSamples = 0;
    nchg = 0;
    finalTheta = 0;
    thetaTracker.clear();
    assign.clear();
    count.clear();
    
    return true;
}

bool KMeans::clear(){
    Clusterer::clear();
    
    numTrainingSamples = 0;
    nchg = 0;
    finalTheta = 0;
    thetaTracker.clear();
    assign.clear();
    count.clear();
    clusters.clear();
    
    return true;
}
    
bool KMeans::setComputeTheta(const bool computeTheta){
    this->computeTheta = computeTheta;
    return true;
}
    
bool KMeans::setClusters(const MatrixDouble &clusters){
    clear();
    numClusters = clusters.getNumRows();
    numInputDimensions = clusters.getNumCols();
    this->clusters = clusters;
    return true;
}


}//End of namespace GRT
