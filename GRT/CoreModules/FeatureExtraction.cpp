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

#include "FeatureExtraction.h"

namespace GRT{
    
FeatureExtraction::StringFeatureExtractionMap* FeatureExtraction::stringFeatureExtractionMap = NULL;
UINT FeatureExtraction::numFeatureExtractionInstances = 0;
    
FeatureExtraction* FeatureExtraction::createInstanceFromString(string const &featureExtractionType){
    
    StringFeatureExtractionMap::iterator iter = getMap()->find( featureExtractionType );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}
    
FeatureExtraction* FeatureExtraction::createNewInstance() const{
    return createInstanceFromString(featureExtractionType);
}
    
FeatureExtraction::FeatureExtraction(){
    featureExtractionType = "NOT_SET"; 
    initialized = false; 
    featureDataReady = false;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    numFeatureExtractionInstances++;
    infoLog.setProceedingText("[FeatureExtraction]");
    warningLog.setProceedingText("[WARNING FeatureExtraction]");
    errorLog.setProceedingText("[ERROR FeatureExtraction]");
}
    
FeatureExtraction::~FeatureExtraction(){
    if( --numFeatureExtractionInstances == 0 ){
        delete stringFeatureExtractionMap;
        stringFeatureExtractionMap = NULL;
    }
}

bool FeatureExtraction::copyBaseVariables(const FeatureExtraction *featureExtractionModule){
    
    if( featureExtractionModule == NULL ){
        errorLog << "copyBaseVariables(const FeatureExtraction *featureExtractionModule) - featureExtractionModule pointer is NULL!" << endl;
        return false;
    }
    
    if( !this->copyGRTBaseVariables( featureExtractionModule ) ){
        return false;
    }
    
    this->featureExtractionType = featureExtractionModule->featureExtractionType;
    this->initialized = featureExtractionModule->initialized;
    this->featureDataReady = featureExtractionModule->featureDataReady;
    this->numInputDimensions = featureExtractionModule->numInputDimensions;
    this->numOutputDimensions = featureExtractionModule->numOutputDimensions;
    this->featureVector = featureExtractionModule->featureVector;
    this->debugLog = featureExtractionModule->debugLog;
    this->errorLog = featureExtractionModule->errorLog;
    this->warningLog = featureExtractionModule->warningLog;

    return true;
}
    
bool FeatureExtraction::init(){
    
    //Clear any previous feature vector
    clear();
    
    if( numOutputDimensions == 0 ){
        errorLog << "init() - Failed to init module, the number of output dimensions is zero!" << endl;
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
   
bool FeatureExtraction::saveModelToFile(const string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool FeatureExtraction::loadModelFromFile(const string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}
    
bool FeatureExtraction::saveFeatureExtractionSettingsToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    file << "Initialized: " << initialized << endl;
    
    return true;
}

bool FeatureExtraction::loadFeatureExtractionSettingsFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Try and load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile( file ) ){
        return false;
    }
    
    string word;
    
    //Load if the filter has been initialized
    file >> word;
    if( word != "Initialized:" ){
        errorLog << "loadPreProcessingSettingsFromFile(fstream &file) - Failed to read Initialized header!" << endl;
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
    
string FeatureExtraction::getFeatureExtractionType() const{ 
    return featureExtractionType; 
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

VectorDouble FeatureExtraction::getFeatureVector() const{ 
    return featureVector; 
}
    

} //End of namespace GRT

