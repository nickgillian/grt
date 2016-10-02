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
#include "RadialBasisFunction.h"

GRT_BEGIN_NAMESPACE
    
//Register the RadialBasisFunction module with the WeakClassifier base class
RegisterWeakClassifierModule< RadialBasisFunction > RadialBasisFunction::registerModule("RadialBasisFunction");
    
RadialBasisFunction::RadialBasisFunction(UINT numSteps,Float positiveClassificationThreshold,Float minAlphaSearchRange,Float maxAlphaSearchRange){
    this->numSteps = numSteps;
    this->positiveClassificationThreshold = positiveClassificationThreshold;
    this->minAlphaSearchRange = minAlphaSearchRange;
    this->maxAlphaSearchRange = maxAlphaSearchRange;
    trained = false;
    numInputDimensions = 0;
    alpha = 0;
    gamma = 0;
    weakClassifierType = "RadialBasisFunction";
    trainingLog.setKey("[DEBUG RadialBasisFunction]");
    warningLog.setKey("[WARNING RadialBasisFunction]");
    errorLog.setKey("[ERROR RadialBasisFunction]");
}
    
RadialBasisFunction::~RadialBasisFunction(){
    
}

RadialBasisFunction::RadialBasisFunction(const RadialBasisFunction &rhs){
    *this = rhs;
}

RadialBasisFunction& RadialBasisFunction::operator=(const RadialBasisFunction &rhs){
    if( this != &rhs ){
        this->numSteps = rhs.numSteps;
        this->alpha = rhs.alpha;
        this->gamma = rhs.gamma;
        this->positiveClassificationThreshold = rhs.positiveClassificationThreshold;
        this->minAlphaSearchRange = rhs.minAlphaSearchRange;
        this->maxAlphaSearchRange = rhs.maxAlphaSearchRange;
        this->rbfCentre = rhs.rbfCentre;
        this->copyBaseVariables( &rhs );
    }
    return *this;
}

bool RadialBasisFunction::deepCopyFrom(const WeakClassifier *weakClassifer){
    if( weakClassifer == NULL ) return false;
    
    if( this->getWeakClassifierType() == weakClassifer->getWeakClassifierType() ){
        //Call the = operator
        *this = *(RadialBasisFunction*)weakClassifer;
        return true;
    }
    return false;
}

bool RadialBasisFunction::train(ClassificationData &trainingData, VectorFloat &weights){
    
    trained = false;
    numInputDimensions = trainingData.getNumDimensions();
    rbfCentre.clear();
    
    //There should only be two classes in the dataset, the positive class (classLable==1) and the negative class (classLabel==2)
    if( trainingData.getNumClasses() != 2 ){
        errorLog << "train(ClassificationData &trainingData, VectorFloat &weights) - There should only be 2 classes in the training data, but there are : " << trainingData.getNumClasses() << std::endl;
        return false;
    }
    
    //There should be one weight for every training sample
    if( trainingData.getNumSamples() != weights.getSize() ){
        errorLog << "train(ClassificationData &trainingData, VectorFloat &weights) - There number of examples in the training data (" << trainingData.getNumSamples() << ") does not match the lenght of the weights vector (" << weights.size() << ")" << std::endl;
        return false;
    }

    //STEP 1: Estimate the centre of the RBF function as the weighted mean of the positive examples
    const UINT M = trainingData.getNumSamples();
    rbfCentre.resize(numInputDimensions,0);
    
    //Search for the sample(s) with the maximum weight(s)
    Float maxWeight = 0;
    Vector< UINT > bestWeights;
    for(UINT i=0; i<M; i++){
        if( trainingData[i].getClassLabel() == WEAK_CLASSIFIER_POSITIVE_CLASS_LABEL ){
            if( weights[i] > maxWeight ){
                maxWeight = weights[i];
                bestWeights.clear();
                bestWeights.push_back(i);
            }else if( weights[i] == maxWeight ){
                bestWeights.push_back( i );
            }
        }
    }
    
    //Estimate the centre of the RBF function as the weighted mean of the most important sample(s)
    const UINT N = (UINT)bestWeights.size();
    
    if( N == 0 ){
        errorLog << "train(ClassificationData &trainingData, VectorFloat &weights) - There are no positive class weigts!" << std::endl;
        return false;
    }
    
    for(UINT i=0; i<N; i++){
        for(UINT j=0; j<numInputDimensions; j++){
            rbfCentre[j] += trainingData[ bestWeights[i] ][j];
        }
    }
    
    //Normalize the RBF centre by the positiveWeightSum so we get the weighted mean 
    for(UINT j=0; j<numInputDimensions; j++){
        rbfCentre[j] /= Float(N);
    }
    
    //STEP 2: Estimate the best value for alpha
    Float step = (maxAlphaSearchRange-minAlphaSearchRange)/numSteps;
    Float bestAlpha = 0;
    Float minError = grt_numeric_limits< Float >::max();
    
    alpha = minAlphaSearchRange;
    while( alpha <= maxAlphaSearchRange ){
        
        //Update gamma (this is used in the rbf function)
        gamma = -1.0/(2.0*grt_sqr(alpha));
        
        //Compute the weighted error over all the training samples given the current alpha value
        Float error = 0;
        for(UINT i=0; i<M; i++){
            bool positiveSample = trainingData[ i ].getClassLabel() == WEAK_CLASSIFIER_POSITIVE_CLASS_LABEL;
            Float v = rbf(trainingData[ i ].getSample(),rbfCentre);
            
            if( (v >= positiveClassificationThreshold && !positiveSample) || (v<positiveClassificationThreshold && positiveSample) ){
                error += weights[i];
            }
        }
        
        //Check if the current error is the best so far
        if( error < minError ){
            minError = error;
            bestAlpha = alpha;
            
            //If the minimum error is zero then we can stop the search
            if( minError == 0 )
                break;
        }
        
        //Update alpha
        alpha += step;
    }
    
    alpha = bestAlpha;
    gamma = -1.0/(2.0*grt_sqr(alpha));
    trained = true;
    
    std::cout << "BestAlpha: " << bestAlpha << " Error: " << minError << std::endl;
    
    return true;
}

