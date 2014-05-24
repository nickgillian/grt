/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a Finite Impulse Response (FIR) Filter.
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

#ifndef GRT_FIR_FILTER_HEADER
#define GRT_FIR_FILTER_HEADER

#include "../CoreModules/PreProcessing.h"

namespace GRT{
    
class FIRFilter : public PreProcessing{
public:
    /**
     Constructor, sets the filter factor, gain and dimensionality of the low pass filter.
     If the cutoffFrequency and delta values are set then the filter will be initialized with these values rather than the filterFactor.
     If the cutoffFrequency and delta values are kept at their default values of -1 then the values will be ignored and the filter factor will be used instead.
     Otherwise the fiterFactor will control the low pass filter, with a smaller filterFactor (i.e. 0.1) resulting in a more aggresive smoothing
     of the input signal.  The filterFactor should be in the range [0.0 1.0].
	 
     @param double filterFactor: controls the low pass filter, a smaller value will result in a more aggresive smoothing of the input signal. Default value filterFactor = 0.1
     @param double gain: multiples the filtered values by a constant ampltidue. Default value = 1.0
     @param UINT numDimensions: the dimensionality of the input data to filter.  Default numDimensions = 1
     @param double cutoffFrequency: sets the cutoffFrequency of the filter (in Hz). If the cutoffFrequency and delta values are set then the filter will be initialized with these values rather than the filterFactor.  Default value cutoffFrequency = -1.0
     @param double delta: the sampling rate of your sensor, delta should be set as 1.0/SR, where SR is the sampling rate of your sensor.  Default value delta = -1.0
     */
    FIRFilter(const UINT filterType = LPF,const UINT numTaps = 50,const double sampleRate = 100,const double cutoffFrequency = 10,const double gain = 1,const UINT numDimensions = 1);
    
    /**
     Copy Constructor, copies the FIRFilter from the rhs instance to this instance
     
	 @param const FIRFilter &rhs: another instance of the FIRFilter class from which the data will be copied to this instance
     */
    FIRFilter(const FIRFilter &rhs);
    
    /**
     Default Destructor
     */
	virtual ~FIRFilter();
    
    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const FIRFilter &rhs: another instance of the FIRFilter class from which the data will be copied to this instance
	 @return a reference to this instance of FIRFilter
     */
	FIRFilter& operator=(const FIRFilter &rhs);
    
    /**
     Sets the PreProcessing deepCopyFrom function, overwriting the base PreProcessing function.
     This function is used to deep copy the values from the input pointer to this instance of the PreProcessing module.
     This function is called by the GestureRecognitionPipeline when the user adds a new PreProcessing module to the pipeline.
     
	 @param const PreProcessing *preProcessing: a pointer to another instance of a FIRFilter, the values of that instance will be cloned to this instance
	 @return true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const PreProcessing *preProcessing);
    
    /**
     Sets the PreProcessing process function, overwriting the base PreProcessing function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function calls the FIRFilter's filter function.
     
	 @param const VectorDouble &inputVector: the inputVector that should be processed.  Must have the same dimensionality as the PreProcessing module
	 @return true if the data was processed, false otherwise
     */
    virtual bool process(const VectorDouble &inputVector);
    
    /**
     Sets the PreProcessing reset function, overwriting the base PreProcessing function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     This function resets the filter values by re-initiliazing the filter.
     
	 @return true if the filter was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     Sets the PreProcessing clear function, overwriting the base PreProcessing function.
     This function clears the filter values and de-initiliazes the filter.
     
	 @return true if the filter was reset, false otherwise
     */
    virtual bool clear();
    
    /**
     This saves the current settings of the FIRFilter to a file.
     This overrides the saveModelToFile function in the PreProcessing base class.
     
     @param string filename: the name of the file to save the settings to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the current settings of the FIRFilter to a file.
     This overrides the saveModelToFile function in the PreProcessing base class.
     
     @param fstream &file: a reference to the file the settings will be saved to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads the FIRFilter settings from a file.
     This overrides the loadModelFromFile function in the PreProcessing base class.
     
     @param string filename: the name of the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads the FIRFilter settings from a file.
     This overrides the loadModelFromFile function in the PreProcessing base class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);

    /**
     Builds the filter, using the current filter parameters.
     
	 @return true if the filter was built, false otherwise
     */
    bool buildFilter();
    
    /**
     Filters the input, this should only be called if the dimensionality of the filter was set to 1.
     
     @param const double x: the value to filter, this should only be called if the dimensionality of the filter was set to 1
	 @return the filtered value.  Zero will be returned if the value was not filtered
     */
    double filter(const double x);
    
