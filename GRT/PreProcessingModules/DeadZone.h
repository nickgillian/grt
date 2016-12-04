/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0
@example PreprocessingModulesExamples/DeadZoneExample/DeadZoneExample.cpp
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

#ifndef GRT_DEADZONE_HEADER
#define GRT_DEADZONE_HEADER

#include "../CoreModules/PreProcessing.h"

GRT_BEGIN_NAMESPACE

/**
 @brief The DeadZone class sets any values in the input signal that fall within the dead-zone region to zero. Any values outside of the dead-zone region will be offset by the dead zone's lower limit and upper limit.
*/
class GRT_API DeadZone : public PreProcessing{
public:
    /**
    Constructor, sets the lower and upper limits of the dead-zone region and the dimensionality of the input data.
    
    @param lowerLimit: sets the lower limit of the dead-zone region.  Default lowerLimit = -0.1
    @param upperLimit: sets the upper limit of the dead-zone region.  Default upperLimit = 0.1
    @param numDimensions: the dimensionality of the input data.  Default numDimensions = 1
    */
    DeadZone( const Float lowerLimit = -0.1, const Float upperLimit = 0.1, const UINT numDimensions = 1 );
    
    /**
    Copy Constructor, copies the DeadZone from the rhs instance to this instance
    
    @param rhs: another instance of the DeadZone class from which the data will be copied to this instance
    */
    DeadZone(const DeadZone &rhs);
    
    /**
    Default Destructor
    */
    virtual ~DeadZone();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance
    
    @param rhs: another instance of the DeadZone class from which the data will be copied to this instance
    @return a reference to this instance of DeadZone
    */
    DeadZone& operator=(const DeadZone &rhs);
    
    /**
    Sets the PreProcessing deepCopyFrom function, overwriting the base PreProcessing function.
    This function is used to clone the values from the input pointer to this instance of the PreProcessing module.
    This function is called by the GestureRecognitionPipeline when the user adds a new PreProcessing module to the pipeline.
    
    @param preProcessing: a pointer to another instance of a DeadZone, the values of that instance will be cloned to this instance
    @return true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const PreProcessing *preProcessing);
    
    /**
    Sets the PreProcessing process function, overwriting the base PreProcessing function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This function calls the DeadZone's filter function.
    
    @param inputVector: the inputVector that should be processed.  Must have the same dimensionality as the PreProcessing module
    @return true if the data was processed, false otherwise
    */
    virtual bool process(const VectorFloat &inputVector);
    
    /**
    Sets the PreProcessing reset function, overwriting the base PreProcessing function.
    This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
    This function resets the deadZone by re-initiliazing the instance.
    
    @return true if the filter was reset, false otherwise
    */
    virtual bool reset();
    
    /**
    This saves the current settings of the DeadZone to a file.
    This overrides the save function in the PreProcessing base class.
    
    @param fstream &file: a reference to the file the settings will be saved to
    @return returns true if the settings were saved successfully, false otherwise
    */
    virtual bool save(std::fstream &file) const;
    
    /**
    This loads the DeadZone settings from a file.
    This overrides the load function in the PreProcessing base class.
    
    @param fstream &file: a reference to the file to load the settings from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load(std::fstream &file);
    
    /**
    Initializes the instance, sets the lower and upper limits of the dead-zone region and the dimensionality of the input data.
    
    @param lowerLimit: sets the lower limit of the dead-zone region
    @param upperLimit: sets the upper limit of the dead-zone region
    @param numDimensions: the dimensionality of the input data
    @return true if the instance was initiliazed, false otherwise
    */
    bool init(Float lowerLimit,Float upperLimit,UINT numDimensions);
    
    /**
    Filters the value x using the dead-zone values, this should only be called if the dimensionality of the instance was set to 1.
    
    @param x: the value to be filtered, this should only be called if the dimensionality of the filter was set to 1
    @return the filtered input value.  Zero will be returned if the value was not computed
    */
    Float filter(const Float x);
    
    /**
    Filters x using the dead-zone values, the dimensionality of the input should match the number of inputs for the dead zone
    
    @param &x: the values to be filtered, the dimensionality of the input should match the number of inputs for the derivative
    @return the filtered input values.  An empty vector will be returned if the values were not filtered
    */
    VectorFloat filter(const VectorFloat &x);

    /**
    Gets the lower limit of the dead-zone region.
    
    @return returns the lower limit if the DeadZone has been initialized, zero otherwise
    */
    Float getLowerLimit() const;
    
    /**
    Gets the upper limit of the dead-zone region.
    
    @return returns the upper limit if the DeadZone has been initialized, zero otherwise
    */
    Float getUpperLimit() const;
    
    /**
    Sets the lower limit of the dead-zone region.
    
    @param lowerLimit: the new lower limit for the dead zone
    @return returns true if the lowerLimit value was set, false otherwise
    */
    bool setLowerLimit(Float lowerLimit);
    
    /**
    Sets the upper limit of the dead-zone region.
    
    @param upperLimit: the new upper limit for the dead zone
    @return returns true if the upperLimit value was set, false otherwise
    */
    bool setUpperLimit(Float upperLimit);

    /**
    Gets a string that represents the ID of this class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    
protected:
    Float lowerLimit;               ///< The lower limit of the dead-zone region
    Float upperLimit;               ///< The upper limit of the dead-zone region
    
private:
    static const std::string id;   
    static RegisterPreProcessingModule< DeadZone > registerModule;
    
};

GRT_END_NAMESPACE

#endif //GRT_DEADZONE_HEADER
    