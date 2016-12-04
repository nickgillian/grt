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
#include "DecisionStump.h"

GRT_BEGIN_NAMESPACE
    
//Register the DecisionStump module with the WeakClassifier base class
RegisterWeakClassifierModule< DecisionStump > DecisionStump::registerModule("DecisionStump");
    
DecisionStump::DecisionStump(const UINT numRandomSplits){
    this->numRandomSplits = numRandomSplits;
    trained = false;
    numInputDimensions = 0;
    decisionFeatureIndex = 0;
    decisionValue = 0;
    direction = 0;
    weakClassifierType = "DecisionStump";
    trainingLog.setKey("[TRAINING DecisionStump]");
    warningLog.setKey("[WARNING DecisionStump]");
    errorLog.setKey("[ERROR DecisionStump]");
}
    
DecisionStump::~DecisionStump(){
    
}

DecisionStump::DecisionStump(const DecisionStump &rhs){
    *this = rhs;
}

DecisionStump& DecisionStump::operator=(const DecisionStump &rhs){
    if( this != &rhs ){
        this->decisionFeatureIndex = rhs.decisionFeatureIndex;
        this->decisionValue = rhs.decisionValue;
        this->direction = rhs.direction;
        this->numRandomSplits = rhs.numRandomSplits;
        this->copyBaseVariables( &rhs );
    }
    return *this;
}

bool DecisionStump::deepCopyFrom(const WeakClassifier *weakClassifer){
    if( weakClassifer == NULL ) return false;
    
    if( this->getWeakClassifierType() == weakClassifer->getWeakClassifierType() ){
        *this = *(DecisionStump*)weakClassifer;
        return true;
    }
    return false;
}

bool DecisionStump::train(ClassificationData &trainingData, VectorFloat &weights){
    
    trained = false;
    numInputDimensions = trainingData.getNumDimensions();
    
    //There should only be two classes in the dataset, the positive class (classLable==1) and the negative class (classLabel==2)
    if( trainingData.getNumClasses() != 2 ){
        errorLog << "train(ClassificationData &trainingData, VectorFloat &weights) - There should only be 2 classes in the training data, but there are : " << trainingData.getNumClasses() << std::endl;
        return false;
    }
    
    //There should be one weight for every training sample
    if( trainingData.getNumSamples() != weights.size() ){
        errorLog << "train(ClassificationData &trainingData, VectorFloat &weights) - There number of examples in the training data (" << trainingData.getNumSamples() << ") does not match the lenght of the weights vector (" << weights.getSize() << ")" << std::endl;
        return false;
    }
    
    //Pick the training sample to use as the stump feature
    const UINT M = trainingData.getNumSamples();
    UINT bestFeatureIndex = 0;
    Vector< MinMax > ranges = trainingData.getRanges();
    Float minError = grt_numeric_limits< Float >::max();
    Float minRange = 0;
    Float maxRange = 0;
    Float step = 0;
    Float threshold = 0;
    Float bestThreshold = 0;
    Random random;
    
    for(UINT k=0; k<numRandomSplits; k++){
        
        //Randomly select a feature and a threshold
        UINT n = random.getRandomNumberInt(0,numInputDimensions);
        minRange = ranges[n].minValue;
        maxRange = ranges[n].maxValue;
        threshold = random.getRandomNumberUniform( minRange, maxRange );
            
        //Compute the error using the current threshold on the current input dimension
        //We need to check both sides of the threshold
        Float rhsError = 0;
        Float lhsError = 0;
        for(UINT i=0; i<M; i++){
            bool positiveClass = trainingData[ i ].getClassLabel() == WEAK_CLASSIFIER_POSITIVE_CLASS_LABEL;
            bool rhs = trainingData[ i ][ n ] >= threshold;
            bool lhs = trainingData[ i ][ n ] <= threshold;
            if( (rhs && !positiveClass) || (!rhs && positiveClass) ) rhsError += weights[ i ];
            if( (lhs && !positiveClass) || (!lhs && positiveClass) ) lhsError += weights[ i ];
        }
            
        //Check to see if either the rhsError or lhsError beats the minError, if so then store the results
        if( rhsError < minError ){
            minError = rhsError;
            bestFeatureIndex = n;
            bestThreshold = threshold;
            direction = 1; //1 means rhs
        }
        if( lhsError < minError ){
            minError = lhsError;
            bestFeatureIndex = n;
            bestThreshold = threshold;
            direction = 0; //0 means lhs
        }
            
    }
    
    decisionFeatureIndex = bestFeatureIndex;
    decisionValue = bestThreshold;
    trained = true;
    
    trainingLog << "Best Feature Index: " << decisionFeatureIndex << " Value: " << decisionValue << " Direction: " << direction << " Error: " << minError << std::endl;
    return true;
}

