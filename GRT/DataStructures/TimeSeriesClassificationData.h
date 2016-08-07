/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief The TimeSeriesClassificationData is the main data structure for recording, labeling, managing, saving, and loading training data for supervised temporal learning problems. Unlike the ClassificationData, in which each sample consists of 1 N dimensional datum, a TimeSeriesClassificationData sample will consist of an N dimensional time series of length M. The length of each time series sample (i.e. M) can be different for each datum in the dataset.
 
 @example DatastructuresExample/TimeSeriesClassificationDataExample/TimeSeriesClassificationDataExample.cpp
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

#ifndef GRT_TIME_SERIES_CLASSIFICATION_DATA_HEADER
#define GRT_TIME_SERIES_CLASSIFICATION_DATA_HEADER

#include "VectorFloat.h"
#include "MatrixFloat.h"
#include "../Util/GRTCommon.h"
#include "TimeSeriesClassificationSample.h"
#include "UnlabelledData.h"

GRT_BEGIN_NAMESPACE

class GRT_API TimeSeriesClassificationData{
public:
    
    /**
     Constructor, sets the name of the dataset and the number of dimensions of the training data.
     The name of the dataset should not contain any spaces.
	 
     @param numDimensions: the number of dimensions of the training data, should be an unsigned integer greater than 0
     @param datasetName: the name of the dataset, should not contain any spaces
     @param infoText: some info about the data in this dataset, this can contain spaces
     */
	TimeSeriesClassificationData(UINT numDimensions = 0,std::string datasetName = "NOT_SET",std::string infoText = "");
    
    /**
     Copy Constructor, copies the TimeSeriesClassificationData from the rhs instance to this instance
     
	 @param rhs: another instance of the TimeSeriesClassificationData class from which the data will be copied to this instance
     */
	TimeSeriesClassificationData(const TimeSeriesClassificationData &rhs);
    
    /**
     Default Destructor
     */
	virtual ~TimeSeriesClassificationData();

    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param rhs: another instance of the TimeSeriesClassificationData class from which the data will be copied to this instance
	 @return a reference to this instance of TimeSeriesClassificationData
     */
	TimeSeriesClassificationData& operator= (const TimeSeriesClassificationData &rhs);

    /**
     Array Subscript Operator, returns the TimeSeriesClassificationSample at index i.
	 It is up to the user to ensure that i is within the range of [0 totalNumSamples-1]
     
	 @param i: the index of the training sample you want to access.  Must be within the range of [0 totalNumSamples-1]
     @return a reference to the i'th TimeSeriesClassificationSample
     */
	inline TimeSeriesClassificationSample& operator[] (const UINT &i){
		return data[i];
	}
    
