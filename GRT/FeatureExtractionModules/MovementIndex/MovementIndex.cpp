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
#include "MovementIndex.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string MovementIndex::id = "MovementIndex";
std::string MovementIndex::getId() { return MovementIndex::id; }

//Register the MovementIndex module with the FeatureExtraction base class
RegisterFeatureExtractionModule< MovementIndex > MovementIndex::registerModule( MovementIndex::getId() );

MovementIndex::MovementIndex(const UINT bufferLength,const UINT numDimensions) : FeatureExtraction( MovementIndex::getId() )
{
    init(bufferLength,numDimensions);
}

MovementIndex::MovementIndex(const MovementIndex &rhs) : FeatureExtraction( MovementIndex::getId() )
{
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

MovementIndex::~MovementIndex(){
    
}

MovementIndex& MovementIndex::operator=(const MovementIndex &rhs){
    if(this!=&rhs){
        this->bufferLength = rhs.bufferLength;
        this->dataBuffer = rhs.dataBuffer;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool MovementIndex::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *dynamic_cast<const MovementIndex*>(featureExtraction);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool MovementIndex::computeFeatures(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.getSize() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    featureVector = update( inputVector );
    
    return true;
}

bool MovementIndex::reset(){
    if( initialized ){
        return init(bufferLength,numInputDimensions);
    }
    return false;
}

bool MovementIndex::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_MOVEMENT_INDEX_FILE_V1.0" << std::endl;
    
    //Save the base settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "save(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    //Write the movement index settings to the file
    file << "BufferLength: " << bufferLength << std::endl;
    
    return true;
}

bool MovementIndex::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_MOVEMENT_INDEX_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "load(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    //Load the BufferLength
    file >> word;
    if( word != "BufferLength:" ){
        errorLog << "load(fstream &file) - Failed to read BufferLength header!" << std::endl;
        return false;
    }
    file >> bufferLength;
    
    //Init the MovementIndex module to ensure everything is initialized correctly
    return init(bufferLength,numInputDimensions);
}

bool MovementIndex::init(const UINT bufferLength,const UINT numDimensions){
    
    initialized = false;
    
    if( bufferLength == 0 ){
        errorLog << "init(...) - The number of bufferLength must be greater than zero!" << std::endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(...) - The number of dimensions must be greater than zero!" << std::endl;
        return false;
    }
    
    this->bufferLength = bufferLength;
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numInputDimensions;
    featureDataReady = false;
    
    //Resize the feature vector
    featureVector.resize(numInputDimensions);
    
    //Resize the raw trajectory data buffer
    dataBuffer.resize( bufferLength, VectorFloat(numInputDimensions,0) );
    
    //Flag that the zero crossing counter has been initialized
    initialized = true;
    
    return true;
}


VectorFloat MovementIndex::update(const Float x){
    return update(VectorFloat(1,x));
}

VectorFloat MovementIndex::update(const VectorFloat &x){
    
    #ifdef GRT_SAFE_CHECKING
    if( !initialized ){
        errorLog << "update(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.getSize() != numInputDimensions ){
        errorLog << "update(const VectorFloat &x)- The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.getSize() << ")!" << std::endl;
        return VectorFloat();
    }
    #endif
    
    //Add the new data to the trajectory data buffer
    dataBuffer.push_back( x );
    
    //Only flag that the feature data is ready if the trajectory data is full
    if( !dataBuffer.getBufferFilled() ){
        featureDataReady = false;
        for(UINT i=0; i<featureVector.getSize(); i++){
            featureVector[i] = 0;
        }
        return featureVector;
    }else featureDataReady = true;
    
    //Compute the movement index (which is the RMS error)
    VectorFloat mu(numInputDimensions,0);
    
    //Compute mu
    for(UINT j=0; j<numInputDimensions; j++){
        for(UINT i=0; i<dataBuffer.getSize(); i++){
            mu[j] += dataBuffer[i][j];
        }
        mu[j] /= Float(dataBuffer.getSize());
    }
    
    for(UINT j=0; j<numInputDimensions; j++){
        featureVector[j] = 0;
        for(UINT i=0; i<dataBuffer.getSize(); i++){
            featureVector[j] += grt_sqr( dataBuffer[i][j] - mu[j] );
        }
        featureVector[j] = grt_sqrt( featureVector[j]/Float(dataBuffer.getSize()) );
    }
    
    return featureVector;
}

CircularBuffer< VectorFloat > MovementIndex::getData() const{
    if( initialized ){
        return dataBuffer;
    }
    return CircularBuffer< VectorFloat >();
}

GRT_END_NAMESPACE
