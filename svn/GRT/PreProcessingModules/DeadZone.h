/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief The DeadZone class sets any values in the input signal that fall within the dead-zone region to zero. 
 Any values outside of the dead-zone region will be offset by the dead zone's lower limit and upper limit.
 
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

namespace GRT{
    
class DeadZone : public PreProcessing{
public:
	/**
     Constructor, sets the lower and upper limits of the dead-zone region and the dimensionality of the input data.
	 
	 @param double lowerLimit: sets the lower limit of the dead-zone region.  Default lowerLimit = -0.1
	 @param UINT upperLimit: sets the upper limit of the dead-zone region.  Default upperLimit = 0.1
     @param UINT numDimensions: the dimensionality of the input data.  Default numDimensions = 1
     */
    DeadZone(double lowerLimit = -0.1,double upperLimit = 0.1,UINT numDimensions = 1);

	/**
     Copy Constructor, copies the DeadZone from the rhs instance to this instance
     
	 @param const DeadZone &rhs: another instance of the DeadZone class from which the data will be copied to this instance
     */
	DeadZone(const DeadZone &rhs);

	/**
     Default Destructor
     */
	virtual ~DeadZone();
	
	/**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const DeadZone &rhs: another instance of the DeadZone class from which the data will be copied to this instance
	 @return a reference to this instance of DeadZone
     */
	DeadZone& operator=(const DeadZone &rhs);
    
    /**
     Sets the PreProcessing deepCopyFrom function, overwriting the base PreProcessing function.
     This function is used to clone the values from the input pointer to this instance of the PreProcessing module.
     This function is called by the GestureRecognitionPipeline when the user adds a new PreProcessing module to the pipeline.
     
	 @param const PreProcessing *preProcessing: a pointer to another instance of a DeadZone, the values of that instance will be cloned to this instance
	 @return true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const PreProcessing *preProcessing);
    
    /**
     Sets the PreProcessing process function, overwriting the base PreProcessing function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function calls the DeadZone's filter function.
     
	 @param const VectorDouble &inputVector: the inputVector that should be processed.  Must have the same dimensionality as the PreProcessing module
	 @return true if the data was processed, false otherwise
     */
    virtual bool process(const VectorDouble &inputVector);
    
    /**
     Sets the PreProcessing reset function, overwriting the base PreProcessing function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     This function resets the deadZone by re-initiliazing the instance.
     
	 @return true if the filter was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This saves the current settings of the DeadZone to a file.
     This overrides the saveModelToFile function in the PreProcessing base class.
     
     @param string filename: the name of the file to save the settings to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the current settings of the DeadZone to a file.
     This overrides the saveModelToFile function in the PreProcessing base class.
     
     @param fstream &file: a reference to the file the settings will be saved to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads the DeadZone settings from a file.
     This overrides the loadModelFromFile function in the PreProcessing base class.
     
     @param string filename: the name of the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads the DeadZone settings from a file.
     This overrides the loadModelFromFile function in the PreProcessing base class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);

	/**
     Initializes the instance, sets the lower and upper limits of the dead-zone region and the dimensionality of the input data.

	 @param double lowerLimit: sets the lower limit of the dead-zone region
	 @param UINT upperLimit: sets the upper limit of the dead-zone region
     @param UINT numDimensions: the dimensionality of the input data
	 @return true if the instance was initiliazed, false otherwise
     */
    bool init(double lowerLimit,double upperLimit,UINT numDimensions);

	/**
     Filters the value x using the dead-zone values, this should only be called if the dimensionality of the instance was set to 1.
     
     @param const double x: the value to be filtered, this should only be called if the dimensionality of the filter was set to 1
	 @return the filtered input value.  Zero will be returned if the value was not computed
     */
	double filter(const double x);
	
	/**
     Filters x using the dead-zone values, the dimensionality of the input should match the number of inputs for the dead zone
     
     @param const VectorDouble &x: the values to be filtered, the dimensionality of the input should match the number of inputs for the derivative
	 @return the filtered input values.  An empty vector will be returned if the values were not filtered
     */
    VectorDouble filter(const VectorDouble &x);
    
    /**
     Sets the lower limit of the dead-zone region.
     
     @param double lowerLimit: the new lower limit for the dead zone
	 @return returns true if the lowerLimit value was set, false otherwise
     */
	bool setLowerLimit(double lowerLimit);

	/**
     Sets the upper limit of the dead-zone region.
     
     @param double upperLimit: the new upper limit for the dead zone
	 @return returns true if the upperLimit value was set, false otherwise
     */
	bool setUpperLimit(double upperLimit);
    
    /**
     Gets the lower limit of the dead-zone region.
     
	 @return returns the lower limit if the DeadZone has been initialized, zero otherwise
     */
    double getLowerLimit(){ if( initialized ){ return lowerLimit; } return 0; }

	/**
     Gets the upper limit of the dead-zone region.
     
	 @return returns the upper limit if the DeadZone has been initialized, zero otherwise
     */
    double getUpperLimit(){ if( initialized ){ return upperLimit; } return 0; }

protected:
    double lowerLimit;				///< The lower limit of the dead-zone region
    double upperLimit;				///< The upper limit of the dead-zone region
    
    static RegisterPreProcessingModule< DeadZone > registerModule;
	
};

}//End of namespace GRT

#endif //GRT_DEADZONE_HEADER
