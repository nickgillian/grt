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
#include "FFT.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string FFT::id = "FFT";
std::string FFT::getId() { return FFT::id; }

//Register the FFT module with the FeatureExtraction base class
RegisterFeatureExtractionModule< FFT > FFT::registerModule("FFT");

FFT::FFT(const UINT fftWindowSize,const UINT hopSize,const UINT numDimensions,const UINT fftWindowFunction,const bool computeMagnitude,const bool computePhase) : FeatureExtraction( FFT::getId() )
{
    
    initialized = false;
    featureDataReady = false;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    
    if( isPowerOfTwo(fftWindowSize) && hopSize > 0 && numDimensions > 0 ){
        init(fftWindowSize,hopSize,numDimensions,fftWindowFunction,computeMagnitude,computePhase);
    }
}

FFT::FFT(const FFT &rhs) : FeatureExtraction( FFT::getId() )
{
    
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

FFT::~FFT(void){
}

FFT& FFT::operator=(const FFT &rhs){
    
    if( this != &rhs ){
        this->hopSize = rhs.hopSize;
        this->dataBufferSize = rhs.dataBufferSize;
        this->fftWindowSize = rhs.fftWindowSize;
        this->fftWindowFunction = rhs.fftWindowFunction;
        this->hopCounter = rhs.hopCounter;
        this->computeMagnitude = rhs.computeMagnitude;
        this->computePhase = rhs.computePhase;
        this->dataBuffer = rhs.dataBuffer;
        this->tempBuffer = rhs.tempBuffer;
        this->fft = rhs.fft;
        this->windowSizeMap = rhs.windowSizeMap;
        
        copyBaseVariables( (FeatureExtraction*)&rhs );
        
        for(UINT i=0; i<dataBufferSize; i++){
            dataBuffer[i].resize( numInputDimensions, 0 );
        }
    }
    return *this;
}

//Clone method
bool FFT::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *dynamic_cast<const FFT*>(featureExtraction);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool FFT::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_FFT_FILE_V1.0" << std::endl;
    
    //Save the base settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    //Write the FFT settings
    file << "HopSize: " << hopSize << std::endl;
    file << "FftWindowSize: " << fftWindowSize << std::endl;
    file << "FftWindowFunction: " << fftWindowFunction << std::endl;
    file << "ComputeMagnitude: " << computeMagnitude << std::endl;
    file << "ComputePhase: " << computePhase << std::endl;
    
    return true;
}

bool FFT::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_FFT_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if( word != "HopSize:" ){
        errorLog << "load(fstream &file) - Failed to read HopSize header!" << std::endl;
        return false;
    }
    file >> hopSize;
    
    file >> word;
    if( word != "FftWindowSize:" ){
        errorLog << "load(fstream &file) - Failed to read FftWindowSize header!" << std::endl;
        return false;
    }
    file >> fftWindowSize;
    
    file >> word;
    if( word != "FftWindowFunction:" ){
        errorLog << "load(fstream &file) - Failed to read FftWindowFunction header!" << std::endl;
        return false;
    }
    file >> fftWindowFunction;
    
    file >> word;
    if( word != "ComputeMagnitude:" ){
        errorLog << "load(fstream &file) - Failed to read ComputeMagnitude header!" << std::endl;
        return false;
    }
    file >> computeMagnitude;
    
    file >> word;
    if( word != "ComputePhase:" ){
        errorLog << "load(fstream &file) - Failed to read ComputePhase header!" << std::endl;
        return false;
    }
    file >> computePhase;
    
    //Init the FFT module to ensure everything is initialized correctly
    return init(fftWindowSize,hopSize,numInputDimensions,fftWindowFunction,computeMagnitude,computePhase);
}

