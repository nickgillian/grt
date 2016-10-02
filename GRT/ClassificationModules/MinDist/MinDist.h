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

#ifndef GRT_MINDIST_HEADER
#define GRT_MINDIST_HEADER

#include "MinDistModel.h"
#include "../../CoreModules/Classifier.h"

GRT_BEGIN_NAMESPACE

/**
@brief This class implements the MinDist classifier algorithm.

@example ClassificationModulesExamples/MinDistExample/MinDistExample.cpp

@remark This implementation is custom algorithm, publication forthcoming.
*/
class GRT_API MinDist : public Classifier
{
public:
    /**
    Default Constructor
    
    @param useScaling: sets if the training and prediction data should be scaled to a specific range.  Default value is useScaling = false
    @param useNullRejection: sets if null rejection will be used for the realtime prediction.  If useNullRejection is set to true then the predictedClassLabel will be set to 0 (which is the default null label) if the distance between the inputVector and the top K datum is greater than the null rejection threshold for the top predicted class.  The null rejection threshold is computed for each class during the training phase. Default value is useNullRejection = false
    @param nullRejectionCoeff: sets the null rejection coefficient, this is a multipler controlling the null rejection threshold for each class.  This will only be used if the useNullRejection parameter is set to true.  Default value is nullRejectionCoeff = 10.0
    @param numClusters: sets how many clusters each model will try to find during the training phase.  Default value = 10
    */
    MinDist(bool useScaling=false,bool useNullRejection=false,Float nullRejectionCoeff=10.0,UINT numClusters=10);
    
    /**
    Defines the copy constructor.
    
    @param rhs: the instance from which all the data will be copied into this instance
    */
    MinDist(const MinDist &rhs);
    
    /**
    Default Destructor
    */
    virtual ~MinDist(void);
    
    /**
    Defines how the data from the rhs MinDist should be copied to this MinDist
    
    @param rhs: another instance of a MinDist
    @return returns a pointer to this instance of the MinDist
    */
    MinDist &operator=(const MinDist &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an MinDist instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another MinDist instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the MinDist model, using the labelled classification data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the MinDist model was trained, false otherwise
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
    This overrides the clear function in the Classifier base class.
    It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the module was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    This saves the trained MinDist model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the MinDist model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained MinDist model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the MinDist model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This recomputes the null rejection thresholds for each of the classes in the MinDist model.
    This will be called automatically if the setGamma(Float gamma) function is called.
    The MinDist model needs to be trained first before this function can be called.
    
    @return returns true if the null rejection thresholds were updated successfully, false otherwise
    */
    virtual bool recomputeNullRejectionThresholds();
    
    /**
    Returns the number of clusters in the model.
    
    @return returns the number of clusters in the model.
    */
    UINT getNumClusters() const;
    
    /**
    Returns the MinDist models for each of the classes.
    
    @return returns a vector of the MinDist models for each of the classes
    */
    Vector< MinDistModel > getModels() const;
    
    /**
    Sets the nullRejectionCoeff parameter.
    The nullRejectionCoeff parameter is a multipler controlling the null rejection threshold for each class.
    This function will also recompute the null rejection thresholds.
    
    @return returns true if the gamma parameter was updated successfully, false otherwise
    */
    virtual bool setNullRejectionCoeff(Float nullRejectionCoeff);
    
    /**
    Sets the numClusters parameter.
    The numClusters parameter sets how many clusters each model will try to find during the training phase.
    You should call this function before you train the model.
    
    @return returns true if the numClusters parameter was updated successfully, false otherwise
    */
    bool setNumClusters(UINT numClusters);
    
    /**
    Gets a string that represents the MinDist class.
    
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
    
    UINT numClusters;
    Vector< MinDistModel > models;            //A buffer to hold all the models
    
private:
    static const std::string id;
    static RegisterClassifierModule< MinDist > registerModule;
};

} //End of namespace GRT

#endif //GRT_MINDIST_HEADER
