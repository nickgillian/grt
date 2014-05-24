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

#include "LowPassFilter.h"

namespace GRT{
    
//Register the LowPassFilter module with the PreProcessing base class
RegisterPreProcessingModule< LowPassFilter > LowPassFilter::registerModule("LowPassFilter");
    
LowPassFilter::LowPassFilter(double filterFactor,double gain,UINT numDimensions,double cutoffFrequency,double delta){
    
    classType = "LowPassFilter";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG LowPassFilter]");
    errorLog.setProceedingText("[ERROR LowPassFilter]");
    warningLog.setProceedingText("[WARNING LowPassFilter]");
    init(filterFactor,gain,numDimensions);
    
    if( cutoffFrequency != -1 && delta != -1 ){
        setCutoffFrequency(cutoffFrequency, delta);
    }
}

LowPassFilter::LowPassFilter(const LowPassFilter &rhs){
    classType = "LowPassFilter";
    preProcessingType = classType;
    debugLog.setProceedingText("[DEBUG LowPassFilter]");
    errorLog.setProceedingText("[ERROR LowPassFilter]");
    warningLog.setProceedingText("[WARNING LowPassFilter]");
    *this = rhs;
}
    
LowPassFilter::~LowPassFilter(){

}

LowPassFilter& LowPassFilter::operator=(const LowPassFilter &rhs){
	if(this!=&rhs){
        this->filterFactor = rhs.filterFactor;
        this->gain = rhs.gain;
		this->yy = rhs.yy;
        copyBaseVariables( (PreProcessing*)&rhs );
	}
	return *this;
}
    
bool LowPassFilter::deepCopyFrom(const PreProcessing *preProcessing){
    
    if( preProcessing == NULL ) return false;
    
    if( this->getPreProcessingType() == preProcessing->getPreProcessingType() ){
        
        //Call the equals operator
        *this = *(LowPassFilter*)preProcessing;
        
        return true;
    }
    
    errorLog << "clone(const PreProcessing *preProcessing) -  PreProcessing Types Do Not Match!" << endl;
    
    return false;
}
    
bool LowPassFilter::process(const VectorDouble &inputVector){
    
    if( !initialized ){
        errorLog << "process(const VectorDouble &inputVector) - Not initialized!" << endl;
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

bool LowPassFilter::reset(){
    if( initialized ) return init(filterFactor,gain,numInputDimensions);
    return false;
}
    
bool LowPassFilter::saveModelToFile(string filename) const{
    
    if( !initialized ){
        errorLog << "saveModelToFile(string filename) - The HighPassFilter has not been initialized" << endl;
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

bool LowPassFilter::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "GRT_LOW_PASS_FILTER_FILE_V1.0" << endl;
    
    file << "NumInputDimensions: " << numInputDimensions << endl;
    file << "NumOutputDimensions: " << numOutputDimensions << endl;
    file << "FilterFactor: " << filterFactor << endl;
    file << "Gain: " << gain << endl;
    
    return true;
}

bool LowPassFilter::loadModelFromFile(string filename){
    
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

bool LowPassFilter::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_LOW_PASS_FILTER_FILE_V1.0" ){
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
    if( word != "FilterFactor:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read FilterFactor header!" << endl;
        return false;     
    }
    file >> filterFactor;
    
    //Load the number of output dimensions
    file >> word;
    if( word != "Gain:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read Gain header!" << endl;
        return false;     
    }
    file >> gain;
    
    //Init the filter module to ensure everything is initialized correctly
    return init(filterFactor,gain,numInputDimensions);  
}
    
bool LowPassFilter::init(double filterFactor,double gain,UINT numDimensions){
    
    initialized = false;
    
    if( numDimensions == 0 ){
        errorLog << "init(double filterFactor,double gain,UINT numDimensions) - NumDimensions must be greater than 0!" << endl;
        return false;
    }
    
    if( filterFactor <= 0 ){
        errorLog << "init(double filterFactor,double gain,UINT numDimensions) - FilterFactor must be greater than 0!" << endl;
        return false;
    }
    
    if( gain <= 0 ){
        errorLog << "init(double filterFactor,double gain,UINT numDimensions) - Gain must be greater than 0!" << endl;
        return false;
    }
    
    this->filterFactor = filterFactor;
    this->gain = gain;
    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = numDimensions;
    yy.clear();
    yy.resize(numDimensions,0);
    processedData.clear();
    processedData.resize(numDimensions,0);
    initialized = true;
    
    return true;
}

double LowPassFilter::filter(double x){
    
    //If the filter has not been initialised then return 0, otherwise filter x and return y
    if( !initialized ){
        errorLog << "filter(double x) - The filter has not been initialized!" << endl;
        return 0;
    }
    
    VectorDouble y = filter(VectorDouble(1,x));
    
    if( y.size() == 0 ) return 0;
    return y[0];

}
    
VectorDouble LowPassFilter::filter(const VectorDouble &x){
    
    if( !initialized ){
        errorLog << "filter(const VectorDouble &x) - Not Initialized!" << endl;
        return VectorDouble();
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "filter(const VectorDouble &x) - The Number Of Input Dimensions (" << numInputDimensions << ") does not match the size of the input vector (" << x.size() << ")!" << endl;
        return VectorDouble();
    }
    
    for(UINT n=0; n<numInputDimensions; n++){
        processedData[n] = (x[n] * filterFactor) + (yy[n] * (1.0 - filterFactor)) * gain;
        yy[n] = processedData[n];
    }
    return processedData;
}
    
bool LowPassFilter::setGain(double gain){
    if( gain > 0 ){
        this->gain = gain;
        reset();
        return true;
    }
    errorLog << "setGain(double gain) - Gain value must be greater than 0!" << endl;
    return false;
}

bool LowPassFilter::setFilterFactor(double filterFactor){
    if( filterFactor > 0 ){
        this->filterFactor = filterFactor;
        reset();
        return true;
    }
    errorLog << "setFilterFactor(double filterFactor) - FilterFactor value must be greater than 0!" << endl;
    return false;
}
    
bool LowPassFilter::setCutoffFrequency(double cutoffFrequency,double delta){
    if( cutoffFrequency > 0 && delta > 0 ){
        double RC = (1.0/TWO_PI) /cutoffFrequency;
        filterFactor = delta / (RC+delta);
        reset();
        return true;
    }
    return false;
}


}//End of namespace GRT
