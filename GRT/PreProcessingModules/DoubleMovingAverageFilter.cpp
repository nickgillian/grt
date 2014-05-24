/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @section LICENSE
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
 
 @section DESCRIPTION
 The DoubleMovingAverageFilter implements a low pass double moving average filter.
 
 */

#include "DoubleMovingAverageFilter.h"

namespace GRT{
    
//Register the DoubleMovingAverageFilter module with the PreProcessing base class
RegisterPreProcessingModule< DoubleMovingAverageFilter > DoubleMovingAverageFilter::registerModule("DoubleMovingAverageFilter");

DoubleMovingAverageFilter::DoubleMovingAverageFilter(UINT filterSize,UINT numDimensions){
    classType = "DoubleMovingAverageFilter";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG DoubleMovingAverageFilter]");
    errorLog.setProceedingText("[ERROR DoubleMovingAverageFilter]");
    warningLog.setProceedingText("[WARNING DoubleMovingAverageFilter]");
    init(filterSize,numDimensions);
}
    
DoubleMovingAverageFilter::DoubleMovingAverageFilter(const DoubleMovingAverageFilter &rhs){
    
    this->filterSize = rhs.filterSize;
    this->filter1 = rhs.filter1;
    this->filter2 = rhs.filter2;
    classType = "DoubleMovingAverageFilter";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG DoubleMovingAverageFilter]");
    errorLog.setProceedingText("[ERROR DoubleMovingAverageFilter]");
    warningLog.setProceedingText("[WARNING DoubleMovingAverageFilter]");
    
    //Copy the base variables
    copyBaseVariables( (PreProcessing*)&rhs );
}
    
DoubleMovingAverageFilter::~DoubleMovingAverageFilter(){

}
    
DoubleMovingAverageFilter& DoubleMovingAverageFilter::operator=(const DoubleMovingAverageFilter &rhs){
    if(this!=&rhs){
        this->filterSize = rhs.filterSize;
        this->filter1 = rhs.filter1;
        this->filter2 = rhs.filter2;
        
        //Copy the base variables
        copyBaseVariables( (PreProcessing*)&rhs );
    }
    return *this;
}
    
bool DoubleMovingAverageFilter::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getPreProcessingType() == preProcessing->getPreProcessingType() ){
        
        DoubleMovingAverageFilter *ptr = (DoubleMovingAverageFilter*)preProcessing;
        
        //Clone the classLabelTimeoutFilter values 
        this->filterSize = ptr->filterSize;
        this->filter1 = ptr->filter1;
        this->filter2 = ptr->filter2;
        
        //Clone the preprocessing base variables
        return copyBaseVariables( preProcessing );
    }
    
    errorLog << "clone(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << endl;
    
    return false;
}

    
bool DoubleMovingAverageFilter::process(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorDouble &inputVector) - The filter has not been initialized!" << endl;
        return false;
    }

    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return false;
    }
    
    processedData = filter( inputVector );
    
    if( processedData.size() == numOutputDimensions ) return true;
    return false;
}

bool DoubleMovingAverageFilter::reset(){
    if( initialized ) return init(filterSize,numInputDimensions);
    return false;
}
    
bool DoubleMovingAverageFilter::saveModelToFile(string filename) const{
    
    if( !initialized ){
        errorLog << "saveModelToFile(string filename) - The DoubleMovingAverageFilter has not been initialized" << endl;
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
    
bool DoubleMovingAverageFilter::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "GRT_DOUBLE_MOVING_AVERAGE_FILTER_FILE_V1.0" << endl;
    
    file << "NumInputDimensions: " << numInputDimensions << endl;
    file << "NumOutputDimensions: " << numOutputDimensions << endl;
    file << "FilterSize: " << filterSize << endl;
    
    return true;
}

bool DoubleMovingAverageFilter::loadModelFromFile(string filename){
    
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

bool DoubleMovingAverageFilter::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_DOUBLE_MOVING_AVERAGE_FILTER_FILE_V1.0" ){
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
    
    //Load the filter size
    file >> word;
    if( word != "FilterSize:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read FilterSize header!" << endl;
        return false;     
    }
    file >> filterSize;
    
    //Init the filter module to ensure everything is initialized correctly
    return init(filterSize,numInputDimensions);  
}

bool DoubleMovingAverageFilter::init(UINT filterSize,UINT numDimensions){
    
    //Cleanup the old memory
    initialized = false;
    
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
    numInputDimensions = numDimensions;
    numOutputDimensions = numDimensions;
    processedData.clear();
    processedData.resize(numDimensions,0 );
    
    if( !filter1.init(filterSize, numDimensions) ){
        errorLog << "init(UINT filterSize,UINT numDimensions) - Failed to initialize filter 1!" << endl;
        return false;
    }
    
    if( !filter2.init(filterSize, numDimensions) ){
        errorLog << "init(UINT filterSize,UINT numDimensions) - Failed to initialize filter 1!" << endl;
        return false;
    }
    
    //Flag that the filter has been initialized
    initialized = true;
    
    return true;
}

double DoubleMovingAverageFilter::filter(const double x){
    
    //If the filter has not been initialised then return 0, otherwise filter x and return y
    if( !initialized ){
        errorLog << "filter(const double x) - The filter has not been initialized!" << endl;
        return 0;
    }
    
    VectorDouble y = filter(VectorDouble(1,x));
    
    if( y.size() == 0 ) return 0;
    return y[0];
}
    
VectorDouble DoubleMovingAverageFilter::filter(const VectorDouble &x){
    
    //If the filter has not been initialised then return 0, otherwise filter x and return y
    if( !initialized ){
        errorLog << "filter(const VectorDouble &x) - The filter has not been initialized!" << endl;
        return VectorDouble();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "filter(const VectorDouble &x) - The size of the input vector (" << x.size() << ") does not match that of the number of dimensions of the filter (" << numInputDimensions << ")!" << endl;
        return VectorDouble();
    }
    
    //Perform the first filter
    VectorDouble y = filter1.filter( x );
    
    if( y.size() == 0 ) return y;
    
    //Perform the second filter
    VectorDouble yy = filter2.filter( y );
    
    if( yy.size() == 0 ) return y;
    
    //Account for the filter lag
    for(UINT i=0; i<y.size(); i++){
        yy[i] = y[i] + (y[i] - yy[i]); 
        processedData[i] = yy[i];
    }
    
    return yy;
}

}//End of namespace GRT