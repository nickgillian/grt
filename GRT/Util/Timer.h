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

#ifndef GRT_TIMER_HEADER
#define GRT_TIMER_HEADER

#include "GRTVersionInfo.h"
#include "GRTTypedefs.h"

//Include the platform specific time headers
#if defined(__GRT_WINDOWS_BUILD__)
	#include <windows.h>
    #include <mmsystem.h>
#endif

#if defined(__GRT_OSX_BUILD__)
    #include <sys/time.h>
#endif

#if defined(__GRT_LINUX_BUILD__)
    #include <sys/time.h>
#endif

GRT_BEGIN_NAMESPACE

class Timer{
public:
    /**
     Default constructor. 
    */
    Timer(){
        timerMode = NORMAL_MODE;
        timerState = NOT_RUNNING;
        timerRunning = false;
    }

    /**
     Default destructor.
	*/
    ~Timer(){}

    /**
     Starts the timer. This starts the timer in NORMAL_MODE, in this mode the timer will run forever.
	
     @return returns true if the timer was started successfully, false otherwise
	*/
    bool start(){
        startTime = getSystemTime();
        timerRunning = true;
        timerMode = NORMAL_MODE;
        timerState = RUNNING;
        return true;
    }

    /**
     Starts the timer. This starts the timer in COUNTDOWN_MODE, in this mode the timer will start at the specified countdown time and count down until it reaches zero.
     At zero, the timer will set its state to timerRunning = false, however the timer will continue to countdown resulting in a negative query time.  The second option
     controls the preperation time, this is useful if you are recording a gesture and want to have a short pause after triggering the recording before data is actually
     recorded/tagged.  If the prepTime parameter is larger than zero, then the timer will enter the PREP_STATE for the specified prepTime before entering the COUNTDOWN_STATE.
	
     @param countDownTime: sets the countdown time, this should be in milliseconds (i.e. start(5000) would start the timer with a countdown time of 5 seconds)
     @return returns true if the timer was started successfully, false otherwise
	*/
    bool start(unsigned long countDownTime,unsigned long prepTime = 0){
        if( countDownTime > 0 ){
            startTime = getSystemTime();
            timerRunning = true;
            this->countDownTime = countDownTime;
            this->prepTime = prepTime;
            timerMode = COUNTDOWN_MODE;

            if( prepTime > 0 ){
                timerState = PREP_STATE;
            }else{
                timerState = COUNTDOWN_STATE;
            }
            
            return true;
        }
        return false;
    }
    
    /**
	Stops the timer. 
	
	@return returns true if the timer was stopped successfully, false otherwise
	*/
    bool stop(){
        timerRunning = false;
        timerState = NOT_RUNNING;
        return true;
    }

    /**
	Gets the current time in milliseconds. If the timer is in NORMAL_MODE then this value will be the time since the timer was started.
	If the timer was in COUNTDOWN_MODE then this value will be the countdown time, minus the time since the timer started. 
	
	@return returns the current time elapsed in milliseconds (1000 milliseconds == 1 second) or 0 if the timer is not running
	*/
    signed long getMilliSeconds() {
        if( !timerRunning ) return 0;

        unsigned long now = getSystemTime();

        switch( timerMode ){
            case NORMAL_MODE:
                return (now-startTime);
                break;
            case COUNTDOWN_MODE:
                if( timerState == PREP_STATE ){
                    if( now-startTime >= prepTime ){
                        timerState = COUNTDOWN_STATE;
                        startTime = now;
                        return countDownTime;
                    }
                    return countDownTime;
                }
                if( timerState == COUNTDOWN_STATE ) return (countDownTime - (now-startTime));
                break;
            default:
                return 0;
                break;
        }

        return 0;
    }
    
    /**
     Gets the current time in seconds. If the timer is in NORMAL_MODE then this value will be the time since the timer was started.
     If the timer was in COUNTDOWN_MODE then this value will be the countdown time, minus the time since the timer started.
     Unlike the #getMilliSeconds() function, in which 1.0 represents 1 millisecond; this function represents 1.0 as 1 second
     (so 0.5 is 500 milliseconds).
     
     @return returns the current time elapsed in seconds or 0 if the timer is not running
    */
    Float getSeconds() {
        if( !timerRunning ) return 0;
        return getMilliSeconds()/1000.0;
    }

    /**
	Gets if the timer is running.
	
	@return returns true if the timer is running, false otherwise
	*/
    bool running() { return getTimerRunning(); }

    /**
    Gets if the timer is running.
    
    @return returns true if the timer is running, false otherwise
    */
    bool getTimerRunning() {
        if( !timerRunning ){
            return false;
        }
        if( getMilliSeconds() > 0 ) return false;
        return true;
    }

    /**
	Gets if the countdown time has been reached.
	
	@return returns true if the countdown time has been reached, false otherwise
	*/
    bool getTimerReached() {
	    if( !timerRunning ){
            return false;
        }
        if( getMilliSeconds() > 0 ) return false;
        return true;
	}

    bool getInPrepState() const {
        return timerState == PREP_STATE;
    }

    bool getInCountdownState() const {
        return timerState == COUNTDOWN_STATE;
    }
	
	/**
	This function is now deprecated, you should use getTimerReached() instead.
	*/
	bool timerReached() {
		return getTimerReached();
    }

    /**
	Gets if the current system time.
	
	@return returns the current system time.
	*/
    static unsigned long getSystemTime( ){
#ifdef __GRT_OSX_BUILD__
        struct timeval now;
        gettimeofday( &now, NULL );
        return now.tv_usec/1000 + now.tv_sec*1000;
#endif
#ifdef __GRT_WINDOWS_BUILD__
        SYSTEMTIME systemTime;
        GetSystemTime(&systemTime);
        return (systemTime.wHour*60*60*1000) + (systemTime.wMinute*60*1000) + (systemTime.wSecond*1000) + systemTime.wMilliseconds;
#endif
#ifdef __GRT_LINUX_BUILD__
        struct timeval now;
        gettimeofday( &now, NULL );
        return now.tv_usec/1000 + now.tv_sec*1000;
#endif
        return 0;
    }

protected:
    enum TimerMode {NORMAL_MODE=0,COUNTDOWN_MODE};
    enum TimerState {NOT_RUNNING=0,RUNNING,COUNTDOWN_STATE,PREP_STATE};

    unsigned long startTime;
    unsigned long countDownTime;
    unsigned long prepTime;
    bool timerRunning;
    TimerMode timerMode;
    TimerState timerState;

};

GRT_END_NAMESPACE

#endif //GRT_TIMER_HEADER
