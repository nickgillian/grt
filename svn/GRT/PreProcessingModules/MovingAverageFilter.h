/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief The MovingAverageFilter implements a low pass moving average filter.
 
 @example PreprocessingModulesExamples/MovingAverageFilterExample/MovingAverageFilterExample.cpp
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

#ifndef GRT_MOVING_AVERAGE_FILTER_HEADER
#define GRT_MOVING_AVERAGE_FILTER_HEADER

#include "../CoreModules/PreProcessing.h"

namespace GRT{

class MovingAverageFilter : public PreProcessing {
public:
    /**
     Constructor, sets the size of the moving average filter and the dimensionality of the data it will filter.
	 
     @param UINT filterSize: the size of the moving average filter, should be a value greater than zero. Default filterSize = 5
     @param UINT numDimensions: the dimensionality of the data to filter.  Default numDimensions = 1
     */
    MovingAverageFilter(UINT filterSize = 5,UINT numDimensions = 1);
    
    /**
     Copy Constructor, copies the MovingAverageFilter from the rhs instance to this instance
     
	 @param const MovingAverageFilter &rhs: another instance of the MovingAverageFilter class from which the data will be copied to this instance
     */
    MovingAverageFilter(const MovingAverageFilter &rhs);

    /**
     Default Destructor
     */
    virtual ~MovingAverageFilter();
    
    /**
     Sets the equals operator, copies the data from the rhs instance to this instance
     
	 @param const MovingAverageFilter &rhs: another instance of the MovingAverageFilter class from which the data will be copied to this instance
	 @return a reference to this instance of MovingAverageFilter
     */
    MovingAverageFilter& operator=(const MovingAverageFilter &rhs);
    
    /**
     Sets the PreProcessing deepCopyFrom function, overwriting the base PreProcessing function.
     This function is used to deep copy the values from the input pointer to this instance of the PreProcessing module.
     This function is called by the GestureRecognitionPipeline when the user adds a new PreProcessing module to the pipeline.
     
	 @param const PreProcessing *preProcessing: a pointer to another instance of a MovingAverageFilter, the values of that instance will be cloned to this instance
	 @return true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const PreProcessing *preProcessing);
    
    /**
     Sets the PreProcessing process function, overwriting the base PreProcessing function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function calls the MovingAverageFilter's filter function.
     
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
     This saves the current settings of the MovingAverageFilter to a file.
     This overrides the saveModelToFile function in the PreProcessing base class.
     
     @param string filename: the name of the file to save the settings to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the current settings of the MovingAverageFilter to a file.
     This overrides the saveModelToFile function in the PreProcessing base class.
     
     @param fstream &file: a reference to the file the settings will be saved to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads the MovingAverageFilter settings from a file.
     This overrides the loadModelFromFile function in the PreProcessing base class.
     
     @param string filename: the name of the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads the MovingAverageFilter settings from a file.
     This overrides the loadModelFromFile function in the PreProcessing base class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    /**
     Initializes the filter, setting the filter size and dimensionality of the data it will filter.
     Sets all the filter values to zero.
     
     @param UINT filterSize: the size of the moving average filter, should be a value greater than zero
	 @return true if the filter was initiliazed, false otherwise
     */
    bool init(UINT filterSize,UINT numDimensions);
    
    /**
     Filters the input, this should only be called if the dimensionality of the filter was set to 1.
     
     @param double x: the value to filter, this should only be called if the dimensionality of the filter was set to 1
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
     Gets the current filter size.
     
     @return returns the filter size
     */
	UINT getFilterSize() const { return filterSize; }
    
    /**
     Returns the last value(s) that were filtered.
     
	 @return the filtered values.  An empty vector will be returned if the values were not filtered
     */
    VectorDouble getFilteredData() const { return processedData; }
    
protected:
    UINT filterSize;                                        ///< The size of the filter
    UINT inputSampleCounter;                                ///< A counter to keep track of the number of input samples
    CircularBuffer< VectorDouble > dataBuffer;          	///< A buffer to store the previous N values, N = filterSize
    
    static RegisterPreProcessingModule< MovingAverageFilter > registerModule;
};

}//End of namespace GRT

#endif //GRT_MOVING_AVERAGE_FILTER_HEADER
