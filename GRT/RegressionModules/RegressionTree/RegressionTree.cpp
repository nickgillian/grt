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

#include "RegressionTree.h"

namespace GRT{

//Register the DecisionTree module with the Classifier base class
RegisterRegressifierModule< RegressionTree >  RegressionTree::registerModule("RegressionTree");
    
RegisterNode< RegressionTreeNode > RegressionTreeNode::registerModule("RegressionTreeNode");

RegressionTree::RegressionTree(const UINT numSplittingSteps,const UINT minNumSamplesPerNode,const UINT maxDepth,const bool removeFeaturesAtEachSpilt,const UINT trainingMode,const bool useScaling)
{
    tree = NULL;
    this->numSplittingSteps = numSplittingSteps;
    this->minNumSamplesPerNode = minNumSamplesPerNode;
    this->maxDepth = maxDepth;
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    this->trainingMode = trainingMode;
    this->useScaling = useScaling;
    classType = "RegressionTree";
    regressifierType = classType;
    debugLog.setProceedingText("[DEBUG RegressionTree]");
    errorLog.setProceedingText("[ERROR RegressionTree]");
    trainingLog.setProceedingText("[TRAINING RegressionTree]");
    warningLog.setProceedingText("[WARNING RegressionTree]");
    
}
    
RegressionTree::RegressionTree(const RegressionTree &rhs){
    tree = NULL;
    classType = "RegressionTree";
    regressifierType = classType;
    debugLog.setProceedingText("[DEBUG RegressionTree]");
    errorLog.setProceedingText("[ERROR RegressionTree]");
    trainingLog.setProceedingText("[TRAINING RegressionTree]");
    warningLog.setProceedingText("[WARNING RegressionTree]");
    *this = rhs;
}

RegressionTree::~RegressionTree(void)
{
    clear();
}
    
RegressionTree& RegressionTree::operator=(const RegressionTree &rhs){
	if( this != &rhs ){
        //Clear this tree
        clear();
        
        if( rhs.getTrained() ){
            //Deep copy the tree
            tree = (RegressionTreeNode*)rhs.deepCopyTree();
        }
        
        this->numSplittingSteps = rhs.numSplittingSteps;
        this->minNumSamplesPerNode = rhs.minNumSamplesPerNode;
        this->maxDepth = rhs.maxDepth;
        this->removeFeaturesAtEachSpilt = rhs.removeFeaturesAtEachSpilt;
        this->trainingMode = rhs.trainingMode;

        //Copy the base classifier variables
        copyBaseVariables( (Regressifier*)&rhs );
	}
	return *this;
}

bool RegressionTree::deepCopyFrom(const Regressifier *regressifier){
    
    if( regressifier == NULL ) return false;
    
    if( this->getRegressifierType() == regressifier->getRegressifierType() ){
        
        RegressionTree *ptr = (RegressionTree*)regressifier;
        
        //Clear this tree
        this->clear();
        
        if( ptr->getTrained() ){
            //Deep copy the decision tree
            this->tree = (RegressionTreeNode*)ptr->deepCopyTree();
        }
        
        this->numSplittingSteps = ptr->numSplittingSteps;
        this->minNumSamplesPerNode = ptr->minNumSamplesPerNode;
        this->maxDepth = ptr->maxDepth;
        this->removeFeaturesAtEachSpilt = ptr->removeFeaturesAtEachSpilt;
        this->trainingMode = ptr->trainingMode;
        
        //Copy the base classifier variables
        return copyBaseVariables( regressifier );
    }
    return false;
}

bool RegressionTree::train_(RegressionData &trainingData){
    
    //Clear any previous model
    clear();
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumInputDimensions();
    const unsigned int T = trainingData.getNumTargetDimensions();
    
    if( M == 0 ){
        errorLog << "train_(RegressionData &trainingData) - Training data has zero samples!" << endl;
        return false;
    }
    
    numInputDimensions = N;
    numOutputDimensions = T;
    inputVectorRanges = trainingData.getInputRanges();
    targetVectorRanges = trainingData.getTargetRanges();
    
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
    tree = buildTree( trainingData, NULL, features );
    
    if( tree == NULL ){
        clear();
        errorLog << "train_(RegressionData &trainingData) - Failed to build tree!" << endl;
        return false;
    }
    
    //Flag that the algorithm has been trained
    trained = true;
    
    return true;
}

bool RegressionTree::predict_(VectorDouble &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - Model Not Trained!" << endl;
        return false;
    }
    
