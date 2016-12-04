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
#include "Softmax.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string Softmax::id = "Softmax";
std::string Softmax::getId() { return Softmax::id; }

//Register the Softmax module with the Classifier base class
RegisterClassifierModule< Softmax >  Softmax::registerModule( Softmax::getId() );

Softmax::Softmax(const bool useScaling,const Float learningRate,const Float minChange,const UINT maxNumEpochs,const UINT batchSize) : Classifier( Softmax::getId() )
{
    this->useScaling = useScaling;
    this->learningRate = learningRate;
    this->minChange = minChange;
    this->maxNumEpochs = maxNumEpochs;
    this->batchSize = batchSize;
    classifierMode = STANDARD_CLASSIFIER_MODE;
}

Softmax::Softmax(const Softmax &rhs) : Classifier( Softmax::getId() )
{
    classifierMode = STANDARD_CLASSIFIER_MODE;
    *this = rhs;
}

Softmax::~Softmax(void)
{
}

Softmax& Softmax::operator=(const Softmax &rhs){
    if( this != &rhs ){
        this->batchSize = rhs.batchSize;
        this->models = rhs.models;
        
        //Copy the base classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}

bool Softmax::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getId() == classifier->getId() ){
        const Softmax *ptr = dynamic_cast<const Softmax*>(classifier);
        
        this->batchSize = ptr->batchSize;
        this->models = ptr->models;
        
        //Copy the base classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool Softmax::train_(ClassificationData &trainingData){
    
    //Clear any previous model
    clear();
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int K = trainingData.getNumClasses();
    
    if( M == 0 ){
        errorLog << "train_(ClassificationData &labelledTrainingData) - Training data has zero samples!" << std::endl;
        return false;
    }
    
    numInputDimensions = N;
    numOutputDimensions = K;
    numClasses = K;
    models.resize(K);
    classLabels.resize(K);
    ranges = trainingData.getRanges();
    ClassificationData validationData;
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }

    if( useValidationSet ){
        validationData = trainingData.split( 100-validationSetSize );
    }
    
    //Train a regression model for each class in the training data
    for(UINT k=0; k<numClasses; k++){
        
        //Set the class label
        classLabels[k] = trainingData.getClassTracker()[k].classLabel;
        
        //Train the model
        if( !trainSoftmaxModel(classLabels[k],models[k],trainingData) ){
            errorLog << "train(ClassificationData labelledTrainingData) - Failed to train model for class: " << classLabels[k] << std::endl;
                return false;
        }
    }

    //Flag that the models have been trained
    trained = true;

    //Compute the final training stats
    trainingSetAccuracy = 0;
    validationSetAccuracy = 0;

    //If scaling was on, then the data will already be scaled, so turn it off temporially so we can test the model accuracy
    bool scalingState = useScaling;
    useScaling = false;
    if( !computeAccuracy( trainingData, trainingSetAccuracy ) ){
        trained = false;
        errorLog << "Failed to compute training set accuracy! Failed to fully train model!" << std::endl;
        return false;
    }
    
    if( useValidationSet ){
        if( !computeAccuracy( validationData, validationSetAccuracy ) ){
            trained = false;
            errorLog << "Failed to compute validation set accuracy! Failed to fully train model!" << std::endl;
            return false;
        }
    }

    trainingLog << "Training set accuracy: " << trainingSetAccuracy << std::endl;

    if( useValidationSet ){
        trainingLog << "Validation set accuracy: " << validationSetAccuracy << std::endl;
    }

    //Reset the scaling state for future prediction
    useScaling = scalingState;

    return trained;
}

