/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief The LeakyIntegrator class computes the following signal: y = y*z + x, where x is the input, y is the output and z is the leakrate.
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

#ifndef GRT_LEAKY_INTEGRATOR_HEADER
#define GRT_LEAKY_INTEGRATOR_HEADER

#include "../CoreModules/PreProcessing.h"

namespace GRT{
    
class LeakyIntegrator : public PreProcessing{
public:
    /**
     Constructor, sets the leak rate and the dimensionality of the input data.
	 
     @param const double leakRate: sets the leak rate of the filter, this should be in the range [0 1].  Default delta = 0.99
     @param const UINT numDimensions: the dimensionality of the input data.  Default numDimensions = 1
     */
    LeakyIntegrator(const double leakRate = 0.99,const UINT numDimensions = 1);
    
    /**
     Copy Constructor, copies the LeakyIntegrator from the rhs instance to this instance
     
	 @param const LeakyIntegrator &rhs: another instance of the LeakyIntegrator class from which the data will be copied to this instance
     */
    LeakyIntegrator(const LeakyIntegrator &rhs);
    
    /**
     Default Destructor
     */
	virtual ~LeakyIntegrator();
    
    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const LeakyIntegrator &rhs: another instance of the LeakyIntegrator class from which the data will be copied to this instance
	 @return a reference to this instance of LeakyIntegrator
     */
    LeakyIntegrator& operator=(const LeakyIntegrator &rhs);
    
    /**
     Sets the PreProcessing deepCopyFrom function, overwriting the base PreProcessing function.
     This function is used to clone the values from the input pointer to this instance of the PreProcessing module.
     This function is called by the GestureRecognitionPipeline when the user adds a new PreProcessing module to the pipeline.
     
	 @param const PreProcessing *preProcessing: a pointer to another instance of a LeakyIntegrator, the values of that instance will be cloned to this instance
	 @return true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const PreProcessing *preProcessing);
    
    /**
     Sets the PreProcessing process function, overwriting the base PreProcessing function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function calls the LeakyIntegrator's computeLeakyIntegrator function.
     
	 @param const VectorDouble &inputVector: the inputVector that should be processed.  Must have the same dimensionality as the PreProcessing module
	 @return true if the data was processed, false otherwise
     */
    virtual bool process(const VectorDouble &inputVector);
    
    /**
     Sets the PreProcessing reset function, overwriting the base PreProcessing function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     This function resets the LeakyIntegrator values by re-initiliazing the filter.
     
	 @return true if the LeakyIntegrator was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This saves the current settings of the LeakyIntegrator to a file.
     This overrides the saveModelToFile function in the PreProcessing base class.
     
     @param fstream &file: a reference to the file the settings will be saved to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads the LeakyIntegrator settings from a file.
     This overrides the loadModelFromFile function in the PreProcessing base class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);

    /**
     Initializes the instance, sets the leak rate and the dimensionality of the input data.
	 
     @param const double leakRate: sets the leak rate of the filter, this should be in the range [0 1].  Default delta = 0.99
     @param const UINT numDimensions: the dimensionality of the input data.  Default numDimensions = 1
	 @return true if the instance was initiliazed, false otherwise
     */
    bool init(const double leakRate,const UINT numDimensions);
    
    /**
     Computes the LeakyIntegrator of the input, this should only be called if the dimensionality of the instance was set to 1.
     
     @param double x: the value to compute the LeakyIntegrator of, this should only be called if the dimensionality of the filter was set to 1
	 @return the LeakyIntegrator of the input.  Zero will be returned if the value was not computed
     */
	double update(const double x);
    
    /**
     Computes the LeakyIntegrator of the input, the dimensionality of the input should match the number of inputs for the LeakyIntegrator
     
     @param const VectorDouble &x: the values to compute the LeakyIntegrator of, the dimensionality of the input should match the number of inputs for the LeakyIntegrator
	 @return the LeakyIntegrators of the input.  An empty vector will be returned if the values were not filtered
     */
    VectorDouble update(const VectorDouble &x);
    
    /**
     Sets the leak rate.  This should be in the range [0 1].
     Setting delta will re-initialize this instance.
     
     @param double leakRate: the new leak rate value
	 @return returns true if delta was set, false otherwise
     */
    bool setLeakRate(const double leakRate);
    
    /**
     Gets the leak rate.
     
	 @return returns the current leak rate
     */
    double getLeakRate();
    
    using PreProcessing::saveModelToFile;
    using PreProcessing::loadModelFromFile;

protected:
    double leakRate;                        ///< The current leak rate
	VectorDouble y;                        ///< A buffer holding the previous input value(s)
    
    static RegisterPreProcessingModule< LeakyIntegrator > registerModule;
    
};

}//End of namespace GRT

#endif //GRT_LeakyIntegrator_HEADER