bool FFT::init(const UINT fftWindowSize,const UINT hopSize,const UINT numDimensions,const UINT fftWindowFunction,const bool computeMagnitude,const bool computePhase,const DataType inputType,const DataType outputType){
    
    //Clear any previous setup
    clear();
    
    if( !isPowerOfTwo(fftWindowSize) ){
        errorLog << "init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase) - fftWindowSize is not a power of two!" << std::endl;
        return false;
    }
    
    if( !validateFFTWindowFunction( fftWindowFunction ) ){
        errorLog << "init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase) - Unkown Window Function!" << std::endl;
        return false;
    }
    
    this->dataBufferSize = fftWindowSize;
    this->fftWindowSize = fftWindowSize;
    this->hopSize = hopSize;
    this->fftWindowFunction = fftWindowFunction;
    this->computeMagnitude = computeMagnitude;
    this->computePhase = computePhase;
    this->inputType = inputType;
    this->outputType = outputType;
    hopCounter = 0;
    featureDataReady = false;
    numInputDimensions = numDimensions;
    
    //Set the output size, the fftWindowSize is divided by 2 because the FFT is symmetrical so only half the actual FFT is returned
    numOutputDimensions = 0;
    if( computePhase ) numOutputDimensions += fftWindowSize/2 * numInputDimensions;
    if( computeMagnitude ) numOutputDimensions += fftWindowSize/2 * numInputDimensions;
    
    //Resize the output feature vector
    featureVector.resize( numOutputDimensions, 0);
    
    dataBuffer.resize( dataBufferSize );
    tempBuffer.resize( dataBufferSize );
    
    for(UINT i=0; i<dataBufferSize; i++){
        dataBuffer[i].resize( numInputDimensions, 0 );
    }
    
    for(UINT i=0; i<dataBufferSize; i++){
        dataBuffer[i].resize( numInputDimensions, 0 );
    }
    
    //Setup the fft for each dimension
    fft.resize(numInputDimensions);
    for(unsigned int i=0; i<numInputDimensions; i++){
        if( !fft[i].init(fftWindowSize,fftWindowFunction,computeMagnitude,computePhase) ){
            errorLog << "init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase) - Failed to initialize fft!" << std::endl;
            clear();
            return false;
        }
    }
    
    initialized = true;
    
    return true;
}

bool FFT::computeFeatures(const VectorFloat &inputVector){
    return update( inputVector );
}

bool FFT::computeFeatures(const MatrixFloat &inputMatrix){
    return update( inputMatrix );
}

