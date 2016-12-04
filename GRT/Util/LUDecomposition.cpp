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
#include "LUDecomposition.h"

GRT_BEGIN_NAMESPACE

LUDecomposition::LUDecomposition(const MatrixFloat &a) : sing(false){
    
    errorLog.setKey("[ERROR LUDecomposition]");
    warningLog.setKey("[WARNING LUDecomposition]");

	N = a.getNumRows();
	lu = a;
	aref = a;
	indx.resize( N );

	const Float TINY=1.0e-20;
	unsigned int i,imax,j,k;
	Float big,temp;
	VectorFloat vv(N);
	d=1.0;
    imax = 0;
	for (i=0;i<N;i++) {
		big=0.0;
		for (j=0;j<N;j++)
			if ((temp=fabs( lu[i][j] )) > big) big=temp;
		if (big == 0.0){
            sing = true;
            errorLog << "Error in LUDecomposition constructor, big == 0.0" << std::endl;
            return;
		}
		vv[i] = 1.0/big;
	}
	for (k=0;k<N;k++) {
		big=0.0;
		for (i=k;i<N;i++) {
			temp=vv[i]*fabs(lu[i][k]);
			if (temp > big) {
				big=temp;
				imax=i;
			}
		}
		if (k != imax) {
			for (j=0;j<N;j++) {
				temp=lu[imax][j];
				lu[imax][j] = lu[k][j];
				lu[k][j] = temp;
			}
			d = -d;
			vv[imax]=vv[k];
		}
		indx[k]=imax;
		if (lu[k][k] == 0.0) lu[k][k] = TINY;
		for (i=k+1; i<N; i++) {
			temp = lu[i][k] /= lu[k][k];
			for (j=k+1;j<N;j++)
				lu[i][j] -= temp * lu[k][j];
		}
	}
	
}
    
LUDecomposition::~LUDecomposition(){

}
    
bool LUDecomposition::solve_vector(const VectorFloat &b,VectorFloat &x)
{
	int i=0,ii=0,ip=0,j=0; //This must be an int (as opposed to an UINT)
	const int n = int(N);
	Float sum=0;
    
	if (b.size() != N || x.size() != N){
        errorLog << "solve_vector(const VectorFloat &b,VectorFloat &x) - the size of the two vectors does not match!" << std::endl;
		return false;
    }
	for (i=0;i<n;i++) x[i] = b[i];
	for (i=0;i<n;i++) {
		ip=indx[i];
		sum=x[ip];
		x[ip] = x[i];
		if (ii != 0)
			for (j=ii-1;j<i;j++) sum -= lu[i][j] * x[j];
		else if (sum != 0.0)
			ii=i+1;
		x[i]=sum;
	}
	for (i=n-1;i>=0;i--) {
		sum=x[i];
		for (j=i+1;j<n;j++) sum -= lu[i][j] * x[j];
		x[i] = sum / lu[i][i];
	}
    
    return true;
}

bool LUDecomposition::solve(const MatrixFloat &b,MatrixFloat &x)
{
	unsigned int m=b.getNumCols();
	if (b.getNumRows() != N || x.getNumRows() != N || b.getNumCols() != x.getNumCols() ){
        errorLog << "solve(const MatrixFloat &b,MatrixFloat &x) - the size of the two matrices does not match!" << std::endl;
		return false;
    }
	VectorFloat  xx(N);
	for (unsigned int j=0; j<m; j++) {
		for(unsigned int i=0; i<N; i++) xx[i] = b[i][j];
		solve_vector(xx,xx);
		for(unsigned int i=0; i<N; i++) x[i][j] = xx[i];
	}
    return true;
}
    
bool LUDecomposition::inverse(MatrixFloat &ainv)
{
	unsigned int i,j;
	ainv.resize(N,N);
	for (i=0;i<N;i++) {
		for (j=0;j<N;j++) ainv[i][j] = 0.0;
		ainv[i][i] = 1.0;
	}
	return solve(ainv,ainv);
}
    
Float LUDecomposition::det()
{
	Float dd = d;
	for (unsigned int i=0;i<N;i++) dd *= lu[i][i];
	return dd;
}
    
bool LUDecomposition::mprove(const VectorFloat &b,VectorFloat &x)
{
	unsigned int i,j;
	VectorFloat r(N);
	LongFloat sdp;
	for (i=0;i<N;i++) {
		sdp = -b[i];
		for (j=0;j<N;j++)
			sdp += (LongFloat) aref[i][j] * (LongFloat)x[j];
		r[i]=sdp;
	}
	if( !solve_vector(r,r) ){
        return false;
    }
	for (i=0;i<N;i++) x[i] -= r[i];
    return true;
}

bool LUDecomposition::getIsSingular(){
	return sing;
}

MatrixFloat LUDecomposition::getLU(){
	return lu;
}
    
GRT_END_NAMESPACE
