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
#include "TimeDomainFeatures.h"

GRT_BEGIN_NAMESPACE
    
//Register the TimeDomainFeatures module with the FeatureExtraction base class
RegisterFeatureExtractionModule< TimeDomainFeatures > TimeDomainFeatures::registerModule("TimeDomainFeatures");
    
TimeDomainFeatures::TimeDomainFeatures(UINT bufferLength,UINT numFrames,UINT numDimensions,bool offsetInput,bool useMean,bool useStdDev,bool useEuclideanNorm,bool useRMS){

    classType = "TimeDomainFeatures";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG TimeDomainFeatures]");
    errorLog.setProceedingText("[ERROR TimeDomainFeatures]");
    warningLog.setProceedingText("[WARNING TimeDomainFeatures]");
    
    init(bufferLength,numFrames,numDimensions,offsetInput,useMean,useStdDev,useEuclideanNorm,useRMS);
}
    
TimeDomainFeatures::TimeDomainFeatures(const TimeDomainFeatures &rhs){
    
    classType = "TimeDomainFeatures";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG TimeDomainFeatures]");
    errorLog.setProceedingText("[ERROR TimeDomainFeatures]");
    warningLog.setProceedingText("[WARNING TimeDomainFeatures]");
    
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

TimeDomainFeatures::~TimeDomainFeatures(){

}
    
TimeDomainFeatures& TimeDomainFeatures::operator=(const TimeDomainFeatures &rhs){
    if(this!=&rhs){
        this->bufferLength = rhs.bufferLength;
        this->numFrames = rhs.numFrames;
        this->offsetInput = rhs.offsetInput;
        this->useMean = rhs.useMean;
        this->useStdDev = rhs.useStdDev;
        this->useEuclideanNorm = rhs.useEuclideanNorm;
        this->useRMS = rhs.useRMS;
        this->dataBuffer = rhs.dataBuffer;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}
    
bool TimeDomainFeatures::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getFeatureExtractionType() == featureExtraction->getFeatureExtractionType() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *(TimeDomainFeatures*)featureExtraction;
        
        return true;
    }
    
    errorLog << "clone(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}
    
bool TimeDomainFeatures::computeFeatures(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    featureVector = update( inputVector );
    
    return true;
}

bool TimeDomainFeatures::reset(){
    if( initialized ){
        return init(bufferLength,numFrames,numInputDimensions,offsetInput,useMean,useStdDev,useEuclideanNorm,useRMS);
    }
    return false;
}
    
bool TimeDomainFeatures::saveModelToFile( std::string filename ) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool TimeDomainFeatures::loadModelFromFile( std::string filename ){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool TimeDomainFeatures::saveModelToFile( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_TIME_DOMAIN_FEATURES_FILE_V1.0" << std::endl;	
    
    //Save the base settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    //Write the time domain settings to the file
    file << "BufferLength: " << bufferLength << std::endl;
    file << "NumFrames: " << numFrames << std::endl;
    file << "OffsetInput: " << offsetInput << std::endl;
    file << "UseMean: " << useMean << std::endl;
    file << "UseStdDev: " << useStdDev << std::endl;
    file << "UseEuclideanNorm: " << useEuclideanNorm << std::endl;
    file << "UseRMS: " << useRMS << std::endl;
    
    return true;
}

bool TimeDomainFeatures::loadModelFromFile( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_TIME_DOMAIN_FEATURES_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << std::endl;
        return false;     
    }
    
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    //Load the BufferLength
    file >> word;
    if( word != "BufferLength:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read BufferLength header!" << std::endl;
        return false;     
    }
    file >> bufferLength;
    
    //Load the NumFrames
    file >> word;
    if( word != "NumFrames:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumFrames header!" << std::endl;
        return false;     
    }
    file >> numFrames;
    
    //Load the OffsetInput
    file >> word;
    if( word != "OffsetInput:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read OffsetInput header!" << std::endl;
        return false;     
    }
    file >> offsetInput;
    
    //Load the UseMean
    file >> word;
    if( word != "UseMean:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read UseMean header!" << std::endl;
        return false;     
    }
    file >> useMean;
    
    //Load the UseStdDev
    file >> word;
    if( word != "UseStdDev:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read UseStdDev header!" << std::endl;
        return false;     
    }
    file >> useStdDev;
    
    //Load the UseEuclideanNorm
    file >> word;
    if( word != "UseEuclideanNorm:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read UseEuclideanNorm header!" << std::endl;
        return false;     
    }
    file >> useEuclideanNorm;
    
    //Load the UseRMS
    file >> word;
    if( word != "UseRMS:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read UseRMS header!" << std::endl;
        return false;
    }
    file >> useRMS;
    
    //Init the TimeDomainFeatures module to ensure everything is initialized correctly
    return init(bufferLength,numFrames,numInputDimensions,offsetInput,useMean,useStdDev,useEuclideanNorm,useRMS);
}
    
