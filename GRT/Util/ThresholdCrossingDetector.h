/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief This class implements a threshold crossing detector.
 */

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

#ifndef GRT_THRESHOLD_CROSSING_DETECTOR_HEADER
#define GRT_THRESHOLD_CROSSING_DETECTOR_HEADER

#include "../PreProcessingModules/MovingAverageFilter.h"
#include "../PreProcessingModules/Derivative.h"
#include "CircularBuffer.h"

GRT_BEGIN_NAMESPACE
    
class GRT_API ThresholdCrossingDetector{
public:
    /**
     Default Constructor.  
     
     @param analysisMode: sets the algorithm used to detect threshold crossings, this should be one of the AnalysisMode enums
     @param thresholdCrossingMode: sets the threshold crossing mode, this should be one of the ThresholdCrossingModes enums
     @param lowerThreshold: sets the lower threshold level
     @param upperThreshold: sets the upper threshold level
     @param searchWindowSize: sets the size of the search window used to detect UPPER_THEN_LOWER_THRESHOLD_CROSSING or LOWER_THEN_UPPER_THRESHOLD_CROSSING
     @param searchTimeoutDuration: sets the duration (in samples) that the threshold detector will ignore any new threshold crossings after detecting a valid threshold crossing
     @param offsetFilterSize: sets the size of the moving average filter used to offset the input value (if the analysisMode is set to MOVING_OFFSET_ANALYSIS_MODE)
     */
    ThresholdCrossingDetector(UINT analysisMode = RAW_DATA_ANALYSIS_MODE,UINT thresholdCrossingMode = UPPER_THRESHOLD_CROSSING,UINT detectionTimeoutMode = TIMEOUT_COUNTER,Float lowerThreshold = -1,Float upperThreshold = 1,Float hysteresisThreshold = 0,UINT searchWindowSize = 20,UINT searchTimeoutDuration = 1000,UINT offsetFilterSize = 10);
    
    /**
     Default Destructor.
     */
	~ThresholdCrossingDetector();
    
    /**
     Copy Constructor.  Copies the data from the rhs instance to this instance.
     
     @param const ThresholdCrossingDetector &rhs: the right hand instance from which the data will be copied to this instance
     */
    ThresholdCrossingDetector(const ThresholdCrossingDetector &rhs);
    
    /**
     Equals Operator.  Copies the data from the rhs instance to this instance.
     
     @param rhs: the right hand instance from which the data will be copied to this instance
     @return returns a reference to this instance
     */
    ThresholdCrossingDetector& operator=(const ThresholdCrossingDetector &rhs);
    
    /**
     This is the main update interface, you should call this every time you get a new sample from your sensor.
     
     @param x: the new value that will be pushed through the threshold crossing algorithm
     @return returns true if a threshold crossing was detected, false otherwise
     */
    bool update( const Float x );
    
    /**
     This function resets the threshold crossing detector.
     
     @return returns true if the threshold crossing detector was reset, false otherwise
     */
    bool reset();
    
    /**
     This function triggers a search timeout (i.e. the algorithm will stop searching for the next N samples, where N = searchTimeoutDuration).
     
     @return returns true if the search timeout was triggered, false otherwise
     */
    bool triggerSearchTimeout();
    
    /**
     This function returns true if a threshold crossing was detected during the last update.
     
     @return returns true if a threshold crossing was detected, false otherwise
     */
    bool getThresholdCrossingDetected() const;
    
    /**
     @return returns true if the search is enabled, false otherwise
     */
    bool getEnableSearch() const;
    
    /**
     @return returns the current analysis mode, this will be one of the AnalysisModes enums
     */
    UINT getAnalysisMode() const;
    
    /**
     @return returns the current threshold crossing mode being used, this will be one of the ThresholdCrossingModes enums
     */
    UINT getThresholdCrossingMode() const;
    
    /**
     @return returns the search window size, this is only used if the thresholdCrossingMode is UPPER_THEN_LOWER_THRESHOLD_CROSSING or LOWER_THEN_UPPER_THRESHOLD_CROSSING
     */
    UINT getSearchWindowSize() const;
    
    /**
     @return returns the size of the moving average filter used to offset the input value, this is only used if the analysisMode is MOVING_OFFSET_ANALYSIS_MODE
     */
    UINT getOffsetFilterSize() const;
    
    /**
     @return gets the current index in the search window when the algorithm is looking for the second threshold crossing for UPPER_THEN_LOWER_THRESHOLD_CROSSING or LOWER_THEN_UPPER_THRESHOLD_CROSSING
     */
    UINT getSearchWindowIndex() const;
    
    /**
     @return gets the current search timeout counter value
     */
    UINT getSearchTimeoutCounter();
    
    /**
     @return gets the search timeout duration
     */
    UINT getSearchTimeoutDuration() const;
    
    /**
     @return gets the most recent analysis value used to compute the threshold crossing
     */
    Float getAnalysisValue() const;
    
    /**
     @return gets the upper threshold value
     */
    Float getUpperThreshold() const;
    
    /**
     @return gets the lower threshold value
     */
    Float getLowerThreshold() const;
    
    /**
     @return gets the hysteresis threshold value
     */
    Float getHysteresisThreshold() const;
    
    /**
     Sets if the algorithm can perform the threshold crossing search. If enableSearch is set to false, then the algorithm will never 
     search for a threshold crossing.
     
     @param const bool enableSearch: the new enableSearch mode
     @return returns true if the parameter was updated, false otherwise
     */
    bool setEnableSearch(const bool enableSearch);
    
