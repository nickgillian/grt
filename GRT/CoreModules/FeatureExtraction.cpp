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
#include "FeatureExtraction.h"

GRT_BEGIN_NAMESPACE
    
FeatureExtraction::StringFeatureExtractionMap* FeatureExtraction::stringFeatureExtractionMap = NULL;
UINT FeatureExtraction::numFeatureExtractionInstances = 0;

std::string FeatureExtraction::getFeatureExtractionType() const { return MLBase::getId(); } //Legacy
FeatureExtraction* FeatureExtraction::createInstanceFromString( const std::string &id ){ return create(id); }
FeatureExtraction* FeatureExtraction::createNewInstance() const{ return create(); }
    
FeatureExtraction* FeatureExtraction::create( const std::string &id ){
    
    StringFeatureExtractionMap::iterator iter = getMap()->find( id );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}
    
FeatureExtraction* FeatureExtraction::create() const{
    return create( MLBase::getId() );
}
    
FeatureExtraction::FeatureExtraction( const std::string id ) : MLBase( id, MLBase::FEATURE_EXTRACTION )
{
    initialized = false; 
    featureDataReady = false;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    inputType = DATA_TYPE_VECTOR;
    outputType = DATA_TYPE_VECTOR;
    numFeatureExtractionInstances++;
}
    
FeatureExtraction::~FeatureExtraction(){
    if( --numFeatureExtractionInstances == 0 ){
        delete stringFeatureExtractionMap;
        stringFeatureExtractionMap = NULL;
    }
}

bool FeatureExtraction::copyBaseVariables(const FeatureExtraction *featureExtractionModule){
    
    if( featureExtractionModule == NULL ){
        errorLog << "copyBaseVariables(const FeatureExtraction *featureExtractionModule) - featureExtractionModule pointer is NULL!" << std::endl;
        return false;
    }
    
    if( !this->copyMLBaseVariables( featureExtractionModule ) ){
        return false;
    }
    
    this->featureExtractionType = featureExtractionModule->featureExtractionType;
    this->initialized = featureExtractionModule->initialized;
    this->featureDataReady = featureExtractionModule->featureDataReady;
    this->numInputDimensions = featureExtractionModule->numInputDimensions;
    this->numOutputDimensions = featureExtractionModule->numOutputDimensions;
    this->featureVector = featureExtractionModule->featureVector;
    this->featureMatrix = featureExtractionModule->featureMatrix;

    return true;
}
    
bool FeatureExtraction::init(){
    
    if( numOutputDimensions == 0 ){
        errorLog << "init() - Failed to init module, the number of output dimensions is zero!" << std::endl;
        initialized = false;
        return false;
    }
    
    //Flag that the feature data has not been computed yet
    featureDataReady = false;
    
    //Resize the feature vector
    featureVector.resize(numOutputDimensions,0);
    
    //Flag the module has been initialized
    initialized = true;
    
    return true;
}
    
bool FeatureExtraction::clear(){
    
    //Clear the base class
    MLBase::clear();
    
    initialized = false;
    featureDataReady = false;
    featureVector.clear();
    
    return true;
}
    
bool FeatureExtraction::saveFeatureExtractionSettingsToFile( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    file << "Initialized: " << initialized << std::endl;
    
    return true;
}

bool FeatureExtraction::loadFeatureExtractionSettingsFromFile( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Try and load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile( file ) ){
        return false;
    }
    
    std::string word;
    
    //Load if the filter has been initialized
    file >> word;
    if( word != "Initialized:" ){
        errorLog << "loadPreProcessingSettingsFromFile(fstream &file) - Failed to read Initialized header!" << std::endl;
        clear();
        return false;
    }
    file >> initialized;
    
    //If the module has been initalized then call the init function to setup the feature data vector
    if( initialized ){
        return init();
    }
    
    return true;
}

UINT FeatureExtraction::getNumInputDimensions() const{ 
    return numInputDimensions; 
}

UINT FeatureExtraction::getNumOutputDimensions() const{ 
    return numOutputDimensions; 
}

bool FeatureExtraction::getInitialized() const{ 
    return initialized; 
}
    
bool FeatureExtraction::getFeatureDataReady() const{
    return featureDataReady;
}

const VectorFloat& FeatureExtraction::getFeatureVector() const{ 
    return featureVector; 
}

const MatrixFloat& FeatureExtraction::getFeatureMatrix() const {
    return featureMatrix;
}

GRT_END_NAMESPACE

