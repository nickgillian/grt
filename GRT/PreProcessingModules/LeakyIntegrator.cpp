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
#include "LeakyIntegrator.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string LeakyIntegrator::id = "LeakyIntegrator";
std::string LeakyIntegrator::getId() { return LeakyIntegrator::id; }

//Register the LeakyIntegrator module with the PreProcessing base class
RegisterPreProcessingModule< LeakyIntegrator > LeakyIntegrator::registerModule( LeakyIntegrator::getId() );

LeakyIntegrator::LeakyIntegrator(const Float leakRate,const UINT numDimensions) : PreProcessing( LeakyIntegrator::getId() )
{
    init(leakRate,numDimensions);
}

LeakyIntegrator::LeakyIntegrator(const LeakyIntegrator &rhs) : PreProcessing( LeakyIntegrator::getId() )
{
    *this = rhs;
}

LeakyIntegrator::~LeakyIntegrator(){
    
}

LeakyIntegrator& LeakyIntegrator::operator=(const LeakyIntegrator &rhs){
    if( this != &rhs ){
        this->leakRate = rhs.leakRate;
        this->y = rhs.y;
        copyBaseVariables( (PreProcessing*)&rhs );
    }
    return *this;
}

bool LeakyIntegrator::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getId() == preProcessing->getId() ){
        
        const LeakyIntegrator *ptr = dynamic_cast<const LeakyIntegrator*>(preProcessing);
        
        //Clone the LeakyIntegrator values
        this->leakRate = ptr->leakRate;
        this->y = ptr->y;
        
        //Clone the base class variables
        return copyBaseVariables( preProcessing );
    }
    
    errorLog << "deepCopyFrom(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << std::endl;
    
    return false;
}

bool LeakyIntegrator::process(const VectorFloat &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
    
    update( inputVector );
    
    if( processedData.size() == numOutputDimensions ) return true;
    return false;
}

bool LeakyIntegrator::reset(){
    if( initialized ) return init(leakRate, numInputDimensions);
    return false;
}

bool LeakyIntegrator::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    file << "GRT_LEAKY_INTEGRATOR_FILE_V1.0" << std::endl;
    
    if( !PreProcessing::savePreProcessingSettingsToFile( file ) ){
        errorLog << "savePreProcessingSettingsToFile(fstream &file) - Failed to save preprocessing settings to file!" << std::endl;
        return false;
    }
    
    file << "LeakRate: " << leakRate << std::endl;
    
    return true;
}

bool LeakyIntegrator::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_LEAKY_INTEGRATOR_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    if( !PreProcessing::loadPreProcessingSettingsFromFile( file ) ){
        errorLog << "loadPreProcessingSettingsFromFile(fstream &file) - Failed to load preprocessing settings from file!" << std::endl;
        return false;
    }
    
    //Load the LeakRate
    file >> word;
    if( word != "LeakRate:" ){
        errorLog << "load(fstream &file) - Failed to read LeakRate header!" << std::endl;
        return false;
    }
    file >> leakRate;
    
    //Init the LeakyIntegrator module to ensure everything is initialized correctly
    return init(leakRate,numInputDimensions);
}

bool LeakyIntegrator::init(const Float leakRate,const UINT numDimensions){
    
    initialized = false;
    
    if( leakRate < 0 || leakRate > 1 ){
        errorLog << "init(const Float leakRate,const UINT numDimensions) - leakRate must be between [0 1]!" << std::endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(const Float leakRate,const UINT numDimensions) - NumDimensions must be greater than 0!" << std::endl;
        return false;
    }
    
    this->leakRate = leakRate;
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numDimensions;
    y.clear();
    y.resize(numDimensions,0);
    processedData.clear();
    processedData.resize(numDimensions,0);
    initialized = true;
    return true;
}

Float LeakyIntegrator::update(const Float x){
    
    if( numInputDimensions != 1 ){
        errorLog << "update(const Float x) - The Number Of Input Dimensions is not 1! NumInputDimensions: " << numInputDimensions << std::endl;
        return 0;
    }
    
    y = update( VectorFloat(1,x) );
    
    if( y.size() == 0 ) return 0 ;
    
    return y[0];
}

VectorFloat LeakyIntegrator::update(const VectorFloat &x){
    
    if( !initialized ){
        errorLog << "update(const VectorFloat &x) - Not Initialized!" << std::endl;
        return VectorFloat();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "update(const VectorFloat &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << std::endl;
        return VectorFloat();
    }
    
    for(UINT n=0; n<numInputDimensions; n++){
        y[n] = y[n]*leakRate + x[n];
    }
    processedData = y;
    
    return processedData;
}

bool LeakyIntegrator::setLeakRate(const Float leakRate){
    if( leakRate >= 0 && leakRate <= 1 ){
        this->leakRate = leakRate;
        if( initialized ) init(leakRate, numInputDimensions);
        return true;
    }
    errorLog << "setLeakRate(const Float leakRate) - The leak rate must be between [0 1]!" << std::endl;
    return false;
}

Float LeakyIntegrator::getLeakRate() const {
    return leakRate;
}

GRT_END_NAMESPACE
