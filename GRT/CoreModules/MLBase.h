/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0
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
#include "../Util/Metrics.h"
#include "../DataStructures/UnlabelledData.h"
#include "../DataStructures/ClassificationData.h"
#include "../DataStructures/ClassificationDataStream.h"
#include "../DataStructures/RegressionData.h"
#include "../DataStructures/TimeSeriesClassificationData.h"

GRT_BEGIN_NAMESPACE

#define DEFAULT_NULL_LIKELIHOOD_VALUE 0
#define DEFAULT_NULL_DISTANCE_VALUE 0

/**
Define the class for handling TrainingResult callbacks
*/
class GRT_API TrainingResultsObserverManager : public ObserverManager< TrainingResult >
{
    public:
    TrainingResultsObserverManager(){
    }
    virtual ~TrainingResultsObserverManager(){}
    
};

/**
Define the class for handling TestInstanceResult callbacks
*/
class GRT_API TestResultsObserverManager : public ObserverManager< TestInstanceResult >
{
    public:
    TestResultsObserverManager(){
    }
    virtual ~TestResultsObserverManager(){}
    
};

/**
@brief This is the main base class that all GRT machine learning algorithms should inherit from.

A large number of the functions in this class are virtual and simply return false as these functions must be overwridden by the inheriting class.
*/
class GRT_API MLBase : public GRTBase, public Observer< TrainingResult >, public Observer< TestInstanceResult >
{
public:
    enum BaseType{BASE_TYPE_NOT_SET=0,CLASSIFIER,REGRESSIFIER,CLUSTERER,PRE_PROCSSING,POST_PROCESSING,FEATURE_EXTRACTION,CONTEXT}; ///<Enum that defines the type of inherited class

    /**
    Default MLBase Constructor
    @param id: the id of the inheriting class
    @param type: the type of the inheriting class (e.g., classifier, regressifier, etc.)
    */
    MLBase( const std::string &id = "", const BaseType type = BASE_TYPE_NOT_SET );
    
    /**
    Default MLBase Destructor
    */
    virtual ~MLBase(void);
    
    /**
    This copies all the MLBase variables from the instance mlBaseA to the instance mlBaseA.
    
    @param mlBase: a pointer to a MLBase class from which the values will be copied to the instance that calls the function
    @return returns true if the copy was successfull, false otherwise
    */
    bool copyMLBaseVariables(const MLBase *mlBase);
    
    /**
    This is the main training interface for ClassificationData.
    By default it will call the train_ function, unless it is overwritten by the derived class.
    
    @param trainingData: the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train(ClassificationData trainingData);
    
    /**
    This is the main training interface for referenced ClassificationData. This should be overwritten by the derived class.
    
    @param trainingData: a reference to the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
    This is the main training interface for regression data.
    By default it will call the train_ function, unless it is overwritten by the derived class.
    
    @param trainingData: the training data that will be used to train a new regression model
    @return returns true if a new regression model was trained, false otherwise
    */
    virtual bool train(RegressionData trainingData);
    
    /**
    This is the main training interface for all the regression algorithms. This should be overwritten by the derived class.
    
    @param trainingData: the training data that will be used to train a new regression model
    @return returns true if a new regression model was trained, false otherwise
    */
    virtual bool train_(RegressionData &trainingData);
     
    /**
    This is the main training interface for training a regression model using a training and validation dataset. This should be overwritten by the derived class.
    By default it will call the train_ function, unless it is overwritten by the derived class.
    
    @param trainingData: the training data that will be used to train a new regression model
    @param validationData: the validation data that will be used to validate the regression model
    @return returns true if a new regression model was trained, false otherwise
    */
    virtual bool train(RegressionData trainingData,RegressionData validationData);
    
    /**
    This is the main training interface for training a regression model using a training and validation dataset. This should be overwritten by the derived class.
    
    @param trainingData: the training data that will be used to train a new regression model
    @param validationData: the validation data that will be used to validate the regression model
    @return returns true if a new regression model was trained, false otherwise
    */
    virtual bool train_(RegressionData &trainingData,RegressionData &validationData);
    
