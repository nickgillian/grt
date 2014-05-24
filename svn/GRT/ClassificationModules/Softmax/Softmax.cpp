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

#include "Softmax.h"

namespace GRT{

//Register the Softmax module with the Classifier base class
RegisterClassifierModule< Softmax >  Softmax::registerModule("Softmax");

Softmax::Softmax(bool useScaling)
{
    this->useScaling = useScaling;
    minChange = 1.0e-10;
    maxNumIterations = 1000;
    learningRate = 0.01;
    classType = "Softmax";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG Softmax]");
    errorLog.setProceedingText("[ERROR Softmax]");
    trainingLog.setProceedingText("[TRAINING Softmax]");
    warningLog.setProceedingText("[WARNING Softmax]");
}
    
Softmax::Softmax(const Softmax &rhs){
    classType = "Softmax";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG Softmax]");
    errorLog.setProceedingText("[ERROR Softmax]");
    trainingLog.setProceedingText("[TRAINING Softmax]");
    warningLog.setProceedingText("[WARNING Softmax]");
    *this = rhs;
}

Softmax::~Softmax(void)
{
}
    
Softmax& Softmax::operator=(const Softmax &rhs){
	if( this != &rhs ){
        this->learningRate = rhs.learningRate;
        this->minChange = rhs.minChange;
        this->maxNumIterations = rhs.maxNumIterations;
        this->models = rhs.models;
        
        //Copy the base classifier variables
        copyBaseVariables( (Classifier*)&rhs );
	}
	return *this;
}

bool Softmax::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        Softmax *ptr = (Softmax*)classifier;
        
        this->learningRate = ptr->learningRate;
        this->minChange = ptr->minChange;
        this->maxNumIterations = ptr->maxNumIterations;
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
        errorLog << "train_(ClassificationData &labelledTrainingData) - Training data has zero samples!" << endl;
        return false;
    }
    
    numInputDimensions = N;
    numClasses = K;
    models.resize(K);
    classLabels.resize(K);
    ranges = trainingData.getRanges();
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }
    
    //Train a regression model for each class in the training data
    for(UINT k=0; k<numClasses; k++){
        
        //Set the class label
        classLabels[k] = trainingData.getClassTracker()[k].classLabel;
        
        //Train the model
        if( !trainSoftmaxModel(classLabels[k],models[k],trainingData) ){
            errorLog << "train(ClassificationData labelledTrainingData) - Failed to train model for class: " << classLabels[k] << endl;
            return false;
        }
    }
    
    //Flag that the algorithm has been trained
    trained = true;
    return trained;
}

bool Softmax::predict_(VectorDouble &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - Model Not Trained!" << endl;
        return false;
    }
    
    predictedClassLabel = 0;
	maxLikelihood = -10000;
    
    if( !trained ) return false;
    
	if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << endl;
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
    double sum = 0;
    double bestEstimate = numeric_limits<double>::min();
    UINT bestIndex = 0;
    for(UINT k=0; k<numClasses; k++){
        double estimate = models[k].compute( inputVector );
        
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
    
    double error = 0;
    double errorSum = 0;
    double lastErrorSum = 0;
    double delta = 0;
    UINT N = data.getNumDimensions();
    UINT M = data.getNumSamples();
    UINT iter = 0;
    bool keepTraining = true;
    Random random;
    VectorDouble y(M);
    vector< UINT > randomTrainingOrder(M);
    
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
    
    //Run the main stochastic gradient descent training algorithm
    while( keepTraining ){
        
        //Run one epoch of training using stochastic gradient descent
        errorSum = 0;
        for(UINT m=0; m<M; m++){
            
            //Select the random sample
            UINT i = randomTrainingOrder[m];
            
            //Compute the error, given the current weights
            error = y[i] - model.compute( data[i].getSample() );
            errorSum += error;
            
            //Update the weights
            for(UINT j=0; j<N; j++){
                model.w[j] += learningRate  * error * data[i][j];
            }
            model.w0 += learningRate  * error;
        }
        
        //Compute the error
        delta = fabs( errorSum-lastErrorSum );
        lastErrorSum = errorSum;
        
        //Check to see if we should stop
        if( delta <= minChange ){
            keepTraining = false;
        }
        
        if( ++iter >= maxNumIterations ){
            keepTraining = false;
        }
        
        trainingLog << "Epoch: " << iter << " TotalError: " << errorSum << " Delta: " << delta << endl;
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
    
bool Softmax::saveModelToFile(string filename) const{

    if( !trained ) return false;
    
	std::fstream file; 
	file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }

	file.close();

	return true;
}
    
bool Softmax::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"loadModelFromFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file<<"GRT_SOFTMAX_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    if( trained ){
        file << "Models:\n";
        for(UINT k=0; k<numClasses; k++){
            file << "ClassLabel: " << models[k].classLabel << endl;
            file << "Weights: " << models[k].w0;
            for(UINT n=0; n<numInputDimensions; n++){
                file << " " << models[k].w[n];
            }
            file << endl;
        }
    }
    
    return true;
}

