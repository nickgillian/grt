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
#include "RegressionTree.h"

GRT_BEGIN_NAMESPACE

//Register the RegressionTreeNode with the Node base class
RegisterNode< RegressionTreeNode > RegressionTreeNode::registerModule("RegressionTreeNode");

//Define the string that will be used to identify the object
const std::string RegressionTree::id = "RegressionTree";
std::string RegressionTree::getId() { return RegressionTree::id; }

//Register the RegressionTree module with the Regressifier base class
RegisterRegressifierModule< RegressionTree >  RegressionTree::registerModule( RegressionTree::getId() );

RegressionTree::RegressionTree(const UINT numSplittingSteps,const UINT minNumSamplesPerNode,const UINT maxDepth,const bool removeFeaturesAtEachSpilt,const Tree::TrainingMode trainingMode,const bool useScaling,const Float minRMSErrorPerNode) : Regressifier( RegressionTree::getId() )
{
    tree = NULL;
    this->numSplittingSteps = numSplittingSteps;
    this->minNumSamplesPerNode = minNumSamplesPerNode;
    this->maxDepth = maxDepth;
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    this->trainingMode = trainingMode;
    this->useScaling = useScaling;
    this->minRMSErrorPerNode = minRMSErrorPerNode;
}

RegressionTree::RegressionTree(const RegressionTree &rhs) : Regressifier( RegressionTree::getId() )
{
    tree = NULL;
    *this = rhs;
}

RegressionTree::~RegressionTree(void)
{
    clear();
}

RegressionTree& RegressionTree::operator=(const RegressionTree &rhs){
    if( this != &rhs ){
        //Clear this tree
        this->clear();
        
        if( rhs.getTrained() ){
            //Deep copy the tree
            this->tree = dynamic_cast< RegressionTreeNode* >( rhs.deepCopyTree() );
        }
        
        this->numSplittingSteps = rhs.numSplittingSteps;
        this->minNumSamplesPerNode = rhs.minNumSamplesPerNode;
        this->maxDepth = rhs.maxDepth;
        this->removeFeaturesAtEachSpilt = rhs.removeFeaturesAtEachSpilt;
        this->trainingMode = rhs.trainingMode;
        this->minRMSErrorPerNode = rhs.minRMSErrorPerNode;
        
        //Copy the base variables
        copyBaseVariables( (Regressifier*)&rhs );
    }
    return *this;
}