    /**
    This is the main training interface for TimeSeriesClassificationData.
    By default it will call the train_ function, unless it is overwritten by the derived class.
    
    @param trainingData: the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train(TimeSeriesClassificationData trainingData);
    
    /**
    This is the main training interface for referenced TimeSeriesClassificationData. This should be overwritten by the derived class.
    
    @param trainingData: a reference to the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train_(TimeSeriesClassificationData &trainingData);
    
    /**
    This is the main training interface for ClassificationDataStream.
    By default it will call the train_ function, unless it is overwritten by the derived class.
    
    @param trainingData: the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train(ClassificationDataStream trainingData);
    
    /**
    This is the main training interface for referenced ClassificationDataStream. This should be overwritten by the derived class.
    
    @param trainingData: a reference to the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train_(ClassificationDataStream &trainingData);
    
    /**
    This is the main training interface for UnlabelledData.
    By default it will call the train_ function, unless it is overwritten by the derived class.
    
    @param trainingData: the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train(UnlabelledData trainingData);
    
    /**
    This is the main training interface for referenced UnlabelledData. This should be overwritten by the derived class.
    
    @param trainingData: a reference to the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train_(UnlabelledData &trainingData);
    
    /**
    This is the main training interface for MatrixFloat data.
    By default it will call the train_ function, unless it is overwritten by the derived class.
    
    @param trainingData: the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train(MatrixFloat data);
    
    /**
    This is the main training interface for referenced MatrixFloat data. This should be overwritten by the derived class.
    
    @param trainingData: a reference to the training data that will be used to train the ML model
    @return returns true if the classifier was successfully trained, false otherwise
    */
    virtual bool train_(MatrixFloat &data);
    
    /**
    This is the main prediction interface for all the GRT machine learning algorithms.
    By defaut it will call the predict_ function, unless it is overwritten by the derived class.
    
    @param inputVector: the new input vector for prediction
    @return returns true if the prediction was completed succesfully, false otherwise (the base class always returns false)
    */
    virtual bool predict(VectorFloat inputVector);
    
    /**
    This is the main prediction interface for all the GRT machine learning algorithms. This should be overwritten by the derived class.
    
    @param inputVector: a reference to the input vector for prediction
    @return returns true if the prediction was completed succesfully, false otherwise (the base class always returns false)
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
    This is the prediction interface for time series data.
    By defaut it will call the predict_ function, unless it is overwritten by the derived class.
    
    @param inputMatrix: the new input matrix for prediction
    @return returns true if the prediction was completed succesfully, false otherwise (the base class always returns false)
    */
    virtual bool predict(MatrixFloat inputMatrix);
    
    /**
    This is the prediction interface for time series data. This should be overwritten by the derived class.
    
    @param inputMatrix: a reference to the new input matrix for prediction
    @return returns true if the prediction was completed succesfully, false otherwise (the base class always returns false)
    */
    virtual bool predict_(MatrixFloat &inputMatrix);
    
    /**
    This is the main mapping interface for all the GRT machine learning algorithms.
    By defaut it will call the map_ function, unless it is overwritten by the derived class.
    
    @param inputVector: the input vector for mapping/regression
    @return returns true if the mapping was completed succesfully, false otherwise (the base class always returns false)
    */
    virtual bool map(VectorFloat inputVector);
    
    /**
    This is the main mapping interface by reference for all the GRT machine learning algorithms. This should be overwritten by the derived class.
    
    @param inputVector: a reference to the input vector for mapping/regression
    @return returns true if the mapping was completed succesfully, false otherwise (the base class always returns false)
    */
    virtual bool map_(VectorFloat &inputVector);
    
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
    This saves the model to a file.
    
    @param filename: the name of the file to save the model to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save(const std::string &filename) const;
    
    /**
    This saves the model to a file.
    
    @param filename: the name of the file to save the model to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool load(const std::string &filename);
    
    /**
    This saves the trained model to a file.
    This function should be overwritten by the derived class.
    
    @param file: a reference to the file the model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save(std::fstream &file) const;
    
    /**
    This loads a trained model from a file.
    This function should be overwritten by the derived class.
    
    @param file: a reference to the file the model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load(std::fstream &file);
    
    /**
    @deprecated use save(std::string filename) instead
    @param the name of the file to save the model to
    @return returns true if the model was saved successfully, false otherwise
    */
    GRT_DEPRECATED_MSG( "saveModelToFile(std::string filename) is deprecated, use save(const std::string &filename) instead", virtual bool saveModelToFile(const std::string &filename) const );
    
    /**
    @deprecated use save(std::fstream &file) instead
    @param file: a reference to the file the model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    GRT_DEPRECATED_MSG( "saveModelToFile(std::fstream &file) is deprecated, use save(std::fstream &file) instead", virtual bool saveModelToFile(std::fstream &file) const );
    
    /**
    @deprecated use load(std::string filename) instead
    @param filename: the name of the file to load the model from
    @return returns true if the model was loaded successfully, false otherwise
    */
    GRT_DEPRECATED_MSG( "loadModelFromFile(std::string filename) is deprecated, use load(const std::string &filename) instead",virtual bool loadModelFromFile(const std::string &filename) );
    
