/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief The TimeSeriesClassificationDataStream is the main data structure for recording, labeling, managing, saving, and loading datasets that can be used to test the continuous classification abilities of the GRT supervised temporal learning algorithms.
 
 @example DatastructuresExample/TimeSeriesClassificationDataStreamExample/TimeSeriesClassificationDataStreamExample.cpp
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

#ifndef GRT_TIME_SERIES_CLASSIFICATION_DATA_STREAM_HEADER
#define GRT_TIME_SERIES_CLASSIFICATION_DATA_STREAM_HEADER

#include "../Util/GRTCommon.h"
#include "TimeSeriesPositionTracker.h"
#include "ClassificationData.h"
#include "TimeSeriesClassificationData.h"

namespace GRT{

class TimeSeriesClassificationDataStream{
public:
	
    /**
     Constructor, sets the name of the dataset and the number of dimensions of the training data.
     The name of the dataset should not contain any spaces.
	 
     @param UINT numDimensions: the number of dimensions of the training data, should be an unsigned integer greater than 0
     @param string datasetName: the name of the dataset, should not contain any spaces
     @param string infoText: some info about the data in this dataset, this can contain spaces
    */
	TimeSeriesClassificationDataStream(const UINT numDimensions=0,const string datasetName = "NOT_SET",const string infoText = "");
    
    /**
     Copy Constructor, copies the TimeSeriesClassificationDataStream from the rhs instance to this instance
     
	 @param const TimeSeriesClassificationDataStream &rhs: another instance of the TimeSeriesClassificationDataStream class from which the data will be copied to this instance
     */
	TimeSeriesClassificationDataStream(const TimeSeriesClassificationDataStream &rhs);
    
    /**
     Default Destructor
     */
	~TimeSeriesClassificationDataStream();

    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const TimeSeriesClassificationDataStream &rhs: another instance of the TimeSeriesClassificationDataStream class from which the data will be copied to this instance
	 @return a reference to this instance of TimeSeriesClassificationDataStream
     */
	TimeSeriesClassificationDataStream& operator= (const TimeSeriesClassificationDataStream &rhs);

    /**
     Array Subscript Operator, returns the ClassificationSample at index i.
	 It is up to the user to ensure that i is within the range of [0 totalNumSamples-1]
     
	 @param const UINT &i: the index of the training sample you want to access.  Must be within the range of [0 totalNumSamples-1]
     @return a reference to the i'th ClassificationSample
     */
	inline ClassificationSample& operator[] (const UINT i){
		return data[i];
	}

    /**
     Clears any previous training data and counters
     */
	void clear();
    
    /**
     Sets the number of dimensions in the training data. 
	 This should be an unsigned integer greater than zero.  
	 This will clear any previous training data and counters.
     This function needs to be called before any new samples can be added to the dataset, unless the numDimensions variable was set in the 
     constructor or some data was already loaded from a file
     
	 @param const UINT numDimensions: the number of dimensions of the training data.  Must be an unsigned integer greater than zero
     @return true if the number of dimensions was correctly updated, false otherwise
     */
    bool setNumDimensions(const UINT numDimensions);
    
    /**
     Sets the name of the dataset.
     There should not be any spaces in the name.
     Will return true if the name is set, or false otherwise.
     
	 @return returns true if the name is set, or false otherwise
     */
    bool setDatasetName(const string datasetName);
    
    /**
     Sets the info string.
	 This can be any string with information about how the training data was recorded for example.
     
	 @param const string infoText: the infoText
     @return true if the infoText was correctly updated, false otherwise
     */
    bool setInfoText(const string infoText);
    
    /**
     Sets the name of the class with the given class label.  
     There should not be any spaces in the className.
     Will return true if the name is set, or false if the class label does not exist.
     
	 @return returns true if the name is set, or false if the class label does not exist
     */
    bool setClassNameForCorrespondingClassLabel(const string className,const UINT classLabel);
    
