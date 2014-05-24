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

#include "SOMQuantizer.h"

namespace GRT{
    
//Register your module with the FeatureExtraction base class
RegisterFeatureExtractionModule< SOMQuantizer > SOMQuantizer::registerModule("SOMQuantizer");
    
SOMQuantizer::SOMQuantizer(const UINT numClusters){
    
    this->numClusters = numClusters;
    
    classType = "SOMQuantizer";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG SOMQuantizer]");
    errorLog.setProceedingText("[ERROR SOMQuantizer]");
    warningLog.setProceedingText("[WARNING SOMQuantizer]");
}
    
SOMQuantizer::SOMQuantizer(const SOMQuantizer &rhs){
    
    classType = "SOMQuantizer";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG SOMQuantizer]");
    errorLog.setProceedingText("[ERROR SOMQuantizer]");
    warningLog.setProceedingText("[WARNING SOMQuantizer]");
    
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

SOMQuantizer::~SOMQuantizer(){
    //Here you should add any specific code to cleanup your custom feature extraction module if needed
}
    
SOMQuantizer& SOMQuantizer::operator=(const SOMQuantizer &rhs){
    if(this!=&rhs){
        //Here you should copy any class variables from the rhs instance to this instance
        this->numClusters = rhs.numClusters;
        this->som = rhs.som;
        this->quantizationDistances = rhs.quantizationDistances;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}
    
bool SOMQuantizer::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getFeatureExtractionType() == featureExtraction->getFeatureExtractionType() ){
        
        //Cast the feature extraction pointer to a pointer to your custom feature extraction module
        //Then invoke the equals operator
        *this = *(SOMQuantizer*)featureExtraction;
        
        return true;
    }
    
    errorLog << "clone(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << endl;
    
    return false;
}
    
bool SOMQuantizer::computeFeatures(const VectorDouble &inputVector){
    
	//Run the quantize algorithm
	quantize( inputVector );
	
    return true;
}

bool SOMQuantizer::reset(){
    
    //Reset the base class
    FeatureExtraction::reset();
    
    if( trained ){
        som.reset();
        std::fill(quantizationDistances.begin(),quantizationDistances.end(),0);
    }
    
    return true;
}
    
bool SOMQuantizer::clear(){
    
    //Clear the base class
    FeatureExtraction::clear();
    
    som.clear();
    quantizationDistances.clear();
    
    return true;
}
    
bool SOMQuantizer::saveModelToFile(const string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool SOMQuantizer::loadModelFromFile(const string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool SOMQuantizer::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //First, you should add a header (with no spaces) e.g.
    file << "SOM_QUANTIZER_FILE_V1.0" << endl;
	
    //Second, you should save the base feature extraction settings to the file
    if( !saveBaseSettingsToFile( file ) ){
        errorLog << "saveModelToFile(fstream &file) - Failed to save base feature extraction settings to file!" << endl;
        return false;
    }
    
    file << "QuantizerTrained: " << trained << endl;
    file << "NumClusters: " << numClusters << endl;
    
    if( trained ){
        file << "SOM: \n";
        if( !som.saveModelToFile( file ) ){
            errorLog << "saveModelToFile(fstream &file) - Failed to save SelfOrganizingMap settings to file!" << endl;
            return false;
        }
    }
    
    return true;
}

bool SOMQuantizer::loadModelFromFile(fstream &file){
    
    //Clear any previous model
    clear();
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //First, you should read and validate the header
    file >> word;
    if( word != "SOM_QUANTIZER_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;
    }
    
    //Second, you should load the base feature extraction settings to the file
    if( !loadBaseSettingsFromFile( file ) ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if( word != "QuantizerTrained:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load QuantizerTrained!" << endl;
        return false;
    }
    file >> trained;
    
    file >> word;
    if( word != "NumClusters:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load NumClusters!" << endl;
        return false;
    }
    file >> numClusters;
    
    if( trained ){
        file >> word;
        if( word != "SOM:" ){
            errorLog << "loadModelFromFile(fstream &file) - Failed to load SOM!" << endl;
            return false;
        }
        
        if( !som.loadModelFromFile( file ) ){
            errorLog << "loadModelFromFile(fstream &file) - Failed to load SelfOrganizingMap settings from file!" << endl;
            return false;
        }
        
        initialized = true;
        featureDataReady = false;
        quantizationDistances.resize(numClusters,0);
    }
    
    return true;
}
    
bool SOMQuantizer::train_(ClassificationData &trainingData){
    MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train_( data );
}
    
bool SOMQuantizer::train_(TimeSeriesClassificationData &trainingData){
    MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train_( data );
}
   
bool SOMQuantizer::train_(TimeSeriesClassificationDataStream &trainingData){
    MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train_( data );
}

bool SOMQuantizer::train_(UnlabelledData &trainingData){
	MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train_( data );
}
    
bool SOMQuantizer::train_(MatrixDouble &trainingData){
    
    //Clear any previous model
    clear();
    
    if( trainingData.getNumRows() == 0 ){
        errorLog << "train_(MatrixDouble &trainingData) - Failed to train quantizer, the training data is empty!" << endl;
        return false;
    }
    
    //Train the SOM model
    som.setNetworkSize( numClusters );
    som.setNetworkTypology( SelfOrganizingMap::RANDOM_NETWORK );
    som.setAlphaStart( 0.5 );
    som.setAlphaEnd(  0.1 );
    som.setMaxNumEpochs( 1000 );
    
    if( !som.train_( trainingData ) ){
        errorLog << "train(MatrixDouble &trainingData) - Failed to train quantizer!" << endl;
        return false;
    }
    
    //Flag that the feature extraction module is now initialized
    initialized = true;
    trained = true;
    numInputDimensions = trainingData.getNumCols();
    numOutputDimensions = 1; //This is always 1 for the SOMQuantizer
    featureVector.resize(numOutputDimensions,0);
    quantizationDistances.resize(numClusters,0);
    
    return true;
}

UINT SOMQuantizer::quantize(const double inputValue){
	return quantize( VectorDouble(1,inputValue) );
}

UINT SOMQuantizer::quantize(const VectorDouble &inputVector){
	
    if( !trained ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - The quantizer model has not been trained!" << endl;
        return 0;
    }

    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return 0;
    }
	
    //Pass the input data through the map
    if( !som.predict( inputVector ) ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - Failed to perform map!" << endl;
        return 0;
    }
    quantizationDistances = som.getMappedData();
    
    //Search for the neuron with the maximum output
    UINT quantizedValue = 0;
    double maxValue = 0;
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
    
bool SOMQuantizer::getQuantizerTrained() const {
    return trained;
}
    
UINT SOMQuantizer::getNumClusters() const{
    return numClusters;
}

UINT SOMQuantizer::getQuantizedValue() const {
    return (trained ? featureVector[0] : 0);
}

VectorDouble SOMQuantizer::getQuantizationDistances() const{
    return quantizationDistances;
}

SelfOrganizingMap SOMQuantizer::getSelfOrganizingMap() const{
    return som;
}
    
bool SOMQuantizer::setNumClusters(const UINT numClusters){
    clear();
    this->numClusters = numClusters;
    return true;
}
    
}//End of namespace GRT