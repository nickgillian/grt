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

#include "PreProcessing.h"

namespace GRT{
    
PreProcessing::StringPreProcessingMap* PreProcessing::stringPreProcessingMap = NULL;
UINT PreProcessing::numPreProcessingInstances = 0;
    
PreProcessing* PreProcessing::createInstanceFromString(string const &preProcessingType){
    
    StringPreProcessingMap::iterator iter = getMap()->find( preProcessingType );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}
    
PreProcessing::PreProcessing(void){
    preProcessingType = "NOT_SET"; 
    initialized = false; 
    numInputDimensions = 0;
    numOutputDimensions = 0;
    numPreProcessingInstances++;
}
    
PreProcessing::~PreProcessing(void){
    if( --numPreProcessingInstances == 0 ){
        delete stringPreProcessingMap;
        stringPreProcessingMap = NULL;
    }
}

bool PreProcessing::copyBaseVariables(const PreProcessing *preProcessingModule){
    
    if( preProcessingModule == NULL ){
        errorLog << "copyBaseVariables(const PreProcessing *preProcessingModule) - preProcessingModule pointer is NULL!" << endl;
        return false;
    }
    
    if( !this->copyGRTBaseVariables( preProcessingModule ) ){
        return false;
    }
    
    this->preProcessingType = preProcessingModule->preProcessingType;
    this->initialized = preProcessingModule->initialized;
    this->numInputDimensions = preProcessingModule->numInputDimensions;
    this->numOutputDimensions = preProcessingModule->numOutputDimensions;
    this->processedData = preProcessingModule->processedData;
    this->debugLog = preProcessingModule->debugLog;
    this->errorLog = preProcessingModule->errorLog;
    this->warningLog = preProcessingModule->warningLog;
    
    return true;
}
    
bool PreProcessing::reset(){
    
    //Reset the processed data vector
    if( processedData.size() > 0 )
        fill(processedData.begin(),processedData.end(),0);
    
    return true;
}
    
bool PreProcessing::clear(){
    initialized = false;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    processedData.clear();
    return true;
}
    
bool PreProcessing::init(){
    
    if( numOutputDimensions == 0 ){
        errorLog << "init() - Failed to init module, the number of output dimensions is zero!" << endl;
        initialized = false;
        return false;
    }
    
    //Setup the output vector
    processedData.resize( numOutputDimensions );
    
    //Flag the module has been initialized
    initialized = true;
    
    return true;
}

bool PreProcessing::saveModelToFile(const string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool PreProcessing::loadModelFromFile(const string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}
    
bool PreProcessing::savePreProcessingSettingsToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "savePreProcessingSettingsToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    file << "Initialized: " << initialized << endl;
    
    return true;
}
    
bool PreProcessing::loadPreProcessingSettingsFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadPreProcessingSettingsFromFile(fstream &file) - The file is not open!" << endl;
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
    
    //If the module has been initalized then call the init function to setup the processed data vector
    if( initialized ){
        return init();
    }
    
    return true;
}
    
PreProcessing* PreProcessing::createNewInstance() const{
    return createInstanceFromString(preProcessingType);
}
    
string PreProcessing::getPreProcessingType() const{ 
    return preProcessingType; 
}
    
UINT PreProcessing::getNumInputDimensions() const{ 
    return numInputDimensions; 
}
    
UINT PreProcessing::getNumOutputDimensions() const{ 
    return numOutputDimensions; 
}
    
bool PreProcessing::getInitialized() const{ 
    return initialized; 
}
    
VectorDouble PreProcessing::getProcessedData() const{ 
    return processedData; 
}

} //End of namespace GRT