    /**
     Adds a new labelled sample to the dataset.  
     The dimensionality of the sample should match the number of dimensions in the TimeSeriesClassificationDataStream.
     The class label can be zero (this should represent a null class).
     
	 @param const UINT classLabel: the class label of the corresponding sample
     @param const VectorDouble &sample: the new sample you want to add to the dataset.  The dimensionality of this sample should match the number of dimensions in the TimeSeriesClassificationDataStream
	 @return true if the sample was correctly added to the dataset, false otherwise
     */
	bool addSample(const UINT classLabel,const VectorDouble &trainingSample);
    
    /**
     Removes the last training sample added to the dataset.
     
	 @return true if the last sample was removed, false otherwise
     */
	bool removeLastSample();
    
    /**
     Deletes from the dataset all the samples with a specific class label.
     
	 @param const UINT classLabel: the class label of the samples you wish to delete from the dataset
	 @return the number of samples deleted from the dataset
     */
	UINT eraseAllSamplesWithClassLabel(const UINT classLabel);
    
    /**
     Relabels all the samples with the class label A with the new class label B.
     
	 @param const UINT oldClassLabel: the class label of the samples you want to relabel
     @param const UINT newClassLabel: the class label the samples will be relabelled with
	 @return returns true if the samples were correctly relablled, false otherwise
     */
	bool relabelAllSamplesWithClassLabel(const UINT oldClassLabel,const UINT newClassLabel);
    
    /**
     Sets the external ranges of the dataset, also sets if the dataset should be scaled using these values.  
     The dimensionality of the externalRanges vector should match the number of dimensions of this dataset.
     
	 @param const vector< MinMax > &externalRanges: an N dimensional vector containing the min and max values of the expected ranges of the dataset.
     @param const bool useExternalRanges: sets if these ranges should be used to scale the dataset, default value is false.
	 @return returns true if the external ranges were set, false otherwise
     */
    bool setExternalRanges(const vector< MinMax > &externalRanges,const bool useExternalRanges = false);
    
    /**
     Sets if the dataset should be scaled using an external range (if useExternalRanges == true) or the ranges of the dataset (if false).
     The external ranges need to be set FIRST before calling this function, otherwise it will return false.
     
     @param bool useExternalRanges: sets if these ranges should be used to scale the dataset
	 @return returns true if the useExternalRanges variable was set, false otherwise
     */
    bool enableExternalRangeScaling(const bool useExternalRanges);
    
	/**
     Scales the dataset to the new target range.  This function uses the minimum and maximum values of the current dataset as the source range.
     
     @param const double minTarget: the minimum value of the target range
     @param const double maxTarget: the maximum value of the target range
	 @return true if the data was scaled correctly, false otherwise
     */
    bool scale(const double minTarget,const double maxTarget);
    
	/**
     Scales the dataset to the new target range, using the vector of ranges as the min and max source ranges.
     
     @param const vector<MinMax> &ranges: the minimum and maximum values for the source range
     @param const double minTarget: the minimum value of the target range
     @param const double maxTarget: the maximum value of the target range
	 @return true if the data was scaled correctly, false otherwise
     */
	bool scale(const vector<MinMax> &ranges,const double minTarget,const double maxTarget);
    
    /**
     Sets the playback index to a specific index.  The index should be within the range [0 totalNumSamples-1].
     
	 @param const UINT playbackIndex: the value you want to set the playback index to
	 @return true if the playback index was set correctly, false otherwise
     */
    bool resetPlaybackIndex(const UINT playbackIndex);
    
    /**
     Gets the next sample, this will also increment the playback index.
     If the playback index reaches the last data sample then it will be reset to 0.
     
	 @return the ClassificationSample at the current playback index
     */
    ClassificationSample getNextSample();
    
    /**
     Gets all the timeseries that have a specific class label.
     
     @param const UINT classLabel: the class label of the timeseries you want to find
	 @return a TimeSeriesClassificationData dataset containing any timeseries that have the matching classlabel
     */
	TimeSeriesClassificationData getAllTrainingExamplesWithClassLabel(const UINT classLabel) const;

    /**
     Saves the labelled timeseries classification data to a custom file format.
     
	 @param string filename: the name of the file the data will be saved to
	 @return true if the data was saved successfully, false otherwise
     */
	bool saveDatasetToFile(const string filename);
    
