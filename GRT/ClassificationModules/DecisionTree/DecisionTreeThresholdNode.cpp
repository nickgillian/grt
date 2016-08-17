
#define GRT_DLL_EXPORTS
#include "DecisionTreeThresholdNode.h"

GRT_BEGIN_NAMESPACE
    
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

bool DecisionTreeThresholdNode::predict(const VectorFloat &x) {

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
    
    std::ostringstream stream;
    
    if( getModel( stream ) ){
        std::cout << stream.str();
        return true;
    }
    
    return false;
}
    
bool DecisionTreeThresholdNode::getModel( std::ostream &stream ) const{
    
    std::string tab = "";
    for(UINT i=0; i<depth; i++) tab += "\t";
    
    stream << tab << "depth: " << depth << " nodeSize: " << nodeSize << " featureIndex: " << featureIndex << " threshold " << threshold << " isLeafNode: " << isLeafNode << std::endl;
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

Float DecisionTreeThresholdNode::getThreshold() const{
    return threshold;
}

bool DecisionTreeThresholdNode::set(const UINT nodeSize,const UINT featureIndex,const Float threshold,const VectorFloat &classProbabilities){
    this->nodeSize = nodeSize;
    this->featureIndex = featureIndex;
    this->threshold = threshold;
    this->classProbabilities = classProbabilities;
    return true;
}

bool DecisionTreeThresholdNode::computeBestSpiltBestIterativeSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError ){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = features.getSize();
    const UINT K = classLabels.getSize();
    
    if( N == 0 ) return false;
    
    minError = grt_numeric_limits< Float >::max();
    UINT bestFeatureIndex = 0;
    Float bestThreshold = 0;
    Float error = 0;
    Float minRange = 0;
    Float maxRange = 0;
    Float step = 0;
    Float giniIndexL = 0;
    Float giniIndexR = 0;
    Float weightL = 0;
    Float weightR = 0;
    Vector< UINT > groupIndex(M);
    VectorFloat groupCounter(2,0);
    Vector< MinMax > ranges = trainingData.getRanges();
    
    MatrixFloat classProbabilities(K,2);
    
    //Loop over each feature and try and find the best split point
    for(UINT n=0; n<N; n++){
        minRange = ranges[n].minValue;
        maxRange = ranges[n].maxValue;
        step = (maxRange-minRange)/Float(numSplittingSteps);
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

bool DecisionTreeThresholdNode::computeBestSpiltBestRandomSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError ){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = (UINT)features.size();
    const UINT K = (UINT)classLabels.size();
    
    if( N == 0 ) return false;
    
    minError = grt_numeric_limits< Float >::max();
    UINT bestFeatureIndex = 0;
    Float bestThreshold = 0;
    Float error = 0;
    Float giniIndexL = 0;
    Float giniIndexR = 0;
    Float weightL = 0;
    Float weightR = 0;
    Random random;
    Vector< UINT > groupIndex(M);
    VectorFloat groupCounter(2,0);
    
    MatrixFloat classProbabilities(K,2);

    //Loop over each feature and try and find the best split point
    UINT m,n;
    const UINT numFeatures = features.getSize();
    for(m=0; m<numSplittingSteps; m++){
        //Chose a random feature
        n = random.getRandomNumberInt(0,numFeatures);
        featureIndex = features[n];
        
        //Randomly choose the threshold, the threshold is based on a randomly selected sample with some random scaling
        threshold = trainingData[ random.getRandomNumberInt(0,M) ][ featureIndex ] * random.getRandomNumberUniform(0.8,1.2);
        
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
    
    //Set the best feature index that will be returned to the DecisionTree that called this function
    featureIndex = bestFeatureIndex;
    
    //Store the node size, feature index, best threshold and class probabilities for this node
    set(M,featureIndex,bestThreshold,trainingData.getClassProbabilities(classLabels));
    
    return true;
}

bool DecisionTreeThresholdNode::saveParametersToFile( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog << "saveParametersToFile(fstream &file) - File is not open!" << std::endl;
        return false;
    }
    
    //Save the DecisionTreeNode parameters
    if( !DecisionTreeNode::saveParametersToFile( file ) ){
        errorLog << "saveParametersToFile(fstream &file) - Failed to save DecisionTreeNode parameters to file!" << std::endl;
        return false;
    }
    
    //Save the custom DecisionTreeThresholdNode parameters
    file << "FeatureIndex: " << featureIndex << std::endl;
    file << "Threshold: " << threshold << std::endl;
    
    return true;
}

bool DecisionTreeThresholdNode::loadParametersFromFile( std::fstream &file ){
    
    if(!file.is_open())
    {
        errorLog << "loadParametersFromFile(fstream &file) - File is not open!" << std::endl;
        return false;
    }
    
    //Load the DecisionTreeNode parameters
    if( !DecisionTreeNode::loadParametersFromFile( file ) ){
        errorLog << "loadParametersFromFile(fstream &file) - Failed to load DecisionTreeNode parameters from file!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Load the custom DecisionTreeThresholdNode Parameters
    file >> word;
    if( word != "FeatureIndex:" ){
        errorLog << "loadParametersFromFile(fstream &file) - Failed to find FeatureIndex header!" << std::endl;
        return false;
    }
    file >> featureIndex;
    
    file >> word;
    if( word != "Threshold:" ){
        errorLog << "loadParametersFromFile(fstream &file) - Failed to find Threshold header!" << std::endl;
        return false;
    }
    file >> threshold;
    
    return true;
}

GRT_END_NAMESPACE

