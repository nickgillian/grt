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

#include "MLBase.h"

namespace GRT{

MLBase::MLBase(void){
    trained = false;
    useScaling = false;
    baseType = BASE_TYPE_NOT_SET;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    minNumEpochs = 0;
    maxNumEpochs = 100;
    validationSetSize = 20;
    minChange = 1.0e-5;
    learningRate = 0.1;
    useValidationSet = false;
    randomiseTrainingOrder = true;
    rootMeanSquaredTrainingError = 0;
    totalSquaredTrainingError = 0;
}

MLBase::~MLBase(void){
    clear();
}

bool MLBase::copyMLBaseVariables(const MLBase *mlBase){

    if( mlBase == NULL ){
        errorLog << "copyMLBaseVariables(MLBase *mlBase) - mlBase pointer is NULL!" << endl;
        return false;
    }
    
    if( !copyGRTBaseVariables( mlBase ) ){
        errorLog << "copyMLBaseVariables(MLBase *mlBase) - Failed to copy GRT Base variables!" << endl;
        return false;
    }

    this->trained = mlBase->trained;
    this->useScaling = mlBase->useScaling;
    this->baseType = mlBase->baseType;
    this->numInputDimensions = mlBase->numInputDimensions;
    this->numOutputDimensions = mlBase->numOutputDimensions;
    this->minNumEpochs = mlBase->minNumEpochs;
    this->maxNumEpochs = mlBase->maxNumEpochs;
    this->validationSetSize = mlBase->validationSetSize;
    this->minChange = mlBase->minChange;
    this->learningRate = mlBase->learningRate;
    this->rootMeanSquaredTrainingError = mlBase->rootMeanSquaredTrainingError;
    this->totalSquaredTrainingError = mlBase->totalSquaredTrainingError;
    this->useValidationSet = mlBase->useValidationSet;
    this->randomiseTrainingOrder = mlBase->randomiseTrainingOrder;
    this->numTrainingIterationsToConverge = mlBase->numTrainingIterationsToConverge;
    this->trainingResults = mlBase->trainingResults;
    this->trainingResultsObserverManager = mlBase->trainingResultsObserverManager;
    this->testResultsObserverManager = mlBase->testResultsObserverManager;

    return true;
}

bool MLBase::train(ClassificationData trainingData){ return train_( trainingData ); }

bool MLBase::train_(ClassificationData &trainingData){ return false; }
    
bool MLBase::train(RegressionData trainingData){ return train_( trainingData ); }
    
bool MLBase::train_(RegressionData &trainingData){ return false; }

bool MLBase::train(TimeSeriesClassificationData trainingData){ return train_( trainingData ); }

bool MLBase::train_(TimeSeriesClassificationData &trainingData){ return false; }

bool MLBase::train(UnlabelledData trainingData){ return train_( trainingData ); }

bool MLBase::train_(UnlabelledData &trainingData){ return false; }

bool MLBase::train(MatrixDouble data){ return train_( data ); }

bool MLBase::train_(MatrixDouble &data){ return false; }

bool MLBase::predict(VectorDouble inputVector){ return predict_( inputVector ); }
    
bool MLBase::predict_(VectorDouble &inputVector){ return false; }

bool MLBase::predict(MatrixDouble inputMatrix){ return predict_( inputMatrix ); }
    
bool MLBase::predict_(MatrixDouble &inputMatrix){ return false; }

bool MLBase::MLBase::map(VectorDouble inputVector){ return map_( inputVector ); }

bool MLBase::map_(VectorDouble &inputVector){ return false; }

bool MLBase::reset(){ return true; }

bool MLBase::clear(){
    trained = false;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    numTrainingIterationsToConverge = 0;
    rootMeanSquaredTrainingError = 0;
    totalSquaredTrainingError = 0;
    trainingResults.clear();
    return true;
}

bool MLBase::print() const { return true; }

bool MLBase::saveModelToFile(const string filename) const{
    
    if( !trained ) return false;
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool MLBase::saveModelToFile(fstream &file) const { return false; }

bool MLBase::loadModelFromFile(const string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool MLBase::loadModelFromFile(fstream &file){ return false; }

UINT MLBase::getBaseType() const{ return baseType; }

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
    
double MLBase::getLearningRate() const{
    return learningRate;
}

double MLBase::getRootMeanSquaredTrainingError() const{
    return rootMeanSquaredTrainingError;
}

double MLBase::getTotalSquaredTrainingError() const{
    return totalSquaredTrainingError;
}

bool MLBase::getTrained() const{ return trained; }

bool MLBase::getModelTrained() const{ return getTrained(); }

bool MLBase::getScalingEnabled() const{ return useScaling; }

bool MLBase::getIsBaseTypeClassifier() const{ return baseType==CLASSIFIER; }

bool MLBase::getIsBaseTypeRegressifier() const{ return baseType==REGRESSIFIER; }
    
bool MLBase::getIsBaseTypeClusterer() const{ return baseType==CLUSTERER; }

bool MLBase::enableScaling(bool useScaling){ this->useScaling = useScaling; return true; }
    
bool MLBase::setMaxNumEpochs(const UINT maxNumEpochs){
    if( maxNumEpochs == 0 ){
        warningLog << "setMaxNumEpochs(const UINT maxNumEpochs) - The maxNumEpochs must be greater than 0!" << endl;
        return false;
    }
    this->maxNumEpochs = maxNumEpochs;
    return true;
}

bool MLBase::setMinNumEpochs(const UINT minNumEpochs){
    this->minNumEpochs = minNumEpochs;
    return true;
}

bool MLBase::setMinChange(const double minChange){
    if( minChange < 0 ){
        warningLog << "setMinChange(const double minChange) - The minChange must be greater than or equal to 0!" << endl;
        return false;
    }
    this->minChange = minChange;
    return true;
}
    
bool MLBase::setLearningRate(double learningRate){
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
    
    warningLog << "setValidationSetSize(const UINT validationSetSize) - The validation size must be in the range [1 99]!" << endl;
    
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

vector< TrainingResult > MLBase::getTrainingResults() const{
    return trainingResults;
}

bool MLBase::saveBaseSettingsToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveBaseSettingsToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "Trained: " << trained << endl;
    file << "UseScaling: " << useScaling << endl;
    file << "NumInputDimensions: " << numInputDimensions << endl;
    file << "NumOutputDimensions: " << numOutputDimensions << endl;
    file << "NumTrainingIterationsToConverge: " << numTrainingIterationsToConverge << endl;
    file << "MinNumEpochs: " << minNumEpochs << endl;
    file << "MaxNumEpochs: " << maxNumEpochs << endl;
    file << "ValidationSetSize: " << validationSetSize << endl;
    file << "LearningRate: " << learningRate << endl;
    file << "MinChange: " << minChange << endl;
    file << "UseValidationSet: " << useValidationSet << endl;
    file << "RandomiseTrainingOrder: " << randomiseTrainingOrder << endl;
    
    return true;
}

bool MLBase::loadBaseSettingsFromFile(fstream &file){
    
    //Clear any previous setup
    clear();
    
    if( !file.is_open() ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the trained state
    file >> word;
    if( word != "Trained:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read Trained header!" << endl;
        return false;
    }
    file >> trained;
    
    //Load the scaling state
    file >> word;
    if( word != "UseScaling:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read UseScaling header!" << endl;
        return false;
    }
    file >> useScaling;
    
    //Load the NumInputDimensions
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NumInputDimensions header!" << endl;
        return false;
    }
    file >> numInputDimensions;
    
    //Load the NumOutputDimensions
    file >> word;
    if( word != "NumOutputDimensions:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NumOutputDimensions header!" << endl;
        return false;
    }
    file >> numOutputDimensions;
    
    //Load the numTrainingIterationsToConverge
    file >> word;
    if( word != "NumTrainingIterationsToConverge:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NumTrainingIterationsToConverge header!" << endl;
        return false;
    }
    file >> numTrainingIterationsToConverge;
    
    //Load the MinNumEpochs
    file >> word;
    if( word != "MinNumEpochs:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read MinNumEpochs header!" << endl;
        return false;
    }
    file >> minNumEpochs;
    
    //Load the maxNumEpochs
    file >> word;
    if( word != "MaxNumEpochs:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read MaxNumEpochs header!" << endl;
        return false;
    }
    file >> maxNumEpochs;
    
    //Load the ValidationSetSize
    file >> word;
    if( word != "ValidationSetSize:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read ValidationSetSize header!" << endl;
        return false;
    }
    file >> validationSetSize;
    
    //Load the LearningRate
    file >> word;
    if( word != "LearningRate:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read LearningRate header!" << endl;
        return false;
    }
    file >> learningRate;
    
    //Load the MinChange
    file >> word;
    if( word != "MinChange:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read MinChange header!" << endl;
        return false;
    }
    file >> minChange;
    
    //Load the UseValidationSet
    file >> word;
    if( word != "UseValidationSet:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read UseValidationSet header!" << endl;
        return false;
    }
    file >> useValidationSet;
    
    //Load the RandomiseTrainingOrder
    file >> word;
    if( word != "RandomiseTrainingOrder:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read RandomiseTrainingOrder header!" << endl;
        return false;
    }
    file >> randomiseTrainingOrder;
    
    return true;
}

} //End of namespace GRT
