/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0
@example PreprocessingModulesExamples/DerivativeExample/DerivativeExample.cpp
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

#ifndef GRT_DERIVATIVE_HEADER
#define GRT_DERIVATIVE_HEADER

#include "../CoreModules/PreProcessing.h"
#include "MovingAverageFilter.h"

GRT_BEGIN_NAMESPACE

/**
 @brief The Derivative class computes either the first or second order derivative of the input signal.
*/
class GRT_API Derivative : public PreProcessing{
public:
    enum DerivativeOrders{FIRST_DERIVATIVE=1,SECOND_DERIVATIVE};
    
    /**
    Constructor, sets the derivativeOrder (which should be either FIRST_DERIVATIVE or SECOND_DERIVATIVE), the delta value (which should be set to 1000.0/sampleRate, the dimensionality of the input data,
    if the data should be filtered before computing the derivative, and the size of the filter if the data is to be filtered.
    
    @param derivativeOrder: the derivative order, should be either FIRST_DERIVATIVE or SECOND_DERIVATIVE. Default derivativeOrder = FIRST_DERIVATIVE
    @param delta: sets the time between samples, this should be set to sampleRate/1000.0, where sampleRate is the sample rate of your sensor data.  Default delta = 1.0
    @param numDimensions: the dimensionality of the input data.  Default numDimensions = 1
    @param filterData: a flag that sets if the data should be filtered before computing the derivative.  Default filterData = true
    @param filterSize: the size of the filter if the data is to be filtered
    */
    Derivative(const UINT derivativeOrder=FIRST_DERIVATIVE,const Float delta = 1,const UINT numDimensions = 1,const bool filterData = true,const UINT filterSize = 3);
    
    /**
    Copy Constructor, copies the Derivative from the rhs instance to this instance
    
    @param &rhs: another instance of the Derivative class from which the data will be copied to this instance
    */
    Derivative(const Derivative &rhs);
    
    /**
    Default Destructor
    */
    virtual ~Derivative();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance
    
    @param rhs: another instance of the Derivative class from which the data will be copied to this instance
    @return a reference to this instance of Derivative
    */
    Derivative& operator=(const Derivative &rhs);
    
    /**
    Sets the PreProcessing deepCopyFrom function, overwriting the base PreProcessing function.
    This function is used to clone the values from the input pointer to this instance of the PreProcessing module.
    This function is called by the GestureRecognitionPipeline when the user adds a new PreProcessing module to the pipeline.
    
    @param preProcessing: a pointer to another instance of a Derivative, the values of that instance will be cloned to this instance
    @return true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const PreProcessing *preProcessing);
    
    /**
    Sets the PreProcessing process function, overwriting the base PreProcessing function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This function calls the Derivative's computeDerivative function.
    
    @param inputVector: the inputVector that should be processed.  Must have the same dimensionality as the PreProcessing module
    @return true if the data was processed, false otherwise
    */
    virtual bool process(const VectorFloat &inputVector);
    
    /**
    Sets the PreProcessing reset function, overwriting the base PreProcessing function.
    This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
    This function resets the Derivative values by re-initiliazing the filter.
    
    @return true if the Derivative was reset, false otherwise
    */
    virtual bool reset();
    
    /**
    This saves the current settings of the Derivative to a file.
    This overrides the save function in the PreProcessing base class.
    
    @param file: a reference to the file the settings will be saved to
    @return returns true if the settings were saved successfully, false otherwise
    */
    virtual bool save(std::fstream &file) const;
    
    /**
    This loads the Derivative settings from a file.
    This overrides the load function in the PreProcessing base class.
    
    @param file: a reference to the file to load the settings from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load(std::fstream &file);
    
    /**
    Initializes the instance, sets the derivativeOrder (which should be either FIRST_DERIVATIVE or SECOND_DERIVATIVE), the delta value (which should be set to 1000.0/sampleRate, the dimensionality of the input data,
    if the data should be filtered before computing the derivative, and the size of the filter if the data is to be filtered.
    
    @param derivativeOrder: the derivative order, should be either FIRST_DERIVATIVE or SECOND_DERIVATIVE. Default derivativeOrder = FIRST_DERIVATIVE
    @param delta: sets the time between samples, this should be set to sampleRate/1000.0, where sampleRate is the sample rate of your sensor data.  Default delta = 1.0
    @param numDimensions: the dimensionality of the input data.  Default numDimensions = 1
    @param filterData: a flag that sets if the data should be filtered before computing the derivative.  Default filterData = true
    @param filterSize: the size of the filter if the data is to be filtered
    @return true if the instance was initiliazed, false otherwise
    */
    bool init(const UINT derivativeOrder,const Float delta,const UINT numDimensions,const bool filterData,const UINT filterSize);
    
