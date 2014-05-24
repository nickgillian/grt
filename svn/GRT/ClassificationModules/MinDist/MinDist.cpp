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

#include "MinDist.h"

namespace GRT{

//Register the MinDist module with the Classifier base class
RegisterClassifierModule< MinDist > MinDist::registerModule("MinDist");

MinDist::MinDist(bool useScaling,bool useNullRejection,double nullRejectionCoeff,UINT numClusters)
{
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    this->numClusters = numClusters;
    classType = "MinDist";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG MinDist]");
    errorLog.setProceedingText("[ERROR MinDist]");
    trainingLog.setProceedingText("[TRAINING MinDist]");
    warningLog.setProceedingText("[WARNING MinDist]");
}
    
MinDist::MinDist(const MinDist &rhs){
    classType = "MinDist";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG MinDist]");
    errorLog.setProceedingText("[ERROR MinDist]");
    trainingLog.setProceedingText("[TRAINING MinDist]");
    warningLog.setProceedingText("[WARNING MinDist]");
    *this = rhs;
}

MinDist::~MinDist(void)
{
}
    
MinDist& MinDist::operator=(const MinDist &rhs){
    if( this != &rhs ){
        //MinDist variables
        this->numClusters = rhs.numClusters;
        this->models = rhs.models;
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}
    
bool MinDist::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        //Invoke the equals operator
        MinDist *ptr = (MinDist*)classifier;
        
        this->numClusters = ptr->numClusters;
        this->models = ptr->models;
        
        //Classifier variables
        return copyBaseVariables( classifier );
    }
    
    return false;
}
    
bool MinDist::train_(ClassificationData &labelledTrainingData){
    
    //Clear any previous models
    clear();
    
    const unsigned int M = labelledTrainingData.getNumSamples();
    const unsigned int N = labelledTrainingData.getNumDimensions();
    const unsigned int K = labelledTrainingData.getNumClasses();
    models.clear();
    
    if( M == 0 ){
        errorLog << "train_(ClassificationData &labelledTrainingData) - Training data has zero samples!" << endl;
        return false;
    }
    
    if( M <= numClusters ){
        errorLog << "train_(ClassificationData &labelledTrainingData) - There are not enough training samples for the number of clusters. Either reduce the number of clusters or increase the number of training samples!" << endl;
        return false;
    }
    
    numInputDimensions = N;
    numClasses = K;
    models.resize(K);
    classLabels.resize(K);
    ranges = labelledTrainingData.getRanges();
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        labelledTrainingData.scale(0, 1);
    }
    
    //Train each of the models
    for(UINT k=0; k<numClasses; k++){
        
        //Get the class label for the kth class
        UINT classLabel = labelledTrainingData.getClassTracker()[k].classLabel;
        
        //Set the kth class label
        classLabels[k] = classLabel;
        
        //Get all the training data for this class
        ClassificationData classData = labelledTrainingData.getClassData(classLabel);
        MatrixDouble data(classData.getNumSamples(),N);
        
        //Copy the training data into a matrix
        for(UINT i=0; i<data.getNumRows(); i++){
            for(UINT j=0; j<data.getNumCols(); j++){
                data[i][j] = classData[i][j];
            }
        }
        
        //Train the model for this class
        models[k].setGamma( nullRejectionCoeff );
        if( !models[k].train(classLabel,data,numClusters) ){
            errorLog << "train_(ClassificationData &labelledTrainingData) - Failed to train model for class: " << classLabel;
            errorLog << ". This is might be because this class does not have enough training samples! You should reduce the number of clusters or increase the number of training samples for this class." << endl;
            models.clear();
            return false;
        }
        
    }
    
    trained = true;
    return true;
}
    

