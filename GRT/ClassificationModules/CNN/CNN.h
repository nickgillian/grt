/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 
 @brief This class implements a wrapper for the tiny-cnn library to add Convolution Neural Network support to the GRT.
 
 @remark This implementation is based on tiny-cnn: https://github.com/nyanp/tiny-cnn
 */

#ifndef GRT_CNN_HEADER
#define GRT_CNN_HEADER

#include "../../CoreModules/Classifier.h"

namespace GRT{

class CNN : public Classifier
{
public:
    /**
     Default Constructor
     */
	CNN();
    
    /**
     Defines the copy constructor.
     
     @param &rhs: the instance from which all the data will be copied into this instance
     */
    CNN(const CNN &rhs);
    
    /**
     Default Destructor
     */
	virtual ~CNN(void);
    
    /**
     Defines how the data from the rhs CNN should be copied to this CNN
     
     @param rhs: another instance of a CNN
     @return returns a reference to this CNN instance
     */
	CNN &operator=(const CNN &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.  
     It clones the data from the Base Class Classifier pointer (which should be pointing to an CNN instance) into this instance
     
     @param classifier: a pointer to the Classifier Base Class, this should be pointing to another CNN instance
     @return returns true if the clone was successfull, false otherwise
    */
	virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
     This trains the CNN model, using the classification data.
     This overrides the train function in the Classifier base class.
     
     @param trainingData: a reference to the training data
     @return returns true if the CNN model was trained, false otherwise
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
     This resets the CNN classifier.
     
     @return returns true if the CNN model was successfully reset, false otherwise.
     */
    virtual bool reset();
    
    /**
     This overrides the clear function in the Classifier base class.
     It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
     
     @return returns true if the module was cleared succesfully, false otherwise
     */
    virtual bool clear();
    
    /**
     This saves the trained CNN model to a file.
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the CNN model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained CNN model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the CNN model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::saveModelToFile;
    using MLBase::loadModelFromFile;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;
    
protected:
    
    static RegisterClassifierModule< CNN > registerModule;
};

} //End of namespace GRT

#endif // header guard
