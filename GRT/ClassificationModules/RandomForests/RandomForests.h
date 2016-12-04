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

#ifndef GRT_RANDOM_FORESTS_HEADER
#define GRT_RANDOM_FORESTS_HEADER

#include "../DecisionTree/DecisionTree.h"

GRT_BEGIN_NAMESPACE

/**
@brief This class implements a Random Decision Forest classifier.

Random Forests are an ensemble learning method that operate by building a number of decision
trees at training time and outputting the class with the majority vote over all the trees in the ensemble.

@example ClassificationModulesExamples/RandomForestsExample/RandomForestsExample.cpp

@remark This implementation is based on Breiman, Leo. "Random forests." Machine learning 45, no. 1 (2001): 5-32.
*/
class GRT_API RandomForests : public Classifier
{
public:
    /**
     Default Constructor
     
     @param decisionTreeNode: sets the type of decision tree node that will be used when training a new RandomForest model. Default: DecisionTreeClusterNode
     @param forestSize: sets the number of decision trees that will be trained. Default value = 10
     @param numRandomSplits: sets the number of random spilts that will be used to search for the best spliting value for each node. Default value = 100
     @param minNumSamplesPerNode: sets the minimum number of samples that are allowed per node, if the number of samples is below that, the node will become a leafNode.  Default value = 5
     @param maxDepth: sets the maximum depth of the tree. Default value = 10
     @param removeFeaturesAtEachSpilt: sets if features are removed at each stage in the tree
     @param useScaling: sets if the training and real-time data should be scaled between [0 1]. Default value = false
     @param bootstrappedDatasetWeight: sets the size of the bootstrapped dataset used to train a tree, the number of bootstrapped samples will be M*bootstrappedDatasetWeight, where M is the number of samples in the original training dataset
     */
	RandomForests(const DecisionTreeNode &decisionTreeNode = DecisionTreeClusterNode(),
                  const UINT forestSize=10,
                  const UINT numRandomSplits=100,
                  const UINT minNumSamplesPerNode=5,
                  const UINT maxDepth=10,
                  const Tree::TrainingMode trainingMode = Tree::BEST_RANDOM_SPLIT,
                  const bool removeFeaturesAtEachSpilt = true,
                  const bool useScaling=false,
                  const Float bootstrappedDatasetWeight = 0.8);
    
    /**
    Defines the copy constructor.
    
    @param const RandomForests &rhs: the instance from which all the data will be copied into this instance
    */
    RandomForests(const RandomForests &rhs);
    
    /**
    Default Destructor
    */
    virtual ~RandomForests(void);
    
