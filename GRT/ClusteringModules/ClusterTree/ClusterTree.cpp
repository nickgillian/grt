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

#define GRT_DLL_EXPORTS
#include "ClusterTree.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string ClusterTree::id = "ClusterTree";
std::string ClusterTree::getId() { return ClusterTree::id; }

//Register the ClusterTreeNode with the Node base class
RegisterNode< ClusterTreeNode > ClusterTreeNode::registerModule("ClusterTreeNode");
    
//Register the ClusterTree module with the Clusterer base class
RegisterClustererModule< ClusterTree >  ClusterTree::registerModule( ClusterTree::getId() );

ClusterTree::ClusterTree(const UINT numSplittingSteps,const UINT minNumSamplesPerNode,const UINT maxDepth,const bool removeFeaturesAtEachSpilt,const Tree::TrainingMode trainingMode,const bool useScaling,const Float minRMSErrorPerNode) : Clusterer( ClusterTree::getId() )
{
    tree = NULL;
    predictedClusterLabel = 0;
    this->numSplittingSteps = numSplittingSteps;
    this->minNumSamplesPerNode = minNumSamplesPerNode;
    this->maxDepth = maxDepth;
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    this->trainingMode = trainingMode;
    this->minRMSErrorPerNode = minRMSErrorPerNode;
}
    
ClusterTree::ClusterTree(const ClusterTree &rhs) : Clusterer( ClusterTree::getId() )
{
    tree = NULL;
    predictedClusterLabel = 0;
    *this = rhs;
}
    
ClusterTree::~ClusterTree(void)
{
    clear();
}
    
ClusterTree& ClusterTree::operator=(const ClusterTree &rhs){
    if( this != &rhs ){
        //Clear this tree
        this->clear();
        
        if( rhs.getTrained() ){
            //Deep copy the tree
            this->tree = dynamic_cast<ClusterTreeNode*>( rhs.deepCopyTree() );
        }
        
        this->numSplittingSteps = rhs.numSplittingSteps;
        this->minNumSamplesPerNode = rhs.minNumSamplesPerNode;
        this->maxDepth = rhs.maxDepth;
        this->removeFeaturesAtEachSpilt = rhs.removeFeaturesAtEachSpilt;
        this->trainingMode = rhs.trainingMode;
        this->minRMSErrorPerNode = rhs.minRMSErrorPerNode;
        this->predictedClusterLabel = rhs.predictedClusterLabel;
        
        //Copy the base variables
        copyBaseVariables( (Clusterer*)&rhs );
	}
	return *this;
}

bool ClusterTree::deepCopyFrom(const Clusterer *clusterer){
    
    if( clusterer == NULL ) return false;
    
    if( this->getId() == clusterer->getId() ){
        
        const ClusterTree *ptr = dynamic_cast<const ClusterTree*>(clusterer);
        
        //Clear this tree
        this->clear();
        
        if( ptr->getTrained() ){
            //Deep copy the tree
            this->tree = dynamic_cast<ClusterTreeNode*>( ptr->deepCopyTree() );
        }
        
        this->numSplittingSteps = ptr->numSplittingSteps;
        this->minNumSamplesPerNode = ptr->minNumSamplesPerNode;
        this->maxDepth = ptr->maxDepth;
        this->removeFeaturesAtEachSpilt = ptr->removeFeaturesAtEachSpilt;
        this->trainingMode = ptr->trainingMode;
        this->minRMSErrorPerNode = ptr->minRMSErrorPerNode;
        this->predictedClusterLabel = ptr->predictedClusterLabel;
        
        //Copy the base classifier variables
        return copyBaseVariables( clusterer );
    }
    return false;
}

bool ClusterTree::train_(MatrixFloat &trainingData){
    
    //Clear any previous model
    clear();
    
    const unsigned int M = trainingData.getNumRows();
    const unsigned int N = trainingData.getNumCols();
    
    if( M == 0 ){
        Clusterer::errorLog << "train_(MatrixFloat &trainingData) - Training data has zero samples!" << std::endl;
        return false;
    }
    
    numInputDimensions = N;
    numOutputDimensions = 1;
    ranges = trainingData.getRanges();
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }
    
    //Setup the valid features - at this point all features can be used
    Vector< UINT > features(N);
    for(UINT i=0; i<N; i++){
        features[i] = i;
    }
    
    //Build the tree
    UINT clusterLabel = 0;
    UINT nodeID = 0;
    tree = buildTree( trainingData, NULL, features, clusterLabel, nodeID );
    numClusters = clusterLabel;
    
    if( tree == NULL ){
        clear();
        Clusterer::errorLog << "train_(MatrixFloat &trainingData) - Failed to build tree!" << std::endl;
        return false;
    }
    
    //Flag that the algorithm has been trained
    trained = true;
    
    //Setup the cluster labels
    clusterLabels.resize(numClusters);
    for(UINT i=0; i<numClusters; i++){
        clusterLabels[i] = i+1;
    }
    clusterLikelihoods.resize(numClusters,0);
    clusterDistances.resize(numClusters,0);
    
    return true;
}

