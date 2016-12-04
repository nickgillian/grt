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
#include "KNN.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string KNN::id = "KNN";
std::string KNN::getId() { return KNN::id; }

//Register the KNN module with the Classifier base class
RegisterClassifierModule< KNN > KNN::registerModule( KNN::getId() );

KNN::KNN(unsigned int K,bool useScaling,bool useNullRejection,Float nullRejectionCoeff,bool searchForBestKValue,UINT minKSearchValue,UINT maxKSearchValue) : Classifier( KNN::getId() )
{
    this->K = K;
    this->distanceMethod = EUCLIDEAN_DISTANCE;
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    this->searchForBestKValue = searchForBestKValue;
    this->minKSearchValue = minKSearchValue;
    this->maxKSearchValue = maxKSearchValue;
    supportsNullRejection = true;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    distanceMethod = EUCLIDEAN_DISTANCE;
}

KNN::KNN(const KNN &rhs) : Classifier( KNN::getId() )
{
    classifierMode = STANDARD_CLASSIFIER_MODE;
    *this = rhs;
}

KNN::~KNN(void)
{
}

KNN& KNN::operator=(const KNN &rhs){
    if( this != &rhs ){
        //KNN variables
        this->K = rhs.K;
        this->distanceMethod = rhs.distanceMethod;
        this->searchForBestKValue = rhs.searchForBestKValue;
        this->minKSearchValue = rhs.minKSearchValue;
        this->maxKSearchValue = rhs.maxKSearchValue;
        this->trainingData = rhs.trainingData;
        this->trainingMu = rhs.trainingMu;
        this->trainingSigma = rhs.trainingSigma;
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}

bool KNN::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getId() == classifier->getId() ){
        //Get a pointer the KNN copy instance
        const KNN *ptr = dynamic_cast<const KNN*>(classifier);
        
        this->K = ptr->K;
        this->distanceMethod = ptr->distanceMethod;
        this->searchForBestKValue = ptr->searchForBestKValue;
        this->minKSearchValue = ptr->minKSearchValue;
        this->maxKSearchValue = ptr->maxKSearchValue;
        this->trainingData = ptr->trainingData;
        this->trainingMu = ptr->trainingMu;
        this->trainingSigma = ptr->trainingSigma;
        
        //Classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool KNN::train_(ClassificationData &trainingData){
    
    //Clear any previous models
    clear();
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << std::endl;
        return false;
    }

    //Store the number of features, classes and the training data
    this->numInputDimensions = trainingData.getNumDimensions();
    this->numOutputDimensions = trainingData.getNumClasses();
    this->numClasses = trainingData.getNumClasses();
    this->ranges = trainingData.getRanges();

    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }

    //TODO: In the future need to build a kdtree from the training data to allow better realtime prediction
    this->trainingData = trainingData;

    ClassificationData validationData;
    if( useValidationSet ){
        validationData = trainingData.split( 100-validationSetSize );
    }
    
    //Set the class labels
    classLabels.resize( numClasses );
    for(UINT k=0; k<numClasses; k++){
        classLabels[k] = trainingData.getClassTracker()[k].classLabel;
    }
    
    //If we do not need to search for the best K value, then call the sub training function with the default value of K
    if( !searchForBestKValue ){
        if( !train_(trainingData,K) ){
            return false;
        }
    }else{
        //If we have got this far then we are going to search for the best K value
        UINT index = 0;
        Float bestAccuracy = 0;
        Vector< IndexedDouble > trainingAccuracyLog;
        
        for(UINT k=minKSearchValue; k<=maxKSearchValue; k++){
            //Randomly spilt the data and use 80% to train the algorithm and 20% to test it
            ClassificationData testSet = useValidationSet ? validationData : trainingData.split(80,true);
            
            if( !train_(trainingData, k) ){
                errorLog << "Failed to train model for a k value of " << k << std::endl;
            }else{
                    
                //Compute the classification error
                Float accuracy = 0;
                for(UINT i=0; i<testSet.getNumSamples(); i++){
                    
                    VectorFloat sample = testSet[i].getSample();
                    
                    if( !predict( sample , k) ){
                        errorLog << "Failed to predict label for test sample with a k value of " << k << std::endl;
                            return false;
                    }
                    
                    if( testSet[i].getClassLabel() == predictedClassLabel ){
                        accuracy++;
                    }
                }
                
                accuracy = accuracy /Float( testSet.getNumSamples() ) * 100.0;
                trainingAccuracyLog.push_back( IndexedDouble(k,accuracy) );
                
                trainingLog << "K:\t" << k << "\tAccuracy:\t" << accuracy << std::endl;
                
                if( accuracy > bestAccuracy ){
                    bestAccuracy = accuracy;
                }
                
                index++;
            }
                
        }
            
        if( bestAccuracy > 0 ){
            //Sort the training log by value
            std::sort(trainingAccuracyLog.begin(),trainingAccuracyLog.end(),IndexedDouble::sortIndexedDoubleByValueDescending);
            
            //Copy the top matching values into a temporary buffer
            Vector< IndexedDouble > tempLog;
            
            //Add the first value
            tempLog.push_back( trainingAccuracyLog[0] );
            
            //Keep adding values until the value changes
            for(UINT i=1; i<trainingAccuracyLog.size(); i++){
                if( trainingAccuracyLog[i].value == tempLog[0].value ){
                    tempLog.push_back( trainingAccuracyLog[i] );
                }else break;
            }
            
            //Sort the temp values by index (the index is the K value so we want to get the minimum K value with the maximum accuracy)
            std::sort(tempLog.begin(),tempLog.end(),IndexedDouble::sortIndexedDoubleByIndexAscending);
            
            trainingLog << "Best K Value: " << tempLog[0].index << "\tAccuracy:\t" << tempLog[0].value << std::endl;
            
            //Use the minimum index, this should give us the best accuracy with the minimum K value
            //We now need to train the model again to make sure all the training metrics are computed correctly
            if( !train_(trainingData,tempLog[0].index) ){
                return false;
            }
        }
    }

    //If we get this far, then a model has been trained

    //Compute the final training stats
    trainingSetAccuracy = 0;
    validationSetAccuracy = 0;

    //If scaling was on, then the data will already be scaled, so turn it off temporially
    bool scalingState = useScaling;
    useScaling = false;
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        if( !predict_( trainingData[i].getSample() ) ){
            trained = false;
            errorLog << "Failed to run prediction for training sample: " << i << "! Failed to fully train model!" << std::endl;
            return false;
        }

        if( predictedClassLabel == trainingData[i].getClassLabel() ){
            trainingSetAccuracy++;
        }
    }

    if( useValidationSet ){
        for(UINT i=0; i<validationData.getNumSamples(); i++){
            if( !predict_( validationData[i].getSample() ) ){
                trained = false;
                errorLog << "Failed to run prediction for validation sample: " << i << "! Failed to fully train model!" << std::endl;
                return false;
            }

            if( predictedClassLabel == validationData[i].getClassLabel() ){
                validationSetAccuracy++;
            }
        }
    }

    trainingSetAccuracy = trainingSetAccuracy / trainingData.getNumSamples() * 100.0;

    trainingLog << "Training set accuracy: " << trainingSetAccuracy << std::endl;

    if( useValidationSet ){
        validationSetAccuracy = validationSetAccuracy / validationData.getNumSamples() * 100.0;
        trainingLog << "Validation set accuracy: " << validationSetAccuracy << std::endl;
    }

    //Reset the scaling state for future prediction
    useScaling = scalingState;
        
    return true;
}
    
