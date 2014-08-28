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

#include "MedianFilter.h"

namespace GRT{
    
//Register the MedianFilter module with the PreProcessing base class
RegisterPreProcessingModule< MedianFilter > MedianFilter::registerModule("MedianFilter");

MedianFilter::MedianFilter(UINT filterSize,UINT numDimensions){
    
    classType = "MedianFilter";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG MedianFilter]");
    errorLog.setProceedingText("[ERROR MedianFilter]");
    warningLog.setProceedingText("[WARNING MedianFilter]");
    init(filterSize,numDimensions);
}
    
MedianFilter::MedianFilter(const MedianFilter &rhs){
    
    classType = "MedianFilter";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG MedianFilter]");
    errorLog.setProceedingText("[ERROR MedianFilter]");
    warningLog.setProceedingText("[WARNING MedianFilter]");
    
    //Zero this instance
    this->filterSize = 0;
    this->inputSampleCounter = 0;
    
	//Copy the settings from the rhs instance
	*this = rhs;
}
    
MedianFilter::~MedianFilter(){

}
    
MedianFilter& MedianFilter::operator=(const MedianFilter &rhs){
    if(this!=&rhs){
        //Clear this instance
        this->filterSize = 0;
        this->inputSampleCounter = 0;
        this->dataBuffer.clear();
        
        //Copy from the rhs instance
        if( rhs.initialized ){
            this->init( rhs.filterSize, rhs.numInputDimensions );
            this->dataBuffer = rhs.dataBuffer;
        }
        
        //Copy the preprocessing base variables
        copyBaseVariables( (PreProcessing*)&rhs );
    }
    return *this;
}
    
bool MedianFilter::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getPreProcessingType() == preProcessing->getPreProcessingType() ){
        
        //Call the equals operator
        *this = *(MedianFilter*)preProcessing;
        
		return true;
    }
    
    errorLog << "clone(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << endl;
    
    return false;
}

    
bool MedianFilter::process(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorDouble &inputVector) - The filter has not been initialized!" << endl;
        return false;
    }

    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return false;
    }
    
    filter( inputVector );
    
    if( processedData.size() == numOutputDimensions ) return true;

    return false;
}

bool MedianFilter::reset(){
    if( initialized ) return init(filterSize,numInputDimensions);
    return false;
}
    
bool MedianFilter::saveModelToFile(string filename) const{
    
    if( !initialized ){
        errorLog << "saveModelToFile(string filename) - The MedianFilter has not been initialized" << endl;
        return false;
    }
    
    std::fstream file; 
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
}

bool MedianFilter::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "GRT_MEDIAN_FILTER_FILE_V1.0" << endl;
    
    file << "NumInputDimensions: " << numInputDimensions << endl;
    file << "NumOutputDimensions: " << numOutputDimensions << endl;
    file << "FilterSize: " << filterSize << endl;
    
    return true;
}

bool MedianFilter::loadModelFromFile(string filename){
    
    std::fstream file; 
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        file.close();
        initialized = false;
        return false;
    }
    
    file.close();
    
    return true;
}

bool MedianFilter::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_MEDIAN_FILTER_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    //Load the number of input dimensions
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumInputDimensions header!" << endl;
        return false;     
    }
    file >> numInputDimensions;
    
    //Load the number of output dimensions
    file >> word;
    if( word != "NumOutputDimensions:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumOutputDimensions header!" << endl;
        return false;     
    }
    file >> numOutputDimensions;
    
    //Load the filter factor
    file >> word;
    if( word != "FilterSize:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read FilterSize header!" << endl;
        return false;     
    }
    file >> filterSize;
    
    //Init the filter module to ensure everything is initialized correctly
    return init(filterSize,numInputDimensions);  
}

bool MedianFilter::init(UINT filterSize,UINT numDimensions){
    
    //Cleanup the old memory
    initialized = false;
    inputSampleCounter = 0;
    
    if( filterSize == 0 ){
        errorLog << "init(UINT filterSize,UINT numDimensions) - Filter size can not be zero!" << endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(UINT filterSize,UINT numDimensions) - The number of dimensions must be greater than zero!" << endl;
        return false;
    }
    
    //Resize the filter
    this->filterSize = filterSize;
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numDimensions;
    processedData.clear();
    processedData.resize(numDimensions,0);
    initialized = dataBuffer.resize( filterSize, VectorDouble(numInputDimensions,0) );
    
    if( !initialized ){
        errorLog << "init(UINT filterSize,UINT numDimensions) - Failed to resize dataBuffer!" << endl;
    }
    
    return initialized;
}

double MedianFilter::filter(const double x){
    
    VectorDouble y = filter(VectorDouble(1,x));
    
    if( y.size() == 0 ) return 0;
    return y[0];
}
    
VectorDouble MedianFilter::filter(const VectorDouble &x){
    
    //If the filter has not been initialised then return 0, otherwise filter x and return y
    if( !initialized ){
        errorLog << "filter(const VectorDouble &x) - The filter has not been initialized!" << endl;
        return VectorDouble();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "filter(const VectorDouble &x) - The size of the input vector (" << x.size() << ") does not match that of the number of dimensions of the filter (" << numInputDimensions << ")!" << endl;
        return VectorDouble();
    }
    
    if( ++inputSampleCounter > filterSize ) inputSampleCounter = filterSize;
    
    //Add the new value to the buffer
    dataBuffer.push_back( x );
    
    //Compute the median value for each dimension
    vector< double > tmp( inputSampleCounter );
    for(unsigned int j=0; j<numInputDimensions; j++){
        for(unsigned int i=0; i<inputSampleCounter; i++) {
            tmp[i] = dataBuffer[i][j];
        }
        std::sort(tmp.begin(),tmp.end());
        
        //Get the median value
        unsigned int medianIndex = (inputSampleCounter/2);
        processedData[j] = tmp[ medianIndex ];
    }
    
    return processedData;
}
    
vector< VectorDouble > MedianFilter::getDataBuffer() const {
    
    if( !initialized ){
        return vector< VectorDouble >();
    }
    
    vector< VectorDouble > data(numInputDimensions,VectorDouble(inputSampleCounter));
    for(unsigned int j=0; j<numInputDimensions; j++){
        for(unsigned int i=0; i<inputSampleCounter; i++){
            data[j][i] = dataBuffer[i][j];
        }
    }
    return data;
}

}//End of namespace GRT
