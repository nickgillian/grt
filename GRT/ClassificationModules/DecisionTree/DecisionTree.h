/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a basic Decision Tree classifier.  Decision Trees are conceptually simple
 classifiers that work well on even complex classification tasks.  Decision Trees partition the feature
 space into a set of rectangular regions, classifying a new datum by finding which region it belongs to.  
 
 @remark This implementation is based on Ross Quinlan's ID3 Decision Tree algorithm: http://en.wikipedia.org/wiki/ID3_algorithm
 
 @example ClassificationModulesExamples/DecisionTreeExample/DecisionTreeExample.cpp
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

namespace GRT{

class DecisionTree : public Tree, public Classifier
{
public:
    /**
     Default Constructor

     @param const DecisionTreeNode &decisionTreeNode: sets the type of decision tree node that will be used when training a new decision tree model. Default: DecisionTreeClusterNode
     @param UINT minNumSamplesPerNode: sets the minimum number of samples that are allowed per node, if the number of samples is below that, the node will become a leafNode.  Default value = 5
     @param UINT maxDepth: sets the maximum depth of the tree. Default value = 10
     @param bool removeFeaturesAtEachSpilt: sets if a feature is removed at each spilt so it can not be used again. Default value = false
     @param UINT trainingMode: sets the training mode, this should be one of the TrainingMode enums. Default value = BEST_ITERATIVE_SPILT
     @param UINT numSplittingSteps: sets the number of steps that will be used to search for the best spliting value for each node. Default value = 100
     @param bool useScaling: sets if the training and real-time data should be scaled between [0 1]. Default value = false
     */
	DecisionTree(const DecisionTreeNode &decisionTreeNode = DecisionTreeClusterNode(),const UINT minNumSamplesPerNode=5,const UINT maxDepth=10,const bool removeFeaturesAtEachSpilt = false,const UINT trainingMode = BEST_ITERATIVE_SPILT,const UINT numSplittingSteps=100,const bool useScaling=false );
    
    /**
     Defines the copy constructor.
     
     @param const DecisionTree &rhs: the instance from which all the data will be copied into this instance
     */
    DecisionTree(const DecisionTree &rhs);
    
    /**
     Default Destructor
     */
	virtual ~DecisionTree(void);
    
    /**
     Defines how the data from the rhs DecisionTree should be copied to this DecisionTree
     
     @param const DecisionTree &rhs: another instance of a DecisionTree
     @return returns a pointer to this instance of the DecisionTree
     */
	DecisionTree &operator=(const DecisionTree &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.  
     It clones the data from the Base Class Classifier pointer (which should be pointing to an DecisionTree instance) into this instance
     
     @param Classifier *classifier: a pointer to the Classifier Base Class, this should be pointing to another DecisionTree instance
     @return returns true if the clone was successfull, false otherwise
    */
	virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
     This trains the DecisionTree model, using the labelled classification data.
     This overrides the train function in the Classifier base class.
     
     @param ClassificationData trainingData: a reference to the training data
     @return returns true if the DecisionTree model was trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the Classifier base class.
     
     @param VectorDouble inputVector: the input vector to classify
     @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorDouble &inputVector);
    
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
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the DecisionTree model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained DecisionTree model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the DecisionTree model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    /**
     This function adds the current model to the formatted stream.
     This function should be overwritten by the derived class.
     
     @param ostream &file: a reference to the stream the model will be added to
     @return returns true if the model was added successfully, false otherwise
     */
    virtual bool getModel(ostream &stream) const;

    /**
     Deep copies the decision tree, returning a pointer to the new decision tree. 
     The user is in charge of cleaning up the memory so must delete the pointer when they no longer need it.
     NULL will be returned if the tree could not be copied.
     
     @return returns a pointer to a deep copy of the decision tree
     */
    DecisionTreeNode* deepCopyTree() const;
    
    /**
     Gets a pointer to the decision tree. NULL will be returned if the decision tree model has not be trained.
     
     @return returns a const pointer to the decision tree
     */
    const DecisionTreeNode* getTree() const;
    
    /**
     Gets a pointer to the decision tree node. NULL will be returned if the decision tree node has not been set.
     
     @return returns a pointer to a deep copy of the decision tree node
     */
    DecisionTreeNode* deepCopyDecisionTreeNode() const;
    
    /**
     Sets the decision tree node, this will be used as the starting node the next time the DecisionTree model is trained.
     
     @return returns true if the decision tree node was updated, false otherwise
     */
    bool setDecisionTreeNode( const DecisionTreeNode &node );
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::saveModelToFile;
    using MLBase::loadModelFromFile;
    using MLBase::train;
    using MLBase::predict;
    using MLBase::print;
    
protected:
    bool loadLegacyModelFromFile_v1( fstream &file );
    bool loadLegacyModelFromFile_v2( fstream &file );
    bool loadLegacyModelFromFile_v3( fstream &file );
    
    DecisionTreeNode* buildTree(ClassificationData &trainingData, DecisionTreeNode *parent, vector< UINT > features, const vector< UINT > &classLabels, UINT nodeID );
    double getNodeDistance( const VectorDouble &x, const UINT nodeID );
    double getNodeDistance( const VectorDouble &x, const VectorDouble &y );
    
    DecisionTreeNode* decisionTreeNode;
    std::map< UINT, VectorDouble > nodeClusters;
    VectorDouble classClusterMean;
    VectorDouble classClusterStdDev;
    static RegisterClassifierModule< DecisionTree > registerModule;
    
};

} //End of namespace GRT

#endif //GRT_DECISION_TREE_HEADER