bool Softmax::loadModelFromFile(string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    file.close();

	return true;

}
    
bool Softmax::loadModelFromFile(fstream &file){
    
    trained = false;
    numInputDimensions = 0;
    numClasses = 0;
    models.clear();
    classLabels.clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << endl;
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
        errorLog << "loadModelFromFile(string filename) - Could not find Model File Header" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    if( trained ){
        //Resize the buffer
        models.resize(numClasses);
        classLabels.resize(numClasses);
        
        //Load the models
        file >> word;
        if(word != "Models:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the Models!" << endl;
            return false;
        }
        
        for(UINT k=0; k<numClasses; k++){
            file >> word;
            if(word != "ClassLabel:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the ClassLabel for model: " << k << "!" << endl;
                return false;
            }
            file >> models[k].classLabel;
            classLabels[k] = models[k].classLabel;
            
            file >> word;
            if(word != "Weights:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the Weights for model: " << k << "!" << endl;
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
    
bool Softmax::setLearningRate(double learningRate){
    if( learningRate > 0 ){
        this->learningRate = learningRate;
        return true;
    }
    return false;
}

bool Softmax::setMinChange(double minChange){
    if( minChange > 0 ){
        this->minChange = minChange;
        return true;
    }
    return false;
}

bool Softmax::setMaxNumIterations(UINT maxNumIterations){
    if( maxNumIterations > 0 ){
        this->maxNumIterations = maxNumIterations;
        return true;
    }
    return false;
}

double Softmax::getLearningRate(){
    return learningRate;
}

double Softmax::getMinChange(){
    return minChange;
}

UINT Softmax::getMaxNumIterations(){
    return maxNumIterations;
}
    
vector< SoftmaxModel > Softmax::getModels(){
    return models;
}
    
bool Softmax::loadLegacyModelFromFile( fstream &file ){
    
    string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadModelFromFile(string filename) - Could not find NumFeatures!" << endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadModelFromFile(string filename) - Could not find NumClasses!" << endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadModelFromFile(string filename) - Could not find UseScaling!" << endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadModelFromFile(string filename) - Could not find UseNullRejection!" << endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numInputDimensions);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the Ranges!" << endl;
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
        errorLog << "loadModelFromFile(string filename) - Could not find the Models!" << endl;
        return false;
    }
    
    for(UINT k=0; k<numClasses; k++){
        file >> word;
        if(word != "ClassLabel:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the ClassLabel for model: " << k << "!" << endl;
            return false;
        }
        file >> models[k].classLabel;
        classLabels[k] = models[k].classLabel;
        
        file >> word;
        if(word != "Weights:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the Weights for model: " << k << "!" << endl;
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

} //End of namespace GRT

