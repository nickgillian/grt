/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief The Class Label Timeout Filter is a useful post-processing module which debounces a gesture (i.e. it stops a single gesture from being recognized multiple times over a short time frame). For instance, it is normally the case that whenever a user performs a gesture, such as a swipe gesture for example, that the recognition system may recognize this single gesture several times because the user's movements are being sensed at a high sample rate (i.e. 100Hz). The Class Label Timeout Filter can be used to ensure that a gesture, such as the previous swipe gesture example, is only recognize once within any given timespan.

The Class Label Timeout Filter module is controlled through two parameters: the timeoutDuration parameter and filterMode parameter. The timeoutDuration sets how long (in milliseconds) the Class Label Timeout Filter will debounce valid predicted class labels for, after it has viewed the first valid predicted class label which triggers the debounce mode to be activated. A valid predicted class label is simply a predicted class label that is not the default null rejection class label (i.e. a label with the class value of 0). The filterMode parameter sets how the Class Label Timeout Filter reacts to different predicted class labels (different from the predicted class label that triggers the debounce mode to be activated). There are two options for the filterMode: ALL_CLASS_LABELS and INDEPENDENT_CLASS_LABELS

In the ALL_CLASS_LABELS filterMode, after the debounce mode has been activated, all class labels will be ignored until the current timeoutDuration period has elapsed, regardless of which class actually triggered the timeout. Alternatively, in the INDEPENDENT_CLASS_LABELS mode, the debounce mode will be reset if a different predicted class label is detected that is different from the predicted class label that initially triggered the debounce mode to be activated. For instance, if the debounce mode was activated with the class label of 1, and then class 2 was input into the class label filter, then the debounce mode would be reset to class 2, even if the timeoutDuration for class 1 had not expired.

The timeoutDuration and filterMode parameters can be set using the setTimeoutDuration(double timeoutDuration) and setFilterMode(UINT filterMode) methods.
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

#ifndef GRT_CLASS_LABEL_TIMEOUT_FILTER_HEADER
#define GRT_CLASS_LABEL_TIMEOUT_FILTER_HEADER

#include "../Util/GRTCommon.h"
#include "../CoreModules/PostProcessing.h"

GRT_BEGIN_NAMESPACE

class GRT_API ClassLabelAndTimer{
    public:
    ClassLabelAndTimer(){
        classLabel = 0;
    }
    ClassLabelAndTimer(UINT classLabel,unsigned long timeoutDuration){
        this->classLabel = classLabel;
        timer.start(timeoutDuration);
    }
    
    //Setters
    bool set(UINT classLabel,unsigned long timeoutDuration){
        if( classLabel > 0 && timeoutDuration > 0 ){
            this->classLabel = classLabel;
            timer.start(timeoutDuration);
            return true;
        }
        return false;
    }
    
    //Getters
    UINT getClassLabel(){ return classLabel; }
    bool timerReached(){ return timer.timerReached(); }
    unsigned long getRemainingTime(){ return timer.getMilliSeconds(); }
    
    protected:
    UINT classLabel;
    Timer timer;
};

class GRT_API ClassLabelTimeoutFilter : public PostProcessing{
public:
    enum FilterModes{ALL_CLASS_LABELS=0,INDEPENDENT_CLASS_LABELS};

    /**
    Default Constructor. Sets the timeoutDuration and filterMode parameters.
    
    The timeoutDuration sets how long (in milliseconds) the Class Label Timeout Filter will debounce valid
    predicted class labels for, after it has viewed the first valid predicted class label which triggers the
    debounce mode to be activated. A valid predicted class label is simply a predicted class label that is not
    the default null rejection class label (i.e. a label with the class value of 0). The filterMode parameter
    sets how the Class Label Timeout Filter reacts to different predicted class labels (different from the
    predicted class label that triggers the debounce mode to be activated). There are two options for the filterMode:
    ALL_CLASS_LABELS or INDEPENDENT_CLASS_LABELS.  In the ALL_CLASS_LABELS filterMode, after the debounce mode
    has been activated, all class labels will be ignored until the current timeoutDuration period has elapsed,
    regardless of which class actually triggered the timeout. Alternatively, in the INDEPENDENT_CLASS_LABELS mode,
    the debounce mode will be reset if a different predicted class label is detected that is different from the
    predicted class label that initially triggered the debounce mode to be activated. For instance, if the debounce
    mode was activated with the class label of 1, and then class 2 was input into the class label filter, then the
    debounce mode would be reset to class 2, even if the timeoutDuration for class 1 had not expired.
    
    @param timeoutDuration: sets the timeoutDuration value (in milliseconds). Default value timeoutDuration=1000
    @param filterMode: sets the filterMode parameter. Default value filterMode=ALL_CLASS_LABELS
    */
    ClassLabelTimeoutFilter(const unsigned long timeoutDuration = 1000,const UINT filterMode = ALL_CLASS_LABELS);
    
