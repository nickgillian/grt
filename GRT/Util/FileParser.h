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
#include "MatrixDouble.h"

namespace GRT {

using namespace std;

class FileParser{
public:
    FileParser():warningLog("[FileParser]"){
	    clear();
    }
    ~FileParser(){
    }
    
    vector< string >& operator[](const unsigned int &index){
        return fileContents[index];
    }
    
    bool parseCSVFile(string filename,bool removeNewLineCharacter=true){
        return parseFile(filename,removeNewLineCharacter,',');
    }
    
    bool parseTSVFile(string filename,bool removeNewLineCharacter=true){
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
  
    vector< vector< string > > getFileContents(){
	  return fileContents;
    }
    
    bool clear(){
        fileParsed = false;
        consistentColumnSize = false;
        columnSize = 0;
        fileContents.clear();
        return true;
    }
    
    static bool parseColumn( const string &row, vector< string > &cols, const char seperator = ',' ){
        
        cols.clear();
        string columnString = "";
        const int sepValue = seperator;
        const unsigned int N = (unsigned int)row.length();
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
    
    bool parseFile(string filename,bool removeNewLineCharacter,const char seperator){
        
        //Clear any previous data
        clear();
        
        ifstream file( filename.c_str(), ifstream::in );
        if ( !file.is_open() ){
            warningLog << "parseFile(...) - Failed to open file: " << filename << endl;
            return false;
        }
        
        vector< string > vec;
        string line;
        
        //Loop over each line of data and parse the contents
        while ( getline(file,line) )
        {
            if( !parseColumn(line, vec,seperator) ){
                clear();
                warningLog << "parseFile(...) - Failed to parse column!" << endl;
                file.close();
                return false;
            }
            
            //Check to make sure all the columns are consistent
            if( columnSize == 0 ){
                consistentColumnSize = true;
                columnSize = (unsigned int)vec.size();
            }else if( columnSize != vec.size() ) consistentColumnSize = false;

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
    WarningLog warningLog;
    vector< vector< string > > fileContents;

};
    
}//End of namespace GRT

#endif //GRT_FILE_PARSER_HEADER