bool KNN::train_(const ClassificationData &trainingData,const UINT K){
    
    //Set the dimensionality of the input data
    this->K = K;
    
    //Flag that the algorithm has been trained so we can compute the rejection thresholds
    trained = true;
    
    //If null rejection is enabled then compute the null rejection thresholds
    if( useNullRejection ){
        
        //Set the null rejection to false so we can compute the values for it (this will be set back to its current value later)
        useNullRejection = false;
        nullRejectionThresholds.clear();
        
        //Compute the rejection thresholds for each of the K classes
        VectorFloat counter(numClasses,0);
        trainingMu.resize( numClasses, 0 );
        trainingSigma.resize( numClasses, 0 );
        nullRejectionThresholds.resize( numClasses, 0 );
        
        //Compute Mu for each of the classes
        const unsigned int numTrainingExamples = trainingData.getNumSamples();
        Vector< IndexedDouble > predictionResults( numTrainingExamples );
        for(UINT i=0; i<numTrainingExamples; i++){
            predict( trainingData[i].getSample(), K);
            
            UINT classLabelIndex = 0;
            for(UINT k=0; k<numClasses; k++){
                if( predictedClassLabel == classLabels[k] ){
                    classLabelIndex = k;
                    break;
                }
            }
            
            predictionResults[ i ].index = classLabelIndex;
            predictionResults[ i ].value = classDistances[ classLabelIndex ];
            
            trainingMu[ classLabelIndex ] += predictionResults[ i ].value;
            counter[ classLabelIndex ]++;
        }
        
        for(UINT j=0; j<numClasses; j++){
            trainingMu[j] /= counter[j];
        }
        
        //Compute Sigma for each of the classes
        for(UINT i=0; i<numTrainingExamples; i++){
            trainingSigma[predictionResults[i].index] += SQR(predictionResults[i].value - trainingMu[predictionResults[i].index]);
        }
        
        for(UINT j=0; j<numClasses; j++){
            Float count = counter[j];
            if( count > 1 ){
                trainingSigma[ j ] = sqrt( trainingSigma[j] / (count-1) );
            }else{
                trainingSigma[ j ] = 1.0;
            }
        }
        
        //Check to see if any of the mu or sigma values are zero or NaN
        bool errorFound = false;
        for(UINT j=0; j<numClasses; j++){
            if( trainingMu[j] == 0 ){
                warningLog << "TrainingMu[ " << j << " ] is zero for a K value of " << K << std::endl;
                }
            if( trainingSigma[j] == 0 ){
                warningLog << "TrainingSigma[ " << j << " ] is zero for a K value of " << K << std::endl;
                }
            if( grt_isnan( trainingMu[j] ) ){
                errorLog << "TrainingMu[ " << j << " ] is NAN for a K value of " << K << std::endl;
                    errorFound = true;
            }
            if( grt_isnan( trainingSigma[j] ) ){
                errorLog << "TrainingSigma[ " << j << " ] is NAN for a K value of " << K << std::endl;
                    errorFound = true;
            }
        }
        
        if( errorFound ){
            trained = false;
            return false;
        }
        
        //Compute the rejection thresholds
        for(unsigned int j=0; j<numClasses; j++){
            nullRejectionThresholds[j] = trainingMu[j] + (trainingSigma[j]*nullRejectionCoeff);
        }
        
        //Restore the actual state of the null rejection
        useNullRejection = true;
        
    }else{
        //Resize the rejection thresholds but set the values to 0
        nullRejectionThresholds.clear();
        nullRejectionThresholds.resize( numClasses, 0 );
    }
    
    return true;
}

