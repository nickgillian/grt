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

#include "ClassLabelChangeFilter.h"

namespace GRT{
    
//Register the ClassLabelChangeFilter module with the PostProcessing base class
RegisterPostProcessingModule< ClassLabelChangeFilter > ClassLabelChangeFilter::registerModule("ClassLabelChangeFilter");
    
ClassLabelChangeFilter::ClassLabelChangeFilter(){
    
    classType = "ClassLabelChangeFilter";
    postProcessingType = classType;
    postProcessingInputMode = INPUT_MODE_PREDICTED_CLASS_LABEL;
    postProcessingOutputMode = OUTPUT_MODE_PREDICTED_CLASS_LABEL;
    debugLog.setProceedingText("[DEBUG ClassLabelChangeFilter]");
    errorLog.setProceedingText("[ERROR ClassLabelChangeFilter]");
    warningLog.setProceedingText("[WARNING ClassLabelChangeFilter]");
    init();
}
    
ClassLabelChangeFilter::ClassLabelChangeFilter(const ClassLabelChangeFilter &rhs){
    
    classType = "ClassLabelChangeFilter";
    postProcessingType = classType;
    postProcessingInputMode = INPUT_MODE_PREDICTED_CLASS_LABEL;
    postProcessingOutputMode = OUTPUT_MODE_PREDICTED_CLASS_LABEL;
    debugLog.setProceedingText("[DEBUG ClassLabelChangeFilter]");
    errorLog.setProceedingText("[ERROR ClassLabelChangeFilter]");
    warningLog.setProceedingText("[WARNING ClassLabelChangeFilter]");
    
    //Copy the ClassLabelChangeFilter values 
    this->filteredClassLabel = rhs.filteredClassLabel;
    this->labelChanged = rhs.labelChanged;
    
    //Clone the post processing base variables
    copyBaseVariables( (PostProcessing*)&rhs );
}

ClassLabelChangeFilter::~ClassLabelChangeFilter(){

}
    
ClassLabelChangeFilter& ClassLabelChangeFilter::operator=(const ClassLabelChangeFilter &rhs){
    
    if( this != &rhs ){
        //Copy the ClassLabelChangeFilter values 
        this->filteredClassLabel = rhs.filteredClassLabel;
        this->labelChanged = rhs.labelChanged;
        
        //Clone the post processing base variables
        copyBaseVariables( (PostProcessing*)&rhs );
    }
        return *this;
}
    
bool ClassLabelChangeFilter::deepCopyFrom(const PostProcessing *postProcessing){
    
    if( postProcessing == NULL ) return false;
    
    if( this->getPostProcessingType() == postProcessing->getPostProcessingType() ){
        
        ClassLabelChangeFilter *ptr = (ClassLabelChangeFilter*)postProcessing;
        
        //Clone the ClassLabelChangeFilter values 
        this->filteredClassLabel = ptr->filteredClassLabel;
        this->labelChanged = ptr->labelChanged;
        
        //Clone the post processing base variables
        copyBaseVariables( postProcessing );
        return true;
    }
    return false;
}
    
bool ClassLabelChangeFilter::process(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return false;
    }
    
    //Use only the first value (as that is the predicted class label)
    processedData[0] = filter( (UINT)inputVector[0] );
    return true;
}
    
bool ClassLabelChangeFilter::reset(){
    filteredClassLabel = 0;
    labelChanged = false;
    processedData.clear();
    processedData.resize(1,0);
    return true;
}
    
bool ClassLabelChangeFilter::init(){

    initialized = false;
    
    reset();
    numInputDimensions = 1;
    numOutputDimensions = 1;
    initialized = true;
    return true;
}

UINT ClassLabelChangeFilter::filter(UINT predictedClassLabel){
    
    labelChanged = false;
    
    if( predictedClassLabel != filteredClassLabel ){
        filteredClassLabel = predictedClassLabel;
        labelChanged = true;
        return filteredClassLabel;
    }
    
	return GRT_DEFAULT_NULL_CLASS_LABEL;
}
    
bool ClassLabelChangeFilter::saveModelToFile(string filename) const{
    
    if( !initialized ){
        errorLog << "saveModelToFile(string filename) - The ClassLabelChangeFilter has not been initialized" << endl;
        return false;
    }
    
    std::fstream file; 
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
}

bool ClassLabelChangeFilter::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "GRT_CLASS_LABEL_CHANGE_FILTER_FILE_V1.0" << endl;
    file << "NumInputDimensions: " << numInputDimensions << endl;
    file << "NumOutputDimensions: " << numOutputDimensions << endl;
    
    return true;
}

bool ClassLabelChangeFilter::loadModelFromFile(string filename){
    
    std::fstream file; 
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
}

bool ClassLabelChangeFilter::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_CLASS_LABEL_CHANGE_FILTER_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumInputDimensions header!" << endl;
        return false;     
    }
    file >> numInputDimensions;
    
    //Load the number of output dimensions
    file >> word;
    if( word != "NumOutputDimensions:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumOutputDimensions header!" << endl;
        return false;     
    }
    file >> numOutputDimensions;
    
    //Init the classLabelTimeoutFilter module to ensure everything is initialized correctly
    return init();
}
    
bool ClassLabelChangeFilter::getChange(){
    return labelChanged;
}

}//End of namespace GRT