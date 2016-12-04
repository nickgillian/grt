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
 
 This code is based on the LU Decomposition code from Numerical Recipes (3rd Edition)
 
*/

#define GRT_DLL_EXPORTS
#include "Cholesky.h"

GRT_BEGIN_NAMESPACE

Cholesky::Cholesky(){
    errorLog.setKey("[ERROR LUdcmp]");
    warningLog.setKey("[WARNING LUdcmp]");
    success = false;
	N = 0;
}

Cholesky::Cholesky(MatrixFloat &a) : N(a.getNumRows()), el(a) {
    
    errorLog.setKey("[ERROR LUdcmp]");
    warningLog.setKey("[WARNING LUdcmp]");
    success = false;
    
	int i,j,k; //k has to an int (rather than a UINT)
 	VectorFloat tmp;
	Float sum = 0;
	if( el.getNumCols() != N ){
        errorLog << "The input matrix is not square!" << std::endl;
		return;
	}

	const int n = int(N);
	for (i=0;i<n;i++) {
		for (j=i;j<n;j++) {
			for (sum=el[i][j],k=i-1;k>=0;k--) sum -= el[i][k]*el[j][k];
			if (i == j) {
				if (sum <= 0.0){
					errorLog << "Sum is <=0.0" << std::endl;
                    return;
				}
				el[i][i]=sqrt(sum);
			}else el[j][i]=sum/el[i][i];
		}
	}
	for(i=0; i<n; i++) 
		for (j=0; j<i; j++) 
			el[j][i] = 0.;
    
    success = true;
}

bool Cholesky::solve(VectorFloat &b,VectorFloat &x) {
	int i,k;
	const int n = int(N);
	Float sum;
	
	if (b.size() != N || x.size() != N){
		errorLog << ":solve(vector<Float> &b, vector<Float> &x) - The input vectors are not the same size!" << std::endl;
		return false;
	}
	for(i=0; i<n; i++) {
		for(sum=b[i],k=i-1;k>=0;k--) sum -= el[i][k]*x[k];
		x[i]=sum/el[i][i];
	}
	for (i=n-1; i>=0; i--) {
		for (sum=x[i],k=i+1;k<n;k++) sum -= el[k][i]*x[k];
		x[i]=sum/el[i][i];
	}		
    return true;
}

bool Cholesky::elmult(VectorFloat &y,VectorFloat &b){
	unsigned int i,j;
	if (b.size() != N || y.size() != N){
		errorLog << "elmult(vector<Float> &y vector<Float> &b) - The input vectors are not the same size!" << std::endl;
		return false;
	}
	for (i=0;i<N;i++) {
		b[i] = 0.;
		for (j=0; j<=i; j++) b[i] += el[i][j]*y[j];
	}
    return true;
}

bool Cholesky::elsolve(VectorFloat &b,VectorFloat &y){
	UINT i,j;
	Float sum = 0;
	
	if (b.size() != N || y.size() != N){
		errorLog << "elsolve(vector<Float> &b vector<Float> &y) - The input vectors are not the same size!" << std::endl;
		return false;
	}
	for (i=0; i<N; i++) {
		for (sum=b[i],j=0; j<i; j++) sum -= el[i][j]*y[j];
		y[i] = sum/el[i][i];
	}
    return true;
}

bool Cholesky::inverse(MatrixFloat &ainv){
	int i,j,k;
	const int n = int(N);
	Float sum = 0;
	ainv.resize(N,N);
	
	for(i=0; i<n; i++) for(j=0; j<=i; j++){
		sum = (i==j? 1. : 0.);
		for(k=i-1; k>=j; k--) sum -= el[i][k]*ainv[j][k];
		ainv[j][i]= sum/el[i][i];
	}
	for(i=n-1; i>=0; i--) for(j=0; j<=i; j++){
		sum = (i<j? 0. : ainv[j][i]);
		for(k=i+1; k<n; k++) sum -= el[k][i]*ainv[j][k];
		ainv[i][j] = ainv[j][i] = sum/el[i][i];
	}		
    return true;
}

Float Cholesky::logdet(){
	Float sum = 0.;
	for(unsigned int i=0; i<N; i++) sum += log(el[i][i]);
	return 2.*sum;
}

GRT_END_NAMESPACE
