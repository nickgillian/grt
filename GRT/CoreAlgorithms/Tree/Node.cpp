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

#define GRT_DLL_EXPORTS
#include "Node.h"

GRT_BEGIN_NAMESPACE

Node::StringNodeMap* Node::stringNodeMap = NULL;
UINT Node::numNodeInstances = 0;

Node* Node::createInstanceFromString( std::string const &nodeType ){
    
    StringNodeMap::iterator iter = getMap()->find( nodeType );
    if( iter == getMap()->end() ){
        return NULL;
    }
    
    return iter->second();
}

Node* Node::createNewInstance() const{
    return createInstanceFromString( nodeType );
}

Node::Node() : MLBase("Node")
{
    nodeType = "";
    parent = NULL;
    leftChild = NULL;
    rightChild = NULL;
    clear();
}

Node::~Node(){
    clear();
}

bool Node::predict(const VectorFloat &x){
    warningLog << "predict(const VectorFloat &x) - Base class not overwritten!" << std::endl;
    return false;
}

bool Node::predict(const VectorFloat &x,VectorFloat &y){
    warningLog << "predict(const VectorFloat &x) - Base class not overwritten!" << std::endl;
    return false;
}

bool Node::clear(){
    
    //Set the parent pointer to null, this is safe as the parent pointer does not own the memory
    parent = NULL;
    
    if( leftChild != NULL ){
        //Recursively clean up the left child
        leftChild->clear();
        
        //Clean up the left child
        delete leftChild;
        leftChild = NULL;
    }
    
    if( rightChild != NULL ){
        //Recursively clean up the right child
        rightChild->clear();
        
        //Clean up the right child
        delete rightChild;
        rightChild = NULL;
    }
    
    depth = 0;
    nodeID = 0;
    predictedNodeID = 0;
    isLeafNode = false;
    
    return true;
}

bool Node::computeFeatureWeights( VectorFloat &weights ) const{
    return false; //Base class always return false
}

bool Node::computeLeafNodeWeights( MatrixFloat &weights ) const{
    return false; //Base class always return false
}

bool Node::print() const{
    
    std::ostringstream stream;
    if( getModel( stream ) ){
        std::cout << stream.str();
        return true;
    }
    
    return false;
}

bool Node::getModel( std::ostream &stream ) const{
    
    std::string tab = "";
    for(UINT i=0; i<depth; i++) tab += "\t";
    
    stream << tab << "depth: " << depth << " isLeafNode: " << isLeafNode << " nodeID: " << nodeID << std::endl;
    
    if( leftChild != NULL ){
        stream << tab << "LeftChild: " << std::endl;
        leftChild->getModel( stream );
    }
    
    if( rightChild != NULL ){
        stream << tab << "RightChild: " << std::endl;
        rightChild->getModel( stream );
    }
    
    return true;
}

bool Node::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog << "save(fstream &file) - File is not open!" << std::endl;
        return false;
    }
    
    file << "NodeType: " << nodeType << std::endl;
    file << "Depth: " << depth << std::endl;
    file << "NodeID: " << nodeID << std::endl;
    file << "IsLeafNode: " << isLeafNode << std::endl;
    file << "HasLeftChild: " << getHasLeftChild() << std::endl;
    file << "HasRightChild: " << getHasRightChild() << std::endl;
    
    //If there is a left child then load the left child's data
    if( getHasLeftChild() ){
        file << "LeftChild\n";
        if( !leftChild->save( file ) ){
            errorLog << "save(fstream &file) - Failed to save left child at depth: " << depth << std::endl;
            return false;
        }
    }
    
    //If there is a right child then load the right child's data
    if( getHasRightChild() ){
        file << "RightChild\n";
        if( !rightChild->save( file ) ){
            errorLog << "save(fstream &file) - Failed to save right child at depth: " << depth << std::endl;
            return false;
        }
    }
    
    //Save the custom parameters to the file
    if( !saveParametersToFile( file ) ){
        errorLog << "save(fstream &file) - Failed to save parameters to file at depth: " << depth << std::endl;
        return false;
    }
    
    return true;
}

