
#define GRT_DLL_EXPORTS
#include "MovementDetector.h"

GRT_BEGIN_NAMESPACE

MovementDetector::MovementDetector( const UINT numDimensions, const Float upperThreshold, const Float lowerThreshold, const Float gamma, const UINT searchTimeout ) : MLBase("MovementDetector")
{    
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

bool MovementDetector::predict_( VectorFloat &input ){
    
    movementDetected = false;
    noMovementDetected = false;
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &input) - AdaBoost Model Not Trained!" << std::endl;
        return false;
    }
    
    if( input.size() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &input) - The size of the input vector (" << input.size() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    //Compute the movement index, unless we are in the first sample
    Float x = 0;
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

bool MovementDetector::save( std::fstream &file ) const{
    
    //Write the header info
    file << "GRT_MOVEMENT_DETECTOR_MODEL_FILE_V1.0\n";
    
    //Write the base settings to the file
    if( !MLBase::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save ML base settings to file!" << std::endl;
        return false;
    }
    
    //Write the detector variables
    file << "SearchTimeout: " << searchTimeout << std::endl;
    file << "UpperThreshold: " << upperThreshold << std::endl;
    file << "LowerThreshold: " << lowerThreshold << std::endl;
    file << "Gamma: " << gamma << std::endl;
    
    return true;
}

bool MovementDetector::load( std::fstream &file ){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "load(string filename) - Could not open file to load model!" << std::endl;
        return false;
    }
    
    std::string word;
    file >> word;
    
    //Write the header info
    if( word != "GRT_MOVEMENT_DETECTOR_MODEL_FILE_V1.0" ){
        errorLog <<"load(fstream &file) - Failed to read file header!" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if( word != "SearchTimeout:" ){
        errorLog <<"load(fstream &file) - Failed to read SearchTimeout header!" << std::endl;
        return false;
    }
    file >> searchTimeout;
    
    file >> word;
    if( word != "UpperThreshold:" ){
        errorLog <<"load(fstream &file) - Failed to read UpperThreshold header!" << std::endl;
        return false;
    }
    file >> upperThreshold;
    
    file >> word;
    if( word != "LowerThreshold:" ){
        errorLog <<"load(fstream &file) - Failed to read LowerThreshold header!" << std::endl;
        return false;
    }
    file >> lowerThreshold;
    
    file >> word;
    if( word != "Gamma:" ){
        errorLog <<"load(fstream &file) - Failed to read Gamma header!" << std::endl;
        return false;
    }
    file >> gamma;
    
    return true;
}

Float MovementDetector::getUpperThreshold() const {
    return upperThreshold;
}

Float MovementDetector::getLowerThreshold() const {
    return lowerThreshold;
}

Float MovementDetector::getMovementIndex() const {
    return movementIndex;
}

Float MovementDetector::getGamma() const {
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

bool MovementDetector::setUpperThreshold(const Float upperThreshold) {
    this->upperThreshold = upperThreshold;
    return true;
}

bool MovementDetector::setLowerThreshold(const Float lowerThreshold) {
    this->lowerThreshold = lowerThreshold;
    return true;
}

bool MovementDetector::setGamma(const Float gamma) {
    this->gamma = gamma;
    return true;
}

bool MovementDetector::setSearchTimeout(const UINT searchTimeout){
    this->searchTimeout = searchTimeout;
    return true;
}

GRT_END_NAMESPACE
