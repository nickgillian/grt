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

#include "ANBC.h"

namespace GRT{

//Register the ANBC module with the Classifier base class
RegisterClassifierModule< ANBC > ANBC::registerModule("ANBC");

ANBC::ANBC(bool useScaling,bool useNullRejection,double nullRejectionCoeff)
{
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    weightsDataSet = false;
    classType = "ANBC";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG ANBC]");
    errorLog.setProceedingText("[ERROR ANBC]");
    trainingLog.setProceedingText("[TRAINING ANBC]");
    warningLog.setProceedingText("[WARNING ANBC]");
}
    
ANBC::ANBC(const ANBC &rhs){
    classType = "ANBC";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG ANBC]");
    errorLog.setProceedingText("[ERROR ANBC]");
    trainingLog.setProceedingText("[TRAINING ANBC]");
    warningLog.setProceedingText("[WARNING ANBC]");
    *this = rhs;
}

ANBC::~ANBC(void)
{
} 

ANBC& ANBC::operator=(const ANBC &rhs){
	if( this != &rhs ){
        //ANBC variables
        this->weightsDataSet = rhs.weightsDataSet;
        this->weightsData = rhs.weightsData;
		this->models = rhs.models;
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
	}
	return *this;
}

bool ANBC::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){

        ANBC *ptr = (ANBC*)classifier;
        //Clone the ANBC values 
        this->weightsDataSet = ptr->weightsDataSet;
        this->weightsData = ptr->weightsData;
		this->models = ptr->models;
        
        //Clone the classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}
    
bool ANBC::train_(ClassificationData &labelledTrainingData){
    
    //Clear any previous model
    clear();
    
    const unsigned int M = labelledTrainingData.getNumSamples();
    const unsigned int N = labelledTrainingData.getNumDimensions();
    const unsigned int K = labelledTrainingData.getNumClasses();
    
    if( M == 0 ){
        errorLog << "train_(ClassificationData &labelledTrainingData) - Training data has zero samples!" << endl;
        return false;
    }
    
    if( weightsDataSet ){
        if( weightsData.getNumDimensions() != N ){
            errorLog << "train_(ClassificationData &labelledTrainingData) - The number of dimensions in the weights data (" << weightsData.getNumDimensions() << ") is not equal to the number of dimensions of the training data (" << N << ")" << endl;
            return false;
        }
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
        
        //Get the weights for this class
        VectorDouble weights(numInputDimensions);
        if( weightsDataSet ){
            bool weightsFound = false;
            for(UINT i=0; i<weightsData.getNumSamples(); i++){
                if( weightsData[i].getClassLabel() == classLabel ){
                    weights = weightsData[i].getSample();
                    weightsFound = true;
                    break;
                }
            }
            
            if( !weightsFound ){
                errorLog << "train_(ClassificationData &labelledTrainingData) - Failed to find the weights for class " << classLabel << endl;
                return false;
            }
        }else{
            //If the weights data has not been set then all the weights are 1
            for(UINT j=0; j<numInputDimensions; j++) weights[j] = 1.0;
        }
        
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
        models[k].gamma = nullRejectionCoeff;
        if( !models[k].train(classLabel,data,weights) ){
            errorLog << "train_(ClassificationData &labelledTrainingData) - Failed to train model for class: " << classLabel << endl;
            
            //Try and work out why the training failed
            if( models[k].N == 0 ){
                errorLog << "train_(ClassificationData &labelledTrainingData) - N == 0!" << endl;
                models.clear();
                return false;
            }
            for(UINT j=0; j<numInputDimensions; j++){
                if( models[k].sigma[j] == 0 ){
                    errorLog << "train_(ClassificationData &labelledTrainingData) - The standard deviation of column " << j+1 << " is zero! Check the training data" << endl;
                    models.clear();
                    return false;
                }
            }
            models.clear();
            return false;
        }
        
    }
    
    //Store the null rejection thresholds
    nullRejectionThresholds.resize(numClasses);
    for(UINT k=0; k<numClasses; k++) {
        nullRejectionThresholds[k] = models[k].threshold;
    }
    
    //Flag that the models have been trained
    trained = true;
    return trained;
}
    
bool ANBC::predict_(VectorDouble &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - ANBC Model Not Trained!" << endl;
        return false;
    }
    
    predictedClassLabel = 0;
	maxLikelihood = -10000;
    
    if( !trained ) return false;
    
	if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, MIN_SCALE_VALUE, MAX_SCALE_VALUE);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    double classLikelihoodsSum = 0;
    double minDist = -99e+99;
	for(UINT k=0; k<numClasses; k++){
		classDistances[k] = models[k].predict( inputVector );
        
        //At this point the class likelihoods and class distances are the same thing
        classLikelihoods[k] = classDistances[k];
        
        //If the distances are very far away then they could be -inf or nan so catch this so the sum still works
        if( std::isinf(classLikelihoods[k]) || std::isnan(classLikelihoods[k]) ){
            classLikelihoods[k] = 0;
        }else{
            classLikelihoods[k] = exp( classLikelihoods[k] );
            classLikelihoodsSum += classLikelihoods[k];

            //The loglikelihood values are negative so we want the values closest to 0
            if( classDistances[k] > minDist ){
                minDist = classDistances[k];
                predictedClassLabel = k;
            }
        }
    }

    //If the class likelihoods sum is zero then all classes are -INF
    if( classLikelihoodsSum == 0 ){
        predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
        maxLikelihood = 0;
        return true;
    }
    
    //Normalize the classlikelihoods
    for(UINT k=0; k<numClasses; k++){
        classLikelihoods[k] /= classLikelihoodsSum;
    }
    maxLikelihood = classLikelihoods[predictedClassLabel];
    
    if( useNullRejection ){
        //Check to see if the best result is greater than the models threshold
        if( minDist >= models[predictedClassLabel].threshold ) predictedClassLabel = models[predictedClassLabel].classLabel;
        else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    }else predictedClassLabel = models[predictedClassLabel].classLabel;
    
    return true;
}

