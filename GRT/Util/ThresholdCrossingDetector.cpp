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
#include "ThresholdCrossingDetector.h"

GRT_BEGIN_NAMESPACE
    
ThresholdCrossingDetector::ThresholdCrossingDetector(UINT analysisMode,UINT thresholdCrossingMode,UINT detectionTimeoutMode,Float lowerThreshold,Float upperThreshold,Float hysteresisThreshold,UINT searchWindowSize,UINT searchTimeoutDuration,UINT offsetFilterSize){
    
    this->analysisMode = analysisMode;
    this->thresholdCrossingMode = thresholdCrossingMode;
    this->detectionTimeoutMode = detectionTimeoutMode;
    this->searchWindowSize = searchWindowSize;
    this->searchTimeoutDuration = searchTimeoutDuration;
    this->offsetFilterSize = offsetFilterSize;
    this->lowerThreshold = lowerThreshold;
    this->upperThreshold = upperThreshold;
    this->hysteresisThreshold = hysteresisThreshold;
    
    enableSearch = false;
    thresholdCrossingDetected = false;
    analysisValue = 0;
    searchWindowIndex = 0;
    currentSearchState = SEARCHING_FOR_FIRST_THRESHOLD_CROSSING;
    
    reset();
}
    
ThresholdCrossingDetector::ThresholdCrossingDetector(const ThresholdCrossingDetector &rhs){
    this->analysisValue = rhs.analysisValue;
    this->lowerThreshold = rhs.lowerThreshold;
    this->upperThreshold = rhs.upperThreshold;
    this->hysteresisThreshold = rhs.hysteresisThreshold;
    this->enableSearch = rhs.enableSearch;
    this->thresholdCrossingDetected = rhs.thresholdCrossingDetected;
    this->analysisMode = rhs.analysisMode;
    this->thresholdCrossingMode = rhs.thresholdCrossingMode;
    this->searchTimeoutCounter = rhs.searchTimeoutCounter;
    this->searchTimeoutDuration = rhs.searchTimeoutDuration;
    this->searchWindowSize = rhs.searchWindowSize;
    this->searchWindowIndex = rhs.searchWindowIndex;
    this->offsetFilterSize = rhs.offsetFilterSize;
    this->currentSearchState = rhs.currentSearchState;
    this->movingAverageFilter = rhs.movingAverageFilter;
    this->derivative = rhs.derivative;
}

ThresholdCrossingDetector::~ThresholdCrossingDetector(){

}
    
ThresholdCrossingDetector& ThresholdCrossingDetector::operator=(const ThresholdCrossingDetector &rhs){
    if(this!=&rhs){
        this->analysisValue = rhs.analysisValue;
        this->lowerThreshold = rhs.lowerThreshold;
        this->upperThreshold = rhs.upperThreshold;
        this->hysteresisThreshold = rhs.hysteresisThreshold;
        this->enableSearch = rhs.enableSearch;
        this->thresholdCrossingDetected = rhs.thresholdCrossingDetected;
        this->analysisMode = rhs.analysisMode;
        this->thresholdCrossingMode = rhs.thresholdCrossingMode;
        this->searchTimeoutCounter = rhs.searchTimeoutCounter;
        this->searchTimeoutDuration = rhs.searchTimeoutDuration;
        this->searchWindowSize = rhs.searchWindowSize;
        this->searchWindowIndex = rhs.searchWindowIndex;
        this->offsetFilterSize = rhs.offsetFilterSize;
        this->currentSearchState = rhs.currentSearchState;
        this->movingAverageFilter = rhs.movingAverageFilter;
        this->derivative = rhs.derivative;
    }
    return *this;
}

