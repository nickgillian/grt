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

#include "DeadZone.h"

namespace GRT{
    
//Register the DeadZone module with the PreProcessing base class
RegisterPreProcessingModule< DeadZone > DeadZone::registerModule("DeadZone");
    
DeadZone::DeadZone(double lowerLimit,double upperLimit,UINT numDimensions){
    classType = "DeadZone";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG DeadZone]");
    errorLog.setProceedingText("[ERROR DeadZone]");
    warningLog.setProceedingText("[WARNING DeadZone]");
    init(lowerLimit,upperLimit,numDimensions);
}

DeadZone::DeadZone(const DeadZone &rhs){
    this->lowerLimit = rhs.lowerLimit;
    this->upperLimit = rhs.upperLimit;
    classType = "DeadZone";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG DeadZone]");
    errorLog.setProceedingText("[ERROR DeadZone]");
    warningLog.setProceedingText("[WARNING DeadZone]");
    copyBaseVariables( (PreProcessing*)&rhs );
}

DeadZone::~DeadZone(){

}

DeadZone& DeadZone::operator=(const DeadZone &rhs){
	if(this!=&rhs){
        this->lowerLimit = rhs.lowerLimit;
        this->upperLimit = rhs.upperLimit;
        copyBaseVariables( (PreProcessing*)&rhs );
	}
	return *this;
}

bool DeadZone::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getPreProcessingType() == preProcessing->getPreProcessingType() ){
        
        DeadZone *ptr = (DeadZone*)preProcessing;
        //Clone the DeadZone values 
        this->lowerLimit = ptr->lowerLimit;
        this->upperLimit = ptr->upperLimit;
        
        //Clone the base class variables
        return copyBaseVariables( preProcessing );
    }
    
    errorLog << "clone(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << endl;
    
    return false;
}
    
bool DeadZone::process(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorDouble &inputVector) - Not initialized!" << endl;
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

bool DeadZone::reset(){
    return true;
}
    
bool DeadZone::saveModelToFile(string filename) const{
    
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

bool DeadZone::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "GRT_DEAD_ZONE_FILE_V1.0" << endl;
    
    file << "NumInputDimensions: " << numInputDimensions << endl;
    file << "NumOutputDimensions: " << numOutputDimensions << endl;
    file << "LowerLimit: " << lowerLimit << endl;
    file << "UpperLimit: " << upperLimit << endl;	
    
    return true;
}

bool DeadZone::loadModelFromFile(string filename){
    
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

bool DeadZone::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_DEAD_ZONE_FILE_V1.0" ){
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
    
    //Load the lower limit
    file >> word;
    if( word != "LowerLimit:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read LowerLimit header!" << endl;
        return false;     
    }
    file >> lowerLimit;
    
    file >> word;
    if( word != "UpperLimit:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read UpperLimit header!" << endl;
        return false;     
    }
    file >> upperLimit;
    
    //Init the deadzone module to ensure everything is initialized correctly
    return init(lowerLimit,upperLimit,numInputDimensions);
}
    
bool DeadZone::init(double lowerLimit,double upperLimit,UINT numDimensions){
    
    initialized = false;
    
    if( numDimensions == 0 ){
        errorLog << "init(double lowerLimit,double upperLimit,UINT numDimensions) - NumDimensions must be greater than 0!" << endl;
        return false;
    }
    
    if( lowerLimit >= upperLimit ){
        errorLog << "init(double lowerLimit,double upperLimit,UINT numDimensions) - The lower limit must be less than the upperlimit!" << endl;
        return false;
    }
    
    this->lowerLimit = lowerLimit;
    this->upperLimit = upperLimit;
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numDimensions;
    processedData.clear();
    processedData.resize(numOutputDimensions,0);
    initialized = true;
    
    return true;
}

double DeadZone::filter(const double x){
    VectorDouble y = filter(VectorDouble(1,x));
    if( y.size() == 0 ) return 0;
	return y[0];
}
    
VectorDouble DeadZone::filter(const VectorDouble &x){
    
    if( !initialized ){
        errorLog << "filter(const VectorDouble &x) - Not Initialized!" << endl;
        return VectorDouble();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "filter(const VectorDouble &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << endl;
        return VectorDouble();
    }
    
    for(UINT n=0; n<numInputDimensions; n++){
        if( x[n] > lowerLimit && x[n] < upperLimit ){
            processedData[n] = 0;
        }else{
            if( x[n] >= upperLimit ) processedData[n] = x[n] - upperLimit;
            else processedData[n] = x[n] - lowerLimit;
        }
    }
    return processedData;
}

bool DeadZone::setLowerLimit(double lowerLimit){ 
	this->lowerLimit = lowerLimit; 
	return true; 
}

bool DeadZone::setUpperLimit(double upperLimit){ 
	this->upperLimit = upperLimit; 
	return true; 
}
    
}//End of namespace GRT