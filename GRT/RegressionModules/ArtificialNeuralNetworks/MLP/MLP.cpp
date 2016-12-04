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

#define GRT_DLL_EXPORTS
#include "MLP.h"
#include "../../../CoreModules/Regressifier.h"

GRT_BEGIN_NAMESPACE

const Float MLP_NEURON_MIN_TARGET = -1.0;
const Float MLP_NEURON_MAX_TARGET = 1.0;

//Define the string that will be used to identify the object
const std::string MLP::id = "MLP";
std::string MLP::getId() { return MLP::id; }

//Register the MLP module with the base class
RegisterRegressifierModule< MLP > MLP::registerModule( MLP::getId() );

MLP::MLP() : Regressifier( MLP::getId() )
{
    inputLayerActivationFunction = Neuron::LINEAR;
    hiddenLayerActivationFunction = Neuron::TANH;
    outputLayerActivationFunction = Neuron::LINEAR;
    minNumEpochs = 10;
    numRandomTrainingIterations = 10;
    validationSetSize = 20; //20% of the training data will be set aside for the validation set
    trainingMode = ONLINE_GRADIENT_DESCENT;
    momentum = 0.5;
    gamma = 2.0;
    trainingError = 0;
    nullRejectionCoeff = 0.9;
    nullRejectionThreshold = 0;
    useValidationSet = true;
    randomiseTrainingOrder = false;
    useScaling = true;
    trained = false;
    initialized = false;
    classificationModeActive = false;
    useNullRejection = true;
    clear();
}

MLP::MLP(const MLP &rhs) : Regressifier( MLP::getId() )
{  
    *this = rhs;
}

MLP::~MLP(){
    clear();
}

MLP& MLP::operator=(const MLP &rhs){
    if( this != &rhs ){
        //MLP variables
        this->numInputNeurons = rhs.numInputNeurons;
        this->numHiddenNeurons = rhs.numHiddenNeurons;
        this->numOutputNeurons = rhs.numOutputNeurons;
        this->inputLayerActivationFunction = rhs.inputLayerActivationFunction;
        this->hiddenLayerActivationFunction = rhs.hiddenLayerActivationFunction;
        this->outputLayerActivationFunction = rhs.outputLayerActivationFunction;
        this->numRandomTrainingIterations = rhs.numRandomTrainingIterations;
        this->trainingMode = rhs.trainingMode;
        this->momentum = rhs.momentum;
        this->trainingError = rhs.trainingError;
        this->gamma = rhs.gamma;
        this->initialized = rhs.initialized;
        this->inputLayer = rhs.inputLayer;
        this->hiddenLayer = rhs.hiddenLayer;
        this->outputLayer = rhs.outputLayer;
        this->inputVectorRanges = rhs.inputVectorRanges;
        this->targetVectorRanges = rhs.targetVectorRanges;
        this->trainingErrorLog = rhs.trainingErrorLog;
        
        this->classificationModeActive = rhs.classificationModeActive;
        this->useNullRejection = rhs.useNullRejection;
        this->predictedClassLabel = rhs.predictedClassLabel;
        this->nullRejectionCoeff = rhs.nullRejectionCoeff;
        this->nullRejectionThreshold = rhs.nullRejectionThreshold;
        this->maxLikelihood = rhs.maxLikelihood;
        this->classLikelihoods = rhs.classLikelihoods;
        
        //Copy the base variables
        copyBaseVariables( (Regressifier*)&rhs );
    }
    return *this;
}

bool MLP::deepCopyFrom(const Regressifier *regressifier){
    
    if( regressifier == NULL ){
        errorLog << "deepCopyFrom(const Regressifier *regressifier) - regressifier is NULL!" << std::endl;
        return false;
    }
    
    if( this->getId() != regressifier->getId() ){
        errorLog << "deepCopyFrom(const Regressifier *regressifier) - regressifier is not the correct type!" << std::endl;
        return false;
    }
    
    *this = *dynamic_cast<const MLP*>(regressifier);
    
    return true;
}

//Classifier interface
bool MLP::train_(ClassificationData &trainingData){
    
    if( !initialized ){
        errorLog << "train_(ClassificationData trainingData) - The MLP has not been initialized!" << std::endl;
        return false;
    }
    
    if( trainingData.getNumDimensions() != numInputNeurons ){
        errorLog << "train_(ClassificationData trainingData) - The number of input dimensions in the training data (" << trainingData.getNumDimensions() << ") does not match that of the MLP (" << numInputNeurons << ")" << std::endl;
        return false;
    }
    if( trainingData.getNumClasses() != numOutputNeurons ){
        errorLog << "train_(ClassificationData trainingData) - The number of classes in the training data (" << trainingData.getNumClasses() << ") does not match that of the MLP (" << numOutputNeurons << ")" << std::endl;
        return false;
    }
    
    //Reformat the LabelledClassificationData as LabelledRegressionData
    RegressionData regressionData = trainingData.reformatAsRegressionData();
    
    //Flag that the MLP is being used for classification, not regression
    classificationModeActive = true;
    
    return trainModel(regressionData);
}

bool MLP::train_(RegressionData &trainingData){
    
    //Flag that the MLP is being used for regression, not classification
    classificationModeActive = false;
    
    return trainModel(trainingData);
}

//Classifier interface
bool MLP::predict_(VectorFloat &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &inputVector) - Model not trained!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputNeurons ){
        errorLog << "predict_(VectorFloat &inputVector) - The sie of the input Vector (" << int(inputVector.size()) << ") does not match that of the number of input dimensions (" << numInputNeurons << ") " << std::endl;
        return false;
    }
    
    //Set the mapped data as the classLikelihoods
    regressionData = feedforward( inputVector );
    
    if( classificationModeActive ){
        
        //Estimate the class likelihoods
        const UINT K = (UINT)regressionData.size();
        classLikelihoods = regressionData;
        
        //Make sure all the values are greater than zero, we do this by finding the min value and adding this onto all the values
        Float minValue = Util::getMin( classLikelihoods );
        for(UINT i=0; i<K; i++){
            classLikelihoods[i] += minValue;
        }
        
        //Normalize the likelihoods so they sum to 1
        Float sum = Util::sum(classLikelihoods);
        if( sum > 0 ){
            for(UINT i=0; i<K; i++){
                classLikelihoods[i] /= sum;
            }
        }
        
        //Find the best value
        Float bestValue = classLikelihoods[0];
        UINT bestIndex = 0;
        for(UINT i=1; i<K; i++){
            if( classLikelihoods[i] > bestValue ){
                bestValue = classLikelihoods[i];
                bestIndex = i;
            }
        }
        
        //Set the maximum likelihood and predicted class label
        maxLikelihood = bestValue;
        predictedClassLabel = bestIndex+1;
        
        if( useNullRejection ){
            if( maxLikelihood < nullRejectionCoeff ){
                predictedClassLabel = 0;
            }
        }
    }
    
    return true;
}

bool MLP::init(const UINT numInputNeurons, const UINT numHiddenNeurons, const UINT numOutputNeurons){
    return init(numInputNeurons, numHiddenNeurons, numOutputNeurons, inputLayerActivationFunction, hiddenLayerActivationFunction, outputLayerActivationFunction );
}

