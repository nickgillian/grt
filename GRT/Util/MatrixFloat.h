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

#ifndef GRT_MATRIX_FLOAT_HEADER
#define GRT_MATRIX_FLOAT_HEADER

#include "GRTTypedefs.h"
#include "Matrix.h"
#include "MinMax.h"
#include "DebugLog.h"
#include "ErrorLog.h"
#include "WarningLog.h"
#include "FileParser.h"
#include "ErrorLog.h"

using namespace std;

namespace GRT{

class MatrixFloat : public Matrix< float_t >{
public:
    /**
     Default Constructor
     */
    MatrixFloat();
    
    /**
     Constructor, sets the size of the matrix to [rows cols]
     
     @param const UINT rows: sets the number of rows in the matrix, must be a value greater than zero
     @param const UINT cols: sets the number of columns in the matrix, must be a value greater than zero
     */
    MatrixFloat(const unsigned int rows,const unsigned int cols);
    
    /**
     Copy Constructor, copies the values from the rhs MatrixFloat to this MatrixFloat instance
     
     @param const MatrixFloat &rhs: the MatrixFloat from which the values will be copied
     */
    MatrixFloat(const MatrixFloat &rhs);
    
    /**
     Copy Constructor, copies the values from the rhs Matrix to this MatrixFloat instance
     
     @param const Matrix< float_t > &rhs: the Matrix from which the values will be copied
     */
    MatrixFloat(const Matrix< float_t > &rhs);
    
    /**
     Destructor, cleans up any memory
     */
    virtual ~MatrixFloat();
    
    /**
     Defines how the data from the rhs MatrixFloat should be copied to this MatrixFloat
     
     @param rhs: another instance of a MatrixFloat
     @return returns a reference to this instance of the MatrixFloat
     */
    MatrixFloat& operator=(const MatrixFloat &rhs);
    
    /**
     Defines how the data from the rhs Matrix< float_t > should be copied to this MatrixFloat
     
     @param rhs: an instance of a Matrix< float_t >
     @return returns a reference to this instance of the MatrixFloat
     */
    MatrixFloat& operator=(const Matrix< float_t > &rhs);
    
    /**
     Defines how the data from the rhs vector of VectorFloats should be copied to this MatrixFloat
     
     @param rhs: a vector of VectorFloats
     @return returns a reference to this instance of the MatrixFloat
     */
    MatrixFloat& operator=(const vector< VectorFloat > &rhs);
    
    /**
     Resizes the MatrixFloat to the new size of [rows cols]
     
     @param rows: the number of rows, must be greater than zero
     @param cols: the number of columns, must be greater than zero
     @return returns true or false, indicating if the resize was successful
     */
    //virtual bool resize(const unsigned int rows,const unsigned int cols);
    
    /**
     Saves the matrix to a CSV file.  This replaces the deprecated saveToCSVFile function.
     
     @param filename: the name of the CSV file
     @return returns true or false, indicating if the data was saved successful
     */
    bool save(const string &filename) const;
    
    /**
     Loads a matrix from a CSV file. This assumes that the data has been saved as rows and columns in the CSV file
     and that there are an equal number of columns per row.
     
     This replaces the deprecated loadFromCSVFile function.
     
     @param filename: the name of the CSV file
     @return returns true or false, indicating if the data was loaded successful
     */
    bool load(const string &filename,const char seperator = ',');
    
    /**
     Saves the matrix to a CSV file.
     
     @param filename: the name of the CSV file
     @return returns true or false, indicating if the data was saved successful
     */
    bool saveToCSVFile(const string &filename) const;
    
    /**
     Loads a matrix from a CSV file. This assumes that the data has been saved as rows and columns in the CSV file
     and that there are an equal number of columns per row.
     
     @param filename: the name of the CSV file
     @return returns true or false, indicating if the data was loaded successful
     */
    bool loadFromCSVFile(const string &filename,const char seperator = ',');
    
    /**
     Prints the MatrixFloat contents to std::cout
     
     @param title: sets the title of the data that will be printed to std::cout
     @return returns true or false, indicating if the print was successful
     */
    bool print(const string title="") const;
    
    /**
     Transposes the data.
     
     @return returns true or false, indicating if the transpose was successful
     */
    bool transpose();
    
    /**
     Scales the matrix to a new range given by the min and max targets.
     
     @return returns true if the matrix was scaled, false otherwise
     */
    bool scale(const float_t minTarget,const float_t maxTarget);
    
    /**
     Scales the matrix to a new range given by the min and max targets using the ranges as the source ranges.
     
     @return returns true if the matrix was scaled, false otherwise
     */
    bool scale(const vector< MinMax > &ranges,const float_t minTarget,const float_t maxTarget);
    
