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
#include "Derivative.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string Derivative::id = "Derivative";
std::string Derivative::getId() { return Derivative::id; }

//Register the Derivative module with the PreProcessing base class
RegisterPreProcessingModule< Derivative > Derivative::registerModule( Derivative::getId() );

Derivative::Derivative(const UINT derivativeOrder,const Float delta,const UINT numDimensions,const bool filterData,const UINT filterSize) : PreProcessing( Derivative::getId() )
{
    init(derivativeOrder,delta,numDimensions,filterData,filterSize);
}

Derivative::Derivative(const Derivative &rhs) : PreProcessing( Derivative::getId() )
{
    
    this->derivativeOrder = rhs.derivativeOrder;
    this->filterSize = rhs.filterSize;
    this->delta = rhs.delta;
    this->filterData = rhs.filterData;
    this->filter = rhs.filter;
    this->yy = rhs.yy;
    this->yyy = rhs.yyy;
    
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
    
    if( this->getId() == preProcessing->getId() ){
        
        const Derivative *ptr = dynamic_cast<const Derivative*>(preProcessing);
        
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
    
    errorLog << "deepCopyFrom(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << std::endl;
    
    return false;
}

bool Derivative::process(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "process(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
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

bool Derivative::save(std::fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    file << "GRT_DERIVATIVE_FILE_V1.0" << std::endl;
    
    file << "NumInputDimensions: " << numInputDimensions << std::endl;
    file << "NumOutputDimensions: " << numOutputDimensions << std::endl;
    file << "DerivativeOrder: " << derivativeOrder << std::endl;
    file << "FilterSize: " << filterSize << std::endl;
    file << "Delta: " << delta << std::endl;
    file << "FilterData: " << filterData << std::endl;
    
    return true;
}

bool Derivative::load(std::fstream &file){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_DERIVATIVE_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    //Load the number of input dimensions
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog << "load(fstream &file) - Failed to read NumInputDimensions header!" << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    //Load the number of output dimensions
    file >> word;
    if( word != "NumOutputDimensions:" ){
        errorLog << "load(fstream &file) - Failed to read NumOutputDimensions header!" << std::endl;
        return false;
    }
    file >> numOutputDimensions;
    
    //Load the DerivativeOrder
    file >> word;
    if( word != "DerivativeOrder:" ){
        errorLog << "load(fstream &file) - Failed to read DerivativeOrder header!" << std::endl;
        return false;
    }
    file >> derivativeOrder;
    
    //Load the FilterSize
    file >> word;
    if( word != "FilterSize:" ){
        errorLog << "load(fstream &file) - Failed to read FilterSize header!" << std::endl;
        return false;
    }
    file >> filterSize;
    
    //Load the Delta
    file >> word;
    if( word != "Delta:" ){
        errorLog << "load(fstream &file) - Failed to read Delta header!" << std::endl;
        return false;
    }
    file >> delta;
    
    //Load if the data should be filtered
    file >> word;
    if( word != "FilterData:" ){
        errorLog << "load(fstream &file) - Failed to read FilterData header!" << std::endl;
        return false;
    }
    file >> filterData;
    
    //Init the derivative module to ensure everything is initialized correctly
    return init(derivativeOrder,delta,numInputDimensions,filterData,filterSize);
}

bool Derivative::init(const UINT derivativeOrder,const Float delta,const UINT numDimensions,const bool filterData,const UINT filterSize){
    
    initialized = false;
    
    if( derivativeOrder != FIRST_DERIVATIVE && derivativeOrder != SECOND_DERIVATIVE ){
        errorLog << "init(UINT derivativeOrder,Float delta,UINT numDimensions,bool filterData,UINT filterSize) - Unknown derivativeOrder!" << std::endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(UINT derivativeOrder,Float delta,UINT numDimensions,bool filterData,UINT filterSize) - NumDimensions must be greater than 0!" << std::endl;
        return false;
    }
    
    if( delta <= 0 ){
        errorLog << "init(UINT derivativeOrder,Float delta,UINT numDimensions,bool filterData,UINT filterSize) - Delta must be greater than 0!" << std::endl;
        return false;
    }
    
    if( filterSize == 0 ){
        errorLog << "init(UINT derivativeOrder,Float delta,UINT numDimensions,bool filterData,UINT filterSize) - FilterSize must be greater than zero!" << std::endl;
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

Float Derivative::computeDerivative(const Float x){
    
    if( numInputDimensions != 1 ){
        errorLog << "computeDerivative(const Float x) - The Number Of Input Dimensions is not 1! NumInputDimensions: " << numInputDimensions << std::endl;
        return 0;
    }
    
    VectorFloat y = computeDerivative( VectorFloat(1,x) );
    
    if( y.size() == 0 ) return 0 ;
    
    return y[0];
}

VectorFloat Derivative::computeDerivative(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "computeDerivative(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "computeDerivative(const VectorFloat &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << std::endl;
        return VectorFloat();
    }
    
    VectorFloat y;
    if( filterData ){
        y = filter.filter( x );
    }else y = x;
    
    for(UINT n=0; n<numInputDimensions; n++){
        processedData[n] = (y[n]-yy[n])/delta;
        yy[n] = y[n];
    }
    
    if( derivativeOrder == SECOND_DERIVATIVE ){
        Float tmp = 0;
        for(UINT n=0; n<numInputDimensions; n++){
            tmp = processedData[n];
            processedData[n] = (processedData[n]-yyy[n])/delta;
            yyy[n] = tmp;
        }
    }
    
    return processedData;
}

bool Derivative::setDerivativeOrder(const UINT derivativeOrder){
    if( derivativeOrder == FIRST_DERIVATIVE || derivativeOrder == SECOND_DERIVATIVE ){
        this->derivativeOrder = derivativeOrder;
        if( initialized ) init(derivativeOrder, delta, numInputDimensions,filterData,filterSize);
        return true;
    }
    errorLog << "setDerivativeOrder(UINT derivativeOrder) - Unkown derivativeOrder" << std::endl;
    return false;
}

bool Derivative::setFilterSize(const UINT filterSize){
    if( filterSize > 0  ){
        this->filterSize = filterSize;
        if( initialized ) init(derivativeOrder, delta, numInputDimensions,filterData,filterSize);
        return true;
    }
    errorLog << "setFilterSize(UINT filterSize) - FilterSize must be greater than zero!" << std::endl;
    return false;
}

bool Derivative::enableFiltering(const bool filterData){
    this->filterData = filterData;
    if( initialized ) init(derivativeOrder, delta, numInputDimensions,filterData,filterSize);
    return true;
}

UINT Derivative::getFilterSize() const { 
    if( initialized ){ return filterSize; } 
    return 0; 
}

Float Derivative::getDerivative(const UINT derivativeOrder) const {
    
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
        warningLog << "getDerivative(UINT derivativeOrder) - Unkown derivativeOrder: " << derivativeOrder << std::endl;
        break;
    }
    
    return 0;
}

VectorFloat Derivative::getDerivatives(const UINT derivativeOrder) const {
    
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
        warningLog << "getDerivative(UINT derivativeOrder) - Unkown derivativeOrder: " << derivativeOrder << std::endl;
        break;
    }
    
    return VectorFloat();
}

GRT_END_NAMESPACE
