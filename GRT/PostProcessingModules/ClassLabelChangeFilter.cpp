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
#include "ClassLabelChangeFilter.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string ClassLabelChangeFilter::id = "ClassLabelChangeFilter";
std::string ClassLabelChangeFilter::getId() { return ClassLabelChangeFilter::id; }

//Register the ClassLabelChangeFilter module with the PostProcessing base class
RegisterPostProcessingModule< ClassLabelChangeFilter > ClassLabelChangeFilter::registerModule( ClassLabelChangeFilter::getId() );

ClassLabelChangeFilter::ClassLabelChangeFilter() : PostProcessing( ClassLabelChangeFilter::getId() )
{
    postProcessingInputMode = INPUT_MODE_PREDICTED_CLASS_LABEL;
    postProcessingOutputMode = OUTPUT_MODE_PREDICTED_CLASS_LABEL;
    init();
}

ClassLabelChangeFilter::ClassLabelChangeFilter(const ClassLabelChangeFilter &rhs) : PostProcessing( ClassLabelChangeFilter::getId() )
{
    postProcessingInputMode = INPUT_MODE_PREDICTED_CLASS_LABEL;
    postProcessingOutputMode = OUTPUT_MODE_PREDICTED_CLASS_LABEL;
    
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
    
    if( this->getId() == postProcessing->getId() ){
        
        const ClassLabelChangeFilter *ptr = dynamic_cast<const ClassLabelChangeFilter*>(postProcessing);
        
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
        errorLog << "process(const VectorDouble &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "process(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.getSize() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
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

UINT ClassLabelChangeFilter::filter(const UINT predictedClassLabel){
    
    labelChanged = false;
    
    if( predictedClassLabel != filteredClassLabel ){
        filteredClassLabel = predictedClassLabel;
        labelChanged = true;
        return filteredClassLabel;
    }
    
    return GRT_DEFAULT_NULL_CLASS_LABEL;
}

bool ClassLabelChangeFilter::save( std::fstream &file ) const {
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    file << "GRT_CLASS_LABEL_CHANGE_FILTER_FILE_V1.0" << std::endl;
    file << "NumInputDimensions: " << numInputDimensions << std::endl;
    file << "NumOutputDimensions: " << numOutputDimensions << std::endl;
    
    return true;
}

bool ClassLabelChangeFilter::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_CLASS_LABEL_CHANGE_FILTER_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog << "load(fstream &file) - Failed to read NumInputDimensions header!" << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    //Load the number of output dimensions
    file >> word;
    if( word != "NumOutputDimensions:" ){
        errorLog << "load(fstream &file) - Failed to read NumOutputDimensions header!" << std::endl;
        return false;
    }
    file >> numOutputDimensions;
    
    //Init the classLabelTimeoutFilter module to ensure everything is initialized correctly
    return init();
}

bool ClassLabelChangeFilter::getChange() const {
    return labelChanged;
}

GRT_END_NAMESPACE