bool MinDist::predict_(VectorDouble &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - MinDist Model Not Trained!" << endl;
        return false;
    }
    
    predictedClassLabel = 0;
	maxLikelihood = 0;
    
    if( !trained ) return false;
    
	if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    double classLikelihoodsSum = 0;
    double minDist = numeric_limits<double>::max();
	for(UINT k=0; k<numClasses; k++){
		classDistances[k] = models[k].predict( inputVector );
        
        //At this point the class likelihoods and class distances are the same thing
        classLikelihoods[k] = classDistances[k];
        classLikelihoodsSum += classDistances[k];

        //Keep track of the best value
		if( classDistances[k] < minDist ){
			minDist = classDistances[k];
			predictedClassLabel = k;
		}
    }
    
    //Normalize the classlikelihoods
	if( classLikelihoodsSum != 0 ){
    	for(UINT k=0; k<numClasses; k++){
        	classLikelihoods[k] = (classLikelihoodsSum-classLikelihoods[k])/classLikelihoodsSum;
    	}
        maxLikelihood = classLikelihoods[predictedClassLabel];
	}else maxLikelihood = classLikelihoods[predictedClassLabel];
    
    if( useNullRejection ){
        //Check to see if the best result is greater than the models threshold
        if( minDist <= models[predictedClassLabel].getRejectionThreshold() ) predictedClassLabel = models[predictedClassLabel].getClassLabel();
        else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    }else predictedClassLabel = models[predictedClassLabel].getClassLabel();
    
    return true;
}
    
bool MinDist::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the MinDist variables
    models.clear();
    return true;
}

bool MinDist::recomputeNullRejectionThresholds(){

    if( trained ){
        for(UINT k=0; k<numClasses; k++) {
			models[k].setGamma( nullRejectionCoeff );
            models[k].recomputeThresholdValue();
		}
        return true;
    }
    return false;
}
    
bool MinDist::setNullRejectionCoeff(double nullRejectionCoeff){
    
    if( nullRejectionCoeff > 0 ){
        this->nullRejectionCoeff = nullRejectionCoeff;
        recomputeNullRejectionThresholds();
        return true;
    }
    return false;
}
    
bool MinDist::saveModelToFile(string filename) const{

    if( !trained ) return false;
    
	std::fstream file; 
	file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }

	file.close();

	return true;
}
    
bool MinDist::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file<<"GRT_MINDIST_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    if( trained ){
    
        //Write each of the models
        for(UINT k=0; k<numClasses; k++){
            file << "ClassLabel: " << models[k].getClassLabel() << endl;
            file << "NumClusters: " << models[k].getNumClusters() << endl;
            file << "RejectionThreshold:  " << models[k].getRejectionThreshold() << endl;
            file << "Gamma: " << models[k].getGamma() << endl;
            file << "TrainingMu: " << models[k].getTrainingMu() << endl;
            file << "TrainingSigma: " << models[k].getTrainingSigma() << endl;
            file << "ClusterData:" << endl;
            Matrix<double> clusters = models[k].getClusters();
            for(UINT i=0; i<models[k].getNumClusters(); i++){
                for(UINT j=0; j<models[k].getNumFeatures(); j++){
                    file << clusters[i][j] << "\t";
                }
                file << endl;
            }
        }
        
    }
    
    return true;
}

bool MinDist::loadModelFromFile(string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    file.close();

	return true;

}
    
