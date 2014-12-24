/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a Cluster Tree. This can be used to automatically build a cluster model (where each leaf node in the tree is given a unique cluster label) and then predict the best cluster label for a new input sample.
 
 @remark This module is still under development
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

#ifndef GRT_CLUSTER_TREE_HEADER
#define GRT_CLUSTER_TREE_HEADER

#include "../../CoreModules/Clusterer.h"
#include "../../CoreAlgorithms/Tree/Tree.h"
#include "ClusterTreeNode.h"

namespace GRT{

class ClusterTree : public Tree, public Clusterer
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
     @param const double minRMSErrorPerNode: sets the minimum RMS error that allowed per node, if the RMS error is below that, the node will become a leafNode. Default value = 0.01
     */
    ClusterTree(const UINT numSplittingSteps=100,const UINT minNumSamplesPerNode=5,const UINT maxDepth=10,const bool removeFeaturesAtEachSpilt = false,const UINT trainingMode = BEST_ITERATIVE_SPILT,const bool useScaling=false,const double minRMSErrorPerNode = 0.01);
    
    /**
     Defines the copy constructor.
     
     @param const ClusterTree &rhs: the instance from which all the data will be copied into this instance
     */
    ClusterTree(const ClusterTree &rhs);
    
    /**
     Default Destructor
     */
     virtual ~ClusterTree(void);
    
    /**
     Defines how the data from the rhs ClusterTree should be copied to this ClusterTree
     
     @param const ClusterTreev &rhs: another instance of a ClusterTree
     @return returns a pointer to this instance of the ClusterTree
     */
    ClusterTree &operator=(const ClusterTree &rhs);

    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setRegressifier(...) method is called.
     It clones the data from the Base Class Regressifier pointer (which should be pointing to an RegressionTree instance) into this instance
     
     @param Regressifier *regressifier: a pointer to the Regressifier Base Class, this should be pointing to another RegressionTree instance
     @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Clusterer *cluster);
    
    /**
     This trains the RegressionTree model, using the labelled regression data.
     This overrides the train function in the Regressifier base class.
     
     @param RegressionData trainingData: a reference to the training data
     @return returns true if the RegressionTree model was trained, false otherwise
    */
    virtual bool train_(MatrixDouble &trainingData);
    
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
     
     @param fstream &file: a reference to the file the RegressionTree model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
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
    ClusterTreeNode* deepCopyTree() const;
    
    /**
     Gets a pointer to the regression tree. NULL will be returned if the decision tree model has not be trained.
     
     @return returns a const pointer to the regression tree
     */
    const ClusterTreeNode* getTree() const;
    
    /**
     Gets the predicted cluster label from the most recent call to predict( ... ).  
     The cluster label will be zero if the model has been trained but no prediction has been run.
     
     @return returns the most recent predicted cluster label
     */
    UINT getPredictedClusterLabel() const;
    
    /**
     Gets the minimum root mean squared error value that needs to be exceeded for the tree to continue growing at a specific node.
     If the RMS error is below this value then the node will be made into a leaf node.
     
     @return returns the minimum RMS error per node
     */
    double getMinRMSErrorPerNode() const;
    
    /**
     Sets the minimum RMS error that needs to be exceeded for the tree to continue growing at a specific node.
     
     @return returns true if the parameter was updated
     */
    bool setMinRMSErrorPerNode(const double minRMSErrorPerNode);
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::saveModelToFile;
    using MLBase::loadModelFromFile;
    using MLBase::train;
    using MLBase::predict;
    
protected:
    double minRMSErrorPerNode;
    
    ClusterTreeNode* buildTree( const MatrixDouble &trainingData, ClusterTreeNode *parent, vector< UINT > features, UINT &clusterLabel, UINT nodeID );
    bool computeBestSpilt( const MatrixDouble &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError );
    bool computeBestSpiltBestIterativeSpilt( const MatrixDouble &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError );
    bool computeBestSpiltBestRandomSpilt( const MatrixDouble &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError );

    static RegisterClustererModule< ClusterTree > registerModule;
    
};

} //End of namespace GRT

#endif //GRT_CLUSTER_TREE_HEADER

