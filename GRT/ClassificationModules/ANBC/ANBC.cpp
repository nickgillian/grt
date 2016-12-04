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
#include "ANBC.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string ANBC::id = "ANBC";
std::string ANBC::getId() { return ANBC::id; }

//Register the ANBC module with the Classifier base class
RegisterClassifierModule< ANBC > ANBC::registerModule( ANBC::getId() );

ANBC::ANBC(bool useScaling,bool useNullRejection,Float nullRejectionCoeff) : Classifier( ANBC::getId() )
{
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    supportsNullRejection = true;
    weightsDataSet = false;
    classifierMode = STANDARD_CLASSIFIER_MODE;
}

ANBC::ANBC(const ANBC &rhs) : Classifier( ANBC::getId() ) 
{
    classifierMode = STANDARD_CLASSIFIER_MODE;
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
    
    if( this->getId() == classifier->getId() ){
        
        const ANBC *ptr = dynamic_cast<const ANBC*>(classifier);

        //Clone the ANBC values
        this->weightsDataSet = ptr->weightsDataSet;
        this->weightsData = ptr->weightsData;
        this->models = ptr->models;
        
        //Clone the classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool ANBC::train_(ClassificationData &trainingData){
    
    //Clear any previous model
    clear();
    
    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int K = trainingData.getNumClasses();
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << std::endl;
        return false;
    }
    
    if( weightsDataSet ){
        if( weightsData.getNumDimensions() != N ){
            errorLog << "train_(ClassificationData &trainingData) - The number of dimensions in the weights data (" << weightsData.getNumDimensions() << ") is not equal to the number of dimensions of the training data (" << N << ")" << std::endl;
            return false;
        }
    }
    
    numInputDimensions = N;
    numOutputDimensions = K;
    numClasses = K;
    models.resize(K);
    classLabels.resize(K);
    ranges = trainingData.getRanges();
    ClassificationData validationData;
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }

    if( useValidationSet ){
        validationData = trainingData.split( 100-validationSetSize );
    }

    const UINT M = trainingData.getNumSamples();
    trainingLog << "Training Naive Bayes model, num training examples: " << M << ", num validation examples: " << validationData.getNumSamples() << ", num classes: " << numClasses << std::endl;
    
    //Train each of the models
    for(UINT k=0; k<numClasses; k++){
        
        //Get the class label for the kth class
        UINT classLabel = trainingData.getClassTracker()[k].classLabel;
        
        //Set the kth class label
        classLabels[k] = classLabel;
        
        //Get the weights for this class
        VectorFloat weights(numInputDimensions);
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
                errorLog << "train_(ClassificationData &trainingData) - Failed to find the weights for class " << classLabel << std::endl;
                    return false;
            }
        }else{
            //If the weights data has not been set then all the weights are 1
            for(UINT j=0; j<numInputDimensions; j++) weights[j] = 1.0;
        }
        
        //Get all the training data for this class
        ClassificationData classData = trainingData.getClassData(classLabel);
        MatrixFloat data(classData.getNumSamples(),N);
        
        //Copy the training data into a matrix
        for(UINT i=0; i<data.getNumRows(); i++){
            for(UINT j=0; j<data.getNumCols(); j++){
                data[i][j] = classData[i][j];
            }
        }
        
        //Train the model for this class
        models[k].gamma = nullRejectionCoeff;
        if( !models[k].train( classLabel, data, weights ) ){
            errorLog << "train_(ClassificationData &trainingData) - Failed to train model for class: " << classLabel << std::endl;
                
            //Try and work out why the training failed
            if( models[k].N == 0 ){
                errorLog << "train_(ClassificationData &trainingData) - N == 0!" << std::endl;
                models.clear();
                return false;
            }
            for(UINT j=0; j<numInputDimensions; j++){
                if( models[k].sigma[j] == 0 ){
                    errorLog << "train_(ClassificationData &trainingData) - The standard deviation of column " << j+1 << " is zero! Check the training data" << std::endl;
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

bool ANBC::predict_(VectorFloat &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &inputVector) - ANBC Model Not Trained!" << std::endl;
        return false;
    }
    
    predictedClassLabel = 0;
    maxLikelihood = -10000;
    
    if( !trained ) return false;
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &inputVector) - The size of the input vector (" << inputVector.getSize() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, MIN_SCALE_VALUE, MAX_SCALE_VALUE);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    Float classLikelihoodsSum = 0;
    Float minDist = 0;
    for(UINT k=0; k<numClasses; k++){
        classDistances[k] = models[k].predict( inputVector );
        
        //At this point the class likelihoods and class distances are the same thing
        classLikelihoods[k] = classDistances[k];
        
        //If the distances are very far away then they could be -inf or nan so catch this so the sum still works
        if( grt_isinf(classLikelihoods[k]) || grt_isnan(classLikelihoods[k]) ){
            classLikelihoods[k] = 0;
        }else{
            classLikelihoods[k] = grt_exp( classLikelihoods[k] );
            classLikelihoodsSum += classLikelihoods[k];
            
            //The loglikelihood values are negative so we want the values closest to 0
            if( classDistances[k] > minDist || k==0 ){
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

bool ANBC::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file<<"GRT_ANBC_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    if( trained ){
        //Write each of the models
        for(UINT k=0; k<numClasses; k++){
            file << "*************_MODEL_*************\n";
            file << "Model_ID: " << k+1 << std::endl;
            file << "N: " << models[k].N << std::endl;
            file << "ClassLabel: " << models[k].classLabel << std::endl;
            file << "Threshold: " << models[k].threshold << std::endl;
            file << "Gamma: " << models[k].gamma << std::endl;
            file << "TrainingMu: " << models[k].trainingMu << std::endl;
            file << "TrainingSigma: " << models[k].trainingSigma << std::endl;
            
            file<<"Mu:";
            for(UINT j=0; j<models[k].N; j++){
                file << "\t" << models[k].mu[j];
            }file << std::endl;
            
            file<<"Sigma:";
            for(UINT j=0; j<models[k].N; j++){
                file << "\t" << models[k].sigma[j];
            }file << std::endl;
            
            file<<"Weights:";
            for(UINT j=0; j<models[k].N; j++){
                file << "\t" << models[k].weights[j];
            }file << std::endl;
        }
    }
    
    return true;
}

bool ANBC::load( std::fstream &file ){
    
    trained = false;
    numInputDimensions = 0;
    numClasses = 0;
    models.clear();
    classLabels.clear();
    
    if(!file.is_open())
    {
        errorLog << "load(string filename) - Could not open file to load model" << std::endl;
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
        errorLog << "load(string filename) - Could not find Model File Header" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
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
                errorLog << "load(string filename) - Could not find header for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            
            file >> word;
            if(word != "Model_ID:"){
                errorLog << "load(string filename) - Could not find model ID for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            file >> modelID;
            
            if(modelID-1!=k){
                errorLog << "ANBC: Model ID does not match the current class ID for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            
            file >> word;
            if(word != "N:"){
                errorLog << "ANBC: Could not find N for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            file >> models[k].N;
            
            file >> word;
            if(word != "ClassLabel:"){
                errorLog << "load(string filename) - Could not find ClassLabel for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            file >> models[k].classLabel;
            classLabels[k] = models[k].classLabel;
            
            file >> word;
            if(word != "Threshold:"){
                errorLog << "load(string filename) - Could not find the threshold for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            file >> models[k].threshold;
            
            file >> word;
            if(word != "Gamma:"){
                errorLog << "load(string filename) - Could not find the gamma parameter for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            file >> models[k].gamma;
            
            file >> word;
            if(word != "TrainingMu:"){
                errorLog << "load(string filename) - Could not find the training mu parameter for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            file >> models[k].trainingMu;
            
            file >> word;
            if(word != "TrainingSigma:"){
                errorLog << "load(string filename) - Could not find the training sigma parameter for the "<<k+1<<"th model" << std::endl;
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
                errorLog << "load(string filename) - Could not find the Mu vector for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            
            //Load Mu
            for(UINT j=0; j<models[k].N; j++){
                Float value;
                file >> value;
                models[k].mu[j] = value;
            }
            
            file >> word;
            if(word != "Sigma:"){
                errorLog << "load(string filename) - Could not find the Sigma vector for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            
            //Load Sigma
            for(UINT j=0; j<models[k].N; j++){
                Float value;
                file >> value;
                models[k].sigma[j] = value;
            }
            
            file >> word;
            if(word != "Weights:"){
                errorLog << "load(string filename) - Could not find the Weights vector for the "<<k+1<<"th model" << std::endl;
                    return false;
            }
            
            //Load Weights
            for(UINT j=0; j<models[k].N; j++){
                Float value;
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

VectorFloat ANBC::getNullRejectionThresholds() const{
    if( !trained ) return VectorFloat();
    return nullRejectionThresholds;
}

bool ANBC::setNullRejectionCoeff(Float nullRejectionCoeff){
    
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

bool ANBC::loadLegacyModelFromFile( std::fstream &file ){
    
    std::string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find NumFeatures " << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find NumClasses" << std::endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find UseScaling" << std::endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadANBCModelFromFile(string filename) - Could not find UseNullRejection" << std::endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numInputDimensions);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Ranges" << std::endl;
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
            errorLog << "loadANBCModelFromFile(string filename) - Could not find header for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        
        file >> word;
        if(word != "Model_ID:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find model ID for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        file >> modelID;
        
        if(modelID-1!=k){
            errorLog << "ANBC: Model ID does not match the current class ID for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        
        file >> word;
        if(word != "N:"){
            errorLog << "ANBC: Could not find N for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        file >> models[k].N;
        
        file >> word;
        if(word != "ClassLabel:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find ClassLabel for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        file >> models[k].classLabel;
        classLabels[k] = models[k].classLabel;
        
        file >> word;
        if(word != "Threshold:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the threshold for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        file >> models[k].threshold;
        
        file >> word;
        if(word != "Gamma:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the gamma parameter for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        file >> models[k].gamma;
        
        file >> word;
        if(word != "TrainingMu:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the training mu parameter for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        file >> models[k].trainingMu;
        
        file >> word;
        if(word != "TrainingSigma:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the training sigma parameter for the "<<k+1<<"th model" << std::endl;
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
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Mu vector for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        
        //Load Mu
        for(UINT j=0; j<models[k].N; j++){
            Float value;
            file >> value;
            models[k].mu[j] = value;
        }
        
        file >> word;
        if(word != "Sigma:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Sigma vector for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        
        //Load Sigma
        for(UINT j=0; j<models[k].N; j++){
            Float value;
            file >> value;
            models[k].sigma[j] = value;
        }
        
        file >> word;
        if(word != "Weights:"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the Weights vector for the "<<k+1<<"th model" << std::endl;
                return false;
        }
        
        //Load Weights
        for(UINT j=0; j<models[k].N; j++){
            Float value;
            file >> value;
            models[k].weights[j] = value;
        }
        
        file >> word;
        if(word != "*********************************"){
            errorLog << "loadANBCModelFromFile(string filename) - Could not find the model footer for the "<<k+1<<"th model" << std::endl;
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

GRT_END_NAMESPACE
