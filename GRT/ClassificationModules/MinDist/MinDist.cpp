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
#include "MinDist.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string MinDist::id = "MinDist";
std::string MinDist::getId() { return MinDist::id; }

//Register the MinDist module with the Classifier base class
RegisterClassifierModule< MinDist > MinDist::registerModule( MinDist::getId() );

MinDist::MinDist(bool useScaling,bool useNullRejection,Float nullRejectionCoeff,UINT numClusters) : Classifier( MinDist::getId() )
{
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    this->numClusters = numClusters;
    supportsNullRejection = true;
    classifierMode = STANDARD_CLASSIFIER_MODE;
}

MinDist::MinDist(const MinDist &rhs) : Classifier( MinDist::getId() )
{
    classifierMode = STANDARD_CLASSIFIER_MODE;
    *this = rhs;
}

MinDist::~MinDist(void)
{
}

MinDist& MinDist::operator=(const MinDist &rhs){
    if( this != &rhs ){
        //MinDist variables
        this->numClusters = rhs.numClusters;
        this->models = rhs.models;
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}

bool MinDist::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getId() == classifier->getId() ){
        //Invoke the equals operator
        const MinDist *ptr = dynamic_cast<const MinDist*>(classifier);
        
        this->numClusters = ptr->numClusters;
        this->models = ptr->models;
        
        //Classifier variables
        return copyBaseVariables( classifier );
    }
    
    return false;
}

bool MinDist::train_(ClassificationData &trainingData){
    
    //Clear any previous models
    clear();
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int K = trainingData.getNumClasses();
    
    if( M == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << std::endl;
        return false;
    }
    
    if( M <= numClusters ){
        errorLog << "train_(ClassificationData &trainingData) - There are not enough training samples for the number of clusters. Either reduce the number of clusters or increase the number of training samples!" << std::endl;
        return false;
    }
    
    numInputDimensions = N;
    numOutputDimensions = K;
    numClasses = K;
    models.resize(K);
    classLabels.resize(K);
    nullRejectionThresholds.resize(K);
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
    
    //Train each of the models
    for(UINT k=0; k<numClasses; k++){
        
        trainingLog << "Training model for class: " << trainingData.getClassTracker()[k].classLabel << std::endl;

        //Pass the logging state onto the kmeans algorithm
        models[k].setTrainingLoggingEnabled( this->getTrainingLoggingEnabled() );
            
        //Get the class label for the kth class
        UINT classLabel = trainingData.getClassTracker()[k].classLabel;
        
        //Set the kth class label
        classLabels[k] = classLabel;
        
        //Get all the training data for this class
        ClassificationData classData = trainingData.getClassData(classLabel);
        MatrixFloat data(classData.getNumSamples(),N);
        
        //Copy the training data into a matrix
        for(UINT i=0; i<data.getNumRows(); i++){
            for(UINT j=0; j<data.getNumCols(); j++){
                data[i][j] = classData[i][j];
            }
        }
        
        //Train the model for this class
        models[k].setGamma( nullRejectionCoeff );
        if( !models[k].train(classLabel,data,numClusters,minChange,maxNumEpochs) ){
            errorLog << "train_(ClassificationData &trainingData) - Failed to train model for class: " << classLabel;
            errorLog << ". This is might be because this class does not have enough training samples! You should reduce the number of clusters or increase the number of training samples for this class." << std::endl;
            models.clear();
            return false;
        }
            
        //Set the null rejection threshold
        nullRejectionThresholds[k] = models[k].getRejectionThreshold();
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


bool MinDist::predict_(VectorFloat &inputVector){
    
    predictedClassLabel = 0;
    maxLikelihood = 0;
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &inputVector) - MinDist Model Not Trained!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = grt_scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0.0, 1.0);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    Float sum = 0;
    Float minDist = grt_numeric_limits< Float >::max();
    for(UINT k=0; k<numClasses; k++){
        //Compute the distance for class k
        classDistances[k] = models[k].predict( inputVector );
        
        //Keep track of the best value
        if( classDistances[k] < minDist ){
            minDist = classDistances[k];
            predictedClassLabel = k;
        }
        
        //Set the class likelihoods as 1.0 / dist[k], the small number is to stop divide by zero
        classLikelihoods[k] = 1.0 / (classDistances[k] + 0.0001);
        sum += classLikelihoods[k];
    }
    
    //Normalize the classlikelihoods
    if( sum != 0 ){
        for(UINT k=0; k<numClasses; k++){
            classLikelihoods[k] /= sum;
        }
        maxLikelihood = classLikelihoods[predictedClassLabel];
    }else maxLikelihood = classLikelihoods[predictedClassLabel];
    
    if( useNullRejection ){
        //Check to see if the best result is greater than the models threshold
        if( minDist <= models[predictedClassLabel].getRejectionThreshold() ) predictedClassLabel = models[predictedClassLabel].getClassLabel();
        else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
        }else predictedClassLabel = models[predictedClassLabel].getClassLabel();
    
    return true;
}

