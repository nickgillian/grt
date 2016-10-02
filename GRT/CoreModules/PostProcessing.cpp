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
#include "PostProcessing.h"

GRT_BEGIN_NAMESPACE
    
PostProcessing::StringPostProcessingMap* PostProcessing::stringPostProcessingMap = NULL;
UINT PostProcessing::numPostProcessingInstances = 0;

PostProcessing* PostProcessing::createNewInstance() const { return create(); } ///<Legacy function
PostProcessing* PostProcessing::createInstanceFromString(const std::string &id) { return create(id); } ///<Legacy function
    
PostProcessing* PostProcessing::create(const std::string &id){
    
    StringPostProcessingMap::iterator iter = getMap()->find( id );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}

PostProcessing* PostProcessing::create() const{
    return create( getId() );
}
    
PostProcessing::PostProcessing( const std::string &id ) : MLBase( id, MLBase::POST_PROCESSING )
{
    postProcessingInputMode = INPUT_MODE_NOT_SET;
    postProcessingOutputMode = OUTPUT_MODE_NOT_SET;
    initialized = false; 
    numInputDimensions = 0;
    numOutputDimensions = 0;
    numPostProcessingInstances++;
}
    
PostProcessing::~PostProcessing(void){
    if( --numPostProcessingInstances == 0 ){
        delete stringPostProcessingMap;
        stringPostProcessingMap = NULL;
    }
}

bool PostProcessing::copyBaseVariables(const PostProcessing *postProcessingModule){
    
    if( postProcessingModule == NULL ){
        errorLog << "copyBaseVariables(const PostProcessing *postProcessingModule) - postProcessingModule pointer is NULL!" << std::endl;
        return false;
    }
    
    if( !this->copyMLBaseVariables( postProcessingModule ) ){
        return false;
    }
    
    this->postProcessingType = postProcessingModule->postProcessingType;
    this->postProcessingInputMode =  postProcessingModule->postProcessingInputMode;
    this->postProcessingOutputMode =  postProcessingModule->postProcessingOutputMode;
    this->initialized = postProcessingModule->initialized;
    this->numInputDimensions = postProcessingModule->numInputDimensions;
    this->numOutputDimensions = postProcessingModule->numOutputDimensions;
    this->processedData = postProcessingModule->processedData;
    this->debugLog = postProcessingModule->debugLog;
    this->errorLog = postProcessingModule->errorLog;
    this->warningLog = postProcessingModule->warningLog;
    return true;
}
    
bool PostProcessing::init(){
    
    if( numOutputDimensions == 0 ){
        errorLog << "init() - Failed to init module, the number of output dimensions is zero!" << std::endl;
        initialized = false;
        return false;
    }
    
    //Setup the output vector
    processedData.resize( numOutputDimensions );
    
    //Flag the module has been initialized
    initialized = true;
    
    return true;
}
    
bool PostProcessing::saveModelToFile(std::string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool PostProcessing::loadModelFromFile(std::string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool PostProcessing::savePostProcessingSettingsToFile(std::fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "savePostProcessingSettingsToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    file << "Initialized: " << initialized << std::endl;
    
    return true;
}

bool PostProcessing::loadPostProcessingSettingsFromFile(std::fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadPostProcessingSettingsFromFile(fstream &file) - The file is not open!" << std::endl;
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
        errorLog << "loadPostProcessingSettingsFromFile(fstream &file) - Failed to read Initialized header!" << std::endl;
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

std::string PostProcessing::getPostProcessingType() const{ 
    return postProcessingType;
}
    
UINT PostProcessing::getPostProcessingInputMode() const{ 
     return postProcessingInputMode; 
}
    
UINT PostProcessing::getPostProcessingOutputMode() const{ 
    return postProcessingOutputMode; 
}
    
UINT PostProcessing::getNumInputDimensions() const{ 
    return numInputDimensions; 
}
    
UINT PostProcessing::getNumOutputDimensions() const{ 
    return numOutputDimensions; 
}
    
bool PostProcessing::getInitialized() const{ 
    return initialized; 
}
    
bool PostProcessing::getIsPostProcessingInputModePredictedClassLabel() const{ 
    return postProcessingInputMode==INPUT_MODE_PREDICTED_CLASS_LABEL; 
}
    
bool PostProcessing::getIsPostProcessingInputModeClassLikelihoods() const{ 
    return postProcessingInputMode==INPUT_MODE_CLASS_LIKELIHOODS; 
}
    
bool PostProcessing::getIsPostProcessingOutputModePredictedClassLabel() const{ 
    return postProcessingOutputMode==OUTPUT_MODE_PREDICTED_CLASS_LABEL; 
}
    
bool PostProcessing::getIsPostProcessingOutputModeClassLikelihoods() const{ 
    return postProcessingOutputMode==OUTPUT_MODE_CLASS_LIKELIHOODS; 
}
    
VectorFloat PostProcessing::getProcessedData() const{ 
    return processedData; 
}

GRT_END_NAMESPACE

