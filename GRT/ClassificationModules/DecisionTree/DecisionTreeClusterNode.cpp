
#define GRT_DLL_EXPORTS
#include "DecisionTreeClusterNode.h"

GRT_BEGIN_NAMESPACE

//Register the DecisionTreeClusterNode module with the Node base class
RegisterNode< DecisionTreeClusterNode > DecisionTreeClusterNode::registerModule("DecisionTreeClusterNode");

DecisionTreeClusterNode::DecisionTreeClusterNode(){
    nodeType = "DecisionTreeClusterNode";
    parent = NULL;
    leftChild = NULL;
    rightChild = NULL;
    clear();
}

DecisionTreeClusterNode::~DecisionTreeClusterNode(){
    clear();
}

bool DecisionTreeClusterNode::predict(const VectorFloat &x) {

    if( x[ featureIndex ] >= threshold ) return true;

    return false;
}

bool DecisionTreeClusterNode::clear(){

    //Call the base class clear function
    DecisionTreeNode::clear();

    featureIndex = 0;
    threshold = 0;

    return true;
}

bool DecisionTreeClusterNode::print() const{

    std::ostringstream stream;

    if( getModel( stream ) ){
        std::cout << stream.str();
        return true;
    }

    return false;
}

bool DecisionTreeClusterNode::computeFeatureWeights( VectorFloat &weights ) const{

    if( isLeafNode ){ //If we reach a leaf node, no weight update needed
        return true;
    }
    
    if( featureIndex >= ((UINT)weights.size()) ){ //Feature index is out of bounds
        warningLog << "computeFeatureWeights( VectorFloat &weights ) - Feature index is greater than weights Vector size!" << std::endl;
        return false;
    }else{
        weights[ featureIndex ]++;
    }
    
    if( leftChild ){ //Recursively compute the weights for the left child
        leftChild->computeFeatureWeights( weights );
    }
    if( rightChild ){ //Recursively compute the weights for the right child
        rightChild->computeFeatureWeights( weights );
    }

    return true;
}

bool DecisionTreeClusterNode::computeLeafNodeWeights( MatrixFloat &weights ) const{

    if( isLeafNode ){ //If we reach a leaf node, there is nothing to do
        return true;
    }

    if( featureIndex >= weights.getNumCols() ){ //Feature index is out of bounds
        warningLog << "computeFeatureWeights( VectorFloat &weights ) - Feature index is greater than weights Vector size!" << std::endl;
        return false;
    }

    if( leftChild ){ //Recursively compute the weights for the left child until we reach the node above a leaf node
        if( leftChild->getIsLeafNode() ){
            if( classProbabilities.getSize() != weights.getNumRows() ){
                warningLog << "computeFeatureWeights( VectorFloat &weights ) - The number of rows in the weights matrix does not match the class probabilities Vector size!" << std::endl;
                return false;
            }
            for(UINT i=0; i<classProbabilities.getSize(); i++){
                weights[ i ][ featureIndex ] += classProbabilities[ i ];
            }
            
        } leftChild->computeLeafNodeWeights( weights );
    }
    if( rightChild ){ //Recursively compute the weights for the right child until we reach the node above a leaf node
        if( rightChild->getIsLeafNode() ){
            if( classProbabilities.getSize() != weights.getNumRows() ){
                warningLog << "computeFeatureWeights( VectorFloat &weights ) - The number of rows in the weights matrix does not match the class probabilities Vector size!" << std::endl;
                return false;
            }
            for(UINT i=0; i<classProbabilities.getSize(); i++){
                weights[ i ][ featureIndex ] += classProbabilities[ i ];
            }
        } rightChild->computeLeafNodeWeights( weights );
    }

    return true;
}

