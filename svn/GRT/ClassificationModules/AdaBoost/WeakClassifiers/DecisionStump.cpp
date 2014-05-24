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

#include "DecisionStump.h"

namespace GRT{
    
//Register the DecisionStump module with the WeakClassifier base class
RegisterWeakClassifierModule< DecisionStump > DecisionStump::registerModule("DecisionStump");
    
DecisionStump::DecisionStump(UINT numSteps){
    this->numSteps = numSteps;
    trained = false;
    numInputDimensions = 0;
    decisionFeatureIndex = 0;
    decisionValue = 0;
    direction = 0;
    weakClassifierType = "DecisionStump";
    trainingLog.setProceedingText("[DEBUG DecisionStump]");
    warningLog.setProceedingText("[WARNING DecisionStump]");
    errorLog.setProceedingText("[ERROR DecisionStump]");
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
        this->numSteps = rhs.numSteps;
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

bool DecisionStump::train(ClassificationData &trainingData, VectorDouble &weights){
    
    trained = false;
    numInputDimensions = trainingData.getNumDimensions();
    
    //There should only be two classes in the dataset, the positive class (classLable==1) and the negative class (classLabel==2)
    if( trainingData.getNumClasses() != 2 ){
        errorLog << "train(ClassificationData &trainingData, VectorDouble &weights) - There should only be 2 classes in the training data, but there are : " << trainingData.getNumClasses() << endl;
        return false;
    }
    
    //There should be one weight for every training sample
    if( trainingData.getNumSamples() != weights.size() ){
        errorLog << "train(ClassificationData &trainingData, VectorDouble &weights) - There number of examples in the training data (" << trainingData.getNumSamples() << ") does not match the lenght of the weights vector (" << weights.size() << ")" << endl;
        return false;
    }
    
    //Pick the training sample to use as the stump feature
    const UINT M = trainingData.getNumSamples();
    UINT bestFeatureIndex = 0;
    vector< MinMax > ranges = trainingData.getRanges();
    double minError = numeric_limits<double>::max();
    double minRange = 0;
    double maxRange = 0;
    double step = 0;
    double threshold = 0;
    double bestThreshold = 0;
    
    for(UINT n=0; n<numInputDimensions; n++){
        minRange = ranges[n].minValue;
        maxRange = ranges[n].maxValue;
        step = (maxRange-minRange)/double(numSteps);
        threshold = minRange;
        while( threshold <= maxRange ){
            
            //Compute the error using the current threshold on the current input dimension
            //We need to check both sides of the threshold
            double rhsError = 0;
            double lhsError = 0;
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
            
            //Update the threshold
            threshold += step;
        }
    }
    
    decisionFeatureIndex = bestFeatureIndex;
    decisionValue = bestThreshold;
    trained = true;
    
    cout << "Best Feature Index: " << decisionFeatureIndex << " Value: " << decisionValue << " Direction: " << direction << " Error: " << minError << endl;
    return true;
}

double DecisionStump::predict(const VectorDouble &x){
    if( direction == 1){
        if( x[ decisionFeatureIndex ] >= decisionValue ) return 1;
    }else if( x[ decisionFeatureIndex ] <= decisionValue ) return 1;
    return -1;
}
    
bool DecisionStump::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the WeakClassifierType data
    file << "WeakClassifierType: " << weakClassifierType << endl;
	file << "Trained: "<< trained << endl;
    file << "NumInputDimensions: " << numInputDimensions << endl;
    
    //Write the DecisionStump data
    file << "DecisionFeatureIndex: " << decisionFeatureIndex << endl;
	file << "Direction: "<< direction << endl;
    file << "NumSteps: " << numSteps << endl;
    file << "DecisionValue: " << decisionValue << endl;
    
    //We don't need to close the file as the function that called this function should handle that
    return true;
}

bool DecisionStump::loadModelFromFile(fstream &file){
    
    if(!file.is_open())
	{
		errorLog <<"loadModelFromFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
    string word;
    
    file >> word;
    if( word != "WeakClassifierType:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read WeakClassifierType header!" << endl;
		return false;
    }
    file >> word;
    
    if( word != weakClassifierType ){
        errorLog <<"loadModelFromFile(fstream &file) - The weakClassifierType:" << word << " does not match: " << weakClassifierType << endl;
		return false;
    }
    
    file >> word;
    if( word != "Trained:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read Trained header!" << endl;
		return false;
    }
    file >> trained;
    
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read NumInputDimensions header!" << endl;
		return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if( word != "DecisionFeatureIndex:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read DecisionFeatureIndex header!" << endl;
		return false;
    }
    file >> decisionFeatureIndex;
    
    file >> word;
    if( word != "Direction:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read Direction header!" << endl;
		return false;
    }
    file >> direction;
    
    file >> word;
    if( word != "NumSteps:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read NumSteps header!" << endl;
		return false;
    }
    file >> numSteps;
    
    file >> word;
    if( word != "DecisionValue:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read DecisionValue header!" << endl;
		return false;
    }
    file >> decisionValue;
    
    //We don't need to close the file as the function that called this function should handle that
    return true;
}

void DecisionStump::print() const{
    cout << "Trained: " << trained;
    cout << "\tDecisionValue: " << decisionValue;
    cout << "\tDecisionFeatureIndex: " << decisionFeatureIndex;
    cout << "\tDirection: " << direction << endl;
}
    
UINT DecisionStump::getDecisionFeatureIndex() const{
    return decisionFeatureIndex;
}

UINT DecisionStump::getDirection() const{
    return direction;
}

UINT DecisionStump::getNumSteps() const{
    return numSteps;
}

double DecisionStump::getDecisionValue() const{
    return decisionValue;
}

} //End of namespace GRT

