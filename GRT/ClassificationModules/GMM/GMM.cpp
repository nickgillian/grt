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
#include "GMM.h"

namespace GRT {
    
//Register the GMM module with the Classifier base class
RegisterClassifierModule< GMM > GMM::registerModule("GMM");
    
GMM::GMM(UINT numMixtureModels,bool useScaling,bool useNullRejection,double nullRejectionCoeff,UINT maxIter,double minChange){
    classType = "GMM";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG GMM]");
    errorLog.setProceedingText("[ERROR GMM]");
    warningLog.setProceedingText("[WARNING GMM]");
    
    this->numMixtureModels = numMixtureModels;
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    this->maxIter = maxIter;
    this->minChange = minChange;
}

GMM::GMM(const GMM &rhs){
    classType = "GMM";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG GMM]");
    errorLog.setProceedingText("[ERROR GMM]");
    warningLog.setProceedingText("[WARNING GMM]");
    *this = rhs;
}
    
GMM::~GMM(){}
    
GMM& GMM::operator=(const GMM &rhs){
    if( this != &rhs ){
        
        this->numMixtureModels = rhs.numMixtureModels;
        this->maxIter = rhs.maxIter;
        this->minChange = rhs.minChange;
        this->models = rhs.models;
        
        this->debugLog = rhs.debugLog;
        this->errorLog = rhs.errorLog;
        this->warningLog = rhs.warningLog;
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}
    
bool GMM::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        
        GMM *ptr = (GMM*)classifier;
        //Clone the GMM values
        this->numMixtureModels = ptr->numMixtureModels;
        this->maxIter = ptr->maxIter;
        this->minChange = ptr->minChange;
        this->models = ptr->models;
        
        this->debugLog = ptr->debugLog;
        this->errorLog = ptr->errorLog;
        this->warningLog = ptr->warningLog;
        
        //Clone the classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool GMM::predict_(VectorDouble &x){

	predictedClassLabel = 0;
	
    if( classDistances.size() != numClasses || classLikelihoods.size() != numClasses ){
        classDistances.resize(numClasses);
        classLikelihoods.resize(numClasses);
    }
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &x) - Mixture Models have not been trained!" << endl;
        return false;
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &x) - The size of the input vector (" << x.size() << ") does not match that of the number of features the model was trained with (" << numInputDimensions << ")." << endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT i=0; i<numInputDimensions; i++){
            x[i] = scale(x[i], ranges[i].minValue, ranges[i].maxValue, GMM_MIN_SCALE_VALUE, GMM_MAX_SCALE_VALUE);
        }
    }

	UINT bestIndex = 0;
	maxLikelihood = 0;
    bestDistance = 0;
    double sum = 0;
	for(UINT k=0; k<numClasses; k++){
        classDistances[k] = computeMixtureLikelihood(x,k);
        
        //cout << "K: " << k << " Dist: " << classDistances[k] << endl;
        classLikelihoods[k] = classDistances[k];
        sum += classLikelihoods[k];
		if( classLikelihoods[k] > bestDistance ){
			bestDistance = classLikelihoods[k];
			bestIndex = k;
		}
	}
    
    //Normalize the likelihoods
    for(unsigned int k=0; k<numClasses; k++){
        classLikelihoods[k] /= sum;
    }
    maxLikelihood = classLikelihoods[bestIndex];
    
    if( useNullRejection ){
        
        //cout << "Dist: " << classDistances[bestIndex] << " RejectionThreshold: " << models[bestIndex].getRejectionThreshold() << endl;
        
        //If the best distance is below the modles rejection threshold then set the predicted class label as the best class label
        //Otherwise set the predicted class label as the default null rejection class label of 0
        if( classDistances[bestIndex] >= models[bestIndex].getNullRejectionThreshold() ){
            predictedClassLabel = models[bestIndex].getClassLabel();
        }else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
   }else{
       //Get the predicted class label
       predictedClassLabel = models[bestIndex].getClassLabel();
   }
	
	return true;
}

