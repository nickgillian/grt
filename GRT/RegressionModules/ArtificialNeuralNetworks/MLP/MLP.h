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

#ifndef GRT_MLP_HEADER
#define GRT_MLP_HEADER

#include "Neuron.h"
#include "../../../CoreModules/Regressifier.h"

GRT_BEGIN_NAMESPACE

/**
 @brief This class implements a Multilayer Perceptron Artificial Neural Network.

 @example RegressionModulesExamples/MLPRegressionExample/MLPRegressionExample.cpp

 @remark This implementation is based on Bishop, Christopher M. Pattern recognition and machine learning. Vol. 1. New York: springer, 2006.
*/
class GRT_API MLP : public Regressifier{
public:
    enum TrainingAlgorithm{ONLINE_GRADIENT_DESCENT=0}; ///<The various training algorithms supported by the MLP algorithm

    /**
    Default Constructor
    */
    MLP();
    
    /**
    Copy Constructor
    
    @param rhs: another instance of a MLP that will be copied to this instance
    */
    MLP(const MLP &rhs);
    
    /**
    Default Destructor
    */
    virtual ~MLP();
    
    /**
    Defines how the data from the rhs MLP should be copied to this MLP
    
    @param rhs: another instance of a MLP
    @return returns a reference to this instance of the MLP
    */
    MLP &operator=(const MLP &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setRegressifier(...) method is called.
    It clones the data from the Base Class Regressifier pointer (which should be pointing to an MLP instance) into this instance
    
    @param regressifier: a pointer to the Regressifier Base Class, this should be pointing to another MLP instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Regressifier *regressifier);
    
    /**
    This trains the MLP model, using the labelled classification data. Calling this function sets the MLP into Classification Model.
    
    @param trainingData: the training data that will be used to train the classification model
    @return returns true if the MLP model was trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
    This trains the MLP model, using the labelled regression data. Calling this function sets the MLP into Regression Model.
    
    @param trainingData: the training data that will be used to train the regression model
    @return returns true if the MLP model was trained, false otherwise
    */
    virtual bool train_(RegressionData &trainingData);
    
    /**
    This function either predicts the class of the input Vector (if the MLP is in Classification Mode), or it performs regression using
    the MLP model.
    
    @param inputVector: the input Vector to classify or perform regression on
    @return returns true if the prediction/regression was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
    Clears any previous model or settings.
    
    @return returns true if the MLP was cleared, false otherwise
    */
    virtual bool clear();
    
    /**
    This function will print the model and settings to the display log.
    
    @return returns true if the model was printed succesfully, false otherwise
    */
    virtual bool print() const;
    
    /**
    This saves the trained MLP model to a file.
    This overrides the save function in the ML base class.
    
    @param file: a reference to the file the MLP model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained MLP model from a file.
    This overrides the load function in the ML base class.
    
    @param file: a reference to the file the MLP model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    Returns the number of classes in the MLP model if the MLP is in classification mode.
    The number of classes in the model is the same as the number of output neurons.
    If the MLP is in regression mode then it will return 0.
    
    @return returns the number of classes in the MLP model if the MLP is in classification mode
    */
    UINT getNumClasses() const;
    
    /**
    Initializes the MLP for training. This should be called before the MLP is trained.
    The number of input neurons should match the number of input dimensions in your training data.
    The number of output neurons should match the number of target dimensions in your training data.
    The number of hidden units should be chosen by the user, a common rule of thumb is to set this as
    a value somewhere between the number of input neurons and the number of output neurons.
    Initializaling the MLP will clear any previous model or settings.
    
    This function calls the other init function below, passing in the layer activation functions.
    
    @param numInputNeurons: the number of input neurons (should match the number of input dimensions in your training data)
    @param numHiddenNeurons: the number of hidden neurons
    @param numOutputNeurons: the number of output neurons (should match the number of target dimensions in your training data)
    @return returns true if the MLP was initialized, false otherwise
    */
    bool init(const UINT numInputNeurons, const UINT numHiddenNeurons, const UINT numOutputNeurons);
    