bool Softmax::predict_(VectorFloat &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &inputVector) - Model Not Trained!" << std::endl;
        return false;
    }
    
    predictedClassLabel = 0;
    maxLikelihood = -10000;
    
    if( !trained ) return false;
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    //Loop over each class and compute the likelihood of the input data coming from class k. Pick the class with the highest likelihood
    Float sum = 0;
    Float bestEstimate = -grt_numeric_limits< Float >::max();
    UINT bestIndex = 0;
    for(UINT k=0; k<numClasses; k++){
        Float estimate = models[k].compute( inputVector );
        
        if( estimate > bestEstimate ){
            bestEstimate = estimate;
            bestIndex = k;
        }
        
        classDistances[k] = estimate;
        classLikelihoods[k] = estimate;
        sum += estimate;
    }
    
    if( sum > 1.0e-5 ){
        for(UINT k=0; k<numClasses; k++){
            classLikelihoods[k] /= sum;
        }
    }else{
        //If the sum is less than the value above then none of the models found a positive class
        maxLikelihood = bestEstimate;
        predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
        return true;
    }
    maxLikelihood = classLikelihoods[bestIndex];
    predictedClassLabel = classLabels[bestIndex];
    
    return true;
}

bool Softmax::trainSoftmaxModel(UINT classLabel,SoftmaxModel &model,ClassificationData &data){
    
    Float error = 0;
    Float errorSum = 0;
    Float lastErrorSum = 0;
    Float delta = 0;
    const UINT N = data.getNumDimensions();
    const UINT M = data.getNumSamples();
    UINT iter = 0;
    bool keepTraining = true;
    Random random;
    VectorFloat y(M);
    VectorFloat batchMean(N);
    Vector< UINT > randomTrainingOrder(M);
    Vector< VectorFloat > batchData(batchSize,VectorFloat(N));
    
    //Init the model
    model.init( classLabel,  N );
    
    //Setup the target vector, the input data is relabelled as positive samples (with label 1.0) and negative samples (with label 0.0)
    for(UINT i=0; i<M; i++){
        y[i] = data[i].getClassLabel()==classLabel ? 1.0 : 0;
    }
    
    //In most cases, the training data is grouped into classes (100 samples for class 1, followed by 100 samples for class 2, etc.)
    //This can cause a problem for stochastic gradient descent algorithm. To avoid this issue, we randomly shuffle the order of the
    //training samples. This random order is then used at each epoch.
    for(UINT i=0; i<M; i++){
        randomTrainingOrder[i] = i;
    }
    std::random_shuffle(randomTrainingOrder.begin(), randomTrainingOrder.end());
    
    //Clear any previous training results
    trainingResults.clear();
    trainingResults.reserve( maxNumEpochs );
    TrainingResult epochResult;

    //Run the main stochastic gradient descent training algorithm
    while( keepTraining ){

        //Run one epoch of training using stochastic gradient descent
        errorSum = 0;
        UINT m=0;
        while( m < M ){
          //Get the batch data for this update
          UINT roundSize = m+batchSize < M ? batchSize : M-m;
          batchMean.fill(0.0);
          for(UINT i=0; i<roundSize; i++){
            for(UINT j=0; j<N; j++){
              batchData[i][j] = data[ randomTrainingOrder[m+i] ][j];
              batchMean[j] += batchData[i][j];
            }
          }

          for(UINT j=0; j<N; j++) batchMean[j] /= roundSize;

          //Compute the error on this batch, given the current weights
          error = 0.0;
          for(UINT i=0; i<roundSize; i++){
            error += y[ randomTrainingOrder[m+i] ] - model.compute( batchData[i] );
          }
          error /= roundSize;
          errorSum += error;

          //Update the weights
          for(UINT j=0; j<N; j++){
            model.w[j] += learningRate  * error * batchMean[j];
          }
          model.w0 += learningRate  * error;

          m += roundSize;
        }

        //Compute the error
        delta = fabs( errorSum-lastErrorSum );
        lastErrorSum = errorSum;

        //Check to see if we should stop
        if( delta <= minChange ){
            keepTraining = false;
        }
        
        if( ++iter >= maxNumEpochs ){
            keepTraining = false;
        }
        
        trainingLog << "Class: " << classLabel << " Epoch: " << iter << " TotalError: " << errorSum << " Delta: " << delta << std::endl;
        epochResult.setClassificationResult( iter, errorSum, this );
        trainingResults.push_back( epochResult );
    }
    
    return true;
}

bool Softmax::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the Softmax model
    models.clear();
    
    return true;
}