    if( tree == NULL ){
        errorLog << "predict_(VectorDouble &inputVector) - DecisionTree pointer is null!" << endl;
        return false;
    }
    
    
    
    return true;
}
    
bool RegressionTree::clear(){
    
    //Clear the Classifier variables
    Regressifier::clear();
    
    if( tree != NULL ){
        tree->clear();
        delete tree;
        tree = NULL;
    }
    
    return true;
}

bool RegressionTree::print() const{
    if( tree != NULL )
        return tree->print();
    return false;
}
    
bool RegressionTree::saveModelToFile(string filename) const{

    if( !trained ) return false;
    
	std::fstream file; 
	file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }

	file.close();

	return true;
}
    
bool RegressionTree::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file << "GRT_REGRESSION_TREE_MODEL_FILE_V1.0\n";
    
    //Write the classifier settings to the file
    if( !Regressifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    file << "NumSplittingSteps: " << numSplittingSteps << endl;
    file << "MinNumSamplesPerNode: " << minNumSamplesPerNode << endl;
    file << "MaxDepth: " << maxDepth << endl;
    file << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSpilt << endl;
    file << "TrainingMode: " << trainingMode << endl;
    file << "TreeBuilt: " << (tree != NULL ? 1 : 0) << endl;
    
    if( tree != NULL ){
        file << "Tree:\n";
        if( !tree->saveToFile( file ) ){
            errorLog << "saveModelToFile(fstream &file) - Failed to save tree to file!" << endl;
            return false;
        }
    }
    
    return true;
}

bool RegressionTree::loadModelFromFile(string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    file.close();

	return true;

}
    
bool RegressionTree::loadModelFromFile(fstream &file){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << endl;
        return false;
    }
    
    std::string word;
    file >> word;
    
    //Find the file type header
    if(word != "GRT_REGRESSION_TREE_MODEL_FILE_V1.0"){
        errorLog << "loadModelFromFile(string filename) - Could not find Model File Header" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Regressifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if(word != "NumSplittingSteps:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the NumSplittingSteps!" << endl;
        return false;
    }
    file >> numSplittingSteps;
    
    file >> word;
    if(word != "MinNumSamplesPerNode:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the MinNumSamplesPerNode!" << endl;
        return false;
    }
    file >> minNumSamplesPerNode;
    
    file >> word;
    if(word != "MaxDepth:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the MaxDepth!" << endl;
        return false;
    }
    file >> maxDepth;
    
    file >> word;
    if(word != "RemoveFeaturesAtEachSpilt:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the RemoveFeaturesAtEachSpilt!" << endl;
        return false;
    }
    file >> removeFeaturesAtEachSpilt;
    
    file >> word;
    if(word != "TrainingMode:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the TrainingMode!" << endl;
        return false;
    }
    file >> trainingMode;
    
    file >> word;
    if(word != "TreeBuilt:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the TreeBuilt!" << endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        file >> word;
        if(word != "Tree:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the Tree!" << endl;
            return false;
        }
    
        //Create a new tree
        tree = new RegressionTreeNode;
        
        if( tree == NULL ){
            clear();
            errorLog << "loadModelFromFile(fstream &file) - Failed to create new RegressionTreeNode!" << endl;
            return false;
        }
        
        tree->setParent( NULL );
        if( !tree->loadFromFile( file ) ){
            clear();
            errorLog << "loadModelFromFile(fstream &file) - Failed to load tree from file!" << endl;
            return false;
        }
    }
    
    return true;
}
    
