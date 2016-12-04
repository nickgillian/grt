/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0
@example PreprocessingModulesExamples/HighPassFilterExample/HighPassFilterExample.cpp
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

#ifndef GRT_HIGH_PASS_FILTER_HEADER
#define GRT_HIGH_PASS_FILTER_HEADER

#include "../CoreModules/PreProcessing.h"

GRT_BEGIN_NAMESPACE

/**
 @brief This class implements a High Pass Filter.
*/
class GRT_API HighPassFilter : public PreProcessing{
public:
    /**
    Constructor, sets the filter factor, gain and dimensionality of the high pass filter.
    If the cutoffFrequency and delta values are set then the filter will be initialized with these values rather than the filterFactor.
    If the cutoffFrequency and delta values are kept at their default values of -1 then the values will be ignored and the filter factor will be used instead.
    Otherwise the fiterFactor will control the high pass filter, with a larger filterFactor (i.e. 0.995) resulting in a more aggresive attenuation of low frequency signals in the input signal.  The filterFactor should be in the range [0.0 1.0].
    
    @param filterFactor: controls the high pass filter, a larger value will result in a more aggresive attenuation of low frequency signals in the input signal. Default value filterFactor = 0.99
    @param gain: multiples the filtered values by a constant ampltidue. Default value = 1.0
    @param numDimensions: the dimensionality of the input data to filter.  Default numDimensions = 1
    @param cutoffFrequency: sets the cutoffFrequency of the filter (in Hz). If the cutoffFrequency and delta values are set then the filter will be initialized with these values rather than the filterFactor.  Default value cutoffFrequency = -1.0
    @param delta: the sampling rate of your sensor, delta should be set as 1.0/SR, where SR is the sampling rate of your sensor.  Default value delta = -1.0
    */
    HighPassFilter(const Float filterFactor = 0.99,const Float gain = 1,const UINT numDimensions = 1,const Float cutoffFrequency = -1,const Float delta = -1);
    
    /**
    Copy Constructor, copies the HighPassFilter from the rhs instance to this instance
    
    @param rhs: another instance of the HighPassFilter class from which the data will be copied to this instance
    */
    HighPassFilter(const HighPassFilter &rhs);
    
    /**
    Default Destructor
    */
    virtual ~HighPassFilter();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance
    
    @param rhs: another instance of the HighPassFilter class from which the data will be copied to this instance
    @return a reference to this instance of HighPassFilter
    */
    HighPassFilter& operator=(const HighPassFilter &rhs);
    
    /**
    Sets the PreProcessing deepCopyFrom function, overwriting the base PreProcessing function.
    This function is used to deep copy the values from the input pointer to this instance of the PreProcessing module.
    This function is called by the GestureRecognitionPipeline when the user adds a new PreProcessing module to the pipeline.
    
    @param preProcessing: a pointer to another instance of a HighPassFilter, the values of that instance will be cloned to this instance
    @return true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const PreProcessing *preProcessing);
    
    /**
    Sets the PreProcessing process function, overwriting the base PreProcessing function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This function calls the HighPassFilter's filter function.
    
    @param inputVector: the inputVector that should be processed.  Must have the same dimensionality as the PreProcessing module
    @return true if the data was processed, false otherwise
    */
    virtual bool process(const VectorFloat &inputVector);
    
    /**
    Sets the PreProcessing reset function, overwriting the base PreProcessing function.
    This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
    This function resets the filter values by re-initiliazing the filter.
    
    @return true if the filter was reset, false otherwise
    */
    virtual bool reset();
    
    /**
    This saves the current settings of the HighPassFilter to a file.
    This overrides the save function in the PreProcessing base class.
    
    @param file: a reference to the file the settings will be saved to
    @return returns true if the settings were saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads the HighPassFilter settings from a file.
    This overrides the load function in the PreProcessing base class.
    
    @param file: a reference to the file to load the settings from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    Initializes the filter, setting the filter size and dimensionality of the data it will filter.
    Sets all the filter values to zero.
    
    @param filterFactor: controls the high pass filter, a larger value will result in a more aggresive attenuation of low frequency signals in the input signal
    @param gain: multiples the filtered values by a constant ampltidue
    @param numDimensions: the dimensionality of the input data to filter
    @return true if the filter was initiliazed, false otherwise
    */
    bool init(const Float filterFactor,const Float gain,const UINT numDimensions);
    
    /**
    Filters the input, this should only be called if the dimensionality of the filter was set to 1.
    
    @param x: the value to filter, this should only be called if the dimensionality of the filter was set to 1
    @return the filtered value.  Zero will be returned if the value was not filtered
    */
    Float filter(const Float x);
    
    /**
    Filters the input, the dimensionality of the input vector should match that of the filter.
    
    @param x: the values to filter, the dimensionality of the input vector should match that of the filter
    @return the filtered values.  An empty vector will be returned if the values were not filtered
    */
    VectorFloat filter(const VectorFloat &x);
    
    /**
    Sets the gain of the high pass filter.
    This will also reset the filter.
    
    @param gain: the new gain value, this multiples the filtered values by a constant ampltidue
    @return true if the gain value was set, false otherwise
    */
    bool setGain(const Float gain);
    
    /**
    Sets the filter factor, this controls the high pass filter, a larger value will result in a more aggresive attenuation of low frequency signals in the input signal.
    This should be a value in the range [0.0 1.0].
    This will also reset the filter.
    
    @param filterFactor: the new filterFactor value
    @return true if the filterFactor value was set, false otherwise
    */
    bool setFilterFactor(const Float filterFactor);
    
    /**
    Sets the cutoff frequency of the filter, this updates the filterFactor.  The cutoffFrequency should in Hz.
    This will also reset the filter.
    
    @param cutoffFrequency: the cutoff frequency of the filter in Hz
    @param delta: the sampling rate of your sensor, delta should be set as 1.0/SR, where SR is the sampling rate of your sensor
    @return true if the filterFactor value was set, false otherwise
    */
    bool setCutoffFrequency(const Float cutoffFrequency,const Float delta);
    
    /**
    Gets the current filter factor if the filter has been initialized.
    
    @return the current filter factor if the filter has been initialized, zero otherwise
    */
    Float getFilterFactor() const;
    
    /**
    Gets the current gain value if the filter has been initialized.
    
    @return the currentgain value if the filter has been initialized, zero otherwise
    */
    Float getGain() const;
    
    /**
    Returns the last value(s) that were filtered.
    
    @return the filtered values.  An empty vector will be returned if the values were not filtered
    */
    VectorFloat getFilteredValues() const;

    /**
    Gets a string that represents the ID of this class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    
protected:
    Float filterFactor;        ///< The filter factor (alpha) of the filter
    Float gain;                ///< The gain factor of the filter
    VectorFloat xx;        ///< The previous input value(s)
    VectorFloat yy;        ///< The previous output value(s)
    
private:
    static const std::string id;   
    static RegisterPreProcessingModule< HighPassFilter > registerModule;
};

GRT_END_NAMESPACE

#endif //GRT_HIGH_PASS_FILTER_HEADER
    