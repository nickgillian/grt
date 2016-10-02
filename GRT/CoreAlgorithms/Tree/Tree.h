/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief This class implements the base class Tree used for the DecisionTree, RegressionTree and ClusterTree.

@remark This algorithm is still under development.
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

#ifndef GRT_TREE_HEADER
#define GRT_TREE_HEADER

#include "Node.h"

GRT_BEGIN_NAMESPACE

class GRT_API Tree : public MLBase
{
public:
    enum TrainingMode{BEST_ITERATIVE_SPILT=0,BEST_RANDOM_SPLIT,NUM_TRAINING_MODES};

    /**
     Default Constructor

     @param numSplittingSteps: sets the number of steps that will be used to search for the best spliting value for each node. Default value = 100
     @param minNumSamplesPerNode: sets the minimum number of samples that are allowed per node, if the number of samples is below that, the node will become a leafNode.  Default value = 5
     @param maxDepth: sets the maximum depth of the tree. Default value = 10
     @param removeFeaturesAtEachSpilt: sets if a feature is removed at each spilt so it can not be used again. Default value = false
     @param trainingMode: sets the training mode, this should be one of the TrainingMode enums. Default value = BEST_ITERATIVE_SPILT
     */
	Tree(const UINT numSplittingSteps=100,const UINT minNumSamplesPerNode=5,const UINT maxDepth=10,const bool removeFeaturesAtEachSpilt = false,const TrainingMode trainingMode = BEST_ITERATIVE_SPILT);
    
    /**
    Default Destructor
    */
    virtual ~Tree(void);
    
    /**
    This overrides the clear function in the Regressifier base class.
    It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the module was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    Prints the tree to std::cout.
    
    @return returns true if the model was printed
    */
    virtual bool print() const;
    
    /**
    This function adds the current model to the formatted stream.
    This function should be overwritten by the derived class.
    
    @param file: a reference to the stream the model will be added to
    @return returns true if the model was added successfully, false otherwise
    */
    virtual bool getModel( std::ostream &stream ) const;
    
    /**
    Deep copies the tree, returning a pointer to the new tree.
    The user is in charge of cleaning up the memory so must delete the pointer when they no longer need it.
    NULL will be returned if the tree could not be copied.
    
    @return returns a pointer to a deep copy of the tree
    */
    virtual Node* deepCopyTree() const;
    
    /**
    Gets a pointer to the root node of the tree. NULL will be returned if the tree model has not be trained.
    
    @return returns a const pointer to the tree
    */
    const Node* getTree() const;
    
    /**
    Gets the current training mode. This will be one of the TrainingModes enums.
    
    @return returns the training mode
    */
    UINT getTrainingMode() const;
    
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
    bool setTrainingMode(const TrainingMode trainingMode);
    
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
    
protected:
    
    UINT trainingMode;
    UINT numSplittingSteps;
    UINT minNumSamplesPerNode;
    UINT maxDepth;
    bool removeFeaturesAtEachSpilt;
    Node *tree; 
};

GRT_END_NAMESPACE

#endif //GRT_TREE_HEADER
