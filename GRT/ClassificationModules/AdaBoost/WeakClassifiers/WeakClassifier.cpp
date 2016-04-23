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
 */

#define GRT_DLL_EXPORTS
#include "WeakClassifier.h"

GRT_BEGIN_NAMESPACE
    
WeakClassifier::StringWeakClassifierMap* WeakClassifier::stringWeakClassifierMap = NULL;
UINT WeakClassifier::numWeakClassifierInstances = 0;

WeakClassifier* WeakClassifier::createInstanceFromString( std::string const &weakClassifierType ){
    
    StringWeakClassifierMap::iterator iter = getMap()->find( weakClassifierType );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}
    
WeakClassifier::WeakClassifier(){
    weakClassifierType = "";
    trained = false;
    numInputDimensions = 0;
    numWeakClassifierInstances++;
}
    
WeakClassifier::~WeakClassifier(){
    if( --numWeakClassifierInstances == 0 ){
        delete stringWeakClassifierMap;
        stringWeakClassifierMap = NULL;
    }
}
    
WeakClassifier& WeakClassifier::operator=(const WeakClassifier &rhs){
    if( this != &rhs ){
        this->weakClassifierType = rhs.weakClassifierType;
        this->trained = rhs.trained;
        this->numInputDimensions = rhs.numInputDimensions;
        this->trainingLog = rhs.trainingLog;
        this->errorLog = rhs.errorLog;
        this->warningLog = rhs.warningLog;
    }
    return *this;
}
    
bool WeakClassifier::copyBaseVariables(const WeakClassifier *weakClassifer){
    if( weakClassifer == NULL ){
        errorLog << "copyBaseVariables(const WeakClassifier *rhs) rhs is NULL!" << std::endl;
        return false;
    }
    this->weakClassifierType = weakClassifer->weakClassifierType;
    this->trained = weakClassifer->trained;
    this->numInputDimensions = weakClassifer->numInputDimensions;
    return true;
}
    
WeakClassifier* WeakClassifier::createNewInstance() const{
    return createInstanceFromString( weakClassifierType );
}

GRT_END_NAMESPACE

