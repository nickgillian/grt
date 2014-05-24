/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This is the main base class that all GRT machine learning algorithms should inherit from.
 
 A large number of the functions in this class are virtual and simply return false as these functions must be overwridden by the inheriting class.
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

#ifndef GRT_MLBASE_HEADER
#define GRT_MLBASE_HEADER

#include "GRTBase.h"
#include "../DataStructures/UnlabelledData.h"
#include "../DataStructures/ClassificationData.h"
#include "../DataStructures/RegressionData.h"
#include "../DataStructures/TimeSeriesClassificationData.h"

namespace GRT{

#define DEFAULT_NULL_LIKELIHOOD_VALUE 0
#define DEFAULT_NULL_DISTANCE_VALUE 0
    
/**
 Define the class for handling TrainingResult callbacks
 */
class TrainingResultsObserverManager : public ObserverManager< TrainingResult >
{
public:
    TrainingResultsObserverManager(){
    }
    virtual ~TrainingResultsObserverManager(){}
    
};
    
/**
 Define the class for handling TestInstanceResult callbacks
 */
class TestResultsObserverManager : public ObserverManager< TestInstanceResult >
{
public:
    TestResultsObserverManager(){
    }
    virtual ~TestResultsObserverManager(){}
    
};

class MLBase : public GRTBase, public Observer< TrainingResult >, public Observer< TestInstanceResult >
{
public:
    /**
     Default MLBase Constructor
     */
	MLBase(void);

    /**
     Default MLBase Destructor
     */
	virtual ~MLBase(void);

    /**
     This copies all the MLBase variables from the instance mlBaseA to the instance mlBaseA.
     
     @param const MLBase *mlBase: a pointer to a MLBase class from which the values will be copied to the instance that calls the function
     @return returns true if the copy was successfull, false otherwise
     */
    bool copyMLBaseVariables(const MLBase *mlBase);
    
    /**
     This is the main training interface for ClassificationData.
     By default it will call the train_ function, unless it is overwritten by the derived class.
     
     @param ClassificationData trainingData: the training data that will be used to train the ML model
     @return returns true if the classifier was successfully trained, false otherwise
     */
    virtual bool train(ClassificationData trainingData);
    
    /**
     This is the main training interface for referenced ClassificationData. This should be overwritten by the derived class.
     
     @param ClassificationData &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the classifier was successfully trained, false otherwise
     */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This is the main training interface for regression data.
     By default it will call the train_ function, unless it is overwritten by the derived class.
     
     @param RegressionData trainingData: the training data that will be used to train a new regression model
     @return returns true if a new regression model was trained, false otherwise
     */
    virtual bool train(RegressionData trainingData);
    
    /**
     This is the main training interface for all the regression algorithms. This should be overwritten by the derived class.
     
     @param RegressionData trainingData: the training data that will be used to train a new regression model
     @return returns true if a new regression model was trained, false otherwise
     */
    virtual bool train_(RegressionData &trainingData);
    
    /**
     This is the main training interface for TimeSeriesClassificationData.
     By default it will call the train_ function, unless it is overwritten by the derived class.
     
     @param TimeSeriesClassificationData trainingData: the training data that will be used to train the ML model
     @return returns true if the classifier was successfully trained, false otherwise
     */
    virtual bool train(TimeSeriesClassificationData trainingData);
    
    /**
     This is the main training interface for referenced TimeSeriesClassificationData. This should be overwritten by the derived class.
     
     @param TimeSeriesClassificationData &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the classifier was successfully trained, false otherwise
     */
    virtual bool train_(TimeSeriesClassificationData &trainingData);
    
    /**
     This is the main training interface for UnlabelledData.
     By default it will call the train_ function, unless it is overwritten by the derived class.
     
     @param UnlabelledData trainingData: the training data that will be used to train the ML model
     @return returns true if the classifier was successfully trained, false otherwise
     */
    virtual bool train(UnlabelledData trainingData);
    
