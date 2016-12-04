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

#ifndef GRT_COMMON_HEADER
#define GRT_COMMON_HEADER

//Include the common C++ headers
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <float.h>
#include <cmath>
#include <math.h>
#include <time.h>
#include <map>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <cstddef>

//Only include the C++ 11 code if C++11 support it is enabled
#ifdef GRT_CXX11_ENABLED
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <random>
#endif //GRT_CXX11_ENABLED

//Include the main GRT version and typedef headers
#include "GRTVersionInfo.h"
#include "GRTTypedefs.h"

//Include the common GRT classes
#include "GRTException.h"
#include "MinMax.h"
#include "ClassTracker.h"
#include "IndexedDouble.h"
#include "DebugLog.h"
#include "ErrorLog.h"
#include "InfoLog.h"
#include "TrainingLog.h"
#include "TestingLog.h"
#include "WarningLog.h"
#include "../DataStructures/VectorFloat.h"
#include "../DataStructures/MatrixFloat.h"
#include "TrainingResult.h"
#include "TestInstanceResult.h"
#include "TestResult.h"
#include "CircularBuffer.h"
#include "Timer.h"
#include "TimeStamp.h"
#include "Random.h"
#include "Util.h"
#include "EigenvalueDecomposition.h"
#include "Cholesky.h"
#include "LUDecomposition.h"
#include "SVD.h"
#include "FileParser.h"
#include "ObserverManager.h"
#include "ThreadPool.h"
#include "DataType.h"
#include "DynamicType.h"
#include "Dict.h"
#include "Metrics.h"

GRT_BEGIN_NAMESPACE

#if _MSC_VER >= 1600
    inline Float round( Float d ) {
        return floor( d + 0.5 );
    }
#endif

GRT_END_NAMESPACE

#endif //GRT_COMMON_HEADER
