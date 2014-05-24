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

#include "Util.h"

namespace GRT{
    
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
    
}
    
double Util::scale(const double &x,const double &minSource,const double &maxSource,const double &minTarget,const double &maxTarget,const bool constrain){
    if( constrain ){
        if( x <= minSource ) return minTarget;
        if( x >= maxSource ) return maxTarget;
    }
    if( minSource == maxSource ) return minTarget;
    return (((x-minSource)*(maxTarget-minTarget))/(maxSource-minSource))+minTarget;
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

std::string Util::toString(const double &v){
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
    
double Util::limit(const double value,const double minValue,const double maxValue){
    if( value <= minValue ) return minValue;
    if( value >= maxValue ) return maxValue;
    return value;
}
    
double Util::sum(const std::vector< double > &x){
    double s = 0;
	std::size_t N = x.size();
    for(std::size_t i=0; i<N; i++)
        s += x[i];
    return s;
}

double Util::dotProduct(const std::vector<double> &a,const std::vector<double> &b){
	if( a.size() != b.size() ) return std::numeric_limits< double >::max();
	std::size_t N = a.size();
	double d = 0;
	for(std::size_t i=0; i<N; i++){
		d += a[i]*b[i];
	}
	return d;
}

double Util::euclideanDistance(const std::vector<double> &a,const std::vector<double> &b){
	if( a.size() != b.size() ) return std::numeric_limits< double >::max();
	std::size_t N = a.size();
	double d = 0;
	for(std::size_t i=0; i<N; i++){
		d += (a[i]-b[i])*(a[i]-b[i]);
	}
	return sqrt( d );
}

double Util::manhattanDistance(const std::vector<double> &a,const std::vector<double> &b){
	if( a.size() != b.size() ) return std::numeric_limits< double >::max();
	std::size_t N = a.size();
	double d = 0;
	for(std::size_t i=0; i<N; i++){
		d += fabs(a[i]-b[i]);
	}
	return d;
}

double Util::cosineDistance(const std::vector<double> &a,const std::vector<double> &b){
	if( a.size() != b.size() ) return std::numeric_limits< double >::max();
	std::size_t N = a.size();
	double dotProduct = 0;
	double aSum = 0;
	double bSum = 0;
	for(std::size_t i=0; i<N; i++){
		dotProduct += a[i]*b[i];
		aSum += a[i]*a[i];
		bSum += b[i]*b[i];
	}
	return dotProduct / sqrt(aSum*bSum);
}
    
std::vector<double> Util::scale(const std::vector<double> &x,const double minSource,const double maxSource,const double minTarget,const double maxTarget,const bool constrain){
    std::size_t N = x.size();
	std::vector<double> y(N);
    for(std::size_t i=0; i<N; i++){
        y[i] = scale(x[i],minSource,maxSource,minTarget,maxTarget,constrain);
    }
    return y;
}

std::vector<double> Util::normalize(const std::vector<double> &x){
	std::size_t N = x.size();
	std::vector<double> y(N);
	double s = 0;
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
    
std::vector<double> Util::limit(const std::vector<double> &x,const double minValue,const double maxValue){
    std::size_t N = x.size();
	std::vector<double> y(N);
    for(std::size_t i=0; i<N; i++)
        y[i] = limit(x[i],minValue,maxValue);
    return y;
}

double Util::getMin(const std::vector< double > &x){
    double min = std::numeric_limits< double >::max();
    std::size_t N = x.size();
    for(std::size_t i=0; i<N; i++){
        if( x[i] < min ){
            min = x[i];
        }
    }
    return min;
}

double Util::getMax(const std::vector< double > &x){
    double max = std::numeric_limits< double >::min();
    std::size_t N = x.size();
    for(std::size_t i=0; i<N; i++){
        if( x[i] > max ){
            max = x[i];
        }
    }
    return max;
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
    
void Util::cartToPolar(const double x,const double y,double &r, double &theta){
    
#ifndef PI
    double PI = 3.14159265358979323846;
#endif
    
#ifndef TWO_PI
    double TWO_PI = 6.28318530718;
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
    
void Util::polarToCart(const double r,const double theta,double &x, double &y){
    x = r * cos(theta);
    y = r * sin(theta);
}
    
}; //End of namespace GRT