bool MLP::init(const UINT numInputNeurons,
                const UINT numHiddenNeurons,
                const UINT numOutputNeurons,
                const Neuron::Type inputLayerActivationFunction,
                const Neuron::Type hiddenLayerActivationFunction,
                const Neuron::Type outputLayerActivationFunction){
    
    //Clear any previous models
    clear();
    
    //Initialize the random seed
    random.setSeed( (UINT)time(NULL) );
    
    if( numInputNeurons == 0 || numHiddenNeurons == 0 || numOutputNeurons == 0 ){
        if( numInputNeurons == 0 ){  errorLog << "init(...) - The number of input neurons is zero!" << std::endl; }
        if( numHiddenNeurons == 0 ){  errorLog << "init(...) - The number of hidden neurons is zero!" << std::endl; }
        if( numOutputNeurons == 0 ){  errorLog << "init(...) - The number of output neurons is zero!" << std::endl; }
        return false;
    }
    
    //Validate the activation functions
    if( !validateActivationFunction(inputLayerActivationFunction) || !validateActivationFunction(hiddenLayerActivationFunction) || !validateActivationFunction(outputLayerActivationFunction) ){
        errorLog << "init(...) - One Of The Activation Functions Failed The Validation Check" << std::endl;
        return false;
    }
    
    //Set the size of the MLP
    this->numInputNeurons = numInputNeurons;
    this->numHiddenNeurons = numHiddenNeurons;
    this->numOutputNeurons = numOutputNeurons;
    
    //Set the regression IO
    this->numInputDimensions = numInputNeurons;
    this->numOutputDimensions = numOutputNeurons;
    
    //Set the validation layers
    this->inputLayerActivationFunction = inputLayerActivationFunction;
    this->hiddenLayerActivationFunction = hiddenLayerActivationFunction;
    this->outputLayerActivationFunction = outputLayerActivationFunction;
    
    //Setup the neurons for each of the layers
    inputLayer.resize(numInputNeurons);
    hiddenLayer.resize(numHiddenNeurons);
    outputLayer.resize(numOutputNeurons);
    
    //Init the neuron memory for each of the layers
    for(UINT i=0; i<numInputNeurons; i++){
        inputLayer[i].init(1,inputLayerActivationFunction); //The input size for each input neuron will always be 1
        inputLayer[i].weights[0] = 1.0; //The weights for the input layer should always be 1
        inputLayer[i].bias = 0.0; //The bias for the input layer should always be 0
        inputLayer[i].gamma = gamma;
    }
    
    for(UINT i=0; i<numHiddenNeurons; i++){
        //The number of inputs to a neuron in the output layer will always match the number of input neurons
        hiddenLayer[i].init(numInputNeurons,hiddenLayerActivationFunction,-0.1,0.1);
        hiddenLayer[i].gamma = gamma;
    }
    
    for(UINT i=0; i<numOutputNeurons; i++){
        //The number of inputs to a neuron in the output layer will always match the number of hidden neurons
        outputLayer[i].init(numHiddenNeurons,outputLayerActivationFunction,-0.1,0.1);
        outputLayer[i].gamma = gamma;
    }
    
    initialized = true;
    
    return true;
}

bool MLP::clear(){
    
    //Clear the base class
    Regressifier::clear();
    
    numInputNeurons = 0;
    numHiddenNeurons = 0;
    numOutputNeurons = 0;
    inputLayer.clear();
    hiddenLayer.clear();
    outputLayer.clear();
    initialized = false;
    
    return true;
}

bool MLP::print() const{
    printNetwork();
    return true;
}

bool MLP::trainModel(RegressionData &trainingData){
    
    trained = false;
    
    if( !initialized ){
        errorLog << "train(RegressionData trainingData) - The MLP has not be initialized!" << std::endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(RegressionData trainingData) - The training data is empty!" << std::endl;
        return false;
    }
    
    //Create a validation dataset, if needed
    RegressionData validationData;
    if( useValidationSet ){
        validationData = trainingData.split( 100 - validationSetSize );
    }
    
    const UINT N = trainingData.getNumInputDimensions();
    const UINT T = trainingData.getNumTargetDimensions();
    
    if( N != numInputNeurons ){
        errorLog << "train(RegressionData trainingData) - The number of input dimensions in the training data (" << N << ") does not match that of the MLP (" << numInputNeurons << ")" << std::endl;
        return false;
    }
    if( T != numOutputNeurons ){
        errorLog << "train(RegressionData trainingData) - The number of target dimensions in the training data (" << T << ") does not match that of the MLP (" << numOutputNeurons << ")" << std::endl;
        return false;
    }
    
    //Set the Regressifier input and output dimensions
    numInputDimensions = numInputNeurons;
    numOutputDimensions = numOutputNeurons;
    
    //Scale the training and validation data, if needed
    if( useScaling ){
        //Find the ranges for the input data
        inputVectorRanges = trainingData.getInputRanges();
        
        //Find the ranges for the target data
        targetVectorRanges = trainingData.getTargetRanges();
        
        //Now scale the training data and the validation data if required
        trainingData.scale(inputVectorRanges,targetVectorRanges,MLP_NEURON_MIN_TARGET,MLP_NEURON_MAX_TARGET);
        
        if( useValidationSet ){
            validationData.scale(inputVectorRanges,targetVectorRanges,MLP_NEURON_MIN_TARGET,MLP_NEURON_MAX_TARGET);
        }
    }
    //If scaling is enabled then the training and validation data will be scaled - so turn it off so the we do not need to scale the data again
    //The actual scaling state will be reset at the end of traiing
    bool tempScalingState = useScaling;
    useScaling = false;
    
    //Setup the memory
    trainingErrorLog.clear();
    inputNeuronsOutput.resize(numInputNeurons);
    hiddenNeuronsOutput.resize(numHiddenNeurons);
    outputNeuronsOutput.resize(numOutputNeurons);
    deltaO.resize(numOutputNeurons);
    deltaH.resize(numHiddenNeurons);
    
    //Call the main training function
    switch( trainingMode ){
        case ONLINE_GRADIENT_DESCENT:
        if( classificationModeActive ){
            trained = trainOnlineGradientDescentClassification( trainingData, validationData );
        }else{
            trained = trainOnlineGradientDescentRegression( trainingData, validationData );
        }
        break;
        default:
        useScaling = tempScalingState;
        errorLog << "train(RegressionData trainingData) - Uknown training mode!" << std::endl;
        return false;
        break;
    }
    
    //Reset the scaling state so the prediction data will be scaled if needed
    useScaling = tempScalingState;
    
    return trained;
}