    /**
     This is the main training interface for referenced UnlabelledData. This should be overwritten by the derived class.
     
     @param UnlabelledData &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the classifier was successfully trained, false otherwise
     */
    virtual bool train_(UnlabelledData &trainingData);
    
    /**
     This is the main training interface for MatrixDouble data. 
     By default it will call the train_ function, unless it is overwritten by the derived class.
     
     @param MatrixDouble trainingData: the training data that will be used to train the ML model
     @return returns true if the classifier was successfully trained, false otherwise
     */
    virtual bool train(MatrixDouble data);
    
    /**
     This is the main training interface for referenced MatrixDouble data. This should be overwritten by the derived class.
     
     @param MatrixDouble &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the classifier was successfully trained, false otherwise
     */
    virtual bool train_(MatrixDouble &data);

    /**
     This is the main prediction interface for all the GRT machine learning algorithms.
     By defaut it will call the predict_ function, unless it is overwritten by the derived class.
     
     @param VectorDouble inputVector: the new input vector for prediction
     @return returns true if the prediction was completed succesfully, false otherwise (the base class always returns false)
     */
    virtual bool predict(VectorDouble inputVector);
    
    /**
     This is the main prediction interface for all the GRT machine learning algorithms. This should be overwritten by the derived class.
     
     @param VectorDouble &inputVector: a reference to the input vector for prediction
     @return returns true if the prediction was completed succesfully, false otherwise (the base class always returns false)
     */
    virtual bool predict_(VectorDouble &inputVector);
    
    /**
     This is the prediction interface for time series data.
     By defaut it will call the predict_ function, unless it is overwritten by the derived class.
     
     @param MatrixDouble inputMatrix: the new input matrix for prediction
     @return returns true if the prediction was completed succesfully, false otherwise (the base class always returns false)
     */
    virtual bool predict(MatrixDouble inputMatrix);
    
    /**
     This is the prediction interface for time series data. This should be overwritten by the derived class.
     
     @param MatrixDouble inputMatrix: a reference to the new input matrix for prediction
     @return returns true if the prediction was completed succesfully, false otherwise (the base class always returns false)
     */
    virtual bool predict_(MatrixDouble &inputMatrix);
    
    /**
     This is the main mapping interface for all the GRT machine learning algorithms.
     By defaut it will call the map_ function, unless it is overwritten by the derived class.
     
     @param VectorDouble inputVector: the input vector for mapping/regression
     @return returns true if the mapping was completed succesfully, false otherwise (the base class always returns false)
     */
    virtual bool map(VectorDouble inputVector);
    
    /**
     This is the main mapping interface by reference for all the GRT machine learning algorithms. This should be overwritten by the derived class.
     
     @param VectorDouble &inputVector: a reference to the input vector for mapping/regression
     @return returns true if the mapping was completed succesfully, false otherwise (the base class always returns false)
     */
    virtual bool map_(VectorDouble &inputVector);
    
    /**
     This is the main reset interface for all the GRT machine learning algorithms.
     It should be used to reset the model (i.e. set all values back to default settings). If you want to completely clear the model
     (i.e. clear any learned weights or values) then you should use the clear function.

     @return returns true if the derived class was reset succesfully, false otherwise (the base class always returns true)
     */
    virtual bool reset();
    
    /**
     This is the main clear interface for all the GRT machine learning algorithms.
     It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
     
     @return returns true if the derived class was cleared succesfully, false otherwise
     */
    virtual bool clear();
    
    /**
     This is the main print interface for all the GRT machine learning algorithms. This should be overwritten by the derived class.
     It will print the model and settings to the display log.
     
     @return returns true if the model was printed succesfully, false otherwise (the base class always returns true)
     */
    virtual bool print() const;
    
