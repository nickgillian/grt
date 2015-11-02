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

#include "RandomForests.h"

namespace GRT{

//Register the RandomForests module with the Classifier base class
RegisterClassifierModule< RandomForests >  RandomForests::registerModule("RandomForests");

RandomForests::RandomForests(const DecisionTreeNode &decisionTreeNode,const UINT forestSize,const UINT numRandomSplits,const UINT minNumSamplesPerNode,const UINT maxDepth,const UINT trainingMode,const bool removeFeaturesAtEachSpilt,const bool useScaling,const double bootstrappedDatasetWeight)
{
    this->decisionTreeNode = decisionTreeNode.deepCopy();
    this->forestSize = forestSize;
    this->numRandomSplits = numRandomSplits;
    this->minNumSamplesPerNode = minNumSamplesPerNode;
    this->maxDepth = maxDepth;
    this->trainingMode = trainingMode;
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    this->useScaling = useScaling;
    this->bootstrappedDatasetWeight = bootstrappedDatasetWeight;
    classType = "RandomForests";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    useNullRejection = false;
    supportsNullRejection = false;
    debugLog.setProceedingText("[DEBUG RandomForests]");
    errorLog.setProceedingText("[ERROR RandomForests]");
    trainingLog.setProceedingText("[TRAINING RandomForests]");
    warningLog.setProceedingText("[WARNING RandomForests]");
}
    
RandomForests::RandomForests(const RandomForests &rhs){
    this->decisionTreeNode = NULL;
    classType = "RandomForests";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG RandomForests]");
    errorLog.setProceedingText("[ERROR RandomForests]");
    trainingLog.setProceedingText("[TRAINING RandomForests]");
    warningLog.setProceedingText("[WARNING RandomForests]");
    *this = rhs;
}

RandomForests::~RandomForests(void)
{
    clear();
    
    if( decisionTreeNode != NULL ){
        delete decisionTreeNode;
        decisionTreeNode = NULL;
    }
}
    
RandomForests& RandomForests::operator=(const RandomForests &rhs){
    if( this != &rhs ){
        //Clear this tree
        clear();
        
        //Copy the base classifier variables
        if( copyBaseVariables( (Classifier*)&rhs ) ){
            
            //Deep copy the main node
            if( this->decisionTreeNode != NULL ){
                delete decisionTreeNode;
                decisionTreeNode = NULL;
            }
            this->decisionTreeNode = rhs.deepCopyDecisionTreeNode();
            
            if( rhs.getTrained() ){
                //Deep copy the forest
                for(UINT i=0; i<rhs.forest.size(); i++){
                    this->forest.push_back( rhs.forest[i]->deepCopy() );
                }
            }
            
            this->forestSize = rhs.forestSize;
            this->numRandomSplits = rhs.numRandomSplits;
            this->minNumSamplesPerNode = rhs.minNumSamplesPerNode;
            this->maxDepth = rhs.maxDepth;
            this->removeFeaturesAtEachSpilt = rhs.removeFeaturesAtEachSpilt;
            this->bootstrappedDatasetWeight = rhs.bootstrappedDatasetWeight;
            this->trainingMode = rhs.trainingMode;
            
        }else errorLog << "deepCopyFrom(const Classifier *classifier) - Failed to copy base variables!" << endl;
	}
	return *this;
}

bool RandomForests::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        
        RandomForests *ptr = (RandomForests*)classifier;
        
        //Clear this tree
        this->clear();
        
        if( copyBaseVariables( classifier ) ){
            
            //Deep copy the main node
            if( this->decisionTreeNode != NULL ){
                delete decisionTreeNode;
                decisionTreeNode = NULL;
            }
            this->decisionTreeNode = ptr->deepCopyDecisionTreeNode();
            
            if( ptr->getTrained() ){
                //Deep copy the forest
                this->forest.reserve( ptr->forest.size() );
                for(size_t i=0; i<ptr->forest.size(); i++){
                    this->forest.push_back( ptr->forest[i]->deepCopy() );
                }
            }
            
            this->forestSize = ptr->forestSize;
            this->numRandomSplits = ptr->numRandomSplits;
            this->minNumSamplesPerNode = ptr->minNumSamplesPerNode;
            this->maxDepth = ptr->maxDepth;
            this->removeFeaturesAtEachSpilt = ptr->removeFeaturesAtEachSpilt;
            this->bootstrappedDatasetWeight = ptr->bootstrappedDatasetWeight;
            this->trainingMode = ptr->trainingMode;
            
            return true;
        }
        
        errorLog << "deepCopyFrom(const Classifier *classifier) - Failed to copy base variables!" << endl;
    }
    return false;
}

