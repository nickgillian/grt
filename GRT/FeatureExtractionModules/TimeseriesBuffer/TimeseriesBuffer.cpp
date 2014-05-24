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

#include "TimeseriesBuffer.h"

namespace GRT{
    
//Register the TimeseriesBuffer module with the FeatureExtraction base class
RegisterFeatureExtractionModule< TimeseriesBuffer > TimeseriesBuffer::registerModule("TimeseriesBuffer");
    
TimeseriesBuffer::TimeseriesBuffer(UINT bufferSize,UINT numDimensions){
    
    classType = "TimeseriesBuffer";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG TimeseriesBuffer]");
    errorLog.setProceedingText("[ERROR TimeseriesBuffer]");
    warningLog.setProceedingText("[WARNING TimeseriesBuffer]");

    init(bufferSize,numDimensions);
}
    
TimeseriesBuffer::TimeseriesBuffer(const TimeseriesBuffer &rhs){
    
    classType = "TimeseriesBuffer";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG TimeseriesBuffer]");
    errorLog.setProceedingText("[ERROR TimeseriesBuffer]");
    warningLog.setProceedingText("[WARNING TimeseriesBuffer]");
    
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
    
    if( this->getFeatureExtractionType() == featureExtraction->getFeatureExtractionType() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *(TimeseriesBuffer*)featureExtraction;
        
        return true;
    }
    
    errorLog << "clone(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << endl;
    
    return false;
}
    
bool TimeseriesBuffer::computeFeatures(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
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
    
bool TimeseriesBuffer::saveModelToFile(const string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool TimeseriesBuffer::loadModelFromFile(const string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool TimeseriesBuffer::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_TIMESERIES_BUFFER_FILE_V1.0" << endl;
    
    //Save the base settings to the file
    if( !saveBaseSettingsToFile( file ) ){
        errorLog << "saveModelToFile(fstream &file) - Failed to save base feature extraction settings to file!" << endl;
        return false;
    }
    
    //Write the zero crossing counter settings
    file << "BufferSize: " << dataBuffer.getSize() << endl;
    
    return true;
}

bool TimeseriesBuffer::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_TIMESERIES_BUFFER_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    if( !loadBaseSettingsFromFile( file ) ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if( word != "BufferSize:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read BufferSize header!" << endl;
        return false;     
    }
    file >> bufferSize;
    
    //Init the TimeseriesBuffer module to ensure everything is initialized correctly
    return init(bufferSize,numInputDimensions);
}
    
bool TimeseriesBuffer::init(UINT bufferSize,UINT numDimensions){
    
    initialized = false;
    featureDataReady = false;
    
    if( bufferSize == 0 ){
        errorLog << "init(UINT bufferSize,UINT numDimensions) - The bufferSize must be greater than zero!" << endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(UINT bufferSize,UINT numDimensions) - The numDimensions must be greater than zero!" << endl;
        return false;
    }
    
    //Setup the databuffer
    numInputDimensions = numDimensions;
    numOutputDimensions = bufferSize * numInputDimensions;
    this->bufferSize = bufferSize;
    dataBuffer.resize( bufferSize, VectorDouble(numInputDimensions,0) );
    featureVector.resize(numOutputDimensions,0);
    
    //Flag that the timeseries buffer has been initialized
    initialized = true;
    
    return true;
}


VectorDouble TimeseriesBuffer::update(double x){
	return update(VectorDouble(1,x));
}
    
VectorDouble TimeseriesBuffer::update(const VectorDouble &x){
    
    if( !initialized ){
        errorLog << "update(const VectorDouble &x) - Not Initialized!" << endl;
        return VectorDouble();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "update(const VectorDouble &x)- The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << endl;
        return VectorDouble();
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
    errorLog << "setBufferSize(UINT bufferSize) - The bufferSize must be larger than zero!" << endl;
    return false;
}
    
UINT TimeseriesBuffer::getBufferSize(){
    if( initialized ) return bufferSize;
    return 0;
}

vector< VectorDouble > TimeseriesBuffer::getDataBuffer(){
    if( initialized ) return dataBuffer.getDataAsVector();
    return vector< VectorDouble >();
}

}//End of namespace GRT