Float RadialBasisFunction::predict(const VectorFloat &x){
    if( rbf(x,rbfCentre) >= positiveClassificationThreshold ) return 1;
    return -1;
}
    
Float RadialBasisFunction::rbf(const VectorFloat &a,const VectorFloat &b){
    const UINT N = (UINT)a.size();
    //Compute the RBF distance, this uses the squared euclidean distance
    Float r = 0;
    for(UINT i=0; i<N; i++){
        r += SQR(a[i]-b[i]);
    }
    return exp( gamma * r );
}
    
bool RadialBasisFunction::saveModelToFile( std::fstream &file ) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << std::endl;
		return false;
	}
    
	//Write the WeakClassifierType data
    file << "WeakClassifierType: " << weakClassifierType << std::endl;
	file << "Trained: "<< trained << std::endl;
    file << "NumInputDimensions: " << numInputDimensions << std::endl;
    
    //Write the RadialBasisFunction data
    file << "NumSteps: " << numSteps << std::endl;
    file << "PositiveClassificationThreshold: " << positiveClassificationThreshold << std::endl;
    file << "Alpha: " << alpha << std::endl;
    file << "MinAlphaSearchRange: " << minAlphaSearchRange << std::endl;
    file << "MaxAlphaSearchRange: " << maxAlphaSearchRange << std::endl;
    file << "RBF: ";
    for(UINT i=0; i<numInputDimensions; i++){
        if( trained ){
            file << rbfCentre[i] << "\t";
        }else file << 0 << "\t";
    }
    file << std::endl;
    
    //We don't need to close the file as the function that called this function should handle that
    return true;
}

bool RadialBasisFunction::loadModelFromFile( std::fstream &file ){
    
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
    if( word != "NumSteps:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read NumSteps header!" << std::endl;
		return false;
    }
    file >> numSteps;
    
    file >> word;
    if( word != "PositiveClassificationThreshold:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read PositiveClassificationThreshold header!" << std::endl;
		return false;
    }
    file >> positiveClassificationThreshold;
    
    file >> word;
    if( word != "Alpha:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read Alpha header!" << std::endl;
		return false;
    }
    file >> alpha;
    
    file >> word;
    if( word != "MinAlphaSearchRange:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read MinAlphaSearchRange header!" << std::endl;
		return false;
    }
    file >> minAlphaSearchRange;
    
    file >> word;
    if( word != "MaxAlphaSearchRange:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read MaxAlphaSearchRange header!" << std::endl;
		return false;
    }
    file >> maxAlphaSearchRange;
    
    file >> word;
    if( word != "RBF:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read RBF header!" << std::endl;
		return false;
    }
    rbfCentre.resize(numInputDimensions);
    
    for(UINT i=0; i<numInputDimensions; i++){
        file >> rbfCentre[i];
    }
    
    //Compute gamma using alpha
    gamma = -1.0/(2.0*SQR(alpha));
   
    //We don't need to close the file as the function that called this function should handle that
    return true;
}

void RadialBasisFunction::print() const{
}
    
VectorFloat RadialBasisFunction::getRBFCentre() const{
    return rbfCentre;
}
    
UINT RadialBasisFunction::getNumSteps() const{
    return numSteps;
}

Float RadialBasisFunction::getPositiveClassificationThreshold() const{
    return positiveClassificationThreshold;
}

Float RadialBasisFunction::getAlpha() const{
    return alpha;
}

Float RadialBasisFunction::getMinAlphaSearchRange() const{
    return minAlphaSearchRange;
}

Float RadialBasisFunction::getMaxAlphaSearchRange() const{
    return maxAlphaSearchRange;
}

GRT_END_NAMESPACE

