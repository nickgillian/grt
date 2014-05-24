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

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
	//Create a matrix for the test data
	Matrix<double> data(4,2);
	
	//Populate the test data
	data[0][0] = 1;	data[0][1] = 2;
	data[1][0] = 3;	data[1][1] = 4;
	data[2][0] = 5;	data[2][1] = 6;
	data[3][0] = 7;	data[3][1] = 8;
	
	cout << "Data:\n";
	for(UINT i=0; i<data.getNumRows(); i++){
		for(UINT j=0; j<data.getNumCols(); j++){
			cout << data[i][j] << "\t";
		}
		cout << endl;
	}
	
	//Create a new instance of the SVD class
	SVD svd;
	
	//Computes the singular value decomposition of the data matrix
	if( !svd.solve(data) ){
		cout << "ERROR: Failed to solve SVD solution!\n";
		return EXIT_FAILURE;
	}
	
	//Get the U, V, and W results (V is sometimes called S in other packages like Matlab)
	Matrix<double> u = svd.getU();
	Matrix<double> v = svd.getV();
	vector<double> w = svd.getW();
	
	cout << "U:\n";
	for(UINT i=0; i<u.getNumRows(); i++){
		for(UINT j=0; j<u.getNumCols(); j++){
			cout << u[i][j] << "\t";
		}
		cout << endl;
	}
	
	cout << "V:\n";
	for(UINT i=0; i<v.getNumRows(); i++){
		for(UINT j=0; j<v.getNumCols(); j++){
			cout << v[i][j] << "\t";
		}
		cout << endl;
	}
	
	cout << "W:\n";
	for(UINT i=0; i<w.size(); i++){
		cout << w[i] << "\t";
	}
	cout << endl;
	
    return EXIT_SUCCESS;
}

