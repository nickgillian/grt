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

#include "DecisionTree.h"

namespace GRT{

//Register the DecisionTree module with the Classifier base class
RegisterClassifierModule< DecisionTree >  DecisionTree::registerModule("DecisionTree");
    
RegisterNode< DecisionTreeNode > DecisionTreeNode::registerModule("DecisionTreeNode");

DecisionTree::DecisionTree(const UINT numSplittingSteps,const UINT minNumSamplesPerNode,const UINT maxDepth,const bool removeFeaturesAtEachSpilt,const UINT trainingMode,const bool useScaling)
{
    decisionTree = NULL;
    this->numSplittingSteps = numSplittingSteps;
    this->minNumSamplesPerNode = minNumSamplesPerNode;
    this->maxDepth = maxDepth;
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    this->trainingMode = trainingMode;
    this->useScaling = useScaling;
    classType = "DecisionTree";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG DecisionTree]");
    errorLog.setProceedingText("[ERROR DecisionTree]");
    trainingLog.setProceedingText("[TRAINING DecisionTree]");
    warningLog.setProceedingText("[WARNING DecisionTree]");
    
}
    
DecisionTree::DecisionTree(const DecisionTree &rhs){
    decisionTree = NULL;
    classType = "DecisionTree";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG DecisionTree]");
    errorLog.setProceedingText("[ERROR DecisionTree]");
    trainingLog.setProceedingText("[TRAINING DecisionTree]");
    warningLog.setProceedingText("[WARNING DecisionTree]");
    *this = rhs;
}

DecisionTree::~DecisionTree(void)
{
    clear();
}
    
DecisionTree& DecisionTree::operator=(const DecisionTree &rhs){
	if( this != &rhs ){
        //Clear this tree
        clear();
        
        if( rhs.getTrained() ){
            //Deep copy the decision tree
            decisionTree = (DecisionTreeNode*)rhs.deepCopyTree();
        }
        
        this->numSplittingSteps = rhs.numSplittingSteps;
        this->minNumSamplesPerNode = rhs.minNumSamplesPerNode;
        this->maxDepth = rhs.maxDepth;
        this->removeFeaturesAtEachSpilt = rhs.removeFeaturesAtEachSpilt;
        this->trainingMode = rhs.trainingMode;

        //Copy the base classifier variables
        copyBaseVariables( (Classifier*)&rhs );
	}
	return *this;
}

bool DecisionTree::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        
        DecisionTree *ptr = (DecisionTree*)classifier;
        
        //Clear this tree
        this->clear();
        
        if( ptr->getTrained() ){
            //Deep copy the decision tree
            this->decisionTree = ptr->deepCopyTree();
        }
        
        this->numSplittingSteps = ptr->numSplittingSteps;
        this->minNumSamplesPerNode = ptr->minNumSamplesPerNode;
        this->maxDepth = ptr->maxDepth;
        this->removeFeaturesAtEachSpilt = ptr->removeFeaturesAtEachSpilt;
        this->trainingMode = ptr->trainingMode;
        
        //Copy the base classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool DecisionTree::train_(ClassificationData &trainingData){
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int K = trainingData.getNumClasses();
    trained = false;
    classLabels.clear();
    clear();
    
    if( M == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << endl;
        return false;
    }
    
    numInputDimensions = N;
    numClasses = K;
    classLabels = trainingData.getClassLabels();
    ranges = trainingData.getRanges();
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }
    
    vector< UINT > features(N);
    for(UINT i=0; i<N; i++){
        features[i] = i;
    }
    
    //Build the tree
    decisionTree = buildTree( trainingData, NULL, features, classLabels );
    
    if( decisionTree == NULL ){
        clear();
        errorLog << "train_(ClassificationData &trainingData) - Failed to build tree!" << endl;
        return false;
    }
    
    //Flag that the algorithm has been trained
    trained = true;

    return trained;
}

bool DecisionTree::predict_(VectorDouble &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - Model Not Trained!" << endl;
        return false;
    }
    
    if( decisionTree == NULL ){
        errorLog << "predict_(VectorDouble &inputVector) - DecisionTree pointer is null!" << endl;
        return false;
    }
    
    predictedClassLabel = 0;
	maxLikelihood = -10000;
    
    if( !trained ) return false;
    
	if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    if( !decisionTree->predict( inputVector, classLikelihoods ) ){
        errorLog << "predict_(VectorDouble &inputVector) - Failed to predict!" << endl;
        return false;
    }
    
    UINT K = (UINT)classLikelihoods.size();
    UINT maxIndex = 0;
    maxLikelihood = 0;
    for(UINT k=0; k<K; k++){
        if( classLikelihoods[k] > maxLikelihood ){
            maxLikelihood = classLikelihoods[k];
            maxIndex = k;
        }
    }
    
    predictedClassLabel = classLabels[ maxIndex ];
    
    return true;
}
    
