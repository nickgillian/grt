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

#ifndef GRT_ADABOOST_HEADER
#define GRT_ADABOOST_HEADER

#include "../../CoreModules/Classifier.h"
#include "AdaBoostClassModel.h"
#include "WeakClassifiers/DecisionStump.h"
#include "WeakClassifiers/RadialBasisFunction.h"

GRT_BEGIN_NAMESPACE

/**
@brief This class contains the AdaBoost classifier. AdaBoost (Adaptive Boosting) is a powerful classifier that works well on both basic and more complex
recognition problems.

AdaBoost works by creating a highly accurate classifier by combining many relatively weak and inaccurate
classifiers. AdaBoost therefore acts as a meta algorithm, which allows you to use it as a wrapper for other classifiers.
In the GRT, these classifiers are called Weak Classifiers such as a GRT::DecisionStump (which is just one node of a DecisionTree).
AdaBoost is adaptive in the sense that subsequent classifiers added at each round of boosting are tweaked in favor of those
instances misclassified by previous classifiers. The default number of boosting rounds for AdaBoost is 20, however
this can easily be set using the GRT::AdaBoost::setNumBoostingIterations(UINT numBoostingIterations) function or
via the AdaBoost constructor.

@remark This implementation is based on Schapire, Robert E., and Yoav Freund. Boosting: Foundations and Algorithms. MIT Press, 2012.

@note  There are two classification modes, GRT::AdaBoost::PredictionMethods (MAX_POSITIVE_VALE and MAX_VALUE), which can give
different classification results so you should experiment with each mode to achieve the best results for your classification task.

@example ClassificationModulesExamples/AdaBoostExample/AdaBoostExample.cpp
*/
class GRT_API AdaBoost : public Classifier
{
public:
    enum PredictionMethods{MAX_POSITIVE_VALUE=0,MAX_VALUE}; ///<These are the two prediction methods that the GRT::AdaBoost classifier can use.

    /**
    Default Constructor
    
    @param weakClassifier: sets the initial weak classifier that is added to the vector of weak classifiers used to train the AdaBoost model
    @param useScaling: sets if the training and prediction data should be scaled to a specific range.  Default value is useScaling = false
    @param useNullRejection: sets if null rejection will be used for the realtime prediction.  If useNullRejection is set to true then the predictedClassLabel will be set to 0 (which is the default null label) if the distance between the inputVector and the top K datum is greater than the null rejection threshold for the top predicted class.  The null rejection threshold is computed for each class during the training phase. Default value is useNullRejection = false
    @param nullRejectionCoeff: sets the null rejection coefficient, this is a multipler controlling the null rejection threshold for each class.  This will only be used if the useNullRejection parameter is set to true.  Default value is nullRejectionCoeff = 10.0
    @param numBoostingIterations: sets the number of boosting iterations to use during training. Default value = 20
    @param predictionMethod: sets the prediction method for AdaBoost, this should be one of the PredictionMethods. Default value = MAX_VALUE
    */
    AdaBoost(const WeakClassifier &weakClassifier = DecisionStump(),bool useScaling=false,bool useNullRejection=false,Float nullRejectionCoeff=10.0,UINT numBoostingIterations=20,UINT predictionMethod=MAX_VALUE);
    
    /**
    Defines the copy constructor.
    
    @param rhs: the instance from which all the data will be copied into this instance
    */
    AdaBoost(const AdaBoost &rhs);
    
    /**
    Default Destructor
    */
    virtual ~AdaBoost();
    
    /**
    Defines how the data from the rhs AdaBoost should be copied to this AdaBoost
    
    @param rhs: another instance of a AdaBoost
    @return returns a reference to this instance of the AdaBoost
    */
    AdaBoost &operator=(const AdaBoost &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an AdaBoost instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another AdaBoost instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the AdaBoost model, using the labelled classification data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the AdaBoost model was trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
    This predicts the class of the inputVector.
    This overrides the predict function in the Classifier base class.
    
    @param inputVector: the input vector to classify
    @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
    This overrides the clear function in the Classifier base class.
    It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the module was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    This saves the trained AdaBoost model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the AdaBoost model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained AdaBoost model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the AdaBoost model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This recomputes the null rejection thresholds for each of the classes in the AdaBoost model.
    This will be called automatically if the setGamma(Float gamma) function is called.
    The AdaBoost model needs to be trained first before this function can be called.
    
    @return returns true if the null rejection thresholds were updated successfully, false otherwise
    */
    virtual bool recomputeNullRejectionThresholds();
    
    /**
    Sets the nullRejectionCoeff parameter.
    The nullRejectionCoeff parameter is a multipler controlling the null rejection threshold for each class.
    This function will also recompute the null rejection thresholds.
    
    @return returns true if the gamma parameter was updated successfully, false otherwise
    */
    bool setNullRejectionCoeff(Float nullRejectionCoeff);
    
    /**
    Sets the WeakClassifier to use for boosting.
    
    If this function is called, any previously set WeakClassifiers will be removed.
    
    @return returns true if the WeakClassifier was added successfully, false otherwise
    */
    bool setWeakClassifier(const WeakClassifier &weakClassifer);
    
    /**
    Adds a WeakClassifier to the list of WeakClassifiers to use for boosting.
    
    If this function is called, the new WeakClassifier will be added to the list of WeakClassifiers.
    
    @return returns true if the WeakClassifier was added successfully, false otherwise
    */
    bool addWeakClassifier(const WeakClassifier &weakClassifer);
    
    /**
    Clears all the current WeakClassifiers.
    
    @return returns true if the WeakClassifiers was cleared successfully, false otherwise
    */
    bool clearWeakClassifiers();
    
    /**
    Sets the number of boosting iterations that should be used when training the AdaBoost model.
    The numBoostingIterations parameter must be greater than zero.
    
    @return returns true if the numBoostingIterations was set successfully, false otherwise
    */
    bool setNumBoostingIterations(UINT numBoostingIterations);
    
    /**
    Sets the prediction method for AdaBoost, this should be one of the PredictionMethods enumerations.
    
    @param predictionMethod: the predictionMethod that should be used by AdaBoost, this should be one of the PredictionMethods enumerations
    @return returns true if the predictionMethod was set successfully, false otherwise
    */
    bool setPredictionMethod(UINT predictionMethod);
    
    /**
    Prints the current model to the std::out.
    
    @return void.
    */
    void printModel();
    
    /**
    Returns the current vector of AdaBoostClassModel models.
    
    @return a vector containing the current AdaBoostClassModel models.
    */
    Vector< AdaBoostClassModel > getModels() const { return models; }

    /**
    Gets a string that represents the AdaBoost class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;
    
protected:
    bool loadLegacyModelFromFile( std::fstream &file );
    
    UINT numBoostingIterations;
    UINT predictionMethod;
    Vector< WeakClassifier* > weakClassifiers;
    Vector< AdaBoostClassModel > models;
    
private:
    static RegisterClassifierModule< AdaBoost > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_ADABOOST_HEADER