    /**
     Filters the input, the dimensionality of the input vector should match that of the filter.
     
     @param const VectorDouble  &x: the values to filter, the dimensionality of the input vector should match that of the filter
	 @return the filtered values.  An empty vector will be returned if the values were not filtered
     */
    VectorDouble filter(const VectorDouble &x);
    
    /**
     Gets the filter type, this should be one of the FilterTypes enums.
     
	 @return an UINT representing the filter type
     */
    UINT getFilterType() const;
    
    /**
     Gets the number of taps in the filter.
     
	 @return an UINT representing the number of taps in the filter
     */
    UINT getNumTaps() const;
    
    /**
     Gets the sample rate that was used to design the filter.
     
	 @return a double representing the sample rate that was used to design the filter
     */
    double getSampleRate() const;
    
    /**
     Gets the cutoff frequency that was used to design the filter.  This is only relevant for a low-pass or high-pass filter.
     
	 @return a double representing the cutoff frequency that was used to design the filter
     */
    double getCutoffFrequency() const;
    
    /**
     Gets the lower cutoff frequency that was used to design the filter.  This is only relevant for a band-pass filter.
     
	 @return a double representing the lower cutoff frequency that was used to design the filter
     */
    double getCutoffFrequencyLower() const;
    
    /**
     Gets the upper cutoff frequency that was used to design the filter.  This is only relevant for a band-pass filter.
     
	 @return a double representing the upper cutoff frequency that was used to design the filter
     */
    double getCutoffFrequencyUpper() const;
    
    /**
     Gets the current gain value.
     
	 @return the current gain value
     */
    double getGain() const;
    
    /**
     Returns the last N value(s) that were input to the filter.
     
	 @return the filtered values.  An empty vector will be returned if the filter has not been initalized
     */
    vector< VectorDouble > getInputBuffer() const;
    
    /**
     Returns the filter coefficents as a VectorDouble.
     
	 @return the filtered values.  An empty vector will be returned if the filter has not been initalized
     */
    VectorDouble getFilterCoefficents() const;
    
    /**
     Sets the filter type, this should be one of the FilterTypes enums.  This will deinitalize the filter, you should rebuild the filter after changing this value.
     
     @param const UINT filterType: the new filterType
	 @return true if the filterType value was set, false otherwise
     */
    bool setFilterType(const UINT filterType);
    
    /**
     Sets the number of taps in the filter, this controls the filter size and accuracy.  
     More taps will give you a more responsive filter, less taps will give you a faster filter.
     This will deinitalize the filter, you should rebuild the filter after changing this value.
     
     @param const UINT numTaps: the number of taps in your filter
	 @return true if the numTaps value was set, false otherwise
     */
    bool setNumTaps(const UINT numTaps);
    
    /**
     Sets the sample rate of your signal.  This will deinitalize the filter, you should rebuild the filter after changing this value.
     
     @param const double sampleRate: the new sampleRate value
	 @return true if the sampleRate value was set, false otherwise
     */
    bool setSampleRate(const double sampleRate);
    
    /**
     Sets the cutoff frequency of the filter.  This should be used with either a low-pass or high-pass filter.  The cutoffFrequency should be in Hz.
     This will deinitalize the filter, you should rebuild the filter after changing this value.
     
     @param const double cutoffFrequency: the cutoff frequency of the filter in Hz
	 @return true if the cutoffFrequency value was set, false otherwise
     */
    bool setCutoffFrequency(const double cutoffFrequency);
    
    /**
     Sets the lower and upper cutoff frequency for a band-pass filter.  The cutoffFrequencies should be in Hz.
     This will deinitalize the filter, you should rebuild the filter after changing this value.
     
     @param const double cutoffFrequencyLower: the lower cutoff frequency of the band-pass filter in Hz
     @param const double cutoffFrequencyUpper: the upper cutoff frequency of the band-pass filter in Hz
	 @return true if the parameters were set, false otherwise
     */
    bool setCutoffFrequency(const double cutoffFrequencyLower,const double cutoffFrequencyUpper);
    
    /**
     Sets the gain of the low pass filter.
     
     @param double gain: the new gain value, this multiples the filtered values by a constant ampltidue
	 @return true if the gain value was set, false otherwise
     */
    bool setGain(const double gain);

protected:
    UINT filterType;
    UINT numTaps;
    double sampleRate;
    double cutoffFrequency;
    double cutoffFrequencyLower;
    double cutoffFrequencyUpper;
    double gain;
    CircularBuffer< VectorDouble > y;
    VectorDouble z;
    
    static RegisterPreProcessingModule< FIRFilter > registerModule;
    
public:
    enum FilterTypes{LPF=0, HPF, BPF};
};

}//End of namespace GRT

#endif //GRT_FIR_FILTER_HEADER
