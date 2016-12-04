/**
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
#include "MLBase.h"

GRT_BEGIN_NAMESPACE

MLBase::MLBase( const std::string &id, const BaseType type ) : GRTBase( id ){
    baseType = type;
    trained = false;
    useScaling = false;
    inputType = DATA_TYPE_UNKNOWN;
    outputType = DATA_TYPE_UNKNOWN;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    minNumEpochs = 0;
    maxNumEpochs = 100;
    validationSetSize = 20;
    validationSetAccuracy = 0;
    minChange = 1.0e-5;
    learningRate = 0.1;
    useValidationSet = false;
    randomiseTrainingOrder = true;
    rmsTrainingError = 0;
    rmsValidationError = 0;
    totalSquaredTrainingError = 0;
    infoLog.setKey("[" + id + "]");
    debugLog.setKey("[DEBUG " + id + "]");
    errorLog.setKey("[ERROR " + id + "]");
    warningLog.setKey("[WARNING " + id + "]");
    trainingLog.setKey("[TRAINING " + id + "]");
    testingLog.setKey("[TESTING " + id + "]");

    if( id == "" ){
        trainingLog.setKey("[TRAINING " + id + "]");
        testingLog.setKey("[TESTING " + id + "]");
    }else{
        trainingLog.setKey("[TRAINING]");
        testingLog.setKey("[TESTING]");
    }
}

MLBase::~MLBase(void){
    clear();
}

bool MLBase::copyMLBaseVariables(const MLBase *mlBase){
    
    if( mlBase == NULL ){
        errorLog << "copyMLBaseVariables(MLBase *mlBase) - mlBase pointer is NULL!" << std::endl;
        return false;
    }
    
    if( !copyGRTBaseVariables( mlBase ) ){
        errorLog << "copyMLBaseVariables(MLBase *mlBase) - Failed to copy GRT Base variables!" << std::endl;
        return false;
    }
    
    this->trained = mlBase->trained;
    this->useScaling = mlBase->useScaling;
    this->baseType = mlBase->baseType;
    this->inputType = mlBase->inputType;
    this->outputType = mlBase->outputType;
    this->numInputDimensions = mlBase->numInputDimensions;
    this->numOutputDimensions = mlBase->numOutputDimensions;
    this->minNumEpochs = mlBase->minNumEpochs;
    this->maxNumEpochs = mlBase->maxNumEpochs;
    this->validationSetSize = mlBase->validationSetSize;
    this->validationSetAccuracy = mlBase->validationSetAccuracy;
    this->validationSetPrecision = mlBase->validationSetPrecision;
    this->validationSetRecall = mlBase->validationSetRecall;
    this->minChange = mlBase->minChange;
    this->learningRate = mlBase->learningRate;
    this->rmsTrainingError = mlBase->rmsTrainingError;
    this->rmsValidationError = mlBase->rmsValidationError;
    this->totalSquaredTrainingError = mlBase->totalSquaredTrainingError;
    this->useValidationSet = mlBase->useValidationSet;
    this->randomiseTrainingOrder = mlBase->randomiseTrainingOrder;
    this->numTrainingIterationsToConverge = mlBase->numTrainingIterationsToConverge;
    this->trainingResults = mlBase->trainingResults;
    this->trainingResultsObserverManager = mlBase->trainingResultsObserverManager;
    this->testResultsObserverManager = mlBase->testResultsObserverManager;
    this->trainingLog = mlBase->trainingLog;
    this->testingLog = mlBase->testingLog;
    
    return true;
}

bool MLBase::train(ClassificationData trainingData){ return train_( trainingData ); }

bool MLBase::train_(ClassificationData &trainingData){ return false; }

bool MLBase::train(RegressionData trainingData){ return train_( trainingData ); }

bool MLBase::train_(RegressionData &trainingData){ return false; }

bool MLBase::train(RegressionData trainingData,RegressionData validationData){ return train_( trainingData, validationData ); }

bool MLBase::train_(RegressionData &trainingData,RegressionData &validationData){ return false; }

bool MLBase::train(TimeSeriesClassificationData trainingData){ return train_( trainingData ); }

bool MLBase::train_(TimeSeriesClassificationData &trainingData){ return false; }

bool MLBase::train(ClassificationDataStream trainingData){ return train_( trainingData ); }

bool MLBase::train_(ClassificationDataStream &trainingData){ return false; }

bool MLBase::train(UnlabelledData trainingData){ return train_( trainingData ); }

bool MLBase::train_(UnlabelledData &trainingData){ return false; }

bool MLBase::train(MatrixFloat data){ return train_( data ); }

bool MLBase::train_(MatrixFloat &data){ return false; }

bool MLBase::predict(VectorFloat inputVector){ return predict_( inputVector ); }

bool MLBase::predict_(VectorFloat &inputVector){ return false; }

bool MLBase::predict(MatrixFloat inputMatrix){ return predict_( inputMatrix ); }

bool MLBase::predict_(MatrixFloat &inputMatrix){ return false; }

bool MLBase::map(VectorFloat inputVector){ return map_( inputVector ); }

bool MLBase::map_(VectorFloat &inputVector){ return false; }

bool MLBase::reset(){ return true; }

bool MLBase::clear(){
    trained = false;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    numTrainingIterationsToConverge = 0;
    rmsTrainingError = 0;
    rmsValidationError = 0;
    totalSquaredTrainingError = 0;
    trainingResults.clear();
    validationSetPrecision.clear();
    validationSetRecall.clear();
    validationSetAccuracy = 0;
    return true;
}

bool MLBase::print() const { std::cout << getModelAsString(); return true; }

bool MLBase::save(const std::string &filename) const {
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !save( file ) )
    {
        return false;
    }
    
    file.close();
    
    return true;
}

bool MLBase::save(std::fstream &file) const {
    return false; //The base class returns false, as this should be overwritten by the inheriting class
}

bool MLBase::saveModelToFile(const std::string &filename) const { return save( filename ); }

bool MLBase::saveModelToFile(std::fstream &file) const { return save( file ); }

bool MLBase::load(const std::string &filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !load( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool MLBase::load(std::fstream &file) {
    return false; //The base class returns false, as this should be overwritten by the inheriting class
}

bool MLBase::loadModelFromFile(const std::string &filename){ return load( filename ); }

bool MLBase::loadModelFromFile(std::fstream &file){ return load( file ); }

bool MLBase::getModel(std::ostream &stream) const { return true; }

std::string MLBase::getModelAsString() const{
    std::stringstream stream;
    if( getModel( stream ) ){
        return stream.str();
    }
    return "";
}

DataType MLBase::getInputType() const { return inputType; }

DataType MLBase::getOutputType() const { return outputType; }

MLBase::BaseType MLBase::getType() const{ return baseType; }

UINT MLBase::getNumInputFeatures() const{ return getNumInputDimensions(); }

UINT MLBase::getNumInputDimensions() const{ return numInputDimensions; }

UINT MLBase::getNumOutputDimensions() const{ return numOutputDimensions; }

UINT MLBase::getNumTrainingIterationsToConverge() const{
    if( trained ){
        return numTrainingIterationsToConverge;
    }
    return 0;
}

UINT MLBase::getMinNumEpochs() const{
    return minNumEpochs;
}

UINT MLBase::getMaxNumEpochs() const{
    return maxNumEpochs;
}

UINT MLBase::getValidationSetSize() const{
    return validationSetSize;
}

Float MLBase::getLearningRate() const{
    return learningRate;
}

Float MLBase::getRMSTrainingError() const{
    return rmsTrainingError;
}

Float MLBase::getRootMeanSquaredTrainingError() const{
    return getRMSTrainingError();
}

Float MLBase::getTotalSquaredTrainingError() const{
    return totalSquaredTrainingError;
}

Float MLBase::getRMSValidationError() const{
    return rmsValidationError;
}

Float MLBase::getValidationSetAccuracy() const {
    return validationSetAccuracy;
}

VectorFloat MLBase::getValidationSetPrecision() const {
    return validationSetPrecision;
}

VectorFloat MLBase::getValidationSetRecall() const {
    return validationSetRecall;
}

bool MLBase::getTrained() const{ return trained; }

bool MLBase::getModelTrained() const{ return getTrained(); }

bool MLBase::getScalingEnabled() const{ return useScaling; }

bool MLBase::getIsBaseTypeClassifier() const{ return baseType==CLASSIFIER; }

bool MLBase::getIsBaseTypeRegressifier() const{ return baseType==REGRESSIFIER; }

bool MLBase::getIsBaseTypeClusterer() const{ return baseType==CLUSTERER; }

bool MLBase::enableScaling(bool useScaling){ this->useScaling = useScaling; return true; }

bool MLBase::getUseValidationSet() const { return useValidationSet; }

bool MLBase::getTrainingLoggingEnabled() const {
    return trainingLog.getInstanceLoggingEnabled();
}

bool MLBase::getTestingLoggingEnabled() const {
    return testingLog.getInstanceLoggingEnabled();
}

bool MLBase::setMaxNumEpochs(const UINT maxNumEpochs){
    if( maxNumEpochs == 0 ){
        warningLog << "setMaxNumEpochs(const UINT maxNumEpochs) - The maxNumEpochs must be greater than 0!" << std::endl;
        return false;
    }
    this->maxNumEpochs = maxNumEpochs;
    return true;
}

bool MLBase::setMinNumEpochs(const UINT minNumEpochs){
    this->minNumEpochs = minNumEpochs;
    return true;
}

bool MLBase::setMinChange(const Float minChange){
    if( minChange < 0 ){
        warningLog << "setMinChange(const Float minChange) - The minChange must be greater than or equal to 0!" << std::endl;
        return false;
    }
    this->minChange = minChange;
    return true;
}

bool MLBase::setLearningRate(const Float learningRate){
    if( learningRate > 0 ){
        this->learningRate = learningRate;
        return true;
    }
    return false;
}

bool MLBase::setValidationSetSize(const UINT validationSetSize){
    
    if( validationSetSize > 0 && validationSetSize < 100 ){
        this->validationSetSize = validationSetSize;
        return true;
    }
    
    warningLog << "setValidationSetSize(const UINT validationSetSize) - The validation size must be in the range [1 99]!" << std::endl;
    
    return false;
}

bool MLBase::setUseValidationSet(const bool useValidationSet){
    this->useValidationSet = useValidationSet;
    return true;
}

bool MLBase::setRandomiseTrainingOrder(const bool randomiseTrainingOrder){
    this->randomiseTrainingOrder = randomiseTrainingOrder;
    return true;
}

bool MLBase::setTrainingLoggingEnabled(const bool loggingEnabled){
    return this->trainingLog.setInstanceLoggingEnabled( loggingEnabled );
}

bool MLBase::setTestingLoggingEnabled(const bool loggingEnabled){
    return this->testingLog.setInstanceLoggingEnabled( loggingEnabled );
}

bool MLBase::registerTrainingResultsObserver( Observer< TrainingResult > &observer ){
    return trainingResultsObserverManager.registerObserver( observer );
}

bool MLBase::registerTestResultsObserver( Observer< TestInstanceResult > &observer ){
    return testResultsObserverManager.registerObserver( observer );
}

bool MLBase::removeTrainingResultsObserver( const Observer< TrainingResult > &observer ){
    return trainingResultsObserverManager.removeObserver( observer );
}

bool MLBase::removeTestResultsObserver( const Observer< TestInstanceResult > &observer ){
    return testResultsObserverManager.removeObserver( observer );
}

bool MLBase::removeAllTrainingObservers(){
    return trainingResultsObserverManager.removeAllObservers();
}

bool MLBase::removeAllTestObservers(){
    return testResultsObserverManager.removeAllObservers();
}

bool MLBase::notifyTrainingResultsObservers( const TrainingResult &data ){
    return trainingResultsObserverManager.notifyObservers( data );
}

bool MLBase::notifyTestResultsObservers( const TestInstanceResult &data ){
    return testResultsObserverManager.notifyObservers( data );
}

MLBase* MLBase::getMLBasePointer(){
    return this;
}

const MLBase* MLBase::getMLBasePointer() const{
    return this;
}

Vector< TrainingResult > MLBase::getTrainingResults() const{
    return trainingResults;
}

bool MLBase::saveBaseSettingsToFile( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "saveBaseSettingsToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    file << "Trained: " << trained << std::endl;
    file << "UseScaling: " << useScaling << std::endl;
    file << "NumInputDimensions: " << numInputDimensions << std::endl;
    file << "NumOutputDimensions: " << numOutputDimensions << std::endl;
    file << "NumTrainingIterationsToConverge: " << numTrainingIterationsToConverge << std::endl;
    file << "MinNumEpochs: " << minNumEpochs << std::endl;
    file << "MaxNumEpochs: " << maxNumEpochs << std::endl;
    file << "ValidationSetSize: " << validationSetSize << std::endl;
    file << "LearningRate: " << learningRate << std::endl;
    file << "MinChange: " << minChange << std::endl;
    file << "UseValidationSet: " << useValidationSet << std::endl;
    file << "RandomiseTrainingOrder: " << randomiseTrainingOrder << std::endl;
    
    return true;
}

bool MLBase::loadBaseSettingsFromFile( std::fstream &file ){
    
    //Clear any previous setup
    clear();
    
    if( !file.is_open() ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the trained state
    file >> word;
    if( word != "Trained:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read Trained header!" << std::endl;
        return false;
    }
    file >> trained;
    
    //Load the scaling state
    file >> word;
    if( word != "UseScaling:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read UseScaling header!" << std::endl;
        return false;
    }
    file >> useScaling;
    
    //Load the NumInputDimensions
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NumInputDimensions header!" << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    //Load the NumOutputDimensions
    file >> word;
    if( word != "NumOutputDimensions:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NumOutputDimensions header!" << std::endl;
        return false;
    }
    file >> numOutputDimensions;
    
    //Load the numTrainingIterationsToConverge
    file >> word;
    if( word != "NumTrainingIterationsToConverge:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NumTrainingIterationsToConverge header!" << std::endl;
        return false;
    }
    file >> numTrainingIterationsToConverge;
    
    //Load the MinNumEpochs
    file >> word;
    if( word != "MinNumEpochs:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read MinNumEpochs header!" << std::endl;
        return false;
    }
    file >> minNumEpochs;
    
    //Load the maxNumEpochs
    file >> word;
    if( word != "MaxNumEpochs:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read MaxNumEpochs header!" << std::endl;
        return false;
    }
    file >> maxNumEpochs;
    
    //Load the ValidationSetSize
    file >> word;
    if( word != "ValidationSetSize:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read ValidationSetSize header!" << std::endl;
        return false;
    }
    file >> validationSetSize;
    
    //Load the LearningRate
    file >> word;
    if( word != "LearningRate:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read LearningRate header!" << std::endl;
        return false;
    }
    file >> learningRate;
    
    //Load the MinChange
    file >> word;
    if( word != "MinChange:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read MinChange header!" << std::endl;
        return false;
    }
    file >> minChange;
    
    //Load the UseValidationSet
    file >> word;
    if( word != "UseValidationSet:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read UseValidationSet header!" << std::endl;
        return false;
    }
    file >> useValidationSet;
    
    //Load the RandomiseTrainingOrder
    file >> word;
    if( word != "RandomiseTrainingOrder:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read RandomiseTrainingOrder header!" << std::endl;
        return false;
    }
    file >> randomiseTrainingOrder;
    
    return true;
}

GRT_END_NAMESPACE