bool MLP::trainOnlineGradientDescentClassification(const RegressionData &trainingData,const RegressionData &validationData){
    
    const UINT M = trainingData.getNumSamples();
    const UINT T = trainingData.getNumTargetDimensions();
    const UINT numTestingExamples = useValidationSet ? validationData.getNumSamples() : M;
    
    //Setup the training loop
    MLP bestNetwork;
    totalSquaredTrainingError = 0;
    rmsTrainingError = 0;
    rmsValidationError = 0;
    trainingError = 0;
    bool keepTraining = true;
    UINT epoch = 0;
    UINT bestIter = 0;
    UINT bestIndex = 0;
    UINT classLabel = 0;
    Float lRate = learningRate;
    Float lMomentum = momentum;
    Float error = 0;
    Float lastError = 0;
    Float accuracy = 0;
    Float trainingSetAccuracy = 0;
    Float trainingSetTotalSquaredError = 0;
    Float bestError = grt_numeric_limits< Float >::max();
    Float bestTSError = grt_numeric_limits< Float >::max();
    Float bestRMSError = grt_numeric_limits< Float >::max();
    Float bestAccuracy = 0;
    Float delta = 0;
    Float backPropError = 0;
    Float bestValue = 0;
    VectorFloat y;
    Vector< UINT > indexList(M);
    Vector< VectorFloat > tempTrainingErrorLog;
    TrainingResult result;
    trainingResults.reserve(M);
    
    //Reset the indexList, this is used to randomize the order of the training examples, if needed
    for(UINT i=0; i<M; i++) indexList[i] = i;
    
    for(UINT iter=0; iter<numRandomTrainingIterations; iter++){
        
        epoch = 0;
        keepTraining = true;
        tempTrainingErrorLog.clear();
        
        //Randomise the start values of the neurons
        init(numInputNeurons,numHiddenNeurons,numOutputNeurons,inputLayerActivationFunction,hiddenLayerActivationFunction,outputLayerActivationFunction);
        
        if( randomiseTrainingOrder ){
            for(UINT i=0; i<M; i++){
                SWAP(indexList[ i ], indexList[ random.getRandomNumberInt(0, M) ]);
            }
        }
        
        while( keepTraining ){
            
            //Perform one training epoch
            accuracy = 0;
            totalSquaredTrainingError = 0;
            
            for(UINT i=0; i<M; i++){
                //Get the i'th training and target Vectors
                const VectorFloat &trainingExample = trainingData[ indexList[i] ].getInputVector();
                const VectorFloat &targetVector = trainingData[ indexList[i] ].getTargetVector();
                
                //Perform the back propagation
                backPropError = back_prop(trainingExample,targetVector,lRate,lMomentum);
                
                //debugLog << "i: " << i << " backPropError: " << backPropError << std::endl;
                
                if( isNAN(backPropError) ){
                    keepTraining = false;
                    errorLog << "train(RegressionData trainingData) - NaN found in back propagation error, training index: " << indexList[i] << std::endl;
                    return false;
                }
                
                //Compute the error for the i'th example
                if( classificationModeActive ){
                    y = feedforward( trainingExample );
                    
                    //Get the class label
                    bestValue = targetVector[0];
                    bestIndex = 0;
                    for(UINT i=1; i<targetVector.size(); i++){
                        if( targetVector[i] > bestValue ){
                            bestValue = targetVector[i];
                            bestIndex = i;
                        }
                    }
                    classLabel = bestIndex + 1;
                    
                    //Get the predicted class label
                    bestValue = y[0];
                    bestIndex = 0;
                    for(UINT i=1; i<numOutputNeurons; i++){
                        if( y[i] > bestValue ){
                            bestValue = y[i];
                            bestIndex = i;
                        }
                    }
                    predictedClassLabel = bestIndex+1;
                    
                    if( classLabel == predictedClassLabel ){
                        accuracy++;
                    }
                    
                }else{
                    totalSquaredTrainingError += backPropError; //The backPropError is already squared
                }
            }
            
            if( checkForNAN() ){
                keepTraining = false;
                errorLog << "train(RegressionData trainingData) - NaN found in weights at " << epoch << std::endl;
                break;
            }
            
            //Compute the error over all the training/validation examples
            if( useValidationSet ){
                trainingSetAccuracy = accuracy;
                trainingSetTotalSquaredError = totalSquaredTrainingError;
                accuracy = 0;
                totalSquaredTrainingError = 0;
                
                //Iterate over the validation samples
                UINT numValidationSamples = validationData.getNumSamples();
                for(UINT i=0; i<numValidationSamples; i++){
                    const VectorFloat &inputVector = validationData[i].getInputVector();
                    const VectorFloat &targetVector = validationData[i].getTargetVector();
                    
                    y = feedforward( inputVector );
                    
                    if( classificationModeActive ){
                        //Get the class label
                        bestValue = targetVector[0];
                        bestIndex = 0;
                        for(UINT i=1; i<numInputNeurons; i++){
                            if( targetVector[i] > bestValue ){
                                bestValue = targetVector[i];
                                bestIndex = i;
                            }
                        }
                        classLabel = bestIndex + 1;
                        
                        //Get the predicted class label
                        bestValue = y[0];
                        bestIndex = 0;
                        for(UINT i=1; i<numOutputNeurons; i++){
                            if( y[i] > bestValue ){
                                bestValue = y[i];
                                bestIndex = i;
                            }
                        }
                        predictedClassLabel = bestIndex+1;
                        
                        if( classLabel == predictedClassLabel ){
                            accuracy++;
                        }
                        
                    }else{
                        //Update the total squared error
                        for(UINT j=0; j<T; j++){
                            totalSquaredTrainingError += SQR( targetVector[j]-y[j] );
                        }
                    }
                }
                
                accuracy = (accuracy/Float(numValidationSamples))*Float(numValidationSamples);
                rmsValidationError = sqrt( totalSquaredTrainingError / Float(numValidationSamples) );
                
            }else{//We are not using a validation set
                accuracy = (accuracy/Float(M))*Float(M);
                rmsTrainingError = sqrt( totalSquaredTrainingError / Float(M) );
            }
            
            //Store the errors
            VectorFloat temp(2);
            temp[0] = 100.0 - trainingSetAccuracy;
            temp[1] = 100.0 - accuracy;
            tempTrainingErrorLog.push_back( temp );
            
            error = 100.0 - accuracy;
            
            //Store the training results
            result.setClassificationResult(iter,accuracy,this);
            trainingResults.push_back( result );
            
            delta = fabs( error - lastError );
            
            trainingLog << "Random Training Iteration: " << iter+1 << " Epoch: " << epoch << " Error: " << error << " Delta: " << delta << std::endl;
            
            //Check to see if we should stop training
            if( ++epoch >= maxNumEpochs ){
                keepTraining = false;
            }
            if( delta <= minChange && epoch >= minNumEpochs ){
                keepTraining = false;
            }
            
            //Update the last error
            lastError = error;
            
            //Notify any observers of the new training result
            trainingResultsObserverManager.notifyObservers( result );
            
        }//End of While( keepTraining )
        
        if( lastError < bestError ){
            bestIter = iter;
            bestError = lastError;
            bestTSError = totalSquaredTrainingError;
            bestRMSError = rmsTrainingError;
            bestAccuracy = accuracy;
            bestNetwork = *this;
            trainingErrorLog = tempTrainingErrorLog;
        }
        
    }//End of For( numRandomTrainingIterations )
    
    trainingLog << "Best Accuracy: " << bestAccuracy << " in Random Training Iteration: " << bestIter+1 << std::endl;
    
    //Check to make sure the best network has not got any NaNs in it
    if( checkForNAN() ){
        errorLog << "train(RegressionData trainingData) - NAN Found!" << std::endl;
        return false;
    }
    
    //Set the MLP model to the model that best during training
    *this = bestNetwork;
    trainingError = bestAccuracy;
    
    //Compute the rejection threshold
    if( useNullRejection ){
        
        Float averageValue = 0;
        VectorFloat classificationPredictions, inputVector, targetVector;
        
        for(UINT i=0; i<numTestingExamples; i++){
            inputVector = useValidationSet ? validationData[i].getInputVector() : trainingData[i].getInputVector();
            targetVector = useValidationSet ? validationData[i].getTargetVector() : trainingData[i].getTargetVector();
            
            //Make the prediction
            y = feedforward( inputVector );
            
            //Get the class label
            bestValue = targetVector[0];
            bestIndex = 0;
            for(UINT i=1; i<targetVector.size(); i++){
                if( targetVector[i] > bestValue ){
                    bestValue = targetVector[i];
                    bestIndex = i;
                }
            }
            classLabel = bestIndex + 1;
            
            //Get the predicted class label
            bestValue = y[0];
            bestIndex = 0;
            for(UINT i=1; i<y.size(); i++){
                if( y[i] > bestValue ){
                    bestValue = y[i];
                    bestIndex = i;
                }
            }
            predictedClassLabel = bestIndex+1;
            
            //Only add the max value if the prediction is correct
            if( classLabel == predictedClassLabel ){
                classificationPredictions.push_back( bestValue );
                averageValue += bestValue;
            }
        }
        
        averageValue /= Float(classificationPredictions.size());
        Float stdDev = 0;
        for(UINT i=0; i<classificationPredictions.size(); i++){
            stdDev += SQR(classificationPredictions[i]-averageValue);
        }
        stdDev = sqrt( stdDev / Float(classificationPredictions.size()-1) );
        
        nullRejectionThreshold = averageValue-(stdDev*nullRejectionCoeff);
    }
    
    //Return true to flag that the model was trained OK
    return true;
}

