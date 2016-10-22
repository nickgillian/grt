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
#include "FFTFeatures.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string FFTFeatures::id = "FFTFeatures";
std::string FFTFeatures::getId() { return FFTFeatures::id; }
    
//Register the FFTFeatures module with the FeatureExtraction base class
RegisterFeatureExtractionModule< FFTFeatures > FFTFeatures::registerModule("FFTFeatures");
    
bool sortIndexDoubleDecendingValue(IndexedDouble i,IndexedDouble j) { return (i.value<j.value); }
    
FFTFeatures::FFTFeatures(const UINT fftWindowSize,const UINT numChannelsInFFTSignal,const bool computeMaxFreqFeature,const bool computeMaxFreqSpectrumRatio,const bool computeCentroidFeature, const bool computeTopNFreqFeatures,const UINT N) : FeatureExtraction( FFTFeatures::getId() )
{ 
    initialized = false; 
    featureDataReady = false;
    
    init(fftWindowSize,numChannelsInFFTSignal,computeMaxFreqFeature,computeMaxFreqSpectrumRatio,computeCentroidFeature,computeTopNFreqFeatures,N);
}
    
FFTFeatures::FFTFeatures(const FFTFeatures &rhs) : FeatureExtraction( FFTFeatures::getId() )
{
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}
    
FFTFeatures::~FFTFeatures(void){
    
}
    
FFTFeatures& FFTFeatures::operator=(const FFTFeatures &rhs){
    if( this != &rhs ){
        this->fftWindowSize = rhs.fftWindowSize;
        this->numChannelsInFFTSignal = rhs.numChannelsInFFTSignal;
        this->computeMaxFreqFeature = rhs.computeMaxFreqFeature;
        this->computeMaxFreqSpectrumRatio = rhs.computeMaxFreqSpectrumRatio;
        this->computeCentroidFeature = rhs.computeCentroidFeature;
        this->computeTopNFreqFeatures = rhs.computeTopNFreqFeatures;
        this->N = rhs.N;
        this->maxFreqFeature = rhs.maxFreqFeature;
        this->maxFreqSpectrumRatio = rhs.maxFreqSpectrumRatio;
        this->centroidFeature = rhs.centroidFeature;
        
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

//Clone method
bool FFTFeatures::deepCopyFrom(const FeatureExtraction *featureExtraction){ 
        
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *(FFTFeatures*)featureExtraction;
        
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool FFTFeatures::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_FFT_FEATURES_FILE_V1.0" << std::endl;
    
    //Save the base settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    //Write the FFT features settings
    file << "FFTWindowSize: " << fftWindowSize << std::endl;
    file << "NumChannelsInFFTSignal: " << numChannelsInFFTSignal << std::endl;
    file << "ComputeMaxFreqFeature: " << computeMaxFreqFeature << std::endl;
    file << "ComputeMaxFreqSpectrumRatio: " << computeMaxFreqSpectrumRatio << std::endl;
    file << "ComputeCentroidFeature: " << computeCentroidFeature << std::endl;	
    file << "ComputeTopNFreqFeatures: " << computeTopNFreqFeatures << std::endl;		
    file << "N: " << N << std::endl;
    
    return true;
}
    
bool FFTFeatures::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_FFT_FEATURES_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;     
    }
    
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    //Load the FFTWindowSize
    file >> word;
    if( word != "FFTWindowSize:" ){
        errorLog << "load(fstream &file) - Failed to read FFTWindowSize header!" << std::endl;
        return false;     
    }
    file >> fftWindowSize;
    
    //Load the NumOutputDimensions
    file >> word;
    if( word != "NumChannelsInFFTSignal:" ){
        errorLog << "load(fstream &file) - Failed to read NumChannelsInFFTSignal header!" << std::endl;
        return false;     
    }
    file >> numChannelsInFFTSignal;
    
    file >> word;
    if( word != "ComputeMaxFreqFeature:" ){
        errorLog << "load(fstream &file) - Failed to read ComputeMaxFreqFeature header!" << std::endl;
        return false;     
    }
    file >> computeMaxFreqFeature;
    
    file >> word;
    if( word != "ComputeMaxFreqSpectrumRatio:" ){
        errorLog << "load(fstream &file) - Failed to read ComputeMaxFreqSpectrumRatio header!" << std::endl;
        return false;     
    }
    file >> computeMaxFreqSpectrumRatio;
    
    file >> word;
    if( word != "ComputeCentroidFeature:" ){
        errorLog << "load(fstream &file) - Failed to read ComputeCentroidFeature header!" << std::endl;
        return false;     
    }
    file >> computeCentroidFeature;
    
    file >> word;
    if( word != "ComputeTopNFreqFeatures:" ){
        errorLog << "load(fstream &file) - Failed to read ComputeTopNFreqFeatures header!" << std::endl;
        return false;     
    }
    file >> computeTopNFreqFeatures;
    
    file >> word;
    if( word != "N:" ){
        errorLog << "load(fstream &file) - Failed to read N header!" << std::endl;
        return false;     
    }
    file >> N;
    
    //Init the FFTFeatures module to ensure everything is initialized correctly
    return init(fftWindowSize,numChannelsInFFTSignal,computeMaxFreqFeature,computeMaxFreqSpectrumRatio,computeCentroidFeature,computeTopNFreqFeatures,N);
}