    /**
    Copy Constructor.
    
    Copies the values from the rhs ClassLabelTimeoutFilter to this instance of the ClassLabelTimeoutFilter.
    
    @param rhs: the rhs from which the values will be copied to this this instance of the ClassLabelTimeoutFilter
    */
    ClassLabelTimeoutFilter(const ClassLabelTimeoutFilter &rhs);
    
    /**
    Default Destructor
    */
    virtual ~ClassLabelTimeoutFilter();
    
    /**
    Assigns the equals operator setting how the values from the rhs instance will be copied to this instance.
    
    @param rhs: the rhs instance from which the values will be copied to this this instance of the ClassLabelTimeoutFilter
    @return returns a reference to this instance of the ClassLabelTimeoutFilter
    */
    ClassLabelTimeoutFilter& operator=(const ClassLabelTimeoutFilter &rhs);
    
    /**
    Sets the PostProcessing deepCopyFrom function, overwriting the base PostProcessing function.
    This function is used to deep copy the values from the input pointer to this instance of the PostProcessing module.
    This function is called by the GestureRecognitionPipeline when the user adds a new PostProcessing module to the pipeline.
    
    @param postProcessing: a pointer to another instance of a ClassLabelTimeoutFilter, the values of that instance will be cloned to this instance
    @return true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const PostProcessing *postProcessing);
    
    /**
    Sets the PostProcessing process function, overwriting the base PostProcessing function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This function calls the ClassLabelTimeoutFilter's filter(...) function.
    
    @param inputVector: the inputVector that should be processed.  This should be a 1-dimensional vector containing a predicted class label
    @return true if the data was processed, false otherwise
    */
    virtual bool process(const VectorDouble &inputVector);
    
    /**
    Sets the PostProcessing reset function, overwriting the base PostProcessing function.
    This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
    This function resets the ClassLabelTimeoutFilter by re-initiliazing the instance.
    
    @return true if the ClassLabelTimeoutFilter was reset, false otherwise
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
    This overrides the load function in the PostProcessing base class.
    
    @param filename: the name of the file to load the settings from
    @return returns true if the settings were loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This function initializes the ClassLabelTimeoutFilter.
    
    @param timeoutDuration: sets the timeoutDuration value (in milliseconds)
    @param filterMode: sets the filterMode parameter
    @return returns true if the ClassLabelTimeoutFilter was initialized, false otherwise
    */
    bool init(const unsigned long timeoutDuration,const UINT filterMode = ALL_CLASS_LABELS);
    
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
    Get if the filter is currently ignorning new inputs because the timeout is active.
    
    @return returns true if the timeout mode is active, false otherwise
    */
    bool isTimeoutActive();
    
    /**
    Sets the timeoutDuration parameter, must be a value greater than 0.
    
    The timeoutDuration sets how long (in milliseconds) the Class Label Timeout Filter will debounce valid
    predicted class labels for, after it has viewed the first valid predicted class label which triggers the
    debounce mode to be activated. A valid predicted class label is simply a predicted class label that is not
    the default null rejection class label (i.e. a label with the class value of 0).  If the Class Label Filter
    has been initialized then the module will be reset.
    
    @param timeoutDuration: the new timeoutDuration parameter
    @return returns true if the timeoutDuration parameter was updated, false otherwise
    */
    bool setTimeoutDuration(const unsigned long timeoutDuration);
    
    /**
    Sets the filterMode parameter, must be a value greater than 0.
    
    The filterMode parameter sets how the Class Label Timeout Filter reacts to different predicted class labels
    (different from the predicted class label that triggers the debounce mode to be activated). There are two
    options for the filterMode: ALL_CLASS_LABELS or INDEPENDENT_CLASS_LABELS.  In the ALL_CLASS_LABELS filterMode,
    after the debounce mode has been activated, all class labels will be ignored until the current timeoutDuration
    period has elapsed, regardless of which class actually triggered the timeout. Alternatively, in the
    INDEPENDENT_CLASS_LABELS mode, the debounce mode will be reset if a different predicted class label is detected
    that is different from the predicted class label that initially triggered the debounce mode to be activated.
    
    For instance, if the debounce mode was activated with the class label of 1, and then class 2 was input into the class
    label filter, then the
    debounce mode would be reset to class 2, even if the timeoutDuration for class 1 had not expired.  If the Class Label
    Filter has been initialized then the module will be reset.
    
    @param filterMode: the new filterMode parameter
    @return returns true if the filterMode parameter was updated, false otherwise
    */
    bool setFilterMode(const UINT filterMode);

    /**
    Gets a string that represents the ID of this class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    
protected:
    UINT filteredClassLabel;
    UINT filterMode;
    unsigned long timeoutDuration;
    Vector< ClassLabelAndTimer > classLabelTimers;
    
private:
    static const std::string id;  
    static RegisterPostProcessingModule< ClassLabelTimeoutFilter > registerModule;
};

GRT_END_NAMESPACE

#endif //GRT_CLASS_LABEL_TIMEOUT_FILTER_HEADER
