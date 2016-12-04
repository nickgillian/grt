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

#ifndef GRT_KNN_HEADER
#define GRT_KNN_HEADER

#include "../../CoreModules/Classifier.h"

GRT_BEGIN_NAMESPACE

#define BIG_DISTANCE 99e+99

/**
@brief This class implements the K-Nearest Neighbor classification algorithm (http://en.wikipedia.org/wiki/K-nearest_neighbor_algorithm).
KNN is a simple but powerful classifier, based on finding the closest K training examples in the feature space for the new input vector.
The KNN algorithm is amongst the simplest of all machine learning algorithms: an object is classified by a majority vote of its neighbors,
with the object being assigned to the class most common amongst its k nearest neighbors (k is a positive integer, typically small).
If k = 1, then the object is simply assigned to the class of its nearest neighbor.

This implementation of the algorithm will return the class label of the class that gains the majoriy vote of its neighbours.  If the average
distance of the closest K neighbors with the class label of the majority vote is greater than that of that classes rejection threshold, then
the predicted class label will be set to 0, indicating that the majority class was rejected.  This feature can be enabled or disabled by setting
the enableNullRejection paramter to false.

@example ClassificationModulesExamples/KNNExample/KNNExample.cpp

@remark This implementation is based on Bishop, Christopher M. Pattern recognition and machine learning. Vol. 1. New York: springer, 2006.
*/
class GRT_API KNN : public Classifier
{
public:
    enum DistanceMethods{EUCLIDEAN_DISTANCE=0,COSINE_DISTANCE,MANHATTAN_DISTANCE};
    
    /**
    Default Constructor
    
    @param K: the number of neigbors the algorithm will us to perform a classification. Default value is K = 10
    @param useScaling: sets if the training and prediction data should be scaled to a specific range.  Default value is useScaling = false
    @param useNullRejection: sets if null rejection will be used for the realtime prediction.  If useNullRejection is set to true then the predictedClassLabel will be set to 0 (which is the default null label) if the distance between the inputVector and the top K datum is greater than the null rejection threshold for the top predicted class.  The null rejection threshold is computed for each class during the training phase. Default value is useNullRejection = false
    @param nullRejectionCoeff: sets the null rejection coefficient, this is a multipler controlling the null rejection threshold for each class.  This will only be used if the useNullRejection parameter is set to true.  Default value is nullRejectionCoeff = 10.0
    @param searchForBestKValue: sets if the training algorithm will search for the best K value.  Default value is searchForBestKValue = false
    @param minKSearchValue: sets the minimum K value to use when searching for the best K value.  Default value is minKSearchValue = 1
    @param maxKSearchValue: sets the maximum K value to use when searching for the best K value.  Default value is maxKSearchValue = 1
    */
    KNN(UINT K=10,bool useScaling=false,bool useNullRejection=false,Float nullRejectionCoeff=10.0,bool searchForBestKValue = false,UINT minKSearchValue = 1,UINT maxKSearchValue = 10);
    
    /**
    Defines the copy constructor.
    
    @param const KNN &rhs: the instance from which all the data will be copied into this instance
    */
    KNN(const KNN &rhs);
    
    /**
    Default Destructor
    */
    virtual ~KNN(void);
    
    /**
    Defines how the data from the rhs KNN should be copied to this KNN
    
    @param rhs: another instance of a KNN
    @return returns a pointer to this instance of the KNN
    */
    KNN &operator=(const KNN &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to a KNN instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another KNN instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the KNN model, using the labelled classification data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the KNN model was trained, false otherwise
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
    This saves the trained KNN model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the KNN model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained KNN model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the KNN model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This recomputes the null rejection thresholds for each of the classes in the KNN model.
    This will be called automatically if the setGamma(Float gamma) function is called.
    The KNN model needs to be trained first before this function can be called.
    
    @return returns true if the null rejection thresholds were updated successfully, false otherwise
    */
    virtual bool recomputeNullRejectionThresholds();
    
    /**
    Gets the K nearest neighbours that will be searched for by the algorithm during prediction
    
    @return returns the K nearest neighbours that will be searched for by the algorithm during prediction
    */
    UINT getK(){ return K; }
    
    /**
    Returns the current distance method being used to compute the neighest neighbours.
    See the enum DistanceMethods.
    
    @return returns the current distance method being used to compute the neighest neighbours
    */
    UINT getDistanceMethod(){ return distanceMethod; }
    
    //Setters
    /**
    Sets the K nearest neighbours that will be searched for by the algorithm during prediction.
    This function should be called prior to running the training algorithm.
    
    @return returns true if the K was set successfully, false otherwise
    */
    bool setK(UINT K);
    
    /**
    Sets the minimum K value to use when searching for the best K value.
    This value should be less than the maxKSearchValue.
    
    @return returns true if the minimumKValue was set successfully, false otherwise
    */
    bool setMinKSearchValue(UINT minKSearchValue);
    
    /**
    Sets the maximum K value to use when searching for the best K value.
    This value should be greater than the minKSearchValue.
    
    @return returns true if the maxKSearchValue was set successfully, false otherwise
    */
    bool setMaxKSearchValue(UINT maxKSearchValue);
    
    /**
    Sets if the best K value should be searched for.
    If true then the best K value will be searched during the training phase.
    If false then the KNN algorithm will be trained with the K value set by the user.
    
    @return returns true if the searchForBestKValue was set successfully, false otherwise
    */
    bool enableBestKValueSearch(bool searchForBestKValue);
    
    /**
    Sets the nullRejectionCoeff parameter.
    The nullRejectionCoeff parameter is a multipler controlling the null rejection threshold for each class.
    This function will also recompute the null rejection thresholds.
    
    @return returns true if the nullRejectionCoeff parameter was updated successfully, false otherwise
    */
    bool setNullRejectionCoeff(Float nullRejectionCoeff);
    
    /**
    Sets the current distance method being used to compute the neighest neighbours.
    This should be called prior to training a KNN model.
    See the enum DistanceMethods for a list of possible distance methods.
    
    @return returns true if the distance method was updated successfully, false otherwise
    */
    bool setDistanceMethod(UINT distanceMethod);
    
    /**
    Gets a string that represents the KNN class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();

    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_;
    using MLBase::predict_;
    using MLBase::predict;
    
protected:
    bool train_(const ClassificationData &trainingData,const UINT K);
    bool predict(const VectorFloat &inputVector,const UINT K);
    bool loadLegacyModelFromFile( std::fstream &file );
    Float computeEuclideanDistance(const VectorFloat &a,const VectorFloat &b);
    Float computeCosineDistance(const VectorFloat &a,const VectorFloat &b);
    Float computeManhattanDistance(const VectorFloat &a,const VectorFloat &b);
    
    UINT K;                                     ///> The number of neighbours to search for
    UINT distanceMethod;                        ///> The distance method used to compute the distance between each data point
    bool searchForBestKValue;                   ///> Sets if the best K value should be searched for or if the model should be trained with K
    UINT minKSearchValue;                       ///> The minimum K value to start the search from
    UINT maxKSearchValue;                       ///> The maximum K value to end the search at
    ClassificationData trainingData;            ///> Holds the trainingData to perform the predictions
    VectorFloat trainingMu;                    ///> Holds the average max-class distance of the training data for each of classes
    VectorFloat trainingSigma;                 ///> Holds the stddev of the max-class distance of the training data for each of classes
    
private:
    static RegisterClassifierModule< KNN > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_KNN_HEADER
    
