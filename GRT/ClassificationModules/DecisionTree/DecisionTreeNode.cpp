
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

bool DecisionTreeNode::predict(const VectorDouble &x,VectorDouble &classLikelihoods){
    
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
            warningLog << "predict(const VectorDouble &x,VectorDouble &classLikelihoods) - Right child failed prediction!" << endl;
            return false;
        }
    }else{
        if( leftChild ){
            if( leftChild->predict( x, classLikelihoods ) ){
                predictedNodeID = leftChild->getPredictedNodeID();
                return true;
            }
            warningLog << "predict(const VectorDouble &x,VectorDouble &classLikelihoods) - Left child failed prediction!" << endl;
            return false;
        }
    }
    
    return false;
}

bool DecisionTreeNode::computeBestSpilt( const UINT &trainingMode, const UINT &numSplittingSteps,const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){
    
    switch( trainingMode ){
        case Tree::BEST_ITERATIVE_SPILT:
            return computeBestSpiltBestIterativeSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError );
            break;
        case Tree::BEST_RANDOM_SPLIT:
            return computeBestSpiltBestRandomSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError );
            break;
        default:
            errorLog << "computeBestSpilt(...) - Uknown trainingMode!" << endl;
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
    
bool DecisionTreeNode::getModel(ostream &stream) const{
    
    string tab = "";
    for(UINT i=0; i<depth; i++) tab += "\t";
    
    stream << tab << "depth: " << depth << " nodeSize: " << nodeSize << " isLeafNode: " << isLeafNode << endl;
    stream << tab << "ClassProbabilities: ";
    for(UINT i=0; i<classProbabilities.size(); i++){
        stream << classProbabilities[i] << "\t";
    }
    stream << endl;
    
    if( leftChild != NULL ){
        stream << tab << "LeftChild: " << endl;
        leftChild->getModel( stream );
    }
    
    if( rightChild != NULL ){
        stream << tab << "RightChild: " << endl;
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

VectorDouble DecisionTreeNode::getClassProbabilities() const{
    return classProbabilities;
}

bool DecisionTreeNode::setLeafNode( const UINT nodeSize, const VectorDouble &classProbabilities ){
    this->nodeSize = nodeSize;
    this->classProbabilities = classProbabilities;
    this->isLeafNode = true;
    return true;
}

bool DecisionTreeNode::setNodeSize(const UINT nodeSize){
    this->nodeSize = nodeSize;
    return true;
}

bool DecisionTreeNode::setClassProbabilities(const VectorDouble &classProbabilities){
    this->classProbabilities = classProbabilities;
    return true;
}
    
UINT DecisionTreeNode::getClassLabelIndexValue(UINT classLabel,const vector< UINT > &classLabels){
    const UINT N = (UINT)classLabels.size();
    for(UINT i=0; i<N; i++){
        if( classLabel == classLabels[i] )
            return i;
    }
    return 0;
}