    /**
     This saves the trained model to a file, it calls the saveModelToFile(fstream &file) function unless it is overwritten by the derived class.
     
     @param const string filename: the name of the file to save the model to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(const string filename) const;
    
    /**
     This saves the trained model to a file.
     This function should be overwritten by the derived class.
     
     @param fstream &file: a reference to the file the model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained model from a file, it calls the loadModelFromFile(fstream &file) function unless it is overwritten by the derived class.
     
     @param const string filename: the name of the file to load the model from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(const string filename);
    
    /**
     This loads a trained model from a file.
     This function should be overwritten by the derived class.
     
     @param fstream &file: a reference to the file the model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    /**
     Scales the input value x (which should be in the range [minSource maxSource]) to a value in the new target range of [minTarget maxTarget].
     
     @param const double &x: the value that should be scaled
     @param const double &minSource: the minimum range that x originates from
     @param const double &maxSource: the maximum range that x originates from
     @param const double &minTarget: the minimum range that x should be scaled to
     @param const double &maxTarget: the maximum range that x should be scaled to
     @param const bool constrain: sets if the scaled value should be constrained to the target range
     @return returns a new value that has been scaled based on the input parameters
     */
    double inline scale(const double &x,const double &minSource,const double &maxSource,const double &minTarget,const double &maxTarget,const bool constrain=false){
        if( constrain ){
            if( x <= minSource ) return minTarget;
            if( x >= maxSource ) return maxTarget;
        }
        if( minSource == maxSource ) return minTarget;
        return (((x-minSource)*(maxTarget-minTarget))/(maxSource-minSource))+minTarget;
    }

    /**
     Gets the current ML base type.
     
     @return returns an UINT representing the current ML base type, this will be one of the BaseTypes enumerations
     */
    UINT getBaseType() const;

    /**
     Gets the number of input dimensions in trained model.
     This function is now depriciated and will be removed in the future, you should use getNumInputDimensions instead.

     @return returns the number of input dimensions in the trained model, a value of 0 will be returned if the model has not been trained
     */
    UINT getNumInputFeatures() const;
    
    /**
     Gets the number of input dimensions in trained model.
     
     @return returns the number of input dimensions
     */
    UINT getNumInputDimensions() const;
    
    /**
     Gets the number of output dimensions in trained model.
     
     @return returns the number of output dimensions
     */
    UINT getNumOutputDimensions() const;
    
    /**
     Gets the minimum number of epochs. This is the minimum number of epochs that can elapse with no change between two training epochs.
     An epoch is a complete iteration of all training samples.
     
     @return returns the minimum number of epochs
     */
	UINT getMinNumEpochs() const;
    
    /**
     Gets the maximum number of epochs. This value controls the maximum number of epochs that can be used by the training algorithm.
     An epoch is a complete iteration of all training samples.
     
     @return returns the maximum number of epochs
     */
	UINT getMaxNumEpochs() const;
    
    /**
     Gets the size (as a percentage) of the validation set (if one should be used). If this value returned 20 this would mean that
     20% of the training data would be set aside to create a validation set and the other 80% would be used to actually train the regression model.
     This will only happen if the useValidationSet parameter is set to true, otherwise 100% of the training data will be used to train the regression model.
     
     @return returns the size of the validation set
     */
	UINT getValidationSetSize() const;
    
    /**
     Gets the number of training iterations that were required for the algorithm to converge.
     
     @return returns the number of training iterations required for the training algorithm to converge, a value of 0 will be returned if the model has not been trained
     */
    UINT getNumTrainingIterationsToConverge() const;
    
    /**
     Gets the minimum change value that controls when the training algorithm should stop.
     
     @return returns the minimum change value
     */
	double getMinChange() const;
    
    /**
     Gets the current learningRate value, this is value used to update the weights at each step of a learning algorithm such as stochastic gradient descent.
     
     @return returns the current learningRate value
     */
    double getLearningRate() const;
    
    /**
     Gets the root mean squared error on the training data during the training phase.
     
     @return returns the RMS error (on the training data during the training phase)
     */
    double getRootMeanSquaredTrainingError() const;
    
