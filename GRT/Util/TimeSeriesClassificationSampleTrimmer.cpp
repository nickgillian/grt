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
#include "TimeSeriesClassificationSampleTrimmer.h"

GRT_BEGIN_NAMESPACE

TimeSeriesClassificationSampleTrimmer::TimeSeriesClassificationSampleTrimmer(Float trimThreshold,Float maximumTrimPercentage){
    this->trimThreshold = trimThreshold;
    this->maximumTrimPercentage = maximumTrimPercentage;
    warningLog.setKey("[WARNING TimeSeriesTrimmer]");
    errorLog.setKey("[ERROR TimeSeriesTrimmer]");
}

TimeSeriesClassificationSampleTrimmer::~TimeSeriesClassificationSampleTrimmer(){}

bool TimeSeriesClassificationSampleTrimmer::trimTimeSeries(TimeSeriesClassificationSample &timeSeries){
    
    const UINT M = timeSeries.getLength();
    const UINT N = timeSeries.getNumDimensions();
    
    if( M == 0 ){
        warningLog << "trimTimeSeries(TimeSeriesClassificationSample &timeSeries) - can't trim data, the length of the input time series is 0!" << std::endl;
        return false;
    }
    
    if( N == 0 ){
        warningLog << "trimTimeSeries(TimeSeriesClassificationSample &timeSeries) - can't trim data, the number of dimensions in the input time series is 0!" << std::endl;
        return false;
    }
    
    //Compute the energy of the time series
    Float maxValue = 0;
    VectorDouble x(M,0);
    
    for(UINT i=1; i<M; i++){
        for(UINT j=0; j<N; j++){
            x[i] += fabs(timeSeries[i][j]-timeSeries[i-1][j]);
        }
        x[i] /= N;
        if( x[i] > maxValue ) maxValue = x[i];
    }
    
    //Normalize x so that the maximum energy has a value of 1
    //At the same time search for the first time x[i] passes the trim threshold
    UINT firstIndex = 0;
    for(UINT i=1; i<M; i++){
        x[i] /= maxValue;
        
        if( x[i] > trimThreshold && firstIndex == 0 ){
            firstIndex = i;
        }
    }
    
    //Search for the last time x[i] passes the trim threshold
    UINT lastIndex = 0;
    for(UINT i=M-1; i>firstIndex; i--){
        if( x[i] > trimThreshold && lastIndex == 0 ){
            lastIndex = i;
            break;
        }
    }
    
    if( firstIndex == 0 && lastIndex == 0 ){
        warningLog << "Failed to find either the first index or the last index!" << std::endl;
        return false;
    }
    
    if( firstIndex == lastIndex ){
        warningLog << "The first index and last index are the same!" << std::endl;
        return false;
    }

	if( firstIndex > lastIndex ){
        warningLog << "The first index is greater than the last index!" << std::endl;
        return false;
    }
    
    if( lastIndex == 0 ){
        warningLog << "Failed to find the last index!" << std::endl;
        lastIndex = M-1;
    }
    
    //Compute how long the new time series would be if we trimmed it
    UINT newM = lastIndex-firstIndex;
    Float trimPercentage = (Float(newM) / Float(M)) * 100.0;
    
    if( 100 - trimPercentage <= maximumTrimPercentage ){
        
        MatrixDouble newTimeSeries(newM,N);
        UINT index = 0;
        for(UINT i=firstIndex; i<lastIndex; i++){
            for(UINT j=0; j<N; j++){
                newTimeSeries[index][j] = timeSeries[i][j];
            }
            index++;
        }
        
        timeSeries.setTrainingSample(timeSeries.getClassLabel(), newTimeSeries);
        return true;
    }
    
    warningLog << "Maximum Trim Percentage Excedded, Can't Trim Sample!";
    warningLog << " Original Timeseries Length: " << M << " Trimmed Timeseries Length: " << newM;
    warningLog << " Percentage: " << (100-trimPercentage) << " MaximumTrimPercentage: " << maximumTrimPercentage << std::endl;
    return false;
}
    
GRT_END_NAMESPACE

