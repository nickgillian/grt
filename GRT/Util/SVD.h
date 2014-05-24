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

namespace GRT{

#define MAX_NUM_SVD_ITER 75

class SVD {
 public:
	
	SVD(){}
	~SVD(){}
	
	bool solve(Matrix<double> &a);
	
	Matrix<double> getU(){ return u; }
	Matrix<double> getV(){ return v; }
	vector<double> getW(){ return w; }
	
protected:
	bool solveVector(vector <double> &b, vector <double> &x, double thresh = -1.);
	bool solve(Matrix <double> &b, Matrix <double> &x, double thresh = -1.);

	UINT rank(double thresh = -1.);
	UINT nullity(double thresh = -1.);
	Matrix <double> range(double thresh = -1.);
	Matrix <double> nullspace(double thresh = -1.);

	double inv_condition();
	bool decompose();
	bool reorder();
	double pythag(const double a, const double b);
	
	UINT m,n;
	Matrix <double> u,v;
	vector <double> w;
	double eps, tsh;
};

}//End of namespace GRT

#endif //GRT_SVD_HEADER