    /**
    @deprecated use load(std::fstream &file) instead
    @param file: a reference to the file the model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    GRT_DEPRECATED_MSG( "loadModelFromFile(std::fstream &file) is deprecated, use load(std::fstream &file) instead",virtual bool loadModelFromFile(std::fstream &file) );

    /**
    This function adds the current model to the formatted stream.
    This function should be overwritten by the derived class.
    
    @param file: a reference to the stream the model will be added to
    @return returns true if the model was added successfully, false otherwise
    */
    virtual bool getModel(std::ostream &stream) const;
    
    /**
    Gets the current model and settings as a std::string.
    
    @return returns a std::string containing the model
    */
    virtual std::string getModelAsString() const;
    
    /**
    Gets the expected input data type for the module
    
    @return returns the expected input data type
    */
    DataType getInputType() const;
    
    /**
    Gets the expected output data type for the module
    
    @return returns the expected output data type
    */
    DataType getOutputType() const;
    
    /**
    Gets the current ML base type.
    
    @return returns an enum representing the current ML base type, this will be one of the BaseType enumerations
    */
    BaseType getType() const;
    
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
    Float getMinChange() const;
    
    /**
    Gets the current learningRate value, this is value used to update the weights at each step of a learning algorithm such as stochastic gradient descent.
    
    @return returns the current learningRate value
    */
    Float getLearningRate() const;

    /**
    Gets the root mean squared error on the training data during the training phase.
    
    @return returns the RMS error (on the training data during the training phase)
    */
    Float getRMSTrainingError() const;
    
    /**
    @deprecated use getRMSTrainingError() instead
    Gets the root mean squared error on the training data during the training phase.
    
    @return returns the RMS error (on the training data during the training phase)
    */
    GRT_DEPRECATED_MSG( "getRootMeanSquaredTrainingError() is deprecated, use getRMSTrainingError() instead", Float getRootMeanSquaredTrainingError() const );
    
    /**
    Gets the total squared error on the training data during the training phase.
    
    @return returns the total squared error (on the training data during the training phase)
    */
    Float getTotalSquaredTrainingError() const;

    /**
    Gets the root mean squared error on the validation data during the training phase, this will be zero if no validation set was used.
    
    @return returns the RMS error (on the validation data during the training phase)
    */
    Float getRMSValidationError() const;
    
    /**
    Gets the accuracy of the validation set on the trained model, only valid if the model was trained with useValidationSet=true.
    
    @return returns the accuracy of validation set on the trained model
    */
    Float getValidationSetAccuracy() const;
    
    /**
    Gets the precision of the validation set on the trained model, only valid if the model was trained with useValidationSet=true.
    
    @return returns the precision of the validation set on the trained model
    */
    VectorFloat getValidationSetPrecision() const;
    
    /**
    Gets the recall of the validation set on the trained model, only valid if the model was trained with useValidationSet=true.
    
    @return returns the recall of the validation set on the trained model
    */
    VectorFloat getValidationSetRecall() const;
    
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
    Gets the logging state for the training log, note this returns the logging state for this specific instance of the training log, not the global TrainingLog state.
    
    @return returns true if logging is enabled, false otherwise
    */
    bool getTrainingLoggingEnabled() const;

    /**
    Gets the logging state for the testing log, note this returns the logging state for this specific instance of the training log, not the global TestingLog state.
    
    @return returns true if logging is enabled, false otherwise
    */
    bool getTestingLoggingEnabled() const;
    
    /**
    Sets if scaling should be used during the training and prediction phases.
    
    @return returns true the scaling parameter was updated, false otherwise
    */
    bool enableScaling(const bool useScaling);
    
    /**
    Sets the maximum number of epochs (a complete iteration of all training samples) that can be run during the training phase.
    The maxNumIterations value must be greater than zero.
    
    @param maxNumIterations: the maximum number of iterations value, must be greater than zero
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setMaxNumEpochs(const UINT maxNumEpochs);
    
    /**
    Sets the minimum number of epochs (a complete iteration of all training samples) that can elapse with no change between two training epochs.
    
    @param minNumEpochs: the minimum number of epochs that can elapse with no change between two training epochs
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setMinNumEpochs(const UINT minNumEpochs);
    
    /**
    Sets the minimum change that must be achieved between two training epochs for the training to continue.
    The minChange value must be greater than zero.
    
    @param minChange: the minimum change value, must be greater than zero
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setMinChange(const Float minChange);
    
    /**
    Sets the learningRate. This is used to update the weights at each step of learning algorithms such as stochastic gradient descent.
    The learningRate value must be greater than zero.
    
    @param learningRate: the learningRate value used during the training phase, must be greater than zero
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setLearningRate(const Float learningRate);
    
    /**
    Sets the size of the validation set used by some learning algorithms for training. This value represents the percentage of the main
    dataset that will be used for training.  For example, if the validationSetSize parameter is 20 then 20% of the training data will be
    used for a validation set leaving 80% of the original data to train the model.
    
    @param validationSetSize: the new validation set size (as a percentage)
    @return returns true if the validationSetSize parameter was updated, false otherwise
    */
    bool setUseValidationSet(const bool useValidationSet);
    
