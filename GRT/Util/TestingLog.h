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

#ifndef GRT_TESTING_LOG_HEADER
#define GRT_TESTING_LOG_HEADER

#include "Log.h"
#include "ObserverManager.h"

GRT_BEGIN_NAMESPACE
    
class TestingLogMessage{
public:
    TestingLogMessage(std::string key = "",std::string message = ""){
        this->key = key;
        this->message = message;
    }
    ~TestingLogMessage(){
        
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

class GRT_API TestingLog : public Log{
public:
    TestingLog( const std::string &key = "" ) : Log( key )
    {
        Log::loggingEnabledPtr = &testingLoggingEnabled; 
    }

    TestingLog(const TestingLog &rhs)
    {
        *this = rhs;
    }

    virtual ~TestingLog(){}

    TestingLog& operator=(const TestingLog &rhs){
        if( this != &rhs ){
            //Copy the base class
            Log *thisBase = this;
            const Log *rhsBase = &rhs;
            *thisBase = *rhsBase;

            //Perform any custom copies
            this->loggingEnabledPtr = &testingLoggingEnabled;
        }
        return *this;
    }

    /**
     @brief returns true if logging is enabled for this class, this supersedes the specific instance logging
     @return returns true if logging is enabled for this class, false otherwise
    */
    static bool getLoggingEnabled() { 
        return testingLoggingEnabled; 
    }

    /**
     @brief sets if logging is enabled for this class, this supersedes the specific instance logging
     @return returns true if the parameter was updated successfully, false otherwise
    */
    static bool setLoggingEnabled(const bool enabled) { 
        testingLoggingEnabled = enabled; 
        return true; 
    }
    
    static bool registerObserver(Observer< TestingLogMessage > &observer);

    static bool removeObserver(Observer< TestingLogMessage > &observer);

    GRT_DEPRECATED_MSG("enableLogging is deprecated, use setLoggingEnabled instead", static bool enableLogging(bool loggingEnabled) );
    GRT_DEPRECATED_MSG("loggingEnabled is deprecated, use getLoggingEnabled instead", bool loggingEnabled() const );
    
protected:
    virtual void triggerCallback( const std::string &message ) const{
        observerManager.notifyObservers( TestingLogMessage(key,message) );
        return;
    }
    
    static ObserverManager< TestingLogMessage > observerManager;
    static bool testingLoggingEnabled;
};

GRT_END_NAMESPACE

#endif //GRT_TESTING_LOG_HEADER
