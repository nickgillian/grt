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
 
 This code is based on the LU Decomposition code from Numerical Recipes (3rd Edition).
 
 */
#ifndef GRT_LUDCMP_HEADER
#define GRT_LUDCMP_HEADER

#include "../DataStructures/VectorFloat.h"
#include "../DataStructures/MatrixFloat.h"

GRT_BEGIN_NAMESPACE
    
class GRT_API LUDecomposition{
	
public:
    LUDecomposition(const MatrixFloat &a);
	~LUDecomposition();
	bool solve_vector(const VectorFloat &b,VectorFloat &x);
	bool solve(const MatrixFloat &b,MatrixFloat &x);
	bool inverse(MatrixFloat &ainv);
	Float det();
	bool mprove(const VectorFloat &b,VectorFloat &x);
	bool getIsSingular();
	MatrixFloat getLU();
	
protected:
	unsigned int N;
	Float d;
	bool sing;
	Vector< int > indx;
	MatrixFloat aref;
	MatrixFloat lu;
    
    ErrorLog errorLog;
    WarningLog warningLog;
};

GRT_END_NAMESPACE

#endif //GRT_LUDCMP_HEADER
