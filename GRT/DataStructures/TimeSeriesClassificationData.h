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

#include "../Util/GRTCommon.h"
#include "TimeSeriesClassificationSample.h"
#include "UnlabelledData.h"

namespace GRT{

class TimeSeriesClassificationData{
public:
    
    /**
     Constructor, sets the name of the dataset and the number of dimensions of the training data.
     The name of the dataset should not contain any spaces.
	 
     @param UINT numDimensions: the number of dimensions of the training data, should be an unsigned integer greater than 0
     @param string datasetName: the name of the dataset, should not contain any spaces
     @param string infoText: some info about the data in this dataset, this can contain spaces
     */
	TimeSeriesClassificationData(UINT numDimensions = 0,string datasetName = "NOT_SET",string infoText = "");
    
    /**
     Copy Constructor, copies the TimeSeriesClassificationData from the rhs instance to this instance
     
	 @param const TimeSeriesClassificationData &rhs: another instance of the TimeSeriesClassificationData class from which the data will be copied to this instance
     */
	TimeSeriesClassificationData(const TimeSeriesClassificationData &rhs);
    
    /**
     Default Destructor
     */
	~TimeSeriesClassificationData();

    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const TimeSeriesClassificationData &rhs: another instance of the TimeSeriesClassificationData class from which the data will be copied to this instance
	 @return a reference to this instance of TimeSeriesClassificationData
     */
	TimeSeriesClassificationData& operator= (const TimeSeriesClassificationData &rhs);

    /**
     Array Subscript Operator, returns the TimeSeriesClassificationSample at index i.
	 It is up to the user to ensure that i is within the range of [0 totalNumSamples-1]
     
	 @param const UINT &i: the index of the training sample you want to access.  Must be within the range of [0 totalNumSamples-1]
     @return a reference to the i'th TimeSeriesClassificationSample
     */
	inline TimeSeriesClassificationSample& operator[] (const UINT &i){
		return data[i];
	}
    
    /**
     Const Array Subscript Operator, returns the TimeSeriesClassificationSample at index i.
	 It is up to the user to ensure that i is within the range of [0 totalNumSamples-1]
     
	 @param const UINT &i: the index of the training sample you want to access.  Must be within the range of [0 totalNumSamples-1]
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
     
	 @param const UINT numDimensions: the number of dimensions of the training data.  Must be an unsigned integer greater than zero
     @return true if the number of dimensions was correctly updated, false otherwise
     */
    bool setNumDimensions(const UINT numDimensions);
    
    /**
     Sets the name of the dataset.
     There should not be any spaces in the name.
     Will return true if the name is set, or false otherwise.
     
     @param const string datasetName: the new name of the dataset
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
     
     @param const className: the new name for the class
     @param const UINT classLabel: the label ID that you want to set the class name for
	 @return returns true if the name is set, or false if the class label does not exist
     */
    bool setClassNameForCorrespondingClassLabel(const string className,const UINT classLabel);
    
    /**
     Sets if the user can add samples to the dataset with the label matching the GRT_DEFAULT_NULL_CLASS_LABEL.
     If the allowNullGestureClass is set to true, then the user can add labels matching the default null class label (which is normally 0).
     If the allowNullGestureClass is set to false, then the user will not be able to add samples that have a class
     label matching the default null class label.
     
     @param const bool allowNullGestureClass: true if you want to use the default null gesture as a label
     @return returns true if the allowNullGestureClass was set, false otherwise
     */
    bool setAllowNullGestureClass(const bool allowNullGestureClass);
    
    /**
     Adds a new labelled timeseries sample to the dataset.  
     The dimensionality of the sample should match the number of dimensions in the dataset.
     The class label should be greater than zero (as zero is used as the default null rejection class label).
     
	 @param const UINT classLabel: the class label of the corresponding sample
     @param const MatrixDouble trainingSample: the new sample you want to add to the dataset.  The dimensionality of this sample (i.e. Matrix columns) should match the number of dimensions in the dataset, the rows of the Matrix represent time and do not have to be any specific length
	 @return true if the sample was correctly added to the dataset, false otherwise
     */
	bool addSample(const UINT classLabel,const MatrixDouble &trainingSample);
    
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
     
     @param const bool useExternalRanges: sets if these ranges should be used to scale the dataset
	 @return returns true if the useExternalRanges variable was set, false otherwise
     */
    bool enableExternalRangeScaling(const bool useExternalRanges);
    
