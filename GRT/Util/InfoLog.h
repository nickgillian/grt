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

#ifndef GRT_INFO_LOG_HEADER
#define GRT_INFO_LOG_HEADER

#include "Log.h"
#include "ObserverManager.h"

namespace GRT{
    
class InfoLogMessage{
public:
    InfoLogMessage(std::string proceedingText = "",std::string message = ""){
        this->proceedingText = proceedingText;
        this->message = message;
    }
    ~InfoLogMessage(){
        
    }
    
    std::string getProceedingText() const {
        return proceedingText;
    }
    
    std::string getMessage() const {
        return message;
    }
    
    std::string proceedingText;
    std::string message;
};

class InfoLog : public Log{
public:
    InfoLog(std::string proceedingText = ""){ setProceedingText(proceedingText); Log::loggingEnabledPtr = &infoLoggingEnabled; }

    virtual ~InfoLog(){}

    InfoLog& operator=(const InfoLog &rhs){
        if( this != &rhs ){
            this->proceedingText = rhs.proceedingText;
            this->writeProceedingText = rhs.writeProceedingText;
            this->loggingEnabledPtr = &infoLoggingEnabled;
            this->writeProceedingTextPtr = &writeProceedingText;
        }
        return *this;
    }

    //Getters
    virtual bool loggingEnabled(){ return infoLoggingEnabled; }
    
    //Setters
    static bool enableLogging(bool loggingEnabled);
    
    static bool registerObserver(Observer< InfoLogMessage > &observer);

protected:
    virtual void triggerCallback( const std::string &message ) const{
        observerManager.notifyObservers( InfoLogMessage(proceedingText,message) );
        return;
    }
    
    static ObserverManager< InfoLogMessage > observerManager;
    static bool infoLoggingEnabled;
};

} //End of namespace GRT

#endif //GRT_INFO_LOG_HEADER
