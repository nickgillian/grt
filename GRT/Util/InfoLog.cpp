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
#include "InfoLog.h"

GRT_BEGIN_NAMESPACE

//Base log functions
bool Log::baseLoggingEnabled = true; ///<Global logging for base class
void Log::setProceedingText(std::string proceedingText){ setKey( proceedingText ); } //Legacy
std::string Log::getProceedingText() const{ return key; } //Legacy
bool Log::setEnableInstanceLogging(const bool loggingEnabled){ return setInstanceLoggingEnabled( loggingEnabled ); } //Legacy

#ifdef GRT_CXX11_ENABLED
std::mutex Log::logMutex;
#endif

//Info log functions
bool InfoLog::infoLoggingEnabled = true;
ObserverManager< InfoLogMessage > InfoLog::observerManager;
    
bool InfoLog::enableLogging(bool loggingEnabled){
    infoLoggingEnabled = loggingEnabled;
    return infoLoggingEnabled;
}
    
bool InfoLog::registerObserver(Observer< InfoLogMessage > &observer){
    observerManager.registerObserver( observer );
    return true;
}

bool InfoLog::removeObserver(Observer< InfoLogMessage > &observer)
{
    return observerManager.removeObserver(observer);
}

bool InfoLog::loggingEnabled() const { return infoLoggingEnabled; } //Legacy

GRT_END_NAMESPACE
