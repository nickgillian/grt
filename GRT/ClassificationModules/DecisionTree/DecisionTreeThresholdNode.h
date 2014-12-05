/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This file implements a DecisionTreeThresholdNode, which is a specific type of node used for a DecisionTree.
 
 @description The node creates a spilt for each node in a DecisionTree by finding the threshold and feature that minimizes
 the error on the training data at that node.  The threshold is found by iterating over the range of the training data and
 finding the threshold and feature index that results in the minimum error.
 
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

#ifndef GRT_DECISION_TREE_THRESHOLD_NODE_HEADER
#define GRT_DECISION_TREE_THRESHOLD_NODE_HEADER

#include "DecisionTreeNode.h"

namespace GRT{
    
class DecisionTreeThresholdNode : public DecisionTreeNode{
public:
    /**
     Default Constructor. Sets all the pointers to NULL.
     */
    DecisionTreeThresholdNode();
    
    /**
     Default Destructor. Cleans up any memory.
     */
    virtual ~DecisionTreeThresholdNode();
    
    /**
     This function predicts if the input is greater than or equal to the nodes threshold.
     If the input is greater than or equal to the nodes threshold then this function will return true, otherwise it will return false.
     
     NOTE: The threshold and featureIndex should be set first BEFORE this function is called. The threshold and featureIndex can be set by
     training the node through the DecisionTree class.
     
     @param const VectorDouble &x: the input vector that will be used for the prediction
     @return returns true if the input is greater than or equal to the nodes threshold, false otherwise
     */
    virtual bool predict(const VectorDouble &x);
    
    /**
     This functions cleans up any dynamic memory assigned by the node.
     It will recursively clear the memory for the left and right child nodes.
     
     @return returns true of the node was cleared correctly, false otherwise
     */
    virtual bool clear();
    
    /**
     This functions prints the node data to std::out.
     It will recursively print all the child nodes.
     
     @return returns true if the data was printed correctly, false otherwise
     */
    virtual bool print() const;
    
    /**
     This function adds the current model to the formatted stream.
     This function should be overwritten by the derived class.
     
     @param ostream &file: a reference to the stream the model will be added to
     @return returns true if the model was added successfully, false otherwise
     */
    virtual bool getModel(ostream &stream) const;
    
    /**
     This function returns a deep copy of the DecisionTreeThresholdNode and all it's children.
     The user is responsible for managing the dynamic data that is returned from this function as a pointer.
     
     @return returns a pointer to a deep copy of the DecisionTreeThresholdNode, or NULL if the deep copy was not successful
     */
    virtual Node* deepCopyNode() const;
    
    /**
     This function returns a deep copy of the DecisionTreeNode and all it's children.
     The user is responsible for managing the dynamic data that is returned from this function as a pointer.
     
     @return returns a pointer to a deep copy of the DecisionTreeThresholdNode, or NULL if the deep copy was not successful
     */
    DecisionTreeThresholdNode* deepCopy() const;
    
    /**
     This function returns the featureIndex, this is index in the input data that the decision threshold is computed on.
     
     @return returns the featureIndex
     */
    UINT getFeatureIndex() const;
    /**
     This function returns the threshold, this is the value used to compute the decision threshold.
     
     @return returns the threshold
     */
    double getThreshold() const;
    
    /**
     This function sets the Decision Tree Threshold Node.
     
     @param const UINT nodeSize: sets the node size, this is the number of training samples at that node
     @param const UINT featureIndex: sets the index of the feature that should be used for the threshold spilt
     @param const double threshold: set the threshold value used for the spilt
     @param const VectorDouble &classProbabilities: the vector of class probabilities at this node
     @return returns true if the node was set, false otherwise
     */
    bool set(const UINT nodeSize,const UINT featureIndex,const double threshold,const VectorDouble &classProbabilities);
    
protected:
    
    virtual bool computeBestSpiltBestIterativeSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError );
    
    virtual bool computeBestSpiltBestRandomSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError );
    
    /**
     This saves the DecisionTreeNode custom parameters to a file. It will be called automatically by the Node base class
     if the saveToFile function is called.
     
     @param fstream &file: a reference to the file the parameters will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveParametersToFile(fstream &file) const;
    
    /**
     This loads the Decision Tree Node parameters from a file.
     
     @param fstream &file: a reference to the file the parameters will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadParametersFromFile(fstream &file);
    
    UINT featureIndex;
    double threshold;
    
    static RegisterNode< DecisionTreeThresholdNode > registerModule;
};

} //End of namespace GRT

#endif //GRT_DECISION_TREE_THRESHOLD_NODE_HEADER

