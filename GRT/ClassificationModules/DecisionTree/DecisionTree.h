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

#ifndef GRT_DECISION_TREE_HEADER
#define GRT_DECISION_TREE_HEADER

#include "../../CoreModules/Classifier.h"
#include "../../CoreAlgorithms/Tree/Tree.h"
#include "DecisionTreeNode.h"
#include "DecisionTreeThresholdNode.h"
#include "DecisionTreeClusterNode.h"
#include "DecisionTreeTripleFeatureNode.h"

GRT_BEGIN_NAMESPACE

/**
@brief This class implements a basic Decision Tree classifier.  Decision Trees are conceptually simple
classifiers that work well on even complex classification tasks.  Decision Trees partition the feature
space into a set of rectangular regions, classifying a new datum by finding which region it belongs to.

@remark This implementation is based on Ross Quinlan's ID3 Decision Tree algorithm: http://en.wikipedia.org/wiki/ID3_algorithm

@example ClassificationModulesExamples/DecisionTreeExample/DecisionTreeExample.cpp
*/
class GRT_API DecisionTree : public Classifier
{
public:
    /**
    Default Constructor
    
    @param decisionTreeNode: sets the type of decision tree node that will be used when training a new decision tree model. Default: DecisionTreeClusterNode
    @param minNumSamplesPerNode: sets the minimum number of samples that are allowed per node, if the number of samples is below that, the node will become a leafNode.  Default value = 5
    @param maxDepth: sets the maximum depth of the tree. Default value = 10
    @param removeFeaturesAtEachSpilt: sets if a feature is removed at each spilt so it can not be used again. Default value = false
    @param trainingMode: sets the training mode, this should be one of the TrainingMode enums. Default value = BEST_ITERATIVE_SPILT
    @param numSplittingSteps: sets the number of steps that will be used to search for the best spliting value for each node. Default value = 100
    @param useScaling: sets if the training and real-time data should be scaled between [0 1]. Default value = false
    */
    DecisionTree(const DecisionTreeNode &decisionTreeNode = DecisionTreeClusterNode(),const UINT minNumSamplesPerNode=5,const UINT maxDepth=10,const bool removeFeaturesAtEachSpilt = false,const Tree::TrainingMode trainingMode = Tree::TrainingMode::BEST_ITERATIVE_SPILT,const UINT numSplittingSteps=100,const bool useScaling=false );
    
    /**
    Defines the copy constructor.
    
    @param rhs: the instance from which all the data will be copied into this instance
    */
    DecisionTree(const DecisionTree &rhs);
    
    /**
    Default Destructor
    */
    virtual ~DecisionTree(void);
    
