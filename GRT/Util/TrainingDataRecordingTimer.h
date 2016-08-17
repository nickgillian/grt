/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief The TrainingDataRecordingTimer is a tool to help record your training data.
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

#ifndef GRT_TRAINING_DATA_RECORDING_TIMER_HEADER
#define GRT_TRAINING_DATA_RECORDING_TIMER_HEADER

#include "GRTCommon.h"

GRT_BEGIN_NAMESPACE

class GRT_API TrainingDataRecordingTimer{
public:

	/**
     Default Constructor.
    */
	TrainingDataRecordingTimer();
    
    /**
     Copy Constructor, copies the TrainingDataRecordingTimer from the rhs instance to this instance
     
	 @param const TrainingDataRecordingTimer &rhs: another instance of the TrainingDataRecordingTimer class from which the data will be copied to this instance
	*/
	TrainingDataRecordingTimer(const TrainingDataRecordingTimer &rhs);

	/**
     Default Destructor
    */
	~TrainingDataRecordingTimer();

	/**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const TrainingDataRecordingTimer &rhs: another instance of the TrainingDataRecordingTimer class from which the data will be copied to this instance
	 @return a reference to this instance of the TrainingDataRecordingTimer
	*/
	TrainingDataRecordingTimer& operator= (const TrainingDataRecordingTimer &rhs);
    
    bool startRecording(unsigned long prepTime,unsigned long recordTime);
    
    bool stopRecording();
    
    bool update();
    
    bool getInPrepMode();
    
    bool getInRecordingMode();
    
    bool getRecordingStopped();
    
    double getSeconds();

protected:
    UINT recordingMode;
    unsigned long prepTime;
    unsigned long recordTime;
    Timer trainingTimer;
    
    
public:
    enum RecordingModes{NOT_RECORDING=0,PREP_COUNTDOWN,RECORDING};
    
};

GRT_END_NAMESPACE

#endif //GRT_TRAINING_DATA_RECORDING_TIMER_HEADER