bool GMM::train_(ClassificationData &trainingData){
    
    //Clear any old models
	clear();
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data is empty!" << endl;
        return false;
    }
    
    //Set the number of features and number of classes and resize the models buffer
    numInputDimensions = trainingData.getNumDimensions();
    numClasses = trainingData.getNumClasses();
    models.resize(numClasses);
    
    if( numInputDimensions >= 6 ){
        warningLog << "train_(ClassificationData &trainingData) - The number of features in your training data is high (" << numInputDimensions << ").  The GMMClassifier does not work well with high dimensional data, you might get better results from one of the other classifiers." << endl;
    }
    
    //Get the ranges of the training data if the training data is going to be scaled
	ranges = trainingData.getRanges();
    if( !trainingData.scale(GMM_MIN_SCALE_VALUE, GMM_MAX_SCALE_VALUE) ){
        errorLog << "train_(ClassificationData &trainingData) - Failed to scale training data!" << endl;
        return false;
    }

    //Fit a Mixture Model to each class (independently)
    for(UINT k=0; k<numClasses; k++){
        UINT classLabel = trainingData.getClassTracker()[k].classLabel;
        ClassificationData classData = trainingData.getClassData( classLabel );
        
        //Train the Mixture Model for this class
        GaussianMixtureModels gaussianMixtureModel;
        gaussianMixtureModel.setNumClusters( numMixtureModels );
        gaussianMixtureModel.setMinChange( minChange );
        gaussianMixtureModel.setMaxNumEpochs( maxIter );
        
        if( !gaussianMixtureModel.train( classData.getDataAsMatrixDouble() ) ){
            errorLog << "train_(ClassificationData &trainingData) - Failed to train Mixture Model for class " << classLabel << endl;
            return false;
        }
        
        //Setup the model container
        models[k].resize( numMixtureModels );
        models[k].setClassLabel( classLabel );
        
        //Store the mixture model in the container
        for(UINT j=0; j<numMixtureModels; j++){
            models[k][j].mu = gaussianMixtureModel.getMu().getRowVector(j);
            models[k][j].sigma = gaussianMixtureModel.getSigma()[j];
            
            //Compute the determinant and invSigma for the realtime prediction
            LUDecomposition ludcmp( models[k][j].sigma );
            if( !ludcmp.inverse( models[k][j].invSigma ) ){
                models.clear();
                errorLog << "train_(ClassificationData &trainingData) - Failed to invert Matrix for class " << classLabel << "!" << endl;
                return false;
            }
            models[k][j].det = ludcmp.det();
        }
        
        //Compute the normalize factor
        models[k].recomputeNormalizationFactor();
        
        //Compute the rejection thresholds
        double mu = 0;
        double sigma = 0;
        VectorDouble predictionResults(classData.getNumSamples(),0);
        for(UINT i=0; i<classData.getNumSamples(); i++){
            vector< double > sample = classData[i].getSample();
            predictionResults[i] = models[k].computeMixtureLikelihood( sample );
            mu += predictionResults[i];
        }
        
        //Update mu
        mu /= double( classData.getNumSamples() );
        
        //Calculate the standard deviation
        for(UINT i=0; i<classData.getNumSamples(); i++) 
            sigma += SQR( (predictionResults[i]-mu) );
        sigma = sqrt( sigma / (double(classData.getNumSamples())-1.0) );
        sigma = 0.2;
        
        //Set the models training mu and sigma 
        models[k].setTrainingMuAndSigma(mu,sigma);
        
        if( !models[k].recomputeNullRejectionThreshold(nullRejectionCoeff) && useNullRejection ){
            warningLog << "train_(ClassificationData &trainingData) - Failed to recompute rejection threshold for class " << classLabel << " - the nullRjectionCoeff value is too high!" << endl;
        }
        
        //cout << "Training Mu: " << mu << " TrainingSigma: " << sigma << " RejectionThreshold: " << models[k].getNullRejectionThreshold() << endl;
        //models[k].printModelValues();
    }
    
    //Reset the class labels
    classLabels.resize(numClasses);
    for(UINT k=0; k<numClasses; k++){
        classLabels[k] = models[k].getClassLabel();
    }
    
    //Resize the rejection thresholds
    nullRejectionThresholds.resize(numClasses);
    for(UINT k=0; k<numClasses; k++){
        nullRejectionThresholds[k] = models[k].getNullRejectionThreshold();
    }
    
    //Flag that the models have been trained
    trained = true;
    
    return true;
}
    
double GMM::computeMixtureLikelihood(const VectorDouble &x,const UINT k){
    if( k >= numClasses ){
        errorLog << "computeMixtureLikelihood(const VectorDouble x,const UINT k) - Invalid k value!" << endl;
        return 0;
    }
    return models[k].computeMixtureLikelihood(x);
}
    