    /**
    Defines how the data from the rhs DecisionTree should be copied to this DecisionTree
    
    @param rhs: another instance of a DecisionTree
    @return returns a pointer to this instance of the DecisionTree
    */
    DecisionTree &operator=(const DecisionTree &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an DecisionTree instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another DecisionTree instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the DecisionTree model, using the labelled classification data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the DecisionTree model was trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
    This predicts the class of the inputVector.
    This overrides the predict function in the Classifier base class.
    
    @param inputVector: the input Vector to classify
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
    This recomputes the null rejection thresholds for each of the classes in the DecisionTree model.
    The DecisionTree model needs to be trained first before this function can be called.
    
    @return returns true if the null rejection thresholds were updated successfully, false otherwise
    */
    virtual bool recomputeNullRejectionThresholds();
    
    /**
    This saves the trained DecisionTree model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the DecisionTree model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained DecisionTree model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the DecisionTree model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This function adds the current model to the formatted stream.
    This function should be overwritten by the derived class.
    
    @param file: a reference to the stream the model will be added to
    @return returns true if the model was added successfully, false otherwise
    */
    virtual bool getModel( std::ostream &stream ) const;
    
    /**
    Deep copies the decision tree, returning a pointer to the new decision tree.
    The user is in charge of cleaning up the memory so must delete the pointer when they no longer need it.
    NULL will be returned if the tree could not be copied.
    
    @return returns a pointer to a deep copy of the decision tree
    */
    DecisionTreeNode* deepCopyTree() const;

    /**
    Gets a pointer to the decision tree node. NULL will be returned if the decision tree node has not been set.
    
    @return returns a pointer to a deep copy of the decision tree node
    */
    DecisionTreeNode* deepCopyDecisionTreeNode() const;
    
    /**
    Gets a pointer to the decision tree. NULL will be returned if the decision tree model has not be trained.
    
    @return returns a const pointer to the decision tree
    */
    const DecisionTreeNode* getTree() const;

    /**
    Gets the current training mode. This will be one of the TrainingModes enums.
    
    @return returns the training mode
    */
    Tree::TrainingMode getTrainingMode() const;
    
    /**
    Gets the number of steps that will be used to search for the best spliting value for each node.
    
    If the trainingMode is set to BEST_ITERATIVE_SPILT, then the numSplittingSteps controls how many iterative steps there will be per feature.
    If the trainingMode is set to BEST_RANDOM_SPLIT, then the numSplittingSteps controls how many random searches there will be per feature.
    
    @return returns the number of steps that will be used to search for the best spliting value for each node
    */
    UINT getNumSplittingSteps() const;
    
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
    This function returns the predictedNodeID, this is ID of the leaf node that was reached during the last prediction call
    
    @return returns the predictedNodeID, this will be zero if the tree does not exist or predict has not been called
    */
    UINT getPredictedNodeID() const;
    
    /**
    Gets if a feature is removed at each spilt so it can not be used again.
    
    @return returns true if a feature is removed at each spilt so it can not be used again, false otherwise
    */
    bool getRemoveFeaturesAtEachSpilt() const;
    
    /**
     Sets the training mode, this should be one of the TrainingModes enums.
     
     @param trainingMode: the new trainingMode, this should be one of the TrainingModes enums
     @return returns true if the trainingMode was set successfully, false otherwise
     */
    bool setTrainingMode(const Tree::TrainingMode trainingMode);
    
    /**
    Sets the number of steps that will be used to search for the best spliting value for each node.
    
    If the trainingMode is set to BEST_ITERATIVE_SPILT, then the numSplittingSteps controls how many iterative steps there will be per feature.
    If the trainingMode is set to BEST_RANDOM_SPLIT, then the numSplittingSteps controls how many random searches there will be per feature.
    
    A higher value will increase the chances of building a better model, but will take longer to train the model.
    Value must be larger than zero.
    
    @param numSplittingSteps: sets the number of steps that will be used to search for the best spliting value for each node.
    @return returns true if the parameter was set, false otherwise
    */
    bool setNumSplittingSteps(const UINT numSplittingSteps);
    
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
    Sets the decision tree node, this will be used as the starting node the next time the DecisionTree model is trained.
    
    @return returns true if the decision tree node was updated, false otherwise
    */
    bool setDecisionTreeNode( const DecisionTreeNode &node );

    /**
    Gets a string that represents the DecisionTree class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_;
    using MLBase::predict_;
    using MLBase::print;
    
protected:
    bool loadLegacyModelFromFile_v1( std::fstream &file );
    bool loadLegacyModelFromFile_v2( std::fstream &file );
    bool loadLegacyModelFromFile_v3( std::fstream &file );
    
    bool trainTree( ClassificationData trainingData, const ClassificationData &trainingDataCopy, const ClassificationData &validationData, Vector< UINT > features );
    DecisionTreeNode* buildTree(ClassificationData &trainingData, DecisionTreeNode *parent, Vector< UINT > features, const Vector< UINT > &classLabels, UINT nodeID );
    Float getNodeDistance( const VectorFloat &x, const UINT nodeID );
    Float getNodeDistance( const VectorFloat &x, const VectorFloat &y );
    
    DecisionTreeNode* decisionTreeNode;
    std::map< UINT, VectorFloat > nodeClusters;
    VectorFloat classClusterMean;
    VectorFloat classClusterStdDev;

    Node *tree; 
    UINT minNumSamplesPerNode;
    UINT maxDepth;
    UINT numSplittingSteps;
    bool removeFeaturesAtEachSpilt;
    Tree::TrainingMode trainingMode;

private:
    static RegisterClassifierModule< DecisionTree > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_DECISION_TREE_HEADER
    