/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a basic Regression Tree.
 
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

#ifndef GRT_REGRESSIONTREE_HEADER
#define GRT_REGRESSION_TREE_HEADER

#include "../../CoreModules/Regressifier.h"
#include "RegressionTreeNode.h"

namespace GRT{

class RegressionTree : public Regressifier
{
public:
    /**
     Default Constructor

     @param UINT numSplittingSteps: sets the number of steps that will be used to search for the best spliting value for each node. Default value = 100
     @param UINT minNumSamplesPerNode: sets the minimum number of samples that are allowed per node, if the number of samples is below that, the node will become a leafNode.  Default value = 5
     @param UINT maxDepth: sets the maximum depth of the tree. Default value = 10
     @param bool removeFeaturesAtEachSpilt: sets if a feature is removed at each spilt so it can not be used again. Default value = false
     @param UINT trainingMode: sets the training mode, this should be one of the TrainingMode enums. Default value = BEST_ITERATIVE_SPILT
     @param bool useScaling: sets if the training and real-time data should be scaled between [0 1]. Default value = false
     */
	RegressionTree(const UINT numSplittingSteps=100,const UINT minNumSamplesPerNode=5,const UINT maxDepth=10,const bool removeFeaturesAtEachSpilt = false,const UINT trainingMode = BEST_ITERATIVE_SPILT,const bool useScaling=false);
    
    /**
     Defines the copy constructor.
     
     @param const RegressionTree &rhs: the instance from which all the data will be copied into this instance
     */
    RegressionTree(const RegressionTree &rhs);
    
    /**
     Default Destructor
     */
	virtual ~RegressionTree(void);
    
    /**
     Defines how the data from the rhs RegressionTree should be copied to this RegressionTree
     
     @param const RegressionTree &rhs: another instance of a RegressionTree
     @return returns a pointer to this instance of the RegressionTree
     */
	RegressionTree &operator=(const RegressionTree &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setRegressifier(...) method is called.
     It clones the data from the Base Class Regressifier pointer (which should be pointing to an RegressionTree instance) into this instance
     
     @param Regressifier *regressifier: a pointer to the Regressifier Base Class, this should be pointing to another RegressionTree instance
     @return returns true if the clone was successfull, false otherwise
    */
	virtual bool deepCopyFrom(const Regressifier *regressifier);
    
    /**
     This trains the RegressionTree model, using the labelled regression data.
     This overrides the train function in the Regressifier base class.
     
     @param RegressionData trainingData: a reference to the training data
     @return returns true if the RegressionTree model was trained, false otherwise
    */
    virtual bool train_(RegressionData &trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the Regressifier base class.
     
     @param VectorDouble inputVector: the input vector to predict
     @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorDouble &inputVector);
    
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
     This overrides the saveModelToFile function in the Regressifier base class.
     
     @param string filename: the name of the file to save the RegressionTree model to
     @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the trained RegressionTree model to a file.
     This overrides the saveModelToFile function in the Regressifier base class.
     
     @param fstream &file: a reference to the file the RegressionTree model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained RegressionTree model from a file.
     This overrides the loadModelFromFile function in the Regressifier base class.
     
     @param string filename: the name of the file to load the RegressionTree model from
     @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads a trained RegressionTree model from a file.
     This overrides the loadModelFromFile function in the Regressifier base class.
     
     @param fstream &file: a reference to the file the RegressionTree model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);

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
     Gets if a feature is removed at each spilt so it can not be used again.
     
     @return returns true if a feature is removed at each spilt so it can not be used again, false otherwise
     */
    bool getRemoveFeaturesAtEachSpilt() const;
    
    /**
     Sets the training mode, this should be one of the TrainingModes enums.
     
     @param const UINT trainingMode: the new trainingMode, this should be one of the TrainingModes enums
     @return returns true if the trainingMode was set successfully, false otherwise
     */
    bool setTrainingMode(const UINT trainingMode);
    
    /**
     Sets the number of steps that will be used to search for the best spliting value for each node.
     
     If the trainingMode is set to BEST_ITERATIVE_SPILT, then the numSplittingSteps controls how many iterative steps there will be per feature.
     If the trainingMode is set to BEST_RANDOM_SPLIT, then the numSplittingSteps controls how many random searches there will be per feature.
     
     A higher value will increase the chances of building a better model, but will take longer to train the model.
     Value must be larger than zero.
     
     @param UINT numSplittingSteps: sets the number of steps that will be used to search for the best spliting value for each node.
     @return returns true if the parameter was set, false otherwise
     */
    bool setNumSplittingSteps(const UINT numSplittingSteps);
    
    /**
     Sets the minimum number of samples that are allowed per node, if the number of samples at a node is below this value then the node will automatically 
     become a leaf node.
     Value must be larger than zero.
     
     @param UINT minNumSamplesPerNode: the minimum number of samples that are allowed per node
     @return returns true if the parameter was set, false otherwise
     */
    bool setMinNumSamplesPerNode(const UINT minNumSamplesPerNode);
    
    /**
     Sets the maximum depth of the tree, any node that reaches this depth will automatically become a leaf node.
     Value must be larger than zero.
     
     @param UINT maxDepth: the maximum depth of the tree
     @return returns true if the parameter was set, false otherwise
     */
    bool setMaxDepth(const UINT maxDepth);
    
    /**
     Sets if a feature is removed at each spilt so it can not be used again.  If true then the best feature selected at each node will be 
     removed so it can not be used in any children of that node.  If false, then the feature that provides the best spilt at each node will
     be used, regardless of how many times it has been used again.
     
     @param bool removeFeaturesAtEachSpilt: if true, then each feature is removed at each spilt so it can not be used again
     @return returns true if the parameter was set, false otherwise
     */
    bool setRemoveFeaturesAtEachSpilt(const bool removeFeaturesAtEachSpilt);
    
    using MLBase::train; ///<Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::predict; ///<Tell the compiler we are using the base class predict method to stop hidden virtual function warnings
    
protected:
    
    UINT trainingMode;
    UINT numSplittingSteps;
    UINT minNumSamplesPerNode;
    UINT maxDepth;
    bool removeFeaturesAtEachSpilt;
    RegressionTreeNode *tree;
    
    RegressionTreeNode* buildTree( const RegressionData &trainingData, RegressionTreeNode *parent, vector< UINT > features );
    bool computeBestSpilt( const RegressionData &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError );
    bool computeBestSpiltBestIterativeSpilt( const RegressionData &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError );
    //bool computeBestSpiltBestRandomSpilt( const RegressionData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &threshold, double &minError );

    static RegisterRegressifierModule< RegressionTree > registerModule;
    
public:
    enum TrainingMode{BEST_ITERATIVE_SPILT=0,BEST_RANDOM_SPLIT,NUM_TRAINING_MODES};
    
};

} //End of namespace GRT

#endif //GRT_REGRESSION_TREE_HEADER