bool RandomForests::train_(ClassificationData &trainingData){
    
    //Clear any previous model
    clear();
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int K = trainingData.getNumClasses();
    
    if( M == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << endl;
        return false;
    }

    if( bootstrappedDatasetWeight <= 0.0 || bootstrappedDatasetWeight > 1.0 ){
        errorLog << "train_(ClassificationData &trainingData) - Bootstrapped Dataset Weight must be [> 0.0 and <= 1.0]" << endl;
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
    
    //Flag that the main algorithm has been trained encase we need to trigger any callbacks
    trained = true;
    
    //Train the random forest
    forest.reserve( forestSize );
    for(UINT i=0; i<forestSize; i++){
        
        //Get a balanced bootstrapped dataset
        UINT datasetSize = (UINT)(trainingData.getNumSamples() * bootstrappedDatasetWeight);
        ClassificationData data = trainingData.getBootstrappedDataset( datasetSize, true );
 
        DecisionTree tree;
        tree.setDecisionTreeNode( *decisionTreeNode );
        tree.enableScaling( false ); //We have already scaled the training data so we do not need to scale it again
        tree.setTrainingMode( trainingMode );
        tree.setNumSplittingSteps( numRandomSplits );
        tree.setMinNumSamplesPerNode( minNumSamplesPerNode );
        tree.setMaxDepth( maxDepth );
        tree.enableNullRejection( useNullRejection );
        tree.setRemoveFeaturesAtEachSpilt( removeFeaturesAtEachSpilt );

        trainingLog << "Training forest " << i+1 << "/" << forestSize << "..." << endl;
        
        //Train this tree
        if( !tree.train( data ) ){
            errorLog << "train_(ClassificationData &labelledTrainingData) - Failed to train tree at forest index: " << i << endl;
            clear();
            return false;
        }
        
        //Deep copy the tree into the forest
        forest.push_back( tree.deepCopyTree() );
    }

    return true;
}

bool RandomForests::predict_(VectorDouble &inputVector){
    
    predictedClassLabel = 0;
    maxLikelihood = 0;
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - Model Not Trained!" << endl;
        return false;
    }
    
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
    
    std::fill(classDistances.begin(),classDistances.end(),0);
    
    //Run the prediction for each tree in the forest
    VectorDouble y;
    for(UINT i=0; i<forestSize; i++){
        if( !forest[i]->predict(inputVector, y) ){
            errorLog << "predict_(VectorDouble &inputVector) - Tree " << i << " failed prediction!" << endl;
            return false;
        }
        
        for(UINT j=0; j<numClasses; j++){
            classDistances[j] += y[j];
        }
    }
    
    //Use the class distances to estimate the class likelihoods
    bestDistance = 0;
    UINT bestIndex = 0;
    double classNorm = 1.0 / double(forestSize);
    for(UINT k=0; k<numClasses; k++){
        classLikelihoods[k] = classDistances[k] * classNorm;
        
        if( classLikelihoods[k] > maxLikelihood ){
            maxLikelihood = classLikelihoods[k];
            bestDistance = classDistances[k];
            bestIndex = k;
        }
    }
    
    predictedClassLabel = classLabels[ bestIndex ];
    
    return true;
}
    
bool RandomForests::clear(){
    
    //Call the classifiers clear function
    Classifier::clear();
    
    //Delete the forest
    for(size_t i=0; i<forest.size(); i++){
        if( forest[i] != NULL ){
            forest[i]->clear();
            delete forest[i];
            forest[i] = NULL;
        }
    }
    forest.clear();
    
    return true;
}

bool RandomForests::print() const{
    
    cout << "RandomForest\n";
    cout << "ForestSize: " << forestSize << endl;
    cout << "NumSplittingSteps: " << numRandomSplits << endl;
    cout << "MinNumSamplesPerNode: " << minNumSamplesPerNode << endl;
    cout << "MaxDepth: " << maxDepth << endl;
    cout << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSpilt << endl;
    cout << "TrainingMode: " << trainingMode << endl;
    cout << "ForestBuilt: " << (trained ? 1 : 0) << endl;
    
    if( trained ){
        cout << "Forest:\n";
        for(UINT i=0; i<forestSize; i++){
            cout << "Tree: " << i+1 << endl;
            forest[i]->print();
        }
    }
    
    return true;
}

