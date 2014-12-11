
#include "MovementDetector.h"

using namespace GRT;

MovementDetector::MovementDetector( const UINT numDimensions, const double upperThreshold, const double lowerThreshold, const double gamma, const UINT searchTimeout ) {
    classType = "MovementDetector";
    infoLog.setProceedingText("[MovementDetector]");
    debugLog.setProceedingText("[DEBUG MovementDetector]");
    errorLog.setProceedingText("[ERROR MovementDetector]");
    trainingLog.setProceedingText("[TRAINING MovementDetector]");
    warningLog.setProceedingText("[WARNING MovementDetector]");

    this->numInputDimensions = numDimensions;
    this->numOutputDimensions = 1;
    this->upperThreshold = upperThreshold;
    this->lowerThreshold = lowerThreshold;
    this->gamma = gamma;
    this->searchTimeout = searchTimeout;
    this->trained = true;
    
    reset();
}
    
MovementDetector::~MovementDetector(){
        
}
    
bool MovementDetector::predict_( VectorDouble &input ){
    
    movementDetected = false;
    noMovementDetected = false;
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &input) - AdaBoost Model Not Trained!" << endl;
        return false;
    }
    
    if( input.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &input) - The size of the input vector (" << input.size() << ") does not match the num features in the model (" << numInputDimensions << endl;
        return false;
    }
    
    //Compute the movement index, unless we are in the first sample
    double x = 0;
    if( !firstSample ){
        for(UINT n=0; n<numInputDimensions; n++){
            x += SQR( input[n] - lastSample[n] );
        }
        movementIndex = (movementIndex*gamma) + sqrt( x );
    }
    
    //Flag that this is not the first sample and store the input for the next prediction
    firstSample = false;
    lastSample = input;
    
    switch( state ){
        case SEARCHING_FOR_MOVEMENT:
            if( movementIndex >= upperThreshold ){
                movementDetected = true;
                state = SEARCHING_FOR_NO_MOVEMENT;
            }
            break;
        case SEARCHING_FOR_NO_MOVEMENT:
            if( movementIndex < lowerThreshold ){
                noMovementDetected = true;
                state = SEARCH_TIMEOUT;
                searchTimer.start();
            }
            break;
        case SEARCH_TIMEOUT:
            // searchTimeout is cast because of a C4018 warning on visual (signed/unsigned incompatibility)
            if( searchTimer.getMilliSeconds() >= (signed long)searchTimeout ){
                state = SEARCH_TIMEOUT;
                searchTimer.stop();
            }
            break;
    }
    
    
    return true;
}

bool MovementDetector::clear(){
    
    trained = false;
    lastSample.clear();
    
    reset();
    
    return true;
}

bool MovementDetector::reset(){
    
    state = SEARCH_TIMEOUT;
    searchTimer.start();
    firstSample = true;
    movementDetected = false;
    noMovementDetected = false;
    movementIndex = 0;
    
    return true;
}

bool MovementDetector::saveModelToFile(fstream &file) const{
    
    //Write the header info
    file << "GRT_MOVEMENT_DETECTOR_MODEL_FILE_V1.0\n";
    
    //Write the base settings to the file
    if( !MLBase::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save ML base settings to file!" << endl;
        return false;
    }
    
    //Write the detector variables
    file << "SearchTimeout: " << searchTimeout << endl;
    file << "UpperThreshold: " << upperThreshold << endl;
    file << "LowerThreshold: " << lowerThreshold << endl;
    file << "Gamma: " << gamma << endl;
    
    return true;
}

bool MovementDetector::loadModelFromFile(fstream &file){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model!" << endl;
        return false;
    }
    
    std::string word;
    file >> word;
    
    //Write the header info
    if( word != "GRT_MOVEMENT_DETECTOR_MODEL_FILE_V1.0" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read file header!" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if( word != "SearchTimeout:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read SearchTimeout header!" << endl;
        return false;
    }
    file >> searchTimeout;
    
    file >> word;
    if( word != "UpperThreshold:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read UpperThreshold header!" << endl;
        return false;
    }
    file >> upperThreshold;
    
    file >> word;
    if( word != "LowerThreshold:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read LowerThreshold header!" << endl;
        return false;
    }
    file >> lowerThreshold;
    
    file >> word;
    if( word != "Gamma:" ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to read Gamma header!" << endl;
        return false;
    }
    file >> gamma;
    
    return true;
}

double MovementDetector::getUpperThreshold() const {
    return upperThreshold;
}

double MovementDetector::getLowerThreshold() const {
    return lowerThreshold;
}

double MovementDetector::getMovementIndex() const {
    return movementIndex;
}

double MovementDetector::getGamma() const {
    return gamma;
}

bool MovementDetector::getMovementDetected() const {
    return movementDetected;
}

bool MovementDetector::getNoMovementDetect() const {
    return noMovementDetected;
}

UINT MovementDetector::getState() const {
    return state;
}

UINT MovementDetector::getSearchTimeout() const {
    return searchTimeout;
}

bool MovementDetector::setUpperThreshold(const double upperThreshold) {
    this->upperThreshold = upperThreshold;
    return true;
}

bool MovementDetector::setLowerThreshold(const double lowerThreshold) {
    this->lowerThreshold = lowerThreshold;
    return true;
}

bool MovementDetector::setGamma(const double gamma) {
    this->gamma = gamma;
    return true;
}

bool MovementDetector::setSearchTimeout(const UINT searchTimeout){
    this->searchTimeout = searchTimeout;
    return true;
}
