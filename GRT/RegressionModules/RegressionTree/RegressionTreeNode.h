/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This file implements a RegressionTreeNode, which is a specific type of node used for a RegressionTree.
 
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

#ifndef GRT_REGRESSION_TREE_NODE_HEADER
#define GRT_REGRESSION_TREE_NODE_HEADER

#include "../../CoreAlgorithms/Tree/Node.h"

namespace GRT{
    
class RegressionTreeNode : public Node{
public:
    /**
     Default Constructor. Sets all the pointers to NULL.
     */
    RegressionTreeNode(){
        nodeType = "RegressionTreeNode";
        parent = NULL;
        leftChild = NULL;
        rightChild = NULL;
        clear();
    }
    
    /**
     Default Destructor. Cleans up any memory.
     */
    virtual ~RegressionTreeNode(){
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
    virtual bool predict(const VectorDouble &x){
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
    virtual bool predict(const VectorDouble &x,VectorDouble &y){
        
        if( isLeafNode ){
            y = this->regressionData;
            return true;
        }
        
        if( leftChild == NULL && rightChild == NULL )
            return false;
        
        if( predict( x ) ){
            if( rightChild )
                return rightChild->predict( x, y );
        }else{
            if( leftChild )
                return leftChild->predict( x, y );
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
        regressionData.clear();
        
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
        cout << tab << "RegressionData: ";
        for(UINT i=0; i<regressionData.size(); i++){
            cout << regressionData[i] << "\t";
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
        
        RegressionTreeNode *node = new RegressionTreeNode;
        
        if( node == NULL ){
            return NULL;
        }
        
        //Copy this node into the node
        node->depth = this->depth;
        node->isLeafNode = this->isLeafNode;
        node->nodeSize = this->nodeSize;
        node->featureIndex = this->featureIndex;
        node->threshold = this->threshold;
        node->regressionData = this->regressionData;
        
        //Recursively deep copy the left child
        if( this->leftChild ){
            node->leftChild = this->leftChild->deepCopyNode();
            node->leftChild->setParent( node );
        }
        
        //Recursively deep copy the right child
        if( this->rightChild ){
            node->rightChild = this->rightChild->deepCopyNode();
            node->rightChild->setParent( node );
        }
        
        return (Node*)node;
    }
    
    RegressionTreeNode* deepCopyTree() const{
        RegressionTreeNode *node = (RegressionTreeNode*)deepCopyNode();
        return node;
    }
    
    /**
     This function sets the Decision Tree Node.
     
     @param const UINT nodeSize: sets the node size, this is the number of training samples at that node
     @param const UINT featureIndex: sets the index of the feature that should be used for the threshold spilt
     @param const double threshold: set the threshold value used for the spilt
     @param const VectorDouble &regressionData: the regression data at this node
     @return returns true if the node was set, false otherwise
     */
    bool set(const UINT nodeSize,const UINT featureIndex,const double threshold,const VectorDouble &regressionData){
        this->nodeSize = nodeSize;
        this->featureIndex = featureIndex;
        this->threshold = threshold;
        this->regressionData = regressionData;
        return true;
    }
    
protected:
    /**
     This saves the ClusterTreeNode custom parameters to a file. It will be called automatically by the Node base class
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
        
        //Save the custom ClusterTreeNode parameters
        file << "NodeSize: " << nodeSize << endl;
        file << "FeatureIndex: " << featureIndex << endl;
        file << "Threshold: " << threshold << endl;
        file << "RegressionDataSize: " << regressionData.size() << endl;
        file << "RegressionData: ";
        for(unsigned int i=0; i<regressionData.size(); i++){
            file << regressionData[i] << " ";
        }
        file << endl;
        
        return true;
    }
    
    /**
     This loads the ClusterTreeNode parameters from a file.
     
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
        UINT regressionDataSize = 0;
        
        //Load the custom ClusterTreeNode Parameters
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
        if( word != "RegressionDataSize:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find RegressionDataSize header!" << endl;
            return false;
        }
        file >> regressionDataSize;
        regressionData.resize(regressionDataSize);
        
        file >> word;
        if( word != "RegressionData:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find RegressionData header!" << endl;
            return false;
        }
        for(unsigned int i=0; i<regressionData.size(); i++){
            file >> regressionData[i];
        }
        
        return true;
    }
    
    UINT nodeSize;
    UINT featureIndex;
    double threshold;
    VectorDouble regressionData;
    
    static RegisterNode< RegressionTreeNode > registerModule;
};

} //End of namespace GRT

#endif //GRT_REGRESSION_TREE_NODE_HEADER

