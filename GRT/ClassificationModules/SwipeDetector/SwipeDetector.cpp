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

#include "SwipeDetector.h"

namespace GRT{

//Register the SwipeDetector module with the Classifier base class
RegisterClassifierModule< SwipeDetector > SwipeDetector::registerModule("SwipeDetector");

SwipeDetector::SwipeDetector(const unsigned int swipeIndex, const unsigned int swipeThreshold,
                                   const unsigned int hysteresisThreshold, const unsigned int swipeDirection,bool useScaling)
{
    this->swipeIndex = swipeIndex;
    this->swipeThreshold = swipeThreshold;
    this->hysteresisThreshold = hysteresisThreshold;
    this->swipeDirection = swipeDirection;
    this->useScaling = useScaling;
    
    supportsNullRejection = false;
    contextInput = true;
    classType = "SwipeDetector";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG SwipeDetector]");
    errorLog.setProceedingText("[ERROR SwipeDetector]");
    trainingLog.setProceedingText("[TRAINING SwipeDetector]");
    warningLog.setProceedingText("[WARNING SwipeDetector]");
    
    swipeIntegrationCoeff = 0.92;
    movementIntegrationCoeff = 0.90;
    movementThreshold = 3000;
    contextFilterSize = 5;
    
    reset();
}
    
SwipeDetector::SwipeDetector(const SwipeDetector &rhs){
    supportsNullRejection = false;
    classType = "SwipeDetector";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG SwipeDetector]");
    errorLog.setProceedingText("[ERROR SwipeDetector]");
    trainingLog.setProceedingText("[TRAINING SwipeDetector]");
    warningLog.setProceedingText("[WARNING SwipeDetector]");
    
    swipeIntegrationCoeff = 0.92;
    movementIntegrationCoeff = 0.90;
    movementThreshold = 3000;
    contextFilterSize = 5;
    
    *this = rhs;
}

SwipeDetector::~SwipeDetector(void)
{
}
    
SwipeDetector& SwipeDetector::operator=(const SwipeDetector &rhs){
    if( this != &rhs ){
        //SwipeDetector variables
        this->firstSample = rhs.firstSample;
        this->swipeDetected = rhs.swipeDetected;
        this->contextInput = rhs.contextInput;
        this->swipeIndex = rhs.swipeIndex;
        this->swipeDirection = rhs.swipeDirection;
        this->contextFilterSize = rhs.contextFilterSize;
        this->swipeIntegrationCoeff = rhs.swipeIntegrationCoeff;
        this->movementIntegrationCoeff = rhs.movementIntegrationCoeff;
        this->swipeThreshold = rhs.swipeThreshold;
        this->hysteresisThreshold = rhs.hysteresisThreshold;
        this->swipeVelocity = rhs.swipeVelocity;
        this->movementVelocity = rhs.movementVelocity;
        this->movementThreshold = rhs.movementThreshold;
        this->contextFilteredValue = rhs.contextFilteredValue;
        this->lastX = rhs.lastX;
        this->thresholdDetector = rhs.thresholdDetector;
        this->contextFilter = rhs.contextFilter;
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}
    
bool SwipeDetector::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        
        SwipeDetector *ptr = (SwipeDetector*)classifier;
        
        this->firstSample = ptr->firstSample;
        this->swipeDetected = ptr->swipeDetected;
        this->contextInput = ptr->contextInput;
        this->swipeIndex = ptr->swipeIndex;
        this->swipeDirection = ptr->swipeDirection;
        this->contextFilterSize = ptr->contextFilterSize;
        this->swipeIntegrationCoeff = ptr->swipeIntegrationCoeff;
        this->movementIntegrationCoeff = ptr->movementIntegrationCoeff;
        this->swipeThreshold = ptr->swipeThreshold;
        this->hysteresisThreshold = ptr->hysteresisThreshold;
        this->swipeVelocity = ptr->swipeVelocity;
        this->movementVelocity = ptr->movementVelocity;
        this->movementThreshold = ptr->movementThreshold;
        this->contextFilteredValue = ptr->contextFilteredValue;
        this->lastX = ptr->lastX;
        this->thresholdDetector = ptr->thresholdDetector;
        this->contextFilter = ptr->contextFilter;
        
        //Classifier variables
        return copyBaseVariables( classifier );
    }
    
    return false;
}
    