    /**
     Loads the labelled timeseries classification data from a custom file format.
     
	 @param string filename: the name of the file the data will be loaded from
	 @return true if the data was loaded successfully, false otherwise
     */
	bool loadDatasetFromFile(const string filename);
    
    /**
     Saves the labelled timeseries classification data to a CSV file.
     This will save the class label as the first column and the sample data as the following N columns, where N is the number of dimensions in the data.  Each row will represent a sample.
     
	 @param string filename: the name of the file the data will be saved to
	 @return true if the data was saved successfully, false otherwise
     */
    bool saveDatasetToCSVFile(const string filename) const;
    
    /**
     Loads the labelled timeseries classification data from a CSV file.
     This assumes the data is formatted with each row representing a sample.
     The class label should be the first column followed by the sample data as the following N columns, where N is the number of dimensions in the data.
     If the class label is not the first column, you should set the 2nd argument (UINT classLabelColumnIndex) to the column index that contains the class label.
     
	 @param const string filename: the name of the file the data will be loaded from
     @param const UINT classLabelColumnIndex: the index of the column containing the class label. Default value = 0
	 @return true if the data was loaded successfully, false otherwise
     */
	bool loadDataSetFromCSVFile(const string filename,const UINT classLabelColumnIndex=0);
    
    /**
     Prints the dataset info (such as its name and infoText) and the stats (such as the number of examples, number of dimensions, number of classes, etc.)
     to the std out.
     
     @return returns true if the dataset info and stats were printed successfully, false otherwise
     */
    bool printStats() const;
    
    /**
     Gets the name of the dataset.
     
	 @return returns the name of the dataset
     */
    string getDatasetName() const { return datasetName; }
    
    /**
     Gets the infotext for the dataset
     
	 @return returns the infotext of the dataset
     */
    string getInfoText() const { return infoText; }
    
	/**
     Gets the number of dimensions of the labelled classification data.
     
	 @return an unsigned int representing the number of dimensions in the classification data
     */
	UINT inline getNumDimensions() const { return numDimensions; }
	
	/**
     Gets the number of samples in the classification data across all the classes.
     
	 @return an unsigned int representing the total number of samples in the classification data
     */
	UINT inline getNumSamples() const { return totalNumSamples; }
	
	/**
     Gets the number of classes.
     
	 @return an unsigned int representing the number of classes
     */
	UINT inline getNumClasses() const { return (UINT)classTracker.size(); }
    
    /**
     Gets the minimum class label in the dataset. If there are no values in the dataset then the value 99999 will be returned.
     
	 @return an unsigned int representing the minimum class label in the dataset
     */
    UINT getMinimumClassLabel() const;
    
    /**
     Gets the maximum class label in the dataset. If there are no values in the dataset then the value 0 will be returned.
     
	 @return an unsigned int representing the maximum class label in the dataset
     */
    UINT getMaximumClassLabel() const;
    
    /**
     Gets the index of the class label from the class tracker.
     
	 @return an unsigned int representing the index of the class label in the class tracker
     */
    UINT getClassLabelIndexValue(const UINT classLabel) const;
    
    /**
     Gets the name of the class with a given class label.  If the class label does not exist then the string "CLASS_LABEL_NOT_FOUND" will be returned.
     
	 @return a string containing the name of the given class label or the string "CLASS_LABEL_NOT_FOUND" if the class label does not exist
     */
    string getClassNameForCorrespondingClassLabel(const UINT classLabel);
    
	/**
     Gets the ranges of the classification data.
     
	 @return a vector of minimum and maximum values for each dimension of the data
     */
	vector<MinMax> getRanges() const;
    
	/**
     Gets the class tracker for each class in the dataset.
     
	 @return a vector of ClassTracker, one for each class in the dataset
     */
    vector< ClassTracker > getClassTracker() const { return classTracker; }
    
    /**
     Gets the timeseries position tracker, a vector of TimeSeriesPositionTracker which indicate the start and end position of each time series in the dataset.
     
	 @return a vector of TimeSeriesPositionTracker, one for each timeseries in the dataset
     */
    vector< TimeSeriesPositionTracker > getTimeSeriesPositionTracker() const { return timeSeriesPositionTracker; }
    