bool MinDist::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the MinDist variables
    models.clear();
    
    return true;
}

bool MinDist::recomputeNullRejectionThresholds(){
    
    if( trained ){
        for(UINT k=0; k<numClasses; k++) {
            models[k].setGamma( nullRejectionCoeff );
            models[k].recomputeThresholdValue();
        }
        return true;
    }
    return false;
}

bool MinDist::setNullRejectionCoeff(Float nullRejectionCoeff){
    
    if( nullRejectionCoeff > 0 ){
        this->nullRejectionCoeff = nullRejectionCoeff;
        recomputeNullRejectionThresholds();
        return true;
    }
    return false;
}

UINT MinDist::getNumClusters() const{
    return numClusters;
}

Vector< MinDistModel > MinDist::getModels() const {
    return models;
}

bool MinDist::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file<<"GRT_MINDIST_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    if( trained ){
        
        //Write each of the models
        for(UINT k=0; k<numClasses; k++){
            file << "ClassLabel: " << models[k].getClassLabel() << std::endl;
            file << "NumClusters: " << models[k].getNumClusters() << std::endl;
            file << "RejectionThreshold:  " << models[k].getRejectionThreshold() << std::endl;
            file << "Gamma: " << models[k].getGamma() << std::endl;
            file << "TrainingMu: " << models[k].getTrainingMu() << std::endl;
            file << "TrainingSigma: " << models[k].getTrainingSigma() << std::endl;
            file << "ClusterData:" << std::endl;
            Matrix<Float> clusters = models[k].getClusters();
            for(UINT i=0; i<models[k].getNumClusters(); i++){
                for(UINT j=0; j<models[k].getNumFeatures(); j++){
                    file << clusters[i][j] << "\t";
                }
                file << std::endl;
            }
        }
        
    }
    
    return true;
}

