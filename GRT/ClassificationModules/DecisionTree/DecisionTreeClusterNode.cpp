
#include "DecisionTreeClusterNode.h"

using namespace GRT;

void do_work(){}

bool decision_tree_cluster_node_compute_split_error( const UINT featureIndex,
                                                     const ClassificationData &trainingData,
                                                     const vector< UINT > &classLabels,
                                                     double &threshold,
                                                     double &error ){
    
    threshold = 0;
    error = numeric_limits<double>::max();

    const UINT M = trainingData.getNumSamples();
    const UINT K = (UINT)classLabels.size();
    double giniIndexL = 0;
    double giniIndexR = 0;
    double weightL = 0;
    double weightR = 0;
    vector< UINT > groupIndex(M);
    VectorDouble groupCounter(2,0);
    vector< MinMax > ranges = trainingData.getRanges();
    MatrixDouble classProbabilities(K,2);
    MatrixDouble data(M,1); //This will store our temporary data for each dimension
    
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
    
    //Disable the logging as this will be a mess with multiple threads
    kmeans.setTrainingLoggingEnabled( false );
    
    if( !kmeans.train( data ) ){
        return false;
    }
    
    //Set the split threshold as the mid point between the two clusters
    MatrixDouble clusters = kmeans.getClusters();
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
        classProbabilities[ DecisionTreeNode::getClassLabelIndexValue(trainingData[i].getClassLabel(),classLabels) ][ groupIndex[i] ]++;
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
    
//Register the DecisionTreeClusterNode module with the Node base class
RegisterNode< DecisionTreeClusterNode > DecisionTreeClusterNode::registerModule("DecisionTreeClusterNode");
    
DecisionTreeClusterNode::DecisionTreeClusterNode(){
    nodeType = "DecisionTreeClusterNode";
    parent = NULL;
    leftChild = NULL;
    rightChild = NULL;
    infoLog.setProceedingText("[DecisionTreeClusterNode]");
    warningLog.setProceedingText("[WARNING DecisionTreeClusterNode]");
    errorLog.setProceedingText("[ERROR DecisionTreeClusterNode]");
    trainingLog.setProceedingText("[TRAINING DecisionTreeClusterNode]");
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
    
    if( N == 0 ) return false;
    
    //Randomly select which features we want to use
    Random random;
    const UINT numRandomFeatures = numSplittingSteps > N ? N : numSplittingSteps;
    vector< UINT > randomFeatures = random.getRandomSubset( 0, N, numRandomFeatures );
    
    //Setup the vectors that will store the errors and thresholds for each random feature
    vector< double > errors( numRandomFeatures, numeric_limits<double>::max() );
    vector< double > thresholds( numRandomFeatures, 0 );

    //Loop over each random feature and try and find the best split point
    //This is done using a thread pool to speed up the search
    
    //Setup the thread pool
    vector< std::future<bool> > results( numRandomFeatures );
    ThreadPool pool;
    
    //Add a new job for each random feature
    for(UINT n=0; n<numRandomFeatures; n++){
        results[n] = pool.enqueue( &decision_tree_cluster_node_compute_split_error,
                     features[ randomFeatures[n] ],
                     std::ref(trainingData),
                     std::ref(classLabels),
                     std::ref(thresholds[n]),
                     std::ref(errors[n]) );
    }
    
    //Wait for each job to complete
    for(UINT n=0; n<numRandomFeatures; n++){
        
        if( results[n].get() ){
            trainingLog << "random feature: " << n+1 << "/" << numRandomFeatures;
            trainingLog << " featureIndex: " <<  features[ randomFeatures[n] ];
            trainingLog << " threshold: " << thresholds[n];
            trainingLog << " error: " << errors[n] << endl;
        }else{
            warningLog << "Failed to get results for random feature: " << n << endl;
        }

    }
    
    //Loop over the results and find the feature with the minimum error
    UINT bestFeatureIndex = 0;
    double bestThreshold = 0;
    minError = numeric_limits<double>::max();
    for(UINT n=0; n<numRandomFeatures; n++){
        
        //Store the best threshold and feature index
        if( errors[n] < minError ){
            minError = errors[n];
            bestThreshold = thresholds[n];
            bestFeatureIndex = features[ randomFeatures[n] ];
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

