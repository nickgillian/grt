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
#include "DeadZone.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string DeadZone::id = "DeadZone";
std::string DeadZone::getId() { return DeadZone::id; }

//Register the DeadZone module with the PreProcessing base class
RegisterPreProcessingModule< DeadZone > DeadZone::registerModule( DeadZone::getId() );

DeadZone::DeadZone( const Float lowerLimit, const Float upperLimit, const UINT numDimensions) : PreProcessing( DeadZone::getId() )
{
    init(lowerLimit,upperLimit,numDimensions);
}

DeadZone::DeadZone(const DeadZone &rhs) : PreProcessing( DeadZone::getId() )
{
    this->lowerLimit = rhs.lowerLimit;
    this->upperLimit = rhs.upperLimit;
    copyBaseVariables( (PreProcessing*)&rhs );
}

DeadZone::~DeadZone(){
    
}

DeadZone& DeadZone::operator=(const DeadZone &rhs){
    if(this!=&rhs){
        this->lowerLimit = rhs.lowerLimit;
        this->upperLimit = rhs.upperLimit;
        copyBaseVariables( (PreProcessing*)&rhs );
    }
    return *this;
}

bool DeadZone::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getId() == preProcessing->getId() ){
        
        const DeadZone *ptr = dynamic_cast<const DeadZone*>(preProcessing);
        //Clone the DeadZone values
        this->lowerLimit = ptr->lowerLimit;
        this->upperLimit = ptr->upperLimit;
        
        //Clone the base class variables
        return copyBaseVariables( preProcessing );
    }
    
    errorLog << "deepCopyFrom(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << std::endl;
    
    return false;
}

bool DeadZone::process(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    filter( inputVector );
    
    if( processedData.size() == numOutputDimensions ) return true;
    return false;
}

bool DeadZone::reset(){
    return true;
}

bool DeadZone::save(std::fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    file << "GRT_DEAD_ZONE_FILE_V1.0" << std::endl;
    
    file << "NumInputDimensions: " << numInputDimensions << std::endl;
    file << "NumOutputDimensions: " << numOutputDimensions << std::endl;
    file << "LowerLimit: " << lowerLimit << std::endl;
    file << "UpperLimit: " << upperLimit << std::endl;
    
    return true;
}

bool DeadZone::load(std::fstream &file){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_DEAD_ZONE_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    //Load the number of input dimensions
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
    
    //Load the lower limit
    file >> word;
    if( word != "LowerLimit:" ){
        errorLog << "load(fstream &file) - Failed to read LowerLimit header!" << std::endl;
        return false;
    }
    file >> lowerLimit;
    
    file >> word;
    if( word != "UpperLimit:" ){
        errorLog << "load(fstream &file) - Failed to read UpperLimit header!" << std::endl;
        return false;
    }
    file >> upperLimit;
    
    //Init the deadzone module to ensure everything is initialized correctly
    return init(lowerLimit,upperLimit,numInputDimensions);
}

bool DeadZone::init(Float lowerLimit,Float upperLimit,UINT numDimensions){
    
    initialized = false;
    
    if( numDimensions == 0 ){
        errorLog << "init(Float lowerLimit,Float upperLimit,UINT numDimensions) - NumDimensions must be greater than 0!" << std::endl;
        return false;
    }
    
    if( lowerLimit >= upperLimit ){
        errorLog << "init(Float lowerLimit,Float upperLimit,UINT numDimensions) - The lower limit must be less than the upperlimit!" << std::endl;
        return false;
    }
    
    this->lowerLimit = lowerLimit;
    this->upperLimit = upperLimit;
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numDimensions;
    processedData.clear();
    processedData.resize(numOutputDimensions,0);
    initialized = true;
    
    return true;
}

Float DeadZone::filter(const Float x){
    VectorFloat y = filter(VectorFloat(1,x));
    if( y.getSize() == 0 ) return 0;
    return y[0];
}

VectorFloat DeadZone::filter(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "filter(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.getSize() != numInputDimensions ){
        errorLog << "filter(const VectorFloat &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << std::endl;
        return VectorFloat();
    }
    
    for(UINT n=0; n<numInputDimensions; n++){
        if( x[n] > lowerLimit && x[n] < upperLimit ){
            processedData[n] = 0;
        }else{
            if( x[n] >= upperLimit ) processedData[n] = x[n] - upperLimit;
            else processedData[n] = x[n] - lowerLimit;
            }
    }
    return processedData;
}

Float DeadZone::getLowerLimit() const { 
    if( initialized ){ return lowerLimit; } 
    return 0; 
}
    
Float DeadZone::getUpperLimit() const { 
    if( initialized ){ return upperLimit; } 
    return 0; 
}

bool DeadZone::setLowerLimit(Float lowerLimit){
    this->lowerLimit = lowerLimit;
    return true;
}

bool DeadZone::setUpperLimit(Float upperLimit){
    this->upperLimit = upperLimit;
    return true;
}

GRT_END_NAMESPACE
