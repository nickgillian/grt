/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief The TestInstanceResult class provides a data structure for storing the results of a classification or regression test instance.
 */

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

#ifndef GRT_TEST_INSTANCE_RESULT_HEADER
#define GRT_TEST_INSTANCE_RESULT_HEADER

#include "GRTTypedefs.h"

GRT_BEGIN_NAMESPACE

class TestInstanceResult{
public:
    /**
     Default Constructor.
     
     Initializes the TrainingResult instance.
     */
    TestInstanceResult(){
		testMode = CLASSIFICATION_MODE;
		testIteration = 0;
		classLabel = 0;
		predictedClassLabel = 0;
		unProcessedPredictedClassLabel = 0;
    }
    
    /**
     Copy Constructor.
     
     Initializes this instance by copying the data from the rhs instance
     
     @param const TestInstanceResult &rhs: another instance of the TestInstanceResult class
     */
    TestInstanceResult(const TestInstanceResult &rhs){
        *this = rhs;
    }
    
    /**
     Default Destructor.
     */
    ~TestInstanceResult(){
        
    }

    /**
     Defines the Equals Operator.

     This copies the data from the rhs instance to this instance, returning a reference to the current instance.

     @param const TestInstanceResult &rhs: another instance of the TestInstanceResult class
     */
    TestInstanceResult& operator=(const TestInstanceResult &rhs){
        if( this != &rhs ){
			this->testMode = rhs.testMode;
			this->testIteration = rhs.testIteration;
			this->classLabel = rhs.classLabel;
			this->predictedClassLabel = rhs.predictedClassLabel;
			this->unProcessedPredictedClassLabel = rhs.unProcessedPredictedClassLabel;
			this->classLikelihoods = rhs.classLikelihoods;
			this->classDistances = rhs.classDistances;
			this->regressionData = rhs.regressionData;
			this->targetData = rhs.targetData;
        }
        return *this;
    }
    
    /**
     Sets the training result for classification data. This will place the training mode into CLASSIFICATION_MODE.

     @param trainingIteration: the current training iteration (or epoch)
     @param accuracy: the accuracy for the current training iteration
     @return returns true if the training result was set successfully
     */
    bool setClassificationResult(const unsigned int testIteration,const unsigned int classLabel,const unsigned int predictedClassLabel,const unsigned int unProcessedPredictedClassLabel,const VectorFloat &classLikelihoods,const VectorFloat &classDistances){
		this->testMode = CLASSIFICATION_MODE;
		this->testIteration = testIteration;
		this->classLabel = classLabel;
		this->predictedClassLabel = predictedClassLabel;
		this->unProcessedPredictedClassLabel = unProcessedPredictedClassLabel;
		this->classLikelihoods = classLikelihoods;
		this->classDistances = classDistances;
        return true;
    }
    
    /**
     Sets the training result for regression data. This will place the training mode into REGRESSION_MODE.

     @param trainingIteration: the current training iteration (or epoch)
     @param totalSquaredTrainingError: the total squared training error for the current iteration
     @param rootMeanSquaredTrainingError: the root mean squared training error for the current iteration
     @return returns true if the training result was set successfully
     */
    bool setRegressionResult(const unsigned int testIteration,const VectorFloat &regressionData,const VectorFloat &targetData){
        this->testMode = REGRESSION_MODE;
        this->testIteration = testIteration;
        this->regressionData = regressionData;
        this->targetData = targetData;
        return true;
    }

    /**
     Gets the current test mode, this will be one of the TestMode enums.

     @return returns the current test mode, this will be one of the TestMode enums
     */
    unsigned int getTestMode() const{
        return testMode;
    }

    /**
     Gets the test iteration, this represents which test example the test results correspond to.

     @return returns the test iteration
     */
    unsigned int getTestIteration() const{
        return testIteration;
    }

    /**
     Gets the class label. This is only useful in CLASSIFICATION_MODE.

     @return returns the class label
     */
    unsigned int getClassLabel() const{
        return classLabel;
    }

    /**
     Gets the predicted class label. This is only useful in CLASSIFICATION_MODE.

     @return returns the predicted class label
     */
    unsigned int getPredictedClassLabel() const{
        return predictedClassLabel;
    }

    /**
     Gets the maximum likelihood. This is only useful in CLASSIFICATION_MODE.

     @return returns the maximum likelihood
     */
    Float getMaximumLikelihood() const{
        Float maxLikelihood = 0;
        for(size_t i=0; i<classLikelihoods.size(); i++){
            if( classLikelihoods[i] > maxLikelihood ){
                maxLikelihood = classLikelihoods[i];
            }
        }
        return maxLikelihood;
    }

    /**
     Gets the squared error between the regressionData and the target data. This is only useful in REGRESSION_MODE.
     
     @return returns the squared error between the regression estimate and the target data
     */
    Float getSquaredError() const{
        Float sum = 0;
        if( regressionData.size() != targetData.size() ) return 0;
        for(size_t i=0; i<regressionData.size(); i++){
            sum += (regressionData[i]-targetData[i])*(regressionData[i]-targetData[i]);
        }
        return sum;
    }

    /**
     Gets the class likelihoods vector. This is only useful in CLASSIFICATION_MODE.
     
     @return returns the class likelihoods vector
     */
    VectorFloat getClassLikelihoods() const {
        return classLikelihoods;
    }

    /**
     Gets the class distances vector. This is only useful in CLASSIFICATION_MODE.
     
     @return returns the class distances vector
     */
    VectorFloat getDistances() const {
        return classDistances;
    }

protected:
	unsigned int testMode;
	unsigned int testIteration;
	unsigned int classLabel;
	unsigned int predictedClassLabel;
	unsigned int unProcessedPredictedClassLabel;
	VectorFloat classLikelihoods;
	VectorFloat classDistances;
	VectorFloat regressionData;
	VectorFloat targetData;
    
public:
    
    enum TestMode{CLASSIFICATION_MODE=0,REGRESSION_MODE};

};

GRT_END_NAMESPACE

#endif //GRT_TEST_INSTANCE_RESULT_HEADER