bool MLP::trainOnlineGradientDescentRegression(const RegressionData &trainingData,const RegressionData &validationData){
    
    const UINT M = trainingData.getNumSamples();
    const UINT T = trainingData.getNumTargetDimensions();
    const UINT numValidationSamples = useValidationSet ? validationData.getNumSamples() : M;
    
    //Setup the training loop
    bool keepTraining = true;
    UINT epoch = 0;
    Float alpha = learningRate;
    Float beta = momentum;
    UINT bestIter = 0;
    MLP bestNetwork;
    totalSquaredTrainingError = 0;
    rmsTrainingError = 0;
    trainingError = 0;
    Float error = 0;
    Float lastError = 0;
    Float trainingSetTotalSquaredError = 0;
    Float bestError = grt_numeric_limits< Float >::max();
    Float bestTSError = grt_numeric_limits< Float >::max();
    Float bestRMSError = grt_numeric_limits< Float >::max();
    Float delta = 0;
    Vector< UINT > indexList(M);
    Vector< VectorFloat > tempTrainingErrorLog;
    TrainingResult result;
    trainingResults.reserve(M);
    
    //Reset the indexList, this is used to randomize the order of the training examples, if needed
    for(UINT i=0; i<M; i++) indexList[i] = i;
    
    for(UINT iter=0; iter<numRandomTrainingIterations; iter++){
        
        epoch = 0;
        keepTraining = true;
        tempTrainingErrorLog.clear();
        
        //Randomise the start values of the neurons
        init(numInputNeurons,numHiddenNeurons,numOutputNeurons,inputLayerActivationFunction,hiddenLayerActivationFunction,outputLayerActivationFunction);
        
        if( randomiseTrainingOrder ){
            for(UINT i=0; i<M; i++){
                SWAP(indexList[ i ], indexList[ random.getRandomNumberInt(0, M) ]);
            }
        }
        
        while( keepTraining ){
            
            //Perform one training epoch
            totalSquaredTrainingError = 0;
            
            for(UINT i=0; i<M; i++){

                //Get the i'th training and target Vectors
                const UINT randomIndex = indexList[i];
                const VectorFloat &trainingExample = trainingData[ randomIndex ].getInputVector();
                const VectorFloat &targetVector = trainingData[ randomIndex ].getTargetVector();
                
                //Perform the back propagation
                Float backPropError = back_prop(trainingExample,targetVector,alpha,beta);
                 
                if( isNAN(backPropError) ){
                    keepTraining = false;
                    errorLog << "train(RegressionData trainingData) - NaN found in back propagation error, epoch: " << epoch << " training iter: " << i << " random index: " << indexList[ randomIndex ] << std::endl;
                    return false;
                }

                //Compute the error for the i'th example
                totalSquaredTrainingError += backPropError; //The backPropError is already squared
            }
            
            if( checkForNAN() ){
                keepTraining = false;
                errorLog << "train(RegressionData trainingData) - NaN found in weights at epoch " << epoch << std::endl;
                return false;
            }
            
            //Compute the error over all the training/validation examples
            if( useValidationSet ){
                trainingSetTotalSquaredError = totalSquaredTrainingError;
                totalSquaredTrainingError = 0;
                
                //Iterate over the validation samples
                for(UINT n=0; n<numValidationSamples; n++){
                    const VectorFloat &trainingExample = validationData[n].getInputVector();
                    const VectorFloat &targetVector = validationData[n].getTargetVector();
                    
                    VectorFloat y = feedforward(trainingExample);
                    
                    //Update the total squared error
                    for(UINT j=0; j<T; j++){
                        totalSquaredTrainingError += SQR( targetVector[j]-y[j] );
                    }
                    
                }
                
                rmsTrainingError = sqrt( totalSquaredTrainingError / Float(numValidationSamples) );
                
            }else{//We are not using a validation set
                rmsTrainingError = sqrt( totalSquaredTrainingError / Float(M) );
            }
            
            //Store the errors
            VectorFloat temp(2);
            temp[0] = trainingSetTotalSquaredError;
            temp[1] = rmsTrainingError;
            tempTrainingErrorLog.push_back( temp );
            
            error = rmsTrainingError;
            
            //Store the training results
            result.setRegressionResult(iter,totalSquaredTrainingError,rmsTrainingError,this);
            trainingResults.push_back( result );
            
            delta = fabs( error - lastError );
            
            trainingLog << "Random Training Iteration: " << iter+1 << " Epoch: " << epoch << " Error: " << error << " Delta: " << delta << std::endl;
            
            //Check to see if we should stop training
            if( ++epoch >= maxNumEpochs ){
                keepTraining = false;
            }
            if( delta <= minChange && epoch >= minNumEpochs ){
                keepTraining = false;
            }
            
            //Update the last error
            lastError = error;
            
            //Notify any observers of the new training result
            trainingResultsObserverManager.notifyObservers( result );
            
        }//End of While( keepTraining )
        
        //Check to see if this is the best model so far
        if( lastError < bestError ){
            bestIter = iter;
            bestError = lastError;
            bestTSError = totalSquaredTrainingError;
            bestRMSError = rmsTrainingError;
            bestNetwork = *this;
            trainingErrorLog = tempTrainingErrorLog;
        }
        
    }//End of For( numRandomTrainingIterations )
    
    trainingLog << "Best RMSError: " << bestRMSError << " in Random Training Iteration: " << bestIter+1 << std::endl;
    
    //Check to make sure the best network has not got any NaNs in it
    if( checkForNAN() ){
        errorLog << "train(RegressionData trainingData) - NAN Found!" << std::endl;
        return false;
    }
    
    //Set the MLP model to the model that best during training
    *this = bestNetwork;
    trainingError = bestRMSError;
    
    //Return true to flag that the model was trained OK
    return true;
}

Float MLP::back_prop(const VectorFloat &trainingExample,const VectorFloat &targetVector,const Float learningRate,const Float learningMomentum){
    
    UINT i,j,k = 0;
    Float update = 0;
    Float error = 0;
    Float sqrError = 0;
    
    //Forward propagation based on the current weights
    feedforward(trainingExample,inputNeuronsOutput,hiddenNeuronsOutput,outputNeuronsOutput);
 
    //Compute the error of the output layer: the derivative of the output neuron, times the error of the output
    for(k=0; k<numOutputNeurons; k++){
        error = targetVector[k]-outputNeuronsOutput[k];
        sqrError += 0.5 * SQR( error );
        deltaO[k] = outputLayer[k].getDerivative( outputNeuronsOutput[k] ) * error;
    }
    
    //Compute the error of the hidden layer: the derivative of the hidden neuron, times the total error of the hidden output 
    for(j=0; j<numHiddenNeurons; j++){
        error = 0;
        for(k=0; k<numOutputNeurons; k++){
            error += outputLayer[k].weights[j] * deltaO[k];
        }
        deltaH[j] = hiddenLayer[j].getDerivative( hiddenNeuronsOutput[j] ) * error;
    }

    //Update the output weights, new weight = old weight + (learningRate * change) + (momenutum * previousChange)
    for(j=0; j<numHiddenNeurons; j++){
        for(k=0; k<numOutputNeurons; k++){
            update = deltaO[k] * hiddenNeuronsOutput[j];
            outputLayer[k].weights[j] += learningRate*update + learningMomentum*outputLayer[k].previousUpdate[j];
            outputLayer[k].previousUpdate[j] = update;
        }
    }

    //Update the hidden weights, new weight = old weight + (learningRate * change) + (momenutum * previousChange)
    for(i=0; i<numInputNeurons; i++){
        for(j=0; j<numHiddenNeurons; j++){
            update = deltaH[j] * inputNeuronsOutput[i];
            hiddenLayer[j].weights[i] += learningRate*update + learningMomentum*hiddenLayer[j].previousUpdate[i];
            hiddenLayer[j].previousUpdate[i] = update;
        }
    }
    
    //Update the output bias, new weight = old weight + (learningRate * change) + (momenutum * previousChange)
    for(k=0; k<numOutputNeurons; k++){
        //Compute the update
        update = learningRate*deltaO[k] + learningMomentum*outputLayer[k].previousBiasUpdate;
        
        //Update the bias
        outputLayer[k].bias += update;
        
        //Store the update
        outputLayer[k].previousBiasUpdate = update;
    }

    //Update the hidden bias, new weight = old weight + (learningRate * change) + (momenutum * previousChange)
    for(j=0; j<numHiddenNeurons; j++){
        //Compute the update
        update = learningRate*deltaH[j] + learningMomentum*hiddenLayer[j].previousBiasUpdate;
        
        //Update the bias
        hiddenLayer[j].bias += update;
        
        //Store the update
        hiddenLayer[j].previousBiasUpdate = update;
    }
    
    //Return the squared error between the output of the network and the target Vector
    return sqrError;
}

