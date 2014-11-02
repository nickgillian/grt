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

#include "FFT.h"

namespace GRT{
    
//Register the FFT module with the FeatureExtraction base class
RegisterFeatureExtractionModule< FFT > FFT::registerModule("FFT");

FFT::FFT(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase){

    classType = "FFT";
    featureExtractionType = classType;
    
    initialized = false; 
    featureDataReady = false;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    
    infoLog.setProceedingText("[FFT]");
    warningLog.setProceedingText("[WARNING FFT]");
    errorLog.setProceedingText("[ERROR FFT]");
    
    if( isPowerOfTwo(fftWindowSize) && hopSize > 0 && numDimensions > 0 ){
        init(fftWindowSize,hopSize,numDimensions,fftWindowFunction,computeMagnitude,computePhase);
    }
}
    
FFT::FFT(const FFT &rhs){
    infoLog.setProceedingText("[FFT]");
    warningLog.setProceedingText("[WARNING FFT]");
    errorLog.setProceedingText("[ERROR FFT]");

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
    
    if( this->getFeatureExtractionType() == featureExtraction->getFeatureExtractionType() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *(FFT*)featureExtraction;
        
        return true;
    }
    
    errorLog << "clone(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << endl;
    
    return false;
}

bool FFT::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_FFT_FILE_V1.0" << endl;
    
    //Save the base settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << endl;
        return false;
    }
    
    //Write the FFT settings
    file << "HopSize: " << hopSize << endl;
    file << "FftWindowSize: " << fftWindowSize << endl;
    file << "FftWindowFunction: " << fftWindowFunction << endl;
    file << "ComputeMagnitude: " << computeMagnitude << endl;
    file << "ComputePhase: " << computePhase << endl;
    
    return true;
}

bool FFT::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_FFT_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if( word != "HopSize:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read HopSize header!" << endl;
        return false;     
    }
    file >> hopSize;
    
    file >> word;
    if( word != "FftWindowSize:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read FftWindowSize header!" << endl;
        return false;     
    }
    file >> fftWindowSize;
    
    file >> word;
    if( word != "FftWindowFunction:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read FftWindowFunction header!" << endl;
        return false;     
    }
    file >> fftWindowFunction;
    
    file >> word;
    if( word != "ComputeMagnitude:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read ComputeMagnitude header!" << endl;
        return false;     
    }
    file >> computeMagnitude;
    
    file >> word;
    if( word != "ComputePhase:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read ComputePhase header!" << endl;
        return false;     
    }
    file >> computePhase;
    
    //Init the FFT module to ensure everything is initialized correctly
    return init(fftWindowSize,hopSize,numInputDimensions,fftWindowFunction,computeMagnitude,computePhase);
}

bool FFT::init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase){
    
    //Clear any previous setup
    clear();
    
    if( !isPowerOfTwo(fftWindowSize) ){
        errorLog << "init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase) - fftWindowSize is not a power of two!" << endl;
        return false;
    }
    
    if( !validateFFTWindowFunction( fftWindowFunction ) ){
        errorLog << "init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase) - Unkown Window Function!" << endl;
        return false;
    }
       
    this->dataBufferSize = fftWindowSize;
    this->fftWindowSize = fftWindowSize;
    this->hopSize = hopSize;
    this->fftWindowFunction = fftWindowFunction;
    this->computeMagnitude = computeMagnitude;
    this->computePhase = computePhase;
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
            errorLog << "init(UINT fftWindowSize,UINT hopSize,UINT numDimensions,UINT fftWindowFunction,bool computeMagnitude,bool computePhase) - Failed to initialize fft!" << endl;
            clear();
            return false;
        }
    }
    
    initialized = true;

    return true;
}
    
bool FFT::computeFeatures(const VectorDouble &inputVector){

    if( !initialized ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the FeatureExtraction (" << numInputDimensions << ")!" << endl;
        return false;
    }
    
    return update(inputVector);
}
    
bool FFT::update(const double x){

    if( !initialized ){
        errorLog << "update(const double x) - Not initialized!" << endl;
        return false;
    }
    
    if( numInputDimensions != 1 ){
        errorLog << "update(const double x) - The size of the input (1) does not match that of the FeatureExtraction (" << numInputDimensions << ")!" << endl;
        return false;
    }
    
    return update(VectorDouble(1,x));
}

