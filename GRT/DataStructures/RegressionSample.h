/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class stores the input vector and target vector for a single labelled regression instance.
 */

/**
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

#ifndef GRT_REGRESSION_SAMPLE_HEADER
#define GRT_REGRESSION_SAMPLE_HEADER

#include "../Util/GRTCommon.h"

GRT_BEGIN_NAMESPACE

class GRT_API RegressionSample{
public:
	RegressionSample();
	RegressionSample(const VectorFloat &inputVector,const VectorFloat &targetVector);
	RegressionSample(const RegressionSample &rhs);
	~RegressionSample();

	RegressionSample& operator= (const RegressionSample &rhs){
		if( this != &rhs){
			this->inputVector = rhs.inputVector;
			this->targetVector = rhs.targetVector;
		}
		return *this;
	}
	
	
	static bool sortByInputVectorAscending(const RegressionSample &a,const RegressionSample &b){
        return a.inputVector < b.inputVector;
    }

    static bool sortByInputVectorDescending(const RegressionSample &a,const RegressionSample &b){
        return a.inputVector > b.inputVector;
    }

	void clear();
	void set(const VectorFloat &inputVector,const VectorFloat &targetVector);

    UINT getNumInputDimensions() const;
    UINT getNumTargetDimensions() const;
    Float getInputVectorValue(const UINT index) const;
    Float getTargetVectorValue(const UINT index) const;
    const VectorFloat& getInputVector() const;
    const VectorFloat& getTargetVector() const;

private:
	VectorFloat inputVector;
	VectorFloat targetVector;
};

GRT_END_NAMESPACE

#endif //GRT_REGRESSION_SAMPLE_HEADER