bool DecisionTree::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    if( decisionTree != NULL ){
        decisionTree->clear();
        delete decisionTree;
        decisionTree = NULL;
    }
    
    return true;
}

bool DecisionTree::print() const{
    if( decisionTree != NULL )
        return decisionTree->print();
    return false;
}
    
bool DecisionTree::saveModelToFile(string filename) const{

    if( !trained ) return false;
    
	std::fstream file; 
	file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }

	file.close();

	return true;
}
    
bool DecisionTree::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file << "GRT_DECISION_TREE_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    file << "NumSplittingSteps: " << numSplittingSteps << endl;
    file << "MinNumSamplesPerNode: " << minNumSamplesPerNode << endl;
    file << "MaxDepth: " << maxDepth << endl;
    file << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSpilt << endl;
    file << "TrainingMode: " << trainingMode << endl;
    file << "TreeBuilt: " << (decisionTree != NULL ? 1 : 0) << endl;
    
    if( decisionTree != NULL ){
        file << "Tree:\n";
        if( !decisionTree->saveToFile( file ) ){
            errorLog << "saveModelToFile(fstream &file) - Failed to save tree to file!" << endl;
            return false;
        }
    }
    
    return true;
}

bool DecisionTree::loadModelFromFile(string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    file.close();

	return true;

}
    
bool DecisionTree::loadModelFromFile(fstream &file){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << endl;
        return false;
    }
    
    std::string word;
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_DECISION_TREE_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Find the file type header
    if(word != "GRT_DECISION_TREE_MODEL_FILE_V2.0"){
        errorLog << "loadModelFromFile(string filename) - Could not find Model File Header" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
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
    
        //Create a new DTree
        decisionTree = new DecisionTreeNode;
        
        if( decisionTree == NULL ){
            clear();
            errorLog << "loadModelFromFile(fstream &file) - Failed to create new DecisionTreeNode!" << endl;
            return false;
        }
        
        decisionTree->setParent( NULL );
        if( !decisionTree->loadFromFile( file ) ){
            clear();
            errorLog << "loadModelFromFile(fstream &file) - Failed to load tree from file!" << endl;
            return false;
        }
        
        //Recompute the null rejection thresholds
        recomputeNullRejectionThresholds();
        
        //Resize the prediction results to make sure it is setup for realtime prediction
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}
    
DecisionTreeNode* DecisionTree::deepCopyTree() const{
    
    if( decisionTree == NULL ){
        return NULL;
    }

    return decisionTree->deepCopyTree();
}

const DecisionTreeNode* DecisionTree::getTree() const{
    return decisionTree;
}
    
UINT DecisionTree::getTrainingMode() const{
    return trainingMode;
}
    
UINT DecisionTree::getNumSplittingSteps()const{
    return numSplittingSteps;
}

UINT DecisionTree::getMinNumSamplesPerNode()const{
    return minNumSamplesPerNode;
}

UINT DecisionTree::getMaxDepth()const{
    return maxDepth;
}
    
bool DecisionTree::getRemoveFeaturesAtEachSpilt() const{
    return removeFeaturesAtEachSpilt;
}
    
bool DecisionTree::setTrainingMode(const UINT trainingMode){
    if( trainingMode >= BEST_ITERATIVE_SPILT && trainingMode < NUM_TRAINING_MODES ){
        this->trainingMode = trainingMode;
        return true;
    }
    warningLog << "Unknown trainingMode: " << trainingMode << endl;
    return false;
}
    
bool DecisionTree::setNumSplittingSteps(const UINT numSplittingSteps){
    if( numSplittingSteps > 0 ){
        this->numSplittingSteps = numSplittingSteps;
        return true;
    }
    return false;
}

bool DecisionTree::setMinNumSamplesPerNode(const UINT minNumSamplesPerNode){
    if( minNumSamplesPerNode > 0 ){
        this->minNumSamplesPerNode = minNumSamplesPerNode;
        return true;
    }
    return false;
}

bool DecisionTree::setMaxDepth(const UINT maxDepth){
    if( maxDepth > 0 ){
        this->maxDepth = maxDepth;
        return true;
    }
    return false;
}
    
bool DecisionTree::setRemoveFeaturesAtEachSpilt(const bool removeFeaturesAtEachSpilt){
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    return true;
}
    