    /**
    Sets the size of the validation set used by some learning algorithms for training. This value represents the percentage of the main
    dataset that will be used for training.  For example, if the validationSetSize parameter is 20 then 20% of the training data will be
    used for a validation set leaving 80% of the original data to train the model.
    
    @param validationSetSize: the new validation set size (as a percentage)
    @return returns true if the validationSetSize parameter was updated, false otherwise
    */
    bool setValidationSetSize(const UINT validationSetSize);
    
    /**
    Sets if the order of the training dataset should be randomized at each epoch of training.
    Randomizing the order of the training dataset stops a learning algorithm from focusing too much on the first few examples in the dataset.
    
    @param randomiseTrainingOrder: if true then the order in which training samples are supplied to a learning algorithm will be randomised
    @return returns true if the parameter was updated, false otherwise
    */
    bool setRandomiseTrainingOrder(const bool randomiseTrainingOrder);
    
    /**
    Sets if training logging is enabled/disabled for this specific ML instance.
    If you want to enable/disable training logging globally, then you should use the TrainingLog::enableLogging( bool ) function.
    
    @param loggingEnabled: if true then training logging will be enabled, if false then training logging will be disabled
    @return returns true if the parameter was updated, false otherwise
    */
    bool setTrainingLoggingEnabled(const bool loggingEnabled);

    /**
    Sets if testing logging is enabled/disabled for this specific ML instance.
    If you want to enable/disable testing logging globally, then you should use the TestingLog::enableLogging( bool ) function.
    
    @param loggingEnabled: if true then training logging will be enabled, if false then training logging will be disabled
    @return returns true if the parameter was updated, false otherwise
    */
    bool setTestingLoggingEnabled(const bool loggingEnabled);
    
    /**
    Registers the observer with the training result observer manager. The observer will then be notified when any new training result is computed.
    
    @param observer: the observer you want to register with the learning algorithm
    @return returns true the observer was added, false otherwise
    */
    bool registerTrainingResultsObserver( Observer< TrainingResult > &observer );
    
    /**
    Registers the observer with the test result observer manager. The observer will then be notified when any new test result is computed.
    
    @param observer: the observer you want to register with the learning algorithm
    @return returns true the observer was added, false otherwise
    */
    bool registerTestResultsObserver( Observer< TestInstanceResult > &observer );
    
    /**
    Removes the observer from the training result observer manager.
    
    @param observer: the observer you want to remove from the learning algorithm
    @return returns true if the observer was removed, false otherwise
    */
    bool removeTrainingResultsObserver( const Observer< TrainingResult > &observer );
    
    /**
    Removes the observer from the test result observer manager.
    
    @param observer: the observer you want to remove from the learning algorithm
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
    
    @param data: stores the training results data for the current update
    @return returns true if all the observers were notified, false otherwise
    */
    bool notifyTrainingResultsObservers( const TrainingResult &data );
    
    /**
    Notifies all observers that have subscribed to the test results observer manager.
    
    @param data: stores the test results data for the current update
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
    Vector< TrainingResult > getTrainingResults() const;
    
protected:
    
    /**
    Saves the core base settings to a file.
    
    @return returns true if the base settings were saved, false otherwise
    */
    bool saveBaseSettingsToFile( std::fstream &file ) const;
    
    /**
    Loads the core base settings from a file.
    
    @return returns true if the base settings were loaded, false otherwise
    */
    bool loadBaseSettingsFromFile( std::fstream &file );
    
    bool trained;
    bool useScaling;
    DataType inputType;
    DataType outputType;
    BaseType baseType;
    UINT numInputDimensions;
    UINT numOutputDimensions;
    UINT numTrainingIterationsToConverge;
    UINT minNumEpochs;
    UINT maxNumEpochs;
    UINT validationSetSize;
    Float learningRate;
    Float minChange;
    Float rmsTrainingError;
    Float rmsValidationError;
    Float totalSquaredTrainingError;
    Float validationSetAccuracy;
    bool useValidationSet;
    bool randomiseTrainingOrder;
    VectorFloat validationSetPrecision;
    VectorFloat validationSetRecall;
    Random random;
    Vector< TrainingResult > trainingResults;
    TrainingResultsObserverManager trainingResultsObserverManager;
    TestResultsObserverManager testResultsObserverManager;
    TrainingLog trainingLog;
    TestingLog testingLog;
    
};

GRT_END_NAMESPACE

#endif //GRT_MLBASE_HEADER
