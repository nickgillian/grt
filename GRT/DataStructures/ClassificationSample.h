/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
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

#ifndef GRT_CLASSIFICATION_SAMPLE_HEADER
#define GRT_CLASSIFICATION_SAMPLE_HEADER

#include "../Util/GRTCommon.h"

GRT_BEGIN_NAMESPACE

/**
 @brief This class stores the class label and raw data for a single labelled classification sample.
 */
class GRT_API ClassificationSample{
public:
	ClassificationSample();
	ClassificationSample(const UINT numDimensions);
	ClassificationSample(const UINT classLabel,const VectorFloat &sample);
	ClassificationSample(const ClassificationSample &rhs);
	~ClassificationSample();

	ClassificationSample& operator= (const ClassificationSample &rhs){
		if( this != &rhs){
			this->classLabel = rhs.classLabel;
			this->sample = rhs.sample;
			this->numDimensions = rhs.numDimensions;
		}
		return *this;
	}

	inline double& operator[] (const UINT &n){
		return sample[n];
	}

	inline const double& operator[] (const UINT &n) const{
    	return sample[n];
	}

	bool clear();

	UINT getNumDimensions() const{ return numDimensions; }
	UINT getClassLabel() const{ return classLabel; }
	const VectorFloat& getSample() const{ return sample; }
	VectorFloat& getSample() { return sample; }
    
	bool set(UINT classLabel,const VectorFloat &sample);
	bool setClassLabel(const UINT classLabel);
	bool setSample(const VectorFloat &sample);

protected:
	UINT numDimensions;
	UINT classLabel;
	VectorFloat sample;

};

GRT_END_NAMESPACE

#endif // GRT_CLASSIFICATION_SAMPLE_HEADER
