/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
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

#ifndef GRT_CIRCULAR_BUFFER_HEADER
#define GRT_CIRCULAR_BUFFER_HEADER
#include <iostream>
#include <vector>
#include "GRTTypedefs.h"
#include "ErrorLog.h"

GRT_BEGIN_NAMESPACE

/**
 @brief The CircularBuffer class provides a data structure for creating a dynamic circular buffer (also known as a 
 cyclic buffer or a ring buffer).  The main advantage of a circular buffer is that it does not need to have
 its elements shuffled around each time a new element is added.  The circular buffer therefore works well
 for FIFO (first in first out) buffers.
 
 @example UtilExamples/CircularBufferExample/CircularBufferExample.cpp
 */
template <class T>
class CircularBuffer{
    public:
    
    /**
     Default Constructor
     */
    CircularBuffer(){
        errorLog.setKey("[ERROR CircularBuffer]");
        bufferSize = 0;
        numValuesInBuffer = 0;
        readPtr = 0;
        writePtr = 0;
        bufferInit = false;
    }
    
    /**
     Copy Constructor. Deep copies the data from the rhs instance to this instance
     
     @param const CircularBuffer &rhs: ths instance the data and settings will be copied from
     */
    CircularBuffer(const CircularBuffer &rhs){
        errorLog.setKey("[ERROR CircularBuffer]");
        this->bufferSize = 0;
        this->numValuesInBuffer = 0;
        this->readPtr = 0;
        this->writePtr = 0;
        this->bufferInit = false;
            
        if( rhs.bufferInit ){
            this->bufferInit = rhs.bufferInit;
            this->bufferSize = rhs.bufferSize;
            this->numValuesInBuffer = rhs.numValuesInBuffer;
            this->buffer.resize( rhs.bufferSize );
            for(unsigned int i=0; i<rhs.bufferSize; i++){
                this->buffer[i] = rhs.buffer[ i ];
            }
            this->readPtr = rhs.readPtr;
            this->writePtr = rhs.writePtr;
        }
    }
    
    /**
     Init Constructor. Resizes the buffer to the size set by bufferSize.
     
     @param unsigned int bufferSize: sets the size of the buffer
     */
    CircularBuffer(unsigned int bufferSize){
        errorLog.setKey("[ERROR CircularBuffer]");
        bufferInit = false;
        resize(bufferSize);
    }
    
    /**
     Default Destructor. Cleans up any memory assigned by the buffer.
     */
    virtual ~CircularBuffer(){
        if( bufferInit ){
            clear();
        }
    }
    
    /**
     Sets the equals operator. Defines how the data will be copied from the rhs instance to this instance.
     
     @param const CircularBuffer &rhs: the instance the data will be copied from
     @return returns a reference to this instance
     */
    CircularBuffer& operator=(const CircularBuffer &rhs){
        if( this != &rhs ){
            this->clear();

            if( rhs.bufferInit ){
                this->bufferInit = rhs.bufferInit;
                this->bufferSize = rhs.bufferSize;
                this->numValuesInBuffer = rhs.numValuesInBuffer;
                this->buffer.resize( rhs.bufferSize );
                for(unsigned int i=0; i<rhs.bufferSize; i++){
                    this->buffer[i] = rhs.buffer[ i ];
                }
                this->readPtr = rhs.readPtr;
                this->writePtr = rhs.writePtr;
            }
        }
        return *this;
    }
    
    /**
     This is the main access operator and will return a value relative to the current read pointer.
     
     @param const unsigned int &index: the index of the element you want access to, should be in the range [0 bufferSize-1]
     @return returns a reference to the element at the index
     */
    inline T& operator[](const unsigned int &index){
        return buffer[ (readPtr + index) % bufferSize ];
    }
    
    /**
     This is the constant access operator and will return a value relative to the current read pointer.
     
     @param const unsigned int &index: the index of the element you want access to, should be in the range [0 bufferSize-1]
     @return returns a const reference to the element at the index
     */
    inline const T& operator[](const unsigned int &index) const{
        return buffer[ (readPtr + index) % bufferSize ];
    }
    
    /**
     This is a special access operator that will return the value at the specific index (regardless of the position of the read pointer).
     
     @param const unsigned int &index: the index of the element you want access to, should be in the range [0 bufferSize-1]
     @return returns a reference to the element at the index
     */
    inline T& operator()(const unsigned int &index){
        return buffer[ index ];
    }
    
    /**
     This is a special const access operator that will return the value at the specific index (regardless of the position of the read pointer).
     
     @param const unsigned int &index: the index of the element you want access to, should be in the range [0 bufferSize-1]
     @return returns a const reference to the element at the index
     */
    inline const T& operator()(const unsigned int &index) const{
        return buffer[ index ];
    }
    
    /**
     Resizes the buffer to the newBufferSize, which must be greater than zero.
     
     @param const unsigned int newBufferSize: the new size of the buffer
     @return returns true if the buffer was resized
     */
    bool resize(const unsigned int newBufferSize){
        return resize(newBufferSize,T());
    }
    