    /**
    Defines how the data from the rhs RandomForests should be copied to this RandomForests
    
    @param rhs: another instance of a RandomForests
    @return returns a pointer to this instance of the RandomForests
    */
    RandomForests &operator=(const RandomForests &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an RandomForests instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another RandomForests instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the RandomForests model, using the labelled classification data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the RandomForests model was trained, false otherwise
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
    This function clears the RandomForests module, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the class was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    This function will print the model and settings to the display log.
    
    @return returns true if the model was printed succesfully, false otherwise
    */
    virtual bool print() const;
    
    /**
    This saves the trained RandomForests model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the RandomForests model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained RandomForests model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the RandomForests model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This function enables multiple random forest models to be merged together.  The model in forest will be combined
    with this instance.  For example, if this instance has 10 trees, and the other forest has 15 trees, the resulting
    model will have 25 trees.  Both forests must be trained and have the same number of inputs.
    
    @param forest: another random forest instance that will be merged with this instance
    @return returns true if the model was combined successfully, false otherwise
    */
    bool combineModels( const RandomForests &forest );
    
    /**
    Gets the number of trees in the random forest.
    
    @return returns the number of trees in the random forest
    */
    UINT getForestSize() const;
    
    /**
    Gets the number of random splits that will be used to search for the best spliting value for each node.
    
    @return returns the number of steps that will be used to search for the best spliting value for each node
    */
    UINT getNumRandomSplits() const;
    
    /**
    Gets the minimum number of samples that are allowed per node, if the number of samples at a node is below
    this value then the node will automatically become a leaf node.
    
    @return returns the minimum number of samples that are allowed per node
    */
    UINT getMinNumSamplesPerNode() const;
    
    /**
    Gets the maximum depth of the tree.
    
    @return returns the maximum depth of the tree
    */
    UINT getMaxDepth() const;
    
    /**
    Gets the training mode that will be used to train each DecisionTree in the forest.
    
    @return returns the trainingMode
    */
    UINT getTrainingMode() const;
    
    /**
    Gets a vector of DecisionTreeNodes pointers that represent the trees in the forest.
    
    @return returns a vector of DecisionTreeNodes
    */
    const Vector< DecisionTreeNode* > &getForest() const;
    
    /**
    Gets if a feature is removed at each spilt so it can not be used again.  If true then the best feature selected at each node will be
    removed so it can not be used in any children of that node.  If false, then the feature that provides the best spilt at each node will
    be used, regardless of how many times it has been used again.
    
    @return returns the removeFeaturesAtEachSpilt parameter
    */
    bool getRemoveFeaturesAtEachSpilt() const;
    
    /**
    Gets bootstrapped dataset weight, this controls the size of the bootstrapped dataset used to train each tree in the forest.
    The number of bootstrapped samples will be M*bootstrappedDatasetWeight, where M is the number of samples in the original training dataset.
    
    @return returns the bootstrappedDatasetWeight parameter
    */
    Float getBootstrappedDatasetWeight() const;
    
    /**
    Gets a pointer to the tree at the specific index in the forest. NULL will be returned if the model has not been trained or
    the index is invalid.
    
    @return returns a pointer to the tree at the specific index
    */
    DecisionTreeNode* getTree( const UINT index ) const;
    
    /**
    Gets a pointer to the decision tree node. NULL will be returned if the decision tree node has not been set.
    
    @return returns a pointer to a deep copy of the decision tree node
    */
    DecisionTreeNode* deepCopyDecisionTreeNode() const;
    
    /**
    Returns a vector of weights reflecting the importance of each feature in the random forest model.  The size of the vector
    will match the number of inputs (i.e. features) to the classifier.  The value in each element in the vector represents the
    weight (i.e. importance) of the corresponding feature.  A higher value represents a higher weight.
    
    The vector will be empty if the model has not been trained.
    
    @note This method only works with DecisionTreeNodes that support the getFeatureWeight() function.
    
    @param normWeights: if true, the weights will be normalized so they sum to 1.0
    @return returns a pointer to a deep copy of the decision tree node
    */
    VectorDouble getFeatureWeights( const bool normWeights = true ) const;
    
    /**
    Returns a vector of weights reflecting the importance of each feature in the random forest model.  The size of the vector
    will match the number of inputs (i.e. features) to the classifier.  The value in each element in the vector represents the
    weight (i.e. importance) of the corresponding feature.  A higher value represents a higher weight.
    
    The vector will be empty if the model has not been trained.
    
    @note This method only works with DecisionTreeNodes that support the getFeatureWeight() function.
    
    @param normWeights: if true, the weights will be normalized so they sum to 1.0
    @return returns a pointer to a deep copy of the decision tree node
    */
    MatrixDouble getLeafNodeFeatureWeights( const bool normWeights = true ) const;
    
    /**
    Sets the number of trees in the forest.  Changing this value will clear any previously trained model.
    
    @param forestSize: sets the number of trees in the forest.
    @return returns true if the parameter was set, false otherwise
    */
    bool setForestSize(const UINT forestSize);
    
    /**
    Sets the number of steps that will be used to search for the best spliting value for each node.
    
    A higher value will increase the chances of building a better model, but will take longer to train the model.
    Value must be larger than zero.
    
    @param numSplittingSteps: sets the number of steps that will be used to search for the best spliting value for each node.
    @return returns true if the parameter was set, false otherwise
    */
    bool setNumRandomSplits(const UINT numSplittingSteps);
    
    /**
    Sets the minimum number of samples that are allowed per node, if the number of samples at a node is below this value then the node will automatically
    become a leaf node.
    Value must be larger than zero.
    
    @param minNumSamplesPerNode: the minimum number of samples that are allowed per node
    @return returns true if the parameter was set, false otherwise
    */
    bool setMinNumSamplesPerNode(const UINT minNumSamplesPerNode);
    
    /**
    Sets the maximum depth of the tree, any node that reaches this depth will automatically become a leaf node.
    Value must be larger than zero.
    
    @param maxDepth: the maximum depth of the tree
    @return returns true if the parameter was set, false otherwise
    */
    bool setMaxDepth(const UINT maxDepth);
    
    /**
    Sets if a feature is removed at each spilt so it can not be used again.  If true then the best feature selected at each node will be
    removed so it can not be used in any children of that node.  If false, then the feature that provides the best spilt at each node will
    be used, regardless of how many times it has been used again.
    
    @param removeFeaturesAtEachSpilt: if true, then each feature is removed at each spilt so it can not be used again
    @return returns true if the parameter was set, false otherwise
    */
    bool setRemoveFeaturesAtEachSpilt(const bool removeFeaturesAtEachSpilt);
    
    /**
     Sets the training mode used to train each DecisionTree in the forest, this should be one of the DecisionTree::TrainingModes enums.
     
     @param trainingMode: the new trainingMode, this should be one of the DecisionTree::TrainingModes enums
     @return returns true if the trainingMode was set successfully, false otherwise
     */
    bool setTrainingMode(const Tree::TrainingMode trainingMode);
    
    /**
    Sets the decision tree node, this will be used as the starting node the next time the RandomForest model is trained.
    
    @return returns true if the decision tree node was updated, false otherwise
    */
    bool setDecisionTreeNode( const DecisionTreeNode &node );
    
    /**
    Sets the size of the bootstrapped dataset used to train a tree.
    The number of bootstrapped samples will be M*bootstrappedDatasetWeight, where M is the number of samples in the original training dataset.
    The weight should be in the range [> 0.0 <= 1.0]
    
    @return returns true if the parameter was updated, false otherwise
    */
    bool setBootstrappedDatasetWeight( const Float bootstrappedDatasetWeight );

    /**
    Gets a string that represents the DecisionTree class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    
protected:
    
    UINT forestSize;
    UINT numRandomSplits;
    UINT minNumSamplesPerNode;
    UINT maxDepth;
    Tree::TrainingMode trainingMode;
    bool removeFeaturesAtEachSpilt;
    Float bootstrappedDatasetWeight;
    DecisionTreeNode* decisionTreeNode;
    Vector< DecisionTreeNode* > forest;
    
private:
    static RegisterClassifierModule< RandomForests > registerModule;
    static const std::string id;
    
};

GRT_END_NAMESPACE

#endif //GRT_RANDOM_FORESTS_HEADER
    