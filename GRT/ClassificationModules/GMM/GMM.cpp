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
#include "GMM.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string GMM::id = "GMM";
std::string GMM::getId() { return GMM::id; }

//Register the GMM module with the Classifier base class
RegisterClassifierModule< GMM > GMM::registerModule( GMM::getId() );

GMM::GMM(UINT numMixtureModels,bool useScaling,bool useNullRejection,Float nullRejectionCoeff,UINT maxNumEpochs,Float minChange) : Classifier( GMM::getId() )
{
    classifierMode = STANDARD_CLASSIFIER_MODE;
    this->numMixtureModels = numMixtureModels;
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    this->maxNumEpochs = maxNumEpochs;
    this->minChange = minChange;
}

GMM::GMM(const GMM &rhs) : Classifier( GMM::getId() )
{
    classifierMode = STANDARD_CLASSIFIER_MODE;
    *this = rhs;
}

GMM::~GMM(){}

GMM& GMM::operator=(const GMM &rhs){
    if( this != &rhs ){
        
        this->numMixtureModels = rhs.numMixtureModels;
        this->models = rhs.models;
        
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
        this->models = ptr->models;
        
        //Clone the classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool GMM::predict_(VectorFloat &x){
    
    predictedClassLabel = 0;
    
    if( classDistances.getSize() != numClasses || classLikelihoods.getSize() != numClasses ){
        classDistances.resize(numClasses);
        classLikelihoods.resize(numClasses);
    }
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &x) - Mixture Models have not been trained!" << std::endl;
        return false;
    }
    
    if( x.getSize() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &x) - The size of the input vector (" << x.getSize() << ") does not match that of the number of features the model was trained with (" << numInputDimensions << ")." << std::endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT i=0; i<numInputDimensions; i++){
            x[i] = grt_scale(x[i], ranges[i].minValue, ranges[i].maxValue, GMM_MIN_SCALE_VALUE, GMM_MAX_SCALE_VALUE);
        }
    }
    
    UINT bestIndex = 0;
    maxLikelihood = 0;
    bestDistance = 0;
    Float sum = 0;
    for(UINT k=0; k<numClasses; k++){
        classDistances[k] = computeMixtureLikelihood(x,k);
        
        //cout << "K: " << k << " Dist: " << classDistances[k] << std::endl;
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
        
        //cout << "Dist: " << classDistances[bestIndex] << " RejectionThreshold: " << models[bestIndex].getRejectionThreshold() << std::endl;
        
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
        errorLog << "train_(ClassificationData &trainingData) - Training data is empty!" << std::endl;
        return false;
    }
    
    //Set the number of features and number of classes and resize the models buffer
    numInputDimensions = trainingData.getNumDimensions();
    numOutputDimensions = trainingData.getNumClasses();
    numClasses = trainingData.getNumClasses();
    models.resize(numClasses);
    
    if( numInputDimensions >= 6 ){
        warningLog << "train_(ClassificationData &trainingData) - The number of features in your training data is high (" << numInputDimensions << ").  The GMMClassifier does not work well with high dimensional data, you might get better results from one of the other classifiers." << std::endl;
    }
    
    //Get the ranges of the training data if the training data is going to be scaled
    ranges = trainingData.getRanges();

    ClassificationData validationData;
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        if( !trainingData.scale(GMM_MIN_SCALE_VALUE, GMM_MAX_SCALE_VALUE) ){
            errorLog << "train_(ClassificationData &trainingData) - Failed to scale training data!" << std::endl;
            return false;
        }
    }

    if( useValidationSet ){
        validationData = trainingData.split( 100-validationSetSize );
    }
    
    //Fit a Mixture Model to each class (independently)
    for(UINT k=0; k<numClasses; k++){
        UINT classLabel = trainingData.getClassTracker()[k].classLabel;
        ClassificationData classData = trainingData.getClassData( classLabel );
        
        //Train the Mixture Model for this class
        GaussianMixtureModels gaussianMixtureModel;
        gaussianMixtureModel.setNumClusters( numMixtureModels );
        gaussianMixtureModel.setMinChange( minChange );
        gaussianMixtureModel.setMaxNumEpochs( maxNumEpochs );
        gaussianMixtureModel.setNumRestarts( 10 ); //The learning algorithm can retry building a model up to N times
        
        if( !gaussianMixtureModel.train( classData.getDataAsMatrixFloat() ) ){
            errorLog << "train_(ClassificationData &trainingData) - Failed to train Mixture Model for class " << classLabel << std::endl;
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
                errorLog << "train_(ClassificationData &trainingData) - Failed to invert Matrix for class " << classLabel << "!" << std::endl;
                    return false;
            }
            models[k][j].det = ludcmp.det();
        }
        
        //Compute the normalize factor
        models[k].recomputeNormalizationFactor();
        
        //Compute the rejection thresholds
        Float mu = 0;
        Float sigma = 0;
        VectorFloat predictionResults(classData.getNumSamples(),0);
        for(UINT i=0; i<classData.getNumSamples(); i++){
            VectorFloat sample = classData[i].getSample();
            predictionResults[i] = models[k].computeMixtureLikelihood( sample );
            mu += predictionResults[i];
        }
        
        //Update mu
        mu /= Float( classData.getNumSamples() );
        
        //Calculate the standard deviation
        for(UINT i=0; i<classData.getNumSamples(); i++)
        sigma += grt_sqr( (predictionResults[i]-mu) );
        sigma = grt_sqrt( sigma / (Float(classData.getNumSamples())-1.0) );
        sigma = 0.2;
        
        //Set the models training mu and sigma
        models[k].setTrainingMuAndSigma(mu,sigma);
        
        if( !models[k].recomputeNullRejectionThreshold(nullRejectionCoeff) && useNullRejection ){
            warningLog << "train_(ClassificationData &trainingData) - Failed to recompute rejection threshold for class " << classLabel << " - the nullRjectionCoeff value is too high!" << std::endl;
            }
        
        //cout << "Training Mu: " << mu << " TrainingSigma: " << sigma << " RejectionThreshold: " << models[k].getNullRejectionThreshold() << std::endl;
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
    
    //Flag that the model has been trained
    trained = true;

    //Compute the final training stats
    trainingSetAccuracy = 0;
    validationSetAccuracy = 0;

    //If scaling was on, then the data will already be scaled, so turn it off temporially so we can test the model accuracy
    bool scalingState = useScaling;
    useScaling = false;
    if( !computeAccuracy( trainingData, trainingSetAccuracy ) ){
        trained = false;
        errorLog << "Failed to compute training set accuracy! Failed to fully train model!" << std::endl;
        return false;
    }
    
    if( useValidationSet ){
        if( !computeAccuracy( validationData, validationSetAccuracy ) ){
            trained = false;
            errorLog << "Failed to compute validation set accuracy! Failed to fully train model!" << std::endl;
            return false;
        }
        
    }

    trainingLog << "Training set accuracy: " << trainingSetAccuracy << std::endl;

    if( useValidationSet ){
        trainingLog << "Validation set accuracy: " << validationSetAccuracy << std::endl;
    }

    //Reset the scaling state for future prediction
    useScaling = scalingState;
    
    return trained;
}

