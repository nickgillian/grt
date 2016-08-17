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
#include "PeakDetection.h"

GRT_BEGIN_NAMESPACE
    
PeakDetection::PeakDetection(const UINT lowPassFilterSize,const UINT searchWindowSize){
    
    this->lowPassFilterSize = lowPassFilterSize;
    this->searchWindowSize = searchWindowSize;
    
    inputTimeoutCounter = 0;
    maximaCounter = 0;
    minimaCounter = 0;
    inputTimeoutLimit = 10;
    searchHistorySize = 10;
    deadZoneThreshold = 0.01;
    enableSearch = false;
    peakDetected = false;
    
    setSearchWindowSize( searchWindowSize );
}
    
PeakDetection::PeakDetection(const PeakDetection &rhs){
    
    this->lowPassFilterSize = rhs.lowPassFilterSize;
    this->searchWindowSize = rhs.searchWindowSize;
    this->lowPassFilter = rhs.lowPassFilter;
}

PeakDetection::~PeakDetection(){

}
    
PeakDetection& PeakDetection::operator=(const PeakDetection &rhs){
    if(this!=&rhs){
        this->lowPassFilterSize = rhs.lowPassFilterSize;
        this->searchWindowSize = rhs.searchWindowSize;
        this->lowPassFilter = rhs.lowPassFilter;
    }
    return *this;
}

bool PeakDetection::update( const Float x){
    
    //Update the input counter
    if( ++inputTimeoutCounter >= inputTimeoutLimit ){
        inputTimeoutCounter = inputTimeoutLimit;
        enableSearch = true;
    }else enableSearch = false;
	
    peakDetected = false;
    peakInfo.clear();
    
    //Low pass filter the input data to remove some noise
    Float filteredValue = lowPassFilter.filter(x);
    
    //Compute the first deriv
    Float firstDeriv = filteredValue - filteredDataBuffer.getBack();
    
    //Compute the second deriv
    Float secondDeriv = firstDeriv - firstDerivBuffer.getBack();
    
    //Filter the second deriv using the deadzone filter, this removes any jitter around the 0
    secondDeriv = deadZoneFilter.filter( secondDeriv );
    
    //Store the values in the buffers
    filteredDataBuffer.push_back( filteredValue );
    firstDerivBuffer.push_back( firstDeriv );
    secondDerivBuffer.push_back( secondDeriv );

    //Search for any maxima/minima within the window
    unsigned int peakType = NO_PEAK_FOUND;
    
    //No matter what, set the global minima and maxima peak info to NO_PEAK_FOUND
    globalMaximaPeakInfo.peakType = NO_PEAK_FOUND;
    globalMinimaPeakInfo.peakType = NO_PEAK_FOUND;
    
    //Reset the global maxima value if it has exceeded the search history size 
    if( ++maximaCounter >= searchHistorySize ){
        maximaCounter = 0;
        globalMaximaPeakInfo.peakValue = DEFAULT_GLOBAL_MAXIMA_VALUE;
    }
    
    //Reset the global minima value if it has exceeded the search history size 
    if( ++minimaCounter >= searchHistorySize ){
        minimaCounter = 0;
        globalMinimaPeakInfo.peakValue = DEFAULT_GLOBAL_MINIMA_VALUE;
    }
    
    if( enableSearch ){
        for(unsigned int i=1; i<searchWindowSize-1; i++){
            peakType = NO_PEAK_FOUND;
            if( (secondDerivBuffer[i-1] <= 0 && secondDerivBuffer[i] > 0) || (secondDerivBuffer[i-1] <= 0 && secondDerivBuffer[i+1] > 0) ){
                
                peakDetected = true;
                peakType = LOCAL_MAXIMA_FOUND;
                
                //Check for a global maxima
                if( filteredDataBuffer[i] > globalMaximaPeakInfo.peakValue ){
                    maximaCounter = 0;
                    globalMaximaPeakInfo.peakType = GLOBAL_MAXIMA_FOUND;
                    globalMaximaPeakInfo.peakIndex = i;
                    globalMaximaPeakInfo.peakValue = filteredDataBuffer[i];
                }
                
                peakInfo.push_back( PeakInfo(peakType,i,filteredDataBuffer[i]) );
                break;
            }
            
            
            if( (secondDerivBuffer[i-1] >= 0 && secondDerivBuffer[i] < 0) || (secondDerivBuffer[i-1] >= 0 && secondDerivBuffer[i+1] < 0) ){
                
                peakDetected = true;
                peakType = LOCAL_MINIMA_FOUND;
                
                if( filteredDataBuffer[i] < globalMinimaPeakInfo.peakValue ){
                    minimaCounter = 0;
                    globalMinimaPeakInfo.peakType = GLOBAL_MINIMA_FOUND;
                    globalMinimaPeakInfo.peakIndex = i;
                    globalMinimaPeakInfo.peakValue = filteredDataBuffer[i];
                }
                
                peakInfo.push_back( PeakInfo(peakType,i,filteredDataBuffer[i]) );
                break;
            }
        }
    }
    
    peakTypesBuffer.push_back( peakType );
    
	return peakDetected ? true : false;
}
    
bool PeakDetection::reset(){
    //Reset the low pass filter
    lowPassFilter.init(lowPassFilterSize,1);
    
    //Reset the deadzone filter
    deadZoneFilter.init(-deadZoneThreshold,deadZoneThreshold,1);
    
    //Setup the data buffers
    filteredDataBuffer.clear();
    firstDerivBuffer.clear();
    secondDerivBuffer.clear();
    peakTypesBuffer.clear();
    
    filteredDataBuffer.resize( searchWindowSize, 0 );
    firstDerivBuffer.resize( searchWindowSize, 0 );
    secondDerivBuffer.resize( searchWindowSize, 0 );
    peakTypesBuffer.resize( searchWindowSize, NO_PEAK_FOUND );
    
    //Enable future searches
    peakDetected = false;
    inputTimeoutCounter = 0;
    maximaCounter = 0;
    minimaCounter = 0;
    
    globalMaximaPeakInfo.peakType = NO_PEAK_FOUND;
    globalMaximaPeakInfo.peakIndex = 0;
    globalMaximaPeakInfo.peakValue = DEFAULT_GLOBAL_MAXIMA_VALUE;
    globalMinimaPeakInfo.peakType = NO_PEAK_FOUND;
    globalMinimaPeakInfo.peakIndex = 0;
    globalMinimaPeakInfo.peakValue = DEFAULT_GLOBAL_MINIMA_VALUE;
    
    return true;
}
    
bool PeakDetection::setSearchWindowSize(const UINT searchWindowSize){
    this->searchWindowSize = searchWindowSize;
    reset();
    return true;
}

GRT_END_NAMESPACE
