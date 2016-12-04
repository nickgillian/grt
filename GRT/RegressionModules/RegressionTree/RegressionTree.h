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

#ifndef GRT_REGRESSION_TREE_HEADER
#define GRT_REGRESSION_TREE_HEADER

#include "../../CoreModules/Regressifier.h"
#include "../../CoreAlgorithms/Tree/Tree.h"
#include "RegressionTreeNode.h"

GRT_BEGIN_NAMESPACE

/**
@brief This class implements a basic Regression Tree. 
@remark This algorithm is still under development.
*/
class GRT_API RegressionTree : public Regressifier
{
public:
    /**
     Default Constructor

     @param numSplittingSteps: sets the number of steps that will be used to search for the best spliting value for each node. Default value = 100
     @param minNumSamplesPerNode: sets the minimum number of samples that are allowed per node, if the number of samples is below that, the node will become a leafNode.  Default value = 5
     @param maxDepth: sets the maximum depth of the tree. Default value = 10
     @param removeFeaturesAtEachSpilt: sets if a feature is removed at each spilt so it can not be used again. Default value = false
     @param trainingMode: sets the training mode, this should be one of the TrainingMode enums. Default value = BEST_ITERATIVE_SPILT
     @param useScaling: sets if the training and real-time data should be scaled between [0 1]. Default value = false
     @param minRMSErrorPerNode: sets the minimum RMS error that allowed per node, if the RMS error is below that, the node will become a leafNode. Default value = 0.01
     */
	RegressionTree(const UINT numSplittingSteps=100,const UINT minNumSamplesPerNode=5,const UINT maxDepth=10,const bool removeFeaturesAtEachSpilt = false,const Tree::TrainingMode trainingMode = Tree::BEST_ITERATIVE_SPILT,const bool useScaling=false,const Float minRMSErrorPerNode = 0.01);
    
    /**
     Defines the copy constructor.
     
     @param rhs: the instance from which all the data will be copied into this instance
     */
    RegressionTree(const RegressionTree &rhs);
    
    /**
     Default Destructor
     */
	virtual ~RegressionTree(void);
    
    /**
     Defines how the data from the rhs RegressionTree should be copied to this RegressionTree
     
     @param rhs: another instance of a RegressionTree
     @return returns a pointer to this instance of the RegressionTree
     */
	RegressionTree &operator=(const RegressionTree &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setRegressifier(...) method is called.
     It clones the data from the Base Class Regressifier pointer (which should be pointing to an RegressionTree instance) into this instance
     
     @param regressifier: a pointer to the Regressifier Base Class, this should be pointing to another RegressionTree instance
     @return returns true if the clone was successfull, false otherwise
    */
	virtual bool deepCopyFrom(const Regressifier *regressifier);
    
    /**
     This trains the RegressionTree model, using the labelled regression data.
     This overrides the train function in the Regressifier base class.
     
     @param trainingData: a reference to the training data
     @return returns true if the RegressionTree model was trained, false otherwise
    */
    virtual bool train_(RegressionData &trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the Regressifier base class.
     
     @param inputVector: the input Vector to predict
     @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
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
     This saves the trained RegressionTree model to a file.
     This overrides the save function in the Regressifier base class.
     
     @param file: a reference to the file the RegressionTree model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool save( std::fstream &file ) const;
    
    /**
     This loads a trained RegressionTree model from a file.
     This overrides the load function in the Regressifier base class.
     
     @param file: a reference to the file the RegressionTree model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool load( std::fstream &file );

    /**
     Deep copies the regression tree, returning a pointer to the new regression tree.
     The user is in charge of cleaning up the memory so must delete the pointer when they no longer need it.
     NULL will be returned if the tree could not be copied.
     
     @return returns a pointer to a deep copy of the regression tree
     */
    RegressionTreeNode* deepCopyTree() const;
    
    /**
     Gets a pointer to the regression tree. NULL will be returned if the decision tree model has not be trained.
     
     @return returns a const pointer to the regression tree
     */
    const RegressionTreeNode* getTree() const;
    
    /**
     Gets the minimum root mean squared error value that needs to be exceeded for the tree to continue growing at a specific node.
     If the RMS error is below this value then the node will be made into a leaf node.
     
     @return returns the minimum RMS error per node
     */
    Float getMinRMSErrorPerNode() const;

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
     Sets the minimum RMS error that needs to be exceeded for the tree to continue growing at a specific node.
     
     @param minRMSErrorPerNode: sets the minRMSErrorPerNode parameter
     @return returns true if the parameter was updated
     */
    bool setMinRMSErrorPerNode(const Float minRMSErrorPerNode);

    /**
    Gets a string that represents the RegressionTree class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_; ///<Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::predict_; ///<Tell the compiler we are using the base class predict method to stop hidden virtual function warnings
    
protected:
    Node *tree; 
    UINT minNumSamplesPerNode;
    UINT maxDepth;
    UINT numSplittingSteps;
    bool removeFeaturesAtEachSpilt;
    Tree::TrainingMode trainingMode;
    Float minRMSErrorPerNode;
    
    RegressionTreeNode* buildTree( const RegressionData &trainingData, RegressionTreeNode *parent, Vector< UINT > features, UINT nodeID );
    bool computeBestSpilt( const RegressionData &trainingData, const Vector< UINT > &features, UINT &featureIndex, Float &threshold, Float &minError );
    bool computeBestSpiltBestIterativeSpilt( const RegressionData &trainingData, const Vector< UINT > &features, UINT &featureIndex, Float &threshold, Float &minError );
    //bool computeBestSpiltBestRandomSpilt( const RegressionData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &threshold, Float &minError );
    bool computeNodeRegressionData( const RegressionData &trainingData, VectorFloat &regressionData );

private:
    static RegisterRegressifierModule< RegressionTree > registerModule;
    static const std::string id;
    
};

GRT_END_NAMESPACE

#endif //GRT_REGRESSION_TREE_HEADER

