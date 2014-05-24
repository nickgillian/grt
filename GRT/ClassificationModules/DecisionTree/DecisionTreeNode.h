/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This file implements a DecisionTreeNode, which is a specific type of node used for a DecisionTree.
 
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

#include "../../Util/Node.h"

namespace GRT{
    
class DecisionTreeNode : public Node{
public:
    /**
     Default Constructor. Sets all the pointers to NULL.
     */
    DecisionTreeNode(){
        nodeType = "DecisionTreeNode";
        parent = NULL;
        leftChild = NULL;
        rightChild = NULL;
        clear();
    }
    
    /**
     Default Destructor. Cleans up any memory.
     */
    virtual ~DecisionTreeNode(){
        clear();
    }
    
    /**
     This function predicts if the input is greater than or equal to the nodes threshold.
     If the input is greater than or equal to the nodes threshold then this function will return true, otherwise it will return false.
     
     NOTE: The threshold and featureIndex should be set first BEFORE this function is called. The threshold and featureIndex can be set by
     training the node through the DecisionTree class.
     
     @param const VectorDouble &x: the input vector that will be used for the prediction
     @return returns true if the input is greater than or equal to the nodes threshold, false otherwise
     */
    virtual bool predict(const VectorDouble &x) const{
        if( x[ featureIndex ] >= threshold ) return true;
        return false;
    }
    
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
    virtual bool predict(const VectorDouble &x,VectorDouble &classLikelihoods) const{
        
        if( isLeafNode ){
            classLikelihoods = classProbabilities;
            return true;
        }
        
        if( leftChild == NULL && rightChild == NULL )
            return false;
        
        if( predict( x ) ){
            if( rightChild )
                return rightChild->predict( x, classLikelihoods );
        }else{
            if( leftChild )
                return leftChild->predict( x, classLikelihoods );
        }
        
        return false;
    }
    
    /**
     This functions cleans up any dynamic memory assigned by the node.
     It will recursively clear the memory for the left and right child nodes.
     
     @return returns true of the node was cleared correctly, false otherwise
     */
    virtual bool clear(){
        
        //Call the base class clear function
        Node::clear();
        
        nodeSize = 0;
        featureIndex = 0;
        threshold = 0;
        classProbabilities.clear();
        
        return true;
    }
    
    /**
     This functions prints the node data to std::out.
     It will recursively print all the child nodes.
     
     @return returns true if the data was printed correctly, false otherwise
     */
    virtual bool print() const{
        
        string tab = "";
        for(UINT i=0; i<depth; i++) tab += "\t";
        
        cout << tab << "depth: " << depth << " nodeSize: " << nodeSize << " featureIndex: " << featureIndex << " threshold " << threshold << " isLeafNode: " << isLeafNode << endl;
        cout << tab << "ClassProbabilities: ";
        for(UINT i=0; i<classProbabilities.size(); i++){
            cout << classProbabilities[i] << "\t";
        }
        cout << endl;
        
        if( leftChild != NULL ){
            cout << tab << "LeftChild: " << endl;
            leftChild->print();
        }
        
        if( rightChild != NULL ){
            cout << tab << "RightChild: " << endl;
            rightChild->print();
        }
        
        return true;
    }
    
    /**
     This function returns a deep copy of the DecisionTreeNode and all it's children.
     The user is responsible for managing the dynamic data that is returned from this function as a pointer.
     
     @return returns a pointer to a deep copy of the DecisionTreeNode, or NULL if the deep copy was not successful
     */
    virtual Node* deepCopyNode() const{
        
        DecisionTreeNode *node = new DecisionTreeNode;
        
        if( node == NULL ){
            return NULL;
        }
        
        //Copy this node into the node
        node->depth = depth;
        node->isLeafNode = isLeafNode;
        node->nodeSize = nodeSize;
        node->featureIndex = featureIndex;
        node->threshold = threshold;
        node->classProbabilities = classProbabilities;
        
        //Recursively deep copy the left child
        if( leftChild ){
            node->leftChild = leftChild->deepCopyNode();
            node->leftChild->setParent( node );
        }
        
        //Recursively deep copy the right child
        if( rightChild ){
            node->rightChild = rightChild->deepCopyNode();
            node->rightChild->setParent( node );
        }
        
        return (Node*)node;
    }
    
