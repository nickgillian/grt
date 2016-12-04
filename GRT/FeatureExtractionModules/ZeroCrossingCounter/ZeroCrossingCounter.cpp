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
#include "ZeroCrossingCounter.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string ZeroCrossingCounter::id = "ZeroCrossingCounter";
std::string ZeroCrossingCounter::getId() { return ZeroCrossingCounter::id; }

//Register the ZeroCrossingCounter module with the FeatureExtraction base class
RegisterFeatureExtractionModule< ZeroCrossingCounter > ZeroCrossingCounter::registerModule( ZeroCrossingCounter::getId() );

ZeroCrossingCounter::ZeroCrossingCounter(const UINT searchWindowSize,const Float deadZoneThreshold,const UINT numDimensions,const UINT featureMode) : FeatureExtraction( ZeroCrossingCounter::getId() )
{
    init(searchWindowSize,deadZoneThreshold,numDimensions,featureMode);
}

ZeroCrossingCounter::ZeroCrossingCounter(const ZeroCrossingCounter &rhs) : FeatureExtraction( ZeroCrossingCounter::getId() )
{
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

ZeroCrossingCounter::~ZeroCrossingCounter(){
    
}

ZeroCrossingCounter& ZeroCrossingCounter::operator=(const ZeroCrossingCounter &rhs){
    if(this!=&rhs){
        this->searchWindowSize = rhs.searchWindowSize;
        this->featureMode = rhs.featureMode;
        this->deadZoneThreshold = rhs.deadZoneThreshold;
        this->derivative = rhs.derivative;
        this->deadZone = rhs.deadZone;
        this->dataBuffer = rhs.dataBuffer;
        
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool ZeroCrossingCounter::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *dynamic_cast<const ZeroCrossingCounter*>(featureExtraction);
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool ZeroCrossingCounter::computeFeatures(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.getSize() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    update( inputVector );
    
    return true;
}

bool ZeroCrossingCounter::reset(){
    if( initialized ){
        return init( searchWindowSize, deadZoneThreshold, numInputDimensions, featureMode );
    }
    return false;
}

bool ZeroCrossingCounter::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_ZERO_CROSSING_COUNTER_FILE_V1.0" << std::endl;
    
    //Save the base settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "saveFeatureExtractionSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    //Write the zero crossing counter settings
    file << "SearchWindowSize: " << searchWindowSize << std::endl;
    file << "FeatureMode: " << featureMode << std::endl;
    file << "DeadZoneThreshold: " << deadZoneThreshold << std::endl;
    
    return true;
}

bool ZeroCrossingCounter::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_ZERO_CROSSING_COUNTER_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if( word != "SearchWindowSize:" ){
        errorLog << "load(fstream &file) - Failed to read SearchWindowSize header!" << std::endl;
        return false;
    }
    file >> searchWindowSize;
    
    file >> word;
    if( word != "FeatureMode:" ){
        errorLog << "load(fstream &file) - Failed to read FeatureMode header!" << std::endl;
        return false;
    }
    file >> featureMode;
    
    file >> word;
    if( word != "DeadZoneThreshold:" ){
        errorLog << "load(fstream &file) - Failed to read DeadZoneThreshold header!" << std::endl;
        return false;
    }
    file >> deadZoneThreshold;
    
    //Init the ZeroCrossingCounter module to ensure everything is initialized correctly
    return init(searchWindowSize,deadZoneThreshold,numInputDimensions,featureMode);
}

bool ZeroCrossingCounter::init(const UINT searchWindowSize,const Float deadZoneThreshold,const UINT numDimensions,const UINT featureMode){
    
    initialized = false;
    featureDataReady = false;
    
    if( searchWindowSize == 0 ){
        errorLog << "init(UINT searchWindowSize,Float deadZoneThreshold,UINT numDimensions,UINT featureMode) - The searchWindowSize must be greater than zero!" << std::endl;
        return false;
    }
    
    if( deadZoneThreshold < 0 ){
        errorLog << "init(UINT searchWindowSize,Float deadZoneThreshold,UINT numDimensions,UINT featureMode) - The deadZoneThreshold must be greater than zero!" << std::endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(UINT searchWindowSize,Float deadZoneThreshold,UINT numDimensions,UINT featureMode) - The numDimensions must be greater than zero!" << std::endl;
        return false;
    }
    
    if( featureMode != INDEPENDANT_FEATURE_MODE && featureMode != COMBINED_FEATURE_MODE ){
        errorLog << "init(UINT searchWindowSize,Float deadZoneThreshold,UINT numDimensions,UINT featureMode) - Unkown feature mode!" << std::endl;
        return false;
    }
    
    //Setup the search variables
    this->searchWindowSize = searchWindowSize;
    this->featureMode = featureMode;
    this->deadZoneThreshold = deadZoneThreshold;
    numInputDimensions = numDimensions;
    numOutputDimensions = (featureMode == INDEPENDANT_FEATURE_MODE ? TOTAL_NUM_ZERO_CROSSING_FEATURES * numInputDimensions : TOTAL_NUM_ZERO_CROSSING_FEATURES);
    derivative.init(Derivative::FIRST_DERIVATIVE, 1.0, numInputDimensions, true, 5);
    deadZone.init(-deadZoneThreshold,deadZoneThreshold,numInputDimensions);
    dataBuffer.resize( searchWindowSize, VectorFloat(numInputDimensions,NAN) );
    featureVector.resize(numOutputDimensions,0);
    
    //Flag that the zero crossing counter has been initialized
    initialized = true;
    
    return true;
}


VectorFloat ZeroCrossingCounter::update(const Float x){
    return update(VectorFloat(1,x));
}

VectorFloat ZeroCrossingCounter::update(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "update(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.getSize() != numInputDimensions ){
        errorLog << "update(const VectorFloat &x)- The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.getSize() << ")!" << std::endl;
        return VectorFloat();
    }
    
    //Clear the feature vector
    std::fill(featureVector.begin(),featureVector.end(),0);
    
    //Update the derivative data and
    derivative.computeDerivative( x );
    
    //Dead zone the derivative data
    deadZone.filter( derivative.getProcessedData() );
    
    //Add the deadzone data to the buffer
    dataBuffer.push_back( deadZone.getProcessedData() );
    
    //Search the buffer for the zero crossing features
    for(UINT j=0; j<numInputDimensions; j++){
        UINT colIndex = (featureMode == INDEPENDANT_FEATURE_MODE ? (TOTAL_NUM_ZERO_CROSSING_FEATURES*j) : 0);
        for(UINT i=1; i<dataBuffer.getSize(); i++){
            //Search for a zero crossing
            if( (dataBuffer[i][j] > 0 && dataBuffer[i-1][j] <= 0) || (dataBuffer[i][j] < 0 && dataBuffer[i-1][j] >= 0) ){
                //Update the zero crossing count
                featureVector[ NUM_ZERO_CROSSINGS_COUNTED + colIndex ]++;
                
                //Update the magnitude, search the last 5 values around the zero crossing to make sure we get the maxima of the peak
                Float maxValue = 0;
                UINT searchSize = i > 5 ? 5 : i;
                for(UINT n=0; n<searchSize; n++){
                    Float value = fabs( dataBuffer[ i-n ][j] );
                    if( value > maxValue ) maxValue = value;
                }
                featureVector[ ZERO_CROSSING_MAGNITUDE + colIndex ] += maxValue;
            }
        }
    }
    
    //Flag that the feature data has been computed
    featureDataReady = true;
    
    return featureVector;
}

bool ZeroCrossingCounter::setSearchWindowSize(const UINT searchWindowSize){
    if( searchWindowSize > 0 ){
        this->searchWindowSize = searchWindowSize;
        if( initialized ) return reset();
        return true;
    }
    errorLog << "setSearchWindowSize(const UINT searchWindowSize) - The searchWindowSize must be larger than zero!" << std::endl;
    return false;
}

bool ZeroCrossingCounter::setFeatureMode(const UINT featureMode){
    if( featureMode == INDEPENDANT_FEATURE_MODE || featureMode == COMBINED_FEATURE_MODE ){
        this->featureMode = featureMode;
        if( initialized ) return reset();
        return true;
    }
    errorLog << "setFeatureMode(UINT featureMode) - Unkown feature mode!" << std::endl;
    return false;
}

bool ZeroCrossingCounter::setDeadZoneThreshold(const Float deadZoneThreshold){
    if( deadZoneThreshold > 0 ){
        this->deadZoneThreshold = deadZoneThreshold;
        if( initialized ) return reset();
        return true;
    }
    errorLog << "setDeadZoneThreshold(const Float deadZoneThreshold) - The deadZoneThreshold must be larger than zero!" << std::endl;
    return false;
}

GRT_END_NAMESPACE