    /**
    Initializes the MLP for training. This should be called before the MLP is trained.
    The number of input neurons should match the number of input dimensions in your training data.
    The number of output neurons should match the number of target dimensions in your training data.
    The number of hidden units should be chosen by the user, a common rule of thumb is to set this as
    a value somewhere between the number of input neurons and the number of output neurons.
    The activation functions should be one of the Neuron ActivationFunctions enums.
    Initializaling the MLP will clear any previous model or settings.
    
    @param numInputNeurons: the number of input neurons (should match the number of input dimensions in your training data)
    @param numHiddenNeurons: the number of hidden neurons
    @param numOutputNeurons: the number of output neurons (should match the number of target dimensions in your training data)
    @param inputLayerActivationFunction: the activation function to use for the input layer
    @param hiddenLayerActivationFunction: the activation function to use for the input layer
    @param outputLayerActivationFunction: the activation function to use for the input layer
    @return returns true if the MLP was initialized, false otherwise
    */
    bool init(const UINT numInputNeurons, const UINT numHiddenNeurons, const UINT numOutputNeurons, 
              const Neuron::Type inputLayerActivationFunction,
              const Neuron::Type hiddenLayerActivationFunction, 
              const Neuron::Type outputLayerActivationFunction);
    
    /**
    Prints the current MLP weights and coefficents to std out.
    This function is depreciated, you should now use print() instead.
    */
    void printNetwork() const;
    
    /**
    Checks if there are any NAN values in any of the layers.
    
    @return returns true if there are any NAN values in any of the layers, false otherwise
    */
    bool checkForNAN() const;
    
    /**
    Gets a string representation of the activation function
    
    @param activationFunction: the activation function you want to convert to a string
    @return returns a string representation of the activation function, returns UNKNOWN if the activation function is invalid
    */
    std::string activationFunctionToString(const Neuron::Type activationFunction) const;
    
    /**
    Gets the activation function value from a string.
    
    @param activationName: the activation function as a string
    @return returns the activation function
    */
    Neuron::Type activationFunctionFromString(const std::string activationName) const;
    
    /**
    Validates if the activationFunction is valid.
    
    @param activationFunction: the activation function you want to valid
    @return returns true if the activation function is valid, false otherwise
    */
    bool validateActivationFunction(const Neuron::Type avactivationFunction) const;
    
    /**
    Gets the number of input neurons.
    
    @return returns the number of input neurons
    */
    UINT getNumInputNeurons() const;
    
    /**
    Gets the number of hidden neurons.
    
    @return returns the number of hidden neurons
    */
    UINT getNumHiddenNeurons() const;
    
    /**
    Gets the number of output neurons.
    
    @return returns the number of output neurons
    */
    UINT getNumOutputNeurons() const;

    /**
    Gets the number of random training iterations that should be performed during the training phase.
    The MLP back propagation algorithm starts with random values, and the accuracy of a trained model can depend on which random values
    the algorithm started with.  The GRT MLP algorithm therefore trains a number of models and picks the best one.  This value therefore
    represents the number of random training iterations that should be used.
    
    @return returns the number of random training iterations that should be performed during the training phase
    */
    UINT getNumRandomTrainingIterations() const;
    
    /**
    Gets the input layer activation function.
    
    @return returns the input layer activation function
    */
    Neuron::Type getInputLayerActivationFunction() const;
    
    /**
    Gets the hidden layer activation function.
    
    @return returns the hidden layer activation function
    */
    Neuron::Type getHiddenLayerActivationFunction() const;
    
    /**
    Gets the output layer activation function.
    
    @return returns the output layer activation function
    */
    Neuron::Type getOutputLayerActivationFunction() const;
    
    /**
    Gets the training rate. This should be a value between [0 1]
    
    @return returns the training rate
    */
    Float getTrainingRate() const;
    
    /**
    Gets the momentum rate. This should be a value between [0 1]
    
    @return returns the momentum
    */
    Float getMomentum() const;
    
    /**
    Gets the gamma value. This controls the gamma parameter for the neurons.
    
    @return returns the gamma value for the neurons
    */
    Float getGamma() const;
    
    /**
    Gets training error from the last round of training. If the MLP is in classification mode, the training error will be the classification
    accuracy.  If the MLP is in regression mode then the training error will be the RMS error.
    
    @return returns the training error from the last round of training
    */
    Float getTrainingError() const;
    
    /**
    Returns true if the MLP is in classification mode.
    
    @return returns true if the MLP is in classification mode, false otherwise
    */
    bool getClassificationModeActive() const;
    
    /**
    Returns true if the MLP is in regression mode.
    
    @return returns true if the MLP is in regression mode, false otherwise
    */
    bool getRegressionModeActive() const;
    
    /**
    Returns the neurons for the input layer.
    
    @return returns a Vector of neurons for the input layer
    */
    Vector< Neuron > getInputLayer() const;
    