    DecisionTreeNode* deepCopyTree() const{
        DecisionTreeNode *node = (DecisionTreeNode*)deepCopyNode();
        return node;
    }
    
    /**
     This function returns the nodeSize, this is the number of training samples that reached the node during the training phase.
     
     @return returns the nodeSize
     */
    UINT getNodeSize() const{
        return nodeSize;
    }
    
    /**
     This function returns the featureIndex, this is index in the input data that the decision threshold is computed on.
     
     @return returns the featureIndex
     */
    UINT getFeatureIndex() const{
        return featureIndex;
    }
    
    /**
     This function returns the number of classes in the class probabilities vector.
     
     @return returns the number of classes in the class probabilities vector
     */
    UINT getNumClasses() const{
        return (UINT)classProbabilities.size();
    }
    
    /**
     This function returns the threshold, this is the value used to compute the decision threshold.
     
     @return returns the threshold
     */
    double getThreshold() const{
        return threshold;
    }
    
    /**
     This function returns the class probabilities vector.
     
     @return returns the classProbabilities vector
     */
    VectorDouble getClassProbabilities() const{
        return classProbabilities;
    }
    
    /**
     This function sets the Decision Tree Node.
     
     @param const UINT nodeSize: sets the node size, this is the number of training samples at that node
     @param const UINT featureIndex: sets the index of the feature that should be used for the threshold spilt
     @param const double threshold: set the threshold value used for the spilt
     @param const VectorDouble &classProbabilities: the vector of class probabilities at this node
     @return returns true if the node was set, false otherwise
     */
    bool set(const UINT nodeSize,const UINT featureIndex,const double threshold,const VectorDouble &classProbabilities){
        this->nodeSize = nodeSize;
        this->featureIndex = featureIndex;
        this->threshold = threshold;
        this->classProbabilities = classProbabilities;
        return true;
    }
    
protected:
    /**
     This saves the DecisionTreeNode custom parameters to a file. It will be called automatically by the Node base class
     if the saveToFile function is called.
     
     @param fstream &file: a reference to the file the parameters will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveParametersToFile(fstream &file) const{
        
        if(!file.is_open())
        {
            errorLog << "saveParametersToFile(fstream &file) - File is not open!" << endl;
            return false;
        }
        
        //Save the custom DecisionTreeNode parameters
        file << "NodeSize: " << nodeSize << endl;
        file << "FeatureIndex: " << featureIndex << endl;
        file << "Threshold: " << threshold << endl;
        file << "NumClasses: " << classProbabilities.size() << endl;
        file << "ClassProbabilities: ";
        for(UINT i=0; i<classProbabilities.size(); i++){
            file << classProbabilities[i];
            if( i < classProbabilities.size()-1 ) file << "\t";
            else file << endl;
        }
        
        return true;
    }
    
    /**
     This loads the Decision Tree Node parameters from a file.
     
     @param fstream &file: a reference to the file the parameters will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadParametersFromFile(fstream &file){
        
        if(!file.is_open())
        {
            errorLog << "loadFromFile(fstream &file) - File is not open!" << endl;
            return false;
        }
        
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
        if( word != "FeatureIndex:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find FeatureIndex header!" << endl;
            return false;
        }
        file >> featureIndex;
        
        file >> word;
        if( word != "Threshold:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find Threshold header!" << endl;
            return false;
        }
        file >> threshold;
        
        file >> word;
        if( word != "NumClasses:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find NumClasses header!" << endl;
            return false;
        }
        file >> numClasses;
        classProbabilities.resize( numClasses );
        
        file >> word;
        if( word != "ClassProbabilities:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find ClassProbabilities header!" << endl;
            return false;
        }
        for(UINT i=0; i<numClasses; i++){
            file >> classProbabilities[i];
        }
        
        return true;
    }
    
    UINT nodeSize;
    UINT featureIndex;
    double threshold;
    VectorDouble classProbabilities;
    
    static RegisterNode< DecisionTreeNode > registerModule;
};

} //End of namespace GRT

#endif //GRT_DECISION_TREE_NODE_HEADER