bool ThresholdCrossingDetector::update( const Float x ){
	
    thresholdCrossingDetected = false;
    
    Float deriv = derivative.computeDerivative( x );
    Float offset = movingAverageFilter.filter( x );
    
    //If the search is disabled then we stop here
    if( !enableSearch ){ return thresholdCrossingDetected; }
    
    //Perform the search
    analysisValue = 0;
    bool upperThresholdCrossingFound = false;
    bool lowerThresholdCrossingFound = false;
    
    switch( analysisMode ){
        case RAW_DATA_ANALYSIS_MODE:
            analysisValue = x;
            break;
        case MOVING_OFFSET_ANALYSIS_MODE:
            analysisValue = x - offset;
            break;
        case DERIVATIVE_ANALYSIS_MODE:
            analysisValue = deriv;
            break;
    }
    
    //Check to see if we should start searching again
    if( currentSearchState == NO_SEARCH_GATE_TIME_OUT ){
        
        switch( detectionTimeoutMode ){
            case TIMEOUT_COUNTER:
                // searchTimeoutDuration is cast because of a C4018 warning on visual (signed/unsigned incompatibility)
                if (searchTimeoutCounter.getMilliSeconds() >= (signed long)searchTimeoutDuration){
                    currentSearchState = SEARCHING_FOR_FIRST_THRESHOLD_CROSSING;
                    searchTimeoutCounter.stop();
                }else return false;
                break;
            case HYSTERESIS_THRESHOLD:
                switch ( thresholdCrossingMode ) {
                    case UPPER_THRESHOLD_CROSSING:
                        if( analysisValue <= hysteresisThreshold ){
                            currentSearchState = SEARCHING_FOR_FIRST_THRESHOLD_CROSSING;
                        }else return false;
                        break;
                    case LOWER_THRESHOLD_CROSSING:
                        if( analysisValue >= hysteresisThreshold ){
                            currentSearchState = SEARCHING_FOR_FIRST_THRESHOLD_CROSSING;
                        }else return false;
                        break;
                    default:
                        break;
                }
                
                break;
        }
        
    }
    
    if( thresholdCrossingMode == UPPER_THRESHOLD_CROSSING || thresholdCrossingMode == LOWER_THRESHOLD_CROSSING || thresholdCrossingMode == UPPER_OR_LOWER_THRESHOLD_CROSSING ){
        //Has the deriv of the input passed either threshold
        switch ( thresholdCrossingMode ) {
            case UPPER_THRESHOLD_CROSSING:
                if( analysisValue >= upperThreshold ){
                    upperThresholdCrossingFound = true;
                    thresholdCrossingDetected = true;
                    currentSearchState = NO_SEARCH_GATE_TIME_OUT;
                    searchTimeoutCounter.start();
                }
                break;
            case LOWER_THRESHOLD_CROSSING:
                if( analysisValue <= lowerThreshold ){
                    lowerThresholdCrossingFound = true;
                    thresholdCrossingDetected = true;
                    currentSearchState = NO_SEARCH_GATE_TIME_OUT;
                    searchTimeoutCounter.start();
                }
                break;
            case UPPER_OR_LOWER_THRESHOLD_CROSSING:
                if( analysisValue >= upperThreshold ){
                    upperThresholdCrossingFound = true;
                    thresholdCrossingDetected = true;
                    currentSearchState = NO_SEARCH_GATE_TIME_OUT;
                    searchTimeoutCounter.start();
                }else if( analysisValue <= lowerThreshold ){
                    lowerThresholdCrossingFound = true;
                    thresholdCrossingDetected = true;
                    currentSearchState = NO_SEARCH_GATE_TIME_OUT;
                    searchTimeoutCounter.start();
                }
                break;
            default:
                break;
        }
        return thresholdCrossingDetected ? true : false;
    }else{
        
        //Check to make sure the search has not timed out
        if( currentSearchState == SEARCHING_FOR_SECOND_THRESHOLD_CROSSING ){
            if( ++searchWindowIndex == searchWindowSize ){
                currentSearchState = SEARCHING_FOR_FIRST_THRESHOLD_CROSSING;
                searchWindowIndex = 0;
            }
        }
        
        //If we are in the NO_SEARCH_GATE_TIME_OUT state then we need to update the counter
        if( currentSearchState == NO_SEARCH_GATE_TIME_OUT ){
            // searchTimeoutDuration is cast because of a C4018 warning on visual (signed/unsigned incompatibility)
            if (searchTimeoutCounter.getMilliSeconds() >= (signed long)searchTimeoutDuration){
                currentSearchState = SEARCHING_FOR_FIRST_THRESHOLD_CROSSING;
                searchTimeoutCounter.stop();
            }
        }
        
        //Has the deriv of the input passed either threshold
        if( analysisValue >= upperThreshold ){ upperThresholdCrossingFound = true; }
        if( analysisValue <= lowerThreshold ){ lowerThresholdCrossingFound = true; }
        if( !upperThresholdCrossingFound && !lowerThresholdCrossingFound && 
           currentSearchState != NO_SEARCH_GATE_TIME_OUT ){ return false; }
        
        switch ( currentSearchState ) {
            case SEARCHING_FOR_FIRST_THRESHOLD_CROSSING:
                if( thresholdCrossingMode == UPPER_THEN_LOWER_THRESHOLD_CROSSING && upperThresholdCrossingFound ){
                    searchWindowIndex = 0;
                    currentSearchState = SEARCHING_FOR_SECOND_THRESHOLD_CROSSING;
                    return false;
                }
                if( thresholdCrossingMode == LOWER_THEN_UPPER_THRESHOLD_CROSSING && lowerThresholdCrossingFound ){
                    searchWindowIndex = 0;
                    currentSearchState = SEARCHING_FOR_SECOND_THRESHOLD_CROSSING;
                    return false;
                }
                break;
            case SEARCHING_FOR_SECOND_THRESHOLD_CROSSING:
                if( thresholdCrossingMode == UPPER_THEN_LOWER_THRESHOLD_CROSSING && lowerThresholdCrossingFound ){
                    currentSearchState = NO_SEARCH_GATE_TIME_OUT;
                    searchWindowIndex = 0;
                    searchTimeoutCounter.start();
                    thresholdCrossingDetected = true;
                    break;
                }
                if( thresholdCrossingMode == LOWER_THEN_UPPER_THRESHOLD_CROSSING && upperThresholdCrossingFound ){
                    currentSearchState = NO_SEARCH_GATE_TIME_OUT;
                    searchWindowIndex = 0;
                    searchTimeoutCounter.start();
                    thresholdCrossingDetected = true;
                    break;
                }
                break;
            default:
                break;
        }
    }
    
	return thresholdCrossingDetected ? true : false;
}
    
