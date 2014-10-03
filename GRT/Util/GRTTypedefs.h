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
#include <vector>

namespace GRT {
	
//Declare any common definitions that are not OS specific
#ifndef PI
    #define PI 3.14159265358979323846264338327950288
#endif
    
#ifndef TWO_PI
	#define TWO_PI 6.28318530718
#endif
    
#ifndef SQRT_TWO_PI
    #define SQRT_TWO_PI 2.506628274631
#endif
	
template<class T> inline T SQR(const T &a) {return a*a;}
template<class T> inline void SWAP(T &a,T &b) { T temp(a); a = b; b = temp; }

inline double SIGN(const double &a, const double &b) {return (b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a));}

inline double antilog(const double &x){ return exp( x ); }

#ifndef MIN
    #define	MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
    #define	MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define GRT_DEFAULT_NULL_CLASS_LABEL 0
#define GRT_SAFE_CHECKING true
	
	
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
	
#endif
	
#ifdef __GRT_OSX_BUILD__
#define grt_isnan(x) (x != x)
#define grt_isinf(x) (!grt_isnan(x) && grt_isnan(x - x))
	
typedef unsigned int UINT;
typedef signed int SINT;
typedef unsigned long ULONG;
#endif
	
#ifdef __GRT_LINUX_BUILD__
#define grt_isnan(x) (x != x)
#define grt_isinf(x) (!grt_isnan(x) && grt_isnan(x - x))

typedef unsigned int UINT;
typedef signed int SINT;
typedef unsigned long ULONG;
#endif
    
//Define any common GRT OS independent typedefs
typedef std::vector<double> VectorDouble;
    
//Declare typedefs for the legacy data types
class ClassificationData;
class RegressionData;
class TimeSeriesClassificationData;
class UnlabelledData;
typedef ClassificationData LabelledClassificationData;
typedef RegressionData LabelledRegressionData;
typedef TimeSeriesClassificationData LabelledTimeSeriesClassificationData;
typedef UnlabelledData UnlabelledClassificationData;
}

#endif //GRT_TYPEDEFS_HEADER