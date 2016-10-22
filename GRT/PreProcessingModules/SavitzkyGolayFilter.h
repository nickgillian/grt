/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0
@example PreprocessingModulesExamples/SavitzkyGolayFilterExample/SavitzkyGolayFilterExample.cpp
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

#ifndef GRT_SAVITZKY_GOLAY_FILTER_HEADER
#define GRT_SAVITZKY_GOLAY_FILTER_HEADER

#include "../CoreModules/PreProcessing.h"
#include "../Util/LUDecomposition.h"

GRT_BEGIN_NAMESPACE

/**
 @brief This implements a Savitzky-Golay filter. This code is based on the Savitzky Golay filter code from Numerical Recipes 3.
*/
class GRT_API SavitzkyGolayFilter : public PreProcessing{
public:
    /**
    Constructor, sets the number of left and right points of the filter, the derivative and polynomial smoothing order, and the dimensionality of the input data.
    
    @param numLeftHandPoints: the number of left hand points used to design the filter, should be a value greater than zero. Default numLeftHandPoints = 10
    @param numRightHandPoints: the number of right hand points used to design the filter, should be a value greater than zero. Default numRightHandPoints = 10
    @param derivativeOrder: the derivative order of the filter.  Default derivativeOrder = 0
    @param smoothingPolynomialOrder: the order of the polynomial used to design the filter, should be either 2 or 4.  Default: smoothingPolynomialOrder = 2
    @param numDimensions: the dimensionality of the data to filter.  Default numDimensions = 1
    */
    SavitzkyGolayFilter(const UINT numLeftHandPoints=10,const UINT numRightHandPoints=10,const UINT derivativeOrder=0,const UINT smoothingPolynomialOrder=2,const UINT numDimensions = 1);
    
    /**
    Copy Constructor, copies the SavitzkyGolayFilter from the rhs instance to this instance
    
    @param rhs: another instance of the SavitzkyGolayFilter class from which the data will be copied to this instance
    */
    SavitzkyGolayFilter(const SavitzkyGolayFilter &rhs);
    
    /**
    Default Destructor
    */
    virtual ~SavitzkyGolayFilter();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance
    
    @param rhs: another instance of the SavitzkyGolayFilter class from which the data will be copied to this instance
    @return a reference to this instance of SavitzkyGolayFilter
    */
    SavitzkyGolayFilter& operator=(const SavitzkyGolayFilter &rhs);
    
    /**
    Sets the PreProcessing deepCopyFrom function, overwriting the base PreProcessing function.
    This function is used to deep copy the values from the input pointer to this instance of the PreProcessing module.
    This function is called by the GestureRecognitionPipeline when the user adds a new PreProcessing module to the pipeline.
    
    @param preProcessing: a pointer to another instance of a SavitzkyGolayFilter, the values of that instance will be cloned to this instance
    @return true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const PreProcessing *preProcessing);
    
    /**
    Sets the PreProcessing process function, overwriting the base PreProcessing function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This function calls the SavitzkyGolayFilter's filter function.
    
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
    This saves the current settings of the SavitzkyGolayFilter to a file.
    This overrides the save function in the PreProcessing base class.
    
    @param file: a reference to the file the settings will be saved to
    @return returns true if the settings were saved successfully, false otherwise
    */
    virtual bool save(std::fstream &file) const;
    
    /**
    This loads the SavitzkyGolayFilter settings from a file.
    This overrides the load function in the PreProcessing base class.
    
    @param file: a reference to the file to load the settings from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load(std::fstream &file);
    
    /**
    Initializes the filter, computes the filter coefficients and sets dimensionality of the data it will filter.
    Sets all the filter values to zero.
    
    @return true if the filter was reset, false otherwise
    */
    bool init(const UINT numLeftHandPoints,const UINT numRightHandPoints,const UINT derivativeOrder,const UINT smoothingPolynomialOrder,const UINT numDimensions);
    
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
    Returns the last value(s) that were filtered.
    
    @return the filtered values.  An empty vector will be returned if the values were not filtered
    */
    VectorFloat getFilteredData() const;

    /**
    Gets a string that represents the ID of this class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    
protected:
    inline int min_(int a,int b) {return b < a ? (b) : (a);}
    inline Float min_(Float a,Float b) {return b < a ? (b) : (a);}
    bool calCoeff();
    
    UINT numPoints;                              //The physical length of the output array
    UINT numLeftHandPoints;                      //Num of leftward (past) points to use
    UINT numRightHandPoints;                     //Num of rightward (future) points to use
    UINT derivativeOrder;                        //Order of the derivative desired
    UINT smoothingPolynomialOrder;               //Order of smoothing polynomial
    CircularBuffer< VectorFloat > data;    //A buffer to hold the input data
    VectorFloat yy;                       //The filtered values
    VectorFloat coeff;                    //Buffer for the filter coefficients
    
private:
    static const std::string id;   
    static RegisterPreProcessingModule< SavitzkyGolayFilter > registerModule;
    
};

GRT_END_NAMESPACE

#endif //GRT_SAVITZKY_GOLAY_FILTER_HEADER
    