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
#include "TimeseriesBuffer.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string TimeseriesBuffer::id = "TimeseriesBuffer";
std::string TimeseriesBuffer::getId() { return TimeseriesBuffer::id; }

//Register the TimeseriesBuffer module with the FeatureExtraction base class
RegisterFeatureExtractionModule< TimeseriesBuffer > TimeseriesBuffer::registerModule( TimeseriesBuffer::getId() );

TimeseriesBuffer::TimeseriesBuffer(const UINT bufferSize,const UINT numDimensions) : FeatureExtraction( TimeseriesBuffer::getId() )
{
    init(bufferSize,numDimensions);
}

TimeseriesBuffer::TimeseriesBuffer(const TimeseriesBuffer &rhs) : FeatureExtraction( TimeseriesBuffer::getId() )
{
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

TimeseriesBuffer::~TimeseriesBuffer(){
    
}

TimeseriesBuffer& TimeseriesBuffer::operator=(const TimeseriesBuffer &rhs){
    if(this!=&rhs){
        this->bufferSize = rhs.bufferSize;
        this->dataBuffer = rhs.dataBuffer;
        
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool TimeseriesBuffer::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *dynamic_cast<const TimeseriesBuffer*>(featureExtraction);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool TimeseriesBuffer::computeFeatures(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    update( inputVector );
    
    return true;
}

bool TimeseriesBuffer::reset(){
    if( initialized ){
        return init( bufferSize, numInputDimensions );
    }
    return false;
}

bool TimeseriesBuffer::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_TIMESERIES_BUFFER_FILE_V1.0" << std::endl;
    
    //Save the base settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    //Write the zero crossing counter settings
    file << "BufferSize: " << dataBuffer.getSize() << std::endl;
    
    return true;
}

bool TimeseriesBuffer::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_TIMESERIES_BUFFER_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if( word != "BufferSize:" ){
        errorLog << "load(fstream &file) - Failed to read BufferSize header!" << std::endl;
        return false;
    }
    file >> bufferSize;
    
    //Init the TimeseriesBuffer module to ensure everything is initialized correctly
    return init(bufferSize,numInputDimensions);
}

bool TimeseriesBuffer::init(const UINT bufferSize,const UINT numDimensions){
    
    initialized = false;
    featureDataReady = false;
    
    if( bufferSize == 0 ){
        errorLog << "init(UINT bufferSize,UINT numDimensions) - The bufferSize must be greater than zero!" << std::endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(UINT bufferSize,UINT numDimensions) - The numDimensions must be greater than zero!" << std::endl;
        return false;
    }
    
    //Setup the databuffer
    numInputDimensions = numDimensions;
    numOutputDimensions = bufferSize * numInputDimensions;
    this->bufferSize = bufferSize;
    dataBuffer.resize( bufferSize, VectorFloat(numInputDimensions,0) );
    featureVector.resize(numOutputDimensions,0);
    
    //Flag that the timeseries buffer has been initialized
    initialized = true;
    
    return true;
}


VectorFloat TimeseriesBuffer::update(const Float x){
    return update(VectorFloat(1,x));
}

VectorFloat TimeseriesBuffer::update(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "update(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.getSize() != numInputDimensions ){
        errorLog << "update(const VectorFloat &x)- The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.getSize() << ")!" << std::endl;
        return VectorFloat();
    }
    
    //Add the new data to the buffer
    dataBuffer.push_back( x );
    
    //Search the buffer for the zero crossing features
    UINT colIndex = 0;
    for(UINT j=0; j<numInputDimensions; j++){
        for(UINT i=0; i<dataBuffer.getSize(); i++){
            featureVector[ colIndex++ ] = dataBuffer[i][j];
        }
    }
    
    //Flag that the feature data has been computed
    if( dataBuffer.getBufferFilled() ){
        featureDataReady = true;
    }else featureDataReady = false;
    
    return featureVector;
}

bool TimeseriesBuffer::setBufferSize(UINT bufferSize){
    if( bufferSize > 0 ){
        this->bufferSize = bufferSize;
        if( initialized ) return init(bufferSize, numInputDimensions);
        return true;
    }
    errorLog << "setBufferSize(UINT bufferSize) - The bufferSize must be larger than zero!" << std::endl;
    return false;
}

UINT TimeseriesBuffer::getBufferSize() const {
    if( initialized ) return bufferSize;
    return 0;
}

Vector< VectorFloat > TimeseriesBuffer::getDataBuffer() const {
    if( initialized ) return dataBuffer.getData();
    return Vector< VectorFloat >();
}

GRT_END_NAMESPACE