bool ANBC::recomputeNullRejectionThresholds(){

    if( trained ){
        if( nullRejectionThresholds.size() != numClasses )
            nullRejectionThresholds.resize(numClasses);
        for(UINT k=0; k<numClasses; k++) {
            models[k].recomputeThresholdValue(nullRejectionCoeff);
            nullRejectionThresholds[k] = models[k].threshold;
        }
        return true;
    }
    return false;
}
    
bool ANBC::reset(){
    return true;
}

bool ANBC::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the ANBC model
    weightsData.clear();
    models.clear();
    
    return true;
}
    
bool ANBC::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveANBCModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file<<"GRT_ANBC_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    if( trained ){
        //Write each of the models
        for(UINT k=0; k<numClasses; k++){
            file<<"*************_MODEL_*************\n";
            file<<"Model_ID: "<<k+1<<endl;
            file<<"N: "<<models[k].N<<endl;
            file<<"ClassLabel: "<<models[k].classLabel<<endl;
            file<<"Threshold: "<<models[k].threshold<<endl;
            file<<"Gamma: "<<models[k].gamma<<endl;
            file<<"TrainingMu: "<<models[k].trainingMu<<endl;
            file<<"TrainingSigma: "<<models[k].trainingSigma<<endl;
            
            file<<"Mu:\n";
            for(UINT j=0; j<models[k].N; j++){
                file << "\t" << models[k].mu[j];
            }file<<endl;
            
            file<<"Sigma:\n";
            for(UINT j=0; j<models[k].N; j++){
                file << "\t" << models[k].sigma[j];
            }file<<endl;
            
            file<<"Weights:\n";
            for(UINT j=0; j<models[k].N; j++){
                file << "\t" << models[k].weights[j];
            }file<<endl;
        }
    }
    
    return true;
}
    
