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

#ifndef GRT_LOG_HEADER
#define GRT_LOG_HEADER

#include "GRTTypedefs.h"
//Only include the C++ 11 code if C++11 support it is enabled
#ifdef GRT_CXX11_ENABLED
#include <atomic>
#include <thread>
#include <mutex>
#endif //GRT_CXX11_ENABLED

GRT_BEGIN_NAMESPACE

/**
 @brief The Log class provides the base class for all GRT logging functionality.  The log 
*/
class GRT_API Log{
public:
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType; ///< this is the type of std::cout
    typedef CoutType& (*StandardEndLine)(CoutType&); ///< this is the function signature of std::endl

    Log(const std::string &key = ""){
        setKey(key);
        instanceLoggingEnabled = true;
        writeKey = true;
        loggingEnabledPtr = &instanceLoggingEnabled;
        writeKeyPtr = &writeKey;
        lastMessagePtr = &lastMessage;
    }

    Log(const Log &rhs){
        this->key = rhs.key;
        this->writeKey = rhs.writeKey;
        this->lastMessage = rhs.lastMessage;
        this->instanceLoggingEnabled = rhs.instanceLoggingEnabled;
        this->loggingEnabledPtr = &(this->instanceLoggingEnabled);
        this->writeKeyPtr = &(this->writeKey);
        this->lastMessagePtr = &(this->lastMessage);
    }

    virtual ~Log(){}

    Log& operator=(const Log &rhs){
        if( this != &rhs ){
            this->key = rhs.key;
            this->writeKey = rhs.writeKey;
            this->lastMessage = rhs.lastMessage;
            this->instanceLoggingEnabled = rhs.instanceLoggingEnabled;
            this->loggingEnabledPtr = &(this->instanceLoggingEnabled);
            this->writeKeyPtr = &(this->writeKey);
            this->lastMessagePtr = &(this->lastMessage);
        }
        return *this;
    }

    template < class T >
    const Log& operator<< (const T &val ) const{

#ifdef GRT_CXX11_ENABLED
        std::unique_lock<std::mutex> lock( logMutex );
#endif
        if( !logLoggingEnabled ) return *this; //If the base class global logging is disabled, then there is nothing to do
        
        if( *loggingEnabledPtr && instanceLoggingEnabled ){
            if( *writeKeyPtr ){
                *writeKeyPtr = false;
                std::cout << key.c_str();
                *lastMessagePtr = ""; //Reset the last message
            }
            std::cout << val;
            std::stringstream stream;
            stream << val;
            *lastMessagePtr += stream.str(); //Update the last message
        }
        return *this;
    }

    // define an operator<< to take in std::endl
    const Log& operator<<(const StandardEndLine manip) const{

#ifdef GRT_CXX11_ENABLED
        std::unique_lock<std::mutex> lock( logMutex );
#endif
        if( !logLoggingEnabled ) return *this; //If the base class global logging is disabled, then there is nothing to do

        if( *loggingEnabledPtr && instanceLoggingEnabled ){
            // call the function, but we cannot return it's value
            manip(std::cout);
            *writeKeyPtr = true; //The message is now complete, so toggle back the key
            
            //Trigger any logging callbacks
            triggerCallback( lastMessage );
        }
        
        return *this;
    }

    //Getters
    virtual bool getLoggingEnabled() const{ return logLoggingEnabled; }
    
    bool getInstanceLoggingEnabled() const { return instanceLoggingEnabled; };

    std::string getProceedingText() const{ return key; }

    
    /**
     @brief returns the key that gets written at the start of each message
     @return returns the key used to tag each message
    */
    virtual std::string getKey() const {
        return key;
    }

    /**
     @brief returns the last message written by the log
     @return returns a string containing the last message written by the log
    */
    virtual std::string getLastMessage() const{ 
        return lastMessage; 
    }
    
    /**
     @brief sets the key that gets written at the start of each message, this will be written in the format 'key message'. The key can be empty.
     @param key: the new key, this can be empty
     @return returns true if the key was updated
    */
    virtual bool setKey(const std::string &key){
        if( key.length() == 0 ) this->key = "";
        else this->key = key + " "; //Add a space to ensure pretty logging
        return true;
    }
    
    bool setEnableInstanceLogging(bool loggingEnabled){
        this->instanceLoggingEnabled = loggingEnabled;
        return true;
    }

    GRT_DEPRECATED_MSG("setProceedingText is deprecated, use setKey instead", void setProceedingText(std::string proceedingText) );

protected:
    /**
     @brief This callback can be used to propagate messages to other interfaces (e.g., a GUI built on top of the GRT). It gets triggered anytime
     a message is ended (with std::endl), the message will contain the full message + the log key. To use this, inherit from the log base class
     and overwrite the contents of this virtual function.
     @param message: the message from the latest log
    */
    virtual void triggerCallback( const std::string &message ) const{ return; }

    template< class T >
    std::string to_str( const T &val ) const {
        std::stringstream s;
        s << val;
        return s.str();
    }
    
    std::string key; ///<The key that will be written at the start of each log
    std::string lastMessage; ///<The last message written
    bool writeKey; ///<If true, then the key will be written at the start of each log message
    bool instanceLoggingEnabled; ///<If true, then this instance should log messages

    bool *loggingEnabledPtr;
    bool *writeKeyPtr;
    std::string *lastMessagePtr;

    static bool logLoggingEnabled; ///<This controls logging across all Log instances, as opposed to a single instance

#ifdef GRT_CXX11_ENABLED
    static std::mutex logMutex;
#endif
};

GRT_END_NAMESPACE

#endif //GRT_LOG_HEADER