bool ThresholdCrossingDetector::reset(){
    
    //Reset the derivative
    derivative.init(Derivative::FIRST_DERIVATIVE, 1, 1, true, 5);
    
    movingAverageFilter.init(offsetFilterSize, 1);
    
    //Set the search state
    currentSearchState = NO_SEARCH_GATE_TIME_OUT;
    
    //Enable future searches
    enableSearch = true;
    thresholdCrossingDetected = false;
    analysisValue = 0;
    searchWindowIndex = 0;
    searchTimeoutCounter.stop();
    
    return true;
}
    
bool ThresholdCrossingDetector::triggerSearchTimeout(){
    currentSearchState = NO_SEARCH_GATE_TIME_OUT;
    searchTimeoutCounter.start();
    return true;
}
    
bool ThresholdCrossingDetector::getThresholdCrossingDetected() const{
    return thresholdCrossingDetected;
}
    
bool ThresholdCrossingDetector::getEnableSearch() const{
    return enableSearch;
}
    
UINT ThresholdCrossingDetector::getAnalysisMode() const{
    return analysisMode;
}

UINT ThresholdCrossingDetector::getThresholdCrossingMode() const{
    return thresholdCrossingMode;
}
    
UINT ThresholdCrossingDetector::getSearchWindowSize() const{
    return searchWindowSize;
}
        
UINT ThresholdCrossingDetector::getOffsetFilterSize() const{
    return offsetFilterSize;
}

UINT ThresholdCrossingDetector::getSearchWindowIndex() const{
    return searchWindowIndex;
}

UINT ThresholdCrossingDetector::getSearchTimeoutCounter() {
    return (UINT)searchTimeoutCounter.getMilliSeconds();
}

UINT ThresholdCrossingDetector::getSearchTimeoutDuration() const{
    return searchTimeoutDuration;
}
    
Float ThresholdCrossingDetector::getAnalysisValue() const{
    return analysisValue;
}
    
Float ThresholdCrossingDetector::getUpperThreshold() const{
    return upperThreshold;
}

Float ThresholdCrossingDetector::getLowerThreshold() const{
    return lowerThreshold;
}

Float ThresholdCrossingDetector::getHysteresisThreshold() const{
    return hysteresisThreshold;
}
    
bool ThresholdCrossingDetector::setEnableSearch(const bool enableSearch){
    this->enableSearch = enableSearch;
    return true;
}
    
bool ThresholdCrossingDetector::setAnalysisMode(const UINT analysisMode){
    this->analysisMode = analysisMode;
    return reset();
}
    
bool ThresholdCrossingDetector::setThresholdCrossingMode(const UINT thresholdCrossingMode){
    this->thresholdCrossingMode = thresholdCrossingMode; 
    return reset();
}
    
bool ThresholdCrossingDetector::setDetectionTimeoutMode(const UINT detectionTimeoutMode){
    this->detectionTimeoutMode = detectionTimeoutMode;
    return reset();
}
    
bool ThresholdCrossingDetector::setSearchWindowSize(const UINT searchWindowSize){
    this->searchWindowSize = searchWindowSize;
    return reset();
}
    
bool ThresholdCrossingDetector::setOffsetFilterSize(const UINT offsetFilterSize){
    this->offsetFilterSize = offsetFilterSize;
    return reset();
}
    
bool ThresholdCrossingDetector::setSearchTimeoutDuration(const UINT searchTimeoutDuration){
    this->searchTimeoutDuration = searchTimeoutDuration;
    return reset();
}

bool ThresholdCrossingDetector::setLowerThreshold(const Float lowerThreshold){ 
    this->lowerThreshold = lowerThreshold; 
    return reset();
} 

bool ThresholdCrossingDetector::setUpperThreshold(const Float upperThreshold){ 
    this->upperThreshold = upperThreshold; 
    return reset(); 
}
    
bool ThresholdCrossingDetector::setHysteresisThreshold(const Float hysteresisThreshold){
    this->hysteresisThreshold = hysteresisThreshold;
    return reset();
}

GRT_END_NAMESPACE
