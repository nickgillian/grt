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

#ifndef GRT_CLASS_TRACKER_HEADER
#define GRT_CLASS_TRACKER_HEADER

#include <iostream>
#include <string>

GRT_BEGIN_NAMESPACE

class ClassTracker{
public:

    ClassTracker(unsigned int classLabel = 0,unsigned int counter = 0, std::string className = "NOT_SET"){
        this->classLabel = classLabel;
        this->counter = counter;
        this->className = className;
    }
    
    ClassTracker(const ClassTracker &rhs){
        this->classLabel = rhs.classLabel;
        this->counter = rhs.counter;
        this->className = rhs.className;
    }
    
    ~ClassTracker(){}
    
    ClassTracker& operator= (const ClassTracker &rhs){
        if( this != &rhs){
            this->classLabel = rhs.classLabel;
            this->counter = rhs.counter;
            this->className = rhs.className;
        }
        return *this;
    }

	static bool sortByClassLabelDescending(ClassTracker a,ClassTracker b){
        return a.classLabel > b.classLabel;
    }
    
    static bool sortByClassLabelAscending(ClassTracker a,ClassTracker b){
        return a.classLabel < b.classLabel;
    }
    
    unsigned int classLabel;
    unsigned int counter;
    std::string className;
};

GRT_END_NAMESPACE

#endif //GRT_CLASS_TRACKER_HEADER
