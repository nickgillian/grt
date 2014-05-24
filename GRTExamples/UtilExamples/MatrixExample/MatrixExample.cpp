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
    //Create an empty matrix for double data
    Matrix<double> mat;
    
    //Resize the matrix
    mat.resize( 100, 2 );
    
    //Set all the values in the matrix to zero
    mat.setAllValues( 0 );
    
    //Clear all the data
    mat.clear();
    
    //Resize the matrix again
    mat.resize(50, 2);
    
    //Loop over the data and set the values to random values
    UINT counter = 0;
    for(UINT i=0; i<mat.getNumRows(); i++){
        for(UINT j=0; j<mat.getNumCols(); j++){
            mat[i][j] = counter++;
        }
    }
    
    //Add a new row at the very end of the matrix
    VectorDouble newRow(2);
    newRow[0] = 1000;
    newRow[1] = 2000;
    mat.push_back( newRow );

    //Reserve a large block of data (1000 rows) such it will be faster to push_back new rows
	mat.reserve( 1000 );
	
	for(UINT i=0; i<100; i++)
		mat.push_back( newRow );
    
    //Print the values
    cout << "Matrix Data: \n";
    for(UINT i=0; i<mat.getNumRows(); i++){
        for(UINT j=0; j<mat.getNumCols(); j++){
            cout << mat[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
    
    //Get the second row as a vector
    VectorDouble rowVector = mat.getRowVector( 1 );
    
    cout << "Row Vector Data: \n";
    for(UINT i=0; i<rowVector.size(); i++){
        cout << rowVector[i] << "\t";
    }
    cout << endl;
    
    //Get the second column as a vector
    VectorDouble colVector = mat.getColVector( 1 );
    
    cout << "Column Vector Data: \n";
    for(UINT i=0; i<colVector.size(); i++){
        cout << colVector[i] << "\n";
    }
    cout << endl;
    
    //Get the entire matrix as a concatenated vector
    VectorDouble concatVector = mat.getConcatenatedVector();
    
    cout << "Concatenated Vector Data: \n";
    for(UINT i=0; i<concatVector.size(); i++){
        cout << concatVector[i] << "\n";
    }
    cout << endl;
    
    return EXIT_SUCCESS;
}