bool GMM::saveModelToFile(fstream &file) const{
    
    if( !trained ){
        errorLog <<"saveGMMToFile(fstream &file) - The model has not been trained!" << endl;
        return false;
    }
    
    if( !file.is_open() )
    {
        errorLog <<"saveGMMToFile(fstream &file) - The file has not been opened!" << endl;
        return false;
    }
    
    //Write the header info
    file << "GRT_GMM_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    file << "NumMixtureModels: " << numMixtureModels << endl;
    
    if( trained ){
        //Write each of the models
        file << "Models:\n";
        for(UINT k=0; k<numClasses; k++){
            file << "ClassLabel: " << models[k].getClassLabel() << endl;
            file << "K: " << models[k].getK() << endl;
            file << "NormalizationFactor: " << models[k].getNormalizationFactor() << endl;
            file << "TrainingMu: " << models[k].getTrainingMu() << endl;
            file << "TrainingSigma: " << models[k].getTrainingSigma() << endl;
            file << "NullRejectionThreshold: " << models[k].getNullRejectionThreshold() << endl;
            
            for(UINT index=0; index<models[k].getK(); index++){
                file << "Determinant: " << models[k][index].det << endl;
                
                file << "Mu: ";
                for(UINT j=0; j<models[k][index].mu.size(); j++) file << "\t" << models[k][index].mu[j];
                file << endl;
                
                file << "Sigma:\n";
                for(UINT i=0; i<models[k][index].sigma.getNumRows(); i++){
                    for(UINT j=0; j<models[k][index].sigma.getNumCols(); j++){
                        file << models[k][index].sigma[i][j];
                        if( j < models[k][index].sigma.getNumCols()-1 ) file << "\t";
                    }
                    file << endl;
                }
                
                file << "InvSigma:\n";
                for(UINT i=0; i<models[k][index].invSigma.getNumRows(); i++){
                    for(UINT j=0; j<models[k][index].invSigma.getNumCols(); j++){
                        file << models[k][index].invSigma[i][j];
                        if( j < models[k][index].invSigma.getNumCols()-1 ) file << "\t";
                    }
                    file << endl;
                }
            }
            
            file << endl;
        }
    }
    
    return true;
}
    
