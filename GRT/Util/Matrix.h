/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief The Matrix class is a basic class for storing any type of data.  This class is a template and can therefore be used with any generic data type.
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

#ifndef GRT_MATRIX_HEADER
#define GRT_MATRIX_HEADER

#include <iostream>
#include <vector>
#include <exception>
#include "ErrorLog.h"

namespace GRT{
    
template <class T> class Matrix{
public:
    /**
     Default Constructor
    */
	Matrix():errorLog("[ERROR Matrix]"){
        rows = 0;
        cols = 0;
        capacity = 0;
        dataPtr = NULL;
	}
    
    /**
    Constructor, sets the size of the matrix to [rows cols]
     
     @param const UINT rows: sets the number of rows in the matrix, must be a value greater than zero
     @param const UINT cols: sets the number of columns in the matrix, must be a value greater than zero
    */
	Matrix(const unsigned int rows,const unsigned int cols):errorLog("[ERROR Matrix]"){
        dataPtr = NULL;
        resize(rows,cols);
	}
    
    /**
     Copy Constructor, copies the values from the rhs Matrix to this Matrix instance
     
     @param const Matrix &rhs: the Matrix from which the values will be copied
    */
	Matrix(const Matrix &rhs):errorLog("[ERROR Matrix]"){
        this->dataPtr = NULL;
        this->rows = 0;
        this->cols = 0;
        this->capacity = 0;
        
		if(this!=&rhs){
			 this->rows = rhs.rows;
			 this->cols = rhs.cols;
			 this->capacity = rhs.rows;
			 dataPtr = new T*[rows];
			 for(unsigned int i=0; i<rows; i++){
				 dataPtr[i] = new T[cols];
				 for(unsigned int j=0; j<cols; j++)
					 this->dataPtr[i][j] = rhs.dataPtr[i][j];
			 }
		}
	}
    
    /**
     Copy Constructor, copies the values from the input vector to this Matrix instance.
     The input vector must be a vector< vector< T > > in a [rows cols] format.  The number of
     columns in each row must be consistent.  Both the rows and columns must be greater than 0.
     
     @param const vector< vector< T > > &data: the input data which will be copied to this Matrix instance
     */
	Matrix(const std::vector< std::vector< T > > &data):errorLog("[ERROR Matrix]"){
		this->dataPtr = NULL;
		this->rows = 0;
		this->cols = 0;
		this->capacity = 0;
        
		unsigned int tempRows = (unsigned int)data.size();
		unsigned int tempCols = 0;
        
		//If there is no data then return
		if( tempRows == 0 ) return;
        
		//Check to make sure all the columns are the same size
		for(unsigned int i=0; i<tempRows; i++){
			if( i == 0 ) tempCols = data[i].size();
			else{
				if( data[i].size() != tempCols ){
                    return;
				}
			}
		}
        
		if( tempCols == 0 ) return;
        
		//Resize the matrix and copy the data
		if( resize(tempRows,tempCols) ){
			for(unsigned int i=0; i<tempRows; i++){
				for(unsigned int j=0; j<tempCols; j++){
                	dataPtr[i][j] = data[i][j];
				}
			}
		}
        
	}
    
    /**
     Destructor, cleans up any memory
    */
	virtual ~Matrix(){ 
		clear(); 
	}
    
    /**
     Defines how the data from the rhs Matrix should be copied to this Matrix
     
     @param const Matrix &rhs: another instance of a Matrix
     @return returns a reference to this instance of the Matrix
    */
	Matrix& operator=(const Matrix &rhs){
		if(this!=&rhs){
			 this->clear();
			 this->rows = rhs.rows;
			 this->cols = rhs.cols;
			 this->capacity = rhs.rows;
			 dataPtr = new T*[rows];
			 for(unsigned int i=0; i<rows; i++){
				 dataPtr[i] = new T[cols];
				 for(unsigned int j=0; j<cols; j++)
					 this->dataPtr[i][j] = rhs.dataPtr[i][j];
			 }
		}
		return *this;
	}
    
    /**
     Returns a pointer to the data at row r
     
     @param const UINT r: the index of the row you want, should be in the range [0 rows-1]
     @return a pointer to the data at row r
    */
	inline T* operator[](const unsigned int r){
		return dataPtr[r];
	}
    
    /**
     Returns a const pointer to the data at row r
     
     @param const UINT r: the index of the row you want, should be in the range [0 rows-1]
     @return a const pointer to the data at row r
     */
	inline const T* operator[](const unsigned int r) const{
		return dataPtr[r];
	}

    /**
     Gets a row vector [1 cols] from the Matrix at the row index r
     
     @param const UINT r: the index of the row, this should be in the range [0 rows-1]
     @return returns a row vector from the Matrix at the row index r
    */
	std::vector<T> getRowVector(const unsigned int r) const{
		std::vector<T> rowVector(cols);
		for(unsigned int c=0; c<cols; c++)
			rowVector[c] = dataPtr[r][c];
		return rowVector;
	}

    /**
     Gets a column vector [rows 1] from the Matrix at the column index c
     
     @param const UINT c: the index of the column, this should be in the range [0 cols-1]
     @return returns a column vector from the Matrix at the column index c
    */
	std::vector<T> getColVector(const unsigned int c) const{
		std::vector<T> columnVector(rows);
		for(unsigned int r=0; r<rows; r++)
			columnVector[r] = dataPtr[r][c];
		return columnVector;
	}
    
    /**
     Concatenates the entire matrix into a single vector and returns the vector.
     The data can either be concatenated by row or by column, by setting the respective concatByRow parameter to true of false.
     If concatByRow is true then the data in the matrix will be added to the vector row-vector by row-vector, otherwise
     the data will be added column-vector by column-vector.
     
     @param const bool concatByRow: sets if the matrix data will be added to the vector row-vector by row-vector
     @return returns a vector containing the entire matrix data
     */
    std::vector<T> getConcatenatedVector(const bool concatByRow = true) const{
        
		if( rows == 0 || cols == 0 ) return std::vector<T>();
        
		std::vector<T> vectorData(rows*cols);
        
		if( concatByRow ){
			for(unsigned int i=0; i<rows; i++){
				for(unsigned int j=0; j<cols; j++){
					vectorData[ (i*cols)+j ] = dataPtr[i][j];
				}
			}
		}else{
			for(unsigned int j=0; j<cols; j++){
				for(unsigned int i=0; i<rows; i++){
					vectorData[ (i*cols)+j ] = dataPtr[i][j];
				}
			}
		}
        
		return vectorData;
    }

    /**
     Resizes the Matrix to the new size of [r c].  If [r c] matches the previous size then the matrix will not be resized but the function will return true.
     
     @param const UINT r: the number of rows, must be greater than zero
     @param const UINT c: the number of columns, must be greater than zero
     @return returns true or false, indicating if the resize was successful 
    */
	virtual bool resize(const unsigned int r,const unsigned int c){
        
        //If the rows and cols are unchanged then do not resize the data
        if( r == rows && c == cols ){
            return true;
        }
        
		//Clear any previous memory
		clear();
        
		if( r > 0 && c > 0 ){
            try{
                rows = r;
                cols = c;
                capacity = r;
                dataPtr = new T*[rows];
                
                //Check to see if the memory was created correctly
                if( dataPtr == NULL ){
                    rows = 0;
                    cols = 0;
                    capacity = 0;
                    return false;
                }
                for(unsigned int i=0; i<rows; i++){
                    dataPtr[i] = new T[cols];
                }
                return true;
                
            }catch( std::exception& e ){
                errorLog << "resize: Failed to allocate memory. Error: " << e.what() << std::endl;
                clear();
                return false;
            }catch( ... ){
                errorLog << "resize: Failed to allocate memory." << std::endl;
                clear();
                return false;
            }
		}
		return false;
	}

    /**
     Sets all the values in the Matrix to the input value
     
     @param const T &value: the value you want to set all the Matrix values to
     @return returns true or false, indicating if the set was successful 
    */
	bool setAllValues(const T &value){
		if(dataPtr!=NULL){
			for(unsigned int i=0; i<rows; i++)
				for(unsigned int j=0; j<cols; j++)
					dataPtr[i][j] = value;
            return true;
		}
        return false;
	}
	
	/**
     Sets all the values in the row at rowIndex with the values in the vector called row.
     The size of the row vector must match the number of columns in this Matrix.
     
     @param const std::vector<T> &row: the vector of row values you want to add
     @param const unsigned int rowIndex: the row index of the row you want to update, must be in the range [0 rows]
     @return returns true or false, indicating if the set was successful 
    */
	bool setRowVector(const std::vector<T> &row,const unsigned int rowIndex){
		if( dataPtr == NULL ) return false;
		if( row.size() != cols ) return false;
		if( rowIndex >= rows ) return false;

		for(unsigned int j=0; j<cols; j++)
			dataPtr[ rowIndex ][ j ] = row[ j ];
        return true;
	}
	
	/**
     Sets all the values in the column at colIndex with the values in the vector called column.
     The size of the column vector must match the number of rows in this Matrix.
     
     @param const std::vector<T> &column: the vector of column values you want to add
     @param const unsigned int colIndex: the column index of the column you want to update, must be in the range [0 cols]
     @return returns true or false, indicating if the set was successful 
    */
	bool setColVector(const std::vector<T> &column,const unsigned int colIndex){
		if( dataPtr == NULL ) return false;
		if( column.size() != rows ) return false;
		if( colIndex >= cols ) return false;

		for(unsigned int i=0; i<rows; i++)
			dataPtr[ i ][ colIndex ] = column[ i ];
        return true;
	}

    /**
     Adds the input sample to the end of the Matrix, extending the number of rows by 1.  The number of columns in the sample must match
     the number of columns in the Matrix, unless the Matrix size has not been set, in which case the new sample size will define the
     number of columns in the Matrix.
     
     @param const std::vector<T> &sample: the new column vector you want to add to the end of the Matrix.  Its size should match the number of columns in the Matrix
     @return returns true or false, indicating if the push was successful 
    */
	bool push_back(const std::vector<T> &sample){
		//If there is no data, but we know how many cols are in a sample then we simply create a new buffer of size 1 and add the sample
		if(dataPtr==NULL){
			cols = (unsigned int)sample.size();
			if( !resize(1,cols) ){
                clear();
                return false;
            }
			for(unsigned int j=0; j<cols; j++)
				dataPtr[0][j] = sample[j];
			return true;
		}

		//If there is data and the sample size does not match the number of columns then return false
		if(sample.size() != cols ){
			return false;
		}

		//Check to see if we have reached the capacity, if not then simply add the new data
		if( rows < capacity ){
			//Add the new sample at the end
			for(unsigned int j=0; j<cols; j++)
				dataPtr[rows][j] = sample[j];
				
		}else{ //Otherwise we copy the existing data from the data ptr into a new buffer of size (rows+1) and add the sample at the end
			T** tempDataPtr = NULL;
			tempDataPtr = new T*[ rows+1 ];
			if( tempDataPtr == NULL ){//If NULL then we have run out of memory
				return false;
			}
			for(unsigned int i=0; i<rows+1; i++){
				tempDataPtr[i] = new T[cols];
			}

			//Copy the original data
			for(unsigned int i=0; i<rows; i++)
				for(unsigned int j=0; j<cols; j++)
					tempDataPtr[i][j] = dataPtr[i][j];

			//Add the new sample at the end
			for(unsigned int j=0; j<cols; j++)
				tempDataPtr[rows][j] = sample[j];

			//Delete the original data and copy the pointer
			for(unsigned int i=0; i<rows; i++){
				delete[] dataPtr[i];
				dataPtr[i] = NULL;
			}
			delete[] dataPtr;
			dataPtr = tempDataPtr;
			
			//Increment the capacity so it matches the number of rows
			capacity++;
		}
		
        //Increment the number of rows
		rows++;

		//Finally return true to signal that the data was added correctly
		return true;
	}
	
	/**
     This function reserves a consistent block of data so new rows can more effecitenly be pushed_back into the Matrix.
     The capacity variable represents the number of rows you want to reserve, based on the current number of columns.
     
     @param const unsigned int capacity: the new capacity value
     @return returns true if the data was reserved, false otherwise
    */
	bool reserve(const unsigned int capacity){
		
		//If the number of columns has not been set, then we can not do anything
		if( cols == 0 ) return false;
		
		//Reserve the data and copy and existing data
		T** tempDataPtr = NULL;
		tempDataPtr = new T*[ capacity ];
		if( tempDataPtr == NULL ){//If NULL then we have run out of memory
			return false;
		}
		for(unsigned int i=0; i<capacity; i++){
			tempDataPtr[i] = new T[cols];
		}

		//Copy the existing data into the new memory
		for(unsigned int i=0; i<rows; i++)
			for(unsigned int j=0; j<cols; j++)
				tempDataPtr[i][j] = dataPtr[i][j];

		//Delete the original data and copy the pointer
		for(unsigned int i=0; i<rows; i++){
			delete[] dataPtr[i];
			dataPtr[i] = NULL;
		}
		delete[] dataPtr;
		dataPtr = tempDataPtr;
		
		//Store the new capacity
		this->capacity = capacity;
		
		return true;
	}

    /**
     Cleans up any dynamic memory and sets the number of rows and columns in the matrix to zero
    */
	void clear(){
		if( dataPtr != NULL ){
			for(unsigned int i=0; i<rows; i++){
                delete[] dataPtr[i];
                dataPtr[i] = NULL;
			}
			delete[] dataPtr;
			dataPtr = NULL;
		}
		rows = 0;
		cols = 0;
		capacity = 0;
	}

    /**
     Gets the number of rows in the Matrix
     
     @return returns the number of rows in the Matrix
    */
	inline unsigned int getNumRows() const{ return rows; }
    
    /**
     Gets the number of columns in the Matrix
     
     @return returns the number of columns in the Matrix
    */
	inline unsigned int getNumCols() const{ return cols; }
	
	/**
     Gets the capacity of the Matrix. This is the number of rows that have been reserved for the Matrix.
     You can control the capacity using the reserve function
     
     @return returns the number of columns in the Matrix
    */
	inline unsigned int getCapacity() const{ return capacity; }
    
    T** getDataPointer() const{
        return dataPtr;
    }

protected:
    
	unsigned int rows;      ///< The number of rows in the Matrix
	unsigned int cols;      ///< The number of columns in the Matrix
	unsigned int capacity;  ///< The actual capacity of the Matrix, this will be the number of rows, not the actual memory size
	T **dataPtr;            ///< A pointer to the data
    ErrorLog errorLog;

};

}//End of namespace GRT

#endif //GRT_MATRIX_HEADER
