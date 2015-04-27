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

#ifndef GRT_COMMAND_LINE_PARSER_HEADER
#define GRT_COMMAND_LINE_PARSER_HEADER

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <math.h>
#include <fstream>
#include <typeinfo>
#include "InfoLog.h"
#include "WarningLog.h"
#include "ErrorLog.h"

namespace GRT{

class CommandLineParser {
public:
    /**
     Default constructor.
    */
    CommandLineParser():infoLog("[CommandLineParser]"),errorLog("[ERROR CommandLineParser]"),warningLog("[WARNING CommandLineParser]"){}

    /**
     Default destructor.
    */
    virtual ~CommandLineParser(){}
    
    /**
     Adds an option with a specific retrival id. For example, addOption("-f","filename") would add the option "-f" which can be retrived using the "filename" id.

     @param const std::string &option: the command line option you want to store when the command line is parsed
     @param const std::string &id: the id that will be used to retrieve the data associated with the option
     @return returns true if the option was added succesfully, false otherwise
    */
    bool addOption(const std::string &option,const std::string &id){

        //Make sure the option has not already been added
        std::map<std::string,std::string>::iterator it = options.find( option );
        if( it != options.end() ){
            return false;
        }

        //Add the option and id
        options[option] = id;
        return true;
    }

    /**
     Parses the command line arguments, storing any relevant results in the results buffer. The can then be accessed using the get(...) function.

     @param const int argc: the number of command line arguments
     @param char * argv[]: an array of arguments
     @return returns true if the command line was parsed succesfully, false otherwise
    */
    bool parse( const int argc, char * argv[] ){
        
        results.clear();
        
        std::map<std::string,std::string>::iterator it;
        for(int i=1; i<argc; i++){
            
            //Search for a match in the options map
            it = options.find( std::string(argv[i]) );
            if( it != options.end() ){
                results[  it->second ] = std::string(argv[i+1]);
            }
        }
        
        return true;
    }
    
    /**
     Retrives the value associated with the relevant id and stores it in value.
     If the id is invalid or the type can not be parsed, then the function will return false.

     @param const std::string &id: the search id
     @param T &value: stores the return value
     @param T defaultValue = T(): sets the default return value if the id is not found
     @return returns true if the value associated with the id was found succesfully, false otherwise
    */
    template<class T> bool get(const std::string &id,T &value,T defaultValue = T()){
        
        //Find the option in the results buffer
        std::map<std::string,std::string>::iterator it;
        it = results.find( id );
        
        if( it == results.end() ){
            value = defaultValue;
            warningLog << "get(const std::string &id,T &value) - Failed to find id: " << id << endl;
            return false;
        }
        
        //Convert the data associated with the id to the relevant type
        try{
            std::stringstream s( it->second );
            s >> value;
            return true;
        }catch( ... ){
            value = defaultValue;
            warningLog << "get(const std::string &id,T &value) - Can not parse type: " << typeid( value ).name() << endl;
        }

        return false;
    }
    
    /**
     Validates if id was succesfully parsed from the command line.
     If the id is invalid or was not parsed, then the function will return false.

     @param const std::string &id: the search id
     @return returns true if the value associated with the id was parsed succesfully, false otherwise
    */
    bool validate(const std::string &id){
        
        //Find the option in the results buffer
        std::map<std::string,std::string>::iterator it;
        it = results.find( id );
        
        if( it == results.end() ){
            warningLog << "validate(const std::string ) - Failed to find id: " << id << endl;
            return false;
        }
        
        //If we get this far then we've found a match
        return true;
    }

    /**
     Sets if info logging is enabled/disabled for this specific instance.
     If you want to enable/disable info logging globally, then you should use the InfoLog::enableLogging( bool ) function.

     @return returns true if the parameter was updated, false otherwise
    */
    bool setInfoLoggingEnabled(const bool loggingEnabled){
        infoLog.setEnableInstanceLogging( loggingEnabled );
        return true;
    }

    /**
     Sets if warning logging is enabled/disabled for this specific instance.
     If you want to enable/disable warning logging globally, then you should use the WarningLog::enableLogging( bool ) function.

     @return returns true if the parameter was updated, false otherwise
     */
    bool setWarningLoggingEnabled(const bool loggingEnabled){
        warningLog.setEnableInstanceLogging( loggingEnabled );
        return true;
    }

    /**
     Sets if error logging is enabled/disabled for this specific instance.
     If you want to enable/disable error logging globally, then you should use the ErrorLog::enableLogging( bool ) function.

     @return returns true if the parameter was updated, false otherwise
    */
    bool setErrorLoggingEnabled(const bool loggingEnabled){
        errorLog.setEnableInstanceLogging( loggingEnabled );
        return true;
    }

protected:
    InfoLog infoLog;
    WarningLog warningLog;
    ErrorLog errorLog;
    std::map< std::string, std::string > options;
    std::map< std::string, std::string > results;

};

} //End of namespace GRT

#endif //GRT_COMMAND_LINE_PARSER_HEADER
