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

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Create an empty matrix double
    MatrixFloat matrix;
    
    //Resize the matrix
    const UINT numRows = 100;
    const UINT numCols = 2;
    matrix.resize( numRows, numCols );
    
    //Set all the values in the matrix to zero
    matrix.setAllValues( 0 );
    
    //Loop over the data and set the values to a basic incrementing value
    UINT counter = 0;
    for(UINT i=0; i<matrix.getNumRows(); i++){
        for(UINT j=0; j<matrix.getNumCols(); j++){
            matrix[i][j] = counter++;
        }
    }
    
    //Add a new row at the very end of the matrix
    VectorFloat newRow( numCols );
    newRow[0] = 1000;
    newRow[1] = 2000;
    matrix.push_back( newRow );
    
    //Print the values
    cout << "Matrix Data: \n";
    for(UINT i=0; i<matrix.getNumRows(); i++){
        for(UINT j=0; j<matrix.getNumCols(); j++){
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
    
    //Get the second row as a vector
    VectorFloat rowVector = matrix.getRow( 1 );
    
    cout << "Row Vector Data: \n";
    for(UINT i=0; i<rowVector.size(); i++){
        cout << rowVector[i] << "\t";
    }
    cout << endl;
    
    //Get the second column as a vector
    VectorFloat colVector = matrix.getCol( 1 );
    
    cout << "Column Vector Data: \n";
    for(UINT i=0; i<colVector.size(); i++){
        cout << colVector[i] << "\n";
    }
    cout << endl;
    
    //Get the mean of each column
	VectorFloat mean = matrix.getMean();
	
	cout << "Mean: \n";
    for(UINT i=0; i<mean.size(); i++){
        cout << mean[i] << "\n";
    }
    cout << endl;
	
	//Get the Standard Deviation of each column
	VectorFloat stdDev = matrix.getStdDev();
	
	cout << "StdDev: \n";
    for(UINT i=0; i<stdDev.size(); i++){
        cout << stdDev[i] << "\n";
    }
    cout << endl;
	
	//Get the covariance matrix
	MatrixFloat cov = matrix.getCovarianceMatrix();
	
	cout << "Covariance Matrix: \n";
    for(UINT i=0; i<cov.getNumRows(); i++){
        for(UINT j=0; j<cov.getNumCols(); j++){
            cout << cov[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;

	Vector< MinMax > ranges = matrix.getRanges();
	
	cout << "Ranges: \n";
    for(UINT i=0; i<ranges.size(); i++){
        cout << "i: " << i << "\tMinValue: " << ranges[i].minValue << "\tMaxValue:" << ranges[i].maxValue << "\n";
    }
    cout << endl;
    
    //Save the matrix data to a csv file
    matrix.save( "data.csv" );
    
    //load the matrix data from a csv file
    matrix.load( "data.csv" );
    
    return EXIT_SUCCESS;
}