bool MinDist::loadModelFromFile(fstream &file){
    
    trained = false;
    numInputDimensions = 0;
    numClasses = 0;
    models.clear();
    classLabels.clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << endl;
        return false;
    }
    
    std::string word;
    
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_MINDIST_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Find the file type header
    if(word != "GRT_MINDIST_MODEL_FILE_V2.0"){
        errorLog << "loadModelFromFile(string filename) - Could not find Model File Header" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    if( trained ){
        
        //Resize the buffer
        models.resize(numClasses);
        classLabels.resize(numClasses);
        
        //Load each of the K models
        for(UINT k=0; k<numClasses; k++){
            double rejectionThreshold;
            double gamma;
            double trainingSigma;
            double trainingMu;
            
            file >> word;
            if( word != "ClassLabel:" ){
                errorLog << "loadModelFromFile(string filename) - Could not load the class label for class " << k << endl;
                return false;
            }
            file >> classLabels[k];
            
            file >> word;
            if( word != "NumClusters:" ){
                errorLog << "loadModelFromFile(string filename) - Could not load the NumClusters for class " << k << endl;
                return false;
            }
            file >> numClusters;
            
            file >> word;
            if( word != "RejectionThreshold:" ){
                errorLog << "loadModelFromFile(string filename) - Could not load the RejectionThreshold for class " << k << endl;
                return false;
            }
            file >> rejectionThreshold;
            
            file >> word;
            if( word != "Gamma:" ){
                errorLog << "loadModelFromFile(string filename) - Could not load the Gamma for class " << k << endl;
                return false;
            }
            file >> gamma;
            
            file >> word;
            if( word != "TrainingMu:" ){
                errorLog << "loadModelFromFile(string filename) - Could not load the TrainingMu for class " << k << endl;
                return false;
            }
            file >> trainingMu;
            
            file >> word;
            if( word != "TrainingSigma:" ){
                errorLog << "loadModelFromFile(string filename) - Could not load the TrainingSigma for class " << k << endl;
                return false;
            }
            file >> trainingSigma;
            
            file >> word;
            if( word != "ClusterData:" ){
                errorLog << "loadModelFromFile(string filename) - Could not load the ClusterData for class " << k << endl;
                return false;
            }
            
            //Load the cluster data
            MatrixDouble clusters(numClusters,numInputDimensions);
            for(UINT i=0; i<numClusters; i++){
                for(UINT j=0; j<numInputDimensions; j++){
                    file >> clusters[i][j];
                }
            }
            
            models[k].setClassLabel( classLabels[k] );
            models[k].setClusters( clusters );
            models[k].setGamma( gamma );
            models[k].setRejectionThreshold( rejectionThreshold );
            models[k].setTrainingSigma( trainingSigma );
            models[k].setTrainingMu( trainingMu );
        }
        
        //Recompute the null rejection thresholds
        recomputeNullRejectionThresholds();
        
        //Resize the prediction results to make sure it is setup for realtime prediction
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}
    
bool MinDist::setNumClusters(UINT numClusters){
    if( numClusters > 0 ){
        this->numClusters = numClusters;
        return true;
    }
    return true;
}
    
bool MinDist::loadLegacyModelFromFile( fstream &file ){
    
    string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadModelFromFile(string filename) - Could not find NumFeatures " << endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadModelFromFile(string filename) - Could not find NumClasses" << endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadModelFromFile(string filename) - Could not find UseScaling" << endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadModelFromFile(string filename) - Could not find UseNullRejection" << endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numInputDimensions);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the Ranges" << endl;
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
        double rejectionThreshold;
        double gamma;
        double trainingSigma;
        double trainingMu;
        
        file >> word;
        if( word != "ClassLabel:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the class label for class " << k << endl;
            return false;
        }
        file >> classLabels[k];
        
        file >> word;
        if( word != "NumClusters:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the NumClusters for class " << k << endl;
            return false;
        }
        file >> numClusters;
        
        file >> word;
        if( word != "RejectionThreshold:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the RejectionThreshold for class " << k << endl;
            return false;
        }
        file >> rejectionThreshold;
        
        file >> word;
        if( word != "Gamma:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the Gamma for class " << k << endl;
            return false;
        }
        file >> gamma;
        
        file >> word;
        if( word != "TrainingMu:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the TrainingMu for class " << k << endl;
            return false;
        }
        file >> trainingMu;
        
        file >> word;
        if( word != "TrainingSigma:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the TrainingSigma for class " << k << endl;
            return false;
        }
        file >> trainingSigma;
        
        file >> word;
        if( word != "ClusterData:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the ClusterData for class " << k << endl;
            return false;
        }
        
        //Load the cluster data
        MatrixDouble clusters(numClusters,numInputDimensions);
        for(UINT i=0; i<numClusters; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                file >> clusters[i][j];
            }
        }
        
        models[k].setClassLabel( classLabels[k] );
        models[k].setClusters( clusters );
        models[k].setGamma( gamma );
        models[k].setRejectionThreshold( rejectionThreshold );
        models[k].setTrainingSigma( trainingSigma );
        models[k].setTrainingMu( trainingMu );
    }
    
    //Recompute the null rejection thresholds
    recomputeNullRejectionThresholds();
    
    //Resize the prediction results to make sure it is setup for realtime prediction
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
    classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    
    trained = true;
    
    return true;
}

} //End of namespace GRT

