/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief The RegressionData is the main data structure for recording, labeling, managing, 
 saving, and loading datasets that can be used to train and test the GRT supervised regression algorithms.
 
 @example DatastructuresExample/RegressionDataExample/RegressionDataExample.cpp
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

#ifndef GRT_REGRESSION_DATA_HEADER
#define GRT_REGRESSION_DATA_HEADER

#include "../Util/GRTTypedefs.h"
 #include "VectorFloat.h"
#include "RegressionSample.h"

GRT_BEGIN_NAMESPACE

class GRT_API RegressionData{
public:
    
    /**
     Constructor, set the number of input dimensions, number of target dimensions, dataset name and the infotext for the dataset.
     The name of the dataset should not contain any spaces.
	 
     @param numInputDimensions: the number of input dimensions of the training data, should be an unsigned integer greater than 0
     @param numTargetDimensions: the number of target dimensions of the training data, should be an unsigned integer greater than 0
     @param datasetName: the name of the dataset, should not contain any spaces
     @param infoText: some info about the data in this dataset, this can contain spaces
     */
    RegressionData(const UINT numInputDimensions=0,const UINT numTargetDimensions=0,const std::string datasetName="NOT_SET",const std::string infoText="");
    
    /**
     Copy Constructor, copies the RegressionData from the rhs instance to this instance
     
	 @param rhs: another instance of the RegressionData class from which the data will be copied to this instance
     */
	RegressionData(const RegressionData &rhs);
    
    /**
     Default Destructor
     */
	~RegressionData();

    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param rhs: another instance of the RegressionData class from which the data will be copied to this instance
	 @return a reference to this instance of RegressionData
     */
	RegressionData& operator=(const RegressionData &rhs);
	
    /**
     Array Subscript Operator, returns the LabelledRegressionSample at index i.  
	 It is up to the user to ensure that i is within the range of [0 totalNumSamples-1]
     
	 @param i: the index of the training sample you want to access.  Must be within the range of [0 totalNumSamples-1]
     @return a reference to the i'th RegressionSample
     */
	inline RegressionSample& operator[](const UINT &i){
		return data[i];
	}

    /**
     Const Array Subscript Operator, returns the LabelledRegressionSample at index i.
     It is up to the user to ensure that i is within the range of [0 totalNumSamples-1]

     @param i: the index of the training sample you want to access.  Must be within the range of [0 totalNumSamples-1]
     @return a reference to the i'th RegressionSample
     */
    const inline RegressionSample& operator[](const UINT &i) const{
        return data[i];
    }
    
    /**
     Clears any previous training data and counters
     */
	void clear();

    /**
     Sets the number of input and target dimensions in the training data. 
	 These should be unsigned integers greater than zero.  
	 This will clear any previous training data and counters.
     This function needs to be called before any new samples can be added to the dataset, unless the numInputDimensions 
     and numTargetDimensions variables was set in the constructor or some data was already loaded from a file
     
     @param numInputDimensions: the number of input dimensions of the training data.  Must be an unsigned integer greater than zero
     @param numTargetDimensions: the number of target dimensions of the training data.  Must be an unsigned integer greater than zero
     @return true if the number of input and target dimensions was correctly updated, false otherwise
     */
    bool setInputAndTargetDimensions(const UINT numInputDimensions,const UINT numTargetDimensions);
    
    /**
     Sets the name of the dataset.
     There should not be any spaces in the name.
     Will return true if the name is set, or false otherwise.
     
     @param datasetName: the new dataset name (must not include any spaces)
	 @return returns true if the name is set, or false otherwise
     */
    bool setDatasetName(const std::string &datasetName);
    
    /**
     Sets the info string.
	 This can be any string with information about how the training data was recorded for example.
     
     @param infoText: the infoText
     @return true if the infoText was correctly updated, false otherwise
     */
    bool setInfoText(const std::string &infoText);
    
    /**
     Adds a new labelled sample to the dataset.  
     The input and target dimensionality of the sample should match that of the dataset.
     
     @param inputVector: the new input Vector you want to add to the dataset.  The dimensionality of this sample should match the number of input dimensions in the dataset
     @param targetVector: the new target Vector you want to add to the dataset.  The dimensionality of this sample should match the number of target dimensions in the dataset
	 @return true if the sample was correctly added to the dataset, false otherwise
     */
	bool addSample(const VectorFloat &inputVector,const VectorFloat &targetVector);
    
    /**
     Removes the last training sample added to the dataset.
     
	 @return true if the last sample was removed, false otherwise
     */
	bool removeLastSample();
    
