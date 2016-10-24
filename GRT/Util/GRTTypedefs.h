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

#ifndef GRT_TYPEDEFS_HEADER
#define GRT_TYPEDEFS_HEADER

#include "GRTVersionInfo.h"
#include <iostream>
#include <iterator>     // std::front_inserter
#include <vector>
#include <algorithm>  	// std::copy
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include <cmath>

#ifdef __GRT_WINDOWS_BUILD__
#define NOMINMAX
#include <windows.h>
#endif

#define GRT_BEGIN_NAMESPACE namespace GRT {
#define GRT_END_NAMESPACE }

GRT_BEGIN_NAMESPACE

//Define any common GRT OS independent typedefs
typedef double Float; ///<This typedef is used to set floating-point precision throughout the GRT
typedef long double LongFloat; ///<This typedef is used to set long floating-point precision throughout the GRT
	
//Declare any common definitions that are not OS specific
#ifndef PI
#define PI 3.14159265358979323846264338327950288
#endif
    
#ifndef TWO_PI
#define TWO_PI 6.28318530718
#endif

#ifndef ONE_OVER_TWO_PI
#define ONE_OVER_TWO_PI (1.0/TWO_PI)
#endif
    
#ifndef SQRT_TWO_PI
#define SQRT_TWO_PI 2.506628274631
#endif
	
template<class T> inline T SQR(const T &a) {return a*a;}
template<class T> inline void SWAP(T &a,T &b) { T temp(a); a = b; b = temp; }
template<class T> inline T SIGN(const T &a, const T &b) {return (b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a));}

template<class T> inline void grt_swap(T &a,T &b) { T temp(a); a = b; b = temp; }

template< class T >
class grt_numeric_limits{
public:
    static T min() { return std::numeric_limits< T >::min(); }
    static T max() { return std::numeric_limits< T >::max(); }
};

inline Float grt_sqr( const Float &x ){ return x*x; }

inline Float grt_sqrt( const Float &x ){ return sqrt(x); }

inline Float grt_antilog( const Float &x ){ return exp( x ); }

inline Float grt_exp( const Float &x ){ return exp( x ); }

inline Float  grt_log( const Float &x ){ return log( x ); }

inline Float grt_sigmoid( const Float &x ) { return 1.0 / (1.0 + exp(-x)); }

template< class T >
T grt_scale(const T &x,const T &minSource,const T &maxSource,const T &minTarget,const T &maxTarget,const bool constrain = false){
    if( constrain ){
        if( x <= minSource ) return minTarget;
        if( x >= maxSource ) return maxTarget;
    }
    if( minSource == maxSource ) return minTarget;
    return (((x-minSource)*(maxTarget-minTarget))/(maxSource-minSource))+minTarget;
}

template< class T >
std::string grt_to_str( const T &value ){
    std::ostringstream s;
    s << value;
    return s.str();
}

template< class T >
T grt_from_str( const std::string &str ){
    std::stringstream s( str );
    T i;
    s >> i;
    return i;
}

#define grt_min(a,b) (((a)<(b))?(a):(b))
#define grt_max(a,b) (((a)>(b))?(a):(b))

#define GRT_DEFAULT_NULL_CLASS_LABEL 0
#define GRT_SAFE_CHECKING true

//No need for this on non-Windows platforms, but
//must have an empty definition.
#define GRT_API

//Specific defines for Windows
#ifdef __GRT_WINDOWS_BUILD__
#define grt_isnan(x) (x != x)
#define grt_isinf(x) (!grt_isnan(x) && grt_isnan(x - x))

//NAN is not defined on Visual Studio version of math.h so define it here
#ifndef NAN
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const float *) __nan)
#endif

#ifndef INFINITY
#define INFINITY (DBL_MAX+DBL_MAX)
#endif

//Remove the min and max macros as they cause lots of issues
#ifndef NOMINMAX
#define NOMINMAX
#endif 

//Allow DLL exports.
#if !defined(GRT_STATIC_LIB) && defined(_MSC_VER)
	#undef GRT_API
	#ifdef GRT_DLL_EXPORTS
		#define GRT_API __declspec(dllexport)
	#else // GRT_DLL_EXPORTS
		#define GRT_API __declspec(dllimport)
	#endif // GRT_DLL_EXPORTS

	//disable warnings about a "dllexport" class using a regular class
	# pragma warning(disable: 4251)
#endif // !GRT_STATIC_LIB && MSC_VER

#endif
	
//Specific defines for OSX
#ifdef __GRT_OSX_BUILD__
#define grt_isnan(x) (x != x)
#define grt_isinf(x) (!grt_isnan(x) && grt_isnan(x - x))
	
typedef unsigned int UINT;
typedef signed int SINT;
typedef unsigned long ULONG;
#endif
	
//Specific defines for Linux
#ifdef __GRT_LINUX_BUILD__
#define grt_isnan(x) (x != x)
#define grt_isinf(x) (!grt_isnan(x) && grt_isnan(x - x))

typedef unsigned int UINT;
typedef signed int SINT;
typedef unsigned long ULONG;
#endif

// Cross-platform deprecation warning, based on openFrameworks OF_DEPRECATED
#ifdef __GNUC__
// clang also has this defined. deprecated(message) is only for gcc>=4.5
#if (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 5)
#define GRT_DEPRECATED_MSG(message, func) func __attribute__ ((deprecated(message)))
#else
#define GRT_DEPRECATED_MSG(message, func) func __attribute__ ((deprecated))
#endif
#define GRT_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define GRT_DEPRECATED_MSG(message, func) __declspec(deprecated(message)) func
#define GRT_DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define GRT_DEPRECATED_MSG(message, func) func
#define GRT_DEPRECATED(func) func
#endif

/**
  @brief returns the filename (stripped of the system path) of the file using this macro
*/
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/**
  @brief converts x to a string for printing
*/
#define grt_to_str(x) (#x)

#if !defined NDEBUG
/**
  @brief custom GRT assert function
*/
#if defined(_MSC_VER) && _MSC_VER <= 1800
//Use __FUNCTION__ instead of __func__ for Visual Studio 2013 & earlier
#define grt_assert(x) \
do { \
if (0 == (x)) { \
fprintf(stderr, "Assertion failed: %s, %s(), %d at \'%s\'\n", __FILENAME__, __FUNCTION__, __LINE__, grt_to_str(x) ); \
abort(); \
} \
} while (0)
#else // _MSC_VER <= 1800
#define grt_assert(x) \
do { \
if (0 == (x)) { \
fprintf(stderr, "Assertion failed: %s, %s(), %d at \'%s\'\n", __FILENAME__, __func__, __LINE__, grt_to_str(x) ); \
abort(); \
} \
} while (0)
#endif // _MSC_VER <= 1800
#else // !NDEBUG
#define grt_assert(x)
#endif // !NDEBUG
    
//Declare typedefs for the legacy data types
class ClassificationData;
class ClassificationDataStream;
class RegressionData;
class TimeSeriesClassificationData;
class UnlabelledData;
class VectorFloat;
class MatrixFloat;
typedef ClassificationData LabelledClassificationData;
typedef RegressionData LabelledRegressionData;
typedef TimeSeriesClassificationData LabelledTimeSeriesClassificationData;
typedef ClassificationDataStream TimeSeriesClassificationDataStream;
typedef UnlabelledData UnlabelledClassificationData;
typedef VectorFloat VectorDouble;
typedef MatrixFloat MatrixDouble;

GRT_END_NAMESPACE

#endif //GRT_TYPEDEFS_HEADER