bool SwipeDetector::init(const unsigned int numInputDimensions){
    
    //Clear any previous models
    clear();
    
    this->numInputDimensions = numInputDimensions;
    numClasses = 2; //This is always 2 for swipe detection [1 == swipe detected, everything else means no swipe detected]
    classLabels.resize( 2 );
    classLabels[0] = 1; //Swipe
    classLabels[1] = 2; //No Swipe
    nullRejectionThresholds.resize(2,0);
    
    //We currently have no way to automatically train the swipe detection, user needs to manually set thresholds, so just flag the model is trained
    trained = true;
    
    return true;
}
    
bool SwipeDetector::train_(ClassificationData &trainingData){
    
    //Clear any previous models
    clear();
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumDimensions();
    
    if( M == 0 ){
        errorLog << "train_(trainingData &labelledTrainingData) - Training data has zero samples!" << endl;
        return false;
    }
    
    numInputDimensions = N;
    numClasses = 2; //This is always 2 for swipe detection [1 == swipe detected, everything else means no swipe detected]
    classLabels.resize( 2 );
    classLabels[0] = 1; //Swipe
    classLabels[1] = 2; //No Swipe
    nullRejectionThresholds.resize(2,0);
    ranges = trainingData.getRanges();
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }
    
    //We currently have no way to automatically train the swipe detection, user needs to manually set thresholds, so just flag the model is trained
    trained = true;
    
    return true;
}
    

bool SwipeDetector::predict_(VectorDouble &inputVector){
    
    predictedClassLabel = 0;
	maxLikelihood = 0;
    swipeDetected = false;
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - SwipeDetector Model Not Trained!" << endl;
        return false;
    }
    
	if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << ")" << endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    //Compute the integrated movement velocity for the none-swipe dimensions
    if( !firstSample ){
        movementVelocity *= movementIntegrationCoeff;
        for(unsigned int i=0; i<numInputDimensions; i++){
            if( i != swipeIndex )
                movementVelocity += GRT::SQR(inputVector[i]-lastX[i]);
        }
    }else firstSample = false;
    lastX = inputVector;
    
    //Compute the integrated swipe velocity and update the threshold detector
    swipeVelocity = (swipeVelocity*swipeIntegrationCoeff) + inputVector[swipeIndex];
    thresholdDetector.update( swipeVelocity );
    
    //Filter the context
    contextFilteredValue = contextFilter.filter( contextInput ? 1 : 0 );
    
    //Check if a swipe has been detected
    swipeDetected = thresholdDetector.getThresholdCrossingDetected() && movementVelocity < movementThreshold && contextFilteredValue == 1;

    if( swipeDetected ){
        predictedClassLabel = 1;
        classLikelihoods[0] = 1.0;
        classDistances[1] = 0;
    }else{
        predictedClassLabel = 2;
        classLikelihoods[0] = 0.0;
        classDistances[1] = 1;
    }
    
    return true;
}
    
bool SwipeDetector::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    reset();
    
    return true;
}
    
bool SwipeDetector::reset(){
    
    movementVelocity = 0;
    lastX.clear();
    lastX.resize( 3, 0 );
    firstSample = true;
    swipeDetected = false;
    contextInput = true;
    
    if( swipeDirection == POSITIVE_SWIPE ){
        thresholdDetector.setThresholdCrossingMode( GRT::ThresholdCrossingDetector::UPPER_THRESHOLD_CROSSING );
        thresholdDetector.setUpperThreshold( swipeThreshold );
    }
    if( swipeDirection == NEGATIVE_SWIPE ){
        thresholdDetector.setThresholdCrossingMode( GRT::ThresholdCrossingDetector::LOWER_THRESHOLD_CROSSING );
        thresholdDetector.setLowerThreshold( swipeThreshold );
    }
    
    thresholdDetector.setAnalysisMode( GRT::ThresholdCrossingDetector::DERIVATIVE_ANALYSIS_MODE );
    thresholdDetector.setDetectionTimeoutMode( GRT::ThresholdCrossingDetector::HYSTERESIS_THRESHOLD );
    thresholdDetector.setHysteresisThreshold( hysteresisThreshold );
    
    contextFilter.init( contextFilterSize, 1 );
    
    return true;
}
    
bool SwipeDetector::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file<<"GRT_SWIPE_DETECTION_MODEL_FILE_V1.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    if( trained ){
    
        file << "SwipeIndex: " << swipeIndex << endl;
        file << "ContextFilterSize: " << contextFilterSize << endl;
        file << "SwipeIntegrationCoeff: " << swipeIntegrationCoeff << endl;
        file << "MovementIntegrationCoeff: " << movementIntegrationCoeff << endl;
        file << "SwipeThreshold: " << swipeThreshold << endl;
        
        file << "HysteresisThreshold: " << hysteresisThreshold << endl;
        file << "SwipeThreshold: " << swipeThreshold << endl;
        file << "MovementThreshold: " << movementThreshold << endl;
        file << "SwipeThreshold: " << swipeThreshold << endl;
        
        //TODO - need to save the thresholdDetector
        
    }
    
    return true;
}
    