    /**
     Const Array Subscript Operator, returns the TimeSeriesClassificationSample at index i.
	 It is up to the user to ensure that i is within the range of [0 totalNumSamples-1]
     
	 @param i: the index of the training sample you want to access.  Must be within the range of [0 totalNumSamples-1]
     @return a reference to the i'th TimeSeriesClassificationSample
     */
	inline const TimeSeriesClassificationSample& operator[] (const UINT &i) const{
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
     
	 @param numDimensions: the number of dimensions of the training data.  Must be an unsigned integer greater than zero
     @return true if the number of dimensions was correctly updated, false otherwise
     */
    bool setNumDimensions(const UINT numDimensions);
    
    /**
     Sets the name of the dataset.
     There should not be any spaces in the name.
     Will return true if the name is set, or false otherwise.
     
     @param datasetName: the new name of the dataset
	 @return returns true if the name is set, or false otherwise
     */
    bool setDatasetName(const std::string datasetName);
    
    /**
     Sets the info std::string.
	 This can be any std::string with information about how the training data was recorded for example.
     
	 @param infoText: the infoText
     @return true if the infoText was correctly updated, false otherwise
     */
    bool setInfoText(const std::string infoText);
    
    /**
     Sets the name of the class with the given class label.  
     There should not be any spaces in the className.
     Will return true if the name is set, or false if the class label does not exist.
     
     @param className: the new name for the class
     @param classLabel: the label ID that you want to set the class name for
	 @return returns true if the name is set, or false if the class label does not exist
     */
    bool setClassNameForCorrespondingClassLabel(const std::string className,const UINT classLabel);
    
    /**
     Sets if the user can add samples to the dataset with the label matching the GRT_DEFAULT_NULL_CLASS_LABEL.
     If the allowNullGestureClass is set to true, then the user can add labels matching the default null class label (which is normally 0).
     If the allowNullGestureClass is set to false, then the user will not be able to add samples that have a class
     label matching the default null class label.
     
     @param allowNullGestureClass: true if you want to use the default null gesture as a label
     @return returns true if the allowNullGestureClass was set, false otherwise
     */
    bool setAllowNullGestureClass(const bool allowNullGestureClass);
    
    /**
     Adds a new labelled timeseries sample to the dataset.  
     The dimensionality of the sample should match the number of dimensions in the dataset.
     The class label should be greater than zero (as zero is used as the default null rejection class label).
     
	 @param classLabel: the class label of the corresponding sample
     @param trainingSample: the new sample you want to add to the dataset.  The dimensionality of this sample (i.e. Matrix columns) should match the number of dimensions in the dataset, the rows of the Matrix represent time and do not have to be any specific length
	 @return true if the sample was correctly added to the dataset, false otherwise
     */
	bool addSample(const UINT classLabel,const MatrixFloat &trainingSample);
    
    /**
     Removes the last training sample added to the dataset.
     
	 @return true if the last sample was removed, false otherwise
     */
    bool removeLastSample();
    
    /**
     Deletes from the dataset all the samples with a specific class label.
     
	 @param classLabel: the class label of the samples you wish to delete from the dataset
	 @return the number of samples deleted from the dataset
     */
	UINT eraseAllSamplesWithClassLabel(const UINT classLabel);
    
    /**
     Relabels all the samples with the class label A with the new class label B.
     
	 @param oldClassLabel: the class label of the samples you want to relabel
     @param newClassLabel: the class label the samples will be relabelled with
	 @return returns true if the samples were correctly relablled, false otherwise
     */
	bool relabelAllSamplesWithClassLabel(const UINT oldClassLabel,const UINT newClassLabel);
    
    /**
     Sets the external ranges of the dataset, also sets if the dataset should be scaled using these values.  
     The dimensionality of the externalRanges vector should match the number of dimensions of this dataset.
     
	 @param externalRanges: an N dimensional vector containing the min and max values of the expected ranges of the dataset.
     @param useExternalRanges: sets if these ranges should be used to scale the dataset, default value is false.
	 @return returns true if the external ranges were set, false otherwise
     */
    bool setExternalRanges(const Vector< MinMax > &externalRanges,const bool useExternalRanges = false);
    
    /**
     Sets if the dataset should be scaled using an external range (if useExternalRanges == true) or the ranges of the dataset (if false).
     The external ranges need to be set FIRST before calling this function, otherwise it will return false.
     
     @param useExternalRanges: sets if these ranges should be used to scale the dataset
	 @return returns true if the useExternalRanges variable was set, false otherwise
     */
    bool enableExternalRangeScaling(const bool useExternalRanges);
    
    /**
     Scales the dataset to the new target range.
     
     @param minTarget: the minimum range you want to scale the data to
     @param maxTarget: the maximum range you want to scale the data to
	 @return true if the data was scaled correctly, false otherwise
     */
    bool scale(const Float minTarget,const Float maxTarget);
    
	/**
     Scales the dataset to the new target range, using the vector of ranges as the min and max source ranges.
     
     @param ranges: a vector of source ranges, should have the same dimensions as your data
     @param minTarget: the minimum range you want to scale the data to
     @param maxTarget: the maximum range you want to scale the data to
	 @return true if the data was scaled correctly, false otherwise
     */
	bool scale(const Vector<MinMax> &ranges,const Float minTarget,const Float maxTarget);
    
    /**
     Saves the data to a file.
     If the file format ends in '.csv' then the data will be saved as comma-seperated-values, otherwise it will be saved
     to a custom GRT file (which contains the csv data with an additional header).
     
     @param filename: the name of the file the data will be saved to
     @return true if the data was saved successfully, false otherwise
     */
    bool save(const std::string &filename) const;
    
    /**
     Load the data from a file.
     If the file format ends in '.csv' then the function will try and load the data from a csv format.  If this fails then it will
     try and load the data as a custom GRT file.
     
     @param filename: the name of the file the data will be loaded from
     @return true if the data was loaded successfully, false otherwise
     */
    bool load(const std::string &filename);
	
	/**
     Saves the labelled timeseries classification data to a custom file format.
     
	 @param filename: the name of the file the data will be saved to
	 @return true if the data was saved successfully, false otherwise
     */
	bool saveDatasetToFile(const std::string filename) const;
	
	/**
     Loads the labelled timeseries classification data from a custom file format.
     
	 @param filename: the name of the file the data will be loaded from
	 @return true if the data was loaded successfully, false otherwise
     */
	bool loadDatasetFromFile(const std::string filename);
    
    /**
     Saves the data to a CSV file.
     This will save the timeseries counter as the first column, the class label as the second column, and the sample data as the following N columns, where N is the number of dimensions in the data.  Each row will represent a sample.
     
     @param filename: the name of the file the data will be saved to
     @return true if the data was saved successfully, false otherwise
     */
    bool saveDatasetToCSVFile(const std::string &filename) const;
	
	/**
     Loads the classification data from a CSV file.
     This assumes the data is formatted with each row representing a sample.
     The first column should represent the timeseries counter.
     The class label should be the second column followed by the sample data as the following N columns, where N is the number of dimensions in the data.
     
     @param filename: the name of the file the data will be loaded from
     @return true if the data was loaded successfully, false otherwise
     */
	bool loadDatasetFromCSVFile(const std::string &filename);
    
    /**
     Prints the dataset info (such as its name and infoText) and the stats (such as the number of examples, number of dimensions, number of classes, etc.)
     to the std out.
     
     @return returns true if the dataset info and stats were printed successfully, false otherwise
     */
    bool printStats() const;
    
    /**
     Gets the dataset info (such as its name and infoText) and the stats (such as the number of examples, number of dimensions, number of classes, etc.)
     as a std::string.
     
     @return returns a std::string containing the dataset stats
     */
    std::string getStatsAsString() const;
    
    /**
     @deprecated use split(...) instead
	 @param partitionPercentage: sets the percentage of data which remains in this instance, the remaining percentage of data is then returned as the testing/validation dataset
     @param useStratifiedSampling: sets if the dataset should be broken into homogeneous groups first before randomly being spilt, default value is false
	 @return a new TimeSeriesClassificationData instance, containing the remaining data not kept but this instance
     */
	GRT_DEPRECATED_MSG( "partition(...) is deprecated, use split(...) instead", TimeSeriesClassificationData partition(const UINT partitionPercentage,const bool useStratifiedSampling = false) );

    /**
     Partitions the dataset into a training dataset (which is kept by this instance of the TimeSeriesClassificationData) and
     a testing/validation dataset (which is returned as a new instance of a TimeSeriesClassificationData).
     
     @param partitionPercentage: sets the percentage of data which remains in this instance, the remaining percentage of data is then returned as the testing/validation dataset
     @param useStratifiedSampling: sets if the dataset should be broken into homogeneous groups first before randomly being spilt, default value is false
     @return a new TimeSeriesClassificationData instance, containing the remaining data not kept but this instance
     */
    TimeSeriesClassificationData split(const UINT partitionPercentage,const bool useStratifiedSampling = false);
    
    /**
     Adds the data in the labelledData set to the current instance of the TimeSeriesClassificationData.
     The number of dimensions in both datasets must match.
     The names of the classes from the labelledData will be added to the current instance.
     
	 @param const TimeSeriesClassificationData &labelledData: the dataset to add to this dataset
	 @return returns true if the datasets were merged, false otherwise
     */
    bool merge(const TimeSeriesClassificationData &labelledData);
    
    /**
     This function prepares the dataset for k-fold cross validation and should be called prior to calling the getTrainingFold(UINT foldIndex) or getTestingFold(UINT foldIndex) functions.  It will spilt the dataset into K-folds, as long as K < M, where M is the number of samples in the dataset.
     
	 @param const UINT K: the number of folds the dataset will be split into, K should be less than the number of samples in the dataset
     @param const bool useStratifiedSampling: sets if the dataset should be broken into homogeneous groups first before randomly being spilt, default value is false
	 @return returns true if the dataset was split correctly, false otherwise
     */
    bool spiltDataIntoKFolds(const UINT K, const bool useStratifiedSampling = false);
    
    /**
     Returns the training dataset for the k-th fold for cross validation.  The spiltDataIntoKFolds(UINT K) function should have been called once before using this function.
     The foldIndex should be in the range [0 K-1], where K is the number of folds the data was spilt into.
     
	 @param const UINT foldIndex: the index of the fold you want the training data for, this should be in the range [0 K-1], where K is the number of folds the data was spilt into 
	 @return returns a training dataset
     */
    TimeSeriesClassificationData getTrainingFoldData(const UINT foldIndex) const;
    
    /**
     Returns the test dataset for the k-th fold for cross validation.  The spiltDataIntoKFolds(UINT K) function should have been called once before using this function.
     The foldIndex should be in the range [0 K-1], where K is the number of folds the data was spilt into.
     
	 @param const UINT foldIndex: the index of the fold you want the test data for, this should be in the range [0 K-1], where K is the number of folds the data was spilt into 
	 @return returns a test dataset
     */
    TimeSeriesClassificationData getTestFoldData(const UINT foldIndex) const;
    
    /**
     Returns the all the data with the class label set by classLabel.
     The classLabel should be a valid classLabel, otherwise the dataset returned will be empty.
     
	 @param const UINT classLabel: the class label of the class you want the data for
	 @return returns a dataset containing all the data with the matching classLabel
     */
    TimeSeriesClassificationData getClassData(const UINT classLabel) const;
    
    /**
     Reformats the TimeSeriesClassificationData as UnlabeledData so the data can be used to train unsupervised training algorithms such as K-Means Clustering and Gaussian Mixture Models.
     
	 @return a new UnlabelledData instance, containing the reformated timeseries classification data
     */
    UnlabelledData reformatAsUnlabelledData() const;
    
    /**
     Gets the name of the dataset.
     
	 @return returns the name of the dataset
     */
    std::string getDatasetName() const { return datasetName; }
    
    /**
     Gets the infotext for the dataset
     
	 @return returns the infotext of the dataset
     */
    std::string getInfoText() const { return infoText; }
    
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
	UINT inline getNumClasses() const { return classTracker.getSize(); }
    
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
     Gets the name of the class with a given class label.  If the class label does not exist then the std::string "CLASS_LABEL_NOT_FOUND" will be returned.
     
	 @return a std::string containing the name of the given class label or the std::string "CLASS_LABEL_NOT_FOUND" if the class label does not exist
     */
    std::string getClassNameForCorrespondingClassLabel(const UINT classLabel) const;
    
    /**
     Gets the ranges of the classification data.
     
	 @return a vector of minimum and maximum values for each dimension of the data
     */
	Vector<MinMax> getRanges() const;
    
	/**
     Gets the class tracker for each class in the dataset.
     
	 @return a vector of ClassTracker, one for each class in the dataset
     */
    Vector< ClassTracker > getClassTracker() const { return classTracker; }
    
	/**
     Gets the classification data.
     
	 @return a vector of TimeSeriesClassificationSample
     */
	Vector< TimeSeriesClassificationSample > getClassificationData() const { return data; }
    
    /**
     Gets the data as a MatrixFloat. This returns just the data, not the labels.
     This will be an M by N MatrixFloat, where M is the number of samples and N is the number of dimensions.
     
     @return a MatrixFloat containing the data from the current dataset.
     */
    MatrixFloat getDataAsMatrixFloat() const;
    
protected:
    
    std::string datasetName;                                     ///< The name of the dataset
    std::string infoText;                                        ///< Some infoText about the dataset
	UINT numDimensions;										///< The number of dimensions in the dataset
	UINT totalNumSamples;                                   ///< The total number of samples in the dataset
    UINT kFoldValue;                                        ///< The number of folds the dataset has been spilt into for cross valiation
    bool crossValidationSetup;                              ///< A flag to show if the dataset is ready for cross validation
    bool useExternalRanges;                                 ///< A flag to show if the dataset should be scaled using the externalRanges values
    bool allowNullGestureClass;                             ///< A flag that enables/disables a user from adding new samples with a class label matching the default null gesture label
    Vector< MinMax > externalRanges;                        ///< A vector containing a set of externalRanges set by the user
	Vector< ClassTracker > classTracker;					///< A vector of ClassTracker, which keeps track of the number of samples of each class
	Vector< TimeSeriesClassificationSample > data;          ///< The labelled time series classification data
    Vector< Vector< UINT > >  crossValidationIndexs;        ///< A vector to hold the indexs of the dataset for the cross validation
     
    DebugLog debugLog;                                      ///< Default debugging log
    ErrorLog errorLog;                                      ///< Default error log
    WarningLog warningLog;                                  ///< Default warning log
    
};

GRT_END_NAMESPACE

#endif //GRT_LABELLED_TIME_SERIES_CLASSIFICATION_DATA_HEADER

