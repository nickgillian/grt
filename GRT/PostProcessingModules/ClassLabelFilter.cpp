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
#include "ClassLabelFilter.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string ClassLabelFilter::id = "ClassLabelFilter";
std::string ClassLabelFilter::getId() { return ClassLabelFilter::id; }

//Register the ClassLabelFilter module with the PostProcessing base class
RegisterPostProcessingModule< ClassLabelFilter > ClassLabelFilter::registerModule( ClassLabelFilter::getId() );

ClassLabelFilter::ClassLabelFilter(const UINT minimumCount,const UINT bufferSize) : PostProcessing( ClassLabelFilter::getId() )
{
    postProcessingInputMode = INPUT_MODE_PREDICTED_CLASS_LABEL;
    postProcessingOutputMode = OUTPUT_MODE_PREDICTED_CLASS_LABEL;
    init(minimumCount,bufferSize);
}

ClassLabelFilter::ClassLabelFilter(const ClassLabelFilter &rhs) : PostProcessing( ClassLabelFilter::getId() )
{
    
    postProcessingInputMode = INPUT_MODE_PREDICTED_CLASS_LABEL;
    postProcessingOutputMode = OUTPUT_MODE_PREDICTED_CLASS_LABEL;
    
    //Copy the ClassLabelFilter values
    this->filteredClassLabel = rhs.filteredClassLabel;
    this->minimumCount = rhs.minimumCount;
    this->bufferSize = rhs.bufferSize;
    this->buffer = rhs.buffer;
    
    //Clone the post processing base variables
    copyBaseVariables( (PostProcessing*)&rhs );
}

ClassLabelFilter::~ClassLabelFilter(){
    
}

ClassLabelFilter& ClassLabelFilter::operator=(const ClassLabelFilter &rhs){
    
    if( this != &rhs ){
        //Copy the ClassLabelFilter values
        this->filteredClassLabel = rhs.filteredClassLabel;
        this->minimumCount = rhs.minimumCount;
        this->bufferSize = rhs.bufferSize;
        this->buffer = rhs.buffer;
        
        //Clone the post processing base variables
        copyBaseVariables( (PostProcessing*)&rhs );
    }
    
    return *this;
}

bool ClassLabelFilter::deepCopyFrom(const PostProcessing *postProcessing){
    
    if( postProcessing == NULL ) return false;
    
    if( this->getId() == postProcessing->getId() ){
        
        const ClassLabelFilter *ptr = dynamic_cast<const ClassLabelFilter*>(postProcessing);
        
        //Clone the ClassLabelFilter values
        this->filteredClassLabel = ptr->filteredClassLabel;
        this->minimumCount = ptr->minimumCount;
        this->bufferSize = ptr->bufferSize;
        this->buffer = ptr->buffer;
        
        //Clone the post processing base variables
        copyBaseVariables( postProcessing );
        return true;
    }
    return false;
}

bool ClassLabelFilter::process(const VectorDouble &inputVector){
    
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

bool ClassLabelFilter::reset(){
    filteredClassLabel = 0;
    processedData.clear();
    processedData.resize(1,0);
    buffer.clear();
    if( bufferSize > 0 ){
        initialized = buffer.resize(bufferSize,0);
    }else initialized = false;
    return true;
}

bool ClassLabelFilter::init(const UINT minimumCount,const UINT bufferSize){
    
    initialized = false;
    
    if( minimumCount < 1 ){
        errorLog << "init(UINT minimumCount,UINT bufferSize) - MinimumCount must be greater than or equal to 1!" << std::endl;
        return false;
    }
    
    if( bufferSize < 1 ){
        errorLog << "init(UINT minimumCount,UINT bufferSize) - BufferSize must be greater than or equal to 1!" << std::endl;
        return false;
    }
    
    if( bufferSize < minimumCount ){
        errorLog << "init(UINT minimumCount,UINT bufferSize) - The buffer size must be greater than or equal to the minimum count!" << std::endl;
        return false;
    }
    
    this->minimumCount = minimumCount;
    this->bufferSize = bufferSize;
    numInputDimensions = 1;
    numOutputDimensions = 1;
    initialized = reset();
    return true;
}

UINT ClassLabelFilter::filter(const UINT predictedClassLabel){
    
    if( !initialized ){
        errorLog << "filter(UINT predictedClassLabel) - The filter has not been initialized!" << std::endl;
        filteredClassLabel = 0;
        return 0;
    }
    
    //Add the current predictedClassLabel to the buffer
    buffer.push_back( predictedClassLabel );
    
    //Count the class values in the buffer, automatically start with the first value in the buffer
    Vector< ClassTracker > classTracker( 1, ClassTracker( buffer[0], 1 ) );
    
    UINT maxCount = classTracker[0].counter;
    UINT maxClass = classTracker[0].classLabel;
    bool classLabelFound = false;
    
    for(UINT i=1; i<bufferSize; i++){
        classLabelFound = false;
        UINT currentCount = 0;
        UINT currentClassLabel = buffer[i];
        for(UINT k=0; k<classTracker.size(); k++){
            if( currentClassLabel == classTracker[k].classLabel ){
                classTracker[k].counter++;
                classLabelFound = true;
                currentCount = classTracker[k].counter;
                break;
            }
        }
        
        //If we have not found the class label then we need to add this class to the classTracker
        if( !classLabelFound ){
            classTracker.push_back( ClassTracker(currentClassLabel,1) );
            currentCount = 1;
        }
        
        //Check to see if we should update the max count and maxClass (ignoring class label 0)
        if( currentCount > maxCount && currentClassLabel != 0 ){
            maxCount = currentCount;
            maxClass = currentClassLabel;
        }
    }
    
    //printf("minimumCount: %i maxCount: %i maxClass: %i\n",minimumCount,maxCount,maxClass);
    
    if( maxCount >= minimumCount ){
        filteredClassLabel = maxClass;
    }else filteredClassLabel = 0;
    
    return filteredClassLabel;
}

bool ClassLabelFilter::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    file << "GRT_CLASS_LABEL_FILTER_FILE_V1.0" << std::endl;
    file << "NumInputDimensions: " << numInputDimensions << std::endl;
    file << "NumOutputDimensions: " << numOutputDimensions << std::endl;
    file << "MinimumCount: " << minimumCount << std::endl;
    file << "BufferSize: " << bufferSize << std::endl;
    
    return true;
}

bool ClassLabelFilter::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_CLASS_LABEL_FILTER_FILE_V1.0" ){
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
    
    //Load the minimumCount
    file >> word;
    if( word != "MinimumCount:" ){
        errorLog << "load(fstream &file) - Failed to read MinimumCount header!" << std::endl;
        return false;
    }
    file >> minimumCount;
    
    file >> word;
    if( word != "BufferSize:" ){
        errorLog << "load(fstream &file) - Failed to read BufferSize header!" << std::endl;
        return false;
    }
    file >> bufferSize;
    
    //Init the classLabelFilter module to ensure everything is initialized correctly
    return init(minimumCount,bufferSize);
}

bool ClassLabelFilter::setMinimumCount(const UINT minimumCount){
    this->minimumCount = minimumCount;
    if( initialized ){
        return reset();
    }
    return true;
}

bool ClassLabelFilter::setBufferSize(const UINT bufferSize){
    this->bufferSize = bufferSize;
    if( initialized ){
        return reset();
    }
    return true;
}

GRT_END_NAMESPACE