RegressionTreeNode* RegressionTree::deepCopyTree() const{
    
    if( tree == NULL ){
        return NULL;
    }

    return tree->deepCopyTree();
}

const RegressionTreeNode* RegressionTree::getTree() const{
    return tree;
}
    
UINT RegressionTree::getTrainingMode() const{
    return trainingMode;
}
    
UINT RegressionTree::getNumSplittingSteps()const{
    return numSplittingSteps;
}

UINT RegressionTree::getMinNumSamplesPerNode()const{
    return minNumSamplesPerNode;
}

UINT RegressionTree::getMaxDepth()const{
    return maxDepth;
}
    
bool RegressionTree::getRemoveFeaturesAtEachSpilt() const{
    return removeFeaturesAtEachSpilt;
}
    
bool RegressionTree::setTrainingMode(const UINT trainingMode){
    if( trainingMode >= BEST_ITERATIVE_SPILT && trainingMode < NUM_TRAINING_MODES ){
        this->trainingMode = trainingMode;
        return true;
    }
    warningLog << "Unknown trainingMode: " << trainingMode << endl;
    return false;
}
    
bool RegressionTree::setNumSplittingSteps(const UINT numSplittingSteps){
    if( numSplittingSteps > 0 ){
        this->numSplittingSteps = numSplittingSteps;
        return true;
    }
    return false;
}

bool RegressionTree::setMinNumSamplesPerNode(const UINT minNumSamplesPerNode){
    if( minNumSamplesPerNode > 0 ){
        this->minNumSamplesPerNode = minNumSamplesPerNode;
        return true;
    }
    return false;
}

bool RegressionTree::setMaxDepth(const UINT maxDepth){
    if( maxDepth > 0 ){
        this->maxDepth = maxDepth;
        return true;
    }
    return false;
}
    
bool RegressionTree::setRemoveFeaturesAtEachSpilt(const bool removeFeaturesAtEachSpilt){
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    return true;
}
    