    /**
     Gets the total squared error on the training data during the training phase.
     
     @return returns the total squared error (on the training data during the training phase)
     */
    double getTotalSquaredTrainingError() const;
    
    /**
     Returns true if a validation set should be used for training. If true, then the training dataset will be partitioned into a smaller training dataset
     and a validation set.
     
     The size of the partition is controlled by the validationSetSize parameter, for example, if the validationSetSize parameter is 20 then 20% of the
     training data will be used for a validation set leaving 80% of the original data to train the model.
     
     @return returns true if a validation set should be used for training, false otherwise
     */
	bool getUseValidationSet() const;
    
    /**
     Returns true if the order of the training dataset should be randomized at each epoch of training. 
     Randomizing the order of the training dataset stops a learning algorithm from focusing too much on the first few examples in the dataset.
     
     @return returns true if the order of the training dataset should be randomized, false otherwise
     */
	bool getRandomiseTrainingOrder() const;
    
	/**
     Gets if the model for the derived class has been succesfully trained.
     
     @return returns true if the model for the derived class has been succesfully trained, false otherwise
     */
    bool getTrained() const;

    /**
     This function is now depreciated. You should use the getTrained() function instead.
     
     @return returns true if the model for the derived class has been succesfully trained, false otherwise
     */
    bool getModelTrained() const;
    
    /**
     Gets if the scaling has been enabled.
     
     @return returns true if scaling is enabled, false otherwise
     */
    bool getScalingEnabled() const;
    
    /**
     Gets if the derived class type is CLASSIFIER.
     
     @return returns true if the derived class type is CLASSIFIER, false otherwise
     */
    bool getIsBaseTypeClassifier() const;
    
    /**
     Gets if the derived class type is REGRESSIFIER.
     
     @return returns true if the derived class type is REGRESSIFIER, false otherwise
     */
    bool getIsBaseTypeRegressifier() const;
    
    /**
     Gets if the derived class type is CLUSTERER.
     
     @return returns true if the derived class type is CLUSTERER, false otherwise
     */
    bool getIsBaseTypeClusterer() const;
    
    /**
     Sets if scaling should be used during the training and prediction phases.
     
     @return returns true the scaling parameter was updated, false otherwise
     */
    bool enableScaling(bool useScaling);
    
    /**
     Sets the maximum number of epochs (a complete iteration of all training samples) that can be run during the training phase.
     The maxNumIterations value must be greater than zero.
     
     @param const UINT maxNumIterations: the maximum number of iterations value, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setMaxNumEpochs(const UINT maxNumEpochs);
    
    /**
     Sets the minimum number of epochs (a complete iteration of all training samples) that can elapse with no change between two training epochs.
     
     @param const UINT minNumEpochs: the minimum number of epochs that can elapse with no change between two training epochs
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setMinNumEpochs(const UINT minNumEpochs);
    
    /**
     Sets the minimum change that must be achieved between two training epochs for the training to continue.
     The minChange value must be greater than zero.
     
     @param const double minChange: the minimum change value, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setMinChange(const double minChange);
    
    /**
     Sets the learningRate. This is used to update the weights at each step of learning algorithms such as stochastic gradient descent.
     The learningRate value must be greater than zero.
     
     @param double learningRate: the learningRate value used during the training phase, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setLearningRate(double learningRate);
    
    /**
     Sets the size of the validation set used by some learning algorithms for training. This value represents the percentage of the main
     dataset that will be used for training.  For example, if the validationSetSize parameter is 20 then 20% of the training data will be
     used for a validation set leaving 80% of the original data to train the model.
     
     @param const UINT validationSetSize: the new validation set size (as a percentage)
     @return returns true if the validationSetSize parameter was updated, false otherwise
     */
    bool setUseValidationSet(const bool useValidationSet);
    
    /**
     Sets the size of the validation set used by some learning algorithms for training. This value represents the percentage of the main 
     dataset that will be used for training.  For example, if the validationSetSize parameter is 20 then 20% of the training data will be 
     used for a validation set leaving 80% of the original data to train the model.
     
     @param const UINT validationSetSize: the new validation set size (as a percentage)
     @return returns true if the validationSetSize parameter was updated, false otherwise
     */
    bool setValidationSetSize(const UINT validationSetSize);
    