bool TimeDomainFeatures::init(UINT bufferLength,UINT numFrames,UINT numDimensions,bool offsetInput,bool useMean,bool useStdDev,bool useEuclideanNorm,bool useRMS){
    
    initialized = false;
    
    if( numFrames > bufferLength ){
        errorLog << "init(...) - The number of numFrames parameter can not be larger than the buffer length parameter!" << std::endl;
        return false;
    }
    if( bufferLength % numFrames != 0 ){
        errorLog << "init(...) - The buffer length parameter must be divisible with no remainders by the number of numFrames parameter!" << std::endl;
        return false;
    }
    
    this->bufferLength = bufferLength;
    this->numFrames = numFrames;
    this->numInputDimensions = numDimensions;
    this->offsetInput = offsetInput;
    this->useMean = useMean;
    this->useStdDev = useStdDev;
    this->useEuclideanNorm = useEuclideanNorm;
    this->useRMS = useRMS;
    featureDataReady = false;
    
    //Set the number of output dimensions
    numOutputDimensions = 0;
    if( useMean ){
        numOutputDimensions += numInputDimensions*numFrames;
    }
    if( useStdDev ){
        numOutputDimensions += numInputDimensions*numFrames;
    }
    if( useEuclideanNorm ){
        numOutputDimensions += numInputDimensions*numFrames;
    }
    if( useRMS ){
        numOutputDimensions += numInputDimensions*numFrames;
    }
    if( numOutputDimensions == 0 ){
        errorLog << "init(...) - The numOutputDimensions is zero!" << std::endl;
        return false;
    }
    
    //Resize the feature vector
    featureVector.resize(numOutputDimensions);
    
    //Resize the raw data buffer
    dataBuffer.resize( bufferLength, VectorFloat(numInputDimensions,0) );

    //Flag that the time domain features has been initialized
    initialized = true;
    
    return true;
}


VectorFloat TimeDomainFeatures::update(Float x){
	return update(VectorFloat(1,x));
}
    
VectorFloat TimeDomainFeatures::update(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "update(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.getSize() != numInputDimensions ){
        errorLog << "update(const VectorFloat &x)- The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.getSize() << ")!" << std::endl;
        return VectorFloat();
    }
    
    //Add the new data to the data buffer
    dataBuffer.push_back( x );
    
    //Only flag that the feature data is ready if the data is full
    if( dataBuffer.getBufferFilled() ){
        featureDataReady = true;
    }else featureDataReady = false;
    
    MatrixFloat meanFeatures(numInputDimensions,numFrames);
    MatrixFloat stdDevFeatures(numInputDimensions,numFrames);
    MatrixFloat normFeatures(numInputDimensions,numFrames);
    MatrixFloat rmsFeatures(numInputDimensions,numFrames);
    MatrixFloat data(bufferLength,numInputDimensions);
    
    if( offsetInput ){
        for(UINT n=0; n<numInputDimensions; n++){
            data[0][n] = dataBuffer[0][n];
            for(UINT i=1; i<bufferLength; i++){
                data[i][n] = dataBuffer[i][n]-dataBuffer[0][n];
            }
        }
    }else{
        for(UINT n=0; n<numInputDimensions; n++){
            for(UINT i=0; i<bufferLength; i++){
                data[i][n] = dataBuffer[i][n];
            }
        }
    }
    
    if( useMean || useStdDev ){ meanFeatures.setAllValues(0); stdDevFeatures.setAllValues(0); }
    if( useEuclideanNorm ) normFeatures.setAllValues(0);
    if( useRMS ) rmsFeatures.setAllValues(0);
    
    UINT frameSize = bufferLength / numFrames;
    UINT frame = 0;
    UINT index = 0;
    for(UINT n=0; n<numInputDimensions; n++){
        frame = 0;
        index = 0;
        for(UINT i=0; i<bufferLength; i++){
            //Update the mean
            meanFeatures[n][frame] += data[i][n];
            
            //Update the norm features
            if( useEuclideanNorm )
                normFeatures[n][frame] += data[i][n]*data[i][n];
            
            //Update the rms features
            if( useRMS )
                rmsFeatures[n][frame] += data[i][n]*data[i][n];
            
            if( ++index == frameSize ){
                frame++;
                index = 0;
            }
        }
        
        //Update the mean
        for(UINT j=0; j<numFrames; j++){
            meanFeatures[n][j] /= frameSize;
        }
        
        //Update the std dev if needed
        if( useStdDev ){
            frame = 0;
            index = 0;
            for(UINT i=0; i<bufferLength; i++){
                stdDevFeatures[n][frame] += (data[i][n]-meanFeatures[n][frame]) * (data[i][n]-meanFeatures[n][frame]);
                if( ++index == frameSize ){
                    frame++;
                    index = 0;
                }
            }
            Float norm = frameSize>1 ? frameSize-1 : 1;
            for(UINT j=0; j<numFrames; j++){
                stdDevFeatures[n][j] = sqrt( stdDevFeatures[n][j]/norm );
            }
        }
        
        //Update the euclidean norm if needed
        if( useEuclideanNorm ){
            for(UINT j=0; j<numFrames; j++){
                normFeatures[n][j] = sqrt( normFeatures[n][j] );
            }
        }
        
        //Update the rms if needed
        if( useRMS ){
            for(UINT j=0; j<numFrames; j++){
                rmsFeatures[n][j] = sqrt( rmsFeatures[n][j] / frameSize );
            }
        }
    }
    
    //Update the features
    index = 0;
    frame = 0;
    for(UINT n=0; n<numInputDimensions; n++){
        for(UINT j=0; j<numFrames; j++){
            if( useMean ){
                featureVector[index++] = meanFeatures[n][j];
            }
            if( useStdDev ){
                featureVector[index++] = stdDevFeatures[n][j];
            }
            if( useEuclideanNorm ){
                featureVector[index++] = normFeatures[n][j];
            }
            if( useRMS ){
                featureVector[index++] = rmsFeatures[n][j];
            }
        }
    }
    
    return featureVector;
}
    
CircularBuffer< VectorFloat > TimeDomainFeatures::getBufferData(){
    if( initialized ){
        return dataBuffer;
    }
    return CircularBuffer< VectorFloat >();
}
    
const CircularBuffer< VectorFloat > &TimeDomainFeatures::getBufferData() const {
    return dataBuffer;
}
    
GRT_END_NAMESPACE
