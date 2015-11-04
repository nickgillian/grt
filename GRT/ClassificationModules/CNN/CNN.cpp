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

#include "CNN.h"

namespace GRT{

//Register the CNN module with the Classifier base class
RegisterClassifierModule< CNN >  CNN::registerModule("CNN");

CNN::CNN()
{
    this->useScaling = useScaling;
    useNullRejection = false;
    classType = "CNN";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG CNN]");
    errorLog.setProceedingText("[ERROR CNN]");
    trainingLog.setProceedingText("[TRAINING CNN]");
    warningLog.setProceedingText("[WARNING CNN]");
}
    
CNN::CNN(const CNN &rhs){
    classType = "CNN";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG CNN]");
    errorLog.setProceedingText("[ERROR CNN]");
    trainingLog.setProceedingText("[TRAINING CNN]");
    warningLog.setProceedingText("[WARNING CNN]");
    *this = rhs;
}

CNN::~CNN(void)
{
}
    
CNN& CNN::operator=(const CNN &rhs){
	if( this != &rhs ){
       
        //Copy the base classifier variables
        copyBaseVariables( (Classifier*)&rhs );
	}
	return *this;
}

bool CNN::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        CNN *ptr = (CNN*)classifier;
        
        
        //Copy the base classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool CNN::train_(ClassificationData &trainingData){
    
    //Clear any previous models
    clear();
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int K = trainingData.getNumClasses();
    
    if( M == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << endl;
        return false;
    }
    
    numInputDimensions = N;
    numClasses = K;
    classLabels.resize(K);
    ranges = trainingData.getRanges();
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }
    
   
    
    //Set the class labels
    classLabels = trainingData.getClassLabels();
    
    //Flag that the model has been trained
    trained = true;
    
    return trained;
}

bool CNN::predict_(VectorDouble &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - Model Not Trained!" << endl;
        return false;
    }
    
    predictedClassLabel = 0;
	maxLikelihood = -10000;
    
    if( !trained ) return false;
    
	if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses);
    
    //Reset the likelihoods and distances
    for(UINT k=0; k<numClasses; k++){
        classLikelihoods[k] = 0;
        classDistances[k] = 0;
    }
    
    
    
    return true;
}
    
bool CNN::reset(){
    
    return true;
}
    
bool CNN::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    return true;
}
    
bool CNN::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
    //NOTE: We do not need to close the file
    
    return true;
}
    
bool CNN::loadModelFromFile(fstream &file){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << endl;
        return false;
    }
    
    return true;
}
    

} //End of namespace GRT