bool KNN::predict_(VectorFloat &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &inputVector) - KNN model has not been trained" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &inputVector) - the size of the input vector " << inputVector.size() << " does not match the number of features " << numInputDimensions <<  std::endl;
        return false;
    }
    
    //Scale the input vector if needed
    if( useScaling ){
        for(UINT i=0; i<numInputDimensions; i++){
            inputVector[i] = scale(inputVector[i], ranges[i].minValue, ranges[i].maxValue, 0, 1);
        }
    }
    
    //Run the prediction
    return predict(inputVector,K);
}

bool KNN::predict(const VectorFloat &inputVector,const UINT K){
    
    if( !trained ){
        errorLog << "predict(VectorFloat inputVector,UINT K) - KNN model has not been trained" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "predict(VectorFloat inputVector) - the size of the input vector " << inputVector.size() << " does not match the number of features " << numInputDimensions <<  std::endl;
        return false;
    }
    
    if( K > trainingData.getNumSamples() ){
        errorLog << "predict(VectorFloat inputVector,UINT K) - K Is Greater Than The Number Of Training Samples" << std::endl;
        return false;
    }
    
    //TODO - need to build a kdtree of the training data to allow better realtime prediction
    const UINT M = trainingData.getNumSamples();
    Vector< IndexedDouble > neighbours;
    
    for(UINT i=0; i<M; i++){
        Float dist = 0;
        UINT classLabel = trainingData[i].getClassLabel();
        VectorFloat trainingSample = trainingData[i].getSample();
        
        switch( distanceMethod ){
            case EUCLIDEAN_DISTANCE:
            dist = computeEuclideanDistance(inputVector,trainingSample);
            break;
            case COSINE_DISTANCE:
            dist = computeCosineDistance(inputVector,trainingSample);
            break;
            case MANHATTAN_DISTANCE:
            dist = computeManhattanDistance(inputVector, trainingSample);
            break;
            default:
            errorLog << "predict(vector< Float > inputVector) - unkown distance measure!" << std::endl;
            return false;
            break;
        }
        
        if( neighbours.size() < K ){
            neighbours.push_back( IndexedDouble(classLabel,dist) );
        }else{
            //Find the maximum value in the neighbours buffer
            Float maxValue = neighbours[0].value;
            UINT maxIndex = 0;
            for(UINT n=1; n<neighbours.size(); n++){
                if( neighbours[n].value > maxValue ){
                    maxValue = neighbours[n].value;
                    maxIndex = n;
                }
            }
            
            //If the dist is less than the maximum value in the buffer, then replace that value with the new dist
            if( dist < maxValue ){
                neighbours[ maxIndex ] = IndexedDouble(classLabel,dist);
            }
        }
    }
    
    //Predict the class ID using the labels of the K nearest neighbours
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses);
    
    std::fill(classLikelihoods.begin(),classLikelihoods.end(),0);
    std::fill(classDistances.begin(),classDistances.end(),0);
    
    //Count the classes
    for(UINT k=0; k<neighbours.size(); k++){
        UINT classLabel = neighbours[k].index;
        if( classLabel == 0 ){
            errorLog << "predict(VectorFloat inputVector) - Class label of training example can not be zero!" << std::endl;
            return false;
        }
        
        //Find the index of the classLabel
        UINT classLabelIndex = 0;
        for(UINT j=0; j<numClasses; j++){
            if( classLabel == classLabels[j] ){
                classLabelIndex = j;
                break;
            }
        }
        classLikelihoods[ classLabelIndex ] += 1;
        classDistances[ classLabelIndex ] += neighbours[k].value;
    }
    
    //Get the max count
    Float maxCount = classLikelihoods[0];
    UINT maxIndex = 0;
    for(UINT i=1; i<classLikelihoods.size(); i++){
        if( classLikelihoods[i] > maxCount ){
            maxCount = classLikelihoods[i];
            maxIndex = i;
        }
    }
    
    //Compute the average distances per class
    for(UINT i=0; i<numClasses; i++){
        if( classLikelihoods[i] > 0 )   classDistances[i] /= classLikelihoods[i];
        else classDistances[i] = BIG_DISTANCE;
        }
    
    //Normalize the likelihoods
    for(UINT i=0; i<numClasses; i++){
        classLikelihoods[i] /= Float( neighbours.size() );
    }
    
    //Set the maximum likelihood value
    maxLikelihood = classLikelihoods[ maxIndex ];
    
    if( useNullRejection ){
        if( classDistances[ maxIndex ] <= nullRejectionThresholds[ maxIndex ] ){
            predictedClassLabel = classLabels[maxIndex];
        }else{
            predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL; //Set the gesture label as the null label
        }
    }else{
        predictedClassLabel = classLabels[maxIndex];
    }
    
    return true;
}