    /**
     Reserves that the Vector capacity be at least enough to contain N elements.
     
     If N is greater than the current Vector capacity, the function causes the container to reallocate its storage increasing its capacity to N (or greater).
     
	 @param N: the new memory size
	 @return true if the memory was reserved successfully, false otherwise
     */
    bool reserve(const UINT N);
    
    /**
     Sets the external input and target ranges of the dataset, also sets if the dataset should be scaled using these values.  
     The dimensionality of the externalRanges Vector should match the numInputDimensions and numTargetDimensions of this dataset.
     
     @param externalInputRanges: an N dimensional Vector containing the min and max values of the expected input ranges of the dataset
     @param externalTargetRanges: an N dimensional Vector containing the min and max values of the expected target ranges of the dataset
     @param useExternalRanges: sets if these ranges should be used to scale the dataset, default value is false
	 @return returns true if the external ranges were set, false otherwise
     */
    bool setExternalRanges(const Vector< MinMax > &externalInputRanges, const Vector< MinMax > &externalTargetRanges, const bool useExternalRanges);
    
    /**
     Sets if the dataset should be scaled using an external range (if useExternalRanges == true) or the ranges of the dataset (if false).
     The external ranges need to be set FIRST before calling this function, otherwise it will return false.
     
     @param useExternalRanges: sets if these ranges should be used to scale the dataset
	 @return returns true if the useExternalRanges variable was set, false otherwise
     */
    bool enableExternalRangeScaling(const bool useExternalRanges);
    
	/**
     Scales the dataset to the new target range.
     
     @param minTarget: the minimum target the dataset will be scaled to
     @param maxTarget: the maximum target the dataset will be scaled to
	 @return true if the data was scaled correctly, false otherwise
     */
    bool scale(const Float minTarget,const Float maxTarget);
    
	/**
     Scales the dataset to the new target range, using the Vector of ranges as the min and max source ranges.
     
	 @return true if the data was scaled correctly, false otherwise
     */
    bool scale(const Vector< MinMax > &inputVectorRanges,const Vector< MinMax > &targetVectorRanges,const Float minTarget,const Float maxTarget);
    
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
     Saves the labelled regression data to a custom file format.
     
     @param filename: the name of the file the data will be saved to
	 @return true if the data was saved successfully, false otherwise
     */
    bool saveDatasetToFile(const std::string &filename) const;
	
	/**
     Loads the labelled regression data from a custom file format.
     
     @param filename: the name of the file the data will be loaded from
	 @return true if the data was loaded successfully, false otherwise
     */
    bool loadDatasetFromFile(const std::string &filename);
    
    /**
     Saves the labelled regression data to a CSV file.
     This will save the input Vector as the first N columns and the target data as the following T columns.  Each row will represent a sample.
     
     @param filename: the name of the file the data will be saved to
	 @return true if the data was saved successfully, false otherwise
     */
    bool saveDatasetToCSVFile(const std::string &filename) const;
	
    /**
     Loads the labelled regression data from a CSV file.
     Each row represents a sample, the first N columns should represent the input Vector data with the remaining T columns representing the target sample.
     The user must specify the length of the input Vector (N) and the length of the target Vector (T).
     
     @param filename: the name of the file the data will be saved to
     @param umInputDimensions: the length of an input Vector
     @param numTargetDimensions: the length of a target Vector
	 @return true if the data was saved successfully, false otherwise
     */
    bool loadDatasetFromCSVFile(const std::string &filename,const UINT numInputDimensions,const UINT numTargetDimensions);
    
    bool printStats() const;
    
    /**
     Adds the data in the regressionData set to the current instance of the RegressionData.
     The number of dimensions in both datasets must match.
     
     @param regressionData: the dataset to add to this dataset
	 @return returns true if the datasets were merged, false otherwise
     */
    bool merge(const RegressionData &regressionData);
    
    /**
     @deprecated use split(...) instead
     @param partitionPercentage: sets the percentage of data which remains in this instance, the remaining percentage of data is then returned as the testing/validation dataset
	 @return a new RegressionData instance, containing the remaining data not kept but this instance
     */
    GRT_DEPRECATED_MSG( "partition(...) is deprecated, use split(...) instead", RegressionData partition(const UINT trainingSizePercentage) );

    /**
     Splits the dataset into a training dataset (which is kept by this instance of the RegressionData) and
     a testing/validation dataset (which is returned as a new instance of a RegressionData).
     
     @param partitionPercentage: sets the percentage of data which remains in this instance, the remaining percentage of data is then returned as the testing/validation dataset
     @return a new RegressionData instance, containing the remaining data not kept but this instance
     */
    RegressionData split(const UINT trainingSizePercentage);
    
