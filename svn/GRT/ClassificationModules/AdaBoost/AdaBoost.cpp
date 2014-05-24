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

#include "AdaBoost.h"

namespace GRT{

//Register the AdaBoost module with the Classifier base class
RegisterClassifierModule< AdaBoost > AdaBoost::registerModule("AdaBoost");

AdaBoost::AdaBoost(const WeakClassifier &weakClassifier,bool useScaling,bool useNullRejection,double nullRejectionCoeff,UINT numBoostingIterations,UINT predictionMethod)
{
    setWeakClassifier( weakClassifier );
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    this->nullRejectionCoeff = nullRejectionCoeff;
    this->numBoostingIterations = numBoostingIterations;
    this->predictionMethod = predictionMethod;
    classType = "AdaBoost";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG AdaBoost]");
    errorLog.setProceedingText("[ERROR AdaBoost]");
    trainingLog.setProceedingText("[TRAINING AdaBoost]");
    warningLog.setProceedingText("[WARNING AdaBoost]");
}
    
AdaBoost::AdaBoost(const AdaBoost &rhs){
    classifierType = "AdaBoost";
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG AdaBoost]");
    errorLog.setProceedingText("[ERROR AdaBoost]");
    trainingLog.setProceedingText("[TRAINING AdaBoost]");
    warningLog.setProceedingText("[WARNING AdaBoost]");
    *this = rhs;
}

AdaBoost::~AdaBoost(void)
{
    //Clean up any weak classifiers
    clearWeakClassifiers();
} 
    
