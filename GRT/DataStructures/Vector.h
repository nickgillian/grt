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
	Vector(){}
    
    /**
    Constructor, sets the size of the vector
     
     @param size: the size of the vector
    */
	Vector( const unsigned int size ):data(size){}

    /**
    Constructor, sets the size of the vector and sets all elements to value
     
     @param size: the size of the vector
    */
    Vector( const unsigned int size, const T &value ):data(size, value){}
    
    /**
     Copy Constructor, copies the values from the rhs Vector to this Vector instance
     
     @param rhs: the Vector from which the values will be copied
    */
	Vector( const Vector &rhs ){
        this->data = rhs.data;
	}
    
    /**
     Copy Constructor, copies the values from the rhs std::vector to this Vector instance
     
     @param rhs: the base class instance from which the values will be copied
    */
    Vector( const std::vector< T > &rhs ){
        this->data = rhs;
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
            this->data = rhs.data;
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
            this->data = rhs;
        }
        return *this;
    }
    
    /**
     Returns a pointer to the data at element i
     
     @param i: the index of the element you want, should be in the range [0 size-1]
     @return a pointer to the data at element i
    */
	inline T& operator[](const unsigned int i){
		return data[i];
	}
    
    /**
     Returns a pointer to the data at element i
     
     @param i: the index of the element you want, should be in the range [0 size-1]
     @return a const pointer to the data at element i
    */
    inline const T& operator[](const unsigned int i) const {
        return data[i];
    }
              
    /**
     Copies the data from the rhs vector to this vector.
     
     @param rhs: the vector you want to copy into this vector
     @return returns true or false, indicating if the copy was successful
     */
    virtual bool copy( const Vector<T> &rhs ){
        
        if( this != &rhs ){
            //Copy the data
            this->data = rhs.data;
        }
        
        return true;
    }

    /**
     Sets all the values in the Vector to the input value
     
     @param value: the value you want to set all the Vector values to
     @return returns true or false, indicating if the set was successful 
    */
	bool setAll(const T &value){

        const size_t N = data.size();

		if( N == 0 ) return false;

        for(size_t i=0; i<N; i++)
                data[i] = value;

        return true;
	}

    /**
     Gets the size of the Vector
     
     @return returns the size of the Vector
    */
	inline unsigned int getSize() const{ return (unsigned int)data.size(); }

    /**
     Gets a pointer to the first element in the vector
     
     @return returns a pointer to the raw data
     */
    T* getData() {
        if( data.size() == 0 ) return NULL;
        return &data[0];
    }

    /**
     Gets a pointer to the first element in the vector
     
     @return returns a pointer to the raw data
     */
    const T* getData() const {
        if( data.size() == 0 ) return NULL;
        return &data[0];
    }

protected:
    std::vector< T > data;
};

GRT_END_NAMESPACE

#endif //GRT_VECTOR_HEADER