bool GMM::loadModelFromFile(fstream &file){
    
    trained = false;
    numInputDimensions = 0;
    numClasses = 0;
    models.clear();
    classLabels.clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(fstream &file) - Could not open file to load model" << endl;
        return false;
    }
    
    std::string word;
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_GMM_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Find the file type header
    if(word != "GRT_GMM_MODEL_FILE_V2.0"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find Model File Header" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if(word != "NumMixtureModels:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NumMixtureModels" << endl;
        return false;
    }
    file >> numMixtureModels;
    
    if( trained ){
        
        //Read the model header
        file >> word;
        if(word != "Models:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find the Models Header" << endl;
            return false;
        }
        
        //Resize the buffer
        models.resize(numClasses);
        classLabels.resize(numClasses);
        
        //Load each of the models
        for(UINT k=0; k<numClasses; k++){
            UINT classLabel = 0;
            UINT K = 0;
            double normalizationFactor;
            double trainingMu;
            double trainingSigma;
            double rejectionThreshold;
            
            file >> word;
            if(word != "ClassLabel:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find the ClassLabel for model " << k+1 << endl;
                return false;
            }
            file >> classLabel;
            models[k].setClassLabel( classLabel );
            classLabels[k] = classLabel;
            
            file >> word;
            if(word != "K:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find K for model " << k+1 << endl;
                return false;
            }
            file >> K;
            
            file >> word;
            if(word != "NormalizationFactor:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find NormalizationFactor for model " << k+1 << endl;
                return false;
            }
            file >> normalizationFactor;
            models[k].setNormalizationFactor(normalizationFactor);
            
            file >> word;
            if(word != "TrainingMu:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find TrainingMu for model " << k+1 << endl;
                return false;
            }
            file >> trainingMu;
            
            file >> word;
            if(word != "TrainingSigma:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find TrainingSigma for model " << k+1 << endl;
                return false;
            }
            file >> trainingSigma;
            
            //Set the training mu and sigma
            models[k].setTrainingMuAndSigma(trainingMu, trainingSigma);
            
            file >> word;
            if(word != "NullRejectionThreshold:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find NullRejectionThreshold for model " << k+1 << endl;
                return false;
            }
            file >>rejectionThreshold;
            
            //Set the rejection threshold
            models[k].setNullRejectionThreshold(rejectionThreshold);
            
            //Resize the buffer for the mixture models
            models[k].resize(K);
            
            //Load the mixture models
            for(UINT index=0; index<models[k].getK(); index++){
                
                //Resize the memory for the current mixture model
                models[k][index].mu.resize( numInputDimensions );
                models[k][index].sigma.resize( numInputDimensions, numInputDimensions );
                models[k][index].invSigma.resize( numInputDimensions, numInputDimensions );
                
                file >> word;
                if(word != "Determinant:"){
                    errorLog << "loadModelFromFile(fstream &file) - Could not find the Determinant for model " << k+1 << endl;
                    return false;
                }
                file >> models[k][index].det;
                
                
                file >> word;
                if(word != "Mu:"){
                    errorLog << "loadModelFromFile(fstream &file) - Could not find Mu for model " << k+1 << endl;
                    return false;
                }
                for(UINT j=0; j<models[k][index].mu.size(); j++){
                    file >> models[k][index].mu[j];
                }
                
                
                file >> word;
                if(word != "Sigma:"){
                    errorLog << "loadModelFromFile(fstream &file) - Could not find Sigma for model " << k+1 << endl;
                    return false;
                }
                for(UINT i=0; i<models[k][index].sigma.getNumRows(); i++){
                    for(UINT j=0; j<models[k][index].sigma.getNumCols(); j++){
                        file >> models[k][index].sigma[i][j];
                    }
                }
                
                file >> word;
                if(word != "InvSigma:"){
                    errorLog << "loadModelFromFile(fstream &file) - Could not find InvSigma for model " << k+1 << endl;
                    return false;
                }
                for(UINT i=0; i<models[k][index].invSigma.getNumRows(); i++){
                    for(UINT j=0; j<models[k][index].invSigma.getNumCols(); j++){
                        file >> models[k][index].invSigma[i][j];
                    }
                }
                
            }
            
        }
        
        //Set the null rejection thresholds
        nullRejectionThresholds.resize(numClasses);
        for(UINT k=0; k<numClasses; k++) {
            models[k].recomputeNullRejectionThreshold(nullRejectionCoeff);
            nullRejectionThresholds[k] = models[k].getNullRejectionThreshold();
        }
        
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}
    
bool GMM::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the GMM model
    models.clear();
    
    return true;
}
    
bool GMM::recomputeNullRejectionThresholds(){
    
    if( trained ){
        for(UINT k=0; k<numClasses; k++) {
            models[k].recomputeNullRejectionThreshold(nullRejectionCoeff);
            nullRejectionThresholds[k] = models[k].getNullRejectionThreshold();
        }
        return true;
    }
    return false;
}
    
UINT GMM::getNumMixtureModels(){
    return numMixtureModels;
}
    
vector< MixtureModel > GMM::getModels(){
    if( trained ){ return models; }
    return vector< MixtureModel >();
}
    
bool GMM::setNumMixtureModels(UINT K){
    if( K > 0 ){
        numMixtureModels = K;
        return true;
    }
    return false;
}
bool GMM::setMinChange(double minChange){
    if( minChange > 0 ){
        this->minChange = minChange;
        return true;
    }
    return false;
}
bool GMM::setMaxIter(UINT maxIter){
    if( maxIter > 0 ){
        this->maxIter = maxIter;
        return true;
    }
    return false;
}
    
