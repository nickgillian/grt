/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief The Class Label Filter is a useful post-processing module which can remove erroneous or sporadic prediction spikes that may be made by a classifier on a continuous input stream of data.

For instance, imagine a classifier that correctly outputs the predicted class label of 1 for a large majority of the time that a user is performing gesture 1, but every so often (perhaps due to sensor noise), the classifier outputs the class label of 2. In this instance the class label filter can be used to remove these sporadic prediction values, with the output of the class label filter in this instance being 1.

The Class Label Filter module is controlled through two parameters: the minimum count value and buffer size value. The minimum count sets the minimum number of class label values that must be present in the class labels buffer for that class label value to be output by the Class Label Filter. The size of the class labels buffer is set by the buffer size parameter. If there is more than one type of class label in the buffer then the class label with the maximum number of instances will be output. If the maximum number of instances for any class label in the buffer is less than the minimum count parameter then the Class Label Filter will output the default null rejection class label of 0. The minimum count and buffer size parameters can be set using the setMinimumCount(UINT minimumCount) and setBufferSize(UINT bufferSize) methods.
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

#ifndef GRT_CLASS_LABEL_FILTER_HEADER
#define GRT_CLASS_LABEL_FILTER_HEADER

#include "../Util/GRTCommon.h"
#include "../CoreModules/PostProcessing.h"

GRT_BEGIN_NAMESPACE

class GRT_API ClassLabelFilter : public PostProcessing{
public:
    /**
    Default Constructor. Sets the minimumCount and bufferSize parameters. The minimum count sets the minimum number of class
    label values that must be present in the class labels buffer for that class label value to be output by the Class Label
    Filter. The size of the class labels buffer is set by the buffer size parameter. If there is more than one type of class
    label in the buffer then the class label with the maximum number of instances will be output. If the maximum number of
    instances for any class label in the buffer is less than the minimum count parameter then the Class Label Filter will
    output the default null rejection class label of 0.
    
    @param minimumCount: sets the minimumCount value. Default value minimumCount=1
    @param bufferSize: sets the size of the class labels buffer. Default value bufferSize=1
    */
    ClassLabelFilter(const UINT minimumCount = 1,const UINT bufferSize = 1);
    
    /**
    Copy Constructor.
    
    Copies the values from the rhs ClassLabelFilter to this instance of the ClassLabelFilter.
    
    @param rhs: the rhs from which the values will be copied to this this instance of the ClassLabelFilter.
    */
    ClassLabelFilter(const ClassLabelFilter &rhs);
    
    /**
    Default Destructor
    */
    virtual ~ClassLabelFilter();
    
    /**
    Assigns the equals operator setting how the values from the rhs instance will be copied to this instance.
    
    @param rhs: the rhs instance from which the values will be copied to this this instance of the ClassLabelFilter
    @return returns a reference to this instance of the ClassLabelFilter
    */
    ClassLabelFilter& operator=(const ClassLabelFilter &rhs);
    
    /**
    Sets the PostProcessing deepCopyFrom function, overwriting the base PostProcessing function.
    This function is used to deep copy the values from the input pointer to this instance of the PostProcessing module.
    This function is called by the GestureRecognitionPipeline when the user adds a new PostProcessing module to the pipeline.
    
    @param postProcessing: a pointer to another instance of a ClassLabelFilter, the values of that instance will be cloned to this instance
    @return true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const PostProcessing *postProcessing);
    
    /**
    Sets the PostProcessing process function, overwriting the base PostProcessing function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This function calls the ClassLabelFilter's filter(...) function.
    
    @param inputVector: the inputVector that should be processed.  This should be a 1-dimensional vector containing a predicted class label
    @return true if the data was processed, false otherwise
    */
    virtual bool process(const VectorDouble &inputVector);
    
    /**
    Sets the PostProcessing reset function, overwriting the base PostProcessing function.
    This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
    This function resets the ClassLabelFilter by re-initiliazing the instance.
    
    @return true if the ClassLabelFilter was reset, false otherwise
    */
    virtual bool reset();
    
    /**
    This saves the post processing settings to a file.
    This overrides the save function in the PostProcessing base class.
    
    @param filename: the name of the file to save the settings to
    @return returns true if the settings were saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads the post processing settings from a file.
    This overrides the loadSettingsFromFile function in the PostProcessing base class.
    
    @param filename: the name of the file to load the settings from
    @return returns true if the settings were loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This function initializes the ClassLabelFilter.
    
    @param minimumCount: sets the minimumCount value
    @param bufferSize: sets the size of the class labels buffer
    @return returns true if the ClassLabelFilter was initialized, false otherwise
    */
    bool init(const UINT minimumCount,const UINT bufferSize);
    
    /**
    This is the main filter function which filters the input predictedClassLabel.
    
    @param predictedClassLabel: the predictedClassLabel which should be filtered
    return returns the filtered class label
    */
    UINT filter(const UINT predictedClassLabel);
    
    /**
    Get the most recently filtered class label value.
    
    @return returns the filtered class label
    */
    UINT getFilteredClassLabel() const { return filteredClassLabel; }
    
    /**
    Sets the minimumCount parameter.
    
    The minimumCount parameter controls how many class labels need to be present in the class labels buffer for that class label to be output by
    the filter.  If the Class Label Filter has been initialized then the module will be reset.
    
    @param minimumCount: the new minimumCount parameter
    @return returns true if the minimumCount parameter was updated, false otherwise
    */
    bool setMinimumCount(const UINT minimumCount);
    
    /**
    Sets the bufferSize parameter.
    
    The bufferSize parameter controls the size of the class labels buffer. If the Class Label Filter has been initialized then the module will be reset.
    
    @param bufferSize: the new bufferSize parameter
    @return returns true if the bufferSize parameter was updated, false otherwise
    */
    bool setBufferSize(const UINT bufferSize);

    /**
    Gets a string that represents the ID of this class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    
protected:
    UINT filteredClassLabel;            ///< The most recent filtered class label value
    UINT minimumCount;                  ///< The minimum count sets the minimum number of class label values that must be present in the class labels buffer for that class label value to be output by the Class Label Filter
    UINT bufferSize;                    ///< The size of the Class Label Filter buffer
    CircularBuffer< UINT > buffer;      ///< The class label filter buffer
    
private:
    static const std::string id;  
    static RegisterPostProcessingModule< ClassLabelFilter > registerModule;
};

GRT_END_NAMESPACE

#endif //GRT_CLASS_LABEL_FILTER_HEADER
