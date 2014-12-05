
#include "DecisionTreeThresholdNode.h"

namespace GRT{
    
//Register the DecisionTreeThresholdNode module with the Node base class
RegisterNode< DecisionTreeThresholdNode > DecisionTreeThresholdNode::registerModule("DecisionTreeThresholdNode");
    
DecisionTreeThresholdNode::DecisionTreeThresholdNode(){
    nodeType = "DecisionTreeThresholdNode";
    parent = NULL;
    leftChild = NULL;
    rightChild = NULL;
    clear();
}

DecisionTreeThresholdNode::~DecisionTreeThresholdNode(){
    clear();
}

bool DecisionTreeThresholdNode::predict(const VectorDouble &x) {

    if( x[ featureIndex ] >= threshold ) return true;
    
    return false;
}

bool DecisionTreeThresholdNode::clear(){
    
    //Call the base class clear function
    DecisionTreeNode::clear();
    
    featureIndex = 0;
    threshold = 0;
    
    return true;
}

bool DecisionTreeThresholdNode::print() const{
    
    ostringstream stream;
    
    if( getModel( stream ) ){
        cout << stream.str();
        return true;
    }
    
    return false;
}
    
bool DecisionTreeThresholdNode::getModel(ostream &stream) const{
    
    string tab = "";
    for(UINT i=0; i<depth; i++) tab += "\t";
    
    stream << tab << "depth: " << depth << " nodeSize: " << nodeSize << " featureIndex: " << featureIndex << " threshold " << threshold << " isLeafNode: " << isLeafNode << endl;
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

Node* DecisionTreeThresholdNode::deepCopyNode() const{
    
    DecisionTreeThresholdNode *node = new DecisionTreeThresholdNode;
    
    if( node == NULL ){
        return NULL;
    }
    
    //Copy this node into the node
    node->depth = depth;
    node->isLeafNode = isLeafNode;
    node->nodeID = nodeID;
    node->predictedNodeID = predictedNodeID;
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
    
    return dynamic_cast< Node* >( node );
}

DecisionTreeThresholdNode* DecisionTreeThresholdNode::deepCopy() const{
    return dynamic_cast< DecisionTreeThresholdNode* >( deepCopyNode() );
}

UINT DecisionTreeThresholdNode::getFeatureIndex() const{
    return featureIndex;
}

double DecisionTreeThresholdNode::getThreshold() const{
    return threshold;
}

bool DecisionTreeThresholdNode::set(const UINT nodeSize,const UINT featureIndex,const double threshold,const VectorDouble &classProbabilities){
    this->nodeSize = nodeSize;
    this->featureIndex = featureIndex;
    this->threshold = threshold;
    this->classProbabilities = classProbabilities;
    return true;
}

bool DecisionTreeThresholdNode::computeBestSpiltBestIterativeSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = (UINT)features.size();
    const UINT K = (UINT)classLabels.size();
    
    if( N == 0 ) return false;
    
    minError = numeric_limits<double>::max();
    UINT bestFeatureIndex = 0;
    double bestThreshold = 0;
    double error = 0;
    double minRange = 0;
    double maxRange = 0;
    double step = 0;
    double giniIndexL = 0;
    double giniIndexR = 0;
    double weightL = 0;
    double weightR = 0;
    vector< UINT > groupIndex(M);
    VectorDouble groupCounter(2,0);
    vector< MinMax > ranges = trainingData.getRanges();
    
    MatrixDouble classProbabilities(K,2);
    
    //Loop over each feature and try and find the best split point
    for(UINT n=0; n<N; n++){
        minRange = ranges[n].minValue;
        maxRange = ranges[n].maxValue;
        step = (maxRange-minRange)/double(numSplittingSteps);
        threshold = minRange;
        featureIndex = features[n];
        while( threshold <= maxRange ){
            
            //Iterate over each sample and work out if it should be in the lhs (0) or rhs (1) group
            groupCounter[0] = groupCounter[1] = 0;
            classProbabilities.setAllValues(0);
            for(UINT i=0; i<M; i++){
                groupIndex[i] = trainingData[ i ][ featureIndex ] >= threshold ? 1 : 0;
                groupCounter[ groupIndex[i] ]++;
                classProbabilities[ getClassLabelIndexValue(trainingData[i].getClassLabel(),classLabels) ][ groupIndex[i] ]++;
            }
            
            //Compute the class probabilities for the lhs group and rhs group
            for(UINT k=0; k<K; k++){
                classProbabilities[k][0] = groupCounter[0]>0 ? classProbabilities[k][0]/groupCounter[0] : 0;
                classProbabilities[k][1] = groupCounter[1]>0 ? classProbabilities[k][1]/groupCounter[1] : 0;
            }
            
            //Compute the Gini index for the lhs and rhs groups
            giniIndexL = giniIndexR = 0;
            for(UINT k=0; k<K; k++){
                giniIndexL += classProbabilities[k][0] * (1.0-classProbabilities[k][0]);
                giniIndexR += classProbabilities[k][1] * (1.0-classProbabilities[k][1]);
            }
            weightL = groupCounter[0]/M;
            weightR = groupCounter[1]/M;
            error = (giniIndexL*weightL) + (giniIndexR*weightR);
            
            //Store the best threshold and feature index
            if( error < minError ){
                minError = error;
                bestThreshold = threshold;
                bestFeatureIndex = featureIndex;
            }
            
            //Update the threshold
            threshold += step;
        }
    }
    
    //Set the best feature index that will be returned to the DecisionTree that called this function
    featureIndex = bestFeatureIndex;
    
    //Store the node size, feature index, best threshold and class probabilities for this node
    set(M,featureIndex,bestThreshold,trainingData.getClassProbabilities(classLabels));
    
    return true;
}

bool DecisionTreeThresholdNode::computeBestSpiltBestRandomSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = (UINT)features.size();
    const UINT K = (UINT)classLabels.size();
    
