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

#ifndef GRT_SVD_HEADER
#define GRT_SVD_HEADER

#include "GRTCommon.h"

GRT_BEGIN_NAMESPACE

#define MAX_NUM_SVD_ITER 75

class GRT_API SVD {
 public:
	
	SVD(){}
	~SVD(){}
	
	bool solve(MatrixFloat &a);
	
	MatrixFloat getU(){ return u; }
	MatrixFloat getV(){ return v; }
	VectorFloat getW(){ return w; }
	
protected:
	bool solveVector(VectorFloat &b, VectorFloat &x, Float thresh = -1.);
	bool solve(MatrixFloat &b, MatrixFloat &x, Float thresh = -1.);

	UINT rank(Float thresh = -1.);
	UINT nullity(Float thresh = -1.);
	MatrixFloat range(Float thresh = -1.);
	MatrixFloat nullspace(Float thresh = -1.);

	Float inv_condition();
	bool decompose();
	bool reorder();
	Float pythag(const Float a, const Float b);
	
	UINT m,n;
	MatrixFloat u,v;
	VectorFloat w;
	Float eps, tsh;
};

GRT_END_NAMESPACE

#endif //GRT_SVD_HEADER
