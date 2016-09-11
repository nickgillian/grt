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

#ifndef GRT_DYNAMIC_TYPE_HEADER
#define GRT_DYNAMIC_TYPE_HEADER

#include "GRTTypedefs.h"
#include <memory>

GRT_BEGIN_NAMESPACE

class DynamicType{
public:
    DynamicType():data(NULL){
    }

    DynamicType( const DynamicType &rhs ):data(NULL){
        this->data = rhs.data;
    }

    ~DynamicType(){
        reset();
    }

    DynamicType& operator=(const DynamicType &rhs){
        if( this != &rhs){
            this->data = rhs.data;
        }
        return *this;
    }

    template<typename T>
    T& get(){
        return *std::static_pointer_cast<T>( data );
    }

    template<typename T>
    const T& get() const {
        return *std::static_pointer_cast<T>( data );
    }

    template<typename T>
    T get() const {
        return *std::static_pointer_cast<T>( data );
    }

    template<typename T>
    bool set( const T &newData ){
        data = std::make_shared< T >( newData );
        return true;
    }

    bool reset(){
        data.reset();
        return true;
    }

private:
    std::shared_ptr<void> data;
};

GRT_END_NAMESPACE

#endif //GRT_HEADER_GUARD