bool FFT::update(const Float x){
    
    if( !initialized ){
        errorLog << "update(const Float x) - Not initialized!" << std::endl;
        return false;
    }
    
    if( numInputDimensions != 1 ){
        errorLog << "update(const Float x) - The size of the input (1) does not match that of the FeatureExtraction (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    return update(VectorFloat(1,x));
}

bool FFT::update(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "update(const VectorFloat &x) - Not initialized!" << std::endl;
        return false;
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "update(const VectorFloat &x) - The size of the input (" << x.size() << ") does not match that of the FeatureExtraction (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    //Add the current input to the data buffers
    dataBuffer.push_back( x );
    
    featureDataReady = false;
    
    if( ++hopCounter == hopSize ){
        hopCounter = 0;
        //Compute the FFT for each dimension
        for(UINT j=0; j<numInputDimensions; j++){
            
            //Copy the input data for this dimension into the temp buffer
            for(UINT i=0; i<dataBufferSize; i++){
                tempBuffer[i] = dataBuffer[i][j];
            }
            
            //Compute the FFT
            if( !fft[j].computeFFT( tempBuffer ) ){
                errorLog << "update(const VectorFloat &x) - Failed to compute FFT!" << std::endl;
                return false;
            }
        }
        
        //Flag that the fft was computed during this update
        featureDataReady = true;
        
        //Copy the FFT data to the feature vector
        UINT index = 0;
        for(UINT j=0; j<numInputDimensions; j++){
            if( computeMagnitude ){
                Float *mag = fft[j].getMagnitudeDataPtr();
                for(UINT i=0; i<fft[j].getFFTSize()/2; i++){
                    featureVector[index++] = *mag++;
                }
            }
            if( computePhase ){
                Float *phase = fft[j].getPhaseDataPtr();
                for(UINT i=0; i<fft[j].getFFTSize()/2; i++){
                    featureVector[index++] = *phase++;
                }
            }
        }
    }
    
    return true;
}

bool FFT::update(const MatrixFloat &x){
    
    if( !initialized ){
        errorLog << "update(const MatrixFloat &x) - Not initialized!" << std::endl;
        return false;
    }
    
    if( x.getNumCols() != numInputDimensions ){
        errorLog << "update(const MatrixFloat &x) - The number of columns in the inputMatrix (" << x.getNumCols() << ") does not match that of the FeatureExtraction (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    featureDataReady = false;
    
    for(UINT k=0; k<x.getNumRows(); k++){
        
        //Add the current input to the data buffers
        dataBuffer.push_back( x.getRow(k) );
        
        if( ++hopCounter == hopSize ){
            hopCounter = 0;
            //Compute the FFT for each dimension
            for(UINT j=0; j<numInputDimensions; j++){
                
                //Copy the input data for this dimension into the temp buffer
                for(UINT i=0; i<dataBufferSize; i++){
                    tempBuffer[i] = dataBuffer[i][j];
                }
                
                //Compute the FFT
                if( !fft[j].computeFFT( tempBuffer ) ){
                    errorLog << "update(const VectorFloat &x) - Failed to compute FFT!" << std::endl;
                    return false;
                }
            }
            
            //Flag that the fft was computed during this update
            featureDataReady = true;
            
            //Copy the FFT data to the feature vector
            UINT index = 0;
            for(UINT j=0; j<numInputDimensions; j++){
                if( computeMagnitude ){
                    Float *mag = fft[j].getMagnitudeDataPtr();
                    for(UINT i=0; i<fft[j].getFFTSize()/2; i++){
                        featureVector[index++] = *mag++;
                    }
                }
                if( computePhase ){
                    Float *phase = fft[j].getPhaseDataPtr();
                    for(UINT i=0; i<fft[j].getFFTSize()/2; i++){
                        featureVector[index++] = *phase++;
                    }
                }
            }
        }
    }
    
    return true;
}

bool FFT::clear(){
    
    //Clear the base class
    FeatureExtraction::clear();
    
    //Clear the buffers
    tempBuffer.clear();
    dataBuffer.clear();
    fft.clear();
    
    return true;
}

bool FFT::reset(){
    if( initialized ) return init(fftWindowSize,hopSize,numInputDimensions,fftWindowFunction,computeMagnitude,computePhase,inputType,outputType);
    return false;
}

UINT FFT::getHopSize() const {
    if(initialized){ return hopSize; }
    return 0;
}

UINT FFT::getDataBufferSize() const {
    if(initialized){ return dataBufferSize; }
    return 0;
}

UINT FFT::getFFTWindowSize() const {
    
    if( !initialized ) return 0;
    return fftWindowSize;
}

UINT FFT::getFFTWindowFunction() const {
    if(initialized){ return fftWindowFunction; }
    return 0;
}

UINT FFT::getHopCounter() const {
    if(initialized){ return hopCounter; }
    return 0;
}

VectorFloat FFT::getFrequencyBins(const unsigned int sampleRate) const {
    if( !initialized ){ return VectorFloat(); }
    
    VectorFloat freqBins( fftWindowSize );
    for(unsigned int i=0; i<fftWindowSize; i++){
        freqBins[i] = (i*sampleRate) / fftWindowSize;
    }
    return freqBins;
}

bool FFT::setHopSize(const UINT hopSize){
    if( hopSize > 0 ){
        this->hopSize = hopSize;
        hopCounter = 0;
        return true;
    }
    errorLog << "setHopSize(UINT hopSize) - The hopSize value must be greater than zero!" << std::endl;
    return false;
}

bool FFT::setFFTWindowSize(const UINT fftWindowSize){
    if( isPowerOfTwo(fftWindowSize) ){
        if( initialized ) return init(fftWindowSize, hopSize, numInputDimensions, fftWindowFunction, computeMagnitude, computePhase);
        this->fftWindowSize = fftWindowSize;
        return true;
    }
    errorLog << "setFFTWindowSize(UINT fftWindowSize) - fftWindowSize must be a power of two!" << std::endl;
    return false;
    
}

bool FFT::setFFTWindowFunction(const UINT fftWindowFunction){
    if( validateFFTWindowFunction( fftWindowFunction ) ){
        this->fftWindowFunction = fftWindowFunction;
        return true;
    }
    return false;
}

bool FFT::setComputeMagnitude(const bool computeMagnitude){
    if( initialized ) return init(fftWindowSize, hopSize, numInputDimensions, fftWindowFunction, computeMagnitude, computePhase);
    this->computeMagnitude = computeMagnitude;
    return true;
}

bool FFT::setComputePhase(const bool computePhase){
    if( initialized ) return init(fftWindowSize, hopSize, numInputDimensions, fftWindowFunction, computeMagnitude, computePhase);
    this->computePhase = computePhase;
    return true;
    
}

bool FFT::isPowerOfTwo(const unsigned int x){
    if (x < 2) return false;
    if (x & (x - 1)) return false;
    return true;
}

bool FFT::validateFFTWindowFunction(const UINT fftWindowFunction){
    if( fftWindowFunction != RECTANGULAR_WINDOW && fftWindowFunction != BARTLETT_WINDOW &&
    fftWindowFunction != HAMMING_WINDOW && fftWindowFunction != HANNING_WINDOW ){
        return false;
    }
    return true;
}

GRT_END_NAMESPACE
