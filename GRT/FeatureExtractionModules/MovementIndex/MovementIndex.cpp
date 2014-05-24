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

#include "MovementIndex.h"

namespace GRT{
    
//Register the MovementIndex module with the FeatureExtraction base class
RegisterFeatureExtractionModule< MovementIndex > MovementIndex::registerModule("MovementIndex");
    
MovementIndex::MovementIndex(UINT bufferLength,UINT numDimensions){
    
    classType = "MovementIndex";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG MovementIndex]");
    errorLog.setProceedingText("[ERROR MovementIndex]");
    warningLog.setProceedingText("[WARNING MovementIndex]");
    init(bufferLength,numDimensions);
}
    
MovementIndex::MovementIndex(const MovementIndex &rhs){
    
    classType = "MovementIndex";
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG MovementIndex]");
    errorLog.setProceedingText("[ERROR MovementIndex]");
    warningLog.setProceedingText("[WARNING MovementIndex]");
    
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
    
    if( this->getFeatureExtractionType() == featureExtraction->getFeatureExtractionType() ){
        
        //Invoke the equals operator to copy the data from the rhs instance to this instance
        *this = *(MovementIndex*)featureExtraction;
        
        return true;
    }
    
    errorLog << "clone(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << endl;
    
    return false;
}
    
bool MovementIndex::computeFeatures(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
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
    
bool MovementIndex::saveModelToFile(const string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool MovementIndex::loadModelFromFile(const string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool MovementIndex::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Write the file header
    file << "GRT_MOVEMENT_INDEX_FILE_V1.0" << endl;	
    
    //Save the base settings to the file
    if( !saveBaseSettingsToFile( file ) ){
        errorLog << "saveModelToFile(fstream &file) - Failed to save base feature extraction settings to file!" << endl;
        return false;
    }
    
    //Write the movement index settings to the file
    file << "BufferLength: " << bufferLength << endl;
    
    return true;
}

bool MovementIndex::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_MOVEMENT_INDEX_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    if( !loadBaseSettingsFromFile( file ) ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << endl;
        return false;
    }
    
    //Load the BufferLength
    file >> word;
    if( word != "BufferLength:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read BufferLength header!" << endl;
        return false;     
    }
    file >> bufferLength;
    
    //Init the MovementIndex module to ensure everything is initialized correctly
    return init(bufferLength,numInputDimensions);
}
    
bool MovementIndex::init(UINT bufferLength,UINT numDimensions){
    
    initialized = false;
    
    if( bufferLength == 0 ){
        errorLog << "init(...) - The number of bufferLength must be greater than zero!" << endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(...) - The number of dimensions must be greater than zero!" << endl;
        return false;
    }
    
    this->bufferLength = bufferLength;
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numInputDimensions;
    featureDataReady = false;
    
    //Resize the feature vector
    featureVector.resize(numInputDimensions);
    
    //Resize the raw trajectory data buffer
    dataBuffer.resize( bufferLength, vector< double >(numInputDimensions,0) );

    //Flag that the zero crossing counter has been initialized
    initialized = true;
    
    return true;
}


vector< double > MovementIndex::update(double x){
	return update(vector<double>(1,x));
}
    
vector< double > MovementIndex::update(const vector< double > &x){
    
#ifdef GRT_SAFE_CHECKING
    if( !initialized ){
        errorLog << "update(const vector< double > &x) - Not Initialized!" << endl;
        return vector<double>();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "update(const vector< double > &x)- The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << endl;
        return vector<double>();
    }
#endif
    
    //Add the new data to the trajectory data buffer
    dataBuffer.push_back( x );
    
    //Only flag that the feature data is ready if the trajectory data is full
    if( !dataBuffer.getBufferFilled() ){
        featureDataReady = false;
        for(UINT i=0; i<featureVector.size(); i++){
            featureVector[i] = 0;
        }
        return featureVector;
    }else featureDataReady = true;
    
    //Compute the movement index (which is the RMS error)
    vector< double > mu(numInputDimensions,0);
    
    //Compute mu
    for(UINT j=0; j<numInputDimensions; j++){
        for(UINT i=0; i<dataBuffer.getSize(); i++){
            mu[j] += dataBuffer[i][j];
        }
        mu[j] /= double(dataBuffer.getSize());
    }
    
    for(UINT j=0; j<numInputDimensions; j++){
        featureVector[j] = 0;
        for(UINT i=0; i<dataBuffer.getSize(); i++){
            featureVector[j] += SQR( dataBuffer[i][j] - mu[j] );
        }
        featureVector[j] = sqrt( featureVector[j]/double(dataBuffer.getSize()) );
    }
    
    return featureVector;
}
    
CircularBuffer< vector< double > > MovementIndex::getData(){
    if( initialized ){
        return dataBuffer;
    }
    return CircularBuffer< vector<double > >();
}
    
}//End of namespace GRT