Float GMM::computeMixtureLikelihood(const VectorFloat &x,const UINT k){
    if( k >= numClasses ){
        errorLog << "computeMixtureLikelihood(const VectorFloat x,const UINT k) - Invalid k value!" << std::endl;
        return 0;
    }
    return models[k].computeMixtureLikelihood( x );
}

bool GMM::save( std::fstream &file ) const{
    
    if( !trained ){
        errorLog <<"saveGMMToFile(fstream &file) - The model has not been trained!" << std::endl;
        return false;
    }
    
    if( !file.is_open() )
    {
        errorLog <<"saveGMMToFile(fstream &file) - The file has not been opened!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "GRT_GMM_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    file << "NumMixtureModels: " << numMixtureModels << std::endl;
    
    if( trained ){
        //Write each of the models
        file << "Models:\n";
        for(UINT k=0; k<numClasses; k++){
            file << "ClassLabel: " << models[k].getClassLabel() << std::endl;
            file << "K: " << models[k].getK() << std::endl;
            file << "NormalizationFactor: " << models[k].getNormalizationFactor() << std::endl;
            file << "TrainingMu: " << models[k].getTrainingMu() << std::endl;
            file << "TrainingSigma: " << models[k].getTrainingSigma() << std::endl;
            file << "NullRejectionThreshold: " << models[k].getNullRejectionThreshold() << std::endl;
            
            for(UINT index=0; index<models[k].getK(); index++){
                file << "Determinant: " << models[k][index].det << std::endl;
                
                file << "Mu: ";
                for(UINT j=0; j<models[k][index].mu.size(); j++) file << "\t" << models[k][index].mu[j];
                file << std::endl;
                
                file << "Sigma:\n";
                for(UINT i=0; i<models[k][index].sigma.getNumRows(); i++){
                    for(UINT j=0; j<models[k][index].sigma.getNumCols(); j++){
                        file << models[k][index].sigma[i][j];
                        if( j < models[k][index].sigma.getNumCols()-1 ) file << "\t";
                    }
                    file << std::endl;
                }
                
                file << "InvSigma:\n";
                for(UINT i=0; i<models[k][index].invSigma.getNumRows(); i++){
                    for(UINT j=0; j<models[k][index].invSigma.getNumCols(); j++){
                        file << models[k][index].invSigma[i][j];
                        if( j < models[k][index].invSigma.getNumCols()-1 ) file << "\t";
                    }
                    file << std::endl;
                }
            }
            
            file << std::endl;
        }
    }
    
    return true;
}

bool GMM::load( std::fstream &file ){
    
    trained = false;
    numInputDimensions = 0;
    numClasses = 0;
    models.clear();
    classLabels.clear();
    
    if(!file.is_open())
    {
        errorLog << "load(fstream &file) - Could not open file to load model" << std::endl;
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
        errorLog << "load(fstream &file) - Could not find Model File Header" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "NumMixtureModels:"){
        errorLog << "load(fstream &file) - Could not find NumMixtureModels" << std::endl;
        return false;
    }
    file >> numMixtureModels;
    
    if( trained ){
        
        //Read the model header
        file >> word;
        if(word != "Models:"){
            errorLog << "load(fstream &file) - Could not find the Models Header" << std::endl;
            return false;
        }
        
        //Resize the buffer
        models.resize(numClasses);
        classLabels.resize(numClasses);
        
        //Load each of the models
        for(UINT k=0; k<numClasses; k++){
            UINT classLabel = 0;
            UINT K = 0;
            Float normalizationFactor;
            Float trainingMu;
            Float trainingSigma;
            Float rejectionThreshold;
            
            file >> word;
            if(word != "ClassLabel:"){
                errorLog << "load(fstream &file) - Could not find the ClassLabel for model " << k+1 << std::endl;
                    return false;
            }
            file >> classLabel;
            models[k].setClassLabel( classLabel );
            classLabels[k] = classLabel;
            
            file >> word;
            if(word != "K:"){
                errorLog << "load(fstream &file) - Could not find K for model " << k+1 << std::endl;
                    return false;
            }
            file >> K;
            
            file >> word;
            if(word != "NormalizationFactor:"){
                errorLog << "load(fstream &file) - Could not find NormalizationFactor for model " << k+1 << std::endl;
                    return false;
            }
            file >> normalizationFactor;
            models[k].setNormalizationFactor(normalizationFactor);
            
            file >> word;
            if(word != "TrainingMu:"){
                errorLog << "load(fstream &file) - Could not find TrainingMu for model " << k+1 << std::endl;
                    return false;
            }
            file >> trainingMu;
            
            file >> word;
            if(word != "TrainingSigma:"){
                errorLog << "load(fstream &file) - Could not find TrainingSigma for model " << k+1 << std::endl;
                    return false;
            }
            file >> trainingSigma;
            
            //Set the training mu and sigma
            models[k].setTrainingMuAndSigma(trainingMu, trainingSigma);
            
            file >> word;
            if(word != "NullRejectionThreshold:"){
                errorLog << "load(fstream &file) - Could not find NullRejectionThreshold for model " << k+1 << std::endl;
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
                    errorLog << "load(fstream &file) - Could not find the Determinant for model " << k+1 << std::endl;
                        return false;
                }
                file >> models[k][index].det;
                
                
                file >> word;
                if(word != "Mu:"){
                    errorLog << "load(fstream &file) - Could not find Mu for model " << k+1 << std::endl;
                        return false;
                }
                for(UINT j=0; j<models[k][index].mu.size(); j++){
                    file >> models[k][index].mu[j];
                }
                
                
                file >> word;
                if(word != "Sigma:"){
                    errorLog << "load(fstream &file) - Could not find Sigma for model " << k+1 << std::endl;
                        return false;
                }
                for(UINT i=0; i<models[k][index].sigma.getNumRows(); i++){
                    for(UINT j=0; j<models[k][index].sigma.getNumCols(); j++){
                        file >> models[k][index].sigma[i][j];
                    }
                }
                
                file >> word;
                if(word != "InvSigma:"){
                    errorLog << "load(fstream &file) - Could not find InvSigma for model " << k+1 << std::endl;
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

Vector< MixtureModel > GMM::getModels(){
    if( trained ){ return models; }
    return Vector< MixtureModel >();
}

bool GMM::setNumMixtureModels(const UINT K){
    if( K > 0 ){
        numMixtureModels = K;
        return true;
    }
    return false;
}

bool GMM::setMaxIter(UINT maxNumEpochs){
    return setMaxNumEpochs( maxNumEpochs );
}

bool GMM::loadLegacyModelFromFile( std::fstream &file ){
    
    std::string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "load(fstream &file) - Could not find NumFeatures " << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "load(fstream &file) - Could not find NumClasses" << std::endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "NumMixtureModels:"){
        errorLog << "load(fstream &file) - Could not find NumMixtureModels" << std::endl;
        return false;
    }
    file >> numMixtureModels;
    
    file >> word;
    if(word != "MaxIter:"){
        errorLog << "load(fstream &file) - Could not find MaxIter" << std::endl;
        return false;
    }
    file >> maxNumEpochs;
    
    file >> word;
    if(word != "MinChange:"){
        errorLog << "load(fstream &file) - Could not find MinChange" << std::endl;
        return false;
    }
    file >> minChange;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "load(fstream &file) - Could not find UseScaling" << std::endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "load(fstream &file) - Could not find UseNullRejection" << std::endl;
        return false;
    }
    file >> useNullRejection;
    
    file >> word;
    if(word != "NullRejectionCoeff:"){
        errorLog << "load(fstream &file) - Could not find NullRejectionCoeff" << std::endl;
        return false;
    }
    file >> nullRejectionCoeff;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numInputDimensions);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "load(fstream &file) - Could not find the Ranges" << std::endl;
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
        errorLog << "load(fstream &file) - Could not find the Models Header" << std::endl;
        return false;
    }
    
    //Resize the buffer
    models.resize(numClasses);
    classLabels.resize(numClasses);
    
    //Load each of the models
    for(UINT k=0; k<numClasses; k++){
        UINT classLabel = 0;
        UINT K = 0;
        Float normalizationFactor;
        Float trainingMu;
        Float trainingSigma;
        Float rejectionThreshold;
        
        file >> word;
        if(word != "ClassLabel:"){
            errorLog << "load(fstream &file) - Could not find the ClassLabel for model " << k+1 << std::endl;
                return false;
        }
        file >> classLabel;
        models[k].setClassLabel( classLabel );
        classLabels[k] = classLabel;
        
        file >> word;
        if(word != "K:"){
            errorLog << "load(fstream &file) - Could not find K for model " << k+1 << std::endl;
                return false;
        }
        file >> K;
        
        file >> word;
        if(word != "NormalizationFactor:"){
            errorLog << "load(fstream &file) - Could not find NormalizationFactor for model " << k+1 << std::endl;
                return false;
        }
        file >> normalizationFactor;
        models[k].setNormalizationFactor(normalizationFactor);
        
        file >> word;
        if(word != "TrainingMu:"){
            errorLog << "load(fstream &file) - Could not find TrainingMu for model " << k+1 << std::endl;
                return false;
        }
        file >> trainingMu;
        
        file >> word;
        if(word != "TrainingSigma:"){
            errorLog << "load(fstream &file) - Could not find TrainingSigma for model " << k+1 << std::endl;
                return false;
        }
        file >> trainingSigma;
        
        //Set the training mu and sigma
        models[k].setTrainingMuAndSigma(trainingMu, trainingSigma);
        
        file >> word;
        if(word != "NullRejectionThreshold:"){
            errorLog << "load(fstream &file) - Could not find NullRejectionThreshold for model " << k+1 << std::endl;
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
                errorLog << "load(fstream &file) - Could not find the Determinant for model " << k+1 << std::endl;
                    return false;
            }
            file >> models[k][index].det;
            
            
            file >> word;
            if(word != "Mu:"){
                errorLog << "load(fstream &file) - Could not find Mu for model " << k+1 << std::endl;
                    return false;
            }
            for(UINT j=0; j<models[k][index].mu.size(); j++){
                file >> models[k][index].mu[j];
            }
            
            
            file >> word;
            if(word != "Sigma:"){
                errorLog << "load(fstream &file) - Could not find Sigma for model " << k+1 << std::endl;
                    return false;
            }
            for(UINT i=0; i<models[k][index].sigma.getNumRows(); i++){
                for(UINT j=0; j<models[k][index].sigma.getNumCols(); j++){
                    file >> models[k][index].sigma[i][j];
                }
            }
            
            file >> word;
            if(word != "InvSigma:"){
                errorLog << "load(fstream &file) - Could not find InvSigma for model " << k+1 << std::endl;
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

GRT_END_NAMESPACE
