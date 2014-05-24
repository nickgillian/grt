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

#include "Accumulator.h"

namespace GRT{
    
//Register the Accumulator module with the FeatureExtraction base class
RegisterFeatureExtractionModule< Accumulator > Accumulator::registerModule("Accumulator");
    
Accumulator::Accumulator(UINT numDimensions,double lastValueWeight){
    featureExtractionType = "Accumulator";
    debugLog.setProceedingText("[DEBUG Accumulator]");
    errorLog.setProceedingText("[ERROR Accumulator]");
    warningLog.setProceedingText("[WARNING Accumulator]");
    
    init(numDimensions, lastValueWeight);
}
    
Accumulator::Accumulator(const Accumulator &rhs){
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

Accumulator::~Accumulator(){
}
    
Accumulator& Accumulator::operator=(const Accumulator &rhs){
    if(this!=&rhs){
        //Copy the accumulator variables
        this->lastValue = rhs.lastValue;
        this->lastValueWeight = rhs.lastValueWeight;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}
    
bool Accumulator::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getFeatureExtractionType() == featureExtraction->getFeatureExtractionType() ){
        
        //Cast the feature extraction pointer to a pointer to your custom feature extraction module
        //Then invoke the equals operator
        *this = *(Accumulator*)featureExtraction;
        return true;
    }
    
    errorLog << "clone(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << endl;
    
    return false;
}
    
bool Accumulator::computeFeatures(const VectorDouble &inputVector){
    
#ifdef GRT_SAFE_CHECKING
    if( !initialized ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return false;
    }
#endif
    
    //Add up the input vector, then add this to the last value
    double sum = 0;
    for(UINT i=0; i<numInputDimensions; i++){
        sum += fabs( inputVector[i] );
    }
    //Set the feature value
    featureVector[0] = sum + lastValue;
    
    //Update the last value
    lastValue = featureVector[0] * lastValueWeight;
    
    //Flag that the feature data has been computed
    featureDataReady = true;
    
    return true;
}

bool Accumulator::reset(){
    if( !initialized ) return false;
    return init(numInputDimensions,lastValueWeight);
}
    
bool Accumulator::saveSettingsToFile(const string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveSettingsToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool Accumulator::loadSettingsFromFile(const string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadSettingsFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool Accumulator::saveSettingsToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveSettingsToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Here is where you should add your own custom code to save any settings to a file, this should be done in three steps...
    
    //First, you should add a header (with no spaces) e.g.
    file << "CUSTOM_FEATURE_EXTRACTION_FILE_V1.0" << endl;
	
    //Second, you should save the base feature extraction settings to the file
    if( !saveBaseSettingsToFile( file ) ){
        errorLog << "saveSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << endl;
        return false;
    }
    
    //Finally, you should add anything needed for your custom feature extraction module
    //For the accumulator we only need to save the lastValueWeight
    file << "LastValueWeight: " << lastValueWeight << endl;
    
    //NOTE: You should not close the file as this will be done by the function that calls this function
    return true;
}

bool Accumulator::loadSettingsFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadSettingsFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Here is where you should add your own custom code to load any settings to a file, this should be done in three steps...
    
    //First, you should read and validate the header
    file >> word;
    if( word != "CUSTOM_FEATURE_EXTRACTION_FILE_V1.0" ){
        errorLog << "loadSettingsFromFile(fstream &file) - Invalid file format!" << endl;
        return false;
    }
    
    //Second, you should load the base feature extraction settings to the file
    if( !loadBaseSettingsFromFile( file ) ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << endl;
        return false;
    }
    
    //Finally, you should load any custom contents    
    //For the accumulator we only need to load the lastValueWeight
    file >> word;
    if( word != "LastValueWeight:" ){
        errorLog << "loadSettingsFromFile(fstream &file) - Failed to load last value weight!" << endl;
        return false;
    }
    file >> lastValueWeight;
    
    //When you have loaded all your content you should then initialize the module
    return init(numInputDimensions,lastValueWeight);
}
    
bool Accumulator::init(UINT numDimensions,double lastValueWeight){
    
    initialized = false;
    
    if( numDimensions == 0 ){
        errorLog << "init( UINT numDimensions ) - The number of input dimensions must be greater than zero!" << endl;
        return false;
    }
    
    //Set the input and output dimensions, we know the number of outputs for the accumulator will be 1
    numInputDimensions = numDimensions;
    numOutputDimensions = 1;
    featureVector.resize(numOutputDimensions);
    
    //Reset the last value
    lastValue = 0;
    this->lastValueWeight = lastValueWeight;
    
    //Flag that the accumulator has been initialized but that no features have been computed yet
    initialized = true;
    featureDataReady = false;
    
    return true;
}
    
double Accumulator::getLastValueWeight(){
    if( !initialized ) return 0;
    return lastValueWeight;
}

double Accumulator::getLastValue(){
    if( !initialized ) return 0;
    return lastValue;
}
    
bool Accumulator::setLastValueWeight(double lastValueWeight){
    this->lastValueWeight = lastValueWeight;
    return true;
}
    
bool Accumulator::setLastValue(double lastValue){
    this->lastValue = lastValue;
    return true;
}
    
}//End of namespace GRT