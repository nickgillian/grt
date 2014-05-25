/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a Multilayer Perceptron Artificial Neural Network.
 
 @example RegressionModulesExamples/MLPRegressionExample/MLPRegressionExample.cpp
 
 @remark This implementation is based on Bishop, Christopher M. Pattern recognition and machine learning. Vol. 1. New York: springer, 2006.
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

namespace GRT{

class MLP : public Regressifier{
public:
    /**
     Default Constructor
     */
    MLP();
    
    /**
     Copy Constructor
     */
    MLP(const MLP &rhs);
    
    /**
     Default Destructor
     */
    virtual ~MLP();
    
    /**
     Defines how the data from the rhs MLP should be copied to this MLP
     
     @param const MLP &rhs: another instance of a MLP
     @return returns a reference to this instance of the MLP
     */
    MLP &operator=(const MLP &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setRegressifier(...) method is called.
     It clones the data from the Base Class Regressifier pointer (which should be pointing to an MLP instance) into this instance
     
     @param Regressifier *regressifier: a pointer to the Regressifier Base Class, this should be pointing to another MLP instance
     @return returns true if the clone was successfull, false otherwise
     */
    virtual bool deepCopyFrom(const Regressifier *regressifier);
    
    /**
     This trains the MLP model, using the labelled classification data. Calling this function sets the MLP into Classification Model.
     
     @param ClassificationData &trainingData: the training data that will be used to train the classification model
     @return returns true if the MLP model was trained, false otherwise
     */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This trains the MLP model, using the labelled regression data. Calling this function sets the MLP into Regression Model.
     
     @param RegressionData trainingData: the training data that will be used to train the regression model
     @return returns true if the MLP model was trained, false otherwise
     */
    virtual bool train_(RegressionData &trainingData);
    
    /**
     This function either predicts the class of the input vector (if the MLP is in Classification Mode), or it performs regression using
     the MLP model.
     
     @param VectorDouble &inputVector: the input vector to classify or perform regression on
     @return returns true if the prediction/regression was performed, false otherwise
     */
    virtual bool predict_(VectorDouble &inputVector);
    
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
     This overrides the saveModelToFile function in the ML base class.
     
     @param fstream &file: a reference to the file the MLP model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained MLP model from a file.
     This overrides the loadModelFromFile function in the ML base class.
     
     @param fstream &file: a reference to the file the MLP model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
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
     
     @param const UINT numInputNeurons: the number of input neurons (should match the number of input dimensions in your training data)
     @param const UINT numHiddenNeurons: the number of hidden neurons
     @param const UINT numOutputNeurons: the number of output neurons (should match the number of target dimensions in your training data)
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
     
     @param const UINT numInputNeurons: the number of input neurons (should match the number of input dimensions in your training data)
     @param const UINT numHiddenNeurons: the number of hidden neurons
     @param const UINT numOutputNeurons: the number of output neurons (should match the number of target dimensions in your training data)
     @param const UINT inputLayerActivationFunction: the activation function to use for the input layer
     @param const UINT hiddenLayerActivationFunction: the activation function to use for the input layer
     @param const UINT outputLayerActivationFunction: the activation function to use for the input layer
     @return returns true if the MLP was initialized, false otherwise
     */
    bool init(const UINT numInputNeurons, const UINT numHiddenNeurons, const UINT numOutputNeurons, const UINT inputLayerActivationFunction,
              const UINT hiddenLayerActivationFunction, const UINT outputLayerActivationFunction);
    
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
     
     @param UINT activationFunction: the activation function you want to convert to a string
     @return returns a string representation of the activation function, returns UNKNOWN if the activation function is invalid
     */
	string activationFunctionToString(const UINT activationFunction) const;
    
    /**
     Gets the activation function value from a string.
     
     @param string activationName: the activation function as a string
     @return returns an UINT activation function
     */
	UINT activationFunctionFromString(const string activationName) const;
    
    /**
     Validates if the activationFunction is valid.
     
     @param UINT activationFunction: the activation function you want to valid
     @return returns true if the activation function is valid, false otherwise
     */
	bool validateActivationFunction(const UINT avactivationFunction) const;

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
     Gets the input layer activation function.
     
     @return returns the input layer activation function
     */
	UINT getInputLayerActivationFunction() const;
    
    /**
     Gets the hidden layer activation function.
     
     @return returns the hidden layer activation function
     */
	UINT getHiddenLayerActivationFunction() const;
    
    /**
     Gets the output layer activation function.
     
     @return returns the output layer activation function
     */
	UINT getOutputLayerActivationFunction() const;
    
    /**
     Gets the number of random training iterations that should be performed during the training phase.
     The MLP back propagation algorithm starts with random values, and the accuracy of a trained model can depend on which random values
     the algorithm started with.  The GRT MLP algorithm therefore trains a number of models and picks the best one.  This value therefore
     represents the number of random training iterations that should be used.
     
     @return returns the number of random training iterations that should be performed during the training phase
     */
	UINT getNumRandomTrainingIterations() const;
    
    /**
     Gets the training rate. This should be a value between [0 1]
     
     @return returns the training rate
     */
	double getTrainingRate() const;
    
    /**
     Gets the momentum rate. This should be a value between [0 1]
     
     @return returns the momentum
     */
	double getMomentum() const;
    
    /**
     Gets the gamma value. This controls the gamma parameter for the neurons.
     
     @return returns the gamma value for the neurons
     */
	double getGamma() const;
    
    /**
     Gets training error from the last round of training. If the MLP is in classification mode, the training error will be the classification
     accuracy.  If the MLP is in regression mode then the training error will be the RMS error.
     
     @return returns the training error from the last round of training
     */
	double getTrainingError() const;

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
     
     @return returns a vector of neurons for the input layer
     */
	vector< Neuron > getInputLayer() const;
    
    /**
     Returns the neurons for the hidden layer.
     
     @return returns a vector of neurons for the hidden layer
     */
	vector< Neuron > getHiddenLayer() const;
    
    /**
     Returns the neurons for the output layer.
     
     @return returns a vector of neurons for the output layer
     */
	vector< Neuron > getOutputLayer() const;
    
    /**
     Returns a vector of VectorDouble representing the training log for each random round of training.
     The outer vector represents each round and the inner vector represents each epoch in that round.
     
     @return returns a vector of MinMax values representing the ranges of the output layer
     */
	vector< VectorDouble > getTrainingLog() const;
    
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
    double getNullRejectionCoeff() const;
    
    /**
     Returns the current null rejection threshold value.
     The null rejection threshold is value controlling if a classification result should be rejected or accepted.
     This is only relevant if the MLP is in classification mode.
     
     @return returns the current null rejection threshold
     */
    double getNullRejectionThreshold() const;
    
    /**
     Returns the current maximumLikelihood value.
     The maximumLikelihood value is computed during the prediction phase and is the likelihood of the most likely model.
     This is only relevant if the MLP is in classification mode.
     This value will return 0 if a prediction has not been made.
     
     @return returns the current maximumLikelihood value
     */
    double getMaximumLikelihood() const;
    
    /**
     Gets a vector of the class likelihoods from the last prediction, this will be an N-dimensional vector, where N is the number of classes in the model.
     
     @return returns a vector of the class likelihoods from the last prediction, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getClassLikelihoods() const;
    
    /**
     Gets a vector of the class distances from the last prediction, this will be an N-dimensional vector, where N is the number of classes in the model.
     
     @return returns a vector of the class distances from the last prediction, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getClassDistances() const;
    
    /**
     Gets the predicted class label from the last prediction. This is only relevant if the MLP is in classification mode.
     
     @return returns the label of the last predicted class, a value of 0 will be returned if the model has not been trained
     */
    UINT getPredictedClassLabel() const;

    /**
     This function sets the activation function for all the Neurons in the input layer.  If the MLP instance has been initialized 
     then this function will also call the init function to reinitialize the instance.
     
     @param const UINT activationFunction: the activation function for the input layer, this should be one of the Neuron ActivationFunctions enums
     @return returns true if the input layer activation function was set successfully, false otherwise
     */
	bool setInputLayerActivationFunction(const UINT activationFunction);
    
    /**
     This function sets the activation function for all the Neurons in the hidden layer.  If the MLP instance has been initialized
     then this function will also call the init function to reinitialize the instance.
     
     @param const UINT activationFunction: the activation function for the hidden layer, this should be one of the Neuron ActivationFunctions enums
     @return returns true if the hidden layer activation function was set successfully, false otherwise
     */
	bool setHiddenLayerActivationFunction(const UINT activationFunction);
    
    /**
     This function sets the activation function for all the Neurons in the output layer.  If the MLP instance has been initialized
     then this function will also call the init function to reinitialize the instance.
     
     @param const UINT activationFunction: the activation function for the output layer, this should be one of the Neuron ActivationFunctions enums
     @return returns true if the output layer activation function was set successfully, false otherwise
     */
	bool setOutputLayerActivationFunction(const UINT activationFunction);
	
    /**
     Sets the training rate, which controls the learning rate parameter. This is used to update the weights at each step of the stochastic gradient descent.
     The learningRate value must be greater than zero, a value of 0.1 normally works well. If you find the MLP fails to train with this value then try 
     setting the learning rate to a smaller value (for example 0.01).
     
     @param const double trainingRate: the learningRate value used during the training phase, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setTrainingRate(const double trainingRate);
    
    /**
     Sets the momentum parameter. This is used to update the weights at each step of the stochastic gradient descent.
     The momentum parameter is normally set between [0.1 0.9], with 0.5 being a common value.
     
     @param const double momentum: the momentum value used during the training phase, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
	bool setMomentum(const double momentum);
    
    /**
     Sets the gamma parameter for the Neurons. Gamma must be greater than zero.
     If the MLP instance has been initialized then this function will also call the init function to reinitialize the instance.
     
     @param const double gamma: the gamma value for each Neuron, gamma must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
	bool setGamma(const double gamma);
	
	/**
     Sets number of times the MLP model should be trained to find the best model.  This value must be greater than zero.
     
     Setting this value to a high number (i.e. 100) will most likely give you a better model, however it will take much longer to train 
     the overall model.  Setting this value to a low number (i.e. 5) will make the training process much faster, but you might not get the 
     best model.
     
     @param const UINT numRandomTrainingIterations: the number of times you want to randomly train the MLP model to search for the best results
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setNumRandomTrainingIterations(const UINT numRandomTrainingIterations);
    
    /**
     Sets if null rejection should be used for the real-time prediction.  This is only used if the MLP is in classificationMode.
     
     @param const bool useNullRejection: if true then null rejection will be used
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setNullRejection(const bool useNullRejection);
    
    /**
     This function lets you manually control the null rejection threshold. Any class with a prediction value less than the null rejection threshold
     will be rejected, setting the predicted class label to 0.
     
     This is only used if the MLP is in classificationMode.
     
     @param const double nullRejectionCoeff: the new null rejection threshold
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setNullRejectionCoeff(const double nullRejectionCoeff);
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::saveModelToFile;
    using MLBase::loadModelFromFile;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;
    
protected:
    bool inline isNAN(const double v) const;
    
    bool trainModel(RegressionData &trainingData);
    
    bool trainOnlineGradientDescentClassification(const RegressionData &trainingData,const RegressionData &validationData);
    
    bool trainOnlineGradientDescentRegression(const RegressionData &trainingData,const RegressionData &validationData);
    
    bool loadLegacyModelFromFile( fstream &file );
    
    /**
     Performs one round of back propagation, using the training example and target vector
     
     @param const VectorDouble &trainingExample: the input vector to use for back propagation
     @param const VectorDouble &targetVector: the target vector to use for back propagation
     @param const double alpha: the training rate
     @param const double beta: the momentum
     @return returns the squared error for the current training example
     */
    double back_prop(const VectorDouble &trainingExample,const VectorDouble &targetVector,const double alpha,const double beta);
    
    /**
     Performs the feedforward step using the current model and the input training example.
     
     @param VectorDouble trainingExample: the input vector to use for the feedforward
     @return returns a new vector of the results from the feedforward step
     */
    VectorDouble feedforward(VectorDouble trainingExample);
    
    /**
     Performs the feedforward step for back propagation, using the input data
     
     @param const VectorDouble &trainingExample: the input vector to use for the feedforward
     @param VectorDouble &inputNeuronsOuput: the results of the input layer 
     @param VectorDouble &hiddenNeuronsOutput: the results of the hidden layer
     @param VectorDouble &outputNeuronsOutput: the results of the output layer
     */
    void feedforward(const VectorDouble &trainingExample,VectorDouble &inputNeuronsOuput,VectorDouble &hiddenNeuronsOutput,VectorDouble &outputNeuronsOutput);
    
    UINT numInputNeurons;
    UINT numHiddenNeurons;
    UINT numOutputNeurons;
    UINT inputLayerActivationFunction;
    UINT hiddenLayerActivationFunction;
    UINT outputLayerActivationFunction;
    UINT numRandomTrainingIterations;
    UINT trainingMode;
	double momentum;
	double gamma;
    double trainingError;
    bool initialized;
    Random random;
    
    vector< Neuron > inputLayer;
    vector< Neuron > hiddenLayer;
    vector< Neuron > outputLayer;
	vector< VectorDouble > trainingErrorLog;
    
    //Classifier Variables
    bool classificationModeActive;
    bool useNullRejection;
    UINT predictedClassLabel;
    double nullRejectionThreshold;
    double nullRejectionCoeff;
    double maxLikelihood;
    VectorDouble classLikelihoods;
    
    static RegisterRegressifierModule< MLP > registerModule;
    
    //Temp Training Stuff
    VectorDouble inputNeuronsOuput;
    VectorDouble hiddenNeuronsOutput;
    VectorDouble outputNeuronsOutput;
    VectorDouble deltaO;
    VectorDouble deltaH;
    
public:
    enum TrainingModes{ONLINE_GRADIENT_DESCENT};
    
};

} //End of namespace GRT

#endif //GRT_MLP_HEADER
