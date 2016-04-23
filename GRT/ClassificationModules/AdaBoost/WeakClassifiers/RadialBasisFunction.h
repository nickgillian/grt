/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a Radial Basis Function Weak Classifier. The Radial Basis Function (RBF) class
 fits an RBF to the weighted training data so as to maximize the number of positive training samples that are
 inside a specific region of the RBF (this region is set by the GRT::RadialBasisFunction::positiveClassificationThreshold
 parameter).  After the RBF has been trained, it will output 1 if the input data is inside the RBF positive classification
 region, otherwise it will output 0.
 */

/**
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

#ifndef GRT_RADIAL_BASIC_FUNCTION_HEADER
#define GRT_RADIAL_BASIC_FUNCTION_HEADER

#include "WeakClassifier.h"

GRT_BEGIN_NAMESPACE
    
class GRT_API RadialBasisFunction : public WeakClassifier{
public:
    /**
     Default Constructor.
     
     Sets the number of steps that will be used to search for the best alpha value during the training phase, in addition to other parameters used
     to control the RBF learning algorithm.
     
     @param numSteps: sets the number of steps that will be used to search for the best alpha value during the training phase. Default value = 100
     @param positiveClassificationThreshold: sets the positive classification threshold, this parameter is the threshold that defines if a value 
     is classified as a positive sample or a negative sample. Default value = 0.9
     @param minAlphaSearchRange: the minimum value used to search for the best alpha value. Default value = 0.001
     @param maxAlphaSearchRange: the maximum value used to search for the best alpha value. Default value = 1.0
     */
    RadialBasisFunction(UINT numSteps=100,Float positiveClassificationThreshold=0.9,Float minAlphaSearchRange=0.001,Float maxAlphaSearchRange=1.0);
    
    /**
     Default Destructor.
     */
    virtual ~RadialBasisFunction();
    
    /**
     Default Copy Constructor. Defines how the data from the rhs GRT::RadialBasisFunction instance is copied to this GRT::RadialBasisFunction instance.
     */
    RadialBasisFunction(const RadialBasisFunction &rhs);
    
    /**
     Defines how the data from the rhs GRT::RadialBasisFunction instance is copied to this GRT::RadialBasisFunction instance.
     */
    RadialBasisFunction& operator=(const RadialBasisFunction &rhs);
    
    /**
     This function enables the data from one GRT::RadialBasisFunction instance to be copied into this GRT::RadialBasisFunction instance.
     
     @param weakClassifer: a pointer to the Classifier Base Class, this should be pointing to another GRT::RadialBasisFunction instance
     @return returns true if the clone was successfull, false otherwise
     */
    virtual bool deepCopyFrom(const WeakClassifier *weakClassifer);
    
    /**
     This function trains the RBF, using the weighted labelled training data.
     
     @param trainingData: the labelled training data
     @param weights: the corresponding weights for each sample in the labelled training data
     @return returns true if the RBF was trained successfull, false otherwise
     */
    virtual bool train(ClassificationData &trainingData, VectorFloat &weights);
    
    /**
     This function predicts the class label of the input vector, given the current RBF model. The class label returned will
     either be positive (WEAK_CLASSIFIER_POSITIVE_CLASS_LABEL) or negative (WEAK_CLASSIFIER_NEGATIVE_CLASS_LABEL).
     
     @param x: the vector used for prediction
     @return returns the predicted class label, which will be either positive or negative
     */
    virtual Float predict(const VectorFloat &x);
    
    /**
     This function saves the current RBF model to a file.
     
     fstream file: a reference to the file you want to save the RBF model to
     @return returns true if the RBF was saved successfull, false otherwise
     */
    virtual bool saveModelToFile( std::fstream &file ) const;
    
    /**
     This function loads an RBF model from a file.
     
     fstream file: a reference to the file you want to load the RBF model from
     @return returns true if the RBF was loaded successfull, false otherwise
     */
    virtual bool loadModelFromFile( std::fstream &file );
    
    /**
     This function prints out some basic info about the RBF to std::cout.
     */
    virtual void print() const;
    
    /**
     This function gets the number of steps parameter which sets how many steps are used to search for the best RBF alpha values.
     
     @return returns an UINT representing the numSteps parameter
     */
    UINT getNumSteps() const;
    
    /**
     This function gets the positiveClassificationThreshold, if the output of the RBF function is greater than or equal to the
     positiveClassificationThreshold then the input sample will be classified as a positive sample, otherwise it will be classified
     as a negative sample.
     
     @return returns the positiveClassificationThreshold
     */
    Float getPositiveClassificationThreshold() const;
    
    /**
     Gets the current alpha value, this is used in the RBF. You can compute the RBF gamma parameter by: -1.0/(2.0*SQR(alpha)).
     
     @return returns the alpha value
     */
    Float getAlpha() const;
    
    /**
     Gets the minAlphaSearchRange value, this is the minimum value used to search for the best alpha value.
     
     @return returns the minAlphaSearchRange value
     */
    Float getMinAlphaSearchRange() const;
    
    /**
     Gets the maxAlphaSearchRange value, this is the maximum value used to search for the best alpha value.
     
     @return returns the maxAlphaSearchRange value
     */
    Float getMaxAlphaSearchRange() const;
    
    /**
     Gets the RBF center.
     
     @return returns the RBF centre.
     */
    VectorFloat getRBFCentre() const;
    
protected:
    Float rbf(const VectorFloat &a,const VectorFloat &b);
  
    UINT numSteps;
    Float positiveClassificationThreshold;
    Float alpha;
    Float gamma;
    Float minAlphaSearchRange;
    Float maxAlphaSearchRange;
    VectorFloat rbfCentre;
    
    static RegisterWeakClassifierModule< RadialBasisFunction > registerModule; ///< This is used to register the DecisionStump with the WeakClassifier base class
};

GRT_END_NAMESPACE

#endif // GRT_DECISION_STUMP_HEADER
