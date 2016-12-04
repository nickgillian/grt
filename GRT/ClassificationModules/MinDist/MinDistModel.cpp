/**
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
#include "MinDistModel.h"

GRT_BEGIN_NAMESPACE

MinDistModel::MinDistModel(){
    classLabel = 0;
    numFeatures = 0;
    numClusters = 0;
    rejectionThreshold = 0;
    gamma = 0;
    trainingMu = 0;
    trainingSigma = 0;
    trainingLoggingEnabled = true;
}
    
MinDistModel::MinDistModel(const MinDistModel &rhs){

    this->classLabel = rhs.classLabel;
    this->numFeatures = rhs.numFeatures;
    this->numClusters = rhs.numClusters;
    this->rejectionThreshold = rhs.rejectionThreshold;
    this->gamma = rhs.gamma;
    this->trainingMu = rhs.trainingMu;
    this->trainingSigma = rhs.trainingSigma;
    this->clusters = rhs.clusters;
    this->trainingLoggingEnabled = rhs.trainingLoggingEnabled;
}
    
MinDistModel::~MinDistModel(void){}
    
MinDistModel& MinDistModel:: operator=(const MinDistModel &rhs){
	if( this != &rhs ){
        //MinDistModel variables
		this->classLabel = rhs.classLabel;
		this->numFeatures = rhs.numFeatures;
		this->numClusters = rhs.numClusters;
		this->rejectionThreshold = rhs.rejectionThreshold;
		this->gamma = rhs.gamma;
		this->trainingMu = rhs.trainingMu;
		this->trainingSigma = rhs.trainingSigma;
		this->clusters = rhs.clusters;
		this->trainingLoggingEnabled = rhs.trainingLoggingEnabled;
	}
	return *this;
}
	
bool MinDistModel::train(UINT classLabel,MatrixFloat &trainingData,UINT numClusters,Float minChange,UINT maxNumEpochs){
	
	if( trainingData.getNumRows() < numClusters ){
        return false;
    }
	
	this->classLabel = classLabel;
	this->numFeatures = trainingData.getNumCols();
	this->numClusters = numClusters;
	
	//Find the clusters
	KMeans kmeans;
    kmeans.setNumClusters(numClusters);
	kmeans.setMinChange( minChange );
	kmeans.setMaxNumEpochs( maxNumEpochs );
	kmeans.setTrainingLoggingEnabled( trainingLoggingEnabled );
	
	if( !kmeans.train_(trainingData) ){
		return false;
	}
	
	clusters = kmeans.getClusters();
    
	//Compute the rejection thresholds
	rejectionThreshold = 0;
	trainingMu = 0;
	trainingSigma = 0;
    
    //Now compute the rejection threshold
    const UINT M = trainingData.getNumRows();
    const UINT N = trainingData.getNumCols();
	VectorFloat predictions(M);
	for(UINT i=0; i<M; i++){
		//Test the ith training example
		VectorFloat testData(N);
		for(UINT j=0; j<N; j++) {
			testData[j] = trainingData[i][j];
        }
        
		predictions[i] = predict( testData );
        trainingMu += predictions[i];
	}
    
	//Calculate the mean prediction value
	trainingMu /= Float(M);
    
	//Calculate the standard deviation
	for(UINT i=0; i<M; i++) {
		trainingSigma += SQR( predictions[i]-trainingMu );
    }
	trainingSigma = sqrt( trainingSigma / (Float(M)-1.0) );
    
	rejectionThreshold = trainingMu + ( trainingSigma * gamma );
	return true;
	
}

Float MinDistModel::predict(const VectorFloat &inputVector){
	
	Float minDist = grt_numeric_limits< Float >::max();
	Float dist = 0;
	
	for(UINT k=0; k<numClusters; k++){
		dist = 0;
        for(UINT n=0; n<numFeatures; n++){
            dist += SQR( clusters[k][n]-inputVector[n] );
        }
		if( dist < minDist )
			minDist = dist;
	}
    
    //Only compute the sqrt for the minimum distance
	return sqrt( minDist );
     
    return minDist;
}
	
void MinDistModel::recomputeThresholdValue(){
	rejectionThreshold = trainingMu + ( trainingSigma * gamma );
}
	
UINT MinDistModel::getClassLabel() const{
	return classLabel;
}

UINT MinDistModel::getNumFeatures() const{
	return numFeatures;
}

UINT MinDistModel::getNumClusters() const{
	return numClusters;
}
    
Float MinDistModel::getRejectionThreshold() const{
    return rejectionThreshold;
}

Float MinDistModel::getGamma() const{
    return gamma;
}
    
Float MinDistModel::getTrainingMu() const{
    return trainingMu;
}
    
Float MinDistModel::getTrainingSigma() const{
    return trainingSigma;
}

MatrixFloat MinDistModel::getClusters() const{
	return clusters;
}
    
bool MinDistModel::setClassLabel(UINT classLabel){
    this->classLabel = classLabel;
    return true;
}

bool MinDistModel::setClusters(MatrixFloat &clusters){
    this->clusters = clusters;
	this->numClusters = clusters.getNumRows();
	this->numFeatures = clusters.getNumCols();
    return true;
}

bool MinDistModel::setGamma(Float gamma){
    this->gamma = gamma;
    return true;
}

bool MinDistModel::setRejectionThreshold(Float rejectionThreshold){
    this->rejectionThreshold = rejectionThreshold;
    return true;
}

bool MinDistModel::setTrainingSigma(Float trainingSigma){
    this->trainingSigma = trainingSigma;
    return true;
}

bool MinDistModel::setTrainingMu(Float trainingMu){
    this->trainingMu = trainingMu;
    return true;
}

bool MinDistModel::setTrainingLoggingEnabled(bool enabled){
	this->trainingLoggingEnabled = enabled;
	return true;
}

GRT_END_NAMESPACE

