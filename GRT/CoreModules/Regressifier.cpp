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
#include "Regressifier.h"

GRT_BEGIN_NAMESPACE
    
Regressifier::StringRegressifierMap* Regressifier::stringRegressifierMap = NULL;
UINT Regressifier::numRegressifierInstances = 0;

    
Regressifier* Regressifier::createNewInstance() const{ return create(); } //Legacy
Regressifier* Regressifier::createInstanceFromString( const std::string &id ){ return create(id); } //Legacy
std::string Regressifier::getRegressifierType() const{ return MLBase::getId(); } //Legacy
    
Regressifier* Regressifier::create( const std::string &id ){
    
    StringRegressifierMap::iterator iter = getMap()->find( id );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}
    
Regressifier* Regressifier::create() const{
    return create( MLBase::getId() );
}

Regressifier* Regressifier::deepCopy() const{
    
    Regressifier *newInstance = create( classId );
    
    if( newInstance == NULL ) return NULL;
    
    if( !newInstance->deepCopyFrom( this ) ){
        delete newInstance;
        return NULL;
    }
    return newInstance;
}
    
Regressifier::Regressifier( const std::string &id ) : MLBase( id, MLBase::REGRESSIFIER )
{
    numOutputDimensions = 0;
    numRegressifierInstances++;
}
    
Regressifier::~Regressifier(void){
    if( --numRegressifierInstances == 0 ){
        delete stringRegressifierMap;
        stringRegressifierMap = NULL;
    }
}

Vector< std::string > Regressifier::getRegisteredRegressifiers(){
    Vector< std::string > registeredRegressifiers;
    
    StringRegressifierMap::iterator iter = getMap()->begin();
    while( iter != getMap()->end() ){
        registeredRegressifiers.push_back( iter->first );
        ++iter; //++iter is faster than iter++ as it does not require a copy/move operator
    }
    return registeredRegressifiers;
}
    
bool Regressifier::copyBaseVariables(const Regressifier *regressifier){
    
    if( regressifier == NULL ){
        errorLog << "copyBaseVariables(Regressifier *regressifier) - regressifier pointer is NULL!" << std::endl;
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
    
    return true;
}

bool Regressifier::clear(){
    
    //Clear the MLBase variables
    MLBase::clear();
    
    //Clear the regressifier variables
    regressionData.clear();
    inputVectorRanges.clear();
    targetVectorRanges.clear();
    
    return true;
}
    
VectorFloat Regressifier::getRegressionData() const{ 
    if( trained ){ 
        return regressionData; 
    } 
    return VectorFloat(); 
}
    
Vector< MinMax > Regressifier::getInputRanges() const{
    return inputVectorRanges;
}

Vector< MinMax > Regressifier::getOutputRanges() const{
    return targetVectorRanges;
}
    
const Regressifier& Regressifier::getBaseRegressifier() const{
    return *this;
}
    
bool Regressifier::saveBaseSettingsToFile( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "saveBaseSettingsToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    //Write the ranges to the file
    if( useScaling ){
		file << "InputVectorRanges: \n";
		for(UINT j=0; j<numInputDimensions; j++){
			file << inputVectorRanges[j].minValue << "\t" << inputVectorRanges[j].maxValue << std::endl;
		}
        
		file << "OutputVectorRanges: \n";
		for(UINT j=0; j<numOutputDimensions; j++){
			file << targetVectorRanges[j].minValue << "\t" << targetVectorRanges[j].maxValue << std::endl;
		}
    }
    
    return true;
}

bool Regressifier::loadBaseSettingsFromFile( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Try and load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile( file ) ){
        return false;
    }
    
    std::string word;
    
    if( useScaling ){
        //Load the ranges
        file >> word;
        if( word != "InputVectorRanges:" ){
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read InputVectorRanges header!" << std::endl;
            return false;
        }
        inputVectorRanges.resize(numInputDimensions);
        for(UINT j=0; j<numInputDimensions; j++){
			file >> inputVectorRanges[j].minValue;
            file >> inputVectorRanges[j].maxValue;
		}
        
        file >> word;
        if( word != "OutputVectorRanges:" ){
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read OutputVectorRanges header!" << std::endl;
            return false;
        }
        targetVectorRanges.resize(numOutputDimensions);
        for(UINT j=0; j<numOutputDimensions; j++){
			file >> targetVectorRanges[j].minValue;
            file >> targetVectorRanges[j].maxValue;
		}
    }
    
    if( trained ){
        //Resize the regression data Vector
        regressionData.clear();
        regressionData.resize(numOutputDimensions,0);
    }
    
    return true;
}
    
GRT_END_NAMESPACE
