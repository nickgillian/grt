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

#include "Derivative.h"

namespace GRT{
    
//Register the Derivative module with the PreProcessing base class
RegisterPreProcessingModule< Derivative > Derivative::registerModule("Derivative");
    
Derivative::Derivative(UINT derivativeOrder,double delta,UINT numDimensions,bool filterData,UINT filterSize){
    classType = "Derivative";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG Derivative]");
    errorLog.setProceedingText("[ERROR Derivative]");
    warningLog.setProceedingText("[WARNING Derivative]");
    if( derivativeOrder == FIRST_DERIVATIVE || derivativeOrder == SECOND_DERIVATIVE ){
        init(derivativeOrder,delta,numDimensions,filterData,filterSize);
    }
}
    
Derivative::Derivative(const Derivative &rhs){
    
    this->derivativeOrder = rhs.derivativeOrder;
    this->filterSize = rhs.filterSize;
    this->delta = rhs.delta;
    this->filterData = rhs.filterData;
    this->filter = rhs.filter;
    this->yy = rhs.yy;
    this->yyy = rhs.yyy;
    
    classType = "Derivative";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG Derivative]");
    errorLog.setProceedingText("[ERROR Derivative]");
    warningLog.setProceedingText("[WARNING Derivative]");
    
    copyBaseVariables( (PreProcessing*)&rhs );
}

Derivative::~Derivative(){

}
    
Derivative& Derivative::operator=(const Derivative &rhs){
    if( this != &rhs ){
        this->derivativeOrder = rhs.derivativeOrder;
	    this->filterSize = rhs.filterSize;
	    this->delta = rhs.delta;
	    this->filterData = rhs.filterData;
	    this->filter = rhs.filter;
	    this->yy = rhs.yy;
	    this->yyy = rhs.yyy;
        copyBaseVariables( (PreProcessing*)&rhs );
    }
    return *this;
}
    
bool Derivative::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getPreProcessingType() == preProcessing->getPreProcessingType() ){
        
        Derivative *ptr = (Derivative*)preProcessing;
        
        //Clone the Derivative values 
        this->derivativeOrder = ptr->derivativeOrder;
        this->filterSize = ptr->filterSize;
        this->delta = ptr->delta;
        this->filterData = ptr->filterData;
        this->filter = ptr->filter;
        this->yy = ptr->yy;
        this->yyy = ptr->yyy;
        
        //Clone the base class variables
        return copyBaseVariables( preProcessing );
    }
    
    errorLog << "clone(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << endl;
    
    return false;
}
    
bool Derivative::process(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return false;
    }
    
    computeDerivative( inputVector );
    
    if( processedData.size() == numOutputDimensions ) return true;
    return false;
}

bool Derivative::reset(){
    if( initialized ) return init(derivativeOrder, delta, numInputDimensions,filterData,filterSize);
    return false;
}
    