    /**
     Scales the dataset to the new target range.
     
     @param const double minTarget: the minimum range you want to scale the data to
     @param const double maxTarget: the maximum range you want to scale the data to
	 @return true if the data was scaled correctly, false otherwise
     */
    bool scale(const double minTarget,const double maxTarget);
    
	/**
     Scales the dataset to the new target range, using the vector of ranges as the min and max source ranges.
     
     @param const vector<MinMax> &ranges: a vector of source ranges, should have the same dimensions as your data
     @param const double minTarget: the minimum range you want to scale the data to
     @param const double maxTarget: the maximum range you want to scale the data to
	 @return true if the data was scaled correctly, false otherwise
     */
	bool scale(const vector<MinMax> &ranges,const double minTarget,const double maxTarget);
	
	/**
     Saves the labelled timeseries classification data to a custom file format.
     
	 @param const string filename: the name of the file the data will be saved to
	 @return true if the data was saved successfully, false otherwise
     */
	bool saveDatasetToFile(const string filename) const;
	
	/**
     Loads the labelled timeseries classification data from a custom file format.
     
	 @param const string filename: the name of the file the data will be loaded from
	 @return true if the data was loaded successfully, false otherwise
     */
	bool loadDatasetFromFile(const string filename);
    
    /**
     Prints the dataset info (such as its name and infoText) and the stats (such as the number of examples, number of dimensions, number of classes, etc.)
     to the std out.
     
     @return returns true if the dataset info and stats were printed successfully, false otherwise
     */
    bool printStats() const;
    
    /**
     Partitions the dataset into a training dataset (which is kept by this instance of the TimeSeriesClassificationData) and
	 a testing/validation dataset (which is returned as a new instance of a TimeSeriesClassificationData).
     
	 @param const UINT partitionPercentage: sets the percentage of data which remains in this instance, the remaining percentage of data is then returned as the testing/validation dataset
     @param const bool useStratifiedSampling: sets if the dataset should be broken into homogeneous groups first before randomly being spilt, default value is false
	 @return a new TimeSeriesClassificationData instance, containing the remaining data not kept but this instance
     */
	TimeSeriesClassificationData partition(const UINT partitionPercentage,const bool useStratifiedSampling = false);
    
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
    string getClassNameForCorrespondingClassLabel(const UINT classLabel) const;
    
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
     Gets the classification data.
     
	 @return a vector of TimeSeriesClassificationSample
     */
	vector< TimeSeriesClassificationSample > getClassificationData() const { return data; }
    
    /**
     Gets the data as a MatrixDouble. This returns just the data, not the labels.
     This will be an M by N MatrixDouble, where M is the number of samples and N is the number of dimensions.
     
     @return a MatrixDouble containing the data from the current dataset.
     */
    MatrixDouble getDataAsMatrixDouble() const;
    
protected:
    
    string datasetName;                                     ///< The name of the dataset
    string infoText;                                        ///< Some infoText about the dataset
	UINT numDimensions;										///< The number of dimensions in the dataset
	UINT totalNumSamples;                                   ///< The total number of samples in the dataset
    UINT kFoldValue;                                        ///< The number of folds the dataset has been spilt into for cross valiation
    bool crossValidationSetup;                              ///< A flag to show if the dataset is ready for cross validation
    bool useExternalRanges;                                 ///< A flag to show if the dataset should be scaled using the externalRanges values
    bool allowNullGestureClass;                             ///< A flag that enables/disables a user from adding new samples with a class label matching the default null gesture label
    vector< MinMax > externalRanges;                        ///< A vector containing a set of externalRanges set by the user
	vector< ClassTracker > classTracker;					///< A vector of ClassTracker, which keeps track of the number of samples of each class
	vector< TimeSeriesClassificationSample > data;          ///< The labelled time series classification data
    vector< vector< UINT > >    crossValidationIndexs;      ///< A vector to hold the indexs of the dataset for the cross validation
    
    DebugLog debugLog;                                      ///< Default debugging log
    ErrorLog errorLog;                                      ///< Default error log
    WarningLog warningLog;                                  ///< Default warning log
    
};

} //End of namespace GRT

#endif //GRT_LABELLED_TIME_SERIES_CLASSIFICATION_DATA_HEADER

