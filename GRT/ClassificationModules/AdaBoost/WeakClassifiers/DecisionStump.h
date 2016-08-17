/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a DecisionStump, which is a single node of a DecisionTree.
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

#ifndef GRT_DECISION_STUMP_HEADER
#define GRT_DECISION_STUMP_HEADER

#include "WeakClassifier.h"

GRT_BEGIN_NAMESPACE
    
class GRT_API DecisionStump : public WeakClassifier{
public:
    /**
     Default Constructor.
     
     Sets the number of random splits that will be used to search for the best split value.
     
     @param numRandomSplits: sets the number of random splits that will be used to search for the best split value. Default value = 100
     */
    DecisionStump(const UINT numRandomSplits = 100);
    
    /**
     Default Destructor.
     */
    virtual ~DecisionStump();
    
    /**
     Default Copy Constructor. Defines how the data from the rhs GRT::DecisionStump instance is copied to this GRT::DecisionStump instance.
     */
    DecisionStump(const DecisionStump &rhs);
    
    /**
     Defines how the data from the rhs GRT::DecisionStump instance is copied to this GRT::DecisionStump instance.
     */
    DecisionStump& operator=(const DecisionStump &rhs);
    
    /**
     This function enables the data from one GRT::DecisionStump instance to be copied into this GRT::DecisionStump instance.
     
     @param weakClassifer: a pointer to the Classifier Base Class, this should be pointing to another GRT::DecisionStump instance
     @return returns true if the clone was successfull, false otherwise
     */
    virtual bool deepCopyFrom(const WeakClassifier *weakClassifer);
    
    /**
     This function trains the DecisionStump model, using the weighted labelled training data.
     
     @param trainingData: the labelled training data
     @param weights: the corresponding weights for each sample in the labelled training data
     @return returns true if the model was trained successfull, false otherwise
     */
    virtual bool train(ClassificationData &trainingData, VectorFloat &weights);
    
    /**
     This function predicts the class label of the input vector, given the current model. The class label returned will
     either be positive (WEAK_CLASSIFIER_POSITIVE_CLASS_LABEL) or negative (WEAK_CLASSIFIER_NEGATIVE_CLASS_LABEL).
     
     @param weights: a reference to the vector used for prediction
     @return returns the estimated class label, which will be positive or negative
     */
    virtual Float predict(const VectorFloat &x);
    
    /**
     This function saves the current model to a file.
     
     fstream file: a reference to the file you want to save the RBF model to
     @return returns true if the model was saved successfull, false otherwise
     */
    virtual bool saveModelToFile( std::fstream &file ) const;
    
    /**
     This function loads an model model from a file.
     
     fstream file: a reference to the file you want to load the RBF model from
     @return returns true if the model was loaded successfull, false otherwise
     */
    virtual bool loadModelFromFile( std::fstream &file );
    
    /**
     This function prints out some basic info about the model to std::cout.
     */
    virtual void print() const;
    
    /**
     @return returns the index of the feature that is used to split the data into a positive or negative class
     */
    UINT getDecisionFeatureIndex() const;
    
    /**
     @return returns if the decision spilt threshold is greater than (1), or less than (0)
     */
    UINT getDirection() const;
    
    /**
    @return returns the number of random splits that will be used to search for the best decision spilt
    */
    UINT getNumRandomSplits() const;
    
    /**
     @return returns the decision spilt threshold
     */
    Float getDecisionValue() const;

protected:
    UINT decisionFeatureIndex;  ///< The dimension that the data will be spilt on
    UINT direction;             ///< Indicates if the decision spilt threshold is greater than (1), or less than (0)
    UINT numRandomSplits;       ///< The number of random splits used to search for the best decision spilt
    Float decisionValue;       ///< The decision spilt threshold
    
    static RegisterWeakClassifierModule< DecisionStump > registerModule; ///< This is used to register the DecisionStump with the WeakClassifier base class
};

GRT_END_NAMESPACE

#endif //GRT_DECISION_STUMP_HEADER
