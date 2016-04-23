/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief The RangeTracker can be used to keep track of the expected ranges that might occur in a dataset.  
 These ranges can then be used to set the external ranges of a dataset for several of the GRT DataStructures.
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

#ifndef GRT_RANGE_TRACKER_HEADER
#define GRT_RANGE_TRACKER_HEADER

#include "GRTCommon.h"

GRT_BEGIN_NAMESPACE
    
#define BIG_POSITIVE_VALUE 99e+99
#define BIG_NEGATIVE_VALUE -99e+99

class GRT_API RangeTracker{
public:

	/**
     Default Constructor.
    */
	RangeTracker();

	/**
     Constructor, sets the number of dimensions of the data.
	 
	 @param UINT numDimensions: the number of dimensions of the data, should be an unsigned integer greater than 0
    */
	RangeTracker(UINT numDimensions);
    
    /**
     Copy Constructor, copies the RangeTracker from the rhs instance to this instance
     
	 @param const RangeTracker &rhs: another instance of the RangeTracker class from which the data will be copied to this instance
	*/
	RangeTracker(const RangeTracker &rhs);

	/**
     Default Destructor
    */
	~RangeTracker();

	/**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const RangeTracker &rhs: another instance of the RangeTracker class from which the data will be copied to this instance
	 @return a reference to this instance of the RangeTracker
	*/
	RangeTracker& operator= (const RangeTracker &rhs){
		if( this != &rhs){
			this->trackData = rhs.trackData;
            this->numDimensions = rhs.numDimensions;
            this->totalNumSamplesViewed = rhs.totalNumSamplesViewed;
            this->ranges = rhs.ranges;
		}
		return *this;
	}

	/**
     Clears any previous ranges and counters
    */
	void clear();
    
    /**
     Sets the number of dimensions in the data. 
	 This should be an unsigned integer greater than zero.  
	 This will clear any previous ranges and counters.

	 @param UINT numDimensions: the number of dimensions of the data.  Must be an unsigned integer greater than zero
     @return true if the number of dimensions was correctly updated, false otherwise
    */
    bool setNumDimensions(UINT numDimensions);
    
    /**
     Sets the if the RangeTracker should look at new data when the update function is called. 
     
	 @param bool trackData: sets if the RangeTracker should look at new data when the update function is called
     @return true if the trackData variable was set, false otherwise
     */
    bool enableTracking(bool trackData){ return this->trackData = trackData; }
    
    /**
     This is the main RangeTracker function, use this function to track the ranges of new data.
     The data will only be tracked if the tracking has been enabled (use the enableTracking function to control this).
     Tracking is enabled by default.
     The dimensionality of the input sample must match the number of dimensions the range tracker has been set to track.
     
	 @return true if the RangeTracker was correctly updated, false otherwise
     */
	bool update(VectorFloat sample);
    
    /**
     Returns if the range tracker is tracking the values of new samples.
     
	 @return true if the RangeTracker is tracking the values of new samples, false otherwise
     */
    bool trackingData(){ return trackData; }
    
    
    /**
     Save the range data to a file.
     
	 @return true if the range data was saved to a file, false otherwise
     */
    bool saveRangeDataToFile(std::string filename);
    
    /**
     Load the range data from a file.
     
	 @return true if the range data was loaded from a file, false otherwise
     */
    bool loadRangeDataFromFile(std::string filename);
    
	/**
     Gets the number of dimensions of the data.
     
	 @return an unsigned int representing the number of dimensions in the data
    */
	UINT inline getNumDimensions(){ return numDimensions; }
	
	/**
     Gets the number of samples viewed thus far.
     
	 @return an unsigned int representing the total number of samples in the classification data
    */
	ULONG inline getNumSamplesViewed(){ return totalNumSamplesViewed; }
	
	/**
     Gets the ranges of the data.
     
	 @return a Vector of minimum and maximum values for each dimension of the data
    */
	Vector<MinMax> getRanges();

private:
    bool trackData;                                         ///< Sets if the range tracker will look at the incoming data
	UINT numDimensions;										///< The number of dimensions in the data
	ULONG totalNumSamplesViewed;                            ///< The total number of samples seen to compute the ranges
    Vector< MinMax > ranges;
    
};

GRT_END_NAMESPACE

#endif //GRT_RANGE_TRACKER_HEADER
