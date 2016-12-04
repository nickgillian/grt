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

#ifndef GRT_ANBC_HEADER
#define GRT_ANBC_HEADER

#include "ANBC_Model.h"
#include "../../CoreModules/Classifier.h"

GRT_BEGIN_NAMESPACE

#define MIN_SCALE_VALUE 1.0e-10
#define MAX_SCALE_VALUE 1

/**
@brief This class implements the Adaptive Naive Bayes Classifier algorithm.  The Adaptive Naive Bayes Classifier (ANBC) is a naive but powerful classifier that works very well on both basic and more complex recognition problems.

The ANBC algorithm is a supervised learning algorithm that can be used to classify any type of N-dimensional signal. The ANBC algorithm essentially works by fitting an N-dimensional Gaussian distribution to each class (i.e. gesture) during the training phase. New gestures can then be recognized in the prediction phase by finding the gesture that results in the maximum likelihood value (given the new sensor data and each of the Gaussian distributions). The ANBC algorithm also computes rejection thresholds that enable the algorithm to automatically reject sensor values that are not the K gestures the algorithm has been trained to recognized (without being explicitly told during the prediction phase if a gesture is, or is not, being performed).

In addition, the ANBC algorithm enables you to weight the importance of each dimension for each gesture. For instance, imagine that you want to create a recognition system that can recognize a user's left-handed gestures, right-handed gestures, and two-handed gestures. To track the user's movements you use a depth sensor and skeleton-tracking algorithm that can track any user who stands in front of the depth sensor and sends out the x-y-z joint position of the user's two hands (along with the user's other joints) at 30Hz. You use the 3-dimensional joint data for each hand to create a 6-dimensional vector (containing {leftHandX, leftHandY, leftHandZ, rightHandX, rightHandY, rightHandZ}) as input to the ANBC algorithm. The ANBC algorithm enables you to weight each dimension of this vector for each of the 3 types of gestures you want to recognize (i.e. left-handed, right-handed, and two-handed gestures), so for a left-handed gesture you would set the weights for this class to {1,1,1,0,0,0}, for the right-handed gesture you would set the weights for this class to {0,0,0,1,1,1}, and for the two-handed gesture you would set the weights for this class to {1,1,1,1,1,1}. You only need to set these weights values once, before you train the ANBC model, the weights will then automatically be incorporated into the Gaussian models for each gesture (and therefore nothing special needs to be done for the prediction phase). You can set the weights using the setWeights(LabelledClassificationData weightsData) function.

The ANBC algorithm is part of the GRT classification modules.

@remark You can find out more about the ANBC algorithm in <a href="http://www.nickgillian.com/papers/Gillian_ANBC.pdf">ANBC.pdf</a>.

@example ClassificationModulesExamples/ANBCExample/ANBCExample.cpp
*/
class GRT_API ANBC : public Classifier
{
public:
    /**
    Default Constructor
    
    @param useScaling: sets if each dimension of the training and prediction data should be scaled to the same range.  This may be useful if different dimensions of your input data have very different ranges. Default value is useScaling = false
    @param useNullRejection: sets if null rejection will be used for the realtime prediction.  If useNullRejection is set to true then the predictedClassLabel will be set to 0 (which is the default null label) if the distance between the inputVector and the top K datum is greater than the null rejection threshold for the top predicted class.  The null rejection threshold is computed for each class during the training phase. Default value is useNullRejection = false
    @param nullRejectionCoeff: sets the null rejection coefficient, this is a multipler controlling the null rejection threshold for each class.  This will only be used if the useNullRejection parameter is set to true.  Default value is nullRejectionCoeff = 10.0
    */
    ANBC(bool useScaling=false,bool useNullRejection=false,double nullRejectionCoeff=10.0);
    
    /**
    Defines the copy constructor.
    
    @param const ANBC &rhs: the instance from which all the data will be copied into this instance
    */
    ANBC(const ANBC &rhs);
    
    /**
    Default Destructor
    */
    virtual ~ANBC(void);
    
    /**
    Defines how the data from the rhs ANBC should be copied to this ANBC
    
    @param rhs: another instance of a ANBC
    @return returns a pointer to this instance of the ANBC
    */
    ANBC &operator=(const ANBC &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an ANBC instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another ANBC instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the ANBC model, using the labelled classification data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the ANBC model was trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
    This predicts the class of the inputVector.
    This overrides the predict function in the Classifier base class.
    
    @param inputVector: the input vector to classify
    @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
    This resets the ANBC classifier.
    
    @return returns true if the ANBC model was successfully reset, false otherwise.
    */
    virtual bool reset();
    
    /**
    This overrides the clear function in the Classifier base class.
    It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the module was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    This saves the trained ANBC model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the ANBC model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained ANBC model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the ANBC model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This recomputes the null rejection thresholds for each of the classes in the ANBC model.
    This will be called automatically if the setGamma(double gamma) function is called.
    The ANBC model needs to be trained first before this function can be called.
    
    @return returns true if the null rejection thresholds were updated successfully, false otherwise
    */
    virtual bool recomputeNullRejectionThresholds();
    
    /**
    Gets a vector containing the null rejection thresholds for each class, this will be an N-dimensional vector, where N is the number of classes in the model.
    
    @return returns a vector containing the null rejection thresholds for each class, an empty vector will be returned if the model has not been trained
    */
    VectorFloat getNullRejectionThresholds() const;
    
    /**
    Returns the ANBC models for each of the classes.
    
    @return returns a vector of the ANBC models for each of the classes
    */
    Vector< ANBC_Model > getModels(){ return models; }
    
    //Setters
    /**
    Sets the nullRejectionCoeff parameter.
    The nullRejectionCoeff parameter is a multipler controlling the null rejection threshold for each class.
    This function will also recompute the null rejection thresholds.
    
    @return returns true if the gamma parameter was updated successfully, false otherwise
    */
    bool setNullRejectionCoeff(double nullRejectionCoeff);
    
    /**
    Sets the weights for the training and prediction.
    The dimensionality of the weights should match that of the training data used to train the ANBC models.
    The weights should be encapsualted into a LabelledClassificationData container, with one training sample for each class.
    
    @return returns true if the weights were correctly set, false otherwise
    */
    bool setWeights(const ClassificationData &weightsData);
    
    /**
    Clears any previously set weights.
    
    @return returns true if the weights were correctly cleared, false otherwise
    */
    bool clearWeights(){ weightsDataSet = false; weightsData.clear(); return true; }

    /**
    Gets a string that represents the ANBC class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_;
    using MLBase::predict_;
    
protected:
    bool loadLegacyModelFromFile( std::fstream &file );
    
    bool weightsDataSet;                  //A flag to indicate if the user has manually set the weights buffer
    ClassificationData weightsData;       //The weights of each feature for each class for training the algorithm
    Vector< ANBC_Model > models;          //A buffer to hold all the models
    
private:
    static RegisterClassifierModule< ANBC > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif // GRT_WEAK_CLASSIFIER_HEADER
    