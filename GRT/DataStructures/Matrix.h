/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>

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

#include "../Util/GRTTypedefs.h"
#include "Vector.h"
#include <exception>
#include "../Util/GRTException.h"
#include "../Util/ErrorLog.h"

GRT_BEGIN_NAMESPACE
    
template <class T> class Matrix{
public:
    /**
     Default Constructor
    */
	Matrix():errorLog("[ERROR Matrix]"){
        rows = 0;
        cols = 0;
        size = 0;
        capacity = 0;
        dataPtr = NULL;
        rowPtr = NULL;
	}
    
    /**
    Constructor, sets the size of the matrix to [rows cols]
     
     @param rows: sets the number of rows in the matrix, must be a value greater than zero
     @param cols: sets the number of columns in the matrix, must be a value greater than zero
    */
	Matrix(const unsigned int rows,const unsigned int cols):errorLog("[ERROR Matrix]"){
        dataPtr = NULL;
        rowPtr = NULL;
        resize(rows,cols);
	}

    /**
    Constructor, sets the size of the matrix to [rows cols] and initalizes all the data to data
     
     @param rows: sets the number of rows in the matrix, must be a value greater than zero
     @param cols: sets the number of columns in the matrix, must be a value greater than zero
     @param data: default value that will be used to initalize all the values in the matrix
    */
    Matrix(const unsigned int rows,const unsigned int cols, const T &data ):errorLog("[ERROR Matrix]"){
        dataPtr = NULL;
        rowPtr = NULL;
        resize(rows,cols,data);
    }
    
    /**
     Copy Constructor, copies the values from the rhs Matrix to this Matrix instance
     
     @param rhs: the Matrix from which the values will be copied
    */
	Matrix(const Matrix &rhs):errorLog("[ERROR Matrix]"){
        this->dataPtr = NULL;
        this->rowPtr = NULL;
        this->rows = 0;
        this->cols = 0;
        this->size = 0;
        this->capacity = 0;
        this->copy( rhs );
	}
    
    /**
     Copy Constructor, copies the values from the input vector to this Matrix instance.
     The input vector must be a vector< vector< T > > in a [rows cols] format.  The number of
     columns in each row must be consistent.  Both the rows and columns must be greater than 0.
     
     @param data: the input data which will be copied to this Matrix instance
     */
	Matrix( const Vector< Vector< T > > &data ):errorLog("[ERROR Matrix]"){
		this->dataPtr = NULL;
        this->rowPtr = NULL;
		this->rows = 0;
		this->cols = 0;
        this->size = 0;
		this->capacity = 0;
        
		unsigned int tempRows = data.getSize();
		unsigned int tempCols = 0;
        
		//If there is no data then return
		if( tempRows == 0 ) return;
        
		//Check to make sure all the columns are the same size
		for(unsigned int i=0; i<tempRows; i++){
			if( i == 0 ) tempCols = data[i].getSize();
			else{
				if( data[i].getSize() != tempCols ){
                    return;
				}
			}
		}
        
		if( tempCols == 0 ) return;
        
		//Resize the matrix and copy the data
		if( resize(tempRows,tempCols) ){
			for(unsigned int i=0; i<tempRows; i++){
				for(unsigned int j=0; j<tempCols; j++){
                	dataPtr[(i*cols)+j] = data[i][j];
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
     
     @param rhs: another instance of a Matrix
     @return returns a reference to this instance of the Matrix
    */
	Matrix& operator=(const Matrix &rhs){
		if(this!=&rhs){
            this->clear();
            this->copy( rhs );
		}
		return *this;
	}
    
    /**
     Returns a pointer to the data at row r
     
     @param r: the index of the row you want, should be in the range [0 rows-1]
     @return a pointer to the data at row r
    */
	inline T* operator[](const unsigned int r){
		return rowPtr[r];
	}
    
    /**
     Returns a const pointer to the data at row r
     
     @param r: the index of the row you want, should be in the range [0 rows-1]
     @return a const pointer to the data at row r
     */
	inline const T* operator[](const unsigned int r) const{
		return rowPtr[r];
	}

    /**
     Gets a row vector [1 cols] from the Matrix at the row index r
     
     @param r: the index of the row, this should be in the range [0 rows-1]
     @return returns a row vector from the Matrix at the row index r
    */
	Vector< T > getRowVector(const unsigned int r) const{
		Vector< T > rowVector(cols);
		for(unsigned int c=0; c<cols; c++)
			rowVector[c] = dataPtr[r*cols+c];
		return rowVector;
	}

    /**
     Gets a column vector [rows 1] from the Matrix at the column index c
     
     @param c: the index of the column, this should be in the range [0 cols-1]
     @return returns a column vector from the Matrix at the column index c
    */
	Vector<T> getColVector(const unsigned int c) const{
		Vector<T> columnVector(rows);
		for(unsigned int r=0; r<rows; r++)
			columnVector[r] = dataPtr[r*cols+c];
		return columnVector;
	}
    
    /**
     Concatenates the entire matrix into a single vector and returns the vector.
     The data can either be concatenated by row or by column, by setting the respective concatByRow parameter to true of false.
     If concatByRow is true then the data in the matrix will be added to the vector row-vector by row-vector, otherwise
     the data will be added column-vector by column-vector.
     
     @param concatByRow: sets if the matrix data will be added to the vector row-vector by row-vector
     @return returns a vector containing the entire matrix data
     */
    Vector<T> getConcatenatedVector(const bool concatByRow = true) const{
        
		if( rows == 0 || cols == 0 ) return Vector<T>();
        
		Vector<T> vectorData(rows*cols);
        
        unsigned int i,j =0;
        
		if( concatByRow ){
			for( i=0; i<rows; i++){
				for(j=0; j<cols; j++){
					vectorData[ (i*cols)+j ] = dataPtr[i*cols+j];
				}
			}
		}else{
			for(j=0; j<cols; j++){
				for(i=0; i<rows; i++){
					vectorData[ (i*cols)+j ] = dataPtr[i*cols+j];
				}
			}
		}
        
		return vectorData;
    }

    /**
     Resizes the Matrix to the new size of [r c].  If [r c] matches the previous size then the matrix will not be resized but the function will return true.
     
     @param r: the number of rows, must be greater than zero
     @param c: the number of columns, must be greater than zero
     @return returns true or false, indicating if the resize was successful 
    */
	virtual bool resize(const unsigned int r,const unsigned int c){
        
		if( r + c == 0 ){
			errorLog << "resize(...) - Failed to resize matrix, rows and cols == zero!" << std::endl;
			return false;
		}

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
                size = r * c;
                capacity = r;
                
                dataPtr = new T[size];
                rowPtr = new T*[rows];
                
                if( dataPtr == NULL ){
                    rows = 0;
                    cols = 0;
                    size = 0;
                    capacity = 0;
					errorLog << "resize(const unsigned r,const unsigned int c) - Failed to allocate memory! r: " << r << " c: " << c << std::endl;
                    throw GRT::Exception("Matrix::resize(const unsigned int r,const unsigned int c) - Failed to allocate memory!");
                    return false;
                }
                
                if( rowPtr == NULL ){
                    rows = 0;
                    cols = 0;
                    size = 0;
                    capacity = 0;
					errorLog << "resize(const unsigned r,const unsigned int c) - Failed to allocate memory! r: " << r << " c: " << c << std::endl;
                    throw Exception("Matrix::resize(const unsigned int r,const unsigned int c) - Failed to allocate memory!");
                    return false;
                }
                
                //Setup the row pointers
                unsigned int i=0;
                T *p = &(dataPtr[0]);
                for(i=0; i<rows; i++){
                    rowPtr[i] = p;
                    p += cols;
                }
                
                return true;
                
            }catch( std::exception& e ){
                errorLog << "resize: Failed to allocate memory. Error: " << e.what() << " rows: " << r << " cols: " << c <<  std::endl;
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
     Resizes the Matrix to the new size of [r c].  If [r c] matches the previous size then the matrix will not be resized but the function will return true.
     
     @param r: the number of rows, must be greater than zero
     @param c: the number of columns, must be greater than zero
     @param value: the default value that will be set across all values in the buffer
     @return returns true or false, indicating if the resize was successful 
    */
    virtual bool resize(const unsigned int r,const unsigned int c,const T &value){
        
        if( !resize( r, c) ){
            return false;
        }

        return setAll( value );
    }
              
    /**
     Copies the data from the rhs matrix to this matrix.
     
     @param rhs: the matrix you want to copy into this matrix
     @return returns true or false, indicating if the copy was successful
     */
    virtual bool copy( const Matrix<T> &rhs ){
        
        if( this != &rhs ){
            
            if( this->size != rhs.size ){
                if( !this->resize( rhs.rows, rhs.cols ) ){
                    throw Exception("Matrix::copy( const Matrix<T> &rhs ) - Failed to allocate resize matrix!");
                    return false;
                }
            }
            
            //Copy the data
            unsigned int i = 0;
            for(i=0; i<size; i++){
                this->dataPtr[i] = rhs.dataPtr[i];
            }
        }
        
        return true;
    }

    /**
     @deprecated
     This function is now depreciated! You should use setAll(const T &value) instead.

     Sets all the values in the Matrix to the input value
     
     @param value: the value you want to set all the Matrix values to
     @return returns true or false, indicating if the set was successful 
    */
    bool setAllValues(const T &value){
        return setAll( value );
    }

    /**
     Sets all the values in the Matrix to the input value
     
     @param value: the value you want to set all the Matrix values to
     @return returns true or false, indicating if the set was successful 
    */
    bool setAll(const T &value){
        if(dataPtr!=NULL){
            unsigned int i =0;
            for(i=0; i<size; i++)
                dataPtr[i] = value;
            return true;
        }
        return false;
    }
	
	/**
     Sets all the values in the row at rowIndex with the values in the vector called row.
     The size of the row vector must match the number of columns in this Matrix.
     
     @param row: the vector of row values you want to add
     @param rowIndex: the row index of the row you want to update, must be in the range [0 rows]
     @return returns true or false, indicating if the set was successful 
    */
	bool setRowVector(const Vector<T> &row,const unsigned int rowIndex){
		if( dataPtr == NULL ) return false;
		if( row.size() != cols ) return false;
		if( rowIndex >= rows ) return false;

        unsigned int j = 0;
		for(j=0; j<cols; j++)
			dataPtr[ rowIndex * cols + j ] = row[ j ];
        return true;
	}
	
	/**
     Sets all the values in the column at colIndex with the values in the vector called column.
     The size of the column vector must match the number of rows in this Matrix.
     
     @param column: the vector of column values you want to add
     @param colIndex: the column index of the column you want to update, must be in the range [0 cols]
     @return returns true or false, indicating if the set was successful 
    */
	bool setColVector(const Vector<T> &column,const unsigned int colIndex){
		if( dataPtr == NULL ) return false;
		if( column.size() != rows ) return false;
		if( colIndex >= cols ) return false;

		for(unsigned int i=0; i<rows; i++)
			dataPtr[ i * cols + colIndex ] = column[ i ];
        return true;
	}

    /**
     Adds the input sample to the end of the Matrix, extending the number of rows by 1.  The number of columns in the sample must match
     the number of columns in the Matrix, unless the Matrix size has not been set, in which case the new sample size will define the
     number of columns in the Matrix.
     
     @param sample: the new column vector you want to add to the end of the Matrix.  Its size should match the number of columns in the Matrix
     @return returns true or false, indicating if the push was successful 
    */
	bool push_back(const Vector<T> &sample){
        
        unsigned int i,j = 0;
        
		//If there is no data, but we know how many cols are in a sample then we simply create a new buffer of size 1 and add the sample
		if(dataPtr==NULL){
			cols = (unsigned int)sample.size();
			if( !resize(1,cols) ){
                clear();
                return false;
            }
			for(j=0; j<cols; j++)
				dataPtr[ j ] = sample[j];
			return true;
		}

		//If there is data and the sample size does not match the number of columns then return false
		if(sample.size() != cols ){
			return false;
		}

		//Check to see if we have reached the capacity, if not then simply add the new data as there are unused rows
		if( rows < capacity ){
			//Add the new sample at the end
			for(j=0; j<cols; j++)
				dataPtr[rows * cols + j] = sample[j];
				
		}else{ //Otherwise we copy the existing data from the data ptr into a new buffer of size (rows+1) and add the sample at the end
            
            const unsigned int tmpRows = rows + 1;
            T* tmpDataPtr = new T[tmpRows*cols];
            T** tmpRowPtr = new T*[tmpRows];
            
			if( tmpDataPtr == NULL || tmpRowPtr == NULL ){//If NULL then we have run out of memory
				return false;
			}
			
            //Setup the row pointers
            T *p = &(tmpDataPtr[0]);
            for(i=0; i<tmpRows; i++){
                tmpRowPtr[i] = p;
                p += cols;
            }
            
			//Copy the original data into the tmp buffer
			for(i=0; i<rows*cols; i++)
					tmpDataPtr[i] = dataPtr[i];

			//Add the new sample at the end of the tmp buffer
			for(j=0; j<cols; j++)
				tmpDataPtr[rows*cols+j] = sample[j];

			//Delete the original data and copy the pointer
			delete[] dataPtr;
            delete[] rowPtr;
			dataPtr = tmpDataPtr;
            rowPtr = tmpRowPtr;
			
			//Increment the capacity so it matches the number of rows
			capacity++;
		}
		
        //Increment the number of rows
		rows++;
        
        //Update the size
        size = rows * cols;

		//Finally return true to signal that the data was added correctly
		return true;
	}
	
	/**
     This function reserves a consistent block of data so new rows can more effecitenly be pushed_back into the Matrix.
     The capacity variable represents the number of rows you want to reserve, based on the current number of columns.
     
     @param capacity: the new capacity value
     @return returns true if the data was reserved, false otherwise
    */
	bool reserve( const unsigned int capacity ){
		
		//If the number of columns has not been set, then we can not do anything
		if( cols == 0 ) return false;
		
		//Reserve the data and copy and existing data
        unsigned int i=0;
		T* tmpDataPtr = new T[ capacity * cols ];
        T** tmpRowPtr = new T*[ capacity ];
		if( tmpDataPtr == NULL || tmpRowPtr == NULL ){//If NULL then we have run out of memory
			return false;
		}
		
        //Setup the row pointers
        T *p = &(tmpDataPtr[0]);
        for(i=0; i<capacity; i++){
            tmpRowPtr[i] = p;
            p += cols;
        }

		//Copy the existing data into the new memory
		for(i=0; i<size; i++)
				tmpDataPtr[i] = dataPtr[i];

		//Delete the original data and copy the pointer
		delete[] dataPtr;
        delete[] rowPtr;
		dataPtr = tmpDataPtr;
        rowPtr = tmpRowPtr;
		
		//Store the new capacity
		this->capacity = capacity;
        
        //Store the size
        size = rows * cols;
		
		return true;
	}

    /**
     Cleans up any dynamic memory and sets the number of rows and columns in the matrix to zero
     @return returns true if the data was cleared successfully
    */
	bool clear(){
		if( dataPtr != NULL ){
			delete[] dataPtr;
			dataPtr = NULL;
		}
        if( rowPtr != NULL ){
			delete[] rowPtr;
			rowPtr = NULL;
		}
		rows = 0;
		cols = 0;
        size = 0;
		capacity = 0;
        return true;
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
                
    /**
     Gets the size of the Matrix. This is rows * size.
     
     @return returns the number of columns in the Matrix
     */
    inline unsigned int getSize() const{ return size; }
    
    /**
     Gets a pointer to the row buffer
     
     @return returns the row pointer
     */
    T** getDataPointer() const{
        if( rowPtr == NULL ){
            throw Exception("Matrix::getDataPointer() - Matrix has not been initialized!");
        }
        return &(rowPtr[0]);
    }

    /**
     Gets a pointer to the main data pointer
     
     @return returns a pointer to the raw data
     */
    T* getData() const {
        return dataPtr;
    }

protected:
	unsigned int rows;      ///< The number of rows in the Matrix
	unsigned int cols;      ///< The number of columns in the Matrix
    unsigned int size;      ///< Stores rows * cols
	unsigned int capacity;  ///< The capacity of the Matrix, this will be the number of rows, not the actual memory size
    T *dataPtr;             ///< A pointer to the raw data
    T **rowPtr;             ///< A pointer to each row in the data
    ErrorLog errorLog;
};

GRT_END_NAMESPACE

#endif //Header guard
