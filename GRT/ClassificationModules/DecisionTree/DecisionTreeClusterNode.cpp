
#include "DecisionTreeClusterNode.h"

using namespace GRT;

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

bool DecisionTreeClusterNode::predict(const VectorDouble &x) {

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

    ostringstream stream;

    if( getModel( stream ) ){
        cout << stream.str();
        return true;
    }

    return false;
}

bool DecisionTreeClusterNode::computeFeatureWeights( VectorDouble &weights ) const{

    if( isLeafNode ){ //If we reach a leaf node, no weight update needed
        return true;
    }
    
    if( featureIndex >= ((UINT)weights.size()) ){ //Feature index is out of bounds
        warningLog << "computeFeatureWeights( VectorDouble &weights ) - Feature index is greater than weights vector size!" << endl;
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

bool DecisionTreeClusterNode::getModel(ostream &stream) const{

    string tab = "";
    for(UINT i=0; i<depth; i++) tab += "\t";

    stream << tab << "depth: " << depth;
    stream << " nodeSize: " << nodeSize;
    stream << " featureIndex: " << featureIndex;
    stream << " threshold " << threshold;
    stream << " isLeafNode: " << isLeafNode << endl;

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

double DecisionTreeClusterNode::getThreshold() const{
    return threshold;
}

bool DecisionTreeClusterNode::set(const UINT nodeSize,const UINT featureIndex,const double threshold,const VectorDouble &classProbabilities){
    this->nodeSize = nodeSize;
    this->featureIndex = featureIndex;
    this->threshold = threshold;
    this->classProbabilities = classProbabilities;
    return true;
}

bool DecisionTreeClusterNode::computeBestSpiltBestIterativeSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){

    return computeBestSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError);
}

bool DecisionTreeClusterNode::computeBestSpiltBestRandomSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){

    return computeBestSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError);
}

bool DecisionTreeClusterNode::computeBestSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){

    const UINT M = trainingData.getNumSamples();
    const UINT N = (UINT)features.size();
    const UINT K = (UINT)classLabels.size();

    if( N == 0 ) return false;

    minError = numeric_limits<double>::max();
    Random random;
    UINT bestFeatureIndex = 0;
    double bestThreshold = 0;
    double error = 0;
    double giniIndexL = 0;
    double giniIndexR = 0;
    double weightL = 0;
    double weightR = 0;
    vector< UINT > groupIndex(M);
    VectorDouble groupCounter(2,0);
    vector< MinMax > ranges = trainingData.getRanges();
    MatrixDouble classProbabilities(K,2);
    MatrixDouble data(M,1); //This will store our temporary data for each dimension

    //Randomly select which features we want to use
    UINT numRandomFeatures = numSplittingSteps > N ? N : numSplittingSteps;
    vector< UINT > randomFeatures = random.getRandomSubset( 0, N, numRandomFeatures );

    //Loop over each random feature and try and find the best split point
    for(UINT n=0; n<numRandomFeatures; n++){

        featureIndex = features[ randomFeatures[n] ];

        //Use the data in this feature dimension to create a sum dataset
        for(UINT i=0; i<M; i++){
            data[i][0] = trainingData[i][featureIndex];
        }

        //Use this data to train a KMeans cluster with 2 clusters
        KMeans kmeans;
        kmeans.setNumClusters( 2 );
        kmeans.setComputeTheta( true );
        kmeans.setMinChange( 1.0e-5 );
        kmeans.setMinNumEpochs( 1 );
        kmeans.setMaxNumEpochs( 100 );

        //Disable the logging to clean things up
        kmeans.setTrainingLoggingEnabled( false );

        if( !kmeans.train( data ) ){
            errorLog << "computeBestSpilt() - Failed to train KMeans model for feature: " << featureIndex << endl;
            return false;
        }

        //Set the split threshold as the mid point between the two clusters
        MatrixDouble clusters = kmeans.getClusters();
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

bool DecisionTreeClusterNode::saveParametersToFile(fstream &file) const{

    if( !file.is_open() )
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

bool DecisionTreeClusterNode::loadParametersFromFile(fstream &file){

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