bool Softmax::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file<<"GRT_SOFTMAX_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    if( trained ){
        file << "Models:\n";
        for(UINT k=0; k<numClasses; k++){
            file << "ClassLabel: " << models[k].classLabel << std::endl;
            file << "Weights: " << models[k].w0;
            for(UINT n=0; n<numInputDimensions; n++){
                file << " " << models[k].w[n];
            }
            file << std::endl;
        }
    }
    
    return true;
}

bool Softmax::load( std::fstream &file ){
    
    trained = false;
    numInputDimensions = 0;
    numClasses = 0;
    models.clear();
    classLabels.clear();
    
    if(!file.is_open())
    {
        errorLog << "load(string filename) - Could not open file to load model" << std::endl;
        return false;
    }
    
    std::string word;
    
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_SOFTMAX_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Find the file type header
    if(word != "GRT_SOFTMAX_MODEL_FILE_V2.0"){
        errorLog << "load(string filename) - Could not find Model File Header" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    if( trained ){
        //Resize the buffer
        models.resize(numClasses);
        classLabels.resize(numClasses);
        
        //Load the models
        file >> word;
        if(word != "Models:"){
            errorLog << "load(string filename) - Could not find the Models!" << std::endl;
            return false;
        }
        
        for(UINT k=0; k<numClasses; k++){
            file >> word;
            if(word != "ClassLabel:"){
                errorLog << "load(string filename) - Could not find the ClassLabel for model: " << k << "!" << std::endl;
                    return false;
            }
            file >> models[k].classLabel;
            classLabels[k] = models[k].classLabel;
            
            file >> word;
            if(word != "Weights:"){
                errorLog << "load(string filename) - Could not find the Weights for model: " << k << "!" << std::endl;
                    return false;
            }
            file >>  models[k].w0;
            
            models[k].N = numInputDimensions;
            models[k].w.resize( numInputDimensions );
            for(UINT n=0; n<numInputDimensions; n++){
                file >> models[k].w[n];
            }
        }
        
        //Recompute the null rejection thresholds
        recomputeNullRejectionThresholds();
        
        //Resize the prediction results to make sure it is setup for realtime prediction
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}

Vector< SoftmaxModel > Softmax::getModels() const{
    return models;
}

bool Softmax::loadLegacyModelFromFile( std::fstream &file ){
    
    std::string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "load(string filename) - Could not find NumFeatures!" << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "load(string filename) - Could not find NumClasses!" << std::endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "load(string filename) - Could not find UseScaling!" << std::endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "load(string filename) - Could not find UseNullRejection!" << std::endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numInputDimensions);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "load(string filename) - Could not find the Ranges!" << std::endl;
            return false;
        }
        for(UINT n=0; n<ranges.size(); n++){
            file >> ranges[n].minValue;
            file >> ranges[n].maxValue;
        }
    }
    
    //Resize the buffer
    models.resize(numClasses);
    classLabels.resize(numClasses);
    
    //Load the models
    file >> word;
    if(word != "Models:"){
        errorLog << "load(string filename) - Could not find the Models!" << std::endl;
        return false;
    }
    
    for(UINT k=0; k<numClasses; k++){
        file >> word;
        if(word != "ClassLabel:"){
            errorLog << "load(string filename) - Could not find the ClassLabel for model: " << k << "!" << std::endl;
                return false;
        }
        file >> models[k].classLabel;
        classLabels[k] = models[k].classLabel;
        
        file >> word;
        if(word != "Weights:"){
            errorLog << "load(string filename) - Could not find the Weights for model: " << k << "!" << std::endl;
                return false;
        }
        file >>  models[k].w0;
        
        models[k].N = numInputDimensions;
        models[k].w.resize( numInputDimensions );
        for(UINT n=0; n<numInputDimensions; n++){
            file >> models[k].w[n];
        }
    }
    
    //Recompute the null rejection thresholds
    recomputeNullRejectionThresholds();
    
    //Resize the prediction results to make sure it is setup for realtime prediction
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
    classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    
    //Flag that the model has been trained
    trained = true;
    
    return true;
}

GRT_END_NAMESPACE
