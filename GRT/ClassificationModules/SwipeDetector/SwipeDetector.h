/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a basic swipe detection classification algorithm.
 
 @remark This implementation is custom algorithm.
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

namespace GRT{

class SwipeDetector : public Classifier
{
public:
    /**
     Default Constructor
     
     @param bool useScaling: sets if the training and prediction data should be scaled to a specific range.  Default value is useScaling = false
     */
	SwipeDetector(const unsigned int swipeIndex = 0, const unsigned int swipeThreshold = 100,
                   const unsigned int hysteresisThreshold =0, const unsigned int swipeDirection = POSITIVE_SWIPE,bool useScaling=false);
    
    /**
     Defines the copy constructor.
     
     @param const SwipeDetector &rhs: the instance from which all the data will be copied into this instance
     */
    SwipeDetector(const SwipeDetector &rhs);
    
    /**
     Default Destructor
     */
	virtual ~SwipeDetector(void);
    
    /**
     Defines how the data from the rhs SwipeDetector should be copied to this SwipeDetector
     
     @param const SwipeDetector &rhs: another instance of a SwipeDetector
     @return returns a pointer to this instance of the SwipeDetector
     */
	SwipeDetector &operator=(const SwipeDetector &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.  
     It clones the data from the Base Class Classifier pointer (which should be pointing to an SwipeDetector instance) into this instance
     
     @param Classifier *classifier: a pointer to the Classifier Base Class, this should be pointing to another SwipeDetector instance
     @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    bool init(const unsigned int numInputDimensions);
    
    /**
     This trains the SwipeDetector model, using the labelled classification data.
     This overrides the train function in the Classifier base class.
     
     @param ClassificationData trainingData: a reference to the training data
     @return returns true if the SwipeDetector model was trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the Classifier base class.
     
     @param VectorDouble inputVector: the input vector to classify
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
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the SwipeDetector model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained SwipeDetector model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the SwipeDetector model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    bool getSwipeDetected();
    double getSwipeValue() const;
    double getSwipeThreshold() const ;
    double getHysteresisThreshold() const;
    double getMovementVelocity() const;
    double getMovementThreshold() const;
    double getContextValue() const;
    double getSwipeIntegrationCoeff() const;
    bool setContextInput(const bool contextInput);
    bool setSwipeIndex(const unsigned int swipeIndex);
    bool setSwipeDirection(const unsigned int swipeDirection);
    bool setSwipeThreshold(const double swipeThreshold);
    bool setHysteresisThreshold(const double hysteresisThreshold);
    bool setMovementThreshold(const double movementThreshold);
    bool setSwipeIntegrationCoeff(const double swipeIntegrationCoeff);


protected:
    
    bool firstSample;
    bool swipeDetected;
    bool contextInput;
    unsigned int swipeIndex;
    unsigned int swipeDirection;
    unsigned int contextFilterSize;
    double swipeIntegrationCoeff;
    double movementIntegrationCoeff;
    double swipeThreshold;
    double hysteresisThreshold;
    double swipeVelocity;
    double movementVelocity;
    double movementThreshold;
    double contextFilteredValue;
    vector< double > lastX;
    GRT::ThresholdCrossingDetector thresholdDetector;
    GRT::MedianFilter contextFilter;
    
    static RegisterClassifierModule< SwipeDetector > registerModule;
    
public:
    enum SwipeDirections{POSITIVE_SWIPE=0,NEGATIVE_SWIPE};
};

} //End of namespace GRT

#endif //GRT_SWIPE_DETECTION_HEADER