bool DecisionTreeClusterNode::getModel( std::ostream &stream ) const{

    std::string tab = "";
    for(UINT i=0; i<depth; i++) tab += "\t";

    stream << tab << "depth: " << depth;
    stream << " nodeSize: " << nodeSize;
    stream << " featureIndex: " << featureIndex;
    stream << " threshold " << threshold;
    stream << " isLeafNode: " << isLeafNode << std::endl;

    stream << tab << "ClassProbabilities: ";
    for(UINT i=0; i<classProbabilities.getSize(); i++){
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

Node* DecisionTreeClusterNode::deepCopyNode() const{

    DecisionTreeClusterNode *node = new DecisionTreeClusterNode;

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

    return dynamic_cast< DecisionTreeClusterNode* >( node );
}

DecisionTreeClusterNode* DecisionTreeClusterNode::deepCopy() const{
    return dynamic_cast< DecisionTreeClusterNode* >( deepCopyNode() );
}

UINT DecisionTreeClusterNode::getFeatureIndex() const{
    return featureIndex;
}

Float DecisionTreeClusterNode::getThreshold() const{
    return threshold;
}

bool DecisionTreeClusterNode::set(const UINT nodeSize,const UINT featureIndex,const Float threshold,const VectorFloat &classProbabilities){
    this->nodeSize = nodeSize;
    this->featureIndex = featureIndex;
    this->threshold = threshold;
    this->classProbabilities = classProbabilities;
    return true;
}

bool DecisionTreeClusterNode::computeBestSpiltBestIterativeSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError ){

    return computeBestSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError);
}

bool DecisionTreeClusterNode::computeBestSpiltBestRandomSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError ){

    return computeBestSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError);
}

bool DecisionTreeClusterNode::computeBestSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError ){

    const UINT M = trainingData.getNumSamples();
    const UINT N = (UINT)features.size();
    const UINT K = (UINT)classLabels.size();

    if( N == 0 ) return false;

    minError = grt_numeric_limits< Float >::max();
    Random random;
    UINT bestFeatureIndex = 0;
    Float bestThreshold = 0;
    Float error = 0;
    Vector< UINT > groupIndex(M);
    Vector< MinMax > ranges = trainingData.getRanges();
    MatrixDouble data(M,1); //This will store our temporary data for each dimension

    //Randomly select which features we want to use
    UINT numRandomFeatures = numSplittingSteps > N ? N : numSplittingSteps;
    Vector< UINT > randomFeatures = random.getRandomSubset( 0, N, numRandomFeatures );

    //Loop over each random feature and try and find the best split point
    for(UINT n=0; n<numRandomFeatures; n++){

        featureIndex = features[ randomFeatures[n] ];

        //Use the data in this feature dimension to create a sum dataset
        for(UINT i=0; i<M; i++){
            data[i][0] = trainingData[i][featureIndex];
        }

        if( computeError( trainingData, data, classLabels, ranges, groupIndex, featureIndex, threshold, error ) ){
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
     set( M, featureIndex, bestThreshold, trainingData.getClassProbabilities(classLabels) );

     return true;
}

bool DecisionTreeClusterNode::computeError( const ClassificationData &trainingData, MatrixFloat &data, const Vector< UINT > &classLabels, Vector< MinMax > ranges, Vector< UINT > groupIndex, const UINT featureIndex, Float &threshold, Float &error ){

    error = 0;
    threshold = 0;

    const UINT M = trainingData.getNumSamples();
    const UINT K = (UINT)classLabels.size();

    Float giniIndexL = 0;
    Float giniIndexR = 0;
    Float weightL = 0;
    Float weightR = 0;
    VectorFloat groupCounter(2,0);
    MatrixFloat classProbabilities(K,2);

    //Use this data to train a KMeans cluster with 2 clusters
    KMeans kmeans;
    kmeans.setNumClusters( 2 );
    kmeans.setComputeTheta( true );
    kmeans.setMinChange( 1.0e-5 );
    kmeans.setMinNumEpochs( 1 );
    kmeans.setMaxNumEpochs( 100 );

    //Disable the logging to clean things up
    kmeans.setTrainingLoggingEnabled( false );

    if( !kmeans.train_( data ) ){
        errorLog << "computeSplitError() - Failed to train KMeans model for feature: " << featureIndex << std::endl;
        return false;
    }

    //Set the split threshold as the mid point between the two clusters
    const MatrixFloat &clusters = kmeans.getClusters();
    threshold = 0;
    for(UINT i=0; i<clusters.getNumRows(); i++){
        threshold += clusters[i][0];
    }
    threshold /= clusters.getNumRows();

    //Iterate over each sample and work out if it should be in the lhs (0) or rhs (1) group based on the current threshold
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

    return true;
}

bool DecisionTreeClusterNode::saveParametersToFile( std::fstream &file ) const{

    if( !file.is_open() )
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

bool DecisionTreeClusterNode::loadParametersFromFile( std::fstream &file ){

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