    if( N == 0 ) return false;
    
    minError = numeric_limits<double>::max();
    UINT bestFeatureIndex = 0;
    double bestThreshold = 0;
    double error = 0;
    double giniIndexL = 0;
    double giniIndexR = 0;
    double weightL = 0;
    double weightR = 0;
    Random random;
    vector< UINT > groupIndex(M);
    VectorDouble groupCounter(2,0);
    vector< MinMax > ranges = trainingData.getRanges();
    
    MatrixDouble classProbabilities(K,2);
    
    //Loop over each feature and try and find the best split point
    for(UINT n=0; n<N; n++){
        featureIndex = features[n];
        for(UINT m=0; m<numSplittingSteps; m++){
            //Randomly choose the threshold
            threshold = random.getRandomNumberUniform(ranges[n].minValue,ranges[n].maxValue);
            
            //Iterate over each sample and work out if it should be in the lhs (0) or rhs (1) group
            groupCounter[0] = groupCounter[1] = 0;
            classProbabilities.setAllValues(0);
            for(UINT i=0; i<M; i++){
                groupIndex[i] = trainingData[ i ][ featureIndex ] >= threshold ? 1 : 0;
                groupCounter[ groupIndex[i] ]++;
                classProbabilities[ getClassLabelIndexValue(trainingData[i].getClassLabel(),classLabels) ][ groupIndex[i] ]++;
            }
            
            //Compute the class probabilities for the lhs group and rhs group
            for(UINT k=0; k<K; k++){
                classProbabilities[k][0] = groupCounter[0]>0 ? classProbabilities[k][0]/groupCounter[0] : 0;
                classProbabilities[k][1] = groupCounter[1]>0 ? classProbabilities[k][1]/groupCounter[1] : 0;
            }
            
            //Compute the Gini index for the lhs and rhs groups
            giniIndexL = giniIndexR = 0;
            for(UINT k=0; k<K; k++){
                giniIndexL += classProbabilities[k][0] * (1.0-classProbabilities[k][0]);
                giniIndexR += classProbabilities[k][1] * (1.0-classProbabilities[k][1]);
            }
            weightL = groupCounter[0]/M;
            weightR = groupCounter[1]/M;
            error = (giniIndexL*weightL) + (giniIndexR*weightR);
            
            //Store the best threshold and feature index
            if( error < minError ){
                minError = error;
                bestThreshold = threshold;
                bestFeatureIndex = featureIndex;
            }
        }
    }
    
    //Set the best feature index that will be returned to the DecisionTree that called this function
    featureIndex = bestFeatureIndex;
    
    //Store the node size, feature index, best threshold and class probabilities for this node
    set(M,featureIndex,bestThreshold,trainingData.getClassProbabilities(classLabels));
    
    return true;
}

bool DecisionTreeThresholdNode::saveParametersToFile(fstream &file) const{
    
    if(!file.is_open())
    {
        errorLog << "saveParametersToFile(fstream &file) - File is not open!" << endl;
        return false;
    }
    
    //Save the DecisionTreeNode parameters
    if( !DecisionTreeNode::saveParametersToFile( file ) ){
        errorLog << "saveParametersToFile(fstream &file) - Failed to save DecisionTreeNode parameters to file!" << endl;
        return false;
    }
    
    //Save the custom DecisionTreeThresholdNode parameters
    file << "FeatureIndex: " << featureIndex << endl;
    file << "Threshold: " << threshold << endl;
    
    return true;
}

bool DecisionTreeThresholdNode::loadParametersFromFile(fstream &file){
    
    if(!file.is_open())
    {
        errorLog << "loadParametersFromFile(fstream &file) - File is not open!" << endl;
        return false;
    }
    
    //Load the DecisionTreeNode parameters
    if( !DecisionTreeNode::loadParametersFromFile( file ) ){
        errorLog << "loadParametersFromFile(fstream &file) - Failed to load DecisionTreeNode parameters from file!" << endl;
        return false;
    }
    
    string word;
    
    //Load the custom DecisionTreeThresholdNode Parameters
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
    
    return true;
}
}

