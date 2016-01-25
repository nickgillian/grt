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

#ifndef GRT_DATATYPE_HEADER
#define GRT_DATATYPE_HEADER

#include "../DataStructures/VectorFloat.h"
#include "../DataStructures/MatrixFloat.h"

GRT_BEGIN_NAMESPACE

enum DataType {DATA_TYPE_UNKNOWN=0,DATA_TYPE_VECTOR,DATA_TYPE_MATRIX};

template< typename T >
const T data_type_cast( const void *data ){
    return dynamic_cast< T >( data );
}

template< typename T >
T data_type_cast( void *data ){
    return dynamic_cast< T >( data );
}

GRT_END_NAMESPACE

#endif //Header Guard