bool RandomForests::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file << "GRT_RANDOM_FOREST_MODEL_FILE_V1.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    if( decisionTreeNode != NULL ){
        file << "DecisionTreeNodeType: " << decisionTreeNode->getNodeType() << endl;
        if( !decisionTreeNode->saveToFile( file ) ){
            Classifier::errorLog <<"saveModelToFile(fstream &file) - Failed to save decisionTreeNode settings to file!" << endl;
            return false;
        }
    }else{
        file << "DecisionTreeNodeType: " << "NULL" << endl;
    }
    
    file << "ForestSize: " << forestSize << endl;
    file << "NumSplittingSteps: " << numRandomSplits << endl;
    file << "MinNumSamplesPerNode: " << minNumSamplesPerNode << endl;
    file << "MaxDepth: " << maxDepth << endl;
    file << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSpilt << endl;
    file << "TrainingMode: " << trainingMode << endl;
    file << "ForestBuilt: " << (trained ? 1 : 0) << endl;
    
    if( trained ){
        file << "Forest:\n";
        for(UINT i=0; i<forestSize; i++){
            file << "Tree: " << i+1 << endl;
            file << "TreeNodeType: " << forest[i]->getNodeType() << endl;
            if( !forest[i]->saveToFile( file ) ){
                errorLog << "saveModelToFile(fstream &file) - Failed to save tree " << i << " to file!" << endl;
                return false;
            }
        }
    }
    
    return true;
}
    
bool RandomForests::loadModelFromFile(fstream &file){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << endl;
        return false;
    }
    
    std::string word;
    std::string treeNodeType;
    
    file >> word;
    
    //Find the file type header
    if(word != "GRT_RANDOM_FOREST_MODEL_FILE_V1.0"){
        errorLog << "loadModelFromFile(string filename) - Could not find Model File Header" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if(word != "DecisionTreeNodeType:"){
        Classifier::errorLog << "loadModelFromFile(string filename) - Could not find the DecisionTreeNodeType!" << endl;
        return false;
    }
    file >> treeNodeType;
    
    if( treeNodeType != "NULL" ){
        
        decisionTreeNode = dynamic_cast< DecisionTreeNode* >( DecisionTreeNode::createInstanceFromString( treeNodeType ) );
        
        if( decisionTreeNode == NULL ){
            Classifier::errorLog << "loadModelFromFile(string filename) - Could not create new DecisionTreeNode from type: " << treeNodeType << endl;
            return false;
        }
        
        if( !decisionTreeNode->loadFromFile( file ) ){
            Classifier::errorLog <<"loadModelFromFile(fstream &file) - Failed to load decisionTreeNode settings from file!" << endl;
            return false;
        }
    }else{
        Classifier::errorLog <<"loadModelFromFile(fstream &file) - Failed to load decisionTreeNode! DecisionTreeNodeType is NULL!" << endl;
        return false;
    }
    
    file >> word;
    if(word != "ForestSize:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the ForestSize!" << endl;
        return false;
    }
    file >> forestSize;
    
    file >> word;
    if(word != "NumSplittingSteps:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the NumSplittingSteps!" << endl;
        return false;
    }
    file >> numRandomSplits;
    
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
    if(word != "ForestBuilt:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the ForestBuilt!" << endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        //Find the forest header
        file >> word;
        if(word != "Forest:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the Forest!" << endl;
            return false;
        }
        
        //Load each tree
        UINT treeIndex;
        forest.reserve( forestSize );
        for(UINT i=0; i<forestSize; i++){
            
            file >> word;
            if(word != "Tree:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the Tree Header!" << endl;
                cout << "WORD: " << word << endl;
                cout << "Tree i: " << i << endl;
                return false;
            }
            file >> treeIndex;
            
            if( treeIndex != i+1 ){
                errorLog << "loadModelFromFile(string filename) - Incorrect tree index: " << treeIndex << endl;
                return false;
            }
            
            file >> word;
            if(word != "TreeNodeType:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the TreeNodeType!" << endl;
                cout << "WORD: " << word << endl;
                cout << "i: " << i << endl;
                return false;
            }
            file >> treeNodeType;
            
            //Create a new DTree
            DecisionTreeNode *tree = dynamic_cast< DecisionTreeNode* >( DecisionTreeNode::createInstanceFromString( treeNodeType ) );
            
            if( tree == NULL ){
                errorLog << "loadModelFromFile(fstream &file) - Failed to create new Tree!" << endl;
                return false;
            }
            
            //Load the tree from the file
            tree->setParent( NULL );
            if( !tree->loadFromFile( file ) ){
                errorLog << "loadModelFromFile(fstream &file) - Failed to load tree from file!" << endl;
                return false;
            }
            
            //Add the tree to the forest
            forest.push_back( tree );
        }
    }
    
    return true;
}