bool KNN::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the KNN model
    trainingData.clear();
    trainingMu.clear();
    trainingSigma.clear();
    
    return true;
}

bool KNN::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog << "save(fstream &file) - Could not open file to save model!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "GRT_KNN_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    file << "K: " << K << std::endl;
    file << "DistanceMethod: " << distanceMethod << std::endl;
    file << "SearchForBestKValue: " << searchForBestKValue << std::endl;
    file << "MinKSearchValue: " << minKSearchValue << std::endl;
    file << "MaxKSearchValue: " << maxKSearchValue << std::endl;
    
    if( trained ){
        if( useNullRejection ){
            file << "TrainingMu: ";
            for(UINT j=0; j<trainingMu.size(); j++){
                file << trainingMu[j] << "\t";
            }file << std::endl;
            
            file << "TrainingSigma: ";
            for(UINT j=0; j<trainingSigma.size(); j++){
                file << trainingSigma[j] << "\t";
            }file << std::endl;
        }
        
        file << "NumTrainingSamples: " << trainingData.getNumSamples() << std::endl;
        file << "TrainingData: \n";
        
        //Right each of the models
        for(UINT i=0; i<trainingData.getNumSamples(); i++){
            file<< trainingData[i].getClassLabel() << "\t";
            
            for(UINT j=0; j<numInputDimensions; j++){
                file << trainingData[i][j] << "\t";
            }
            file << std::endl;
        }
    }
    
    return true;
}

