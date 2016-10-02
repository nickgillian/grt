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
#include "EnvelopeExtractor.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string EnvelopeExtractor::id = "EnvelopeExtractor";
std::string EnvelopeExtractor::getId() { return EnvelopeExtractor::id; }

//Register your module with the FeatureExtraction base class
RegisterFeatureExtractionModule< EnvelopeExtractor > EnvelopeExtractor::registerModule( EnvelopeExtractor::getId() );

EnvelopeExtractor::EnvelopeExtractor( const UINT bufferSize,const UINT numDimensions ) : FeatureExtraction( EnvelopeExtractor::getId() )
{
    this->bufferSize = 0;
    
    if( bufferSize > 0 && numDimensions > 0 ){
        init( bufferSize, numDimensions );
    }
}

EnvelopeExtractor::EnvelopeExtractor(const EnvelopeExtractor &rhs) : FeatureExtraction( EnvelopeExtractor::getId() )
{
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

EnvelopeExtractor::~EnvelopeExtractor(){
}

EnvelopeExtractor& EnvelopeExtractor::operator=(const EnvelopeExtractor &rhs){
    
    if(this!=&rhs){
        //Here you should copy any class variables from the rhs instance to this instance
        this->bufferSize = rhs.bufferSize;
        this->buffer = rhs.buffer;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool EnvelopeExtractor::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //Cast the feature extraction pointer to a pointer to your custom feature extraction module
        //Then invoke the equals operator
        *this = *dynamic_cast<const EnvelopeExtractor*>(featureExtraction);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool EnvelopeExtractor::computeFeatures(const VectorFloat &inputVector){
    
    if( !initialized ) return false;
    
    //Add the value to the buffer
    buffer.push_back( inputVector );
    
    UINT count = buffer.getNumValuesInBuffer();
    for(UINT j=0; j<numInputDimensions; j++){
        featureVector[j] = 0;
        for(UINT i=0; i<count; i++){
            featureVector[j] += SQR( buffer[i][j] );
        }
        featureVector[j] = sqrt( featureVector[j] / count );
    }
    
    //Flag that the feature data is ready
    featureDataReady = true;
    
    return true;
}

bool EnvelopeExtractor::reset(){
    return true;
}

bool EnvelopeExtractor::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //First, you should add a header (with no spaces) e.g.
    file << "ENVELOPE_EXTRACTOR_FILE_V1.0" << std::endl;
    
    //Second, you should save the base feature extraction settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "save(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    file << "BufferSize: " << bufferSize << std::endl;
    
    return true;
}

bool EnvelopeExtractor::load( std::fstream &file ){
    
    clear();
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    UINT numLayers = 0;
    UINT numRows = 0;
    UINT numCols = 0;
    
    //First, you should read and validate the header
    file >> word;
    if( word != "ENVELOPE_EXTRACTOR_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    //Second, you should load the base feature extraction settings to the file
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    //Load the buffer size header
    file >> word;
    if( word != "BufferSize:" ){
        errorLog << "load(fstream &file) - Failed to read BufferSize header!" << std::endl;
        return false;
    }
    file >> bufferSize;
    
    return init( bufferSize, numInputDimensions );
}

bool EnvelopeExtractor::init( const UINT bufferSize, const UINT numDimensions ){
    
    clear();
    
    if( bufferSize == 0 || numDimensions == 0 ) return false;
    
    this->bufferSize = bufferSize;
    numInputDimensions = numDimensions;
    numOutputDimensions = numDimensions;
    buffer.resize( bufferSize, VectorFloat(numDimensions,0) );
    
    //Call the feature extraction base class init function to setup the feature extraction buffers
    return FeatureExtraction::init();
}

GRT_END_NAMESPACE
