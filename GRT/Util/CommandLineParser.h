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
#include "ErrorLog.h"
#include "Util.h"

namespace GRT{

class CommandLineParser{
public:
    /**
     Default constructor.
    */
    CommandLineParser():infoLog("[CommandLineParser]"),errorLog("[ERROR CommandLineParser]"),warningLog("[WARNING CommandLineParser]"){}

    /**
     Default destructor.
    */
    ~CommandLineParser(){}
    
    bool addOption(const std::string option,const std::string destination){
        options[option] = destination;
        return true;
    }
    
    bool parse(const int argc,const char * argv[] ){
        
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
    
    template<class T> bool get(const std::string destination,T &option,T defaultValue = T()){
        
        //Find the option in the results buffer
        std::map<std::string,std::string>::iterator it;
        it = results.find( destination );
        
        if( it == results.end() ){
		option = defaultValue;
            	warningLog << "get(const std::string destination,T &option) - Failed to find destination: " << destination << endl;
            	return false;
        }
        
        try{
            	std::stringstream s( it->second );
           	 s >> option;
		return true;
        }catch( ... ){
		warningLog << "get(const std::string destination,T &option) - Can not parse type: " << typeid( option ).name() << endl;
        }

        return false;
    }
    
    
    InfoLog infoLog;
    WarningLog warningLog;
    ErrorLog errorLog;
    std::map< std::string, std::string > options;
    std::map< std::string, std::string > results;

};

} //End of namespace GRT

#endif //GRT_COMMAND_LINE_PARSER_HEADER
