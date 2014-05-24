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
    
    //Declare any common typedefs, some of these are already declared in windef.h so if we are using Windows then we don't need to declare them
#ifndef __GRT_WINDOWS_BUILD__
	typedef unsigned int UINT;
	typedef signed int SINT;
	typedef unsigned long ULONG;
#endif
    
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