Float DecisionStump::predict(const VectorFloat &x){
    if( direction == 1){
        if( x[ decisionFeatureIndex ] >= decisionValue ) return 1;
    }else if( x[ decisionFeatureIndex ] <= decisionValue ) return 1;
    return -1;
}
    
bool DecisionStump::saveModelToFile( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
	//Write the WeakClassifierType data
    file << "WeakClassifierType: " << weakClassifierType << std::endl;
    file << "Trained: "<< trained << std::endl;
    file << "NumInputDimensions: " << numInputDimensions << std::endl;
    
    //Write the DecisionStump data
    file << "DecisionFeatureIndex: " << decisionFeatureIndex << std::endl;
    file << "Direction: "<< direction << std::endl;
    file << "NumRandomSplits: " << numRandomSplits << std::endl;
    file << "DecisionValue: " << decisionValue << std::endl;
    
    //We don't need to close the file as the function that called this function should handle that
    return true;
}

bool DecisionStump::loadModelFromFile( std::fstream &file ){
    
    if(!file.is_open())
    {
	   errorLog <<"loadModelFromFile(fstream &file) - The file is not open!" << std::endl;
	   return false;
    }
    
    std::string word;
    
    file >> word;
    if( word != "WeakClassifierType:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read WeakClassifierType header!" << std::endl;
	return false;
    }
    file >> word;
    
    if( word != weakClassifierType ){
        errorLog <<"loadModelFromFile(fstream &file) - The weakClassifierType:" << word << " does not match: " << weakClassifierType << std::endl;
	return false;
    }
    
    file >> word;
    if( word != "Trained:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read Trained header!" << std::endl;
	return false;
    }
    file >> trained;
    
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read NumInputDimensions header!" << std::endl;
	return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if( word != "DecisionFeatureIndex:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read DecisionFeatureIndex header!" << std::endl;
	return false;
    }
    file >> decisionFeatureIndex;
    
    file >> word;
    if( word != "Direction:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read Direction header!" << std::endl;
	return false;
    }
    file >> direction;
    
    file >> word;
    if( word != "NumRandomSplits:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read NumRandomSplits header!" << std::endl;
	return false;
    }
    file >> numRandomSplits;
    
    file >> word;
    if( word != "DecisionValue:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read DecisionValue header!" << std::endl;
	return false;
    }
    file >> decisionValue;
    
    //We don't need to close the file as the function that called this function should handle that
    return true;
}

void DecisionStump::print() const{
    std::cout << "Trained: " << trained;
    std::cout << "\tDecisionValue: " << decisionValue;
    std::cout << "\tDecisionFeatureIndex: " << decisionFeatureIndex;
    std::cout << "\tDirection: " << direction << std::endl;
}
    
UINT DecisionStump::getDecisionFeatureIndex() const{
    return decisionFeatureIndex;
}

UINT DecisionStump::getDirection() const{
    return direction;
}

UINT DecisionStump::getNumRandomSplits() const{
    return numRandomSplits;
}

Float DecisionStump::getDecisionValue() const{
    return decisionValue;
}

GRT_END_NAMESPACE