bool MinDist::load( std::fstream &file ){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "load(string filename) - Could not open file to load model" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the file header
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_MINDIST_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Find the file type header
    if(word != "GRT_MINDIST_MODEL_FILE_V2.0"){
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
        
        //Load each of the K models
        for(UINT k=0; k<numClasses; k++){
            Float rejectionThreshold;
            Float gamma;
            Float trainingSigma;
            Float trainingMu;
            
            file >> word;
            if( word != "ClassLabel:" ){
                errorLog << "load(string filename) - Could not load the class label for class " << k << std::endl;
                    return false;
            }
            file >> classLabels[k];
            
            file >> word;
            if( word != "NumClusters:" ){
                errorLog << "load(string filename) - Could not load the NumClusters for class " << k << std::endl;
                    return false;
            }
            file >> numClusters;
            
            file >> word;
            if( word != "RejectionThreshold:" ){
                errorLog << "load(string filename) - Could not load the RejectionThreshold for class " << k << std::endl;
                    return false;
            }
            file >> rejectionThreshold;
            
            file >> word;
            if( word != "Gamma:" ){
                errorLog << "load(string filename) - Could not load the Gamma for class " << k << std::endl;
                    return false;
            }
            file >> gamma;
            
            file >> word;
            if( word != "TrainingMu:" ){
                errorLog << "load(string filename) - Could not load the TrainingMu for class " << k << std::endl;
                    return false;
            }
            file >> trainingMu;
            
            file >> word;
            if( word != "TrainingSigma:" ){
                errorLog << "load(string filename) - Could not load the TrainingSigma for class " << k << std::endl;
                    return false;
            }
            file >> trainingSigma;
            
            file >> word;
            if( word != "ClusterData:" ){
                errorLog << "load(string filename) - Could not load the ClusterData for class " << k << std::endl;
                    return false;
            }
            
            //Load the cluster data
            MatrixFloat clusters(numClusters,numInputDimensions);
            for(UINT i=0; i<numClusters; i++){
                for(UINT j=0; j<numInputDimensions; j++){
                    file >> clusters[i][j];
                }
            }
            
            models[k].setClassLabel( classLabels[k] );
            models[k].setClusters( clusters );
            models[k].setGamma( gamma );
            models[k].setRejectionThreshold( rejectionThreshold );
            models[k].setTrainingSigma( trainingSigma );
            models[k].setTrainingMu( trainingMu );
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

bool MinDist::setNumClusters(UINT numClusters){
    if( numClusters > 0 ){
        this->numClusters = numClusters;
        return true;
    }
    return true;
}

bool MinDist::loadLegacyModelFromFile( std::fstream &file ){
    
    std::string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "load(string filename) - Could not find NumFeatures " << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "load(string filename) - Could not find NumClasses" << std::endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "load(string filename) - Could not find UseScaling" << std::endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "load(string filename) - Could not find UseNullRejection" << std::endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize(numInputDimensions);
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "load(string filename) - Could not find the Ranges" << std::endl;
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
    
    //Load each of the K models
    for(UINT k=0; k<numClasses; k++){
        Float rejectionThreshold;
        Float gamma;
        Float trainingSigma;
        Float trainingMu;
        
        file >> word;
        if( word != "ClassLabel:" ){
            errorLog << "load(string filename) - Could not load the class label for class " << k << std::endl;
                return false;
        }
        file >> classLabels[k];
        
        file >> word;
        if( word != "NumClusters:" ){
            errorLog << "load(string filename) - Could not load the NumClusters for class " << k << std::endl;
                return false;
        }
        file >> numClusters;
        
        file >> word;
        if( word != "RejectionThreshold:" ){
            errorLog << "load(string filename) - Could not load the RejectionThreshold for class " << k << std::endl;
                return false;
        }
        file >> rejectionThreshold;
        
        file >> word;
        if( word != "Gamma:" ){
            errorLog << "load(string filename) - Could not load the Gamma for class " << k << std::endl;
                return false;
        }
        file >> gamma;
        
        file >> word;
        if( word != "TrainingMu:" ){
            errorLog << "load(string filename) - Could not load the TrainingMu for class " << k << std::endl;
                return false;
        }
        file >> trainingMu;
        
        file >> word;
        if( word != "TrainingSigma:" ){
            errorLog << "load(string filename) - Could not load the TrainingSigma for class " << k << std::endl;
                return false;
        }
        file >> trainingSigma;
        
        file >> word;
        if( word != "ClusterData:" ){
            errorLog << "load(string filename) - Could not load the ClusterData for class " << k << std::endl;
                return false;
        }
        
        //Load the cluster data
        MatrixFloat clusters(numClusters,numInputDimensions);
        for(UINT i=0; i<numClusters; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                file >> clusters[i][j];
            }
        }
        
        models[k].setClassLabel( classLabels[k] );
        models[k].setClusters( clusters );
        models[k].setGamma( gamma );
        models[k].setRejectionThreshold( rejectionThreshold );
        models[k].setTrainingSigma( trainingSigma );
        models[k].setTrainingMu( trainingMu );
    }
    
    //Recompute the null rejection thresholds
    recomputeNullRejectionThresholds();
    
    //Resize the prediction results to make sure it is setup for realtime prediction
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
    classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    
    trained = true;
    
    return true;
}

GRT_END_NAMESPACE