bool SwipeDetector::loadModelFromFile(fstream &file){
    
    trained = false;
    numInputDimensions = 0;
    numClasses = 0;
    classLabels.clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << endl;
        return false;
    }
    
    std::string word;
    
    file >> word;

    //Find the file type header
    if(word != "GRT_SWIPE_DETECTION_MODEL_FILE_V1.0"){
        errorLog << "loadModelFromFile(string filename) - Could not find Model File Header" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    if( trained ){
        
        file >> word;
        if( word != "SwipeIndex:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the SwipeIndex!" << endl;
            return false;
        }
        file >> swipeIndex;
        
        file >> word;
        if( word != "ContextFilterSize:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the ContextFilterSize!" << endl;
            return false;
        }
        file >> contextFilterSize;
        
        file >> word;
        if( word != "SwipeIntegrationCoeff:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the SwipeIntegrationCoeff!" << endl;
            return false;
        }
        file >> swipeIntegrationCoeff;
        
        file >> word;
        if( word != "MovementIntegrationCoeff:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the MovementIntegrationCoeff!" << endl;
            return false;
        }
        file >> movementIntegrationCoeff;
        
        file >> word;
        if( word != "SwipeThreshold:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the SwipeThreshold!" << endl;
            return false;
        }
        file >> swipeThreshold;
        
        file >> word;
        if( word != "HysteresisThreshold:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the HysteresisThreshold!" << endl;
            return false;
        }
        file >> hysteresisThreshold;
        
        file >> word;
        if( word != "SwipeThreshold:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the SwipeThreshold!" << endl;
            return false;
        }
        file >> swipeThreshold;
        
        file >> word;
        if( word != "MovementThreshold:" ){
            errorLog << "loadModelFromFile(string filename) - Could not load the MovementThreshold!" << endl;
            return false;
        }
        file >> movementThreshold;
        
        //Resize the prediction results to make sure it is setup for realtime prediction
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
        
        reset();
    }
    
    return true;
}
    
    
bool SwipeDetector::getSwipeDetected() const {
    return swipeDetected;
}

double SwipeDetector::getSwipeValue() const {
    return thresholdDetector.getAnalysisValue();
}

double SwipeDetector::getSwipeThreshold() const {
    return swipeThreshold;
}
    
double SwipeDetector::getHysteresisThreshold() const{
    return hysteresisThreshold;
}

double SwipeDetector::getMovementVelocity() const {
    return movementVelocity;
}

double SwipeDetector::getMovementThreshold() const {
    return movementThreshold;
}

double SwipeDetector::getContextValue() const {
    return contextFilteredValue;
}

double SwipeDetector::getSwipeIntegrationCoeff() const{
    return swipeIntegrationCoeff;
}
    
bool SwipeDetector::setContext(const bool context){
    this->contextInput = context;
    return true;
}

bool SwipeDetector::setSwipeIndex(const unsigned int swipeIndex){
    this->swipeIndex = swipeIndex;
    reset();
    return true;
}

bool SwipeDetector::setSwipeDirection(const unsigned int swipeDirection){
    
    if( swipeDirection != POSITIVE_SWIPE && swipeDirection != NEGATIVE_SWIPE ){
        errorLog << "setSwipeDirection(const unsigned int swipeDirection) - Unknown swipeDirection!" << endl;
        return false;
    }
    
    this->swipeDirection = swipeDirection;
    reset();
    
    return true;
}

bool SwipeDetector::setSwipeThreshold(const double swipeThreshold){
    this->swipeThreshold = swipeThreshold;
    reset();
    return true;
}

bool SwipeDetector::setHysteresisThreshold(const double hysteresisThreshold){
    this->hysteresisThreshold = hysteresisThreshold;
    reset();
    return true;
}

bool SwipeDetector::setMovementThreshold(const double movementThreshold){
    this->movementThreshold = movementThreshold;
    reset();
    return true;
}

bool SwipeDetector::setSwipeIntegrationCoeff(const double swipeIntegrationCoeff){
    this->swipeIntegrationCoeff = swipeIntegrationCoeff;
    reset();
    return true;
}

} //End of namespace GRT