VectorFloat MLP::feedforward(VectorFloat trainingExample){
    
    if( inputNeuronsOutput.size() != numInputNeurons ) inputNeuronsOutput.resize(numInputNeurons,0);
    if( hiddenNeuronsOutput.size() != numHiddenNeurons ) hiddenNeuronsOutput.resize(numHiddenNeurons,0);
    if( outputNeuronsOutput.size() != numOutputNeurons ) outputNeuronsOutput.resize(numOutputNeurons,0);

    UINT i,j,k=0;
    
    //Scale the input vector if required
    if( useScaling ){
        for(i=0; i<numInputNeurons; i++){
            trainingExample[i] = scale(trainingExample[i],inputVectorRanges[i].minValue,inputVectorRanges[i].maxValue,MLP_NEURON_MIN_TARGET,MLP_NEURON_MAX_TARGET);
        }
    }
    
    //Input layer
    VectorFloat input(1);
    for(i=0; i<numInputNeurons; i++){
        input[0] = trainingExample[i];
        inputNeuronsOutput[i] = inputLayer[i].fire( input );
    }
    
    //Hidden Layer
    for(j=0; j<numHiddenNeurons; j++){
        hiddenNeuronsOutput[j] = hiddenLayer[j].fire( inputNeuronsOutput );
    }
    
    //Output Layer
    for(k=0; k<numOutputNeurons; k++){
        outputNeuronsOutput[k] = outputLayer[k].fire( hiddenNeuronsOutput );
    }
    
    //Scale the output vector if required
    if( useScaling ){
        for(k=0; k<numOutputNeurons; k++){
            outputNeuronsOutput[k] = scale(outputNeuronsOutput[k],MLP_NEURON_MIN_TARGET,MLP_NEURON_MAX_TARGET,targetVectorRanges[k].minValue,targetVectorRanges[k].maxValue);
        }
    }
    
    return outputNeuronsOutput;
}

void MLP::feedforward(const VectorFloat &data,VectorFloat &inputNeuronsOutput,VectorFloat &hiddenNeuronsOutput,VectorFloat &outputNeuronsOutput){
    
    if( inputNeuronsOutput.size() != numInputNeurons ) inputNeuronsOutput.resize(numInputNeurons,0);
    if( hiddenNeuronsOutput.size() != numHiddenNeurons ) hiddenNeuronsOutput.resize(numHiddenNeurons,0);
    if( outputNeuronsOutput.size() != numOutputNeurons ) outputNeuronsOutput.resize(numOutputNeurons,0);

    UINT i,j,k=0;
    
    //Input layer
    VectorFloat input(1);
    for(i=0; i<numInputNeurons; i++){
        input[0] = data[i];
        inputNeuronsOutput[i] = inputLayer[i].fire( input );
    }
    
    //Hidden Layer
    for(j=0; j<numHiddenNeurons; j++){
        hiddenNeuronsOutput[j] = hiddenLayer[j].fire( inputNeuronsOutput );
    }
    
    //Output Layer
    for(k=0; k<numOutputNeurons; k++){
        outputNeuronsOutput[k] = outputLayer[k].fire( hiddenNeuronsOutput );
    }
    
}

void MLP::printNetwork() const{
    std::cout<<"***************** MLP *****************\n";
    std::cout<<"NumInputNeurons: "<<numInputNeurons<< std::endl;
    std::cout<<"NumHiddenNeurons: "<<numHiddenNeurons<< std::endl;
    std::cout<<"NumOutputNeurons: "<<numOutputNeurons<< std::endl;

    std::cout << "ScalingEnabled: " << useScaling << std::endl;

    if( useScaling ){
        std::cout << "InputRanges: " << std::endl;
        for(UINT i=0; i<numInputNeurons; i++){
            std::cout << "Input: " << i << "\t" << inputVectorRanges[i].minValue << "\t" << inputVectorRanges[i].maxValue << std::endl;
        }

        std::cout << "OutputRanges: " << std::endl;
        for(UINT i=0; i<numOutputNeurons; i++){
            std::cout << "Output: " << i << "\t" << targetVectorRanges[i].minValue << "\t" << targetVectorRanges[i].maxValue << std::endl;
        }
    }
    
    std::cout<<"InputWeights:\n";
    for(UINT i=0; i<numInputNeurons; i++){
        std::cout<<"Neuron: "<<i<<" Bias: " << inputLayer[i].bias << " Weights: ";
        for(UINT j=0; j<inputLayer[i].weights.getSize(); j++){
            std::cout << inputLayer[i].weights[j] << "\t";
        } std::cout << std::endl;
    }
    
    std::cout<<"HiddenWeights:\n";
    for(UINT i=0; i<numHiddenNeurons; i++){
        std::cout<<"Neuron: "<<i<<" Bias: " << hiddenLayer[i].bias << " Weights: ";
        for(UINT j=0; j<hiddenLayer[i].weights.getSize(); j++){
            std::cout << hiddenLayer[i].weights[j] << "\t";
        } std::cout << std::endl;
    }
    
    std::cout<<"OutputWeights:\n";
    for(UINT i=0; i<numOutputNeurons; i++){
        std::cout<<"Neuron: "<<i<<" Bias: " << outputLayer[i].bias << " Weights: ";
        for(UINT j=0; j<outputLayer[i].weights.getSize(); j++){
            std::cout << outputLayer[i].weights[j] << "\t";
        } std::cout << std::endl;
    }
    
}

bool MLP::checkForNAN() const{
    
    UINT N = 0;
    for(UINT i=0; i<numInputNeurons; i++){
        if( isNAN(inputLayer[i].bias) ) return true;
        N = inputLayer[i].weights.getSize();
        for(UINT j=0; j<N; j++){
            if( isNAN(inputLayer[i].weights[j]) ) return true;
        }
    }
    
    for(UINT i=0; i<numHiddenNeurons; i++){
        if( isNAN(hiddenLayer[i].bias) ) return true;
        N = hiddenLayer[i].weights.getSize();
        for(UINT j=0; j<N; j++){
            if( isNAN(hiddenLayer[i].weights[j]) ) return true;
        }
    }
    
    for(UINT i=0; i<numOutputNeurons; i++){
        if( isNAN(outputLayer[i].bias) ) return true;
        N = outputLayer[i].weights.getSize();
        for(UINT j=0; j<N; j++){
            if( isNAN(outputLayer[i].weights[j]) ) return true;
        }
    }
    
    return false;
}

bool inline MLP::isNAN(const Float v) const{
    if( v != v ) return true;
    return false;
}

bool MLP::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - File is not open!" << std::endl;
        return false;
    }
    
    file << "GRT_MLP_FILE_V2.0\n";
    
    //Write the regressifier settings to the file
    if( !Regressifier::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save Regressifier base settings to file!" << std::endl;
        return false;
    }
    
    file << "NumInputNeurons: "<<numInputNeurons<< std::endl;
    file << "NumHiddenNeurons: "<<numHiddenNeurons<< std::endl;
    file << "NumOutputNeurons: "<<numOutputNeurons<< std::endl;
    file << "InputLayerActivationFunction: " <<activationFunctionToString(inputLayerActivationFunction)<< std::endl;
    file << "HiddenLayerActivationFunction: " <<activationFunctionToString(hiddenLayerActivationFunction)<< std::endl;
    file << "OutputLayerActivationFunction: " <<activationFunctionToString(outputLayerActivationFunction)<< std::endl;
    file << "NumRandomTrainingIterations: " << numRandomTrainingIterations << std::endl;
    file << "Momentum: " << momentum << std::endl;
    file << "Gamma: " << gamma << std::endl;
    file << "ClassificationMode: " << classificationModeActive << std::endl;
    file << "UseNullRejection: " << useNullRejection << std::endl;
    file << "RejectionThreshold: " << nullRejectionThreshold << std::endl;
    
    if( trained ){
        file << "InputLayer: \n";
        for(UINT i=0; i<numInputNeurons; i++){
            file << "InputNeuron: " << i+1 << std::endl;
            file << "NumInputs: " << inputLayer[i].numInputs << std::endl;
            file << "Bias: " << inputLayer[i].bias << std::endl;
            file << "Gamma: " << inputLayer[i].gamma << std::endl;
            file << "Weights: " << std::endl;
            for(UINT j=0; j<inputLayer[i].numInputs; j++){
                file << inputLayer[i].weights[j] << "\t";
            }
            file << std::endl;
        }
        file << "\n";
        
        file << "HiddenLayer: \n";
        for(UINT i=0; i<numHiddenNeurons; i++){
            file << "HiddenNeuron: " << i+1 << std::endl;
            file << "NumInputs: " << hiddenLayer[i].numInputs << std::endl;
            file << "Bias: " << hiddenLayer[i].bias << std::endl;
            file << "Gamma: " << hiddenLayer[i].gamma << std::endl;
            file << "Weights: " << std::endl;
            for(UINT j=0; j<hiddenLayer[i].numInputs; j++){
                file << hiddenLayer[i].weights[j] << "\t";
            }
            file << std::endl;
        }
        file << "\n";
        
        file << "OutputLayer: \n";
        for(UINT i=0; i<numOutputNeurons; i++){
            file << "OutputNeuron: " << i+1 << std::endl;
            file << "NumInputs: " << outputLayer[i].numInputs << std::endl;
            file << "Bias: " << outputLayer[i].bias << std::endl;
            file << "Gamma: " << outputLayer[i].gamma << std::endl;
            file << "Weights: " << std::endl;
            for(UINT j=0; j<outputLayer[i].numInputs; j++){
                file << outputLayer[i].weights[j] << "\t";
            }
            file << std::endl;
        }
    }
    
    return true;
}

