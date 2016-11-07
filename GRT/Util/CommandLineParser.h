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

GRT_BEGIN_NAMESPACE

class CommandLineParser {
public:
    /**
     Default constructor.
    */
    CommandLineParser():infoLog("[CommandLineParser]"),warningLog("[WARNING CommandLineParser]"),errorLog("[ERROR CommandLineParser]"){}

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
        parsed[ id ] = false; //Flag that this ID has not been parsed
        return true;
    }

    /**
     Adds an option with a specific retrival id. For example, addOption("-f","filename") would add the option "-f" which can be retrived using the "filename" id.

     @param const std::string &option: the command line option you want to store when the command line is parsed
     @param const std::string &id: the id that will be used to retrieve the data associated with the option
     @return returns true if the option was added succesfully, false otherwise
    */
    template<class T> bool addOption(const std::string &option,const std::string &id,const T &defaultValue){

        //Make sure the option has not already been added
        std::map<std::string,std::string>::iterator it = options.find( option );
        if( it != options.end() ){
            return false;
        }

        //Add the option and id
        options[option] = id;
        results[ id ] = toString( defaultValue ); //Set the default value
        parsed[ id ] = false; //Flag that this ID has not been parsed
        return true;
    }

    /**
     Parses the command line arguments, storing any relevant results in the results buffer. The can then be accessed using the get(...) function.

     @param const int argc: the number of command line arguments
     @param char * argv[]: an array of arguments
     @return returns true if the command line was parsed succesfully, false otherwise
    */
    bool parse( const int argc, char * argv[] ){
 
      //Reset the parsed state
      {
        std::map<std::string,bool>::iterator it;
        it = parsed.begin();
        while( it != parsed.end() ){
          it->second = false;
          ++it;
        }
      }

      std::map<std::string,std::string>::iterator it;
      for(int i=1; i<argc; i++){
        //Search for a match in the options map
        it = options.find( std::string(argv[i]) );
        if( it != options.end() ){
          if( i+1 < argc ){
            results[ it->second ] = std::string(argv[i+1]);
          }else{
            results[ it->second ] = "";
          }
          parsed[ it->second ] = true;
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
    template<class T> bool get(const std::string &id,T &value){
        
        //Find the option in the results buffer
        std::map<std::string,std::string>::iterator it;
        it = results.find( id );

        //If the iterator is empty, then we failed to find a match
        if( it == results.end() ){
          warningLog << "get(const std::string &id,T &value) - Failed to find id: " << id << ", it must not have been parsed and no default value set!" << std::endl;
          return false;
        }
        
        //Convert the data associated with the id to the relevant type
        try{
            std::stringstream s( it->second );
            s >> value;
            return true;
        }catch( ... ){
            warningLog << "get(const std::string &id,T &value) - Failed to convert type: " << typeid( value ).name() << " from: " << it->second << " for " << it->first << std::endl;
        }

        return false;
    }
    
    /**
     Validates if id is validate (i.e., it exists in the lookup table). Any value in the lookup table will be validate, regardless of its parsed state.
     If the id is invalid, then the function will return false.

     @param const std::string &id: the search id
     @return returns true if the value associated with the id is valid, false otherwise
    */
    bool getValidId(const std::string &id){
        
        //Find the option in the parsed buffer
        std::map<std::string,bool>::iterator it;
        it = parsed.find( id );
        
        if( it == parsed.end() ){
            return false;
        }
        
        //If we get this far then we've found a match, we don't care about the
        //parsed state
        return true;
    }

    /**
     Checks if the id was succesfully parsed from the command line.
     If the id is invalid or was not parsed, then the function will return false.

     @param const std::string &id: the search id
     @return returns true if the value associated with the id was parsed from the command line succesfully, false otherwise
    */
    bool getOptionParsed(const std::string &id){
        //Find the id in the parsed buffer
        std::map<std::string,bool>::iterator it;
        it = parsed.find( id );
        
        if( it == parsed.end() ){
            return false;
        }
        
        //If we get this far then we've found a match, return the parsed state
        return it->second;
    }

    /**
     @return returns the number of options in the options map
    */
    unsigned int getNumOptions() const {
      return (unsigned int)options.size();
    }

    /**
     Sets if info logging is enabled/disabled for this specific instance.
     If you want to enable/disable info logging globally, then you should use the InfoLog::enableLogging( bool ) function.

     @return returns true if the parameter was updated, false otherwise
    */
    bool setInfoLoggingEnabled(const bool loggingEnabled){
        return infoLog.setInstanceLoggingEnabled( loggingEnabled );
    }

    /**
     Sets if warning logging is enabled/disabled for this specific instance.
     If you want to enable/disable warning logging globally, then you should use the WarningLog::enableLogging( bool ) function.

     @return returns true if the parameter was updated, false otherwise
     */
    bool setWarningLoggingEnabled(const bool loggingEnabled){
        return warningLog.setInstanceLoggingEnabled( loggingEnabled );
    }

    /**
     Sets if error logging is enabled/disabled for this specific instance.
     If you want to enable/disable error logging globally, then you should use the ErrorLog::enableLogging( bool ) function.

     @return returns true if the parameter was updated, false otherwise
    */
    bool setErrorLoggingEnabled(const bool loggingEnabled){
        return errorLog.setInstanceLoggingEnabled( loggingEnabled );
    }

protected:
    InfoLog infoLog;
    WarningLog warningLog;
    ErrorLog errorLog;
    std::map< std::string, std::string > options; ///<Holds the options lookup map
    std::map< std::string, std::string > results; ///<Stores the results for each option in the options lookup map
    std::map< std::string, bool > parsed; ///<Stores the results that have been parsed

    template <class T>
    std::string toString(const T&value){
      std::ostringstream out;
      out << value;
      return out.str();
    }
};

GRT_END_NAMESPACE

#endif //GRT_COMMAND_LINE_PARSER_HEADER
