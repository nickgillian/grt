/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0

 @brief This class contains the main Node base class.
 
 */

/*
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

#ifndef GRT_NODE_HEADER
#define GRT_NODE_HEADER

#include "GRTCommon.h"
#include "../CoreModules/GRTBase.h"

namespace GRT{
    
class Node : public GRTBase{
public:
    /**
     Default Constructor. Sets all the pointers to NULL.
     */
    Node(){
        nodeType = "";
        parent = NULL;
        leftChild = NULL;
        rightChild = NULL;
        clear();
    }
    
    /**
     Default Destructor. Cleans up any memory.
     */
    virtual ~Node(){
        clear();
    }
    
    /**
     This function predicts if the input is greater than or equal to the nodes threshold.
     This function should be overwritten by the inheriting class.
     
     @param const VectorDouble &x: the input vector that will be used for the prediction
     @return returns true if the input is greater than or equal to the nodes threshold, false otherwise
     */
    virtual bool predict(const VectorDouble &x) const{
        return false;
    }
    
    /**
     This function recursively predicts if the probability of the input vector.  
     This function should be overwritten by the inheriting class.
  
     @param const VectorDouble &x: the input vector that will be used for the prediction
     @param VectorDouble &y: a reference to a vector that will store the results
     @return returns true if the input is greater than or equal to the nodes threshold, false otherwise
     */
    virtual bool predict(const VectorDouble &x,VectorDouble &y) const{
        
        return false;
    }
    
    /**
     This functions cleans up any dynamic memory assigned by the node.
     It will recursively clear the memory for the left and right child nodes.
     
     @return returns true of the node was cleared correctly, false otherwise
     */
    virtual bool clear(){
        
        parent = NULL;
        
        if( leftChild != NULL ){
            leftChild->clear();
            delete leftChild;
            leftChild = NULL;
        }
        
        if( rightChild != NULL ){
            rightChild->clear();
            delete rightChild;
            rightChild = NULL;
        }
        
        depth = 0;
        isLeafNode = false;
        
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
        
        cout << tab << "depth: " << depth << " isLeafNode: " << isLeafNode << endl;
        
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
     This saves the Node to a file.
     
     @param fstream &file: a reference to the file the Node model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveToFile(fstream &file) const{
        
        if(!file.is_open())
        {
            errorLog << "saveToFile(fstream &file) - File is not open!" << endl;
            return false;
        }
        
        file << "NodeType: " << nodeType << endl;
        file << "Depth: " << depth << endl;
        file << "IsLeafNode: " << isLeafNode << endl;
        file << "HasLeftChild: " << getHasLeftChild() << endl;
        file << "HasRightChild: " << getHasRightChild() << endl;
        
        //If there is a left child then load the left child's data
        if( getHasLeftChild() ){
            file << "LeftChild\n";
            if( !leftChild->saveToFile( file ) ){
                errorLog << "saveToFile(fstream &file) - Failed to save left child at depth: " << depth << endl;
                return false;
            }
        }
        
        //If there is a right child then load the right child's data
        if( getHasRightChild() ){
            file << "RightChild\n";
            if( !rightChild->saveToFile( file ) ){
                errorLog << "saveToFile(fstream &file) - Failed to save right child at depth: " << depth << endl;
                return false;
            }
        }
        
        //Save the custom parameters to the file
        if( !saveParametersToFile( file ) ){
            errorLog << "saveToFile(fstream &file) - Failed to save parameters to file at depth: " << depth << endl;
            return false;
        }
        
        return true;
    }
    
    /**
     This loads the Node from a file.
     
     @param fstream &file: a reference to the file the Node model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadFromFile(fstream &file){
        
        //Clear any previous nodes
        clear();
        
        if(!file.is_open())
        {
            errorLog << "loadFromFile(fstream &file) - File is not open!" << endl;
            return false;
        }
        
        string word;
        bool hasLeftChild = false;
        bool hasRightChild = false;
        
        file >> word;
        if( word != "NodeType:" ){
            errorLog << "loadFromFile(fstream &file) - Failed to find Node header!" << endl;
            return false;
        }
        file >> nodeType;
        
        file >> word;
        if( word != "Depth:" ){
            errorLog << "loadFromFile(fstream &file) - Failed to find Depth header!" << endl;
            return false;
        }
        file >> depth;
 
        file >> word;
        if( word != "IsLeafNode:" ){
            errorLog << "loadFromFile(fstream &file) - Failed to find IsLeafNode header!" << endl;
            return false;
        }
        file >> isLeafNode;
        
        file >> word;
        if( word != "HasLeftChild:" ){
            errorLog << "loadFromFile(fstream &file) - Failed to find HasLeftChild header!" << endl;
            return false;
        }
        file >> hasLeftChild;
        
        file >> word;
        if( word != "HasRightChild:" ){
            errorLog << "loadFromFile(fstream &file) - Failed to find HasRightChild header!" << endl;
            return false;
        }
        file >> hasRightChild;
        
        if( hasLeftChild ){
            file >> word;
            if( word != "LeftChild" ){
                errorLog << "loadFromFile(fstream &file) - Failed to find LeftChild header!" << endl;
                return false;
            }
            leftChild = createNewInstance();
            leftChild->setParent( this );
            if( !leftChild->loadFromFile(file) ){
                errorLog << "loadFromFile(fstream &file) - Failed to load left child at depth: " << depth << endl;
                return false;
            }
        }
        
        if( hasRightChild ){
            file >> word;
            if( word != "RightChild" ){
                errorLog << "loadFromFile(fstream &file) - Failed to find RightChild header!" << endl;
                cout << "WORD: " << word << endl;
                cout << "Depth: " << depth << endl;
                return false;
            }
            rightChild = createNewInstance();
            rightChild->setParent( this );
            if( !rightChild->loadFromFile( file ) ){
                errorLog << "loadFromFile(fstream &file) - Failed to load right child at depth: " << depth << endl;
                return false;
            }
        }
        
        //Load the custom parameters from a file
        if( !loadParametersFromFile( file ) ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to load parameters from file at depth: " << depth << endl;
            return false;
        }
        
        return true;
    }
    
    /**
     This function returns a deep copy of the Node and all it's children.
     The user is responsible for managing the dynamic data that is returned from this function as a pointer.
     
     @return returns a pointer to a deep copy of the Node, or NULL if the deep copy was not successful
     */
    virtual Node* deepCopyNode() const{
        
        Node *node = createNewInstance();
        
        if( node == NULL ){
            return NULL;
        }
        
        //Copy this node into the node
        node->setDepth( depth );
        node->setIsLeafNode( isLeafNode );
        
        //Recursively deep copy the left child
        if( getHasLeftChild() ){
            node->setLeftChild( leftChild->deepCopyNode() );
            node->leftChild->setParent( node );
        }
        
        //Recursively deep copy the right child
        if( getHasRightChild() ){
            node->setRightChild( rightChild->deepCopyNode() );
            node->rightChild->setParent( node );
        }
        
        return node;
    }
    
    /**
     This function returns the node type, this is the type of node defined by the class that inherits from the Node base class.
     
     @return returns the nodeType
     */
    string getNodeType() const{
        return nodeType;
    }
    
    /**
     This function returns the depth of the node. The depth is the level in the tree at which the node is located, the root node has a
     depth of 0.
     
     @return returns the depth of the node in the tree
     */
    UINT getDepth() const{
        return depth;
    }
    
    /**
     This function returns true if this node is a leaf node, false otherwise.
     
     @return returns true if this node is a leaf node, false otherwise
     */
    bool getIsLeafNode() const{
        return isLeafNode;
    }
    
    /**
     This function returns true if this node has a parent, false otherwise.
     
     @return returns true if this node has a parent, false otherwise
     */
    bool getHasParent() const{
        return (parent != NULL);
    }
    
    /**
     This function returns true if this node has a leftChild, false otherwise.
     
     @return returns true if this node has a leftChild, false otherwise
     */
    bool getHasLeftChild() const{
        return (leftChild != NULL);
    }
    
    /**
     This function returns true if this node has a rightChild, false otherwise.
     
     @return returns true if this node has a rightChild, false otherwise
     */
    bool getHasRightChild() const{
        return (rightChild != NULL);
    }
    
    bool initNode(Node *parent,const UINT depth,const bool isLeafNode = false){
        this->parent = parent;
        this->depth = depth;
        this->isLeafNode = isLeafNode;
        return true;
    }
    
    bool setParent(Node *parent){
        this->parent = parent;
        return true;
    }
    
    bool setLeftChild(Node *leftChild){
        this->leftChild = leftChild;
        return true;
    }
    
    bool setRightChild(Node *rightChild){
        this->rightChild = rightChild;
        return true;
    }
    
    bool setDepth(const UINT depth){
        this->depth = depth;
        return true;
    }
    
    bool setIsLeafNode(const bool isLeafNode){
        this->isLeafNode = isLeafNode;
        return true;
    }
    
    /**
     Defines a map between a string (which will contain the name of the node, such as DecisionTreeNode) and a function returns a new instance of that node.
     */
    typedef std::map< string, Node*(*)() > StringNodeMap;
    
    /**
     Creates a new classifier instance based on the input string (which should contain the name of a valid classifier such as ANBC).
     
     @param string const &classifierType: the name of the classifier
     @return Classifier*: a pointer to the new instance of the classifier
     */
    static Node* createInstanceFromString(string const &nodeType);
    
    /**
     Creates a new classifier instance based on the current classifierType string value.
     
     @return Classifier*: a pointer to the new instance of the classifier
     */
    Node* createNewInstance() const;
    
protected:
    
    /**
     This saves the custom parameters to a file. This can be used by any class that inherits from the Node class to save
     the custom parameters from that class to a file by overridding this function.
     
     @param fstream &file: a reference to the file the parameters will be saved to
     @return returns true if the parameters were saved successfully, false otherwise
     */
    virtual bool saveParametersToFile(fstream &file) const{
        
        cout << "Node::saveParametersToFile\n";
        
        if(!file.is_open())
        {
            errorLog << "saveParametersToFile(fstream &file) - File is not open!" << endl;
            return false;
        }
        
        return true;
    }
    
    /**
     This loads the custom parameters to from file. This can be used by any class that inherits from the Node class to load
     the custom parameters from that class from a file by overridding this function.
     
     @param fstream &file: a reference to the file the parameters will be loaded from
     @return returns true if the parameters were loaded successfully, false otherwise
     */
    virtual bool loadParametersFromFile(fstream &file){
        
        cout << "Node::loadParametersFromFile\n";
        
        if(!file.is_open())
        {
            errorLog << "loadParametersFromFile(fstream &file) - File is not open!" << endl;
            return false;
        }
        
        return true;
    }
    
    string nodeType;
    UINT depth;
    bool isLeafNode;
    Node *parent;
    Node *leftChild;
    Node *rightChild;
    
    static StringNodeMap *getMap() {
        if( !stringNodeMap ){ stringNodeMap = new StringNodeMap; }
        return stringNodeMap;
    }
    
private:
    static StringNodeMap *stringNodeMap;
    static UINT numNodeInstances;
    
};

//These two functions/classes are used to register any new Node with the Node base class
template< typename T >  Node* getNewNodeInstance() { return new T; }

template< typename T >
class RegisterNode : Node {
public:
    RegisterNode(string const &newNodeName) {
        getMap()->insert( std::pair<string, Node*(*)()>(newNodeName, &getNewNodeInstance< T > ) );
    }
};

} //End of namespace GRT

#endif //GRT_NODE_HEADER

