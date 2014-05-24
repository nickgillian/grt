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

#ifndef GRT_MINMAX_HEADER
#define GRT_MINMAX_HEADER

namespace GRT{

//Common classes
class MinMax{
public:
	MinMax():minValue(0),maxValue(0){};
    MinMax(double minValue,double maxValue){
        this->minValue = minValue;
        this->maxValue = maxValue;
    }
	~MinMax(){};

	MinMax& operator= (const MinMax &rhs){
		if( this != &rhs){
			this->minValue = rhs.minValue;
			this->maxValue = rhs.maxValue;
		}
		return *this;
	}
    
    bool updateMinMax(double newValue){
        if( newValue < minValue ){
            minValue = newValue;
            return true;
        }
        if( newValue > maxValue ){
            maxValue = newValue;
            return true;
        }
        return false;
    }

	double minValue;
	double maxValue;
};

}; //End of namespace GRT

#endif //GRT_MINMAX_HEADER