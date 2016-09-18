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
#include "Util.h"
#include "TimeStamp.h"

namespace GRT {

bool Util::getCxx11Enabled(){
    #ifdef GRT_CXX11_ENABLED
        return true;
    #else
        return false;
    #endif
}
    
bool Util::sleep(const unsigned int &numMilliseconds){
    
#if defined( __GRT_WINDOWS_BUILD__ )
    Sleep( numMilliseconds );
    return true;
#endif
    
#if defined(__GRT_OSX_BUILD__)
    usleep( numMilliseconds * 1000 );
    return true;
#endif
    
#if defined(__GRT_LINUX_BUILD__)
    usleep( numMilliseconds * 1000 );
    return true;
#endif

    return false;
}
    
Float Util::scale(const Float &x,const Float &minSource,const Float &maxSource,const Float &minTarget,const Float &maxTarget,const bool constrain){
    if( constrain ){
        if( x <= minSource ) return minTarget;
        if( x >= maxSource ) return maxTarget;
    }
    if( minSource == maxSource ) return minTarget;
    return (((x-minSource)*(maxTarget-minTarget))/(maxSource-minSource))+minTarget;
}

std::string Util::timeAsString(const bool includeDate){
    TimeStamp timestamp("now");
    return timestamp.getTimeStampAsString( includeDate );
}
    
std::string Util::intToString(const int &i){
    std::stringstream s;
    s << i;
    return s.str();
}
    
std::string Util::intToString(const unsigned int &i){
    std::stringstream s;
    s << i;
    return s.str();
}

std::string Util::toString(const int &i){
    std::stringstream s;
    s << i;
    return s.str();
}

std::string Util::toString(const unsigned int &i){
    std::stringstream s;
    s << i;
    return s.str();
}

std::string Util::toString(const long &i){
    std::stringstream s;
    s << i;
    return s.str();
}

std::string Util::toString(const unsigned long &i){
    std::stringstream s;
    s << i;
    return s.str();
}
    
std::string Util::toString(const unsigned long long &i){
    std::stringstream s;
    s << i;
    return s.str();
}
    
std::string Util::toString(const bool &b){
    return b ? "1" : "0";
}

std::string Util::toString(const Float &v){
	std::stringstream s;
    s << v;
    return s.str();
}

std::string Util::toString(const long double &v){
    std::stringstream s;
    s << v;
    return s.str();
}

std::string Util::toString(const float &v){
	std::stringstream s;
    s << v;
    return s.str();
}
    
int Util::stringToInt(const std::string &value){
    std::stringstream s( value );
    int i;
    s >> i;
    return i;
}

Float Util::stringToFloat(const std::string &value){
    std::stringstream s( value );
    Float d;
    s >> d;
    return d;
}

double Util::stringToDouble(const std::string &value){
    std::stringstream s( value );
    double d;
    s >> d;
    return d;
}
    
bool Util::stringToBool(const std::string &value){
    if( value == "true" ) return true;
    if( value == "True" ) return true;
    if( value == "TRUE" ) return true;
    if( value == "t" ) return true;
    if( value == "T" ) return true;
    if( value == "1" ) return true;
    return false;
}
    
bool Util::stringEndsWith(const std::string &str, const std::string &ending) {
    if (str.length() >= ending.length()) {
        return (0 == str.compare (str.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
    
Float Util::limit(const Float value,const Float minValue,const Float maxValue){
    if( value <= minValue ) return minValue;
    if( value >= maxValue ) return maxValue;
    return value;
}
    
Float Util::sum(const VectorFloat &x){
    Float s = 0;
	std::size_t N = x.size();
    for(std::size_t i=0; i<N; i++)
        s += x[i];
    return s;
}

Float Util::dotProduct(const VectorFloat &a,const VectorFloat &b){
	if( a.size() != b.size() ) return std::numeric_limits< Float >::max();
	std::size_t N = a.size();
	Float d = 0;
	for(std::size_t i=0; i<N; i++){
		d += a[i]*b[i];
	}
	return d;
}

Float Util::euclideanDistance(const VectorFloat &a,const VectorFloat &b){
	if( a.size() != b.size() ) return std::numeric_limits< Float >::max();
	std::size_t N = a.size();
	Float d = 0;
	for(std::size_t i=0; i<N; i++){
		d += (a[i]-b[i])*(a[i]-b[i]);
	}
	return sqrt( d );
}

Float Util::squaredEuclideanDistance(const VectorFloat &a,const VectorFloat &b){
    if( a.size() != b.size() ) return std::numeric_limits< Float >::max();
    std::size_t N = a.size();
    Float d = 0;
    for(std::size_t i=0; i<N; i++){
        d += (a[i]-b[i])*(a[i]-b[i]);
    }
    return d;
}

Float Util::manhattanDistance(const VectorFloat &a,const VectorFloat &b){
	if( a.size() != b.size() ) return std::numeric_limits< Float >::max();
	std::size_t N = a.size();
	Float d = 0;
	for(std::size_t i=0; i<N; i++){
		d += fabs(a[i]-b[i]);
	}
	return d;
}

Float Util::cosineDistance(const VectorFloat &a,const VectorFloat &b){
	if( a.size() != b.size() ) return std::numeric_limits< Float >::max();
	std::size_t N = a.size();
	Float dotProduct = 0;
	Float aSum = 0;
	Float bSum = 0;
	for(std::size_t i=0; i<N; i++){
		dotProduct += a[i]*b[i];
		aSum += a[i]*a[i];
		bSum += b[i]*b[i];
	}
	return dotProduct / sqrt(aSum*bSum);
}
    
VectorFloat Util::scale(const VectorFloat &x,const Float minSource,const Float maxSource,const Float minTarget,const Float maxTarget,const bool constrain){
    std::size_t N = x.size();
	VectorFloat y(N);
    for(std::size_t i=0; i<N; i++){
        y[i] = scale(x[i],minSource,maxSource,minTarget,maxTarget,constrain);
    }
    return y;
}

VectorFloat Util::normalize(const VectorFloat &x){
	std::size_t N = x.size();
	VectorFloat y(N);
	Float s = 0;
    for(std::size_t i=0; i<N; i++)
        s += x[i];
	
	if( s != 0 ){
		for(std::size_t i=0; i<N; i++)
			y[i] = x[i] / s;
	}else{
		for(std::size_t i=0; i<N; i++)
			y[i] = 0;
	}
	return y;
}
    
VectorFloat Util::limit(const VectorFloat &x,const Float minValue,const Float maxValue){
    std::size_t N = x.size();
	VectorFloat y(N);
    for(std::size_t i=0; i<N; i++)
        y[i] = limit(x[i],minValue,maxValue);
    return y;
}

Float Util::getMin(const VectorFloat &x){
    Float min = std::numeric_limits< Float >::max();
    std::size_t N = x.size();
    for(std::size_t i=0; i<N; i++){
        if( x[i] < min ){
            min = x[i];
        }
    }
    return min;
}
    
unsigned int getMinIndex(const VectorFloat &x){
    unsigned int minIndex = 0;
    Float min = std::numeric_limits< Float >::max();
    unsigned int N = (unsigned int)x.size();
    for(unsigned int i=0; i<N; i++){
        if( x[i] < min ){
            min = x[i];
            minIndex = i;
        }
    }
    return minIndex;
}

Float Util::getMax(const VectorFloat &x){
    Float max = std::numeric_limits< Float >::min();
    std::size_t N = x.size();
    for(std::size_t i=0; i<N; i++){
        if( x[i] > max ){
            max = x[i];
        }
    }
    return max;
}
    
unsigned int Util::getMaxIndex(const VectorFloat &x){
    unsigned int maxIndex = 0;
    Float max = std::numeric_limits< Float >::min();
    unsigned int N = (unsigned int)x.size();
    for(unsigned int i=0; i<N; i++){
        if( x[i] > max ){
            max = x[i];
            maxIndex = i;
        }
    }
    return maxIndex;
}

unsigned int Util::getMin(const std::vector< unsigned int > &x){
    unsigned int min = std::numeric_limits< unsigned int >::max();
    const std::size_t N = x.size();
    for(std::size_t i=0; i<N; i++){
        if( x[i] < min ){
            min = x[i];
        }
    }
    return min;
}

unsigned int Util::getMax(const std::vector< unsigned int > &x){
    unsigned int max = std::numeric_limits< unsigned int >::min();
    const std::size_t N = x.size();
    for(size_t i=0; i<N; i++){
        if( x[i] > max ){
            max = x[i];
        }
    }
    return max;
}

unsigned int Util::getOS(){
#ifdef __GRT_OSX_BUILD__
    return OS_OSX;
#endif
	
#ifdef __GRT_LINUX_BUILD__
    return OS_LINUX;
#endif
	
#ifdef __GRT_WINDOWS_BUILD__
    return OS_WINDOWS;
#endif
	
	return OS_UNKNOWN;
}
    
void Util::cartToPolar(const Float x,const Float y,Float &r, Float &theta){
    
#ifndef PI
    Float PI = 3.14159265358979323846;
#endif
    
#ifndef TWO_PI
    Float TWO_PI = 6.28318530718;
#endif
    
    r = 0;
    theta = 0;
    
    //Compute r
    r = sqrt( (x*x) + (y*y) );
    
    //Compute theta
    int type = 0;
    if( x > 0 && y >= 0) type = 1;
    if( x > 0 && y < 0 ) type = 2;
    if( x < 0 ) type = 3;
    if( x==0 && y > 0 ) type = 4;
    if( x==0 && y < 0 ) type = 5;
    if( x==0 && y==0 ) type = 6;
    
    switch(type){
        case(1):
            theta = atan( y/x ) * (180.0/PI);
            break;
        case(2):
            theta = (atan( y/x ) + TWO_PI) * (180.0/PI);
            break;
        case(3):
            theta = (atan( y/x ) + PI) * (180.0/PI);
            break;
        case(4):
            theta = (PI/2.0) * (180.0/PI);
            break;
        case(5):
            theta = ( (3*PI)/2.0 ) * (180.0/PI);
            break;
        case(6):
            theta = 0.0;
            break;
        default:
            theta=0.0;
            break;
    } 
}
    
void Util::polarToCart(const Float r,const Float theta,Float &x, Float &y){
    x = r * cos(theta);
    y = r * sin(theta);
}

bool Util::parseDirectory( const std::string directoryPath, const std::string type, std::vector< std::string > &filenames ){

    filenames.clear();

#if defined( __GRT_WINDOWS_BUILD__ )
    return false; //Windows not supported yet
#endif

#if defined( __GRT_OSX_BUILD__ ) || defined( __GRT_LINUX_BUILD__ )

    std::vector< std::string > types; //Stores the file types 

    //Parse out the types to search for, types should be seperated by |, e.g. .csv|.grt
    std::string temp = "";
    unsigned int i = 0;
    while( i < type.length() ){
        if( type[i] == '|' ){
            types.push_back( temp );
            temp = "";
        }else{
            temp += type[i];
        }
        i++;
    }
    if( temp.length() > 0 ) types.push_back( temp );
    unsigned int numTypes = (unsigned int)types.size();

    //Search the directory for the files
    DIR *dir;
    struct dirent *ent;
    bool matchFound = false;
    std::string f = "";
    if ((dir = opendir ( directoryPath.c_str() )) != NULL) {
      while ((ent = readdir (dir)) != NULL) {
          f = ent->d_name;
          matchFound = false;
          for(i=0; i<numTypes; i++){
            if( f != "." && f != ".." ){
                if( GRT::Util::stringEndsWith(f,types[i]) || types[i] == ".*" ){
                    matchFound = true;
                    break;
                }
            }
          }
          if( matchFound ){
              filenames.push_back( directoryPath + "/" + ent->d_name );
          }
      }
      closedir (dir);
    } else {
      //Failed to open directory
      perror ("");
      return false;
    }

#endif

    return true;
}

} //End of GRT namespace

