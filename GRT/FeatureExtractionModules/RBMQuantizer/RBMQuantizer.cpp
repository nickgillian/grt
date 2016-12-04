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
#include "RBMQuantizer.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string RBMQuantizer::id = "RBMQuantizer";
std::string RBMQuantizer::getId() { return RBMQuantizer::id; }

//Register your module with the FeatureExtraction base class
RegisterFeatureExtractionModule< RBMQuantizer > RBMQuantizer::registerModule( RBMQuantizer::getId() );

RBMQuantizer::RBMQuantizer(const UINT numClusters) : FeatureExtraction( RBMQuantizer::getId() )
{
    this->numClusters = numClusters;
}

RBMQuantizer::RBMQuantizer(const RBMQuantizer &rhs) : FeatureExtraction( RBMQuantizer::getId() )
{    
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

RBMQuantizer::~RBMQuantizer(){
}

RBMQuantizer& RBMQuantizer::operator=(const RBMQuantizer &rhs){
    if(this!=&rhs){
        this->numClusters = rhs.numClusters;
        this->rbm = rhs.rbm;
        this->quantizationDistances = rhs.quantizationDistances;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool RBMQuantizer::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //invoke the equals operator
        *this = *dynamic_cast<const RBMQuantizer*>(featureExtraction);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool RBMQuantizer::computeFeatures(const VectorFloat &inputVector){
    
    //Run the quantize algorithm
    quantize( inputVector );
    
    return true;
}

bool RBMQuantizer::reset(){
    
    //Reset the base class
    FeatureExtraction::reset();
    
    if( trained ){
        rbm.reset();
        std::fill(quantizationDistances.begin(),quantizationDistances.end(),0);
    }
    
    return true;
}

bool RBMQuantizer::clear(){
    
    //Clear the base class
    FeatureExtraction::clear();
    
    rbm.clear();
    quantizationDistances.clear();
    
    return true;
}

bool RBMQuantizer::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header
    file << "RBM_QUANTIZER_FILE_V1.0" << std::endl;
    
    //Save the base feature extraction settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    file << "QuantizerTrained: " << trained << std::endl;
    file << "NumClusters: " << numClusters << std::endl;
    
    if( trained ){
        if( !rbm.save( file ) ){
            errorLog << "save(fstream &file) - Failed to save RBM settings to file!" << std::endl;
            return false;
        }
    }
    
    return true;
}

bool RBMQuantizer::load( std::fstream &file ){
    
    //Clear any previous model
    clear();
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //First, you should read and validate the header
    file >> word;
    if( word != "RBM_QUANTIZER_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    //Second, you should load the base feature extraction settings to the file
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if( word != "QuantizerTrained:" ){
        errorLog << "load(fstream &file) - Failed to load QuantizerTrained!" << std::endl;
        return false;
    }
    file >> trained;
    
    file >> word;
    if( word != "NumClusters:" ){
        errorLog << "load(fstream &file) - Failed to load NumClusters!" << std::endl;
        return false;
    }
    file >> numClusters;
    
    if( trained ){
        if( !rbm.load( file ) ){
            errorLog << "load(fstream &file) - Failed to load SelfOrganizingMap settings from file!" << std::endl;
            return false;
        }
        initialized = true;
        featureDataReady = false;
        quantizationDistances.resize(numClusters,0);
    }
    
    return true;
}

bool RBMQuantizer::train_(ClassificationData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}

bool RBMQuantizer::train_(TimeSeriesClassificationData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}

bool RBMQuantizer::train_(ClassificationDataStream &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}

bool RBMQuantizer::train_(UnlabelledData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}

bool RBMQuantizer::train_(MatrixFloat &trainingData){
    
    //Clear any previous model
    clear();
    
    if( trainingData.getNumRows() == 0 ){
        errorLog << "train_(MatrixFloat &trainingData) - Failed to train quantizer, the training data is empty!" << std::endl;
        return false;
    }
    
    //Train the RBM model
    rbm.setNumHiddenUnits( numClusters );
    rbm.setLearningRate( learningRate );
    rbm.setMinNumEpochs( minNumEpochs );
    rbm.setMaxNumEpochs( maxNumEpochs );
    rbm.setMinChange( minChange );
    
    if( !rbm.train_( trainingData ) ){
        errorLog << "train_(MatrixFloat &trainingData) - Failed to train quantizer!" << std::endl;
        return false;
    }
    
    //Flag that the feature vector is now initalized
    initialized = true;
    trained = true;
    numInputDimensions = trainingData.getNumCols();
    numOutputDimensions = 1; //This is always 1 for the quantizer
    featureVector.resize(numOutputDimensions,0);
    quantizationDistances.resize(numClusters,0);
    
    return true;
}

UINT RBMQuantizer::quantize(const Float inputValue){
    return quantize( VectorFloat(1,inputValue) );
}

UINT RBMQuantizer::quantize(const VectorFloat &inputVector){
    
    if( !trained ){
        errorLog << "quantize(const VectorFloat &inputVector) - The quantizer model has not been trained!" << std::endl;
        return 0;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "quantize(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.getSize() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return 0;
    }
    
    if( !rbm.predict( inputVector ) ){
        errorLog << "quantize(const VectorFloat &inputVector) - Failed to quantize input!" << std::endl;
        return 0;
    }
    
    quantizationDistances = rbm.getOutputData();
    
    //Search for the neuron with the maximum output
    UINT quantizedValue = 0;
    Float maxValue = 0;
    for(UINT k=0; k<numClusters; k++){
        if( quantizationDistances[k] > maxValue ){
            maxValue = quantizationDistances[k];
            quantizedValue = k;
        }
    }
    
    featureVector[0] = quantizedValue;
    featureDataReady = true;
    
    return quantizedValue;
}

bool RBMQuantizer::getQuantizerTrained() const {
    return trained;
}

UINT RBMQuantizer::getNumClusters() const{
    return numClusters;
}

UINT RBMQuantizer::getQuantizedValue() const {
    return (trained ? static_cast<UINT>(featureVector[0]) : 0);
}

VectorFloat RBMQuantizer::getQuantizationDistances() const{
    return quantizationDistances;
}

BernoulliRBM RBMQuantizer::getBernoulliRBM() const{
    return rbm;
}

bool RBMQuantizer::setNumClusters(const UINT numClusters){
    clear();
    this->numClusters = numClusters;
    return true;
}

GRT_END_NAMESPACE