bool RandomForests::combineModels( const RandomForests &forest ){

    if( !getTrained() ){
        errorLog << "combineModels( const RandomForests &forest ) - This instance has not been trained!" << endl;
        return false;
    }

    if( !forest.getTrained() ){
        errorLog << "combineModels( const RandomForests &forest ) - This external forest instance has not been trained!" << endl;
        return false;
    }

    if( this->getNumInputDimensions() != forest.getNumInputDimensions() ) {
        errorLog << "combineModels( const RandomForests &forest ) - The number of input dimensions of the external forest (";
        errorLog << forest.getNumInputDimensions() << ") does not match the number of input dimensions of this instance (";
        errorLog << this->getNumInputDimensions() << ")!" << endl;
        return false;
    }

    //Add the trees in the other forest to this model
    DecisionTreeNode *node;
    for(UINT i=0; i<forest.getForestSize(); i++){
        node = forest.getTree(i);
        if( node ){
            this->forest.push_back( node->deepCopy() );
            forestSize++;
        }
    }

    return true;
}
    
UINT RandomForests::getForestSize()const{
    return forestSize;
}
    
UINT RandomForests::getNumRandomSplits()const{
    return numRandomSplits;
}

UINT RandomForests::getMinNumSamplesPerNode()const{
    return minNumSamplesPerNode;
}

UINT RandomForests::getMaxDepth()const{
    return maxDepth;
}
    
UINT RandomForests::getTrainingMode() const {
    return trainingMode;
}
    
bool RandomForests::getRemoveFeaturesAtEachSpilt() const {
    return removeFeaturesAtEachSpilt;
}

double RandomForests::getBootstrappedDatasetWeight() const {
    return bootstrappedDatasetWeight;
}

const vector< DecisionTreeNode* > RandomForests::getForest() const {
    return forest;
}
    
DecisionTreeNode* RandomForests::deepCopyDecisionTreeNode() const{
    
    if( decisionTreeNode == NULL ){
        return NULL;
    }
    
    return decisionTreeNode->deepCopy();
}

DecisionTreeNode* RandomForests::getTree( const UINT index ) const{

    if( !trained || index >= forestSize ) return NULL;

    return forest[ index ];
}

VectorDouble RandomForests::getFeatureWeights( const bool normWeights ) const{

    if( !trained ) return VectorDouble();

    VectorDouble weights( numInputDimensions, 0 );

    for(UINT i=0; i<forestSize; i++){
        if( !forest[i]->computeFeatureWeights( weights ) ){
            warningLog << "getFeatureWeights( const bool normWeights ) - Failed to compute weights for tree: " << i << endl;
        }
    }

    //Normalize the weights
    if( normWeights ){
        double sum = Util::sum( weights );
        if( sum > 0.0 ){
            const double norm = 1.0 / sum;
            for(UINT j=0; j<numInputDimensions; j++){
                weights[j] *= norm;
            }
        }
    }

    return weights;
}
    
bool RandomForests::setForestSize(const UINT forestSize){
    if( forestSize > 0 ){
        this->forestSize = forestSize;
        clear();
        return true;
    }
    return false;
}
    
bool RandomForests::setNumRandomSplits(const UINT numRandomSplits){
    if( numRandomSplits > 0 ){
        this->numRandomSplits = numRandomSplits;
        return true;
    }
    return false;
}

bool RandomForests::setMinNumSamplesPerNode(const UINT minNumSamplesPerNode){
    if( minNumSamplesPerNode > 0 ){
        this->minNumSamplesPerNode = minNumSamplesPerNode;
        return true;
    }
    return false;
}

bool RandomForests::setMaxDepth(const UINT maxDepth){
    if( maxDepth > 0 ){
        this->maxDepth = maxDepth;
        return true;
    }
    return false;
}
    
bool RandomForests::setRemoveFeaturesAtEachSpilt(const bool removeFeaturesAtEachSpilt){
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    return true;
}
    
bool RandomForests::setTrainingMode(const UINT trainingMode){
    
    if( trainingMode == DecisionTree::BEST_ITERATIVE_SPILT || trainingMode == DecisionTree::BEST_RANDOM_SPLIT ){
        this->trainingMode = trainingMode;
        return true;
    }
    
    warningLog << "setTrainingMode(const UINT mode) - Unknown training mode!" << endl;
    return false;
}
    
bool RandomForests::setDecisionTreeNode( const DecisionTreeNode &node ){
    
    if( decisionTreeNode != NULL ){
        delete decisionTreeNode;
        decisionTreeNode = NULL;
    }
    this->decisionTreeNode = node.deepCopy();
    
    return true;
}

bool RandomForests::setBootstrappedDatasetWeight( const double bootstrappedDatasetWeight ){

    if( bootstrappedDatasetWeight > 0.0 && bootstrappedDatasetWeight <= 1.0 ){
        this->bootstrappedDatasetWeight = bootstrappedDatasetWeight;
        return true;
    }

    warningLog << "setBootstrappedDatasetWeight(...) - Bad parameter, the weight must be > 0.0 and <= 1.0. Weight: " << bootstrappedDatasetWeight << endl;
    return false;
}

} //End of namespace GRT