bool Node::load( std::fstream &file ){
    
    //Clear any previous nodes
    clear();
    
    if(!file.is_open())
    {
        errorLog << "load(fstream &file) - File is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    bool hasLeftChild = false;
    bool hasRightChild = false;
    
    file >> word;
    if( word != "NodeType:" ){
        errorLog << "load(fstream &file) - Failed to find Node header!" << std::endl;
        return false;
    }
    file >> nodeType;
    
    file >> word;
    if( word != "Depth:" ){
        errorLog << "load(fstream &file) - Failed to find Depth header!" << std::endl;
        return false;
    }
    file >> depth;
    
    file >> word;
    if( word != "NodeID:" ){
        errorLog << "load(fstream &file) - Failed to find NodeID header!" << std::endl;
        return false;
    }
    file >> nodeID;
    
    file >> word;
    if( word != "IsLeafNode:" ){
        errorLog << "load(fstream &file) - Failed to find IsLeafNode header!" << std::endl;
        return false;
    }
    file >> isLeafNode;
    
    file >> word;
    if( word != "HasLeftChild:" ){
        errorLog << "load(fstream &file) - Failed to find HasLeftChild header!" << std::endl;
        return false;
    }
    file >> hasLeftChild;
    
    file >> word;
    if( word != "HasRightChild:" ){
        errorLog << "load(fstream &file) - Failed to find HasRightChild header!" << std::endl;
        return false;
    }
    file >> hasRightChild;
    
    if( hasLeftChild ){
        file >> word;
        if( word != "LeftChild" ){
            errorLog << "load(fstream &file) - Failed to find LeftChild header!" << std::endl;
            return false;
        }
        leftChild = createNewInstance();
        leftChild->setParent( this );
        if( !leftChild->load(file) ){
            errorLog << "load(fstream &file) - Failed to load left child at depth: " << depth << std::endl;
            return false;
        }
    }
    
    if( hasRightChild ){
        file >> word;
        if( word != "RightChild" ){
            errorLog << "load(fstream &file) - Failed to find RightChild header!" << std::endl;
            return false;
        }
        rightChild = createNewInstance();
        rightChild->setParent( this );
        if( !rightChild->load( file ) ){
            errorLog << "load(fstream &file) - Failed to load right child at depth: " << depth << std::endl;
            return false;
        }
    }
    
    //Load the custom parameters from a file
    if( !loadParametersFromFile( file ) ){
        errorLog << "loadParametersFromFile(fstream &file) - Failed to load parameters from file at depth: " << depth << std::endl;
        return false;
    }
    
    return true;
}

Node* Node::deepCopyNode() const{
    
    Node *node = createNewInstance();
    
    if( node == NULL ){
        return NULL;
    }
    
    //Copy this node into the node
    node->setNodeID( nodeID );
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

std::string Node::getNodeType() const{
    return nodeType;
}

UINT Node::getDepth() const{
    return depth;
}

UINT Node::getNodeID() const{
    return nodeID;
}

UINT Node::getPredictedNodeID() const{
    return predictedNodeID;
}

UINT Node::getMaxDepth() const {
    
    UINT maxDepth = depth;
    
    //Search for the maximum depth in the left child
    if( getHasLeftChild() ){
        UINT maxLeftDepth = leftChild->getMaxDepth();
        if( maxLeftDepth > maxDepth ){
            maxDepth = maxLeftDepth;
        }
    }
    
    //Search for the maximum depth in the right child
    if( getHasRightChild() ){
        UINT maxRightDepth = rightChild->getMaxDepth();
        if( maxRightDepth > maxDepth ){
            maxDepth = maxRightDepth;
        }
    }
    
    return maxDepth;
}

bool Node::getIsLeafNode() const{
    return isLeafNode;
}

bool Node::getHasParent() const{
    return (parent != NULL);
}

bool Node::getHasLeftChild() const {
    return (leftChild != NULL);
}

bool Node::getHasRightChild() const {
    return (rightChild != NULL);
}

bool Node::initNode(Node *parent,const UINT depth,const UINT nodeID,const bool isLeafNode){
    this->parent = parent;
    this->depth = depth;
    this->nodeID = nodeID;
    this->isLeafNode = isLeafNode;
    return true;
}

bool Node::setParent(Node *parent){
    this->parent = parent;
    return true;
}

bool Node::setLeftChild(Node *leftChild){
    this->leftChild = leftChild;
    return true;
}

bool Node::setRightChild(Node *rightChild){
    this->rightChild = rightChild;
    return true;
}

bool Node::setDepth(const UINT depth){
    this->depth = depth;
    return true;
}

bool Node::setNodeID(const UINT nodeID){
    this->nodeID = nodeID;
    return true;
}

bool Node::setIsLeafNode(const bool isLeafNode){
    this->isLeafNode = isLeafNode;
    return true;
}

GRT_END_NAMESPACE