    /**
     This function prepares the dataset for k-fold cross validation and should be called prior to calling the getTrainingFold(UINT foldIndex) or getTestingFold(UINT foldIndex) functions.  It will spilt the dataset into K-folds, as long as K < M, where M is the number of samples in the dataset.
     
     @param K: the number of folds the dataset will be split into, K should be less than the number of samples in the dataset
	 @return returns true if the dataset was split correctly, false otherwise
     */
    bool spiltDataIntoKFolds(const UINT K);
    
    /**
     Returns the training dataset for the k-th fold for cross validation.  The spiltDataIntoKFolds(UINT K) function should have been called once before using this function.
     The foldIndex should be in the range [0 K-1], where K is the number of folds the data was spilt into.
     
	 @param foldIndex: the index of the fold you want the training data for, this should be in the range [0 K-1], where K is the number of folds the data was spilt into 
	 @return returns a training dataset
     */
    RegressionData getTrainingFoldData(const UINT foldIndex) const;
    
    /**
     Returns the test dataset for the k-th fold for cross validation.  The spiltDataIntoKFolds(UINT K) function should have been called once before using this function.
     The foldIndex should be in the range [0 K-1], where K is the number of folds the data was spilt into.
     
	 @param foldIndex: the index of the fold you want the test data for, this should be in the range [0 K-1], where K is the number of folds the data was spilt into 
	 @return returns a test dataset
     */
    RegressionData getTestFoldData(const UINT foldIndex) const;

    UINT removeDuplicateSamples();
    
    /**
     Gets the name of the dataset.
     
	 @return returns the name of the dataset
     */
    std::string getDatasetName()const { return datasetName; }
    
    /**
     Gets the infotext for the dataset
     
	 @return returns the infotext of the dataset
     */
    std::string getInfoText() const { return infoText; }

    std::string getStatsAsString() const;
    
	/**
     Gets the number of input dimensions of the labelled regression data.
     
	 @return an unsigned int representing the number of input dimensions in the dataset
     */
    UINT inline getNumInputDimensions() const{ return numInputDimensions; }
    
    /**
     Gets the number of target dimensions of the labelled regression data.
     
	 @return an unsigned int representing the number of target dimensions in the dataset
     */
    UINT inline getNumTargetDimensions() const{ return numTargetDimensions; }
	
	/**
     Gets the number of samples in the classification data across all the classes.
     
	 @return an unsigned int representing the total number of samples in the classification data
     */
    UINT inline getNumSamples() const{ return totalNumSamples; }
    
    /**
     Gets the input ranges of the dataset.
     
	 @return a Vector of minimum and maximum values for each input dimension of the data
     */
    Vector<MinMax> getInputRanges() const;
    
    /**
     Gets the target ranges of the dataset.
     
	 @return a Vector of minimum and maximum values for each target dimension of the data
     */
    Vector<MinMax> getTargetRanges() const;
    
	/**
     Gets the regression data.
     
	 @return a Vector of RegressionSample
     */
    Vector< RegressionSample > getData() const{ return data; }

private:
    std::string datasetName;                                     ///< The name of the dataset
    std::string infoText;                                        ///< Some infoText about the dataset						
	UINT numInputDimensions;                                ///< The number of input dimensions in the dataset
	UINT numTargetDimensions;                               ///< The number of target dimensions in the dataset
	UINT totalNumSamples;                                   ///< The total number of training samples in the dataset
    UINT kFoldValue;                                        ///< The number of folds the dataset has been spilt into for cross valiation
    bool crossValidationSetup;                              ///< A flag to show if the dataset is ready for cross validation
    bool useExternalRanges;                                 ///< A flag to show if the dataset should be scaled using the externalRanges values
    Vector< MinMax > externalInputRanges;                   ///< A Vector containing a set of externalRanges set by the user
    Vector< MinMax > externalTargetRanges;                  ///< A Vector containing a set of externalRanges set by the user
	Vector< RegressionSample > data;                        ///< The regression data
    Vector< Vector< UINT > >    crossValidationIndexs;      ///< A Vector to hold the indexs of the dataset for the cross validation
    
    DebugLog debugLog;                                      ///< Default debugging log
    ErrorLog errorLog;                                      ///< Default error log
    WarningLog warningLog;                                  ///< Default warning log
};

GRT_END_NAMESPACE

#endif //GRT_REGRESSION_DATA_HEADER

