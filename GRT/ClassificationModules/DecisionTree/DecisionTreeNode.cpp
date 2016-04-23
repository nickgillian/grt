
#define GRT_DLL_EXPORTS
#include "DecisionTreeNode.h"

using namespace GRT;
    
//Register the DecisionTreeNode with the Node base class
RegisterNode< DecisionTreeNode > DecisionTreeNode::registerModule("DecisionTreeNode");
    
DecisionTreeNode::DecisionTreeNode(){
    nodeType = "DecisionTreeNode";
    parent = NULL;
    leftChild = NULL;
    rightChild = NULL;
    clear();
}

DecisionTreeNode::~DecisionTreeNode(){
    clear();
}

bool DecisionTreeNode::predict(const VectorFloat &x,VectorFloat &classLikelihoods){
    
    predictedNodeID = 0;
    
    if( isLeafNode ){
        classLikelihoods = classProbabilities;
        predictedNodeID = nodeID;
        return true;
    }
    
    if( leftChild == NULL && rightChild == NULL )
        return false;
    
    if( predict( x ) ){
        if( rightChild ){
            if( rightChild->predict( x, classLikelihoods ) ){
                predictedNodeID = rightChild->getPredictedNodeID();
                return true;
            }
            warningLog << "predict(const VectorFloat &x,VectorFloat &classLikelihoods) - Right child failed prediction!" << std::endl;
            return false;
        }
    }else{
        if( leftChild ){
            if( leftChild->predict( x, classLikelihoods ) ){
                predictedNodeID = leftChild->getPredictedNodeID();
                return true;
            }
            warningLog << "predict(const VectorFloat &x,VectorFloat &classLikelihoods) - Left child failed prediction!" << std::endl;
            return false;
        }
    }
    
    return false;
}

bool DecisionTreeNode::computeBestSpilt( const UINT &trainingMode, const UINT &numSplittingSteps,const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError ){
    
    switch( trainingMode ){
        case Tree::BEST_ITERATIVE_SPILT:
            return computeBestSpiltBestIterativeSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError );
            break;
        case Tree::BEST_RANDOM_SPLIT:
            return computeBestSpiltBestRandomSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError );
            break;
        default:
            errorLog << "computeBestSpilt(...) - Uknown trainingMode!" << std::endl;
            return false;
            break;
    }
    
    return false;
}

bool DecisionTreeNode::clear(){
    
    //Call the base class clear function
    Node::clear();
    
    nodeSize = 0;
    classProbabilities.clear();
    
    return true;
}
    
bool DecisionTreeNode::getModel( std::ostream &stream ) const{
    
    std::string tab = "";
    for(UINT i=0; i<depth; i++) tab += "\t";
    
    stream << tab << "depth: " << depth << " nodeSize: " << nodeSize << " isLeafNode: " << isLeafNode << std::endl;
    stream << tab << "ClassProbabilities: ";
    for(UINT i=0; i<classProbabilities.size(); i++){
        stream << classProbabilities[i] << "\t";
    }
    stream << std::endl;
    
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

Node* DecisionTreeNode::deepCopyNode() const{
    
    DecisionTreeNode *node = dynamic_cast< DecisionTreeNode* >( DecisionTreeNode::createInstanceFromString( nodeType ) );
    
    if( node == NULL ){
        return NULL;
    }
    
    //Copy this node into the node
    node->depth = depth;
    node->isLeafNode = isLeafNode;
    node->nodeID = nodeID;
    node->predictedNodeID = predictedNodeID;
    node->nodeSize = nodeSize;
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
    
    return node;
}

DecisionTreeNode* DecisionTreeNode::deepCopy() const{
    return dynamic_cast< DecisionTreeNode* >( deepCopyNode() );
}

UINT DecisionTreeNode::getNodeSize() const{
    return nodeSize;
}

UINT DecisionTreeNode::getNumClasses() const{
    return (UINT)classProbabilities.size();
}

VectorFloat DecisionTreeNode::getClassProbabilities() const{
    return classProbabilities;
}

bool DecisionTreeNode::setLeafNode( const UINT nodeSize, const VectorFloat &classProbabilities ){
    this->nodeSize = nodeSize;
    this->classProbabilities = classProbabilities;
    this->isLeafNode = true;
    return true;
}

bool DecisionTreeNode::setNodeSize(const UINT nodeSize){
    this->nodeSize = nodeSize;
    return true;
}

bool DecisionTreeNode::setClassProbabilities(const VectorFloat &classProbabilities){
    this->classProbabilities = classProbabilities;
    return true;
}
    
UINT DecisionTreeNode::getClassLabelIndexValue(UINT classLabel,const Vector< UINT > &classLabels){
    const UINT N = classLabels.getSize();
    for(UINT i=0; i<N; i++){
        if( classLabel == classLabels[i] )
            return i;
    }
    return 0;
}
