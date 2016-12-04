/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief This class implements the TimeseriesBuffer feature extraction module.
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

#ifndef GRT_TIMESERIES_BUFFER_HEADER
#define GRT_TIMESERIES_BUFFER_HEADER

#include "../../CoreModules/FeatureExtraction.h"

GRT_BEGIN_NAMESPACE

class GRT_API TimeseriesBuffer : public FeatureExtraction{
public:
    /**
    Constructor, sets the size of the timeseries buffer and number of input dimensions.
    
    @param bufferSize: sets the size of the timeseries buffer. Default value = 5
    @param numDimensions: sets the number of dimensions that will be input to the feature extraction. Default value = 1
    */
    TimeseriesBuffer(const UINT bufferSize = 5,const UINT numDimensions = 1);
    
    /**
    Copy constructor, copies the TimeseriesBuffer from the rhs instance to this instance.
    
    @param rhs: another instance of the TimeseriesBuffer class from which the data will be copied to this instance
    */
    TimeseriesBuffer(const TimeseriesBuffer &rhs);
    
    /**
    Default Destructor
    */
    virtual ~TimeseriesBuffer();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance.
    
    @param rhs: another instance of the TimeseriesBuffer class from which the data will be copied to this instance
    @return a reference to this instance of TimeseriesBuffer
    */
    TimeseriesBuffer& operator=(const TimeseriesBuffer &rhs);
    
    /**
    Sets the FeatureExtraction deepCopyFrom function, overwriting the base FeatureExtraction function.
    This function is used to deep copy the values from the input pointer to this instance of the FeatureExtraction module.
    This function is called by the GestureRecognitionPipeline when the user adds a new FeatureExtraction module to the pipeleine.
    
    @param featureExtraction: a pointer to another instance of a TimeseriesBuffer, the values of that instance will be cloned to this instance
    @return returns true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const FeatureExtraction *featureExtraction);
    
    /**
    Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This function calls the TimeseriesBuffer's update function.
    
    @param inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
    @return returns true if the data was processed, false otherwise
    */
    virtual bool computeFeatures(const VectorFloat &inputVector);
    
    /**
    Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
    This function resets the feature extraction by re-initiliazing the instance.
    
    @return true if the filter was reset, false otherwise
    */
    virtual bool reset();
    
    /**
    This saves the feature extraction settings to a file.
    This overrides the save function in the FeatureExtraction base class.
    
    @param file: a reference to the file to save the settings to
    @return returns true if the settings were saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads the feature extraction settings from a file.
    This overrides the load function in the FeatureExtraction base class.
    
    @param file: a reference to the file to load the settings from
    @return returns true if the settings were loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    Initializes the TimeseriesBuffer, setting the bufferSize and the dimensionality of the data it will buffer.
    The search bufferSize and numDimensions values must be larger than 0.
    Sets all the data buffer values to zero.
    
    @param bufferSize: sets the size of the timeseries buffer
    @param numDimensions: sets the number of dimensions that will be input to the feature extraction
    @return true if the TimeseriesBuffer was initiliazed, false otherwise
    */
    bool init(const UINT bufferSize,const UINT numDimensions);
    
    /**
    Updates the timeseries buffer with the new data x, this should only be called if the dimensionality of this instance was set to 1.
    
    @param x: the value to add to the buffer, this should only be called if the dimensionality of the filter was set to 1
    @return a vector containing the timeseries buffer, an empty vector will be returned if the buffer is not initialized
    */
    VectorFloat update(const Float x);
    
    /**
    Updates the timeseries buffer with the new data x, the dimensionality of x should match that of this instance.
    
    @param x: a vector containing the values to be processed, must be the same size as the numInputDimensions
    @return a vector containing the timeseries buffer, an empty vector will be returned if the buffer is not initialized
    */
    VectorFloat update(const VectorFloat &x);
    
    /**
    Sets the timeseries buffer size.  The buffer size must be larger than zero.
    Calling this function will reset the feature extraction.
    
    @param bufferSize: sets the size of the timeseries buffer
    @return true if the bufferSize value was updated, false otherwise
    */
    bool setBufferSize(const UINT bufferSize);
    
    /**
    Gets the buffer size.
    
    @return returns an unsigned int representing the buffer size, returns zero if the feature extraction module has not been initialized
    */
    UINT getBufferSize() const;
    
    /**
    Gets the current values in the timeseries buffer.
    An empty vector will be returned if the buffer has not been initialized.
    
    @return returns a vector containing the timeseries values, an empty vector will be returned if the module has not been initialized
    */
    Vector< VectorFloat > getDataBuffer() const;
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;

    /**
    Gets a string that represents the TimeseriesBuffer class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
protected:
    UINT bufferSize;
    CircularBuffer< VectorFloat > dataBuffer;              ///< A buffer used to store the timeseries data
    
private:
    static RegisterFeatureExtractionModule< TimeseriesBuffer > registerModule;
    static std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_TIMESERIES_BUFFER_HEADER
    