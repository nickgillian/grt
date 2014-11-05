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

#include "LeakyIntegrator.h"

namespace GRT{
    
//Register the LeakyIntegrator module with the PreProcessing base class
RegisterPreProcessingModule< LeakyIntegrator > LeakyIntegrator::registerModule("LeakyIntegrator");
    
LeakyIntegrator::LeakyIntegrator(const double leakRate,const UINT numDimensions){
    classType = "LeakyIntegrator";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG LeakyIntegrator]");
    errorLog.setProceedingText("[ERROR LeakyIntegrator]");
    warningLog.setProceedingText("[WARNING LeakyIntegrator]");
    init(leakRate,numDimensions);
}
    
LeakyIntegrator::LeakyIntegrator(const LeakyIntegrator &rhs){
    
    classType = "LeakyIntegrator";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG LeakyIntegrator]");
    errorLog.setProceedingText("[ERROR LeakyIntegrator]");
    warningLog.setProceedingText("[WARNING LeakyIntegrator]");
    
    this->leakRate = rhs.leakRate;
    this->y = rhs.y;
    
    copyBaseVariables( (PreProcessing*)&rhs );
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
    
    if( this->getPreProcessingType() == preProcessing->getPreProcessingType() ){
        
        LeakyIntegrator *ptr = (LeakyIntegrator*)preProcessing;
        
        //Clone the LeakyIntegrator values 
        this->leakRate = ptr->leakRate;
        this->y = ptr->y;
        
        //Clone the base class variables
        return copyBaseVariables( preProcessing );
    }
    
    errorLog << "clone(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << endl;
    
    return false;
}
    
bool LeakyIntegrator::process(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
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
    
bool LeakyIntegrator::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "GRT_LEAKY_INTEGRATOR_FILE_V1.0" << endl;
    
    if( !PreProcessing::savePreProcessingSettingsToFile( file ) ){
        errorLog << "savePreProcessingSettingsToFile(fstream &file) - Failed to save preprocessing settings to file!" << endl;
        return false;
    }
    
    file << "LeakRate: " << leakRate << endl;
    
    return true;
}

bool LeakyIntegrator::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_LEAKY_INTEGRATOR_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    if( !PreProcessing::loadPreProcessingSettingsFromFile( file ) ){
        errorLog << "loadPreProcessingSettingsFromFile(fstream &file) - Failed to load preprocessing settings from file!" << endl;
        return false;
    }
    
    //Load the LeakRate
    file >> word;
    if( word != "LeakRate:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read LeakRate header!" << endl;
        return false;     
    }
    file >> leakRate;

    //Init the LeakyIntegrator module to ensure everything is initialized correctly
    return init(leakRate,numInputDimensions);
}
    
bool LeakyIntegrator::init(const double leakRate,const UINT numDimensions){

    initialized = false;
    
    if( leakRate < 0 || leakRate > 1 ){
        errorLog << "init(const double leakRate,const UINT numDimensions) - leakRate must be between [0 1]!" << endl;
        return false;
    }
    
    if( numDimensions == 0 ){
        errorLog << "init(const double leakRate,const UINT numDimensions) - NumDimensions must be greater than 0!" << endl;
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

double LeakyIntegrator::update(const double x){
    
    if( numInputDimensions != 1 ){
        errorLog << "update(const double x) - The Number Of Input Dimensions is not 1! NumInputDimensions: " << numInputDimensions << endl;
        return 0;
    }
    
    y = update( VectorDouble(1,x) );
    
    if( y.size() == 0 ) return 0 ;
    
	return y[0];
}
    
VectorDouble LeakyIntegrator::update(const VectorDouble &x){
    
    if( !initialized ){
        errorLog << "update(const VectorDouble &x) - Not Initialized!" << endl;
        return vector<double>();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "update(const VectorDouble &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << endl;
        return vector<double>();
    }
    
    for(UINT n=0; n<numInputDimensions; n++){
        y[n] = y[n]*leakRate + x[n];
    }
    processedData = y;
    
    return processedData;
}
    
bool LeakyIntegrator::setLeakRate(const double leakRate){
    if( leakRate >= 0 && leakRate <= 1 ){
        this->leakRate = leakRate;
        if( initialized ) init(leakRate, numInputDimensions);
        return true;
    }
    errorLog << "setLeakRate(const double leakRate) - The leak rate must be between [0 1]!" << endl;
    return false;
}

double LeakyIntegrator::getLeakRate(){
    return leakRate;
}

}//End of namespace GRT