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

#include "../../CoreModules/MLBase.h"

GRT_BEGIN_NAMESPACE

class GRT_API Node : public MLBase{
public:
    /**
    Default Constructor. Sets all the pointers to NULL.
    */
    Node();
    
    /**
    Default Destructor. Cleans up any memory.
    */
    virtual ~Node();
    
    /**
    This function predicts if the input is greater than or equal to the nodes threshold.
    This function should be overwritten by the inheriting class.
    
    @param x: the input vector that will be used for the prediction
    @return returns true if the input is greater than or equal to the nodes threshold, false otherwise
    */
    virtual bool predict(const VectorFloat &x);
    
    /**
    This function recursively predicts if the probability of the input vector.
    This function should be overwritten by the inheriting class.
    
    @param x: the input vector that will be used for the prediction
    @param y: a reference to a vector that will store the results
    @return returns true if the input is greater than or equal to the nodes threshold, false otherwise
    */
    virtual bool predict(const VectorFloat &x,VectorFloat &y);
    
    /**
    This function recursively computes the weights of features used for classification nodes and stores the results in the weights vector.
    This function should be overwritten by the inheriting class.
    
    @param weights: the input vector that will be used to store the weights
    @return returns true if the weights were updated, false otherwise
    */
    virtual bool computeFeatureWeights( VectorFloat &weights ) const;
    
    /**
    This function recursively computes the weights of features used for classification nodes and stores the results in the weights vector.
    This function should be overwritten by the inheriting class.
    
    @param weights: the input matrix that will be used to store the weights, rows represent classes, columns represent features
    @return returns true if the weights were updated, false otherwise
    */
    virtual bool computeLeafNodeWeights( MatrixFloat &weights ) const;
    
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
    
    @param file: a reference to the stream the model will be added to
    @return returns true if the model was added successfully, false otherwise
    */
    virtual bool getModel( std::ostream &stream ) const;
    
    /**
    This saves the Node to a file.
    
    @param file: a reference to the file the Node model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads the Node from a file.
    
    @param file: a reference to the file the Node model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This function returns a deep copy of the Node and all it's children.
    The user is responsible for managing the dynamic data that is returned from this function as a pointer.
    
    @return returns a pointer to a deep copy of the Node, or NULL if the deep copy was not successful
    */
    virtual Node* deepCopyNode() const;
    
    /**
    This function returns the node type, this is the type of node defined by the class that inherits from the Node base class.
    
    @return returns the nodeType
    */
    std::string getNodeType() const;
    
    /**
    This function returns the depth of the node. The depth is the level in the tree at which the node is located, the root node has a
    depth of 0.
    
    @return returns the depth of the node in the tree
    */
    UINT getDepth() const;
    
    /**
    This function returns the nodeID, this is a unique ID that represents this node within a Tree.
    
    @return returns the nodeID of this node
    */
    UINT getNodeID() const;
    
    /**
    This function returns the predictedNodeID, this is ID of the leaf node that was reached during the last prediction call
    
    @return returns the predictedNodeID
    */
    UINT getPredictedNodeID() const;
    
    UINT getMaxDepth() const;
    
    /**
    This function returns true if this node is a leaf node, false otherwise.
    
    @return returns true if this node is a leaf node, false otherwise
    */
    bool getIsLeafNode() const;
    
    /**
    This function returns true if this node has a parent, false otherwise.
    
    @return returns true if this node has a parent, false otherwise
    */
    bool getHasParent() const;
    
    /**
    This function returns true if this node has a leftChild, false otherwise.
    
    @return returns true if this node has a leftChild, false otherwise
    */
    bool getHasLeftChild() const;
    
    /**
    This function returns true if this node has a rightChild, false otherwise.
    
    @return returns true if this node has a rightChild, false otherwise
    */
    bool getHasRightChild() const;
    
    Node *getParent() const { return parent; }
    Node *getLeftChild() const { return leftChild; }
    Node *getRightChild() const { return rightChild; }
    
    bool initNode(Node *parent,const UINT depth,const UINT nodeID,const bool isLeafNode = false);
    
    bool setParent(Node *parent);
    
    bool setLeftChild(Node *leftChild);
    
    bool setRightChild(Node *rightChild);
    
    bool setDepth(const UINT depth);
    
    bool setNodeID(const UINT nodeID);
    
    bool setIsLeafNode(const bool isLeafNode);
    
    /**
    Defines a map between a string (which will contain the name of the node, such as DecisionTreeNode) and a function returns a new instance of that node.
    */
    typedef std::map< std::string, Node*(*)() > StringNodeMap;
    
    /**
    Creates a new classifier instance based on the input string (which should contain the name of a valid classifier such as ANBC).
    
    @param string const &classifierType: the name of the classifier
    @return Classifier*: a pointer to the new instance of the classifier
    */
    static Node* createInstanceFromString( std::string const &nodeType );
    
    /**
    Creates a new classifier instance based on the current classifierType string value.
    
    @return Classifier*: a pointer to the new instance of the classifier
    */
    Node* createNewInstance() const;

    using MLBase::save;
    using MLBase::load;
    
protected:
    
    /**
    This saves the custom parameters to a file. This can be used by any class that inherits from the Node class to save
    the custom parameters from that class to a file by overridding this function.
    
    @param file: a reference to the file the parameters will be saved to
    @return returns true if the parameters were saved successfully, false otherwise
    */
    virtual bool saveParametersToFile( std::fstream &file ) const{ return true; }
    
    /**
    This loads the custom parameters to from file. This can be used by any class that inherits from the Node class to load
    the custom parameters from that class from a file by overridding this function.
    
    @param file: a reference to the file the parameters will be loaded from
    @return returns true if the parameters were loaded successfully, false otherwise
    */
    virtual bool loadParametersFromFile( std::fstream &file ){ return true; }
    
    std::string nodeType;
    UINT depth;
    UINT nodeID;
    UINT predictedNodeID;
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
    RegisterNode(std::string const &newNodeName) {
        getMap()->insert( std::pair< std::string, Node*(*)() >(newNodeName, &getNewNodeInstance< T > ) );
    }
};

GRT_END_NAMESPACE

#endif //GRT_NODE_HEADER
    