bool ClusterTree::predict_(VectorFloat &inputVector){
    
    if( !trained ){
        Clusterer::errorLog << "predict_(VectorFloat &inputVector) - Model Not Trained!" << std::endl;
        return false;
    }
    
    if( tree == NULL ){
        Clusterer::errorLog << "predict_(VectorFloat &inputVector) - DecisionTree pointer is null!" << std::endl;
        return false;
    }
    
	if( inputVector.size() != numInputDimensions ){
        Clusterer::errorLog << "predict_(VectorFloat &inputVector) - The size of the input Vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
    
    VectorFloat clusterLabel(1);
    if( !tree->predict( inputVector, clusterLabel ) ){
        Clusterer::errorLog << "predict_(VectorFloat &inputVector) - Failed to predict!" << std::endl;
        return false;
    }
    predictedClusterLabel = (UINT)clusterLabel[0];
    
    return true;
}
    
bool ClusterTree::clear(){
    
    //Clear the base variables
    Clusterer::clear();
    
    if( tree != NULL ){
        tree->clear();
        delete tree;
        tree = NULL;
    }
    
    return true;
}

bool ClusterTree::print() const{
    if( tree != NULL )
        return tree->print();
    return false;
}
    
bool ClusterTree::saveModelToFile( std::fstream &file ) const{
    
    if( !file.is_open() )
    {
        Clusterer::errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "GRT_CLUSTER_TREE_MODEL_FILE_V1.0" << std::endl;
    
    //Write the clusterer settings to the file
    if( !saveClustererSettingsToFile(file) ){
        Clusterer::errorLog <<"saveModelToFile(fstream &file) - Failed to save clusterer settings to file!" << std::endl;
        return false;
    }
    
	file << "NumSplittingSteps: " << numSplittingSteps << std::endl;
	file << "MinNumSamplesPerNode: " << minNumSamplesPerNode << std::endl;
    file << "MaxDepth: " << maxDepth << std::endl;
	file << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSpilt << std::endl;
	file << "TrainingMode: " << trainingMode << std::endl;
	file << "MinRMSErrorPerNode: " << minRMSErrorPerNode << std::endl;
	file << "TreeBuilt: " << (tree != NULL ? 1 : 0) << std::endl;

	if( tree != NULL ){
    	file << "Tree:\n";
    	if( !tree->save( file ) ){
        	Clusterer::errorLog << "saveModelToFile(fstream &file) - Failed to save tree to file!" << std::endl;
        	return false;
    	}
	}

	return true;
}
    
bool ClusterTree::loadModelFromFile( std::fstream &file ){
    
	clear();

	if(!file.is_open())
	{
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << std::endl;
    	return false;
	}

	std::string word;

	//Find the file type header
	file >> word;
	if(word != "GRT_CLUSTER_TREE_MODEL_FILE_V1.0"){
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find Model File Header" << std::endl;
    	return false;
	}

	//Load the base settings from the file
	if( !loadClustererSettingsFromFile(file) ){
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << std::endl;
    	return false;
	}

	file >> word;
	if(word != "NumSplittingSteps:"){
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the NumSplittingSteps!" << std::endl;
    	return false;
	}
	file >> numSplittingSteps;

	file >> word;
	if(word != "MinNumSamplesPerNode:"){
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the MinNumSamplesPerNode!" << std::endl;
    	return false;
	}
	file >> minNumSamplesPerNode;

	file >> word;
	if(word != "MaxDepth:"){
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the MaxDepth!" << std::endl;
    	return false;
	}
	file >> maxDepth;

	file >> word;
	if(word != "RemoveFeaturesAtEachSpilt:"){
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the RemoveFeaturesAtEachSpilt!" << std::endl;
    	return false;
	}
	file >> removeFeaturesAtEachSpilt;

	file >> word;
	if(word != "TrainingMode:"){
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the TrainingMode!" << std::endl;
    	return false;
	}
    UINT tempTrainingMode = 0;
	file >> tempTrainingMode;
    trainingMode = static_cast<Tree::TrainingMode>(tempTrainingMode);

	file >> word;
	if(word != "MinRMSErrorPerNode:"){
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the MinRMSErrorPerNode!" << std::endl;
    	return false;
	}	
	file >> minRMSErrorPerNode;

	file >> word;
	if(word != "TreeBuilt:"){
    	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the TreeBuilt!" << std::endl;
    	return false;
    }
	file >> trained;

	if( trained ){
    	file >> word;
    	if(word != "Tree:"){
        		Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the Tree!" << std::endl;
        		return false;
    	}

    	//Create a new tree
   		tree = new ClusterTreeNode;
    
    	if( tree == NULL ){
            clear();
            Clusterer::errorLog << "loadModelFromFile(fstream &file) - Failed to create new RegressionTreeNode!" << std::endl;
            return false;
    	}
    
    	tree->setParent( NULL );
    	if( !tree->load( file ) ){
            clear();
            Clusterer::errorLog << "loadModelFromFile(fstream &file) - Failed to load tree from file!" << std::endl;
            return false;
    	}
        
        //Setup the cluster labels
        clusterLabels.resize(numClusters);
        for(UINT i=0; i<numClusters; i++){
            clusterLabels[i] = i+1;
        }
        clusterLikelihoods.resize(numClusters,0);
        clusterDistances.resize(numClusters,0);
	}

	return true;
}
    
ClusterTreeNode* ClusterTree::deepCopyTree() const{
    
    if( tree == NULL ){
        return NULL;
    }

    return (ClusterTreeNode*)tree->deepCopyNode();
}

const ClusterTreeNode* ClusterTree::getTree() const{
    return (ClusterTreeNode*)tree;
}
    
UINT ClusterTree::getPredictedClusterLabel() const{
    return predictedClusterLabel;
}
    
Float ClusterTree::getMinRMSErrorPerNode() const{
    return minRMSErrorPerNode;
}

Tree::TrainingMode ClusterTree::getTrainingMode() const{
    return trainingMode;
}

UINT ClusterTree::getNumSplittingSteps()const{
    return numSplittingSteps;
}

UINT ClusterTree::getMinNumSamplesPerNode()const{
    return minNumSamplesPerNode;
}

UINT ClusterTree::getMaxDepth()const{
    return maxDepth;
}

UINT ClusterTree::getPredictedNodeID()const{
    
    if( tree == NULL ){
        return 0;
    }
    
    return tree->getPredictedNodeID();
}

bool ClusterTree::getRemoveFeaturesAtEachSpilt() const{
    return removeFeaturesAtEachSpilt;
}

bool ClusterTree::setTrainingMode(const Tree::TrainingMode trainingMode){ 
    if( trainingMode >= Tree::BEST_ITERATIVE_SPILT && trainingMode < Tree::NUM_TRAINING_MODES ){
        this->trainingMode = trainingMode;
        return true;
    }
    warningLog << "Unknown trainingMode: " << trainingMode << std::endl;
    return false;
}

bool ClusterTree::setNumSplittingSteps(const UINT numSplittingSteps){
    if( numSplittingSteps > 0 ){
        this->numSplittingSteps = numSplittingSteps;
        return true;
    }
    warningLog << "setNumSplittingSteps(const UINT numSplittingSteps) - The number of splitting steps must be greater than zero!" << std::endl;
    return false;
}

bool ClusterTree::setMinNumSamplesPerNode(const UINT minNumSamplesPerNode){
    if( minNumSamplesPerNode > 0 ){
        this->minNumSamplesPerNode = minNumSamplesPerNode;
        return true;
    }
    warningLog << "setMinNumSamplesPerNode(const UINT minNumSamplesPerNode) - The minimum number of samples per node must be greater than zero!" << std::endl;
    return false;
}

bool ClusterTree::setMaxDepth(const UINT maxDepth){
    if( maxDepth > 0 ){
        this->maxDepth = maxDepth;
        return true;
    }
    warningLog << "setMaxDepth(const UINT maxDepth) - The maximum depth must be greater than zero!" << std::endl;
    return false;
}

bool ClusterTree::setRemoveFeaturesAtEachSpilt(const bool removeFeaturesAtEachSpilt){
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    return true;
}

bool ClusterTree::setMinRMSErrorPerNode(const Float minRMSErrorPerNode){
    this->minRMSErrorPerNode = minRMSErrorPerNode;
    return true;
}
    
ClusterTreeNode* ClusterTree::buildTree(const MatrixFloat &trainingData,ClusterTreeNode *parent,Vector< UINT > features,UINT &clusterLabel,UINT nodeID){
    
    const UINT M = trainingData.getNumRows();
    
    //Update the nodeID

    //Get the depth
    UINT depth = 0;
    
    if( parent != NULL )
        depth = parent->getDepth() + 1;
    
    //If there are no training data then return NULL
    if( M == 0 )
        return NULL;
    
    //Create the new node
    ClusterTreeNode *node = new ClusterTreeNode;
    
    if( node == NULL )
        return NULL;
    
    //Set the parent
    node->initNode( parent, depth, nodeID );
    
    //If there are no features left then create a leaf node and return
    if( features.getSize() == 0 || M < minNumSamplesPerNode || depth >= maxDepth ){
        
        //Update the clusterLabel
        clusterLabel++;
        
        //Flag that this is a leaf node
        node->setIsLeafNode( true );
        
        //Set the node - the feature and threshold are both 0
        node->set( M, 0, 0, clusterLabel );
        
        Clusterer::trainingLog << "Reached leaf node. Depth: " << depth << " NumSamples: " << M << std::endl;
        
        return node;
    }
    
    //Compute the best spilt point
    UINT featureIndex = 0;
    Float threshold = 0;
    Float minError = 0;
    if( !computeBestSpilt( trainingData, features, featureIndex, threshold, minError ) ){
        delete node;
        return NULL;
    }
    
    Clusterer::trainingLog << "Depth: " << depth << " FeatureIndex: " << featureIndex << " Threshold: " << threshold << " MinError: " << minError << std::endl;
    
    //If the minError is below the minRMSError then create a leaf node and return
    if( minError <= minRMSErrorPerNode ){
        //Update the clusterLabel
        clusterLabel++;
        
        //Flag that this is a leaf node
        node->setIsLeafNode( true );
        
        //Set the node
        node->set( M, featureIndex, threshold, clusterLabel );
        
        Clusterer::trainingLog << "Reached leaf node. Depth: " << depth << " NumSamples: " << M << std::endl;
        
        return node;
    }
    
    //Set the node (any node that is not a leaf node has a cluster label of 0
    node->set( M, featureIndex, threshold, 0 );
    
    //Remove the selected feature so we will not use it again
    if( removeFeaturesAtEachSpilt ){
        for(UINT i=0; i<features.getSize(); i++){
            if( features[i] == featureIndex ){
                features.erase( features.begin()+i );
                break;
            }
        }
    }
    
    //Split the data
    MatrixFloat lhs;
    MatrixFloat rhs;
    
    for(UINT i=0; i<M; i++){
        if( node->predict( trainingData.getRow(i) ) ){
            rhs.push_back(trainingData.getRow(i));
        }else lhs.push_back(trainingData.getRow(i));
    }
    
    //Run the recursive tree building on the children
    node->setLeftChild( buildTree( lhs, node, features, clusterLabel, nodeID ) );
    node->setRightChild( buildTree( rhs, node, features, clusterLabel, nodeID ) );
    
    return node;
}
    
bool ClusterTree::computeBestSpilt( const MatrixFloat &trainingData, const Vector< UINT > &features, UINT &featureIndex, Float &threshold, Float &minError ){
    
    switch( trainingMode ){
        case Tree::BEST_ITERATIVE_SPILT:
            return computeBestSpiltBestIterativeSpilt( trainingData, features, featureIndex, threshold, minError );
            break;
        case Tree::BEST_RANDOM_SPLIT:
            return computeBestSpiltBestRandomSpilt( trainingData, features, featureIndex, threshold, minError );
            break;
        default:
            Clusterer::errorLog << "Uknown trainingMode!" << std::endl;
            return false;
            break;
    }
    
    return true;
}
    
bool ClusterTree::computeBestSpiltBestIterativeSpilt( const MatrixFloat &trainingData, const Vector< UINT > &features, UINT &featureIndex, Float &threshold, Float &minError ){
    
    const UINT M = trainingData.getNumRows();
    const UINT N = (UINT)features.size();
    
    //Clusterer::debugLog << "computeBestSpiltBestIterativeSpilt() M: " << M << std::endl;
    
    if( N == 0 ) return false;
    
    minError = grt_numeric_limits< Float >::max();
    UINT bestFeatureIndex = 0;
    UINT groupID = 0;
    Float bestThreshold = 0;
    Float error = 0;
    Float minRange = 0;
    Float maxRange = 0;
    Float step = 0;
    Vector< UINT > groupIndex(M);
    Vector< Float > groupCounter(2);
    Vector< Float > groupMean(2);
    Vector< Float > groupMSE(2);
    Vector< MinMax > ranges = trainingData.getRanges();

    //Loop over each feature and try and find the best split point
    for(UINT n=0; n<N; n++){
        minRange = ranges[n].minValue;
        maxRange = ranges[n].maxValue;
        step = (maxRange-minRange)/Float(numSplittingSteps);
        threshold = minRange;
        featureIndex = features[n];
        
        while( threshold <= maxRange ){
            
            //Reset the counters to zero
            groupCounter[0] = groupCounter[1] = 0;
            groupMean[0] = groupMean[1] = 0;
            groupMSE[0] = groupMSE[1] = 0;
            
            //Iterate over each sample and work out what group it falls into
            for(UINT i=0; i<M; i++){
                groupID = trainingData[i][featureIndex] >= threshold ? 1 : 0;
                groupIndex[i] = groupID;
                
                //Update the group mean and group counters
                groupMean[ groupID ] += trainingData[i][featureIndex];
                groupCounter[ groupID ]++;
            }
            
            //Compute the group mean
            groupMean[0] /= (groupCounter[0] > 0 ? groupCounter[0] : 1);
            groupMean[1] /= (groupCounter[1] > 0 ? groupCounter[1] : 1);
            
            //Compute the MSE for each group
            for(UINT i=0; i<M; i++){
                groupMSE[ groupIndex[i] ] += grt_sqr( groupMean[ groupIndex[i] ] - trainingData[i][featureIndex] );
            }
            groupMSE[0] /= (groupCounter[0] > 0 ? groupCounter[0] : 1);
            groupMSE[1] /= (groupCounter[1] > 0 ? groupCounter[1] : 1);
            
            error = grt_sqrt( groupMSE[0] + groupMSE[1] );
            
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
    
    //Set the best feature index and threshold
    featureIndex = bestFeatureIndex;
    threshold = bestThreshold;
    
    return true;
}
    
bool ClusterTree::computeBestSpiltBestRandomSpilt( const MatrixFloat &trainingData, const Vector< UINT > &features, UINT &featureIndex, Float &threshold, Float &minError ){
    
    const UINT M = trainingData.getNumRows();
    const UINT N = (UINT)features.size();
    
    debugLog << "computeBestSpiltBestRandomSpilt() M: " << M << std::endl;
    
    if( N == 0 ) return false;
    
    minError = grt_numeric_limits< Float >::max();
    UINT bestFeatureIndex = 0;
    UINT groupID = 0;
    Float bestThreshold = 0;
    Float error = 0;
    Vector< UINT > groupIndex(M);
    Vector< Float > groupCounter(2);
    Vector< Float > groupMean(2);
    Vector< Float > groupMSE(2);
    Vector< MinMax > ranges = trainingData.getRanges();
    
    //Loop over each feature and try and find the best split point
    for(UINT n=0; n<N; n++){
        featureIndex = features[n];
        
        for(UINT m=0; m<numSplittingSteps; m++){
            //Randomly select a threshold value
            threshold = random.getRandomNumberUniform(ranges[n].minValue,ranges[n].maxValue);
            
            //Reset the counters to zero
            groupCounter[0] = groupCounter[1] = 0;
            groupMean[0] = groupMean[1] = 0;
            groupMSE[0] = groupMSE[1] = 0;
            
            //Iterate over each sample and work out what group it falls into
            for(UINT i=0; i<M; i++){
                groupID = trainingData[i][featureIndex] >= threshold ? 1 : 0;
                groupIndex[i] = groupID;
                
                //Update the group mean and group counters
                groupMean[ groupID ] += trainingData[i][featureIndex];
                groupCounter[ groupID ]++;
            }
            
            //Compute the group mean
            groupMean[0] /= (groupCounter[0] > 0 ? groupCounter[0] : 1);
            groupMean[1] /= (groupCounter[1] > 0 ? groupCounter[1] : 1);
            
            //Compute the MSE for each group
            for(UINT i=0; i<M; i++){
                groupMSE[ groupIndex[i] ] += MLBase::SQR( groupMean[ groupIndex[i] ] - trainingData[i][featureIndex] );
            }
            groupMSE[0] /= (groupCounter[0] > 0 ? groupCounter[0] : 1);
            groupMSE[1] /= (groupCounter[1] > 0 ? groupCounter[1] : 1);
            
            error = sqrt( groupMSE[0] + groupMSE[1] );
            
            //Store the best threshold and feature index
            if( error < minError ){
                minError = error;
                bestThreshold = threshold;
                bestFeatureIndex = featureIndex;
            }
        }
    }
    
    //Set the best feature index and threshold
    featureIndex = bestFeatureIndex;
    threshold = bestThreshold;
    
    return true;
}
    
GRT_END_NAMESPACE

