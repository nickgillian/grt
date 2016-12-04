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

#define GRT_DLL_EXPORTS
#include "VectorFloat.h"
#include "../Util/FileParser.h"

GRT_BEGIN_NAMESPACE
   
VectorFloat::VectorFloat(){
    warningLog.setKey("[WARNING VectorFloat]");
    errorLog.setKey("[ERROR VectorFloat]");
}
    
VectorFloat::VectorFloat(const size_type size){
    warningLog.setKey("[WARNING VectorFloat]");
    errorLog.setKey("[ERROR VectorFloat]");
    resize( size );
}

VectorFloat::VectorFloat( const size_type size, const Float &value ){
    warningLog.setKey("[WARNING VectorFloat]");
    errorLog.setKey("[ERROR VectorFloat]");
    resize( size, value );
}
    
VectorFloat::VectorFloat(const VectorFloat &rhs):Vector(rhs){
    warningLog.setKey("[WARNING VectorFloat]");
    errorLog.setKey("[ERROR VectorFloat]");
}

VectorFloat::~VectorFloat(){
    clear();
}
    
VectorFloat& VectorFloat::operator=(const VectorFloat &rhs){
    if( this != &rhs ){
        UINT N = rhs.getSize();
        if( N > 0 ){
            resize( N );
            std::copy( rhs.begin(), rhs.end(), this->begin() );
        }else this->clear();
    }
    return *this;
}
    
VectorFloat& VectorFloat::operator=(const Vector< Float > &rhs){
    if( this != &rhs ){
        UINT N = rhs.getSize();
        if( N > 0 ){
            resize( N );
            std::copy( rhs.begin(), rhs.end(), this->begin() );
        }else this->clear();
    }
    return *this;
}
  
bool VectorFloat::print(const std::string title) const {
    
    if( title != "" ){
        std::cout << title << std::endl;
    }
    const size_type size = this->size();
    const Float *data = getData();
    for(size_type i=0; i<size; i++){
        std::cout << data[i] << "\t";
    }
    std::cout << std::endl;

    return true;
}
    
bool VectorFloat::scale( const Float minTarget, const Float maxTarget, const bool constrain ){
    
    MinMax range = getMinMax();
    
    return scale( range.minValue, range.maxValue, minTarget, maxTarget, constrain );
}
    
bool VectorFloat::scale( const Float minSource, const Float maxSource, const Float minTarget, const Float maxTarget, const bool constrain ){
    
    const size_type N = this->size();
    
    if( N == 0 ){
        return false;
    }
    
    size_type i = 0;
    Float *data = getData();
    for( i=0; i<N; i++ ){
        data[i] = grt_scale(data[i],minSource,maxSource,minTarget,maxTarget,constrain);
    }

    return true;
}
    
Float VectorFloat::getMinValue() const{
    Float minValue = 99e+99;
    const size_type N = this->size();
    const Float *data = getData();
    for(size_type i=0; i<N; i++){
        if( data[i] < minValue ) minValue = data[i];
    }
    return minValue;
}

Float VectorFloat::getMaxValue() const{
    Float maxValue = -99e99;
    const size_type N = this->size();
    const Float *data = getData();
    for(size_type i=0; i<N; i++){
        if( data[i] > maxValue ) maxValue = data[i];
    }
    return maxValue;
}
    
Float VectorFloat::getMean() const {
    
    Float mean = 0.0;
    const size_type N = this->size();
    const Float *data = getData();
    for(size_type i=0; i<N; i++){
        mean += data[i];
    }
    mean /= N;
    
    return mean;
}
    
Float VectorFloat::getStdDev() const {
    
    Float mean = getMean();
	Float stdDev = 0.0;
    const size_type N = this->size();
    const Float *data = getData();
	
	for(size_type i=0; i<N; i++){
		stdDev += grt_sqr(data[i]-mean);
	}
    stdDev = grt_sqrt( stdDev / Float(N-1) );

    return stdDev;
}
    
MinMax VectorFloat::getMinMax() const {

    const size_type N = this->size();
    MinMax range;
    
    if( N == 0 ) return range;

    const Float *data = getData();
    
    for(size_type i=0; i<N; i++){
        range.updateMinMax( data[ i ] );
    }

    return range;
}
  
bool VectorFloat::save(const std::string &filename) const {

    const size_type N = this->size();

    if( N == 0 ){
        warningLog << "save(...) - Vector is empty, nothing to save!" << std::endl;
        return false;
    }
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !file.is_open() ){
        return false;
    }
    
    const Float *data = getData();
    for(size_type i=0; i<N; i++){
        file << data[i] << (i<N-1 ? "," : "\n");
    }
    
    file.close();

    return true;
}
    
bool VectorFloat::load(const std::string &filename,const char seperator){
    
    //Clear any previous data
    clear();

    //Open the file
    std::ifstream file( filename.c_str(), std::ifstream::in );
    if ( !file.is_open() ){
        warningLog << "load(...) - Failed to open file: " << filename << std::endl;
        return false;
    }
    
    Vector< std::string > vec;
    Vector< Float > row;
    std::string line;
    std::string columnString = "";
    const int sepValue = seperator;
    unsigned int numElements = 0;
    size_t length = 0;
    
    //Get the data, for a vector it should just be one line
    if( !getline(file,line) ){
        warningLog << "load(...) - Failed to read first row!" << std::endl;
        return false;
    }
    
    //Scan the line contents for the seperator token and parse the contents between each token
    columnString = "";
    length = line.length();
    vec.reserve( length );
    for(size_t i=0; i<length; i++){
        if( int(line[i]) == sepValue ){
            vec.push_back( columnString );
            columnString = "";
        }else columnString += line[i];
    }
        
    //Add the last column
    vec.push_back( columnString );
            
    //Remove the new line character from the string in the last column
    if( vec.size() >= 1 ){
        size_t K = vec.size()-1;
        size_t foundA = vec[ K ].find('\n');
        size_t foundB = vec[K ].find('\r');
        if( foundA != std::string::npos || foundB != std::string::npos ){
            vec[ K ] = vec[ K ].substr(0,vec[ K ].length()-1);
        }
    }
    
    numElements = vec.getSize();
    
    //Assign the memory
    if( !resize( numElements ) ){
        warningLog << "load(...) - Failed to resize memory!" << std::endl;
        return false;
    }
        
    //Convert the string column values to Float values
    Float *data = getData();
    for(unsigned int i=0; i<numElements; i++){
        data[i] = grt_from_str< Float >( vec[i] );
    }
    
    //Close the file
    file.close();
    
    return true;
}
    
GRT_END_NAMESPACE