    /**
     Resizes the buffer to the newBufferSize, which must be greater than zero, and sets all the values to the default value.
     
     @param const unsigned int newBufferSize: the new size of the buffer
     @param const T &defaultValue: the default value that will be copied to every element
     @return returns true if the buffer was resized
     */
    bool resize(const unsigned int newBufferSize,const T &defaultValue){
        
        //Cleanup the old memory
        clear();
        
        if( newBufferSize == 0 ) return false;
        
        //Setup the memory for the new buffer
        bufferSize = newBufferSize;
        buffer.resize(bufferSize,defaultValue);
        numValuesInBuffer = 0;
        readPtr = 0;
        writePtr = 0;
        
        //Flag that the filter has been initialised
        bufferInit = true;
        
        return true;
    }
    
    /**
     Push the new value into the end of the buffer, this will move both the read and write pointers.
     
     @param const T &value: the value that should be added to the end of the buffer
     @return returns true if the value was pushed, false otherwise
     */
    bool push_back(const T &value){
        
        if( !bufferInit ){
            errorLog << "Can't push_back value to circular buffer as the buffer has not been initialized!" << std::endl;
            return false;
        }
    
		//Add the value to the buffer
        buffer[ writePtr ] = value;

		//Update the write pointer
        writePtr++;
        writePtr = writePtr % bufferSize;
        
        //Check if the buffer is full
        if( ++numValuesInBuffer > bufferSize ){
            numValuesInBuffer = bufferSize;

			//Only update the read pointer if the buffer has been filled
            readPtr++;
	        readPtr = readPtr % bufferSize;
        }

        return true;
    }
    
    /**
     Sets all the values in the buffer to the value.
     
     @param const T &value: the value that will be copied to all the elements in the buffer
     @return returns true if the buffer was updated, false otherwise
     */
    bool setAllValues(const T &value){
        if( !bufferInit ){
            return false;
        }
        
        for(unsigned int i=0; i<bufferSize; i++){
            buffer[i] = value;
        }
        
        return true;
    }
    
    /**
     Resets the numValuesInBuffer, read and write pointers to 0.
     @return returns true if the buffer was reset, false otherwise
     */
    bool reset(){
		numValuesInBuffer = 0;
		readPtr = 0;
		writePtr = 0;
        return true;
    }
    
    /**
     Clears the buffer, setting the size to 0.
     */
    void clear(){
		numValuesInBuffer = 0;
		readPtr = 0;
		writePtr = 0;
		buffer.clear();
		bufferInit = false;
    }
    
    /**
     @deprecated This function is now deprecated, you should use getData instead!
     Gets all the data in the buffer as a std::vector.
     
     @return returns a vector will all the data in the buffer
     */
    GRT_DEPRECATED_MSG("Use getData() instead!", std::vector< T > getDataAsVector() const );

    /**
     Gets the data in the CircularBuffer's internal buffer as a std::vector.  The argument, rawBuffer, controls if the function
     returns (if rawBuffer = true ) the entire contents of the internal buffer (with unsorted values and a constant size regardless on whether it is filled),
     or the current contents of the Circular Buffer in order from oldest to newest (if rawBuffer = false).
     
     @return returns a std::vector with the data in the buffer
     */
    Vector< T > getData( const bool rawBuffer = false ) const{
        if( bufferInit ){

            if( rawBuffer ){
                return buffer; //Here we return the entire data buffer
            }else{ //Here we return only valid elements and in order from oldest to newest
                Vector< T > data( numValuesInBuffer );
                for(unsigned int i=0; i<numValuesInBuffer; i++){
                    data[i] = (*this)[i]; //Gets the ordered element
                }
                return data;
            }
        }
        return Vector< T >();
    }
    
    /**
     Returns true if the buffer has been initialized.
     
     @return returns true if the buffer has been initialized, false otherwise
     */
    bool getInit() const { return bufferInit; }
    
    /**
     Returns true if the buffer has been filled. If the buffer has not been initialized then this function will always return false.
     
     @return returns true if the buffer has been filled, false otherwise
     */
    bool getBufferFilled() const { return bufferInit ? numValuesInBuffer==bufferSize : false; }
    
    /**
     Returns the size of the buffer.
     
     @return returns the size of the bufer
     */
    unsigned int getSize() const { return bufferInit ? bufferSize : 0; }
    
    /**
     Returns the number of values in the buffer.
     
     @return returns the number of values in the buffer
     */
    unsigned int getNumValuesInBuffer() const { return bufferInit ? numValuesInBuffer : 0; }
    
    /**
     Returns the current position of the read pointer.
     
     @return returns the current position of the read pointer
     */
    unsigned int getReadPointerPosition() const { return bufferInit ? readPtr : 0; }
    
    /**
     Returns the current position of the write pointer.
     
     @return returns the current position of the write pointer
     */
    unsigned int getWritePointerPosition() const { return bufferInit ? writePtr : 0; }
    
    /**
     Returns the most recent value added to the buffer.
     
     @return returns the most recent value added to the buffer
     */
    T getBack() const {
        if( !bufferInit ) return T();
        return buffer[ (readPtr + numValuesInBuffer - 1) % bufferSize ];
    }
    
protected:
    bool bufferInit;
    unsigned int bufferSize;
    unsigned int numValuesInBuffer;
    unsigned int readPtr;
    unsigned int writePtr;
    Vector< T > buffer;
    
    ErrorLog errorLog;
};

//Deprecated function
template< class T > std::vector< T > CircularBuffer< T >::getDataAsVector() const{ return getData(); };

GRT_END_NAMESPACE

#endif //GRT_CIRCULAR_BUFFER_HEADER