    /**
     Sets if the order of the training dataset should be randomized at each epoch of training.
     Randomizing the order of the training dataset stops a learning algorithm from focusing too much on the first few examples in the dataset.
     
     @return returns true if the parameter was updated, false otherwise
     */
    bool setRandomiseTrainingOrder(const bool randomiseTrainingOrder);
    
    /**
     Registers the observer with the training result observer manager. The observer will then be notified when any new training result is computed.
     
     @return returns true the observer was added, false otherwise
     */
    bool registerTrainingResultsObserver( Observer< TrainingResult > &observer );
    
    /**
     Registers the observer with the test result observer manager. The observer will then be notified when any new test result is computed.
     
     @return returns true the observer was added, false otherwise
     */
    bool registerTestResultsObserver( Observer< TestInstanceResult > &observer );
    
    /**
     Removes the observer from the training result observer manager.
     
     @return returns true if the observer was removed, false otherwise
     */
    bool removeTrainingResultsObserver( const Observer< TrainingResult > &observer );
    
    /**
     Removes the observer from the test result observer manager.
     
     @return returns true if the observer was removed, false otherwise
     */
    bool removeTestResultsObserver( const Observer< TestInstanceResult > &observer );
    
    /**
     Removes all observers from the training result observer manager.
     
     @return returns true if all the observers were removed, false otherwise
     */
    bool removeAllTrainingObservers();
    
    /**
     Removes all observers from the training result observer manager.
     
     @return returns true if all the observers were removed, false otherwise
     */
    bool removeAllTestObservers();
    
    /**
     Notifies all observers that have subscribed to the training results observer manager.
     
     @return returns true if all the observers were notified, false otherwise
     */
    bool notifyTrainingResultsObservers( const TrainingResult &data );
    
    /**
     Notifies all observers that have subscribed to the test results observer manager.
     
     @return returns true if all the observers were notified, false otherwise
     */
    bool notifyTestResultsObservers( const TestInstanceResult &data );
    
    /**
	 This functions returns a pointer to the current instance.
     
     @return returns a MLBase pointer to the current instance.
     */
    MLBase* getMLBasePointer();
    
    /**
	 This functions returns a const pointer to the current instance.
     
     @return returns a const MLBase pointer to the current instance.
     */
    const MLBase* getMLBasePointer() const;
    
    /**
     Gets the training results from the last training phase. Each element in the vector represents the training results from 1 training iteration.
     
     @return returns a vector of TrainingResult instances containing the training results from the most recent training phase
     */
    vector< TrainingResult > getTrainingResults() const;

protected:
    
    /**
     Saves the core base settings to a file.
     
     @return returns true if the base settings were saved, false otherwise
     */
    bool saveBaseSettingsToFile(fstream &file) const;
    
    /**
     Loads the core base settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadBaseSettingsFromFile(fstream &file);
    
    bool trained;
    bool useScaling;
    UINT baseType;
    UINT numInputDimensions;
    UINT numOutputDimensions;
    UINT numTrainingIterationsToConverge;
    UINT minNumEpochs;
    UINT maxNumEpochs;
    UINT validationSetSize;
    double learningRate;
    double minChange;
    double rootMeanSquaredTrainingError;
    double totalSquaredTrainingError;
    bool useValidationSet;
    bool randomiseTrainingOrder;
    Random random;
    vector< TrainingResult > trainingResults;
    TrainingResultsObserverManager trainingResultsObserverManager;
    TestResultsObserverManager testResultsObserverManager;
    
public:

    enum BaseTypes{BASE_TYPE_NOT_SET=0,CLASSIFIER,REGRESSIFIER,CLUSTERER};
};

} //End of namespace GRT

#endif //GRT_MLBASE_HEADER