RegressionTreeNode* RegressionTree::buildTree(const RegressionData &trainingData,RegressionTreeNode *parent,vector< UINT > features){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = trainingData.getNumInputDimensions();
    const UINT T = trainingData.getNumTargetDimensions();
    VectorDouble nodeHistogram; //TODO!
    
    //Get the depth
    UINT depth = 0;
    
    if( parent != NULL )
        depth = parent->getDepth() + 1;
    
    //If there are no training data then return NULL
    if( trainingData.getNumSamples() == 0 )
        return NULL;
    
    //Create the new node
    RegressionTreeNode *node = new RegressionTreeNode;
    
    if( node == NULL )
        return NULL;
    
    //Set the parent
    node->initNode( parent, depth );
    
    //If there are no features left then create a leaf node and return
    if( features.size() == 0 || M < minNumSamplesPerNode || depth >= maxDepth ){
        
        //Flag that this is a leaf node
        node->setIsLeafNode( true );
        
        //Set the node
        node->set( trainingData.getNumSamples(), 0, 0, nodeHistogram );
        
        trainingLog << "Reached leaf node. Depth: " << depth << " NumSamples: " << trainingData.getNumSamples() << endl;
        
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
    
    trainingLog << "Depth: " << depth << " FeatureIndex: " << featureIndex << " Threshold: " << threshold << " MinError: " << minError << endl;
    
    //Set the node
    node->set( trainingData.getNumSamples(), features[ featureIndex ], threshold, nodeHistogram );
    
    //Remove the selected feature so we will not use it again
    if( removeFeaturesAtEachSpilt ){
        features.erase( features.begin()+featureIndex );
    }
    
    //Split the data
    RegressionData lhs(N,T);
    RegressionData rhs(N,T);
    
    for(UINT i=0; i<M; i++){
        if( node->predict( trainingData[i].getInputVector() ) ){
            rhs.addSample(trainingData[i].getInputVector(), trainingData[i].getTargetVector());
        }else lhs.addSample(trainingData[i].getInputVector(), trainingData[i].getTargetVector());
    }
    
    //Run the recursive tree building on the children
    node->setLeftChild( buildTree( lhs, node, features ) );
    node->setRightChild( buildTree( rhs, node, features ) );
    
    return node;
}
    
bool RegressionTree::computeBestSpilt( const RegressionData &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError ){
    
    switch( trainingMode ){
        case BEST_ITERATIVE_SPILT:
            return computeBestSpiltBestIterativeSpilt( trainingData, features, featureIndex, threshold, minError );
            break;
        case BEST_RANDOM_SPLIT:
            //return computeBestSpiltBestRandomSpilt( trainingData, features, featureIndex, threshold, minError );
            break;
        default:
            errorLog << "Uknown trainingMode!" << endl;
            return false;
            break;
    }
    
    return true;
}
    
bool RegressionTree::computeBestSpiltBestIterativeSpilt( const RegressionData &trainingData, const vector< UINT > &features, UINT &featureIndex, double &threshold, double &minError ){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = (UINT)features.size();
    
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
    VectorDouble groupCounter(2,0);
    VectorDouble groupMean(2,0);
    VectorDouble groupMSE(2,0);
    vector< MinMax > ranges = trainingData.getInputRanges();
    
    //Loop over each feature and try and find the best split point
    for(UINT n=0; n<N; n++){
        minRange = ranges[n].minValue;
        maxRange = ranges[n].maxValue;
        step = (maxRange-minRange)/double(numSplittingSteps);
        threshold = minRange;
        while( threshold <= maxRange ){
            
            //Iterate over each sample and work out what group it falls into
            for(UINT i=0; i<M; i++){
                groupID = trainingData[ i ].getInputVector()[features[n]] >= threshold ? 1 : 0;
                groupIndex[i] = groupID;
                groupMean[ groupID ] += trainingData[ i ].getInputVector()[features[n]];
                groupCounter[ groupID ]++;
            }
            groupMean[0] /= groupCounter[0] > 0 ? groupCounter[0] : 1;
            groupMean[1] /= groupCounter[1] > 0 ? groupCounter[1] : 1;
            
            //Compute the MSE for each group
            for(UINT i=0; i<M; i++){
                groupMSE[ groupIndex[i] ] += SQR( groupMean[ groupIndex[i] ] - trainingData[ i ].getInputVector()[features[n]] );
            }
            groupMSE[0] /= groupCounter[0] > 0 ? groupCounter[0] : 1;
            groupMSE[1] /= groupCounter[1] > 0 ? groupCounter[1] : 1;
            
            error = groupMSE[0] + groupMSE[1];
            
            //Store the best threshold and feature index
            if( error < minError ){
                minError = error;
                bestThreshold = threshold;
                bestFeatureIndex = n;
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
    
    /*
bool RegressionTree::computeBestSpiltBestRandomSpilt( const RegressionData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &threshold, double &minError ){
    
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
    vector< UINT > groupIndex(M);
    VectorDouble groupCounter(2,0);
    vector< MinMax > ranges = trainingData.getRanges();
    
    MatrixDouble classProbabilities(K,2);
    
    //Loop over each feature and try and find the best split point
    for(UINT n=0; n<N; n++){
        for(UINT m=0; m<numSplittingSteps; m++){
            //Randomly choose the threshold
            threshold = random.getRandomNumberUniform(ranges[n].minValue,ranges[n].maxValue);
        
            //Iterate over each sample and work out if it should be in the lhs (0) or rhs (1) group
            groupCounter[0] = groupCounter[1] = 0;
            classProbabilities.setAllValues(0);
            for(UINT i=0; i<M; i++){
                groupIndex[i] = trainingData[ i ][ features[n] ] >= threshold ? 1 : 0;
                groupCounter[ groupIndex[i] ]++;
                classProbabilities[ getClassLabelIndexValue(trainingData[i].getClassLabel()) ][ groupIndex[i] ]++;
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
                bestFeatureIndex = n;
            }
        }
    }
    
    //Set the best feature index and threshold
    featureIndex = bestFeatureIndex;
    threshold = bestThreshold;
    
    return true;
}
    

*/
} //End of namespace GRT

