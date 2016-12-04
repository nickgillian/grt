/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This code is based on the LU Decomposition code from Numerical Recipes (3rd Edition)
 */


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

#ifndef GRT_CHOLESKY_HEADER
#define GRT_CHOLESKY_HEADER

#include "Log.h"
#include "DebugLog.h"
#include "ErrorLog.h"
#include "WarningLog.h"
#include "../DataStructures/VectorFloat.h"
#include "../DataStructures/MatrixFloat.h"

GRT_BEGIN_NAMESPACE

class GRT_API Cholesky{
public:
	Cholesky();
	Cholesky(MatrixFloat &a);
	bool solve(VectorFloat &b,VectorFloat &x);
	bool elmult(VectorFloat &y,VectorFloat &b);
	bool elsolve(VectorFloat &b,VectorFloat &y);
	bool inverse(MatrixFloat &ainv);
	Float logdet();
    
    bool getSuccess(){ return success; }

	unsigned int N;
    bool success;
	MatrixFloat el;
    
    ErrorLog errorLog;
    WarningLog warningLog;
};

GRT_END_NAMESPACE

#endif //GRT_CHOLESKY_HEADER