    /**
     Sets the anlysis mode used to detect threshold crossings.  This should be one of the AnalysisModes enums.
     
     @param const UINT analysisMode: the new analysisMode
     @return returns true if the parameter was updated, false otherwise
     */
    bool setAnalysisMode(const UINT analysisMode);
    
    /**
     Sets the threshold crossing mode used to detect threshold crossings.  This should be one of the ThresholdCrossingModes enums.
     
     @param const UINT thresholdCrossingMode: the new thresholdCrossingMode
     @return returns true if the parameter was updated, false otherwise
     */
    bool setThresholdCrossingMode(const UINT thresholdCrossingMode);
    
    /**
     Sets the searchWindowSize.  This is the number of samples the algorithm will search for a second threshold crossing after the first 
     threshold crossing has been found (in the UPPER_THEN_LOWER_THRESHOLD_CROSSING or LOWER_THEN_UPPER_THRESHOLD_CROSSING modes).
     
     @param const UINT searchWindowSize: the new searchWindowSize
     @return returns true if the parameter was updated, false otherwise
     */
    bool setSearchWindowSize(const UINT searchWindowSize);
    
    /**
     Sets the size of the moving average filter used to offset the input data.  This is only used if the analysisMode is MOVING_OFFSET_ANALYSIS_MODE.
     
     @param const UINT offsetFilterSize: the new offsetFilterSize
     @return returns true if the parameter was updated, false otherwise
     */
    bool setOffsetFilterSize(const UINT offsetFilterSize);
    
    /**
     Sets the searchTimeoutDuration, this is the time (in milliseconds) that will be ignored after a threshold crossing has been detected.
     
     @param const UINT searchTimeoutDuration: the new searchTimeoutDuration in milliseconds
     @return returns true if the parameter was updated, false otherwise
     */
    bool setSearchTimeoutDuration(const UINT searchTimeoutDuration);
    
    /**
     Sets the detectionTimeoutMode, this controls how the detection algorithm triggers detection timeouts after a threshold crossing has been detected.
     If the detectionTimeoutMode is TIMEOUT_COUNTER then the detection algorithm will wait for until the searchTimeoutDuration before searching for 
     the next threshold crossing.  Alternatively, if the detectionTimeoutMode is HYSTERESIS_THRESHOLD then the detection algorithm will wait for the 
     analysis signal to pass the hysteresisThreshold (regardless of the time this takes) before searching for a new threshold crossing.
     
     @param const UINT searchTimeoutDuration: the new searchTimeoutDuration
     @return returns true if the parameter was updated, false otherwise
     */
    bool setDetectionTimeoutMode(const UINT detectionTimeoutMode);
    
    /**
     Sets the lowerThreshold value, this lower threshold that must be crossed to trigger a LOWER_THRESHOLD_CROSSING.
     
     @param lowerThreshold: the new lowerThreshold
     @return returns true if the parameter was updated, false otherwise
     */
    bool setLowerThreshold(const Float lowerThreshold);
    
    /**
     Sets the upperThreshold value, this upper threshold that must be crossed to trigger a UPPER_THRESHOLD_CROSSING.
     
     @param upperThreshold: the new upperThreshold
     @return returns true if the parameter was updated, false otherwise
     */
    bool setUpperThreshold(const Float upperThreshold);
    
    /**
     Sets the hysteresisThreshold value, this threshold is used to debounce triggers if the detectionTimeoutMode is set to HYSTERESIS_THRESHOLD.
     
     @param hysteresisThreshold: the new hysteresisThreshold
     @return returns true if the parameter was updated, false otherwise
     */
    bool setHysteresisThreshold(const Float hysteresisThreshold);
    
protected:
    Float analysisValue;
    Float lowerThreshold;
    Float upperThreshold;
    Float hysteresisThreshold;
    bool enableSearch;
    bool thresholdCrossingDetected;
    unsigned int analysisMode;
    unsigned int thresholdCrossingMode;
    unsigned int detectionTimeoutMode;
    unsigned int searchTimeoutDuration;
    unsigned int searchWindowSize;
    unsigned int searchWindowIndex;
    unsigned int offsetFilterSize;
    unsigned int currentSearchState;
    Timer searchTimeoutCounter;
    MovingAverageFilter movingAverageFilter;
    Derivative derivative;
    
public:
    enum SearchStates{SEARCHING_FOR_FIRST_THRESHOLD_CROSSING=0,SEARCHING_FOR_SECOND_THRESHOLD_CROSSING,NO_SEARCH_GATE_TIME_OUT};

    enum ThresholdCrossingModes{UPPER_THRESHOLD_CROSSING=0,
                                LOWER_THRESHOLD_CROSSING,
                                UPPER_OR_LOWER_THRESHOLD_CROSSING,
                                UPPER_THEN_LOWER_THRESHOLD_CROSSING,
                                LOWER_THEN_UPPER_THRESHOLD_CROSSING};
    
    enum AnalysisModes{RAW_DATA_ANALYSIS_MODE=0,MOVING_OFFSET_ANALYSIS_MODE,DERIVATIVE_ANALYSIS_MODE};
    
    enum DetectionTimeoutMode{TIMEOUT_COUNTER=0,HYSTERESIS_THRESHOLD};
	
};

GRT_END_NAMESPACE

#endif //GRT_THRESHOLD_CROSSING_DETECTOR_HEADER