	/**
     Gets the classification data as a vector of ClassificationSample.
     
	 @return a vector of ClassificationSamples
     */
	vector< ClassificationSample > getClassificationSamples() const { return data; }
    
    /**
     Gets a new TimeSeriesClassificationDataStream dataset drawn from the startIndex and endIndex values.
     The startIndex and endIndex values must be valid (i.e. if the current dataset has 1000 samples then you can not have a startIndex
     or endIndex value that is greater or equal to 1000).
     
     @param UINT startIndex: the index of the first value from the current dataset that you want to start the new subset from
     @param UINT endIndex: the index of the last value from the current dataset that you want to end the new dataset at (inclusive)
     @return returns a new TimeSeriesClassificationDataStream subset of the current dataset drawn from the startIndex and endIndex values
     */
    TimeSeriesClassificationDataStream getSubset(const UINT startIndex,const UINT endIndex) const;
    
    /**
     This function segments the continuous time series data into individual time series and then adds these time series into a new
     TimeSeriesClassificationData dataset.  This new dataset is then returned. The new dataset will not contain any time series
     with the class label of GRT_DEFAULT_NULL_CLASS_LABEL (as this is assumed to be a NULL gesture).
     
     @return returns a new TimeSeriesClassificationData built from the time series data in this dataset.
     */
    TimeSeriesClassificationData getTimeSeriesClassificationData() const;
    
    /**
     This function segments the continuous time series data into individual samples and then adds these samples into a new
     ClassificationData dataset.  This new dataset is then returned. The new dataset will not contain any samples
     with the class label of GRT_DEFAULT_NULL_CLASS_LABEL (as this is assumed to be a NULL gesture).
     
     @return returns a new ClassificationData built from the samples in this dataset.
     */
    ClassificationData getClassificationData() const;
    
    /**
     This function segments a specific time series from the main data set and returns this as a MatrixDouble.
     You should use one of the TimeSeriesPositionTracker elements from the timeSeriesPositionTracker vector to 
     indicate to the function which time series you want to retrieve.  You can get the TimeSeriesPositionTracker elements
     using the #getTimeSeriesPositionTracker() function.
     
     The MatrixDouble will be empty if the trackerInfo indexs (the startIndex and the endIndex) are not valid.
     
     @return returns a new LabelledClassificationData built from the samples in this dataset.
     */
    MatrixDouble getTimeSeriesData( const TimeSeriesPositionTracker &trackerInfo ) const;
    
    /**
     Gets all the data as a MatrixDouble. This returns just the data, not the labels.
     This will be an M by N MatrixDouble, where M is the number of samples and N is the number of dimensions.
     
     @return a MatrixDouble containing the data from the current dataset.
     */
    MatrixDouble getDataAsMatrixDouble() const;
    
    /**
     Gets the class labels in the current dataset.
     
     @return a vector containing the class labels from the current dataset.
     */
    vector< UINT > getClassLabels() const;

protected:
    string datasetName;                                     ///< The name of the dataset
    string infoText;                                        ///< Some infoText about the dataset
	UINT numDimensions;										///< The number of dimensions in the dataset
	UINT totalNumSamples;
	UINT lastClassID;
    UINT playbackIndex;
	bool trackingClass;
    bool useExternalRanges;                                 ///< A flag to show if the dataset should be scaled using the externalRanges values
    vector< MinMax > externalRanges;                        ///< A vector containing a set of externalRanges set by the user
	vector< ClassTracker > classTracker;
	vector< ClassificationSample > data;
	vector< TimeSeriesPositionTracker > timeSeriesPositionTracker;
    
    DebugLog debugLog;                                      ///< Default debugging log
    ErrorLog errorLog;                                      ///< Default error log
    WarningLog warningLog;                                  ///< Default warning log
};

} //End of namespace GRT

#endif //GRT_TIME_SERIES_CLASSIFICATION_DATA_STREAM_HEADER