bool MLP::load( std::fstream &file ){
    
    std::string activationFunction;
    
    //Clear any previous models
    clear();
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - File is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    file >> word;
    
    //See if we should load a legacy file
    if( word == "GRT_MLP_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Check to make sure this is a file with the MLP File Format
    if( word != "GRT_MLP_FILE_V2.0" ){
        file.close();
        errorLog << "load(fstream &file) - Failed to find file header!" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Regressifier::loadBaseSettingsFromFile( file ) ){
        file.close();
        errorLog << "load(fstream &file) - Failed to load regressifier base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "NumInputNeurons:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find NumInputNeurons!" << std::endl;
        return false;
    }
    file >> numInputNeurons;
    numInputDimensions = numInputNeurons;
    
    file >> word;
    if(word != "NumHiddenNeurons:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find NumHiddenNeurons!" << std::endl;
        return false;
    }
    file >> numHiddenNeurons;
    
    file >> word;
    if(word != "NumOutputNeurons:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find NumOutputNeurons!" << std::endl;
        return false;
    }
    file >> numOutputNeurons;
    
    file >> word;
    if(word != "InputLayerActivationFunction:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find InputLayerActivationFunction!" << std::endl;
        return false;
    }
    file >> activationFunction;
    inputLayerActivationFunction = activationFunctionFromString(activationFunction);
    
    file >> word;
    if(word != "HiddenLayerActivationFunction:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find HiddenLayerActivationFunction!" << std::endl;
        return false;
    }
    file >> activationFunction;
    hiddenLayerActivationFunction = activationFunctionFromString(activationFunction);
    
    file >> word;
    if(word != "OutputLayerActivationFunction:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find OutputLayerActivationFunction!" << std::endl;
        return false;
    }
    file >> activationFunction;
    outputLayerActivationFunction = activationFunctionFromString(activationFunction);
    
    file >> word;
    if(word != "NumRandomTrainingIterations:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find NumRandomTrainingIterations!" << std::endl;
        return false;
    }
    file >> numRandomTrainingIterations;
    
    file >> word;
    if(word != "Momentum:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find Momentum!" << std::endl;
        return false;
    }
    file >> momentum;
    
    file >> word;
    if(word != "Gamma:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find Gamma!" << std::endl;
        return false;
    }
    file >> gamma;
    
    file >> word;
    if(word != "ClassificationMode:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find ClassificationMode!" << std::endl;
        return false;
    }
    file >> classificationModeActive;
    
    file >> word;
    if(word != "UseNullRejection:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find UseNullRejection!" << std::endl;
        return false;
    }
    file >> useNullRejection;
    
    file >> word;
    if(word != "RejectionThreshold:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find RejectionThreshold!" << std::endl;
        return false;
    }
    file >> nullRejectionThreshold;
    
    if( trained ) initialized = true;
    else init(numInputNeurons,numHiddenNeurons,numOutputNeurons);
        
    if( trained ){
        
        //Resize the layers
        inputLayer.resize( numInputNeurons );
        hiddenLayer.resize( numHiddenNeurons );
        outputLayer.resize( numOutputNeurons );
        
        //Load the neuron data
        file >> word;
        if(word != "InputLayer:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find InputLayer!" << std::endl;
            return false;
        }
        
        for(UINT i=0; i<numInputNeurons; i++){
            UINT tempNeuronID = 0;
            
            file >> word;
            if(word != "InputNeuron:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find InputNeuron!" << std::endl;
                return false;
            }
            file >> tempNeuronID;
            
            if( tempNeuronID != i+1 ){
                file.close();
                errorLog << "load(fstream &file) - InputNeuron ID does not match!" << std::endl;
                return false;
            }
            
            file >> word;
            if(word != "NumInputs:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find NumInputs!" << std::endl;
                return false;
            }
            file >> inputLayer[i].numInputs;
            
            //Resize the buffers
            inputLayer[i].weights.resize( inputLayer[i].numInputs );
            
            file >> word;
            if(word != "Bias:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find Bias!" << std::endl;
                return false;
            }
            file >> inputLayer[i].bias;
            
            file >> word;
            if(word != "Gamma:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find Gamma!" << std::endl;
                return false;
            }
            file >> inputLayer[i].gamma;
            
            file >> word;
            if(word != "Weights:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find Weights!" << std::endl;
                return false;
            }
            
            for(UINT j=0; j<inputLayer[i].numInputs; j++){
                file >> inputLayer[i].weights[j];
            }
        }
        
        //Load the Hidden Layer
        file >> word;
        if(word != "HiddenLayer:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find HiddenLayer!" << std::endl;
            return false;
        }
        
        for(UINT i=0; i<numHiddenNeurons; i++){
            UINT tempNeuronID = 0;
            
            file >> word;
            if(word != "HiddenNeuron:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find HiddenNeuron!" << std::endl;
                return false;
            }
            file >> tempNeuronID;
            
            if( tempNeuronID != i+1 ){
                file.close();
                errorLog << "load(fstream &file) - Failed to find HiddenNeuron ID does not match!" << std::endl;
                return false;
            }
            
            file >> word;
            if(word != "NumInputs:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find NumInputs!" << std::endl;
                return false;
            }
            file >> hiddenLayer[i].numInputs;
            
            //Resize the buffers
            hiddenLayer[i].weights.resize( hiddenLayer[i].numInputs );
            
            file >> word;
            if(word != "Bias:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find Bias!" << std::endl;
                return false;
            }
            file >> hiddenLayer[i].bias;
            
            file >> word;
            if(word != "Gamma:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find Gamma!" << std::endl;
                return false;
            }
            file >> hiddenLayer[i].gamma;
            
            file >> word;
            if(word != "Weights:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find Weights!" << std::endl;
                return false;
            }
            
            for(unsigned int j=0; j<hiddenLayer[i].numInputs; j++){
                file >> hiddenLayer[i].weights[j];
            }
        }
        
        //Load the Output Layer
        file >> word;
        if(word != "OutputLayer:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find OutputLayer!" << std::endl;
            return false;
        }
        
        for(UINT i=0; i<numOutputNeurons; i++){
            UINT tempNeuronID = 0;
            
            file >> word;
            if(word != "OutputNeuron:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find OutputNeuron!" << std::endl;
                return false;
            }
            file >> tempNeuronID;
            
            if( tempNeuronID != i+1 ){
                file.close();
                errorLog << "load(fstream &file) - Failed to find OuputNeuron ID does not match!!" << std::endl;
                return false;
            }
            
            file >> word;
            if(word != "NumInputs:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find NumInputs!" << std::endl;
                return false;
            }
            file >> outputLayer[i].numInputs;
            
            //Resize the buffers
            outputLayer[i].weights.resize( outputLayer[i].numInputs );
            
            file >> word;
            if(word != "Bias:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find Bias!" << std::endl;
                return false;
            }
            file >> outputLayer[i].bias;
            
            file >> word;
            if(word != "Gamma:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find Gamma!" << std::endl;
                return false;
            }
            file >> outputLayer[i].gamma;
            
            file >> word;
            if(word != "Weights:"){
                file.close();
                errorLog << "load(fstream &file) - Failed to find Weights!" << std::endl;
                return false;
            }
            
            for(UINT j=0; j<outputLayer[i].numInputs; j++){
                file >> outputLayer[i].weights[j];
            }
        }
        
    }
    
    return true;
}

UINT MLP::getNumClasses() const{
    if( classificationModeActive )
    return numOutputNeurons;
    return 0;
}

UINT MLP::getNumInputNeurons() const{
    return numInputNeurons;
}

UINT MLP::getNumHiddenNeurons() const{
    return numHiddenNeurons;
}

UINT MLP::getNumOutputNeurons() const{
    return numOutputNeurons;
}

Neuron::Type MLP::getInputLayerActivationFunction() const{
    return inputLayerActivationFunction;
}

Neuron::Type MLP::getHiddenLayerActivationFunction() const{
    return hiddenLayerActivationFunction;
}

Neuron::Type MLP::getOutputLayerActivationFunction() const{
    return outputLayerActivationFunction;
}

UINT MLP::getNumRandomTrainingIterations() const{
    return numRandomTrainingIterations;
}

Float MLP::getTrainingRate() const{
    return learningRate;
}

Float MLP::getMomentum() const{
    return momentum;
}

Float MLP::getGamma() const{
    return gamma;
}

Float MLP::getTrainingError() const{
    return trainingError;
}

bool MLP::getClassificationModeActive() const{
    return classificationModeActive;
}

bool MLP::getRegressionModeActive() const{
    return !classificationModeActive;
}

Vector< Neuron > MLP::getInputLayer() const{
    return inputLayer;
}

Vector< Neuron > MLP::getHiddenLayer() const{
    return hiddenLayer;
}

Vector< Neuron > MLP::getOutputLayer() const{
    return outputLayer;
}

Vector< VectorFloat > MLP::getTrainingLog() const{
    return trainingErrorLog;
}

bool MLP::getNullRejectionEnabled() const{
    return useNullRejection;
}

Float MLP::getNullRejectionCoeff() const{
    return nullRejectionCoeff;
}

Float MLP::getNullRejectionThreshold() const{
    return nullRejectionThreshold;
}

Float MLP::getMaximumLikelihood() const{
    if( trained ) return maxLikelihood;
    return DEFAULT_NULL_LIKELIHOOD_VALUE;
}

VectorFloat MLP::getClassLikelihoods() const{
    if( trained && classificationModeActive ) return classLikelihoods;
    return VectorFloat();
}

VectorFloat MLP::getClassDistances() const{
    //The class distances is simply the regression data
    if( trained && classificationModeActive ) return regressionData;
    return VectorFloat();
}

UINT MLP::getPredictedClassLabel() const{
    if( trained && classificationModeActive ) return predictedClassLabel;
    return 0;
}

std::string MLP::activationFunctionToString(const Neuron::Type activationFunction) const{
    std::string activationName;
    
    switch(activationFunction){
        case(Neuron::LINEAR):
        activationName = "LINEAR";
        break;
        case(Neuron::SIGMOID):
        activationName = "SIGMOID";
        break;
        case(Neuron::BIPOLAR_SIGMOID):
        activationName = "BIPOLAR_SIGMOID";
        break;
        case(Neuron::TANH):
        activationName = "TANH";
        break;
        default:
        activationName = "UNKNOWN";
        break;
    }
    
    return activationName;
}

Neuron::Type MLP::activationFunctionFromString(const std::string activationName) const{
    Neuron::Type activationFunction = Neuron::LINEAR;
    
    if(activationName == "LINEAR" ){
        activationFunction = Neuron::LINEAR;
        return activationFunction;
    }
    if(activationName == "SIGMOID" ){
        activationFunction = Neuron::SIGMOID;
        return activationFunction;
    }
    if(activationName == "BIPOLAR_SIGMOID" ){
        activationFunction = Neuron::BIPOLAR_SIGMOID;
        return activationFunction;
    }
    if(activationName == "TANH" ){
        activationFunction = Neuron::TANH;
        return activationFunction;
    }
    return activationFunction;
}

bool MLP::validateActivationFunction(const Neuron::Type actvationFunction) const{
    if( actvationFunction >= Neuron::LINEAR && actvationFunction < Neuron::NUMBER_OF_ACTIVATION_FUNCTIONS ) return true;
    return false;
}

bool MLP::setInputLayerActivationFunction(const Neuron::Type activationFunction){
    
    if( !validateActivationFunction(activationFunction) ){
        warningLog << "setInputLayerActivationFunction(const Neuron::Type activationFunction) - The activation function is not valid. It should be one of the Neuron ActivationFunctions enums." << std::endl;
    }
    
    this->inputLayerActivationFunction = activationFunction;
    
    if( initialized ){
        return init(numInputNeurons,numHiddenNeurons,numOutputNeurons);
    }
    
    return true;
}


bool MLP::setHiddenLayerActivationFunction(const Neuron::Type activationFunction){
    
    if( !validateActivationFunction(activationFunction) ){
        warningLog << "setHiddenLayerActivationFunction(const Neuron::Type activationFunction) - The activation function is not valid. It should be one of the Neuron ActivationFunctions enums." << std::endl;
    }
    
    this->hiddenLayerActivationFunction = activationFunction;
    
    if( initialized ){
        return init(numInputNeurons,numHiddenNeurons,numOutputNeurons);
    }
    
    return true;
}


bool MLP::setOutputLayerActivationFunction(const Neuron::Type activationFunction){
    
    if( !validateActivationFunction(activationFunction) ){
        warningLog << "setOutputLayerActivationFunction(const Neuron::Type activationFunction) - The activation function is not valid. It should be one of the Neuron ActivationFunctions enums." << std::endl;
    }
    
    this->outputLayerActivationFunction = activationFunction;
    
    if( initialized ){
        return init(numInputNeurons,numHiddenNeurons,numOutputNeurons);
    }
    
    return true;
}

bool MLP::setTrainingRate(const Float trainingRate){
    return setLearningRate( trainingRate );
}

bool MLP::setMomentum(const Float momentum){
    if( momentum >= 0 && momentum <= 1.0 ){
        this->momentum = momentum;
        return true;
    }
    return false;
}

bool MLP::setGamma(const Float gamma){
    
    if( gamma < 0 ){
        warningLog << "setGamma(const Float gamma) - Gamma must be greater than zero!" << std::endl;
    }
    
    this->gamma = gamma;
    
    if( initialized ){
        return init(numInputNeurons,numHiddenNeurons,numOutputNeurons);
    }
    
    return true;
}

bool MLP::setNumRandomTrainingIterations(const UINT numRandomTrainingIterations){
    if( numRandomTrainingIterations > 0 ){
        this->numRandomTrainingIterations = numRandomTrainingIterations;
        return true;
    }
    return false;
}

bool MLP::setNullRejection(const bool useNullRejection){
    this->useNullRejection = useNullRejection;
    return true;
}

bool MLP::setNullRejectionCoeff(const Float nullRejectionCoeff){
    if( nullRejectionCoeff > 0 ){
        this->nullRejectionCoeff = nullRejectionCoeff;
        return true;
    }
    return false;
}

bool MLP::loadLegacyModelFromFile( std::fstream &file ){
    
    std::string word;
    
    file >> word;
    if(word != "NumInputNeurons:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find NumInputNeurons!" << std::endl;
        return false;
    }
    file >> numInputNeurons;
    numInputDimensions = numInputNeurons;
    
    file >> word;
    if(word != "NumHiddenNeurons:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find NumHiddenNeurons!" << std::endl;
        return false;
    }
    file >> numHiddenNeurons;
    
    file >> word;
    if(word != "NumOutputNeurons:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find NumOutputNeurons!" << std::endl;
        return false;
    }
    file >> numOutputNeurons;
    
    file >> word;
    if(word != "InputLayerActivationFunction:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find InputLayerActivationFunction!" << std::endl;
        return false;
    }
    file >> word;
    inputLayerActivationFunction = activationFunctionFromString(word);
    
    file >> word;
    if(word != "HiddenLayerActivationFunction:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find HiddenLayerActivationFunction!" << std::endl;
        return false;
    }
    file >> word;
    hiddenLayerActivationFunction = activationFunctionFromString(word);
    
    file >> word;
    if(word != "OutputLayerActivationFunction:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find OutputLayerActivationFunction!" << std::endl;
        return false;
    }
    file >> word;
    outputLayerActivationFunction = activationFunctionFromString(word);
    
    file >> word;
    if(word != "MinNumEpochs:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find MinNumEpochs!" << std::endl;
        return false;
    }
    file >> minNumEpochs;
    
    file >> word;
    if(word != "MaxNumEpochs:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find MaxNumEpochs!" << std::endl;
        return false;
    }
    file >> maxNumEpochs;
    
    file >> word;
    if(word != "NumRandomTrainingIterations:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find NumRandomTrainingIterations!" << std::endl;
        return false;
    }
    file >> numRandomTrainingIterations;
    
    file >> word;
    if(word != "ValidationSetSize:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find ValidationSetSize!" << std::endl;
        return false;
    }
    file >> validationSetSize;
    
    file >> word;
    if(word != "MinChange:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find MinChange!" << std::endl;
        return false;
    }
    file >> minChange;
    
    file >> word;
    if(word != "TrainingRate:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find TrainingRate!" << std::endl;
        return false;
    }
    file >> learningRate;
    
    file >> word;
    if(word != "Momentum:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find Momentum!" << std::endl;
        return false;
    }
    file >> momentum;
    
    file >> word;
    if(word != "Gamma:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find Gamma!" << std::endl;
        return false;
    }
    file >> gamma;
    
    file >> word;
    if(word != "UseValidationSet:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find UseValidationSet!" << std::endl;
        return false;
    }
    file >> useValidationSet;
    
    file >> word;
    if(word != "RandomiseTrainingOrder:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find RandomiseTrainingOrder!" << std::endl;
        return false;
    }
    file >> randomiseTrainingOrder;
    
    file >> word;
    if(word != "UseScaling:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find UseScaling!" << std::endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "ClassificationMode:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find ClassificationMode!" << std::endl;
        return false;
    }
    file >> classificationModeActive;
    
    file >> word;
    if(word != "UseNullRejection:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find UseNullRejection!" << std::endl;
        return false;
    }
    file >> useNullRejection;
    
    file >> word;
    if(word != "RejectionThreshold:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find RejectionThreshold!" << std::endl;
        return false;
    }
    file >> nullRejectionThreshold;
    
    //Resize the layers
    inputLayer.resize( numInputNeurons );
    hiddenLayer.resize( numHiddenNeurons );
    outputLayer.resize( numOutputNeurons );
    
    //Load the neuron data
    file >> word;
    if(word != "InputLayer:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find InputLayer!" << std::endl;
        return false;
    }
    
    for(UINT i=0; i<numInputNeurons; i++){
        UINT tempNeuronID = 0;
        
        file >> word;
        if(word != "InputNeuron:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find InputNeuron!" << std::endl;
            return false;
        }
        file >> tempNeuronID;
        
        if( tempNeuronID != i+1 ){
            file.close();
            errorLog << "load(fstream &file) - InputNeuron ID does not match!" << std::endl;
            return false;
        }
        
        file >> word;
        if(word != "NumInputs:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find NumInputs!" << std::endl;
            return false;
        }
        file >> inputLayer[i].numInputs;
        
        //Resize the buffers
        inputLayer[i].weights.resize( inputLayer[i].numInputs );
        
        file >> word;
        if(word != "Bias:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find Bias!" << std::endl;
            return false;
        }
        file >> inputLayer[i].bias;
        
        file >> word;
        if(word != "Gamma:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find Gamma!" << std::endl;
            return false;
        }
        file >> inputLayer[i].gamma;
        
        file >> word;
        if(word != "Weights:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find Weights!" << std::endl;
            return false;
        }
        
        for(UINT j=0; j<inputLayer[i].numInputs; j++){
            file >> inputLayer[i].weights[j];
        }
    }
    
    //Load the Hidden Layer
    file >> word;
    if(word != "HiddenLayer:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find HiddenLayer!" << std::endl;
        return false;
    }
    
    for(UINT i=0; i<numHiddenNeurons; i++){
        UINT tempNeuronID = 0;
        
        file >> word;
        if(word != "HiddenNeuron:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find HiddenNeuron!" << std::endl;
            return false;
        }
        file >> tempNeuronID;
        
        if( tempNeuronID != i+1 ){
            file.close();
            errorLog << "load(fstream &file) - Failed to find HiddenNeuron ID does not match!" << std::endl;
            return false;
        }
        
        file >> word;
        if(word != "NumInputs:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find NumInputs!" << std::endl;
            return false;
        }
        file >> hiddenLayer[i].numInputs;
        
        //Resize the buffers
        hiddenLayer[i].weights.resize( hiddenLayer[i].numInputs );
        
        file >> word;
        if(word != "Bias:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find Bias!" << std::endl;
            return false;
        }
        file >> hiddenLayer[i].bias;
        
        file >> word;
        if(word != "Gamma:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find Gamma!" << std::endl;
            return false;
        }
        file >> hiddenLayer[i].gamma;
        
        file >> word;
        if(word != "Weights:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find Weights!" << std::endl;
            return false;
        }
        
        for(unsigned int j=0; j<hiddenLayer[i].numInputs; j++){
            file >> hiddenLayer[i].weights[j];
        }
    }
    
    //Load the Output Layer
    file >> word;
    if(word != "OutputLayer:"){
        file.close();
        errorLog << "load(fstream &file) - Failed to find OutputLayer!" << std::endl;
        return false;
    }
    
    for(UINT i=0; i<numOutputNeurons; i++){
        UINT tempNeuronID = 0;
        
        file >> word;
        if(word != "OutputNeuron:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find OutputNeuron!" << std::endl;
            return false;
        }
        file >> tempNeuronID;
        
        if( tempNeuronID != i+1 ){
            file.close();
            errorLog << "load(fstream &file) - Failed to find OuputNeuron ID does not match!!" << std::endl;
            return false;
        }
        
        file >> word;
        if(word != "NumInputs:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find NumInputs!" << std::endl;
            return false;
        }
        file >> outputLayer[i].numInputs;
        
        //Resize the buffers
        outputLayer[i].weights.resize( outputLayer[i].numInputs );
        
        file >> word;
        if(word != "Bias:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find Bias!" << std::endl;
            return false;
        }
        file >> outputLayer[i].bias;
        
        file >> word;
        if(word != "Gamma:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find Gamma!" << std::endl;
            return false;
        }
        file >> outputLayer[i].gamma;
        
        file >> word;
        if(word != "Weights:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find Weights!" << std::endl;
            return false;
        }
        
        for(UINT j=0; j<outputLayer[i].numInputs; j++){
            file >> outputLayer[i].weights[j];
        }
    }
    
    if( useScaling ){
        //Resize the ranges buffers
        inputVectorRanges.resize( numInputNeurons );
        targetVectorRanges.resize( numOutputNeurons );
        
        //Load the ranges
        file >> word;
        if(word != "InputVectorRanges:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find InputVectorRanges!" << std::endl;
            return false;
        }
        for(UINT j=0; j<inputVectorRanges.size(); j++){
            file >> inputVectorRanges[j].minValue;
            file >> inputVectorRanges[j].maxValue;
        }
        
        file >> word;
        if(word != "OutputVectorRanges:"){
            file.close();
            errorLog << "load(fstream &file) - Failed to find OutputVectorRanges!" << std::endl;
            return false;
        }
        for(UINT j=0; j<targetVectorRanges.size(); j++){
            file >> targetVectorRanges[j].minValue;
            file >> targetVectorRanges[j].maxValue;
        }
    }
    
    initialized = true;
    trained = true;
    
    return true;
}

GRT_END_NAMESPACE
