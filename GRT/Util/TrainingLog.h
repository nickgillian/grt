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

#ifndef GRT_TRAINING_LOG_HEADER
#define GRT_TRAINING_LOG_HEADER

#include "Log.h"
#include "ObserverManager.h"

namespace GRT{
    
class TrainingLogMessage{
public:
    TrainingLogMessage(std::string proceedingText = "",std::string message = ""){
        this->proceedingText = proceedingText;
        this->message = message;
    }
    ~TrainingLogMessage(){
        
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

class TrainingLog : public Log{
public:
    TrainingLog(std::string proceedingText = ""){ setProceedingText(proceedingText); Log::loggingEnabledPtr = &trainingLoggingEnabled; }

    virtual ~TrainingLog(){}

    TrainingLog& operator=(const TrainingLog &rhs){
        if( this != &rhs ){
            this->proceedingText = rhs.proceedingText;
            this->writeProceedingText = rhs.writeProceedingText;
            this->lastMessage = rhs.lastMessage;
            this->loggingEnabledPtr = &trainingLoggingEnabled;
            this->writeProceedingTextPtr = &writeProceedingText;
            this->lastMessagePtr = &lastMessage;
        }
        return *this;
    }
    
    //Getters
    virtual bool loggingEnabled() const{ return trainingLoggingEnabled; }
    
    //Setters
    static bool enableLogging(bool loggingEnabled);
    
    static bool registerObserver(Observer< TrainingLogMessage > &observer);
    
protected:
    virtual void triggerCallback( const std::string &message ) const{
        observerManager.notifyObservers( TrainingLogMessage(proceedingText,message) );
        return;
    }
    
    static bool trainingLoggingEnabled;
    static ObserverManager< TrainingLogMessage > observerManager;
};

}; //End of namespace GRT

#endif //GRT_TRAINING_LOG_HEADER
