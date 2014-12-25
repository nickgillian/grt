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

#include "ClusterTree.h"

namespace GRT{

//Register the ClusterTreeNode with the Node base class
RegisterNode< ClusterTreeNode > ClusterTreeNode::registerModule("ClusterTreeNode");
    
//Register the ClusterTree module with the Clusterer base class
RegisterClustererModule< ClusterTree >  ClusterTree::registerModule("ClusterTree");

ClusterTree::ClusterTree(const UINT numSplittingSteps,const UINT minNumSamplesPerNode,const UINT maxDepth,const bool removeFeaturesAtEachSpilt,const UINT trainingMode,const bool useScaling,const double minRMSErrorPerNode){
    tree = NULL;
    predictedClusterLabel = 0;
    this->numSplittingSteps = numSplittingSteps;
    this->minNumSamplesPerNode = minNumSamplesPerNode;
    this->maxDepth = maxDepth;
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    this->trainingMode = trainingMode;
    this->minRMSErrorPerNode = minRMSErrorPerNode;
    Clusterer::classType = "ClusterTree";
    clustererType = Clusterer::classType;
    Clusterer::debugLog.setProceedingText("[DEBUG ClusterTree]");
    Clusterer::errorLog.setProceedingText("[ERROR ClusterTree]");
    Clusterer::trainingLog.setProceedingText("[TRAINING ClusterTree]");
    Clusterer::warningLog.setProceedingText("[WARNING ClusterTree]");
}
    
ClusterTree::ClusterTree(const ClusterTree &rhs){
    tree = NULL;
    predictedClusterLabel = 0;
    Clusterer::classType = "ClusterTree";
    clustererType = Clusterer::classType;
    Clusterer::debugLog.setProceedingText("[DEBUG ClusterTree]");
    Clusterer::errorLog.setProceedingText("[ERROR ClusterTree]");
    Clusterer::trainingLog.setProceedingText("[TRAINING ClusterTree]");
    Clusterer::warningLog.setProceedingText("[WARNING ClusterTree]");
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
            this->tree = (ClusterTreeNode*)rhs.deepCopyTree();
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
    
    if( this->getClustererType() == clusterer->getClustererType() ){
        
        ClusterTree *ptr = (ClusterTree*)clusterer;
        
        //Clear this tree
        this->clear();
        
        if( ptr->getTrained() ){
            //Deep copy the tree
            this->tree = (ClusterTreeNode*)ptr->deepCopyTree();
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

bool ClusterTree::train_(MatrixDouble &trainingData){
    
    //Clear any previous model
    clear();
    
    const unsigned int M = trainingData.getNumRows();
    const unsigned int N = trainingData.getNumCols();
    
    if( M == 0 ){
        Clusterer::errorLog << "train_(MatrixDouble &trainingData) - Training data has zero samples!" << endl;
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
    vector< UINT > features(N);
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
        Clusterer::errorLog << "train_(MatrixDouble &trainingData) - Failed to build tree!" << endl;
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

bool ClusterTree::predict_(VectorDouble &inputVector){
    
    if( !trained ){
        Clusterer::errorLog << "predict_(VectorDouble &inputVector) - Model Not Trained!" << endl;
        return false;
    }
    
    if( tree == NULL ){
        Clusterer::errorLog << "predict_(VectorDouble &inputVector) - DecisionTree pointer is null!" << endl;
        return false;
    }
    
	if( inputVector.size() != numInputDimensions ){
        Clusterer::errorLog << "predict_(VectorDouble &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
    
    VectorDouble clusterLabel(1);
    if( !tree->predict( inputVector, clusterLabel ) ){
        Clusterer::errorLog << "predict_(VectorDouble &inputVector) - Failed to predict!" << endl;
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
    
bool ClusterTree::saveModelToFile(fstream &file) const{
    
        if( !file.is_open() )
        {
            Clusterer::errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
            return false;
        }
    
        //Write the header info
        file << "GRT_CLUSTER_TREE_MODEL_FILE_V1.0" << endl;
    
        //Write the clusterer settings to the file
    	if( !saveClustererSettingsToFile(file) ){
        	Clusterer::errorLog <<"saveModelToFile(fstream &file) - Failed to save clusterer settings to file!" << endl;
            return false;
    	}
    
    	file << "NumSplittingSteps: " << numSplittingSteps << endl;
    	file << "MinNumSamplesPerNode: " << minNumSamplesPerNode << endl;
        file << "MaxDepth: " << maxDepth << endl;
    	file << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSpilt << endl;
    	file << "TrainingMode: " << trainingMode << endl;
    	file << "MinRMSErrorPerNode: " << minRMSErrorPerNode << endl;
    	file << "TreeBuilt: " << (tree != NULL ? 1 : 0) << endl;
    
    	if( tree != NULL ){
        	file << "Tree:\n";
        	if( !tree->saveToFile( file ) ){
            	Clusterer::errorLog << "saveModelToFile(fstream &file) - Failed to save tree to file!" << endl;
            	return false;
        	}
    	}
    
    	return true;
}
    
bool ClusterTree::loadModelFromFile(fstream &file){
    
    	clear();
    
    	if(!file.is_open())
    	{
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << endl;
        	return false;
    	}
    
    	std::string word;
    
    	//Find the file type header
    	file >> word;
    	if(word != "GRT_CLUSTER_TREE_MODEL_FILE_V1.0"){
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find Model File Header" << endl;
        	return false;
    	}
    
    	//Load the base settings from the file
    	if( !loadClustererSettingsFromFile(file) ){
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        	return false;
    	}
    
    	file >> word;
    	if(word != "NumSplittingSteps:"){
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the NumSplittingSteps!" << endl;
        	return false;
    	}
    	file >> numSplittingSteps;
    
    	file >> word;
    	if(word != "MinNumSamplesPerNode:"){
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the MinNumSamplesPerNode!" << endl;
        	return false;
    	}
    	file >> minNumSamplesPerNode;
    
    	file >> word;
    	if(word != "MaxDepth:"){
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the MaxDepth!" << endl;
        	return false;
    	}
    	file >> maxDepth;
    
    	file >> word;
    	if(word != "RemoveFeaturesAtEachSpilt:"){
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the RemoveFeaturesAtEachSpilt!" << endl;
        	return false;
    	}
    	file >> removeFeaturesAtEachSpilt;
    
    	file >> word;
    	if(word != "TrainingMode:"){
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the TrainingMode!" << endl;
        	return false;
    	}
    	file >> trainingMode;
    
    	file >> word;
    	if(word != "MinRMSErrorPerNode:"){
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the MinRMSErrorPerNode!" << endl;
        	return false;
    	}	
    	file >> minRMSErrorPerNode;
    
    	file >> word;
    	if(word != "TreeBuilt:"){
        	Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the TreeBuilt!" << endl;
        	return false;
        }
    	file >> trained;
    
    	if( trained ){
        	file >> word;
        	if(word != "Tree:"){
            		Clusterer::errorLog << "loadModelFromFile(string filename) - Could not find the Tree!" << endl;
            		return false;
        	}
    
        	//Create a new tree
       		tree = new ClusterTreeNode;
        
        	if( tree == NULL ){
                clear();
                Clusterer::errorLog << "loadModelFromFile(fstream &file) - Failed to create new RegressionTreeNode!" << endl;
                return false;
        	}
        
        	tree->setParent( NULL );
        	if( !tree->loadFromFile( file ) ){
                clear();
                Clusterer::errorLog << "loadModelFromFile(fstream &file) - Failed to load tree from file!" << endl;
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
    
double ClusterTree::getMinRMSErrorPerNode() const{
    return minRMSErrorPerNode;
}
    
bool ClusterTree::setMinRMSErrorPerNode(const double minRMSErrorPerNode){
    this->minRMSErrorPerNode = minRMSErrorPerNode;
    return true;
}
    
ClusterTreeNode* ClusterTree::buildTree(const MatrixDouble &trainingData,ClusterTreeNode *parent,vector< UINT > features,UINT &clusterLabel,UINT nodeID){
    
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
    if( features.size() == 0 || M < minNumSamplesPerNode || depth >= maxDepth ){
        
        //Update the clusterLabel
        clusterLabel++;
        
        //Flag that this is a leaf node
        node->setIsLeafNode( true );
        
        //Set the node - the feature and threshold are both 0
        node->set( M, 0, 0, clusterLabel );
        
        Clusterer::trainingLog << "Reached leaf node. Depth: " << depth << " NumSamples: " << M << endl;
        
        return node;
    }
    
    //Compute the best spilt point
    UINT featureIndex = 0;
    double threshold = 0;
    double minError = 0;
    if( !computeBestSpilt( trainingData, features, featureIndex, threshold, minError ) ){
        delete node;
        return NULL;
    }
    
    Clusterer::trainingLog << "Depth: " << depth << " FeatureIndex: " << featureIndex << " Threshold: " << threshold << " MinError: " << minError << endl;
    
    //If the minError is below the minRMSError then create a leaf node and return
    if( minError <= minRMSErrorPerNode ){
        //Update the clusterLabel
        clusterLabel++;
        
        //Flag that this is a leaf node
        node->setIsLeafNode( true );
        
        //Set the node
        node->set( M, featureIndex, threshold, clusterLabel );
        
        Clusterer::trainingLog << "Reached leaf node. Depth: " << depth << " NumSamples: " << M << endl;
        
        return node;
    }
    
    //Set the node (any node that is not a leaf node has a cluster label of 0
    node->set( M, featureIndex, threshold, 0 );
    
    //Remove the selected feature so we will not use it again
    if( removeFeaturesAtEachSpilt ){
        for(size_t i=0; i<features.size(); i++){
            if( features[i] == featureIndex ){
                features.erase( features.begin()+i );
                break;
            }
        }
    }
    
    //Split the data
    MatrixDouble lhs;
    MatrixDouble rhs;
    
    for(UINT i=0; i<M; i++){
        if( node->predict( trainingData.getRowVector(i) ) ){
            rhs.push_back(trainingData.getRowVector(i));
        }else lhs.push_back(trainingData.getRowVector(i));
    }
    
    //Run the recursive tree building on the children
    node->setLeftChild( buildTree( lhs, node, features, clusterLabel, nodeID ) );
    node->setRightChild( buildTree( rhs, node, features, clusterLabel, nodeID ) );
    
    return node;
}
    
bool ClusterTree::computeBestSpilt( const MatrixDouble &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError ){
    
    switch( trainingMode ){
        case BEST_ITERATIVE_SPILT:
            return computeBestSpiltBestIterativeSpilt( trainingData, features, featureIndex, threshold, minError );
            break;
        case BEST_RANDOM_SPLIT:
            return computeBestSpiltBestRandomSpilt( trainingData, features, featureIndex, threshold, minError );
            break;
        default:
            Clusterer::errorLog << "Uknown trainingMode!" << endl;
            return false;
            break;
    }
    
    return true;
}
    
bool ClusterTree::computeBestSpiltBestIterativeSpilt( const MatrixDouble &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError ){
    
    const UINT M = trainingData.getNumRows();
    const UINT N = (UINT)features.size();
    
    //Clusterer::debugLog << "computeBestSpiltBestIterativeSpilt() M: " << M << endl;
    
    if( N == 0 ) return false;
    
    minError = numeric_limits<double>::max();
    UINT bestFeatureIndex = 0;
    UINT groupID = 0;
    double bestThreshold = 0;
    double error = 0;
    double minRange = 0;
    double maxRange = 0;
    double step = 0;
    vector< UINT > groupIndex(M);
    vector< double > groupCounter(2);
    vector< double > groupMean(2);
    vector< double > groupMSE(2);
    vector< MinMax > ranges = trainingData.getRanges();

    //Loop over each feature and try and find the best split point
    for(UINT n=0; n<N; n++){
        minRange = ranges[n].minValue;
        maxRange = ranges[n].maxValue;
        step = (maxRange-minRange)/double(numSplittingSteps);
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
            
            //Update the threshold
            threshold += step;
        }
    }
    
    //Set the best feature index and threshold
    featureIndex = bestFeatureIndex;
    threshold = bestThreshold;
    
    return true;
}
    
bool ClusterTree::computeBestSpiltBestRandomSpilt( const MatrixDouble &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError ){
    
    const UINT M = trainingData.getNumRows();
    const UINT N = (UINT)features.size();
    
    Clusterer::debugLog << "computeBestSpiltBestRandomSpilt() M: " << M << endl;
    
    if( N == 0 ) return false;
    
    minError = numeric_limits<double>::max();
    UINT bestFeatureIndex = 0;
    UINT groupID = 0;
    double bestThreshold = 0;
    double error = 0;
    vector< UINT > groupIndex(M);
    vector< double > groupCounter(2);
    vector< double > groupMean(2);
    vector< double > groupMSE(2);
    vector< MinMax > ranges = trainingData.getRanges();
    
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
    
} //End of namespace GRT

