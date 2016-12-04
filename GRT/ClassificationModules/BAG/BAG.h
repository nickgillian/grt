/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
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

#ifndef GRT_BAG_HEADER
#define GRT_BAG_HEADER

#include "../../CoreModules/Classifier.h"

GRT_BEGIN_NAMESPACE

/**
@brief This class implements the bootstrap aggregator classifier.  Bootstrap aggregating (bagging) is a machine
learning ensemble meta-algorithm designed to improve the stability and accuracy of other machine learning
algorithms.  Bagging also reduces variance and helps to avoid overfitting. Although it is usually applied
to decision tree methods, the BAG class can be used with any type of GRT classifier. Bagging is a special case of
the model averaging.

@remark This implementation is based on Breiman, Leo. "Bagging predictors." Machine learning 24, no. 2 (1996): 123-140.

@example ClassificationModulesExamples/BAGExample/BAGExample.cpp
*/
class GRT_API BAG : public Classifier
{
public:
    /**
    Default Constructor
    
    @param useScaling: sets if the training and real-time data should be scaled between [0 1]. Default value = false
    */
    BAG(bool useScaling=false);
    
    /**
    Defines the copy constructor.
    
    @param rhs: the instance from which all the data will be copied into this instance
    */
    BAG(const BAG &rhs);
    
    /**
    Default Destructor
    */
    virtual ~BAG(void);
    
    /**
    Defines how the data from the rhs BAG should be copied to this BAG
    
    @param rhs: another instance of a BAG
    @return returns a pointer to this instance of the BAG
    */
    BAG &operator=(const BAG &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an BAG instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another BAG instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the BAG model, using the labelled classification data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the BAG model was trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
    This predicts the class of the inputVector.
    This overrides the predict function in the Classifier base class.
    
    @param inputVector: the input Vector to classify
    @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
    This resets the BAG classifier.
    
    @return returns true if the BAG model was successfully reset, false otherwise.
    */
    virtual bool reset();
    
    /**
    This overrides the clear function in the Classifier base class.
    It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the module was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    This saves the trained BAG model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the BAG model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained BAG model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the BAG model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    Gets the number of classifiers in the ensemble.
    
    @return returns the size of the ensemble
    */
    UINT getEnsembleSize() const;
    
    /**
    Gets the weights for each classifier in the ensemble.
    
    @return returns a Vector of weights.
    */
    VectorFloat getEnsembleWeights() const;
    
    /**
    Gets the ensemble.
    
    @return returns a Vector of Classifier pointers.
    */
    const Vector< Classifier* > getEnsemble() const;
    
    /**
    This functions adds a copy of the input classifier to the ensemble. This classifier will then
    be trained (in addition to the other classifiers in the ensemble) when you call the BAG train
    function.
    
    @param classifier: a reference to the classifier you want to add to the ensemble
    @return returns true if a copy of the classifier was successfully added to the ensemble, false otherwise
    */
    bool addClassifierToEnsemble(const Classifier &classifier,Float weight=1);
    
    /**
    This functions clears the current ensemble, removing all classifiers and weights.
    
    @return returns true if the ensemble was successfully cleared, false otherwise
    */
    bool clearEnsemble();
    
    /**
    This functions lets you dynamically update the weights for each ensemble. This can be useful if you
    have some exterior knowledge that could be used to weight the vote for each classifier.  The weights
    should all be positive, however they do not need to sum to one.
    
    You should can call this function both before training the ensemble, and in real-time to update the weights for
    each prediction.
    
    @return returns true if the ensemble weights were successfully updated, false otherwise
    */
    bool setWeights(const VectorFloat &weights);
    
    /**
    Gets a string that represents the BAG class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();

    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_;
    using MLBase::predict_;
    
protected:
    bool loadLegacyModelFromFile( std::fstream &file );
    
    VectorFloat weights;
    Vector< Classifier* > ensemble;
    
private:
    static RegisterClassifierModule< BAG > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_BAG_HEADER
    