/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @section LICENSE
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
 
 @section DESCRIPTION
 The ZeroCrossingCounter class counts the number of zero crossings that occur in an N dimensional signal over a given window. 
 
 The ZeroCrossingCounter stores the incoming N dimensional signal in a circular buffer, it then computes two main features from the data in this buffer, these are:
 zero-crossing count: the number of times the derivative of the input signal crosses zero within the buffer
 zero-crossing magnitude: the absolute sum of the input signal at the locations where the derivative of the input signal crosses zero within the buffer
 
 The ZeroCrossingCounter can compute these features in two modes: INDEPENDANT_FEATURE_MODE or COMBINED_FEATURE_MODE. 
 
 In INDEPENDANT_FEATURE_MODE the zero-crossing count and zero-crossing magnitude features will be computed independently for each of the N dimensions in the input signal. 
 In COMBINED_FEATURE_MODE the zero-crossing count and zero-crossing magnitude features will be integrated across all of the N dimensions in the input signal.
 This means that if the ZeroCrossingCounter is set to INDEPENDANT_FEATURE_MODE, the size of the output feature vector will be 2 * N, where 2 is the two features (zero-crossing count and zero-crossing magnitude) and N is the number of dimensions in the input signal. Alternatively in COMBINED_FEATURE_MODE the size of the output vector will simply be 2, where 2 is the two features (zero-crossing count and zero-crossing magnitude). The feature modes can be set either in the ZeroCrossingCounter constructor or by using the setFeatureMode(UINT featureMode) function.
 
 The ZeroCrossingCounter class is part of the Feature Extraction Modules.
 */

#ifndef GRT_ZERO_CROSSING_COUNTER_HEADER
#define GRT_ZERO_CROSSING_COUNTER_HEADER

#include "../../CoreModules/FeatureExtraction.h"
#include "../../PreProcessingModules/Derivative.h"
#include "../../PreProcessingModules/DeadZone.h"

namespace GRT{
    
class ZeroCrossingCounter : public FeatureExtraction{
public:
    /**
     Constructor, sets the search window size, deadZoneThreshold, and the dimensionality of the input data.
     The search window size sets how much data should be held in memory and searched each time the update function is called.
     
     @param UINT searchWindowSize: sets how much data should be held in memory and searched each time the update function is called. Default value = 20
     @param double deadZoneThreshold: sets the dead zone threshold. Default value = 0.01
     @param UINT numDimensions: the dimensionality of the input data to filter.  Default numDimensions = 1
     @param UINT featureMode: sets how the features are added to the feature vector, shoule be either INDEPENDANT_FEATURE_MODE or COMBINED_FEATURE_MODE.  Default is featureMode = INDEPENDANT_FEATURE_MODE
     */
    ZeroCrossingCounter(UINT searchWindowSize = 20,double deadZoneThreshold = 0.01,UINT numDimensions = 1,UINT featureMode = INDEPENDANT_FEATURE_MODE);
	
    /**
     Copy constructor, copies the ZeroCrossingCounter from the rhs instance to this instance.
     
     @param const ZeroCrossingCounter &rhs: another instance of the ZeroCrossingCounter class from which the data will be copied to this instance
     */
    ZeroCrossingCounter(const ZeroCrossingCounter &rhs);
    
    /**
     Default Destructor
     */
    virtual ~ZeroCrossingCounter();
    
    /**
     Sets the equals operator, copies the data from the rhs instance to this instance.
     
     @param const ZeroCrossingCounter &rhs: another instance of the ZeroCrossingCounter class from which the data will be copied to this instance
     @return a reference to this instance of ZeroCrossingCounter
     */
    ZeroCrossingCounter& operator=(const ZeroCrossingCounter &rhs);

    /**
     Sets the FeatureExtraction deepCopyFrom function, overwriting the base FeatureExtraction function.
     This function is used to deep copy the values from the input pointer to this instance of the FeatureExtraction module.
     This function is called by the GestureRecognitionPipeline when the user adds a new FeatureExtraction module to the pipeleine.
     
     @param const FeatureExtraction *featureExtraction: a pointer to another instance of a ZeroCrossingCounter, the values of that instance will be cloned to this instance
     @return returns true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const FeatureExtraction *featureExtraction);
    
    /**
     Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function calls the ZeroCrossingCounter's update function.
     
     @param const VectorDouble &inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
     @return returns true if the data was processed, false otherwise
     */
    virtual bool computeFeatures(const VectorDouble &inputVector);
    
    /**
     Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     This function resets the feature extraction by re-initiliazing the instance.
     
     @return true if the filter was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This saves the feature extraction settings to a file.
     
     @param const string filename: the filename to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(const string filename) const;
    
    /**
     This saves the feature extraction settings to a file.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool loadModelFromFile(const string filename);
    
    /**
     This saves the feature extraction settings to a file.
     This overrides the saveSettingsToFile function in the FeatureExtraction base class.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads the feature extraction settings from a file.
     This overrides the loadSettingsFromFile function in the FeatureExtraction base class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);

    /**
     Initializes the ZeroCrossingCounter, setting the searchWindowSize, deadZoneThreshold, and dimensionality of the data it will filter.
     The search window size, deadZoneThreshold, and numDimensions values must be larger than 0.
     Sets all the data buffer values to zero.
     
     @param UINT searchWindowSize: sets how much data should be held in memory and searched each time the update function is called
     @param double deadZoneThreshold: sets the dead zone threshold value
     @param UINT numDimensions: the dimensionality of the input data to filter
     @param UINT featureMode: sets how the features are added to the feature vector, shoule be either INDEPENDANT_FEATURE_MODE or COMBINED_FEATURE_MODE
	 @return true if the filter was initiliazed, false otherwise
     */
    bool init(UINT searchWindowSize,double deadZoneThreshold,UINT numDimensions,UINT featureMode);
    
