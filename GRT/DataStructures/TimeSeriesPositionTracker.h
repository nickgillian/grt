/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class can be used to track the class label, start and end indexs for labelled data.
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

#ifndef GRT_TIME_SERIES_POSITION_TRACKER_HEADER
#define GRT_TIME_SERIES_POSITION_TRACKER_HEADER

#include "../Util/GRTCommon.h"

GRT_BEGIN_NAMESPACE

class TimeSeriesPositionTracker{
public:
    /**
     Constructor, sets the start index, end index and class label of the timeseries that is to be tracked.
	 
     @param startIndex: the index that the tracked timeseries starts in a given dataset.  Default value is startIndex=0
     @param endIndex: the index that the tracked timeseries ends in a given dataset.  Default value is endIndex=0
     @param classLabel: the class label of the tracked timeseries.  Default value is classLabel=0
     */
	TimeSeriesPositionTracker(UINT startIndex=0,UINT endIndex=0,UINT classLabel=0){
		this->startIndex = startIndex;
	    this->endIndex = endIndex;
	    this->classLabel = classLabel;
	}
    
    /**
     Copy Constructor, copies the TimeSeriesPositionTracker from the rhs instance to this instance
     
	 @param rhs: another instance of the TimeSeriesPositionTracker class from which the data will be copied to this instance
     */
	TimeSeriesPositionTracker(const TimeSeriesPositionTracker &rhs){
		this->startIndex = rhs.startIndex;
		this->endIndex = rhs.endIndex;
		this->classLabel = rhs.classLabel;
	}
    
    /**
     Default Destructor
     */
	~TimeSeriesPositionTracker(){};

    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param rhs: another instance of the TimeSeriesPositionTracker class from which the data will be copied to this instance
	 @return a reference to this instance of TimeSeriesPositionTracker
     */
	TimeSeriesPositionTracker& operator= (const TimeSeriesPositionTracker &rhs){
		if( this != &rhs){
			this->startIndex = rhs.startIndex;
			this->endIndex = rhs.endIndex;
			this->classLabel = rhs.classLabel;
		}
		return *this;
	}

    
    /**
     Sets the tracker values
     
	 @param startIndex: the index that the tracked timeseries starts in a given dataset
     @param endIndex: the index that the tracked timeseries ends in a given dataset
     @param classLabel: the class label of the tracked timeseries
	 @return void
     */
	void setTracker(UINT startIndex,UINT endIndex,UINT classLabel){
		this->startIndex = startIndex;
		this->endIndex = endIndex;
		this->classLabel = classLabel;
	}

    /**
     Sets the start index
     
	 @param startIndex: the index that the tracked timeseries starts in a given dataset
	 @return void
     */
	void setStartIndex(UINT startIndex){ this->startIndex = startIndex; }
    
    /**
     Sets the end index
     
	 @param endIndex: the index that the tracked timeseries ends in a given dataset
	 @return void
     */
	void setEndIndex(UINT endIndex){ this->endIndex = endIndex; }
    
    /**
     Sets the class label
     
	 @param classLabel: the class label of the tracked timeseries
	 @return void
     */
    void setClassLabel(UINT classLabel){ this->classLabel = classLabel; }

    /**
     Gets the start index
     
	 @return an UINT representing the index that the tracked timeseries starts in a given dataset
     */
	UINT getStartIndex() const { return startIndex; }
    
    /**
     Gets the end index
     
	 @return an UINT representing the index that the tracked timeseries ends in a given dataset
     */
	UINT getEndIndex() const { return endIndex; }
    
    /**
     Gets the length of the timeseries
     
	 @return an UINT representing the length of the tracked timeseries
     */
	UINT getLength() const { return endIndex-startIndex+1; }
    
    /**
     Gets the class label of the tracked timeseries
     
	 @return an UINT representing the class label of the tracked timeseries in a given dataset
     */
	UINT getClassLabel() const { return classLabel; }

protected:
	UINT startIndex;
	UINT endIndex;
	UINT classLabel;
};

GRT_END_NAMESPACE

#endif //GRT_TIME_SERIES_POSITION_TRACKER_HEADER
