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
#include "TrainingDataRecordingTimer.h"

namespace GRT{

TrainingDataRecordingTimer::TrainingDataRecordingTimer(){
    recordingMode = NOT_RECORDING;
    prepTime = 0;
    recordTime = 0;
}

TrainingDataRecordingTimer::TrainingDataRecordingTimer(const TrainingDataRecordingTimer &rhs){
    this->recordingMode = rhs.recordingMode;
    this->prepTime = rhs.prepTime;
    this->recordTime = rhs.recordTime;
    this->trainingTimer = rhs.trainingTimer;
}

TrainingDataRecordingTimer::~TrainingDataRecordingTimer(){

}
    
TrainingDataRecordingTimer& TrainingDataRecordingTimer::operator=(const TrainingDataRecordingTimer &rhs){
    if( this != &rhs ){
        this->recordingMode = rhs.recordingMode;
        this->prepTime = rhs.prepTime;
        this->recordTime = rhs.recordTime;
        this->trainingTimer = rhs.trainingTimer;
    }
	return *this;
}
    
bool TrainingDataRecordingTimer::startRecording(unsigned long prepTime,unsigned long recordTime){
    this->prepTime = prepTime;
    this->recordTime = recordTime;
   
    //If the prepTime is less than 1 then go straight ahead and start recording
    if( prepTime < 1 ){
        recordingMode = RECORDING;
        
        //If recordTime is less than 1 then record forever (until the user tells us to stop)
        if( recordTime < 1 ) trainingTimer.start();
        else trainingTimer.start( recordTime );
    }else{
        recordingMode = PREP_COUNTDOWN;
        trainingTimer.start( prepTime );
    }

    return true;
}

bool TrainingDataRecordingTimer::stopRecording(){
    recordingMode = NOT_RECORDING;
    trainingTimer.stop();
    return true;
}
    
bool TrainingDataRecordingTimer::update(){
    
    switch( recordingMode ){
        case NOT_RECORDING:
            //Do nothing
            return true;
            break;
        case PREP_COUNTDOWN:
            //Check to see if the prep countdown phase is over, if so then move to the recording mode
            if( trainingTimer.getTimerReached() ){
                recordingMode = RECORDING;
                //If recordTime is less than 1 then record forever (until the user tells us to stop)
                if( recordTime < 1 ) trainingTimer.start();
                else trainingTimer.start( recordTime );
            }
            break;
        case RECORDING:
            if( trainingTimer.getTimerReached() ){
                recordingMode = NOT_RECORDING;
            }
            break;
        default:
            return false;
            break;
    }
    return true;
}
    
bool TrainingDataRecordingTimer::getInPrepMode(){
    update();
    return recordingMode==PREP_COUNTDOWN ? true : false;
}

bool TrainingDataRecordingTimer::getInRecordingMode(){
    update();
    return recordingMode==RECORDING ? true : false;
}
    
bool TrainingDataRecordingTimer::getRecordingStopped(){
    update();
    return recordingMode==NOT_RECORDING ? true : false;
}
    
double TrainingDataRecordingTimer::getSeconds(){
    update();
    return trainingTimer.getSeconds();
}

}; //End of namespace GRT