bool Derivative::saveModelToFile(string filename) const{
    
    if( !initialized ){
        errorLog << "saveModelToFile(string filename) - The DeadZone has not been initialized" << endl;
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

bool Derivative::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "GRT_DERIVATIVE_FILE_V1.0" << endl;
    
    file << "NumInputDimensions: " << numInputDimensions << endl;
    file << "NumOutputDimensions: " << numOutputDimensions << endl;
    file << "DerivativeOrder: " << derivativeOrder << endl;
    file << "FilterSize: " << filterSize << endl;	
    file << "Delta: " << delta << endl;
    file << "FilterData: " << filterData << endl;
    
    return true;
}

bool Derivative::loadModelFromFile(string filename){
    
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

bool Derivative::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_DERIVATIVE_FILE_V1.0" ){
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
    
    //Load the DerivativeOrder
    file >> word;
    if( word != "DerivativeOrder:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read DerivativeOrder header!" << endl;
        return false;     
    }
    file >> derivativeOrder;
    
    //Load the FilterSize
    file >> word;
    if( word != "FilterSize:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read FilterSize header!" << endl;
        return false;     
    }
    file >> filterSize;
    
    //Load the Delta
    file >> word;
    if( word != "Delta:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read Delta header!" << endl;
        return false;     
    }
    file >> delta;
    
    //Load if the data should be filtered 
    file >> word;
    if( word != "FilterData:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read FilterData header!" << endl;
        return false;     
    }
    file >> filterData;

    //Init the derivative module to ensure everything is initialized correctly
    return init(derivativeOrder,delta,numInputDimensions,filterData,filterSize);    
}
    
bool Derivative::init(UINT derivativeOrder,double delta,UINT numDimensions,bool filterData,UINT filterSize){

    initialized = false;
    
    if( derivativeOrder != FIRST_DERIVATIVE && derivativeOrder != SECOND_DERIVATIVE ){
        errorLog << "init(UINT derivativeOrder,double delta,UINT numDimensions,bool filterData,UINT filterSize) - Unknown derivativeOrder!" << endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(UINT derivativeOrder,double delta,UINT numDimensions,bool filterData,UINT filterSize) - NumDimensions must be greater than 0!" << endl;
        return false;
    }
    
    if( delta <= 0 ){
        errorLog << "init(UINT derivativeOrder,double delta,UINT numDimensions,bool filterData,UINT filterSize) - Delta must be greater than 0!" << endl;
        return false;
    }
    
    if( filterSize == 0 ){
        errorLog << "init(UINT derivativeOrder,double delta,UINT numDimensions,bool filterData,UINT filterSize) - FilterSize must be greater than zero!" << endl;
        return false;
    }
    
    this->derivativeOrder = derivativeOrder;
    this->delta = delta;
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numDimensions;
    this->filterData = filterData;
    this->filterSize = filterSize;
    filter.init(filterSize, numDimensions);
    yy.clear();
    yy.resize(numDimensions,0);
    yyy.clear();
    yyy.resize(numDimensions,0);
    processedData.clear();
    processedData.resize(numDimensions,0);
    initialized = true;
    return true;
}

double Derivative::computeDerivative(const double x){
    
    if( numInputDimensions != 1 ){
        errorLog << "computeDerivative(const double x) - The Number Of Input Dimensions is not 1! NumInputDimensions: " << numInputDimensions << endl;
        return 0;
    }
    
    VectorDouble y = computeDerivative( VectorDouble(1,x) );
    
    if( y.size() == 0 ) return 0 ;
    
	return y[0];
}
    
VectorDouble Derivative::computeDerivative(const VectorDouble &x){
    
    if( !initialized ){
        errorLog << "computeDerivative(const VectorDouble &x) - Not Initialized!" << endl;
        return vector<double>();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "computeDerivative(const VectorDouble &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << endl;
        return vector<double>();
    }
    
    VectorDouble y;
    if( filterData ){
        y = filter.filter( x );
    }else y = x;
    
    for(UINT n=0; n<numInputDimensions; n++){
        processedData[n] = (y[n]-yy[n])/delta;
        yy[n] = y[n];
    }
    
    if( derivativeOrder == SECOND_DERIVATIVE ){
        double tmp = 0;
        for(UINT n=0; n<numInputDimensions; n++){
            tmp = processedData[n];
            processedData[n] = (processedData[n]-yyy[n])/delta;
            yyy[n] = tmp;
        }
    }
    
    return processedData;
}
    
bool Derivative::setDerivativeOrder(UINT derivativeOrder){
    if( derivativeOrder == FIRST_DERIVATIVE || derivativeOrder == SECOND_DERIVATIVE ){
        this->derivativeOrder = derivativeOrder;
        if( initialized ) init(derivativeOrder, delta, numInputDimensions,filterData,filterSize);
        return true;
    }
    errorLog << "setDerivativeOrder(UINT derivativeOrder) - Unkown derivativeOrder" << endl;
    return false;
}

bool Derivative::setFilterSize(UINT filterSize){
    if( filterSize > 0  ){
        this->filterSize = filterSize;
        if( initialized ) init(derivativeOrder, delta, numInputDimensions,filterData,filterSize);
        return true;
    }
    errorLog << "setFilterSize(UINT filterSize) - FilterSize must be greater than zero!" << endl;
    return false;
}
    
bool Derivative::enableFiltering(bool filterData){
    this->filterData = filterData;
    if( initialized ) init(derivativeOrder, delta, numInputDimensions,filterData,filterSize);
    return true;
}
    
double Derivative::getDerivative(UINT derivativeOrder){ 
    
    switch( derivativeOrder ){
        case 0:
            return processedData[0];
            break;
        case( FIRST_DERIVATIVE ):
            return yy[0]; 
            break;
        case( SECOND_DERIVATIVE ):
            return yyy[0]; 
            break;
        default:
            warningLog << "getDerivative(UINT derivativeOrder) - Unkown derivativeOrder: " << derivativeOrder << endl;
            break;
    }
    
    return 0;
}

VectorDouble Derivative::getDerivatives(UINT derivativeOrder){ 
    
    switch( derivativeOrder ){
        case 0:
            return processedData;
            break;
        case( FIRST_DERIVATIVE ):
            return yy; 
            break;
        case( SECOND_DERIVATIVE ):
            return yyy; 
            break;
        default:
            warningLog << "getDerivative(UINT derivativeOrder) - Unkown derivativeOrder: " << derivativeOrder << endl;
            break;
    }
    
    return VectorDouble();
}

}//End of namespace GRT