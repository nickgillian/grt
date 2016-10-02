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

#ifndef GRT_ERROR_LOG_HEADER
#define GRT_ERROR_LOG_HEADER

#include "ObserverManager.h"
#include "Log.h"

GRT_BEGIN_NAMESPACE

class GRT_API ErrorLogMessage{
public:
    ErrorLogMessage(std::string key = "",std::string message = ""){
        this->key = key;
        this->message = message;
    }
    ~ErrorLogMessage(){
        
    }
    
    std::string getProceedingText() const {
        return key;
    }
    
    std::string getMessage() const {
        return message;
    }
    
    std::string key;
    std::string message;
};
    
class GRT_API ErrorLog : public Log {
public:
    ErrorLog(const std::string &key = "" ) : Log( key )
    {
        Log::loggingEnabledPtr = &errorLoggingEnabled;
    }
    
    virtual ~ErrorLog(){}

    ErrorLog& operator=(const ErrorLog &rhs){
        if( this != &rhs ){
            this->loggingEnabledPtr = &errorLoggingEnabled;
        }
        return *this;
    }
    
    //Getters
    virtual bool getLoggingEnabled() const{ return errorLoggingEnabled; }
    
    //Setters
    static bool enableLogging(bool loggingEnabled);
    
    static bool registerObserver(Observer< ErrorLogMessage > &observer);

    static bool removeObserver(Observer< ErrorLogMessage > &observer);
    
protected:
    virtual void triggerCallback( const std::string &message ) const{
        observerManager.notifyObservers( ErrorLogMessage(key,message) );
        return;
    }
    
    static ObserverManager< ErrorLogMessage > observerManager;
    static bool errorLoggingEnabled;
};

GRT_END_NAMESPACE

#endif //GRT_ERROR_LOG_HEADER