    /**
    Returns the neurons for the hidden layer.
    
    @return returns a Vector of neurons for the hidden layer
    */
    Vector< Neuron > getHiddenLayer() const;
    
    /**
    Returns the neurons for the output layer.
    
    @return returns a Vector of neurons for the output layer
    */
    Vector< Neuron > getOutputLayer() const;
    
    /**
    Returns a Vector of VectorFloat representing the training log for each random round of training.
    The outer Vector represents each round and the inner Vector represents each epoch in that round.
    
    @return returns a Vector of MinMax values representing the ranges of the output layer
    */
    Vector< VectorFloat > getTrainingLog() const;
    
    /**
    Returns true if the MLP should use null rejection during classification. This is only relevant if the MLP is in classification mode.
    
    @return returns true if the null rejection is enabled, false otherwise
    */
    bool getNullRejectionEnabled() const;
    
    /**
    Returns the current null rejection coefficient value.
    The null rejection coefficient is a multipler controlling the null rejection threshold for each class.
    This is only relevant if the MLP is in classification mode.
    
    @return returns the current null rejection coefficient
    */
    Float getNullRejectionCoeff() const;
    
    /**
    Returns the current null rejection threshold value.
    The null rejection threshold is value controlling if a classification result should be rejected or accepted.
    This is only relevant if the MLP is in classification mode.
    
    @return returns the current null rejection threshold
    */
    Float getNullRejectionThreshold() const;
    
    /**
    Returns the current maximumLikelihood value.
    The maximumLikelihood value is computed during the prediction phase and is the likelihood of the most likely model.
    This is only relevant if the MLP is in classification mode.
    This value will return 0 if a prediction has not been made.
    
    @return returns the current maximumLikelihood value
    */
    Float getMaximumLikelihood() const;
    
    /**
    Gets a Vector of the class likelihoods from the last prediction, this will be an N-dimensional Vector, where N is the number of classes in the model.
    
    @return returns a Vector of the class likelihoods from the last prediction, an empty Vector will be returned if the model has not been trained
    */
    VectorFloat getClassLikelihoods() const;
    
    /**
    Gets a Vector of the class distances from the last prediction, this will be an N-dimensional Vector, where N is the number of classes in the model.
    
    @return returns a Vector of the class distances from the last prediction, an empty Vector will be returned if the model has not been trained
    */
    VectorFloat getClassDistances() const;
    
    /**
    Gets the predicted class label from the last prediction. This is only relevant if the MLP is in classification mode.
    
    @return returns the label of the last predicted class, a value of 0 will be returned if the model has not been trained
    */
    UINT getPredictedClassLabel() const;
    
    /**
    This function sets the activation function for all the Neurons in the input layer.  If the MLP instance has been initialized
    then this function will also call the init function to reinitialize the instance.
    
    @param activationFunction: the activation function for the input layer, this should be one of the Neuron ActivationFunctions enums
    @return returns true if the input layer activation function was set successfully, false otherwise
    */
    bool setInputLayerActivationFunction(const Neuron::Type activationFunction);
    
    /**
    This function sets the activation function for all the Neurons in the hidden layer.  If the MLP instance has been initialized
    then this function will also call the init function to reinitialize the instance.
    
    @param activationFunction: the activation function for the hidden layer, this should be one of the Neuron ActivationFunctions enums
    @return returns true if the hidden layer activation function was set successfully, false otherwise
    */
    bool setHiddenLayerActivationFunction(const Neuron::Type activationFunction);
    
    /**
    This function sets the activation function for all the Neurons in the output layer.  If the MLP instance has been initialized
    then this function will also call the init function to reinitialize the instance.
    
    @param activationFunction: the activation function for the output layer, this should be one of the Neuron ActivationFunctions enums
    @return returns true if the output layer activation function was set successfully, false otherwise
    */
    bool setOutputLayerActivationFunction(const Neuron::Type activationFunction);
    
