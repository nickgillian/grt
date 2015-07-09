/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This file implements a DecisionTreeNode, which is a specific base node used for a DecisionTree.
 
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

#ifndef GRT_DECISION_TREE_NODE_HEADER
#define GRT_DECISION_TREE_NODE_HEADER

#include "../../CoreAlgorithms/Tree/Node.h"
#include "../../CoreAlgorithms/Tree/Tree.h"
#include "../../DataStructures/ClassificationData.h"

namespace GRT{
    
class DecisionTreeNode : public Node{
public:
    /**
     Default Constructor. Sets all the pointers to NULL.
     */
    DecisionTreeNode();
    
    /**
     Default Destructor. Cleans up any memory.
     */
    virtual ~DecisionTreeNode();
    
    /**
     This function recursively predicts if the probability of the input vector.  
     If this node is a leaf node, then the class likelihoods are equal to the class probabilities at the leaf node.
     If this node is not a leaf node, then this function will recursively call the predict function on either the left or right children
     until a leaf node is reached.
     
     NOTE: The threshold, featureIndex and classProbabilities should be set first BEFORE this function is called. The threshold, featureIndex 
     and classProbabilities can be set by training the node through the DecisionTree class.
     
     @param const VectorDouble &x: the input vector that will be used for the prediction
     @param VectorDouble &classLikelihoods: a reference to a vector that will store the class probabilities
     @return returns true if the input is greater than or equal to the nodes threshold, false otherwise
     */
    virtual bool predict(const VectorDouble &x,VectorDouble &classLikelihoods);
    
    /**
     This function calls the best spliting algorithm based on the current trainingMode.  
     
     This function will return true if the best spliting algorithm found a split, false otherwise.
     
     @param const UINT &trainingMode: the training mode to use, this should be one of the
     @param const UINT &numSplittingSteps: sets the number of iterations that will be used to search for the best threshold
     @param const ClassificationData &trainingData: the training data to use for the best split search
     @param const const vector< UINT > &features: a vector containing the indexs of the features that can be used for the search
     @param const vector< UINT > &classLabels: a vector containing the class labels for the search
     @param UINT &featureIndex: this will store the best feature index found during the search
     @param double &minError: this will store the minimum error found during the search
     @return returns true if the best spliting algorithm found a split, false otherwise
     */
    virtual bool computeBestSpilt( const UINT &trainingMode, const UINT &numSplittingSteps,const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError );

    /**
     This functions cleans up any dynamic memory assigned by the node.
     It will recursively clear the memory for the left and right child nodes.
     
     @return returns true of the node was cleared correctly, false otherwise
     */
    virtual bool clear();
    
    /**
     This function adds the current model to the formatted stream.
     This function should be overwritten by the derived class.
     
     @param ostream &file: a reference to the stream the model will be added to
     @return returns true if the model was added successfully, false otherwise
     */
    virtual bool getModel(ostream &stream) const;
    
    /**
     This function returns a deep copy of the DecisionTreeNode and all it's children.
     The user is responsible for managing the dynamic data that is returned from this function as a pointer.
     
     @return returns a pointer to a deep copy of the DecisionTreeNode, or NULL if the deep copy was not successful
     */
    virtual Node* deepCopyNode() const;
    
    /**
     This function returns a deep copy of the DecisionTreeNode and all it's children.
     The user is responsible for managing the dynamic data that is returned from this function as a pointer.
     
     @return returns a pointer to a deep copy of the DecisionTreeNode, or NULL if the deep copy was not successful
     */
    DecisionTreeNode* deepCopy() const;
    
    /**
     This function returns the nodeSize, this is the number of training samples that reached the node during the training phase.
     
     @return returns the nodeSize
     */
    UINT getNodeSize() const;
    
    /**
     This function returns the number of classes in the class probabilities vector.
     
     @return returns the number of classes in the class probabilities vector
     */
    UINT getNumClasses() const;
    
    /**
     This function returns the class probabilities vector.
     
     @return returns the classProbabilities vector
     */
    VectorDouble getClassProbabilities() const;
    
    /**
     This function sets the Decision Tree Node as a leaf node.
     
     @param const UINT nodeSize: sets the node size, this is the number of training samples at that node
     @param const VectorDouble &classProbabilities: the vector of class probabilities at this node
     @return returns true if the node was updated, false otherwise
     */
    bool setLeafNode( const UINT nodeSize, const VectorDouble &classProbabilities );
    
    /**
     This function sets the Decision Tree Node nodeSize.
     
     @param const UINT nodeSize: sets the node size, this is the number of training samples at that node
     @return returns true if the node size was set, false otherwise
     */
    bool setNodeSize(const UINT nodeSize);
    
    /**
     This function sets the Decision Tree Node class probabilities.
     
     @param const VectorDouble &classProbabilities: the vector of class probabilities at this node
     @return returns true if the node was set classProbabilities, false otherwise
     */
    bool setClassProbabilities(const VectorDouble &classProbabilities);
    
    static UINT getClassLabelIndexValue(UINT classLabel,const vector< UINT > &classLabels);
    
    using Node::predict;
    
protected:
    virtual bool computeBestSpiltBestIterativeSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){
        
        errorLog << "computeBestSpiltBestIterativeSpilt(...) - Base class not overwritten!" << endl;
        
        return false;
    }
    
    virtual bool computeBestSpiltBestRandomSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){
        
        errorLog << "computeBestSpiltBestRandomSpilt(...) - Base class not overwritten!" << endl;
        
        return false;
    }
    
    /**
     This saves the DecisionTreeNode custom parameters to a file. It will be called automatically by the Node base class
     if the saveToFile function is called.
     
     @param fstream &file: a reference to the file the parameters will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveParametersToFile(fstream &file) const{
        
        if( !file.is_open() )
        {
            errorLog << "saveParametersToFile(fstream &file) - File is not open!" << endl;
            return false;
        }
        
        //Save the custom DecisionTreeNode parameters
        file << "NodeSize: " << nodeSize << endl;
        file << "NumClasses: " << classProbabilities.size() << endl;
        file << "ClassProbabilities: ";
        if( classProbabilities.size() > 0 ){
            for(UINT i=0; i<classProbabilities.size(); i++){
                file << classProbabilities[i];
                if( i < classProbabilities.size()-1 ) file << "\t";
                else file << endl;
            }
        }
        
        return true;
    }
    
    /**
     This loads the Decision Tree Node parameters from a file.
     
     @param fstream &file: a reference to the file the parameters will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadParametersFromFile(fstream &file){
        
        if( !file.is_open() )
        {
            errorLog << "loadParametersFromFile(fstream &file) - File is not open!" << endl;
            return false;
        }
        
        classProbabilities.clear();
        
        string word;
        UINT numClasses;
        
        //Load the custom DecisionTreeNode Parameters
        file >> word;
        if( word != "NodeSize:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find NodeSize header!" << endl;
            return false;
        }
        file >> nodeSize;
        
        file >> word;
        if( word != "NumClasses:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find NumClasses header!" << endl;
            return false;
        }
        file >> numClasses;
        if( numClasses > 0 )
            classProbabilities.resize( numClasses );
        
        file >> word;
        if( word != "ClassProbabilities:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find ClassProbabilities header!" << endl;
            return false;
        }
        if( numClasses > 0 ){
            for(UINT i=0; i<numClasses; i++){
                file >> classProbabilities[i];
            }
        }
        
        return true;
    }
    
    UINT nodeSize;
    VectorDouble classProbabilities;
    
    static RegisterNode< DecisionTreeNode > registerModule;
};

} //End of namespace GRT

#endif //GRT_DECISION_TREE_NODE_HEADER

