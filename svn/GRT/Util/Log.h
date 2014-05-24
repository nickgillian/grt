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

#include <iostream>
#include <string.h>
#include "Util.h"

namespace GRT{

class Log{
public:
    Log(std::string proceedingText = ""){
        setProceedingText(proceedingText);
        loggingEnabledPtr = NULL;
        writeProceedingText = true;
        writeProceedingTextPtr = &writeProceedingText;
        lastMessagePtr = &lastMessage;
    }

    virtual ~Log(){}
    
    const Log& operator<< (const bool val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << proceedingText.c_str() << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;
    }
    
    const Log& operator<< (const short val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;
    }

    const Log& operator<< (const unsigned short val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;
    }

    const Log& operator<< (const int val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;
    }

    const Log& operator<< (const unsigned int val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;
    }

    const Log& operator<< (const long val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;    }

    const Log& operator<< (const unsigned long val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;    }

    const Log& operator<< (const float val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;    }

    const Log& operator<< (const double val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;
    }

    const Log& operator<< (const long double val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
            *lastMessagePtr += Util::toString(val);
        }
        return *this;    }

    const Log& operator<< (const void* val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val;
        }
        return *this;
    }
    
    const Log& operator<< (const std::string val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                *lastMessagePtr = "";
                std::cout << proceedingText.c_str();
            }
            std::cout << val.c_str();
            *lastMessagePtr += val;
        }
        return *this;
    }
    
    const Log& operator<< (const char* val ) const{
        if( *loggingEnabledPtr ){
            if( *writeProceedingTextPtr ){
                *writeProceedingTextPtr = false;
                std::cout << proceedingText.c_str();
                *lastMessagePtr = "";
            }
            std::cout << val;
            *lastMessagePtr += val;
        }
        return *this;
    }
    
    // this is the type of std::cout
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
    
    // this is the function signature of std::endl
    typedef CoutType& (*StandardEndLine)(CoutType&);
    
    // define an operator<< to take in std::endl
    const Log& operator<<(const StandardEndLine manip) const{
        if( *loggingEnabledPtr ){
            // call the function, but we cannot return it's value
            manip(std::cout);
            *writeProceedingTextPtr = true;
            
            //Trigger any logging callbacks
            triggerCallback( lastMessage );
        }
        
        return *this;
    }

    //Getters
    virtual bool getLoggingEnabled() const{ return false; }

    std::string getProceedingText() const{ return proceedingText; }

    virtual std::string getLastMessage() const{ return lastMessage; }
    
    //Setters
    void setProceedingText(const std::string &proceedingText){
        if( proceedingText.length() == 0 ) this->proceedingText = "";
        else this->proceedingText = proceedingText + " ";
    }

protected:
    virtual void triggerCallback( const std::string &message ) const{
        return;
    }
    
    std::string proceedingText;
    std::string lastMessage;
    bool *loggingEnabledPtr;
    bool *writeProceedingTextPtr;
    std::string *lastMessagePtr;
    bool writeProceedingText;
};

} //End of namespace GRT

#endif //GRT_LOG_HEADER