bool RegressionTree::deepCopyFrom(const Regressifier *regressifier){
    
    if( regressifier == NULL ) return false;
    
    if( this->getId() == regressifier->getId() ){
        
        const RegressionTree *ptr = dynamic_cast<const RegressionTree*>(regressifier);
        
        //Clear this tree
        this->clear();
        
        if( ptr->getTrained() ){
            //Deep copy the tree
            this->tree = dynamic_cast< RegressionTreeNode* >( ptr->deepCopyTree() );
        }
        
        this->numSplittingSteps = ptr->numSplittingSteps;
        this->minNumSamplesPerNode = ptr->minNumSamplesPerNode;
        this->maxDepth = ptr->maxDepth;
        this->removeFeaturesAtEachSpilt = ptr->removeFeaturesAtEachSpilt;
        this->trainingMode = ptr->trainingMode;
        this->minRMSErrorPerNode = ptr->minRMSErrorPerNode;
        
        //Copy the base variables
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
        Regressifier::errorLog << "train_(RegressionData &trainingData) - Training data has zero samples!" << std::endl;
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
    Vector< UINT > features(N);
    for(UINT i=0; i<N; i++){
        features[i] = i;
    }
    
    //Build the tree
    UINT nodeID = 0;
    tree = buildTree( trainingData, NULL, features, nodeID );
    
    if( tree == NULL ){
        clear();
        Regressifier::errorLog << "train_(RegressionData &trainingData) - Failed to build tree!" << std::endl;
        return false;
    }
    
    //Flag that the algorithm has been trained
    trained = true;
    
    return true;
}

bool RegressionTree::predict_(VectorFloat &inputVector){
    
    if( !trained ){
        Regressifier::errorLog << "predict_(VectorFloat &inputVector) - Model Not Trained!" << std::endl;
        return false;
    }
    
    if( tree == NULL ){
        Regressifier::errorLog << "predict_(VectorFloat &inputVector) - Tree pointer is null!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        Regressifier::errorLog << "predict_(VectorFloat &inputVector) - The size of the input Vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], inputVectorRanges[n].minValue, inputVectorRanges[n].maxValue, 0, 1);
        }
    }
    
    if( !tree->predict( inputVector, regressionData ) ){
        Regressifier::errorLog << "predict_(VectorFloat &inputVector) - Failed to predict!" << std::endl;
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

bool RegressionTree::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        Regressifier::errorLog <<"save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "GRT_REGRESSION_TREE_MODEL_FILE_V1.0\n";
    
    //Write the classifier settings to the file
    if( !Regressifier::saveBaseSettingsToFile(file) ){
        Regressifier::errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    file << "NumSplittingSteps: " << numSplittingSteps << std::endl;
    file << "MinNumSamplesPerNode: " << minNumSamplesPerNode << std::endl;
    file << "MaxDepth: " << maxDepth << std::endl;
    file << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSpilt << std::endl;
    file << "TrainingMode: " << trainingMode << std::endl;
    file << "TreeBuilt: " << (tree != NULL ? 1 : 0) << std::endl;
    
    if( tree != NULL ){
        file << "Tree:\n";
        if( !tree->save( file ) ){
            Regressifier::errorLog << "save(fstream &file) - Failed to save tree to file!" << std::endl;
            return false;
        }
    }
    
    return true;
}

bool RegressionTree::load( std::fstream &file ){
    
    clear();
    
    if(!file.is_open())
    {
        Regressifier::errorLog << "load(string filename) - Could not open file to load model" << std::endl;
        return false;
    }
    
    std::string word;
    file >> word;
    
    //Find the file type header
    if(word != "GRT_REGRESSION_TREE_MODEL_FILE_V1.0"){
        Regressifier::errorLog << "load(string filename) - Could not find Model File Header" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Regressifier::loadBaseSettingsFromFile(file) ){
        Regressifier::errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "NumSplittingSteps:"){
        Regressifier::errorLog << "load(string filename) - Could not find the NumSplittingSteps!" << std::endl;
        return false;
    }
    file >> numSplittingSteps;
    
    file >> word;
    if(word != "MinNumSamplesPerNode:"){
        Regressifier::errorLog << "load(string filename) - Could not find the MinNumSamplesPerNode!" << std::endl;
        return false;
    }
    file >> minNumSamplesPerNode;
    
    file >> word;
    if(word != "MaxDepth:"){
        Regressifier::errorLog << "load(string filename) - Could not find the MaxDepth!" << std::endl;
        return false;
    }
    file >> maxDepth;
    
    file >> word;
    if(word != "RemoveFeaturesAtEachSpilt:"){
        Regressifier::errorLog << "load(string filename) - Could not find the RemoveFeaturesAtEachSpilt!" << std::endl;
        return false;
    }
    file >> removeFeaturesAtEachSpilt;
    
    file >> word;
    if(word != "TrainingMode:"){
        Regressifier::errorLog << "load(string filename) - Could not find the TrainingMode!" << std::endl;
        return false;
    }
    UINT tempTrainingMode;
    file >> tempTrainingMode;
    trainingMode = static_cast< Tree::TrainingMode >( tempTrainingMode );
    
    file >> word;
    if(word != "TreeBuilt:"){
        Regressifier::errorLog << "load(string filename) - Could not find the TreeBuilt!" << std::endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        file >> word;
        if(word != "Tree:"){
            Regressifier::errorLog << "load(string filename) - Could not find the Tree!" << std::endl;
            return false;
        }
        
        //Create a new tree
        tree = new RegressionTreeNode;
        
        if( tree == NULL ){
            clear();
            Regressifier::errorLog << "load(fstream &file) - Failed to create new RegressionTreeNode!" << std::endl;
            return false;
        }
        
        tree->setParent( NULL );
        if( !tree->load( file ) ){
            clear();
            Regressifier::errorLog << "load(fstream &file) - Failed to load tree from file!" << std::endl;
            return false;
        }
    }
    
    return true;
}

RegressionTreeNode* RegressionTree::deepCopyTree() const{
    
    if( tree == NULL ){
        return NULL;
    }
    
    return (RegressionTreeNode*)tree->deepCopyNode();
}

const RegressionTreeNode* RegressionTree::getTree() const{
    return dynamic_cast< RegressionTreeNode* >( tree );
}

Float RegressionTree::getMinRMSErrorPerNode() const{
    return minRMSErrorPerNode;
}

Tree::TrainingMode RegressionTree::getTrainingMode() const{
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

UINT RegressionTree::getPredictedNodeID()const{
    
    if( tree == NULL ){
        return 0;
    }
    
    return tree->getPredictedNodeID();
}

bool RegressionTree::getRemoveFeaturesAtEachSpilt() const{
    return removeFeaturesAtEachSpilt;
}

bool RegressionTree::setTrainingMode(const Tree::TrainingMode trainingMode){ 
    if( trainingMode >= Tree::BEST_ITERATIVE_SPILT && trainingMode < Tree::NUM_TRAINING_MODES ){
        this->trainingMode = trainingMode;
        return true;
    }
    warningLog << "Unknown trainingMode: " << trainingMode << std::endl;
    return false;
}

bool RegressionTree::setNumSplittingSteps(const UINT numSplittingSteps){
    if( numSplittingSteps > 0 ){
        this->numSplittingSteps = numSplittingSteps;
        return true;
    }
    warningLog << "setNumSplittingSteps(const UINT numSplittingSteps) - The number of splitting steps must be greater than zero!" << std::endl;
    return false;
}

bool RegressionTree::setMinNumSamplesPerNode(const UINT minNumSamplesPerNode){
    if( minNumSamplesPerNode > 0 ){
        this->minNumSamplesPerNode = minNumSamplesPerNode;
        return true;
    }
    warningLog << "setMinNumSamplesPerNode(const UINT minNumSamplesPerNode) - The minimum number of samples per node must be greater than zero!" << std::endl;
    return false;
}

bool RegressionTree::setMaxDepth(const UINT maxDepth){
    if( maxDepth > 0 ){
        this->maxDepth = maxDepth;
        return true;
    }
    warningLog << "setMaxDepth(const UINT maxDepth) - The maximum depth must be greater than zero!" << std::endl;
    return false;
}

bool RegressionTree::setRemoveFeaturesAtEachSpilt(const bool removeFeaturesAtEachSpilt){
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    return true;
}

bool RegressionTree::setMinRMSErrorPerNode(const Float minRMSErrorPerNode){
    this->minRMSErrorPerNode = minRMSErrorPerNode;
    return true;
}

RegressionTreeNode* RegressionTree::buildTree(const RegressionData &trainingData,RegressionTreeNode *parent,Vector< UINT > features,UINT nodeID){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = trainingData.getNumInputDimensions();
    const UINT T = trainingData.getNumTargetDimensions();
    VectorFloat regressionData(T);
    
    //Update the nodeID
    
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
    node->initNode( parent, depth, nodeID );
    
    //If there are no features left then create a leaf node and return
    if( features.size() == 0 || M < minNumSamplesPerNode || depth >= maxDepth ){
        
        //Flag that this is a leaf node
        node->setIsLeafNode( true );
        
        //Compute the regression data that will be stored at this node
        computeNodeRegressionData( trainingData, regressionData );
        
        //Set the node
        node->set( trainingData.getNumSamples(), 0, 0, regressionData );
        
        Regressifier::trainingLog << "Reached leaf node. Depth: " << depth << " NumSamples: " << trainingData.getNumSamples() << std::endl;
        
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
    
    trainingLog << "Depth: " << depth << " FeatureIndex: " << featureIndex << " Threshold: " << threshold << " MinError: " << minError << std::endl;
    
    //If the minError is below the minRMSError then create a leaf node and return
    if( minError <= minRMSErrorPerNode ){
        //Compute the regression data that will be stored at this node
        computeNodeRegressionData( trainingData, regressionData );
        
        //Set the node
        node->set( trainingData.getNumSamples(), featureIndex, threshold, regressionData );
        
        trainingLog << "Reached leaf node. Depth: " << depth << " NumSamples: " << M << std::endl;
        
        return node;
    }
    
    //Set the node
    node->set( trainingData.getNumSamples(), featureIndex, threshold, regressionData );
    
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
    RegressionData lhs(N,T);
    RegressionData rhs(N,T);
    
    for(UINT i=0; i<M; i++){
        if( node->predict( trainingData[i].getInputVector() ) ){
            rhs.addSample(trainingData[i].getInputVector(), trainingData[i].getTargetVector());
        }else lhs.addSample(trainingData[i].getInputVector(), trainingData[i].getTargetVector());
    }
    
    //Run the recursive tree building on the children
    node->setLeftChild( buildTree( lhs, node, features, nodeID ) );
    node->setRightChild( buildTree( rhs, node, features, nodeID ) );
    
    return node;
}

bool RegressionTree::computeBestSpilt( const RegressionData &trainingData, const Vector< UINT > &features, UINT &featureIndex, Float &threshold, Float &minError ){
    
    switch( trainingMode ){
        case Tree::BEST_ITERATIVE_SPILT:
        return computeBestSpiltBestIterativeSpilt( trainingData, features, featureIndex, threshold, minError );
        break;
        case Tree::BEST_RANDOM_SPLIT:
        //return computeBestSpiltBestRandomSpilt( trainingData, features, featureIndex, threshold, minError );
        break;
        default:
        Regressifier::errorLog << "Uknown trainingMode!" << std::endl;
        return false;
        break;
    }
    
    return false;
}

bool RegressionTree::computeBestSpiltBestIterativeSpilt( const RegressionData &trainingData, const Vector< UINT > &features, UINT &featureIndex, Float &threshold, Float &minError ){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = (UINT)features.size();
    
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
    VectorFloat groupCounter(2,0);
    VectorFloat groupMean(2,0);
    VectorFloat groupMSE(2,0);
    Vector< MinMax > ranges = trainingData.getInputRanges();
    
    //Loop over each feature and try and find the best split point
    for(UINT n=0; n<N; n++){
        minRange = ranges[n].minValue;
        maxRange = ranges[n].maxValue;
        step = (maxRange-minRange)/Float(numSplittingSteps);
        threshold = minRange;
        featureIndex = features[n];
        while( threshold <= maxRange ){
            
            //Iterate over each sample and work out what group it falls into
            for(UINT i=0; i<M; i++){
                groupID = trainingData[i].getInputVector()[featureIndex] >= threshold ? 1 : 0;
                groupIndex[i] = groupID;
                groupMean[ groupID ] += trainingData[i].getInputVector()[featureIndex];
                groupCounter[ groupID ]++;
            }
            groupMean[0] /= groupCounter[0] > 0 ? groupCounter[0] : 1;
            groupMean[1] /= groupCounter[1] > 0 ? groupCounter[1] : 1;
            
            //Compute the MSE for each group
            for(UINT i=0; i<M; i++){
                groupMSE[ groupIndex[i] ] += grt_sqr( groupMean[ groupIndex[i] ] - trainingData[ i ].getInputVector()[features[n]] );
            }
            groupMSE[0] /= groupCounter[0] > 0 ? groupCounter[0] : 1;
            groupMSE[1] /= groupCounter[1] > 0 ? groupCounter[1] : 1;
            
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

/*
bool RegressionTree::computeBestSpiltBestRandomSpilt( const RegressionData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &threshold, Float &minError ){

const UINT M = trainingData.getNumSamples();
const UINT N = (UINT)features.size();
const UINT K = (UINT)classLabels.size();

if( N == 0 ) return false;

minError = numeric_limits<Float>::max();
UINT bestFeatureIndex = 0;
Float bestThreshold = 0;
Float error = 0;
Float giniIndexL = 0;
Float giniIndexR = 0;
Float weightL = 0;
Float weightR = 0;
Vector< UINT > groupIndex(M);
VectorFloat groupCounter(2,0);
Vector< MinMax > ranges = trainingData.getRanges();

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

//Compute the regression data that will be stored at this node
bool RegressionTree::computeNodeRegressionData( const RegressionData &trainingData, VectorFloat &regressionData ){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = trainingData.getNumInputDimensions();
    const UINT T = trainingData.getNumTargetDimensions();
    
    if( M == 0 ){
        Regressifier::errorLog << "computeNodeRegressionData(...) - Failed to compute regression data, there are zero training samples!" << std::endl;
        return false;
    }
    
    //Make sure the regression data is the correct size
    regressionData.clear();
    regressionData.resize( T, 0 );
    
    //The regression data at this node is simply an average over all the training data at this node
    for(unsigned int j=0; j<N; j++){
        for(unsigned int i=0; i<M; i++){
            regressionData[j] += trainingData[i].getTargetVector()[j];
        }
        regressionData[j] /= M;
    }
    
    return true;
}

GRT_END_NAMESPACE
