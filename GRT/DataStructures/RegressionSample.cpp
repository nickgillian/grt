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
#include "RegressionSample.h"

GRT_BEGIN_NAMESPACE

RegressionSample::RegressionSample(){
}

RegressionSample::RegressionSample(const VectorFloat &inputVector,const VectorFloat &targetVector){
	this->inputVector = inputVector;
	this->targetVector = targetVector;
}

RegressionSample::RegressionSample(const RegressionSample &rhs){
	this->inputVector = rhs.inputVector;
	this->targetVector = rhs.targetVector;
}

RegressionSample::~RegressionSample(){
}

void RegressionSample::clear(){
	inputVector.clear();
	targetVector.clear();
}

void RegressionSample::set(const VectorFloat &inputVector,const VectorFloat &targetVector){
	this->inputVector = inputVector;
	this->targetVector = targetVector;
}

UINT RegressionSample::getNumInputDimensions() const{
    return inputVector.getSize();
}

UINT RegressionSample::getNumTargetDimensions() const{
    return targetVector.getSize();
}
    
Float RegressionSample::getInputVectorValue(const UINT index) const{
    if( index < inputVector.getSize() ) return inputVector[index];
    else return 0;
}
Float RegressionSample::getTargetVectorValue(const UINT index) const{
    if( index < targetVector.getSize() ) return targetVector[index];
    else return 0;
}

const VectorFloat& RegressionSample::getInputVector() const{
    return inputVector;
}

const VectorFloat& RegressionSample::getTargetVector() const{
    return targetVector;
}
	
GRT_END_NAMESPACE