    /**
    Computes the derivative of the input, this should only be called if the dimensionality of the instance was set to 1.
    
    @param x: the value to compute the derivative of, this should only be called if the dimensionality of the filter was set to 1
    @return the derivative of the input.  Zero will be returned if the value was not computed
    */
    Float computeDerivative(const Float x);
    
    /**
    Computes the derivative of the input, the dimensionality of the input should match the number of inputs for the derivative
    
    @param x: the values to compute the derivative of, the dimensionality of the input should match the number of inputs for the derivative
    @return the derivatives of the input.  An empty vector will be returned if the values were not filtered
    */
    VectorFloat computeDerivative(const VectorFloat &x);
    
    /**
    Sets the derivative order.  This should either be FIRST_DERIVATIVE (1) or SECOND_DERIVATIVE (2).
    Setting the derivative order will re-initialize this instance.
    
    @param derivativeOrder: the derivative order you wish to set, this should either be FIRST_DERIVATIVE (1) or SECOND_DERIVATIVE (2)
    @return returns true if the derivative order was set, false otherwise
    */
    bool setDerivativeOrder(const UINT derivativeOrder);
    
    /**
    Sets the size of the moving average filter used to smooth the input data (if the filterData parameter is set to true).
    The filterSize value must be greater than zero.
    Setting the filterSize will re-initialize this instance.
    
    @param filterSize: the size of the moving average filter used to smooth the input data, must be greater than zero
    @return returns true if the filterSize was set, false otherwise
    */
    bool setFilterSize(const UINT filterSize);
    
    /**
    Sets the delta value.  This is the time between samples and should be set to sampleRate/1000.0, where sampleRate is the sample rate of your sensor data.
    For example, if your sensor runs at 30FPS then delta would be 30.0/1000.0 = 0.03.
    Delta must be greater than zero.
    Setting delta will re-initialize this instance.
    
    @param delta: the estimated sampling time between sensor samples, must be greater than zero
    @return returns true if delta was set, false otherwise
    */
    bool setDelta(const Float delta);
    
    /**
    Sets if the input data will be filtered using a moving average filter before the derivative is computed.
    Updating this parameter will re-initialize this instance.
    
    @param filterData: sets if the data should be filtered before computing the derivative
    @return returns true if the filterData parameter was set, false otherwise
    */
    bool enableFiltering(const bool filterData);
    
    /**
    Gets the size of the moving average filter.  If the instance has not been initialized then zero will be returned.
    
    @return returns the size of the moving average filter, will return 0 if no values have been computed
    */
    UINT getFilterSize() const;
    
    /**
    Gets the last computed derivative value.
    Will return either the first or second derivative (you can only get the second derivative if the derivativeOrder is set to 2nd derivative).
    
    @param derivativeOrder: flags which derivative order you want, the default value is 0 which will return whatever the current derivativeOrder is
    @return returns the last computed derivative value, will return 0 if no values have been computed
    */
    Float getDerivative(const UINT derivativeOrder = FIRST_DERIVATIVE) const;
    
    /**
    Gets the last computed derivative values.
    Will return either the first or second derivative (you can only get the second derivative if the derivativeOrder is set to 2nd derivative).
    
    @param derivativeOrder: flags which derivative order you want, the default value is 0 which will return whatever the current derivativeOrder is
    @return returns the last computed derivative values, will return 0 if no values have been computed
    */
    VectorFloat getDerivatives(const UINT derivativeOrder = FIRST_DERIVATIVE) const;

    /**
    Gets a string that represents the ID of this class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;
    
protected:
    UINT derivativeOrder;                   ///< The order of the derivative that will be computed (either FIRST_DERIVATIVE or SECOND_DERIVATIVE)
    UINT filterSize;                        ///< The size of the filter used to filter the input data before the derivative is computed
    Float delta;                           ///< The estimated time between sensor samples
    bool filterData;                        ///< Flags if the input data should be filtered before the derivative is computed
    MovingAverageFilter filter;             ///< The filter used to low pass filter the input data
    VectorFloat yy;                        ///< A buffer holding the previous input value(s)
    VectorFloat yyy;                       ///< A buffer holding the previous first derivative values
    
private:
    static const std::string id;   
    static RegisterPreProcessingModule< Derivative > registerModule;
    
};

GRT_END_NAMESPACE

#endif //GRT_DERIVATIVE_HEADER
    