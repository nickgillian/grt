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
#include "MedianFilter.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string MedianFilter::id = "MedianFilter";
std::string MedianFilter::getId() { return MedianFilter::id; }

//Register the MedianFilter module with the PreProcessing base class
RegisterPreProcessingModule< MedianFilter > MedianFilter::registerModule( MedianFilter::getId() );

MedianFilter::MedianFilter(UINT filterSize,UINT numDimensions) : PreProcessing( MedianFilter::getId() )
{
    init(filterSize,numDimensions);
}

MedianFilter::MedianFilter(const MedianFilter &rhs) : PreProcessing( MedianFilter::getId() )
{
    //Zero this instance
    this->filterSize = 0;
    this->inputSampleCounter = 0;
    
    //Copy the settings from the rhs instance
    *this = rhs;
}

MedianFilter::~MedianFilter(){
    
}

MedianFilter& MedianFilter::operator=(const MedianFilter &rhs){
    if(this!=&rhs){
        //Clear this instance
        this->filterSize = 0;
        this->inputSampleCounter = 0;
        this->dataBuffer.clear();
        
        //Copy from the rhs instance
        if( rhs.initialized ){
            this->init( rhs.filterSize, rhs.numInputDimensions );
            this->dataBuffer = rhs.dataBuffer;
        }
        
        //Copy the preprocessing base variables
        copyBaseVariables( (PreProcessing*)&rhs );
    }
    return *this;
}

bool MedianFilter::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getId() == preProcessing->getId() ){
        
        //Call the equals operator
        *this = *dynamic_cast<const MedianFilter*>(preProcessing);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << std::endl;
    
    return false;
}


bool MedianFilter::process(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorFloat &inputVector) - The filter has not been initialized!" << std::endl;
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

bool MedianFilter::reset(){
    if( initialized ) return init(filterSize,numInputDimensions);
    return false;
}

bool MedianFilter::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    file << "GRT_MEDIAN_FILTER_FILE_V1.0" << std::endl;
    
    file << "NumInputDimensions: " << numInputDimensions << std::endl;
    file << "NumOutputDimensions: " << numOutputDimensions << std::endl;
    file << "FilterSize: " << filterSize << std::endl;
    
    return true;
}

bool MedianFilter::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_MEDIAN_FILTER_FILE_V1.0" ){
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
    
    //Load the filter factor
    file >> word;
    if( word != "FilterSize:" ){
        errorLog << "load(fstream &file) - Failed to read FilterSize header!" << std::endl;
        return false;
    }
    file >> filterSize;
    
    //Init the filter module to ensure everything is initialized correctly
    return init(filterSize,numInputDimensions);
}

bool MedianFilter::init(const UINT filterSize,const UINT numDimensions){
    
    //Cleanup the old memory
    initialized = false;
    inputSampleCounter = 0;
    
    if( filterSize == 0 ){
        errorLog << "init(UINT filterSize,UINT numDimensions) - Filter size can not be zero!" << std::endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(UINT filterSize,UINT numDimensions) - The number of dimensions must be greater than zero!" << std::endl;
        return false;
    }
    
    //Resize the filter
    this->filterSize = filterSize;
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numDimensions;
    processedData.clear();
    processedData.resize(numDimensions,0);
    initialized = dataBuffer.resize( filterSize, VectorFloat(numInputDimensions,0) );
    
    if( !initialized ){
        errorLog << "init(UINT filterSize,UINT numDimensions) - Failed to resize dataBuffer!" << std::endl;
    }
    
    return initialized;
}

Float MedianFilter::filter(const Float x){
    
    VectorFloat y = filter(VectorFloat(1,x));
    
    if( y.size() == 0 ) return 0;
    return y[0];
}

VectorFloat MedianFilter::filter(const VectorFloat &x){
    
    //If the filter has not been initialised then return 0, otherwise filter x and return y
    if( !initialized ){
        errorLog << "filter(const VectorFloat &x) - The filter has not been initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.getSize() != numInputDimensions ){
        errorLog << "filter(const VectorFloat &x) - The size of the input Vector (" << x.getSize() << ") does not match that of the number of dimensions of the filter (" << numInputDimensions << ")!" << std::endl;
        return VectorFloat();
    }
    
    if( ++inputSampleCounter > filterSize ) inputSampleCounter = filterSize;
    
    //Add the new value to the buffer
    dataBuffer.push_back( x );
    
    //Compute the median value for each dimension
    VectorFloat tmp( inputSampleCounter );
    for(unsigned int j=0; j<numInputDimensions; j++){
        for(unsigned int i=0; i<inputSampleCounter; i++) {
            tmp[i] = dataBuffer[i][j];
        }
        std::sort(tmp.begin(),tmp.end());
        
        //Get the median value
        unsigned int medianIndex = (inputSampleCounter/2);
        processedData[j] = tmp[ medianIndex ];
    }
    
    return processedData;
}

UINT MedianFilter::getFilterSize() const { return filterSize; }
    
VectorFloat MedianFilter::getFilteredData() const { return processedData; }

Vector< VectorFloat > MedianFilter::getDataBuffer() const {
    
    if( !initialized ){
        return Vector< VectorFloat >();
    }
    
    Vector< VectorFloat > data(numInputDimensions,VectorFloat(inputSampleCounter));
    for(unsigned int j=0; j<numInputDimensions; j++){
        for(unsigned int i=0; i<inputSampleCounter; i++){
            data[j][i] = dataBuffer[i][j];
        }
    }
    return data;
}

GRT_END_NAMESPACE