DecisionTreeNode* DecisionTree::buildTree(const ClassificationData &trainingData,DecisionTreeNode *parent,vector< UINT > features,const vector< UINT > &classLabels){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = trainingData.getNumDimensions();
    
    //Get the depth
    UINT depth = 0;
    
    if( parent != NULL )
        depth = parent->getDepth() + 1;
    
    //If there are no training data then return NULL
    if( trainingData.getNumSamples() == 0 )
        return NULL;
    
    //Create the new node
    DecisionTreeNode *node = new DecisionTreeNode;
    
    if( node == NULL )
        return NULL;
    
    //Set the parent
    node->initNode( parent, depth );
    
    //If all the training data belongs to the same class or there are no features left then create a leaf node and return
    if( trainingData.getNumClasses() == 1 || features.size() == 0 || M < minNumSamplesPerNode || depth >= maxDepth ){
        
        //Flag that this is a leaf node
        node->setIsLeafNode( true );
        
        //Set the node
        node->set( trainingData.getNumSamples(), 0, 0, getClassProbabilities( trainingData, classLabels ) );
        
        trainingLog << "Reached leaf node. Depth: " << depth << " NumSamples: " << trainingData.getNumSamples() << endl;
        
        return node;
    }
    
    //Compute the best spilt point
    UINT featureIndex = 0;
    double threshold = 0;
    double minError = 0;
    if( !computeBestSpilt( trainingData, features, classLabels, featureIndex, threshold, minError ) ){
        delete node;
        return NULL;
    }
    
    trainingLog << "Depth: " << depth << " FeatureIndex: " << featureIndex << " Threshold: " << threshold << " MinError: " << minError << endl;
    
    //Set the node
    node->set( trainingData.getNumSamples(), features[ featureIndex ], threshold, getClassProbabilities( trainingData, classLabels ) );
    
    //Remove the selected feature so we will not use it again
    if( removeFeaturesAtEachSpilt ){
        features.erase( features.begin()+featureIndex );
    }
    
    //Split the data
    ClassificationData lhs(N);
    ClassificationData rhs(N);
    
    for(UINT i=0; i<M; i++){
        if( node->predict( trainingData[i].getSample() ) ){
            rhs.addSample(trainingData[i].getClassLabel(), trainingData[i].getSample());
        }else lhs.addSample(trainingData[i].getClassLabel(), trainingData[i].getSample());
    }
    
    //Run the recursive tree building on the children
    node->setLeftChild( buildTree( lhs, node, features, classLabels ) );
    node->setRightChild( buildTree( rhs, node, features, classLabels ) );
    
    return node;
}
    
bool DecisionTree::computeBestSpilt( const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &threshold, double &minError ){
    
    switch( trainingMode ){
        case BEST_ITERATIVE_SPILT:
            return computeBestSpiltBestIterativeSpilt( trainingData, features, classLabels, featureIndex, threshold, minError );
            break;
        case BEST_RANDOM_SPLIT:
            return computeBestSpiltBestRandomSpilt( trainingData, features, classLabels, featureIndex, threshold, minError );
            break;
        default:
            errorLog << "Uknown trainingMode!" << endl;
            return false;
            break;
    }
    
    return true;
}
    
bool DecisionTree::computeBestSpiltBestIterativeSpilt( const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &threshold, double &minError ){
    
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
        while( threshold <= maxRange ){
            
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
            
            //Update the threshold
            threshold += step;
        }
    }
    
    //Set the best feature index and threshold
    featureIndex = bestFeatureIndex;
    threshold = bestThreshold;
    
    return true;
}
    
bool DecisionTree::computeBestSpiltBestRandomSpilt( const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &threshold, double &minError ){
    
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
    
VectorDouble DecisionTree::getClassProbabilities( const ClassificationData &trainingData, const vector< UINT > &classLabels ){
    const UINT K = (UINT)classLabels.size();
    const UINT N = (UINT)trainingData.getNumClasses();
    const double M = (double)trainingData.getNumSamples();
    VectorDouble x(K,0);
    for(UINT k=0; k<K; k++){
        for(UINT n=0; n<N; n++){
            if( classLabels[k] == trainingData.getClassTracker()[n].classLabel ){
                x[k] = trainingData.getClassTracker()[n].counter / M;
                break;
            }
        }
    }
    return x;
}
    
bool DecisionTree::loadLegacyModelFromFile( fstream &file ){
    
    string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadModelFromFile(string filename) - Could not find NumFeatures!" << endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << "loadModelFromFile(string filename) - Could not find NumClasses!" << endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadModelFromFile(string filename) - Could not find UseScaling!" << endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << "loadModelFromFile(string filename) - Could not find UseNullRejection!" << endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize( numInputDimensions );
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the Ranges!" << endl;
            return false;
        }
        for(UINT n=0; n<ranges.size(); n++){
            file >> ranges[n].minValue;
            file >> ranges[n].maxValue;
        }
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
        
        //Create a new DTree
        decisionTree = new DecisionTreeNode;
        
        if( decisionTree == NULL ){
            clear();
            errorLog << "loadModelFromFile(fstream &file) - Failed to create new DecisionTreeNode!" << endl;
            return false;
        }
        
        decisionTree->setParent( NULL );
        if( !decisionTree->loadFromFile( file ) ){
            clear();
            errorLog << "loadModelFromFile(fstream &file) - Failed to load tree from file!" << endl;
            return false;
        }
    }
    
    return true;
}

} //End of namespace GRT