    /**
     Normalizes each row in the matrix by subtracting the row mean and dividing by the row standard deviation.
     A small amount (alpha) is added to the standard deviation to stop the normalization from exploding.
     
     @param alpha: a small value that will be added to the standard deviation
     @return returns true if the matrix was normalized, false otherwise
     */
    bool znorm(const float_t alpha = 0.001);
    
    /**
     Performs the multiplication of the data by the scalar value.
     
     @return returns a new MatrixFloat with the results from the multiplcation
     */
    MatrixFloat multiple(const float_t value) const;
    
    /**
     Performs the multiplcation of this matrix (a) by the vector b.
     This will return a new vector (c): c = a * b
     
     @param b: the vector to multiple with this matrix
     @return a VectorFloat with the results from the multiplcation
     */
    VectorFloat multiple(const VectorFloat &b) const;
    
    /**
     Performs the multiplcation of this matrix (a) by the matrix b.
     This will return a new matrix (c): c = a * b
     
     @param b: the matrix to multiple with this matrix
     @return a new MatrixFloat with the results from the multiplcation
     */
    MatrixFloat multiple(const MatrixFloat &b) const;
    
    /**
     Performs the multiplcation of the matrix a by the matrix b, directly storing the new data in the this matrix instance.
     This will resize the current matrix if needed.
     This makes this matrix c and gives: c = a * b, or if the aTransposed value is true: c = a' * b
     
     @param a: the matrix to multiple with b
     @param b: the matrix to multiple with a
     @param aTranspose: a flag to indicate if matrix a should be transposed
     @return true if the operation was completed successfully, false otherwise
     */
    bool multiple(const MatrixFloat &a,const MatrixFloat &b,const bool aTranspose = false);
    
    /**
     Adds the input matrix data (b) to this matrix (a), giving: a = a + b.
     This rows and cols of b must match that of this matrix.
     
     @param a: the matrix to multiple with b
     @param b: the matrix to multiple with a
     @param aTranspose: a flag to indicate if matrix a should be transposed
     @return true if the operation was completed successfully, false otherwise
     */
    bool add(const MatrixFloat &b);
    
    /**
     Adds the input matrix data (a) to the input matrix (b), storing the data in this matrix (c) giving: c = a + b.
     The rows and cols in a and b must match.
     This will resize the current matrix if needed.
     
     @param a: the matrix to add with b
     @param b: the matrix to add with a
     @return true if the operation was completed successfully, false otherwise
     */
    bool add(const MatrixFloat &a,const MatrixFloat &b);
    
    /**
     Subtracts the input matrix data (b) from this matrix (a), giving: a = a - b.
     This rows and cols of b must match that of this matrix.
     
     @param a: the matrix to subtract from this instance
     @return true if the operation was completed successfully, false otherwise
     */
    bool subtract(const MatrixFloat &b);
    
    /**
     Subtracts the input matrix data (b) from this matrix (a), giving (c): c = a - b.
     This rows and cols of b must match that of this matrix.
     This will resize the current matrix if needed.
     
     @param a: the matrix to subtract with b
     @param b: the matrix to subtract from a
     @return true if the operation was completed successfully, false otherwise
     */
    bool subtract(const MatrixFloat &a,const MatrixFloat &b);
    
    /**
     Gets the ranges min value throughout the entire matrix.
     
     @return a float_t value containing the minimum matrix value
     */
    float_t getMinValue() const;
    
    /**
     Gets the ranges max value throughout the entire matrix.
     
     @return a float_t value containing the maximum matrix value
     */
    float_t getMaxValue() const;
    
    /**
     Gets the mean of each column in the matrix and returns this as a VectorFloat.
     
     @return a VectorFloat with the mean of each column
     */
    VectorFloat getMean() const;
    
    /**
     Gets the standard deviation of each column in the matrix and returns this as a VectorFloat.
     
     @return a VectorFloat with the standard deviation of each column
     */
    VectorFloat getStdDev() const;
    
    /**
     Gets the covariance matrix of this matrix and returns this as a MatrixFloat.
     
     @return a MatrixFloat with the covariance matrix of this matrix
     */
    MatrixFloat getCovarianceMatrix() const;
    
    /**
     Gets the ranges (min and max values) of each column in the matrix.
     
     @return a vector with the ranges (min and max values) of each column in the matrix
     */
    std::vector< MinMax > getRanges() const;
    
    /**
     Gets the trace of this matrix.
     
     @return the trace of this matrix as a float_t
     */
    float_t getTrace() const;
    
protected:
    
    float_t stringToFloat(const std::string &value){
        std::stringstream s( value );
        float_t d;
        s >> d;
        return d;
    }
  
    WarningLog warningLog;
    ErrorLog errorLog;
    
};
    
} //End of namespace GRT

#endif //GRT_MATRIX_float_t_HEADER