bool FFTFeatures::init(const UINT fftWindowSize,const UINT numChannelsInFFTSignal,const bool computeMaxFreqFeature,const bool computeMaxFreqSpectrumRatio,const bool computeCentroidFeature,const bool computeTopNFreqFeatures,const UINT N){
    
    initialized = false;
    featureDataReady = false;
    maxFreqFeature = 0;
    centroidFeature = 0;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    featureVector.clear();
    topNFreqFeatures.clear();
    
    this->fftWindowSize = fftWindowSize;
    this->numChannelsInFFTSignal = numChannelsInFFTSignal;
    this->computeMaxFreqFeature = computeMaxFreqFeature;
    this->computeMaxFreqSpectrumRatio = computeMaxFreqSpectrumRatio;
    this->computeCentroidFeature = computeCentroidFeature;
    this->computeTopNFreqFeatures = computeTopNFreqFeatures;
    this->N = N;
    
    numInputDimensions = fftWindowSize*numChannelsInFFTSignal;
    numOutputDimensions = 0;
    if( computeMaxFreqFeature ) numOutputDimensions += 1;
    if( computeMaxFreqSpectrumRatio ) numOutputDimensions += 1;
    if( computeCentroidFeature ) numOutputDimensions += 1;
    if( computeTopNFreqFeatures ){
        numOutputDimensions += N;
        topNFreqFeatures.resize(N,0); 
    }
    numOutputDimensions *= numChannelsInFFTSignal;
    
    //Resize the feature vector
    featureVector.resize(numOutputDimensions,0);
    
    initialized = true;

    return true;
}
    
bool FFTFeatures::computeFeatures(const VectorFloat &inputVector){

    if( !initialized ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    //The input vector should be the magnitude data from an FFT
    if( inputVector.getSize() != fftWindowSize*numChannelsInFFTSignal ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.getSize() << ") does not match the expected size! Verify that the FFT module that generated this inputVector has a window size of " << fftWindowSize << " and the number of input channels is: " << numChannelsInFFTSignal << ". Also verify that only the magnitude values are being computed (and not the phase)." << std::endl;
        return false;
    }
    
    featureDataReady = false;
    
    UINT featureIndex = 0;
    IndexedDouble maxFreq(0,0);
    Vector< IndexedDouble > fftMagData(fftWindowSize);
    
    for(UINT i=0; i<numChannelsInFFTSignal; i++){
        Float spectrumSum = 0;
        maxFreq.value = 0;
        maxFreq.index = 0;
        centroidFeature = 0;

        for(UINT n=0; n<fftWindowSize; n++){
            
            //Find the max freq
            if( inputVector[i*fftWindowSize + n] > maxFreq.value ){
                maxFreq.value = inputVector[i*fftWindowSize + n];
                maxFreq.index = n;
            }

			centroidFeature += (n+1) * inputVector[i*fftWindowSize + n];
            
            spectrumSum += inputVector[i*fftWindowSize + n];
            
            //Copy the magnitude data so we can sort it later if needed
            fftMagData[n].value = inputVector[i*fftWindowSize + n];
            fftMagData[n].index = n;
        }
        
        maxFreqFeature = maxFreq.index;
        maxFreqSpectrumRatio = spectrumSum > 0 ? maxFreq.value/spectrumSum : 0;
        centroidFeature = spectrumSum > 0 ? centroidFeature/spectrumSum : 0;
        
        if( computeMaxFreqFeature ){
            featureVector[ featureIndex++ ] = maxFreqFeature;
        }
        
        if( computeMaxFreqSpectrumRatio ){
            featureVector[ featureIndex++ ] = maxFreqSpectrumRatio;
        }
        
        if( computeCentroidFeature ){
            featureVector[ featureIndex++ ] = centroidFeature;
        }
        
        if( computeTopNFreqFeatures ){
            
            sort(fftMagData.begin(),fftMagData.end(),sortIndexDoubleDecendingValue);
            for(UINT n=0; n<N; n++){
                topNFreqFeatures[n] = fftMagData[n].index;
            }
            
            for(UINT n=0; n<N; n++){
                featureVector[ featureIndex++ ] = topNFreqFeatures[n];
            }
        }
    }
    
    //Flag that the features are ready
    featureDataReady = true;
    
    return true;
}
    
bool FFTFeatures::reset(){ 
    if( initialized ) return init(fftWindowSize,numChannelsInFFTSignal,computeMaxFreqFeature,computeMaxFreqSpectrumRatio,computeCentroidFeature,computeTopNFreqFeatures,N);
    return false;
}
    
GRT_END_NAMESPACE
