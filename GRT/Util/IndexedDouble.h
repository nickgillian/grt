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

#ifndef GRT_INDEXED_DOUBLE_HEADER
#define GRT_INDEXED_DOUBLE_HEADER

GRT_BEGIN_NAMESPACE

class IndexedDouble{
public:
    IndexedDouble(){
        index = 0;
        value = 0;
    }
    IndexedDouble(unsigned int index,double value){
        this->index = index;
        this->value = value;
    }
    IndexedDouble(const IndexedDouble &rhs){
        this->index = rhs.index;
        this->value = rhs.value;
    }
    ~IndexedDouble(){}
    
    IndexedDouble& operator= (const IndexedDouble &rhs){
        if( this != &rhs){
            this->index = rhs.index;
            this->value = rhs.value;
        }
        return *this;
    }
    
    static bool sortIndexedDoubleByIndexDescending(IndexedDouble a,IndexedDouble b){
        return a.index > b.index;
    }
    
    static bool sortIndexedDoubleByIndexAscending(IndexedDouble a,IndexedDouble b){
        return a.index < b.index;
    }
    
    static bool sortIndexedDoubleByValueDescending(IndexedDouble a,IndexedDouble b){
        return a.value > b.value;
    }
    
    static bool sortIndexedDoubleByValueAscending(IndexedDouble a,IndexedDouble b){
        return a.value < b.value;
    }
    
    unsigned int index;
    double value;
};

GRT_END_NAMESPACE

#endif //GRT_INDEXED_DOUBLE_HEADER
