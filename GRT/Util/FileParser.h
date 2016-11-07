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

#ifndef GRT_FILE_PARSER_HEADER
#define GRT_FILE_PARSER_HEADER

#include <iostream>     // cout, endl
#include <fstream>      // fstream
#include <vector>
#include <string>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <sstream>
#include <deque>
#include "../DataStructures/Vector.h"
#include "InfoLog.h"

GRT_BEGIN_NAMESPACE

class FileParser{
public:
    FileParser():infoLog("[FileParser]"),warningLog("[WARNING FileParser]"){
	    clear();
    }
    ~FileParser(){
    }
    
    Vector< std::string >& operator[](const unsigned int &index){
        return fileContents[index];
    }
    
    bool parseCSVFile(std::string filename,bool removeNewLineCharacter=true){
        return parseFile(filename,removeNewLineCharacter,',');
    }
    
    bool parseTSVFile(std::string filename,bool removeNewLineCharacter=true){
        return parseFile(filename,removeNewLineCharacter,'\t');
    }
  
    bool getFileParsed(){
	  return fileParsed;
    }
  
    bool getConsistentColumnSize(){
	  return consistentColumnSize;
    }
  
    unsigned int getRowSize(){
	  return (unsigned int)fileContents.size();
    }
  
    unsigned int getColumnSize(){
	  return columnSize;
    }
  
    std::deque< Vector< std::string > >& getFileContents(){
        return fileContents;
    }
    
    bool clear(){
        fileParsed = false;
        consistentColumnSize = false;
        columnSize = 0;
        fileContents.clear();
        return true;
    }
    
    static bool parseColumn( const std::string &row, Vector< std::string > &cols, const char seperator = ',' ){

        const unsigned int N = (unsigned int)row.length();
        if( N == 0 ) return false;
        
        size_t lastSize = cols.size();
        cols.clear();
        if( lastSize > 0 ) cols.reserve( lastSize ); //Reserve the previous column size
        std::string columnString = "";
        const int sepValue = seperator;
        for(unsigned int i=0; i<N; i++){
            if( int(row[i]) == sepValue ){
                cols.push_back( columnString );
                columnString = "";
            }else columnString += row[i];
        }
        
        //Add the last column
        cols.push_back( columnString );
        
        //Remove the new line char from the string in the last column
        if( cols.size() >= 1 ){
            size_t K = cols.size()-1;
            size_t foundA = cols[ K ].find('\n');
            size_t foundB = cols[ K ].find('\r');
            if( foundA != std::string::npos || foundB != std::string::npos ){
                cols[ K ]  = cols[ K ].substr(0,cols[K].length()-1);
            }
        }
        
        return true;
    }
  
protected:
    
    bool parseFile(const std::string &filename,const bool removeNewLineCharacter,const char seperator){
        
        //Clear any previous data
        clear();
        
        std::ifstream file( filename.c_str(), std::ifstream::in );
        if ( !file.is_open() ){
            warningLog << "parseFile(...) - Failed to open file: " << filename << std::endl;
            return false;
        }

        //Get the size of the file
        std::streampos begin,end;
        begin = file.tellg();
        file.seekg (0, std::ios::end);
        end = file.tellg();
        file.seekg (0, std::ios::beg); //Reset the file pointer to the start of the file so we can read it
        
        Vector< std::string > vec;
        std::string line;
        
        //Loop over each line of data and parse the contents
        while ( getline( file, line ) )
        {
            if( !parseColumn(line, vec, seperator) ){
                clear();
                warningLog << "parseFile(...) - Failed to parse column!" << std::endl;
                file.close();
                return false;
            }
            
            //Check to make sure all the columns are consistent
            if( columnSize == 0 ){
                consistentColumnSize = true;
                columnSize = vec.getSize();
            }else if( columnSize != vec.getSize() ) consistentColumnSize = false;

            fileContents.push_back( vec );
        }
        
        //Close the file
        file.close();
        
        //Flag that we have parsed the file
        fileParsed = true;
        
        return true;
    }
    
    bool fileParsed;
    bool consistentColumnSize;
    unsigned int columnSize;
    InfoLog infoLog;
    WarningLog warningLog;
    std::deque< Vector< std::string > > fileContents;

};
    
GRT_END_NAMESPACE

#endif //GRT_FILE_PARSER_HEADER
