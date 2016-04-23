/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class stores the timeseries data for a single labelled timeseries classification sample.
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

#ifndef GRT_TIME_SERIES_CLASSIFICATION_SAMPLE_HEADER
#define GRT_TIME_SERIES_CLASSIFICATION_SAMPLE_HEADER

#include "VectorFloat.h"
#include "MatrixFloat.h"

GRT_BEGIN_NAMESPACE

class GRT_API TimeSeriesClassificationSample{
public:
	TimeSeriesClassificationSample();
	TimeSeriesClassificationSample(const UINT classLabel,const MatrixFloat &data);
	TimeSeriesClassificationSample(const TimeSeriesClassificationSample &rhs);
	~TimeSeriesClassificationSample();

	TimeSeriesClassificationSample& operator= (const TimeSeriesClassificationSample &rhs){
		if( this != &rhs){
			this->classLabel = rhs.classLabel;
			this->data = rhs.data;
		}
		return *this;
	}

	inline Float* operator[] (const UINT &n){
		return data[n];
	}
    
    inline const Float* operator[] (const UINT &n) const {
		return data[n];
	}

	bool clear();
    bool addSample( const UINT classLabel, const VectorFloat &sample );
	bool setTrainingSample( const UINT classLabel, const MatrixFloat &data );
	inline UINT getLength() const { return data.getNumRows(); }
    inline UINT getNumDimensions() const { return data.getNumCols(); }
    inline UINT getClassLabel() const { return classLabel; }
    MatrixFloat &getData(){ return data; }
    const MatrixFloat &getData() const { return data; }

protected:
	UINT classLabel;
	MatrixFloat data;
};

GRT_END_NAMESPACE

#endif //GRT_LABELLED_TIME_SERIES_CLASSIFICATION_SAMPLE_HEADER
