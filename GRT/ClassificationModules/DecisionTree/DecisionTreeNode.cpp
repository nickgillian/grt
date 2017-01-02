
#define GRT_DLL_EXPORTS
#include "DecisionTreeNode.h"

using namespace GRT;
    
//Register the DecisionTreeNode with the Node base class
RegisterNode< DecisionTreeNode > DecisionTreeNode::registerModule("DecisionTreeNode");
    
DecisionTreeNode::DecisionTreeNode( const std::string id ) : Node(id){
    clear();
}

DecisionTreeNode::~DecisionTreeNode(){
    clear();
}

bool DecisionTreeNode::predict_(VectorFloat &x,VectorFloat &classLikelihoods){
    
    predictedNodeID = 0;
    
    if( isLeafNode ){
        classLikelihoods = classProbabilities;
        predictedNodeID = nodeID;
        return true;
    }
    
    if( leftChild == NULL && rightChild == NULL ){
        classLikelihoods = classProbabilities;
        predictedNodeID = nodeID;
        warningLog << __GRT_LOG__ << " Left and right children are NULL but node not marked as leaf node!" << std::endl;
        return false;
    }
    
    if( predict_( x ) ){
        if( rightChild ){
            if( rightChild->predict_( x, classLikelihoods ) ){
                predictedNodeID = rightChild->getPredictedNodeID();
                return true;
            }
            warningLog << __GRT_LOG__ << " Right child failed prediction!" << std::endl;
            return false;
        }
        classLikelihoods = classProbabilities;
        predictedNodeID = nodeID;
        return true;
    }else{
        if( leftChild ){
            if( leftChild->predict_( x, classLikelihoods ) ){
                predictedNodeID = leftChild->getPredictedNodeID();
                return true;
            }
            warningLog << __GRT_LOG__ << " Left child failed prediction!" << std::endl;
            return false;
        }
        classLikelihoods = classProbabilities;
        predictedNodeID = nodeID;
        return true;
    }
    
    return false;
}

bool DecisionTreeNode::computeBestSplit( const UINT &trainingMode, const UINT &numSplittingSteps,const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError ){
    
    switch( trainingMode ){
        case Tree::BEST_ITERATIVE_SPILT:
            return computeBestSplitBestIterativeSplit( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError );
            break;
        case Tree::BEST_RANDOM_SPLIT:
            return computeBestSplitBestRandomSplit( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError );
            break;
        default:
            errorLog << __GRT_LOG__ << " Uknown trainingMode!" << std::endl;
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

Node* DecisionTreeNode::deepCopy() const{
    
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
        node->leftChild = leftChild->deepCopy();
        node->leftChild->setParent( node );
    }
    
    //Recursively deep copy the right child
    if( rightChild ){
        node->rightChild = rightChild->deepCopy();
        node->rightChild->setParent( node );
    }
    
    return dynamic_cast< Node* >(node);
}

UINT DecisionTreeNode::getNodeSize() const{
    return nodeSize;
}

UINT DecisionTreeNode::getNumClasses() const{
    return classProbabilities.getSize();
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
