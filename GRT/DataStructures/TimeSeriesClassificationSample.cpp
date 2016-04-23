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

#define GRT_DLL_EXPORTS
#include "TimeSeriesClassificationSample.h"

GRT_BEGIN_NAMESPACE

//Constructors and Destructors
TimeSeriesClassificationSample::TimeSeriesClassificationSample():classLabel(0){};

TimeSeriesClassificationSample::TimeSeriesClassificationSample(const UINT classLabel,const MatrixFloat &data){
	this->classLabel = classLabel;
	this->data = data;
}

TimeSeriesClassificationSample::TimeSeriesClassificationSample(const TimeSeriesClassificationSample &rhs){
	this->classLabel = rhs.classLabel;
	this->data = rhs.data;
}

TimeSeriesClassificationSample::~TimeSeriesClassificationSample(){};

bool TimeSeriesClassificationSample::clear(){
	classLabel = 0;
	data.clear();
    return true;
}

bool TimeSeriesClassificationSample::addSample(const UINT classLabel,const VectorFloat &sample){
    this->classLabel = classLabel;
    this->data.push_back( sample );
    return true;
}
    
bool TimeSeriesClassificationSample::setTrainingSample(const UINT classLabel,const MatrixFloat &data){
	this->classLabel = classLabel;
	this->data = data;
    return true;
}

GRT_END_NAMESPACE