bool KNN::load( std::fstream &file ){
    
    if(!file.is_open())
    {
        errorLog << "load(fstream &file) - Could not open file to load model!" << std::endl;
        return false;
    }
    
    std::string word;
    
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_KNN_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Find the file type header
    if(word != "GRT_KNN_MODEL_FILE_V2.0"){
        errorLog << "load(fstream &file) - Could not find Model File Header!" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "K:"){
        errorLog << "load(fstream &file) - Could not find K!" << std::endl;
        return false;
    }
    file >> K;
    
    file >> word;
    if(word != "DistanceMethod:"){
        errorLog << "load(fstream &file) - Could not find DistanceMethod!" << std::endl;
        return false;
    }
    file >> distanceMethod;
    
    file >> word;
    if(word != "SearchForBestKValue:"){
        errorLog << "load(fstream &file) - Could not find SearchForBestKValue!" << std::endl;
        return false;
    }
    file >> searchForBestKValue;
    
    file >> word;
    if(word != "MinKSearchValue:"){
        errorLog << "load(fstream &file) - Could not find MinKSearchValue!" << std::endl;
        return false;
    }
    file >> minKSearchValue;
    
    file >> word;
    if(word != "MaxKSearchValue:"){
        errorLog << "load(fstream &file) - Could not find MaxKSearchValue!" << std::endl;
        return false;
    }
    file >> maxKSearchValue;
    
    if( trained ){
        
        //Resize the buffers
        trainingMu.resize(numClasses,0);
        trainingSigma.resize(numClasses,0);
        
        if( useNullRejection ){
            file >> word;
            if(word != "TrainingMu:"){
                errorLog << "load(fstream &file) - Could not find TrainingMu!" << std::endl;
                return false;
            }
            
            //Load the trainingMu data
            for(UINT j=0; j<numClasses; j++){
                file >> trainingMu[j];
            }
            
            file >> word;
            if(word != "TrainingSigma:"){
                errorLog << "load(fstream &file) - Could not find TrainingSigma!" << std::endl;
                return false;
            }
            
            //Load the trainingSigma data
            for(UINT j=0; j<numClasses; j++){
                file >> trainingSigma[j];
            }
        }
        
        file >> word;
        if(word != "NumTrainingSamples:"){
            errorLog << "load(fstream &file) - Could not find NumTrainingSamples!" << std::endl;
            return false;
        }
        unsigned int numTrainingSamples = 0;
        file >> numTrainingSamples;
        
        file >> word;
        if(word != "TrainingData:"){
            errorLog << "load(fstream &file) - Could not find TrainingData!" << std::endl;
            return false;
        }
        
        //Load the training data
        trainingData.setNumDimensions(numInputDimensions);
        unsigned int classLabel = 0;
        VectorFloat sample(numInputDimensions,0);
        for(UINT i=0; i<numTrainingSamples; i++){
            //Read the class label
            file >> classLabel;
            
            //Read the feature vector
            for(UINT j=0; j<numInputDimensions; j++){
                file >> sample[j];
            }
            
            //Add it to the training data
            trainingData.addSample(classLabel, sample);
        }
        
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}

bool KNN::recomputeNullRejectionThresholds(){
    
    if( !trained ){
        return false;
    }
    
    nullRejectionThresholds.resize(numClasses,0);
    
    if( trainingMu.size() != numClasses || trainingSigma.size() != numClasses ){
        return false;
    }
    
    for(unsigned int j=0; j<numClasses; j++){
        nullRejectionThresholds[j] = trainingMu[j] + (trainingSigma[j]*nullRejectionCoeff);
    }
    
    return true;
}

bool KNN::setK(UINT K){
    if( K > 0 ){
        this->K = K;
        return true;
    }
    return false;
}

bool KNN::setMinKSearchValue(UINT minKSearchValue){
    this->minKSearchValue = minKSearchValue;
    return true;
}

bool KNN::setMaxKSearchValue(UINT maxKSearchValue){
    this->maxKSearchValue = maxKSearchValue;
    return true;
}

bool KNN::enableBestKValueSearch(bool searchForBestKValue){
    this->searchForBestKValue = searchForBestKValue;
    return true;
}

bool KNN::setNullRejectionCoeff(Float nullRejectionCoeff){
    if( nullRejectionCoeff > 0 ){
        this->nullRejectionCoeff = nullRejectionCoeff;
        recomputeNullRejectionThresholds();
        return true;
    }
    return false;
}

bool KNN::setDistanceMethod(UINT distanceMethod){
    if( distanceMethod == EUCLIDEAN_DISTANCE || distanceMethod == COSINE_DISTANCE || distanceMethod == MANHATTAN_DISTANCE ){
        this->distanceMethod = distanceMethod;
        return true;
    }
    return false;
}

Float KNN::computeEuclideanDistance(const VectorFloat &a,const VectorFloat &b){
    Float dist = 0;
    for(UINT j=0; j<numInputDimensions; j++){
        dist += SQR( a[j] - b[j] );
    }
    return sqrt( dist );
}

Float KNN::computeCosineDistance(const VectorFloat &a,const VectorFloat &b){
    Float dist = 0;
    
    Float dotAB = 0;
    Float magA = 0;
    Float magB = 0;
    
    for(UINT j=0; j<numInputDimensions; j++){
        dotAB += a[j] * b[j];
        magA += SQR(a[j]);
        magB += SQR(b[j]);
    }
    
    dist = dotAB / (sqrt(magA) * sqrt(magB));
    
    return dist;
}

Float KNN::computeManhattanDistance(const VectorFloat &a,const VectorFloat &b){
    Float dist = 0;
    
    for(UINT j=0; j<numInputDimensions; j++){
        dist += fabs( a[j] - b[j] );
    }
    
    return dist;
}

bool KNN::loadLegacyModelFromFile( std::fstream &file ){
    
    std::string word;
    
    //Find the file type header
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find NumFeatures!" << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find NumClasses!" << std::endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "K:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find K!" << std::endl;
        return false;
    }
    file >> K;
    
    file >> word;
    if(word != "DistanceMethod:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find DistanceMethod!" << std::endl;
        return false;
    }
    file >> distanceMethod;
    
    file >> word;
    if(word != "SearchForBestKValue:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find SearchForBestKValue!" << std::endl;
        return false;
    }
    file >> searchForBestKValue;
    
    file >> word;
    if(word != "MinKSearchValue:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find MinKSearchValue!" << std::endl;
        return false;
    }
    file >> minKSearchValue;
    
    file >> word;
    if(word != "MaxKSearchValue:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find MaxKSearchValue!" << std::endl;
        return false;
    }
    file >> maxKSearchValue;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find UseScaling!" << std::endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find UseNullRejection!" << std::endl;
        return false;
    }
    file >> useNullRejection;
    
    file >> word;
    if(word != "NullRejectionCoeff:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find NullRejectionCoeff!" << std::endl;
        return false;
    }
    file >> nullRejectionCoeff;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize( numInputDimensions );
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find Ranges!" << std::endl;
            std::cout << "Word: " << word << std::endl;
            return false;
        }
        for(UINT n=0; n<ranges.size(); n++){
            file >> ranges[n].minValue;
            file >> ranges[n].maxValue;
        }
    }
    
    //Resize the buffers
    trainingMu.resize(numClasses,0);
    trainingSigma.resize(numClasses,0);
    
    file >> word;
    if(word != "TrainingMu:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find TrainingMu!" << std::endl;
        return false;
    }
    
    //Load the trainingMu data
    for(UINT j=0; j<numClasses; j++){
        file >> trainingMu[j];
    }
    
    file >> word;
    if(word != "TrainingSigma:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find TrainingSigma!" << std::endl;
        return false;
    }
    
    //Load the trainingSigma data
    for(UINT j=0; j<numClasses; j++){
        file >> trainingSigma[j];
    }
    
    file >> word;
    if(word != "NumTrainingSamples:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find NumTrainingSamples!" << std::endl;
        return false;
    }
    unsigned int numTrainingSamples = 0;
    file >> numTrainingSamples;
    
    file >> word;
    if(word != "TrainingData:"){
        errorLog << "loadLegacyModelFromFile(fstream &file) - Could not find TrainingData!" << std::endl;
        return false;
    }
    
    //Load the training data
    trainingData.setNumDimensions(numInputDimensions);
    unsigned int classLabel = 0;
    VectorFloat sample(numInputDimensions,0);
    for(UINT i=0; i<numTrainingSamples; i++){
        //Read the class label
        file >> classLabel;
        
        //Read the feature vector
        for(UINT j=0; j<numInputDimensions; j++){
            file >> sample[j];
        }
        
        //Add it to the training data
        trainingData.addSample(classLabel, sample);
    }
    
    //Flag that the model has been trained
    trained = true;
    
    //Compute the null rejection thresholds
    recomputeNullRejectionThresholds();
    
    return true;
}

GRT_END_NAMESPACE