AdaBoost& AdaBoost::operator=(const AdaBoost &rhs){
    if( this != &rhs ){
        //Clear the current weak classifiers
        clearWeakClassifiers();
        
        this->numBoostingIterations = rhs.numBoostingIterations;
        this->predictionMethod = rhs.predictionMethod;
        this->models = rhs.models;
        
        if( rhs.weakClassifiers.size() > 0 ){
            for(UINT i=0; i<rhs.weakClassifiers.size(); i++){
                WeakClassifier *weakClassiferPtr = rhs.weakClassifiers[i]->createNewInstance();
                weakClassifiers.push_back( weakClassiferPtr );
            }
        }
        
        //Clone the classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}
    
bool AdaBoost::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ){
        errorLog << "deepCopyFrom(const Classifier *classifier) - The classifier pointer is NULL!" << endl;
        return false;
    }
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        //Clone the AdaBoost values
        AdaBoost *ptr = (AdaBoost*)classifier;
        
        //Clear the current weak classifiers
        clearWeakClassifiers();
        
        this->numBoostingIterations = ptr->numBoostingIterations;
        this->predictionMethod = ptr->predictionMethod;
        this->models = ptr->models;
        
        if( ptr->weakClassifiers.size() > 0 ){
            for(UINT i=0; i<ptr->weakClassifiers.size(); i++){
                WeakClassifier *weakClassiferPtr = ptr->weakClassifiers[i]->createNewInstance();
                weakClassifiers.push_back( weakClassiferPtr );
            }
        }
        
        //Clone the classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}
    
bool AdaBoost::train_(ClassificationData &trainingData){
    
    //Clear any previous model
    clear();
    
    if( trainingData.getNumSamples() <= 1 ){
        errorLog << "train_(ClassificationData &trainingData) - There are not enough training samples to train a model! Number of samples: " << trainingData.getNumSamples()  << endl;
        return false;
    }
    
    numInputDimensions = trainingData.getNumDimensions();
    numClasses = trainingData.getNumClasses();
    const UINT M = trainingData.getNumSamples();
    const UINT POSITIVE_LABEL = WEAK_CLASSIFIER_POSITIVE_CLASS_LABEL;
    const UINT NEGATIVE_LABEL = WEAK_CLASSIFIER_NEGATIVE_CLASS_LABEL;
    double alpha = 0;
    const double beta = 0.001;
    double epsilon = 0;
    TrainingResult trainingResult;
    
    const UINT K = (UINT)weakClassifiers.size();
    if( K == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - No weakClassifiers have been set. You need to set at least one weak classifier first." << endl;
        return false;
    }

    classLabels.resize(numClasses);
    models.resize(numClasses);
    ranges = trainingData.getRanges();

    //Scale the training data if needed
    if( useScaling ){
        trainingData.scale(ranges,0,1);
    }
    
    //Create the weights vector
    VectorDouble weights(M);
    
    //Create the error matrix
    MatrixDouble errorMatrix(K,M);
    
    for(UINT classIter=0; classIter<numClasses; classIter++){
        
        //Get the class label for the current class
        classLabels[classIter] = trainingData.getClassLabels()[classIter];
        
        //Set the class label of the current model
        models[ classIter ].setClassLabel( classLabels[classIter] );
        
        //Setup the labels for this class, POSITIVE_LABEL == 1, NEGATIVE_LABEL == 2
        ClassificationData classData;
        classData.setNumDimensions(trainingData.getNumDimensions());
        for(UINT i=0; i<M; i++){
            UINT label = trainingData[i].getClassLabel()==classLabels[classIter] ? POSITIVE_LABEL : NEGATIVE_LABEL;
            VectorDouble trainingSample = trainingData[i].getSample();
            classData.addSample(label,trainingSample);
        }
        
        //Setup the initial training sample weights
        std::fill(weights.begin(),weights.end(),1.0/M);
        
        //Run the boosting loop
        bool keepBoosting = true;
        UINT t = 0;
        
        while( keepBoosting ){
            
            //Pick the classifier from the family of classifiers that minimizes the total error
            UINT bestClassifierIndex = 0;
            double minError = numeric_limits<double>::max();
            for(UINT k=0; k<K; k++){
                //Get the k'th possible classifier
                WeakClassifier *weakLearner = weakClassifiers[k];
                
                //Train the current classifier
                if( !weakLearner->train(classData,weights) ){
                    errorLog << "Failed to train weakLearner!" << endl;
                    return false;
                }
                
                //Compute the weighted error for this clasifier
                double e = 0;
                double positiveLabel = weakLearner->getPositiveClassLabel();
                double numCorrect = 0;
                double numIncorrect = 0;
                for(UINT i=0; i<M; i++){
                    //Only penalize errors
                    double prediction = weakLearner->predict( classData[i].getSample() );
                    
                    if( (prediction == positiveLabel && classData[i].getClassLabel() != POSITIVE_LABEL) ||        //False positive
                        (prediction != positiveLabel && classData[i].getClassLabel() == POSITIVE_LABEL) ){       //False negative
                        e += weights[i]; //Increase the error proportional to the weight of the example
                        errorMatrix[k][i] = 1; //Flag that there was an error
                        numIncorrect++;
                    }else{
                        errorMatrix[k][i] = 0; //Flag that there was no error
                        numCorrect++;
                    }
                }
                
                trainingLog << "PositiveClass: " << classLabels[classIter] << " Boosting Iter: " << t << " Classifier: " << k << " WeightedError: " << e << " NumCorrect: " << numCorrect/M << " NumIncorrect: " <<numIncorrect/M << endl;
                
                if( e < minError ){
                    minError = e;
                    bestClassifierIndex = k;
                }
                
            }
  
            epsilon = minError;
            
            //Set alpha, using the M1 weight value, small weights (close to 0) will receive a strong weight in the final classifier
            alpha = 0.5 * log( (1.0-epsilon)/epsilon );
            
            trainingLog << "PositiveClass: " << classLabels[classIter] << " Boosting Iter: " << t << " Best Classifier Index: " << bestClassifierIndex << " MinError: " << minError << " Alpha: " << alpha << endl;
            
            if( std::isinf(alpha) ){ keepBoosting = false; trainingLog << "Alpha is INF. Stopping boosting for current class" << endl; }
            if( 0.5 - epsilon <= beta ){ keepBoosting = false; trainingLog << "Epsilon <= Beta. Stopping boosting for current class" << endl; }
            if( ++t >= numBoostingIterations ) keepBoosting = false;

            trainingResult.setClassificationResult(t, minError, this);
            trainingResults.push_back(trainingResult);
            trainingResultsObserverManager.notifyObservers( trainingResult );
            
            if( keepBoosting ){
                
                //Add the best weak classifier to the committee
                models[ classIter ].addClassifierToCommitee( weakClassifiers[bestClassifierIndex], alpha );
                
                //Update the weights for the next boosting iteration
                double reWeight = (1.0 - epsilon) / epsilon;
                double oldSum = 0;
                double newSum = 0;
                for(UINT i=0; i<M; i++){
                    oldSum += weights[i];
                    //Only update the weights that resulted in an incorrect prediction
                    if( errorMatrix[bestClassifierIndex][i] == 1 ) weights[i] *= reWeight;
                    newSum += weights[i];
                }
                
                //Normalize all the weights
                //This results to increasing the weights of the samples that were incorrectly labelled
                //While decreasing the weights of the samples that were correctly classified
                reWeight = oldSum/newSum;
                for(UINT i=0; i<M; i++){
                    weights[i] *= reWeight;
                }
                
            }else{
                trainingLog << "Stopping boosting training at iteration : " << t-1 << " with an error of " << epsilon << endl;
                if( t-1 == 0 ){
                    //Add the best weak classifier to the committee (we have to add it as this is the first iteration)
                    if( std::isinf(alpha) ){ alpha = 1; } //If alpha is infinite then the first classifier got everything correct
                    models[ classIter ].addClassifierToCommitee( weakClassifiers[bestClassifierIndex], alpha );
                }
            }
            
        }
    }
    
    //Normalize the weights
    for(UINT k=0; k<numClasses; k++){
        models[k].normalizeWeights();
    }
    
    //Flag that the model has been trained
    trained = true;
    
    //Setup the data for prediction
    predictedClassLabel = 0;
    maxLikelihood = 0;
    classLikelihoods.resize(numClasses);
    classDistances.resize(numClasses);
    
    return true;
}
    
bool AdaBoost::predict_(VectorDouble &inputVector){
    
    predictedClassLabel = 0;
    maxLikelihood = -10000;
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - AdaBoost Model Not Trained!" << endl;
        return false;
    }
    
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
    
    UINT bestClassIndex = 0;
    UINT numPositivePredictions = 0;
    bestDistance = -numeric_limits<double>::max();
    double worstDistance = numeric_limits<double>::max();
    double sum = 0;
    for(UINT k=0; k<numClasses; k++){
        double result = models[k].predict( inputVector );
        
        switch ( predictionMethod ) {
            case MAX_POSITIVE_VALUE:
                if( result > 0 ){
                    if( result > bestDistance ){
                        bestDistance = result;
                        bestClassIndex = k;
                    }
                    numPositivePredictions++;
                    classLikelihoods[k] = result;
                }else classLikelihoods[k] = 0;
                
                classDistances[k] = result;
                sum += classLikelihoods[k];
                
                break;
            case MAX_VALUE:
                if( result > bestDistance ){
                    bestDistance = result;
                    bestClassIndex = k;
                }
                if( result < worstDistance ){
                    worstDistance = result;
                }
                numPositivePredictions++; //In the MAX_VALUE mode we assume all samples are valid
                classLikelihoods[k] = result;
                classDistances[k] = result;
                
                break;
            default:
                errorLog << "predict_(VectorDouble &inputVector) - Unknown prediction method!" << endl;
                break;
        }
    }
    
    if( predictionMethod == MAX_VALUE ){
        //Some of the class likelihoods might be negative, so we add the most negative value to each to offset this
        worstDistance = fabs( worstDistance );
        for(UINT k=0; k<numClasses; k++){
            classLikelihoods[k] += worstDistance;
            sum += classLikelihoods[k];
        }
    }
    
    //Normalize the class likelihoods
    if( sum > 0 ){
        for(UINT k=0; k<numClasses; k++)
            classLikelihoods[k] /= sum;
    }
    maxLikelihood = classLikelihoods[ bestClassIndex ];
    
    if( numPositivePredictions == 0 ){
        predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    }else predictedClassLabel = classLabels[ bestClassIndex ];
    
    return true;
}

