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

#include "../Util/GRTVersionInfo.h"

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

namespace GRT{

class Timer{
public:
	/**
	Default constructor. 
	*/
    Timer(){}

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
        return true;
    }

    /**
	Starts the timer. This starts the timer in COUNTDOWN_MODE, in this mode the timer will start at the specified countdown time and count down until it reaches zero.
	At zero, the timer will set its state to timerRunning = false, however the timer will continue to countdown resulting in a negative query time.
	
	@param unsigned long countDownTime: sets the countdown time, this should be in milliseconds (i.e. start(5000) would start the timer with a countdown time of 5 seconds)
	@return returns true if the timer was started successfully, false otherwise
	*/
    bool start(unsigned long countDownTime){
        if( countDownTime > 0 ){
            startTime = getSystemTime();
            timerRunning = true;
            timerMode = COUNTDOWN_MODE;
            this->countDownTime = countDownTime;
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
        return true;
    }

    /**
	Gets the current time in milliseconds. If the timer is in NORMAL_MODE then this value will be the time since the timer was started.
	If the timer was in COUNTDOWN_MODE then this value will be the countdown time, minus the time since the timer started. 
	
	@return returns the current time elapsed in milliseconds (1000 milliseconds == 1 second) or 0 if the timer is not running
	*/
    signed long getMilliSeconds(){
        if( !timerRunning ) return 0;

        unsigned long now = getSystemTime();

        switch( timerMode ){
            case NORMAL_MODE:
                return (now-startTime);
                break;
            case COUNTDOWN_MODE:
                return (countDownTime - (now-startTime));
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
    double getSeconds(){
        if( !timerRunning ) return 0;
        return getMilliSeconds()/1000.0;
    }

    /**
	Gets if the timer is running.
	
	@return returns true if the timer is running, false otherwise
	*/
    bool running(){ return timerRunning; }

    /**
	Gets if the countdown time has been reached.
	
	@return returns true if the countdown time has been reached, false otherwise
	*/
    bool getTimerReached(){
	    if( !timerRunning ){
            return false;
        }
        if( getMilliSeconds() > 0 ) return false;
        return true;
	}
	
	/**
	This function is now deprecated, you should use getTimerReached() instead.
	*/
	bool timerReached(){
		return getTimerReached();
    }

    /**
	Gets if the current system time.
	
	@return returns the current system time.
	*/
    static unsigned long getSystemTime( ) {
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
    unsigned long startTime;
    unsigned long countDownTime;
    unsigned int timerMode;
    bool timerRunning;

    enum TimerModes{NORMAL_MODE=0,COUNTDOWN_MODE};

};

}; //End of namespace GRT


#endif //GRT_TIMER_HEADER