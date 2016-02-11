/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 
 @brief The Class Likelihood Filter can be used to smooth predictions made by a classifier on a continuous stream of data.
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

#ifndef GRT_CLASS_LIKELIHOOD_FILTER_HEADER
#define GRT_CLASS_LIKELIHOOD_FILTER_HEADER

#include "../Util/GRTCommon.h"
#include "../CoreModules/PostProcessing.h"

GRT_BEGIN_NAMESPACE

class ClassLikelihoodFilter : public PostProcessing{
public:
    /**
     Default Constructor.
    
     @param filterSize: sets the size of the class likelihood filter. Default value bufferSize=10
     */
    ClassLikelihoodFilter(UINT filterSize = 10);
    
    /**
     Copy Constructor.
     
     Copies the values from the rhs ClassLikelihoodFilter to this instance of the ClassLikelihoodFilter.
     
     @param rhs: the rhs from which the values will be copied to this this instance of the ClassLikelihoodFilter.
     */
    ClassLikelihoodFilter(const ClassLikelihoodFilter &rhs);
    
    /**
     Default Destructor
     */
    virtual ~ClassLikelihoodFilter();
    
    /**
     Assigns the equals operator setting how the values from the rhs instance will be copied to this instance.
     
     @param rhs: the rhs instance from which the values will be copied to this this instance of the ClassLikelihoodFilter
     @return returns a reference to this instance of the ClassLikelihoodFilter
     */
    ClassLikelihoodFilter& operator=(const ClassLikelihoodFilter &rhs);
    
    /**
     Sets the PostProcessing deepCopyFrom function, overwriting the base PostProcessing function.
     This function is used to deep copy the values from the input pointer to this instance of the PostProcessing module.
     This function is called by the GestureRecognitionPipeline when the user adds a new PostProcessing module to the pipeline.
     
     @param postProcessing: a pointer to another instance of a ClassLikelihoodFilter, the values of that instance will be cloned to this instance
	 @return true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const PostProcessing *postProcessing);
    
    /**
     Sets the PostProcessing process function, overwriting the base PostProcessing function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function calls the ClassLikelihoodFilter's filter(...) function.
     
     @param inputVector: the inputVector that should be processed, this should contain the likelihoods
	 @return true if the data was processed, false otherwise
     */
    virtual bool process(const VectorDouble &inputVector);
    
    /**
     Sets the PostProcessing reset function, overwriting the base PostProcessing function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     This function resets the ClassLikelihoodFilter by re-initiliazing the instance.
     
     @return true if the ClassLikelihoodFilter was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This saves the post processing settings to a file.
     This overrides the saveSettingsToFile function in the PostProcessing base class.
     
     @param filename: the name of the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile( std::string filename ) const;
    
    /**
     This saves the post processing settings to a file.
     This overrides the saveSettingsToFile function in the PostProcessing base class.
     
     @param filename: the name of the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile( std::fstream &file ) const;
    
    /**
     This loads the post processing  settings from a file.
     This overrides the loadSettingsFromFile function in the PostProcessing base class.
     
     @param filename: the name of the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile( std::string filename );
    
    /**
     This loads the post processing settings from a file.
     This overrides the loadSettingsFromFile function in the PostProcessing base class.
     
     @param filename: the name of the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile( std::fstream &file );
    
    /**
     This function initializes the ClassLikelihoodFilter.
     
     @param bufferSize: sets the size of the class labels buffer
     @return returns true if the ClassLikelihoodFilter was initialized, false otherwise
     */
    bool init(UINT bufferSize); 
    
    /**
     This is the main filter function which filters the input predictedClassLabel.
     
     @param predictedClassLabel: the predictedClassLabel which should be filtered
     return returns the filtered class label
     */
    UINT filter(UINT predictedClassLabel);
    
    /**
     Get the most recently filtered class label value.
     
     @return returns the filtered class label
     */
    UINT getFilteredClassLabel(){ return filteredClassLabel; }
    
    /**
     Sets the minimumCount parameter.
     
     The minimumCount parameter controls how many class labels need to be present in the class labels buffer for that class label to be output by 
     the filter.  If the Class Label Filter has been initialized then the module will be reset.
     
     @param minimumCount: the new minimumCount parameter
     @return returns true if the minimumCount parameter was updated, false otherwise
     */
    bool setMinimumCount(UINT minimumCount);
    
    /**
     Sets the bufferSize parameter.
     
     The bufferSize parameter controls the size of the class labels buffer. If the Class Label Filter has been initialized then the module will be reset.
     
     @param bufferSize: the new bufferSize parameter
     @return returns true if the bufferSize parameter was updated, false otherwise
     */
    bool setBufferSize(UINT bufferSize);
    
protected:
    UINT filteredClassLabel;            ///< The most recent filtered class label value
    UINT bufferSize;                    ///< The size of the Class Label Filter buffer
    Float filteredLikelihood;           ///< The likelihood of the filtered class label
    WeightedAverageFilter filter;  ///< The filter
    
    static RegisterPostProcessingModule< ClassLikelihoodFilter > registerModule;
};
    
GRT_END_NAMESPACE

#endif //header guard