bool AdaBoost::recomputeNullRejectionThresholds(){

    if( trained ){
        //Todo - need to add null rejection for AdaBoost
        return false;
    }
    return false;
}
    
bool AdaBoost::setNullRejectionCoeff(double nullRejectionCoeff){
    
    if( nullRejectionCoeff > 0 ){
        this->nullRejectionCoeff = nullRejectionCoeff;
        recomputeNullRejectionThresholds();
        return true;
    }
    return false;
}
    
bool AdaBoost::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file<<"GRT_ADABOOST_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    //Write the AdaBoost settings to the file
    file << "PredictionMethod: " << predictionMethod << endl;
    
    //If the model has been trained then write the model
    if( trained ){
        file << "Models: " << endl;
        for(UINT i=0; i<models.size(); i++){
            if( !models[i].saveModelToFile( file ) ){
                errorLog <<"saveModelToFile(fstream &file) - Failed to write model " << i << " to file!" << endl;
                file.close();
                return false;
            }
        }
    }
    
    return true;
}
    
bool AdaBoost::loadModelFromFile(fstream &file){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model!" << endl;
        return false;
    }
    
    std::string word;
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_ADABOOST_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    if( word != "GRT_ADABOOST_MODEL_FILE_V2.0" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read file header!" << endl;
        errorLog << word << endl;
		return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if( word != "PredictionMethod:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read PredictionMethod header!" << endl;
		return false;
    }
    file >> predictionMethod;
    
    if( trained ){
        file >> word;
        if( word != "Models:" ){
            errorLog <<"loadModelFromFile(fstream &file) - Failed to read Models header!" << endl;
            return false;
        }
        
        //Load the models
        models.resize( numClasses );
        for(UINT i=0; i<models.size(); i++){
            if( !models[i].loadModelFromFile( file ) ){
                errorLog << "loadModelFromFile(fstream &file) - Failed to load model " << i << " from file!" << endl;
                file.close();
                return false;
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
    
bool AdaBoost::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the AdaBoost model
    models.clear();
    
    return true;
}
    
bool AdaBoost::setWeakClassifier(const WeakClassifier &weakClassifer){
    
    //Clear any previous weak classifers
    clearWeakClassifiers();
    
    WeakClassifier *weakClassiferPtr = weakClassifer.createNewInstance();
    
    weakClassifiers.push_back( weakClassiferPtr );
    
    return true;
}
    
bool AdaBoost::addWeakClassifier(const WeakClassifier &weakClassifer){
    
    WeakClassifier *weakClassiferPtr = weakClassifer.createNewInstance();
    weakClassifiers.push_back( weakClassiferPtr );
    
    return true;
}

bool AdaBoost::clearWeakClassifiers(){
    
    for(UINT i=0; i<weakClassifiers.size(); i++){
        if( weakClassifiers[i] != NULL ){
            delete weakClassifiers[i];
            weakClassifiers[i] = NULL;
        }
    }
    weakClassifiers.clear();
    return true;
}

bool AdaBoost::setNumBoostingIterations(UINT numBoostingIterations){
    if( numBoostingIterations > 0 ){
        this->numBoostingIterations = numBoostingIterations;
        return true;
    }
    return false;
}

bool AdaBoost::setPredictionMethod(UINT predictionMethod){
    if( predictionMethod != MAX_POSITIVE_VALUE && predictionMethod != MAX_VALUE ){
        return false;
    }
    this->predictionMethod = predictionMethod;
    return true;
}
 
void AdaBoost::printModel(){
    
    cout <<"AdaBoostModel: \n";
    cout<<"NumFeatures: " << numInputDimensions << endl;
    cout<<"NumClasses: " << numClasses << endl;
    cout <<"UseScaling: " << useScaling << endl;
    cout<<"UseNullRejection: " << useNullRejection << endl;
    
    for(UINT k=0; k<numClasses; k++){
        cout << "Class: " << k+1 << " ClassLabel: " << classLabels[k] << endl;
        models[k].print();
    }
    
}
    
bool AdaBoost::loadLegacyModelFromFile( fstream &file ){
    
    string word;
    
    file >> word;
    if( word != "NumFeatures:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read NumFeatures header!" << endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if( word != "NumClasses:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read NumClasses header!" << endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if( word != "UseScaling:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read UseScaling header!" << endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if( word != "UseNullRejection:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read UseNullRejection header!" << endl;
        return false;
    }
    file >> useNullRejection;
    
    if( useScaling ){
        file >> word;
        if( word != "Ranges:" ){
            errorLog <<"loadModelFromFile(fstream &file) - Failed to read Ranges header!" << endl;
            return false;
        }
        ranges.resize( numInputDimensions );
        
        for(UINT n=0; n<ranges.size(); n++){
            file >> ranges[n].minValue;
            file >> ranges[n].maxValue;
        }
    }
    
    file >> word;
    if( word != "Trained:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read Trained header!" << endl;
        return false;
    }
    file >> trained;
    
    file >> word;
    if( word != "PredictionMethod:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read PredictionMethod header!" << endl;
        return false;
    }
    file >> predictionMethod;
    
    if( trained ){
        file >> word;
        if( word != "Models:" ){
            errorLog <<"loadModelFromFile(fstream &file) - Failed to read Models header!" << endl;
            return false;
        }
        
        //Load the models
        models.resize( numClasses );
        classLabels.resize( numClasses );
        for(UINT i=0; i<models.size(); i++){
            if( !models[i].loadModelFromFile( file ) ){
                errorLog << "loadModelFromFile(fstream &file) - Failed to load model " << i << " from file!" << endl;
                file.close();
                return false;
            }
            
            //Set the class label
            classLabels[i] = models[i].getClassLabel();
        }
    }
    
    //Recompute the null rejection thresholds
    recomputeNullRejectionThresholds();
    
    //Resize the prediction results to make sure it is setup for realtime prediction
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
    classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    
    return true;
}

} //End of namespace GRT