    /**
    Sets the training rate, which controls the learning rate parameter. This is used to update the weights at each step of the stochastic gradient descent.
    The learningRate value must be greater than zero, a value of 0.1 normally works well. If you find the MLP fails to train with this value then try
    setting the learning rate to a smaller value (for example 0.01).
    
    @param trainingRate: the learningRate value used during the training phase, must be greater than zero
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setTrainingRate(const Float trainingRate);
    
    /**
    Sets the momentum parameter. This is used to update the weights at each step of the stochastic gradient descent.
    The momentum parameter is normally set between [0.1 0.9], with 0.5 being a common value.
    
    @param momentum: the momentum value used during the training phase, must be greater than zero
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setMomentum(const Float momentum);
    
    /**
    Sets the gamma parameter for the Neurons. Gamma must be greater than zero.
    If the MLP instance has been initialized then this function will also call the init function to reinitialize the instance.
    
    @param gamma: the gamma value for each Neuron, gamma must be greater than zero
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setGamma(const Float gamma);
    
    /**
    Sets number of times the MLP model should be trained to find the best model.  This value must be greater than zero.
    
    Setting this value to a high number (i.e. 100) will most likely give you a better model, however it will take much longer to train
    the overall model.  Setting this value to a low number (i.e. 5) will make the training process much faster, but you might not get the
    best model.
    
    @param numRandomTrainingIterations: the number of times you want to randomly train the MLP model to search for the best results
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setNumRandomTrainingIterations(const UINT numRandomTrainingIterations);
    
    /**
    Sets if null rejection should be used for the real-time prediction.  This is only used if the MLP is in classificationMode.
    
    @param useNullRejection: if true then null rejection will be used
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setNullRejection(const bool useNullRejection);
    
    /**
    This function lets you manually control the null rejection threshold. Any class with a prediction value less than the null rejection threshold
    will be rejected, setting the predicted class label to 0.
    
    This is only used if the MLP is in classificationMode.
    
    @param nullRejectionCoeff: the new null rejection threshold
    @return returns true if the value was updated successfully, false otherwise
    */
    bool setNullRejectionCoeff(const Float nullRejectionCoeff);

    /**
    Gets a string that represents the MLP class.
    
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
    bool inline isNAN(const Float v) const;
    
    bool trainModel(RegressionData &trainingData);
    
    bool trainOnlineGradientDescentClassification(const RegressionData &trainingData,const RegressionData &validationData);
    
    bool trainOnlineGradientDescentRegression(const RegressionData &trainingData,const RegressionData &validationData);
    
    bool loadLegacyModelFromFile( std::fstream &file );
    
    /**
    Performs one round of back propagation, using the training example and target Vector
    
    @param inputVector: the input Vector to use for back propagation
    @param targetVector: the target Vector to use for back propagation
    @param alpha: the training rate
    @param beta: the momentum
    @return returns the squared error for the current training example
    */
    Float back_prop(const VectorFloat &inputVector,const VectorFloat &targetVector,const Float alpha,const Float beta);
    
    /**
    Performs the feedforward step using the current model and the input training example.
    
    @param data: the input Vector to use for the feedforward
    @return returns a new Vector of the results from the feedforward step
    */
    VectorFloat feedforward(VectorFloat data);
    
    /**
    Performs the feedforward step for back propagation, using the input data
    
    @param data: the input Vector to use for the feedforward
    @param inputNeuronsOutput: the results of the input layer
    @param hiddenNeuronsOutput: the results of the hidden layer
    @param outputNeuronsOutput: the results of the output layer
    */
    void feedforward(const VectorFloat &data,VectorFloat &inputNeuronsOutput,VectorFloat &hiddenNeuronsOutput,VectorFloat &outputNeuronsOutput);
    
    UINT numInputNeurons;
    UINT numHiddenNeurons;
    UINT numOutputNeurons;
    Neuron::Type inputLayerActivationFunction;
    Neuron::Type hiddenLayerActivationFunction;
    Neuron::Type outputLayerActivationFunction;
    UINT numRandomTrainingIterations;
    UINT trainingMode;
    Float momentum;
    Float gamma;
    Float trainingError;
    bool initialized;
    
    Vector< Neuron > inputLayer;
    Vector< Neuron > hiddenLayer;
    Vector< Neuron > outputLayer;
    Vector< VectorFloat > trainingErrorLog;
    
    //Classifier Variables
    bool classificationModeActive;
    bool useNullRejection;
    UINT predictedClassLabel;
    Float nullRejectionThreshold;
    Float nullRejectionCoeff;
    Float maxLikelihood;
    VectorFloat classLikelihoods;
    
    //Temp Training Stuff
    VectorFloat inputNeuronsOutput;
    VectorFloat hiddenNeuronsOutput;
    VectorFloat outputNeuronsOutput;
    VectorFloat deltaO;
    VectorFloat deltaH;

private:
    static RegisterRegressifierModule< MLP > registerModule;
    static const std::string id;
     
};

GRT_END_NAMESPACE

#endif //GRT_MLP_HEADER
