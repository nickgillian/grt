/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief This class provides a useful tool to automatically trim timeseries data.

  Sometimes, when a user records a timeseries example, there is a significant pause at the start and end of the example 
  where the recording has started but the user has not started to move yet.  These areas of 'static' data (at the start 
  and end of a recording) can generate poor classification models (as the classification model expects to see similar 
  static data in the real time data).

  The LabelledTimeSeriesClassificationSampleTrimmer attempts to detect and remove these static areas of data. This is done
  by computing the summed absolute energy of the timeseries data, normalizing the energy profile by the maximum energy value,
  and then searching for areas at the start and end of the timeseries that are below a specific trimthreshold (set by the user).
  Any data that is below the trimthreshold will be removed, up until the first value that exceeds the threshold.  This search is
  run both from the start of the timeseries (searching forward) and the end of the timeseries (searching backwards).  If the length
  of the new timeseries is below the maximumTrimPercentage, then the timeseries will be trimmed and the trimTimeSeries function
  will return true.  If the length of the new is above the maximumTrimPercentage, then the timeseries will not be trimmed and the
  trimTimeSeries function will return false. Set the maximumTrimPercentage to 100 if you want the timeseries to always be trimmed.
 */

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

#ifndef GRT_TIME_SERIES_TRIMMER_HEADER
#define GRT_TIME_SERIES_TRIMMER_HEADER

#include "../DataStructures/TimeSeriesClassificationData.h"

GRT_BEGIN_NAMESPACE

class GRT_API TimeSeriesClassificationSampleTrimmer{
public:

	/**
     Default Constructor.
  */
  TimeSeriesClassificationSampleTrimmer(Float trimThreshold=0.1,Float maximumTrimPercentage=80);

	/**
    Default Destructor
  */
  ~TimeSeriesClassificationSampleTrimmer();

	/**
   Defines the equals operator. Copies the settings from the rhs instance to this instance

   @param rhs: the instance from which the settings will be copied
   @return returns a reference to this instance
  */
	TimeSeriesClassificationSampleTrimmer& operator= (const TimeSeriesClassificationSampleTrimmer &rhs){
    if( this != &rhs){
      this->trimThreshold = rhs.trimThreshold;
      this->maximumTrimPercentage = rhs.maximumTrimPercentage;
      this->warningLog = rhs.warningLog;
      this->errorLog = rhs.errorLog;
    }
    return *this;
  }

  /**
   The function attempts to detect and remove these static areas of data. This is done by computing the summed absolute energy of the 
   timeseries data, normalizing the energy profile by the maximum energy value, and then searching for areas at the start and end of 
   the timeseries that are below a specific trimthreshold (set by the user).
	  
   Any data that is below the trimthreshold will be removed, up until the first value that exceeds the threshold.  This search is
   run both from the start of the timeseries (searching forward) and the end of the timeseries (searching backwards).  If the length
   of the new timeseries is below the maximumTrimPercentage, then the timeseries will be trimmed and the trimTimeSeries function
   will return true.  If the length of the new is above the maximumTrimPercentage, then the timeseries will not be trimmed and the
   trimTimeSeries function will return false. Set the maximumTrimPercentage to 100 if you want the timeseries to always be trimmed.

   @param timeSeries: the timeseries to be trimmed (will be trimmed in place)
   @return returns true if the timeseries was trimmed, false otherwise
  */
  bool trimTimeSeries(TimeSeriesClassificationSample &timeSeries);

protected:
    Float trimThreshold;
    Float maximumTrimPercentage;
    WarningLog warningLog;
    ErrorLog errorLog;
    
};

GRT_END_NAMESPACE

#endif // GRT_TIME_SERIES_TRIMMER_HEADER
