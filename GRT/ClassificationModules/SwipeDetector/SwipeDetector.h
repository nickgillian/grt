/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
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

#ifndef GRT_SWIPE_DETECTION_HEADER
#define GRT_SWIPE_DETECTION_HEADER

#include "../../CoreModules/Classifier.h"
#include "../../Util/ThresholdCrossingDetector.h"
#include "../../PreProcessingModules/MedianFilter.h"

GRT_BEGIN_NAMESPACE

/**
 @brief This class implements a basic swipe detection classification algorithm.
 
 @remark This implementation is custom algorithm.
*/
class GRT_API SwipeDetector : public Classifier
{
public:
    enum SwipeDirections{POSITIVE_SWIPE=0,NEGATIVE_SWIPE};
    
    /**
     Default Constructor
     
     @param useScaling: sets if the training and prediction data should be scaled to a specific range.  Default value is useScaling = false
     */
	SwipeDetector(const unsigned int swipeIndex = 0, const unsigned int swipeThreshold = 100,
                   const unsigned int hysteresisThreshold =0, const unsigned int swipeDirection = POSITIVE_SWIPE,bool useScaling=false);
    
    /**
     Defines the copy constructor.
     
     @param rhs: the instance from which all the data will be copied into this instance
     */
    SwipeDetector(const SwipeDetector &rhs);
    
    /**
     Default Destructor
     */
	virtual ~SwipeDetector(void);
    
    /**
     Defines how the data from the rhs SwipeDetector should be copied to this SwipeDetector
     
     @param rhs: another instance of a SwipeDetector
     @return returns a pointer to this instance of the SwipeDetector
     */
	SwipeDetector &operator=(const SwipeDetector &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.  
     It clones the data from the Base Class Classifier pointer (which should be pointing to an SwipeDetector instance) into this instance
     
     @param classifier: a pointer to the Classifier Base Class, this should be pointing to another SwipeDetector instance
     @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    bool init(const unsigned int numInputDimensions);
    
    /**
     This trains the SwipeDetector model, using the labelled classification data.
     This overrides the train function in the Classifier base class.
     
     @param trainingData: a reference to the training data
     @return returns true if the SwipeDetector model was trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the Classifier base class.
     
     @param inputVector: the input vector to classify
     @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorDouble &inputVector);
    
    /**
     This overrides the clear function in the Classifier base class.
     It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
     
     @return returns true if the module was cleared succesfully, false otherwise
     */
    virtual bool clear();
    
    /**
     This overrides the reset function in the Classifier base class.
     
     @return returns true if the module was reset succesfully, false otherwise
     */
    virtual bool reset();
    
    /**
     This saves the trained SwipeDetector model to a file.
     This overrides the save function in the Classifier base class.
     
     @param file: a reference to the file the SwipeDetector model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool save( std::fstream &file ) const;
    
    /**
     This loads a trained SwipeDetector model from a file.
     This overrides the load function in the Classifier base class.
     
     @param file: a reference to the file the SwipeDetector model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool load( std::fstream &file );
    
    /**
     @return returns true if a swipe was detected, false otherwise
     */
    bool getSwipeDetected() const;
    
    /**
     @return returns the swipe analysis value, this is the value that the swipe threshold is matched against
     */
    Float getSwipeValue() const;
    
    /**
     @return returns the swipe threshold, this is the value that the swipe analysis value must cross for a swipe to be trigger
     */
    Float getSwipeThreshold() const ;
    
    /**
     @return returns the hysteresis threshold, this is the value that the swipe analysis value must cross (after a valid swipe detection), before the next swipe will be detected
     */
    Float getHysteresisThreshold() const;
    
    /**
     This function returns the movement velocity.  This is the integrated difference between each of the additional inputs (that are not the swipe index) that is used to detected a valid swipe.
     The movement velocity must be below the movement threshold for a valid swipe to be detected.
     @return returns the movement velocity
     */
    Float getMovementVelocity() const;
    
    /**
     @return returns the movement threshold, the movement velocity must be below this value for a valid swipe to be detected
     */
    Float getMovementThreshold() const;
    
    /**
     @return returns the context value, this is the latest output of the context filter
     */
    Float getContextValue() const;
    
    /**
     @return returns the swipe integration coeff
     */
    Float getSwipeIntegrationCoeff() const;
    
    /**
     This function lets you set the current context input.  This should be called before you call the predict function.
     @return returns true if the context value was updated, false otherwise
     */
    bool setContext(const bool context);
    
    /**
     This function lets you set the swipe index, this is the index in the input vector that contains the main axis you want to use for the swipe detection.
     The other elements in the input vector will be combined to compute the movement index.
     The swipeIndex should be a valid index in your input vector.
     @param swipeIndex: the index in the input vector you want to use for the swipe detection
     @return returns true if the parameter was updated, false otherwise
     */
    bool setSwipeIndex(const unsigned int swipeIndex);
    
    /**
     This function lets you set the swipe direction, this should be one of the SwipeDirections enums.
     @param swipeDirection: the direction you want to search for a swipe in
     @return returns true if the parameter was updated, false otherwise
     */
    bool setSwipeDirection(const unsigned int swipeDirection);
    
    /**
     This function lets you set the swipe threshold, this is the threshold that the swipe analysis value must pass for a swipe to be detected.
     
     @param swipeThreshold: the new swipe threshold
     @return returns true if the parameter was updated, false otherwise
     */
    bool setSwipeThreshold(const Float swipeThreshold);
    
    /**
     This function lets you set the swipe hysteresisThreshold, this is the threshold that the swipe analysis value must pass after a valid swipe before a new swipe can be detected.
     
     @param hysteresisThreshold: the new hysteresis threshold
     @return returns true if the parameter was updated, false otherwise
     */
    bool setHysteresisThreshold(const Float hysteresisThreshold);
    
    /**
     This function lets you set the swipe movementThreshold.  The movement index must be below the movementThreshold for 
     a valid swipe to be detected.
     
     @param movementThreshold: the new movement threshold
     @return returns true if the parameter was updated, false otherwise
     */
    bool setMovementThreshold(const Float movementThreshold);
    
    /**
     This function lets you set the swipe integration coeff.  This controls how much 'memory' is used to when integrating the input data.
     
     @param swipeIntegrationCoeff: the new integration coefficient
     @return returns true if the parameter was updated, false otherwise
     */
    bool setSwipeIntegrationCoeff(const Float swipeIntegrationCoeff);

    /**
    Gets a string that represents the SwipeDetector class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();

    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_;
    using MLBase::predict_;

protected:
    
    bool firstSample;
    bool swipeDetected;
    bool contextInput;
    unsigned int swipeIndex;
    unsigned int swipeDirection;
    unsigned int contextFilterSize;
    Float swipeIntegrationCoeff;
    Float movementIntegrationCoeff;
    Float swipeThreshold;
    Float hysteresisThreshold;
    Float swipeVelocity;
    Float movementVelocity;
    Float movementThreshold;
    Float contextFilteredValue;
    VectorFloat lastX;
    GRT::ThresholdCrossingDetector thresholdDetector;
    GRT::MedianFilter contextFilter;

private:
    static const std::string id;
    static RegisterClassifierModule< SwipeDetector > registerModule;
};

GRT_END_NAMESPACE

#endif //GRT_SWIPE_DETECTION_HEADER

