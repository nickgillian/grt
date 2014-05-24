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

#include "Regressifier.h"
namespace GRT{
    
Regressifier::StringRegressifierMap* Regressifier::stringRegressifierMap = NULL;
UINT Regressifier::numRegressifierInstances = 0;
    
Regressifier* Regressifier::createInstanceFromString(string const &regressifierType){
    
    StringRegressifierMap::iterator iter = getMap()->find( regressifierType );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}
    
Regressifier* Regressifier::createNewInstance() const{
    return createInstanceFromString( regressifierType );
}

Regressifier* Regressifier::deepCopy() const{
    
    Regressifier *newInstance = createInstanceFromString( regressifierType );
    
    if( newInstance == NULL ) return NULL;
    
    if( !newInstance->deepCopyFrom( this ) ){
        delete newInstance;
        return NULL;
    }
    return newInstance;
}
    
Regressifier::Regressifier(void){
    baseType = MLBase::REGRESSIFIER;
    regressifierType = "NOT_SET";
    numOutputDimensions = 0;
    numRegressifierInstances++;
}
    
Regressifier::~Regressifier(void){
    if( --numRegressifierInstances == 0 ){
        delete stringRegressifierMap;
        stringRegressifierMap = NULL;
    }
}
    
bool Regressifier::copyBaseVariables(const Regressifier *regressifier){
    
    if( regressifier == NULL ){
        errorLog << "copyBaseVariables(Regressifier *regressifier) - regressifier pointer is NULL!" << endl;
        return false;
    }
    
    if( !this->copyMLBaseVariables( regressifier ) ){
        return false;
    }

    this->regressifierType = regressifier->regressifierType;
    this->regressionData = regressifier->regressionData;
    this->inputVectorRanges = regressifier->inputVectorRanges;
    this->targetVectorRanges = regressifier->targetVectorRanges;
    
    return true;
}
    
bool Regressifier::reset(){
    
    //Reset the base class
    MLBase::reset();
    
    rootMeanSquaredTrainingError = 0;
    totalSquaredTrainingError = 0;
    return true;
}

bool Regressifier::clear(){
    
    //Clear the MLBase variables
    MLBase::clear();
    
    //Clear the regressifier variables
    rootMeanSquaredTrainingError = 0;
    totalSquaredTrainingError = 0;
    regressionData.clear();
    inputVectorRanges.clear();
    targetVectorRanges.clear();
    
    return true;
}

string Regressifier::getRegressifierType() const{ 
    return regressifierType; 
}
    
VectorDouble Regressifier::getRegressionData() const{ 
    if( trained ){ 
        return regressionData; 
    } 
    return VectorDouble(); 
}
    
vector< MinMax > Regressifier::getInputRanges() const{
    return inputVectorRanges;
}

vector< MinMax > Regressifier::getOutputRanges() const{
    return targetVectorRanges;
}
    
const Regressifier& Regressifier::getBaseRegressifier() const{
    return *this;
}
    
    
bool Regressifier::saveBaseSettingsToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveBaseSettingsToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    //Write the ranges to the file
    if( useScaling ){
		file << "InputVectorRanges: \n";
		for(UINT j=0; j<numInputDimensions; j++){
			file << inputVectorRanges[j].minValue << "\t" << inputVectorRanges[j].maxValue << endl;
		}
		file << endl;
        
		file << "OutputVectorRanges: \n";
		for(UINT j=0; j<numOutputDimensions; j++){
			file << targetVectorRanges[j].minValue << "\t" << targetVectorRanges[j].maxValue << endl;
		}
		file << endl;
    }
    
    return true;
}


bool Regressifier::loadBaseSettingsFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Try and load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile( file ) ){
        return false;
    }
    
    string word;
    
    if( useScaling ){
        //Load the ranges
        file >> word;
        if( word != "InputVectorRanges:" ){
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read InputVectorRanges header!" << endl;
            return false;
        }
        inputVectorRanges.resize(numInputDimensions);
        for(UINT j=0; j<numInputDimensions; j++){
			file >> inputVectorRanges[j].minValue;
            file >> inputVectorRanges[j].maxValue;
		}
        
        file >> word;
        if( word != "OutputVectorRanges:" ){
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read OutputVectorRanges header!" << endl;
            return false;
        }
        targetVectorRanges.resize(numOutputDimensions);
        for(UINT j=0; j<numOutputDimensions; j++){
			file >> targetVectorRanges[j].minValue;
            file >> targetVectorRanges[j].maxValue;
		}
    }
    
    if( trained ){
        //Resize the regression data vector
        regressionData.resize(numOutputDimensions,0);
    }
    
    return true;
}
    
} //End of namespace GRT