    /**
     Computes the ZeroCrossingCounter features from the input, this should only be called if the dimensionality of this instance was set to 1.
     
     @param double x: the value to compute features from, this should only be called if the dimensionality of the filter was set to 1
	 @return a vector containing the ZeroCrossingCounter features, an empty vector will be returned if the features were not computed
     */
	VectorDouble update(double x);
    
    /**
     Computes the ZeroCrossingCounter features from the input, the dimensionality of x should match that of this instance.
     
     @param const VectorDouble &x: a vector containing the values to be processed, must be the same size as the numInputDimensions
	 @return a vector containing the ZeroCrossingCounter features, an empty vector will be returned if the features were not computed
     */
    VectorDouble update(const VectorDouble &x);
    
    /**
     Sets the search window size.  The search window size must be larger than zero.
     Calling this function will reset the feature extraction.
     
     @param UINT searchWindowSize: sets how much data should be held in memory and searched each time the update function is called
	 @return true if the searchWindowSize value was updated, false otherwise
     */
    bool setSearchWindowSize(UINT searchWindowSize);
    
    /**
     Sets the deadZoneThreshold.  The deadZoneThreshold must be larger than zero.
     Calling this function will reset the feature extraction.
     
     @param double deadZoneThreshold: sets the dead zone threshold value
	 @return true if the deadZoneThreshold value was updated, false otherwise
     */
    bool setDeadZoneThreshold(UINT deadZoneThreshold);
    
    /**
     Sets the featureMode, this should be either INDEPENDANT_FEATURE_MODE (0) or COMBINED_FEATURE_MODE (1).
     Calling this function will reset the feature extraction.
     
     @param UINT featureMode: sets the featureMode, options are either INDEPENDANT_FEATURE_MODE (0) or COMBINED_FEATURE_MODE (1)
	 @return true if the featureMode value was updated, false otherwise
     */
    bool setFeatureMode(UINT featureMode);

    /**
     Gets the search window size.
     
     @return returns an unsigned int representing the search window size, returns zero if the feature extraction module has not been initialized
     */
    UINT getSearchWindowSize(){ if( initialized ){ return searchWindowSize; } return 0; }
    
    /**
     Gets the number of feature values computed for each input dimensions.  
     The size of the feature vector will be getNumFeatures() * numInputDimensions.
     
     @return returns an unsigned int representing the total number of zero crossing features per input dimension
     */
    UINT getNumFeatures(){ return TOTAL_NUM_ZERO_CROSSING_FEATURES; }
    
    /**
     Gets the current featureMode, this will be either INDEPENDANT_FEATURE_MODE (0) or COMBINED_FEATURE_MODE (1).
     
     @return returns an unsigned int representing the current feature mode
     */
    UINT getFeatureMode(){ return featureMode; }
    
    /**
     Gets the deadZoneThreshold value.
     
     @return returns a double representing the deadZoneThreshold, returns zero if the feature extraction module has not been initialized
     */
    double getDeadZoneThreshold(){ if( initialized ){ return deadZoneThreshold; } return 0; }
    
    /** 
     Gets the current values in the data buffer.  
     An empty circular buffer will be returned if the ZeroCrossingCounter has not been initialized.
     
     @return returns a curcular buffer containing the data buffer values, an empty circular buffer will be returned if the ZeroCrossingCounter has not been initialized
     */
    CircularBuffer< VectorDouble > getDataBuffer(){ if( initialized ){ return dataBuffer; } return CircularBuffer< VectorDouble >(); }
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using FeatureExtraction::saveModelToFile;
    using FeatureExtraction::loadModelFromFile;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;
    
protected:
    UINT searchWindowSize;                                  ///< The size of the search window, i.e. the amount of previous data stored and searched
    UINT featureMode;                                       ///< The featureMode controls how the features are added to the feature vector
    double deadZoneThreshold;                               ///< The threshold value used for the dead zone filter
    Derivative derivative;                                  ///< Used to compute the derivative of the input signal
    DeadZone deadZone;                                      ///< Used to remove small amounts of noise from the data
    CircularBuffer< VectorDouble > dataBuffer;              ///< A buffer used to store the previous derivative data
    
    static RegisterFeatureExtractionModule< ZeroCrossingCounter > registerModule;
    
public:
    enum ZeroCrossingFeatureIDs{NUM_ZERO_CROSSINGS_COUNTED=0,ZERO_CROSSING_MAGNITUDE,TOTAL_NUM_ZERO_CROSSING_FEATURES};
    enum FeatureModes{INDEPENDANT_FEATURE_MODE=0,COMBINED_FEATURE_MODE};
};

}//End of namespace GRT

#endif //GRT_ZERO_CROSSING_COUNTER_HEADER