bool ANBC::loadModelFromFile(fstream &file){
    
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
    if( word == "GRT_ANBC_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Find the file type header
    if(word != "GRT_ANBC_MODEL_FILE_V2.0"){
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
        
        //Load each of the K models
        for(UINT k=0; k<numClasses; k++){
            UINT modelID;
            file >> word;
            if(word != "*************_MODEL_*************"){
                errorLog << "loadModelFromFile(string filename) - Could not find header for the "<<k+1<<"th model" << endl;
                return false;
            }
            
            file >> word;
            if(word != "Model_ID:"){
                errorLog << "loadModelFromFile(string filename) - Could not find model ID for the "<<k+1<<"th model" << endl;
                return false;
            }
            file >> modelID;
            
            if(modelID-1!=k){
                cout<<"ANBC: Model ID does not match the current class ID for the "<<k+1<<"th model" << endl;
                return false;
            }
            
            file >> word;
            if(word != "N:"){
                cout<<"ANBC: Could not find N for the "<<k+1<<"th model" << endl;
                return false;
            }
            file >> models[k].N;
            
            file >> word;
            if(word != "ClassLabel:"){
                errorLog << "loadModelFromFile(string filename) - Could not find ClassLabel for the "<<k+1<<"th model" << endl;
                return false;
            }
            file >> models[k].classLabel;
            classLabels[k] = models[k].classLabel;
            
            file >> word;
            if(word != "Threshold:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the threshold for the "<<k+1<<"th model" << endl;
                return false;
            }
            file >> models[k].threshold;
            
            file >> word;
            if(word != "Gamma:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the gamma parameter for the "<<k+1<<"th model" << endl;
                return false;
            }
            file >> models[k].gamma;
            
            file >> word;
            if(word != "TrainingMu:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the training mu parameter for the "<<k+1<<"th model" << endl;
                return false;
            }
            file >> models[k].trainingMu;
            
            file >> word;
            if(word != "TrainingSigma:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the training sigma parameter for the "<<k+1<<"th model" << endl;
                return false;
            }
            file >> models[k].trainingSigma;
            
            //Resize the buffers
            models[k].mu.resize(numInputDimensions);
            models[k].sigma.resize(numInputDimensions);
            models[k].weights.resize(numInputDimensions);
            
            //Load Mu, Sigma and Weights
            file >> word;
            if(word != "Mu:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the Mu vector for the "<<k+1<<"th model" << endl;
                return false;
            }
            
            //Load Mu
            for(UINT j=0; j<models[k].N; j++){
                double value;
                file >> value;
                models[k].mu[j] = value;
            }
            
            file >> word;
            if(word != "Sigma:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the Sigma vector for the "<<k+1<<"th model" << endl;
                return false;
            }
            
            //Load Sigma
            for(UINT j=0; j<models[k].N; j++){
                double value;
                file >> value;
                models[k].sigma[j] = value;
            }
            
            file >> word;
            if(word != "Weights:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the Weights vector for the "<<k+1<<"th model" << endl;
                return false;
            }
            
            //Load Weights
            for(UINT j=0; j<models[k].N; j++){
                double value;
                file >> value;
                models[k].weights[j] = value;
            }
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
    
VectorDouble ANBC::getNullRejectionThresholds() const{
    if( !trained ) return VectorDouble();
    return nullRejectionThresholds;
}
    
bool ANBC::setNullRejectionCoeff(double nullRejectionCoeff){
    
    if( nullRejectionCoeff > 0 ){
        this->nullRejectionCoeff = nullRejectionCoeff;
        recomputeNullRejectionThresholds();
        return true;
    }
    return false;
}

bool ANBC::setWeights(const ClassificationData &weightsData){
    
    if( weightsData.getNumSamples() > 0 ){
        weightsDataSet = true;
        this->weightsData = weightsData;
        return true;
    }
    return false;
}
    
bool ANBC::loadLegacyModelFromFile( fstream &file ){
    
    string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find NumFeatures " << endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find NumClasses" << endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find UseScaling" << endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find UseNullRejection" << endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numInputDimensions);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Ranges" << endl;
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
        UINT modelID;
        file >> word;
        if(word != "*************_MODEL_*************"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find header for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        file >> word;
        if(word != "Model_ID:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find model ID for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> modelID;
        
        if(modelID-1!=k){
            cout<<"ANBC: Model ID does not match the current class ID for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        file >> word;
        if(word != "N:"){
            cout<<"ANBC: Could not find N for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].N;
        
        file >> word;
        if(word != "ClassLabel:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find ClassLabel for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].classLabel;
        classLabels[k] = models[k].classLabel;
        
        file >> word;
        if(word != "Threshold:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the threshold for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].threshold;
        
        file >> word;
        if(word != "Gamma:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the gamma parameter for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].gamma;
        
        file >> word;
        if(word != "TrainingMu:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the training mu parameter for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].trainingMu;
        
        file >> word;
        if(word != "TrainingSigma:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the training sigma parameter for the "<<k+1<<"th model" << endl;
            return false;
        }
        file >> models[k].trainingSigma;
        
        //Resize the buffers
        models[k].mu.resize(numInputDimensions);
        models[k].sigma.resize(numInputDimensions);
        models[k].weights.resize(numInputDimensions);
        
        //Load Mu, Sigma and Weights
        file >> word;
        if(word != "Mu:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Mu vector for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        //Load Mu
        for(UINT j=0; j<models[k].N; j++){
            double value;
            file >> value;
            models[k].mu[j] = value;
        }
        
        file >> word;
        if(word != "Sigma:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Sigma vector for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        //Load Sigma
        for(UINT j=0; j<models[k].N; j++){
            double value;
            file >> value;
            models[k].sigma[j] = value;
        }
        
        file >> word;
        if(word != "Weights:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Weights vector for the "<<k+1<<"th model" << endl;
            return false;
        }
        
        //Load Weights
        for(UINT j=0; j<models[k].N; j++){
            double value;
            file >> value;
            models[k].weights[j] = value;
        }
        
        file >> word;
        if(word != "*********************************"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the model footer for the "<<k+1<<"th model" << endl;
            return false;
        }
    }
    
    //Flag that the model is trained
    trained = true;
    
    //Recompute the null rejection thresholds
    recomputeNullRejectionThresholds();
    
    //Resize the prediction results to make sure it is setup for realtime prediction
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
    classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    
    return true;
    
}

} //End of namespace GRT