bool GMM::loadLegacyModelFromFile( fstream &file ){
    
    string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NumFeatures " << endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NumClasses" << endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "NumMixtureModels:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NumMixtureModels" << endl;
        return false;
    }
    file >> numMixtureModels;
    
    file >> word;
    if(word != "MaxIter:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find MaxIter" << endl;
        return false;
    }
    file >> maxIter;
    
    file >> word;
    if(word != "MinChange:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find MinChange" << endl;
        return false;
    }
    file >> minChange;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find UseScaling" << endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find UseNullRejection" << endl;
        return false;
    }
    file >> useNullRejection;
    
    file >> word;
    if(word != "NullRejectionCoeff:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find NullRejectionCoeff" << endl;
        return false;
    }
    file >> nullRejectionCoeff;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numInputDimensions);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find the Ranges" << endl;
            return false;
        }
        for(UINT n=0; n<ranges.size(); n++){
            file >> ranges[n].minValue;
            file >> ranges[n].maxValue;
        }
    }
    
    //Read the model header
    file >> word;
    if(word != "Models:"){
        errorLog << "loadModelFromFile(fstream &file) - Could not find the Models Header" << endl;
        return false;
    }
    
    //Resize the buffer
    models.resize(numClasses);
    classLabels.resize(numClasses);
    
    //Load each of the models
    for(UINT k=0; k<numClasses; k++){
        UINT classLabel = 0;
        UINT K = 0;
        double normalizationFactor;
        double trainingMu;
        double trainingSigma;
        double rejectionThreshold;
        
        file >> word;
        if(word != "ClassLabel:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find the ClassLabel for model " << k+1 << endl;
            return false;
        }
        file >> classLabel;
        models[k].setClassLabel( classLabel );
        classLabels[k] = classLabel;
        
        file >> word;
        if(word != "K:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find K for model " << k+1 << endl;
            return false;
        }
        file >> K;
        
        file >> word;
        if(word != "NormalizationFactor:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find NormalizationFactor for model " << k+1 << endl;
            return false;
        }
        file >> normalizationFactor;
        models[k].setNormalizationFactor(normalizationFactor);
        
        file >> word;
        if(word != "TrainingMu:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find TrainingMu for model " << k+1 << endl;
            return false;
        }
        file >> trainingMu;
        
        file >> word;
        if(word != "TrainingSigma:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find TrainingSigma for model " << k+1 << endl;
            return false;
        }
        file >> trainingSigma;
        
        //Set the training mu and sigma
        models[k].setTrainingMuAndSigma(trainingMu, trainingSigma);
        
        file >> word;
        if(word != "NullRejectionThreshold:"){
            errorLog << "loadModelFromFile(fstream &file) - Could not find NullRejectionThreshold for model " << k+1 << endl;
            return false;
        }
        file >>rejectionThreshold;
        
        //Set the rejection threshold
        models[k].setNullRejectionThreshold(rejectionThreshold);
        
        //Resize the buffer for the mixture models
        models[k].resize(K);
        
        //Load the mixture models
        for(UINT index=0; index<models[k].getK(); index++){
            
            //Resize the memory for the current mixture model
            models[k][index].mu.resize( numInputDimensions );
            models[k][index].sigma.resize( numInputDimensions, numInputDimensions );
            models[k][index].invSigma.resize( numInputDimensions, numInputDimensions );
            
            file >> word;
            if(word != "Determinant:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find the Determinant for model " << k+1 << endl;
                return false;
            }
            file >> models[k][index].det;
            
            
            file >> word;
            if(word != "Mu:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find Mu for model " << k+1 << endl;
                return false;
            }
            for(UINT j=0; j<models[k][index].mu.size(); j++){
                file >> models[k][index].mu[j];
            }
            
            
            file >> word;
            if(word != "Sigma:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find Sigma for model " << k+1 << endl;
                return false;
            }
            for(UINT i=0; i<models[k][index].sigma.getNumRows(); i++){
                for(UINT j=0; j<models[k][index].sigma.getNumCols(); j++){
                    file >> models[k][index].sigma[i][j];
                }
            }
            
            file >> word;
            if(word != "InvSigma:"){
                errorLog << "loadModelFromFile(fstream &file) - Could not find InvSigma for model " << k+1 << endl;
                return false;
            }
            for(UINT i=0; i<models[k][index].invSigma.getNumRows(); i++){
                for(UINT j=0; j<models[k][index].invSigma.getNumCols(); j++){
                    file >> models[k][index].invSigma[i][j];
                }
            }
            
        }
        
    }
    
    //Set the null rejection thresholds
    nullRejectionThresholds.resize(numClasses);
    for(UINT k=0; k<numClasses; k++) {
        models[k].recomputeNullRejectionThreshold(nullRejectionCoeff);
        nullRejectionThresholds[k] = models[k].getNullRejectionThreshold();
    }
    
    //Flag that the models have been trained
    trained = true;
    
    return true;
}
    
}//End of namespace GRT