bool FFT::update(const VectorDouble &x){

    if( !initialized ){
        errorLog << "update(const VectorDouble &x) - Not initialized!" << endl;
        return false;
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "update(const VectorDouble &x) - The size of the input (" << x.size() << ") does not match that of the FeatureExtraction (" << numInputDimensions << ")!" << endl;
        return false;
    }

    //Add the current input to the data buffers
    dataBuffer.push_back(x);
    
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
                errorLog << "update(const VectorDouble &x) - Failed to compute FFT!" << endl;
                return false;
            }
        }
        
        //Flag that the fft was computed during this update
        featureDataReady = true;
        
        //Copy the FFT data to the feature vector
        UINT index = 0;
        for(UINT j=0; j<numInputDimensions; j++){
            if( computeMagnitude ){
                double *mag = fft[j].getMagnitudeDataPtr();
                for(UINT i=0; i<fft[j].getFFTSize()/2; i++){
                    featureVector[index++] = *mag++;
                }
            }
            if( computePhase ){
                double *phase = fft[j].getPhaseDataPtr();
                for(UINT i=0; i<fft[j].getFFTSize()/2; i++){
                    featureVector[index++] = *phase++;
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
    if( initialized ) return init(fftWindowSize,hopSize,numInputDimensions,fftWindowFunction,computeMagnitude,computePhase);
    return false;
}
    
UINT FFT::getHopSize(){ 
    if(initialized){ return hopSize; } 
    return 0; 
}
    
UINT FFT::getDataBufferSize(){ 
    if(initialized){ return dataBufferSize; } 
    return 0; 
}
    
UINT FFT::getFFTWindowSize(){ 
    
    if( !initialized ) return 0;
    return fftWindowSize;
}
    
UINT FFT::getFFTWindowFunction(){ 
    if(initialized){ return fftWindowFunction; } 
    return 0; 
}
    
UINT FFT::getHopCounter(){ 
    if(initialized){ return hopCounter; } 
    return 0; 
}
    
VectorDouble FFT::getFrequencyBins(const unsigned int sampleRate){
    if( !initialized ){ return VectorDouble(); }
    
    VectorDouble freqBins( fftWindowSize );
    for(unsigned int i=0; i<fftWindowSize; i++){
        freqBins[i] = (i*sampleRate) / fftWindowSize;
    }
    return freqBins;
}
    
bool FFT::setHopSize(UINT hopSize){
    if( hopSize > 0 ){
        this->hopSize = hopSize;
        hopCounter = 0;
        return true;
    }
    errorLog << "setHopSize(UINT hopSize) - The hopSize value must be greater than zero!" << endl;
    return false;
}

bool FFT::setFFTWindowSize(UINT fftWindowSize){
    if( isPowerOfTwo(fftWindowSize) ){
        if( initialized ) return init(fftWindowSize, hopSize, numInputDimensions, fftWindowFunction, computeMagnitude, computePhase);
        this->fftWindowSize = fftWindowSize;
        return true;
    }
    errorLog << "setFFTWindowSize(UINT fftWindowSize) - fftWindowSize must be a power of two!" << endl;
    return false;

}
    
bool FFT::setFFTWindowFunction(UINT fftWindowFunction){
    if( validateFFTWindowFunction( fftWindowFunction ) ){
        this->fftWindowFunction = fftWindowFunction;
        return true;
    }
    return false;
}
    
bool FFT::setComputeMagnitude(bool computeMagnitude){
    if( initialized ) return init(fftWindowSize, hopSize, numInputDimensions, fftWindowFunction, computeMagnitude, computePhase);
    this->computeMagnitude = computeMagnitude;
    return true;
}
    
bool FFT::setComputePhase(bool computePhase){
    if( initialized ) return init(fftWindowSize, hopSize, numInputDimensions, fftWindowFunction, computeMagnitude, computePhase);
    this->computePhase = computePhase;
    return true;

}

bool FFT::isPowerOfTwo(unsigned int x){
    if (x < 2) return false;
    if (x & (x - 1)) return false;
    return true;
}
    
bool FFT::validateFFTWindowFunction(UINT fftWindowFunction){
    if( fftWindowFunction != RECTANGULAR_WINDOW && fftWindowFunction != BARTLETT_WINDOW && 
        fftWindowFunction != HAMMING_WINDOW && fftWindowFunction != HANNING_WINDOW ){
        return false;
    }
    return true;
}
    
    
}//End of namespace GRT
