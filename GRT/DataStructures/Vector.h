/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief The Vector class is a basic class for storing any type of data.  The default Vector is an interface for std::vector, but the idea is
 this can easily be changed when needed (e.g., when running the GRT on an embedded device with limited STL support). This class is a template and can therefore be used with any generic data type.
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

#ifndef GRT_VECTOR_HEADER
#define GRT_VECTOR_HEADER

#include <iostream>
#include <iterator>     // std::front_inserter
#include <algorithm>    // std::copy
#include <vector>
#include "../Util/GRTTypedefs.h"

GRT_BEGIN_NAMESPACE

template <class T> class Vector : public std::vector< T >{
public:
    /**
     Default Constructor
    */
	//Vector(){}
    
    /**
    Constructor, sets the size of the vector
     
     @param size: the size of the vector
    */
	Vector( const unsigned int size = 0 ):std::vector< T >(size){}

    /**
    Constructor, sets the size of the vector and sets all elements to value
     
     @param size: the size of the vector
    */
    Vector( const unsigned int size, const T &value ):std::vector< T >(size, value){}
    
    /**
     Copy Constructor, copies the values from the rhs Vector to this Vector instance
     
     @param rhs: the Vector from which the values will be copied
    */
	Vector( const Vector &rhs ){
        unsigned int N = rhs.getSize();
        if( N > 0 ){
            this->resize( N );
            std::copy( rhs.begin(), rhs.end(), this->begin() );
        }else this->clear();
	}
    
    /**
     Copy Constructor, copies the values from the rhs std::vector to this Vector instance
     
     @param rhs: the base class instance from which the values will be copied
    */
    Vector( const std::vector< T > &rhs ){
        unsigned int N = rhs.size();
        if( N > 0 ){
            this->resize( N );
            std::copy( rhs.begin(), rhs.end(), this->begin() );
        }else this->clear();
    }
    
    /**
     Destructor, cleans up any memory
    */
	virtual ~Vector(){ }
    
    /**
     Defines how the data from the rhs Vector should be copied to this Vector
     
     @param rhs: another instance of a Vector
     @return returns a reference to this instance of the Vector
    */
	Vector& operator=(const Vector &rhs){
		if(this!=&rhs){
            unsigned int N = rhs.getSize();
            if( N > 0 ){
                this->resize( N );
                std::copy( rhs.begin(), rhs.end(), this->begin() );
            }else this->clear();
		}
		return *this;
	}

    /**
     Defines how the data from the rhs std::vector instance should be copied to this Vector
     
     @param rhs: another instance of a Vector
     @return returns a reference to this instance of the Vector
    */
    Vector& operator=(const std::vector< T > &rhs){
        if(this!=&rhs){
            unsigned int N = rhs.size();
            if( N > 0 ){
                this->resize( N );
                std::copy( rhs.begin(), rhs.end(), this->begin() );
            }else this->clear();
        }
        return *this;
    }

    /**
     Defines how the vector should be resized
     
     @param size: the new size of the vector
     @return returns true if the vector was resized correctly, false otherwise
    */
    virtual bool resize( const unsigned int size ){
        std::vector< T >::resize( size );
        return getSize() == size;
    }

    /**
     Defines how the vector should be resized, this will also set all the values in the vector to [value]
     
     @param size: the new size of the vector
     @param value: the value that will be copied to all elements in the vector
     @return returns true if the vector was resized correctly, false otherwise
    */
    virtual bool resize( const unsigned int size, const T& value ){
        std::vector< T >::resize( size, value );
        return getSize() == size;
    }
              
    /**
     Copies the data from the rhs vector to this vector.
     
     @param rhs: the vector you want to copy into this vector
     @return returns true or false, indicating if the copy was successful
     */
    virtual bool copy( const Vector<T> &rhs ){
        
        if( this != &rhs ){
            unsigned int N = rhs.getSize();
            if( N > 0 ){
                this->resize( N );
                std::copy( rhs.begin(), rhs.end(), this->begin() );
            }
        }
        
        return true;
    }

    /**
     Sets all the values in the Vector to the input value
     
     @param value: the value you want to set all the Vector values to
     @return returns true or false, indicating if the set was successful 
    */
    bool fill(const T &value){

        const unsigned int N = this->size();
        
        if( N == 0 ) return false;

        std::fill(this->begin(),this->end(),value);

        return true;
    }

    /**
     Sets all the values in the Vector to the input value
     
     @param value: the value you want to set all the Vector values to
     @return returns true or false, indicating if the set was successful 
    */
    bool setAll(const T &value){
      return fill(value);
    }

    /**
     Gets the size of the Vector
     
     @return returns the size of the Vector
    */
	inline UINT getSize() const{ return static_cast<UINT>(this->size()); }

    /**
     Gets a pointer to the first element in the vector
     
     @return returns a pointer to the raw data
     */
    T* getData() {
        if( this->size() == 0 ) return NULL;
        return &(*this)[0];
    }

    /**
     Gets a pointer to the first element in the vector
     
     @return returns a pointer to the raw data
     */
    const T* getData() const {
        if( this->size() == 0 ) return NULL;
        return &(*this)[0];
    }

protected:
};

GRT_END_NAMESPACE

#endif //GRT_VECTOR_HEADER
