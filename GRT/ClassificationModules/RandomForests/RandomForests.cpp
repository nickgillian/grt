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
#include "RandomForests.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string RandomForests::id = "RandomForests";
std::string RandomForests::getId() { return RandomForests::id; }

//Register the RandomForests module with the Classifier base class
RegisterClassifierModule< RandomForests >  RandomForests::registerModule( RandomForests::getId() );

RandomForests::RandomForests(const DecisionTreeNode &decisionTreeNode,const UINT forestSize,const UINT numRandomSplits,const UINT minNumSamplesPerNode,const UINT maxDepth,const Tree::TrainingMode trainingMode,const bool removeFeaturesAtEachSpilt,const bool useScaling,const Float bootstrappedDatasetWeight) : Classifier( RandomForests::getId() )
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
    classifierMode = STANDARD_CLASSIFIER_MODE;
    useNullRejection = false;
    supportsNullRejection = false;
    useValidationSet = true;
    validationSetSize = 20;
}

RandomForests::RandomForests(const RandomForests &rhs) : Classifier( RandomForests::getId() )
{
    this->decisionTreeNode = NULL;
    classifierMode = STANDARD_CLASSIFIER_MODE;
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
            
        }else errorLog << "deepCopyFrom(const Classifier *classifier) - Failed to copy base variables!" << std::endl;
    }
    return *this;
}

bool RandomForests::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getId() == classifier->getId() ){
        
        const RandomForests *ptr = dynamic_cast<const RandomForests*>(classifier);
        
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
                this->forest.reserve( ptr->forest.getSize() );
                for(UINT i=0; i<ptr->forest.getSize(); i++){
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
        
        errorLog << "deepCopyFrom(const Classifier *classifier) - Failed to copy base variables!" << std::endl;
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
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << std::endl;
        return false;
    }
    
    if( bootstrappedDatasetWeight <= 0.0 || bootstrappedDatasetWeight > 1.0 ){
        errorLog << "train_(ClassificationData &trainingData) - Bootstrapped Dataset Weight must be [> 0.0 and <= 1.0]" << std::endl;
        return false;
    }
    
    numInputDimensions = N;
    numOutputDimensions = K;
    numClasses = K;
    classLabels = trainingData.getClassLabels();
    ranges = trainingData.getRanges();
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }
    
    if( useValidationSet ){
        validationSetAccuracy = 0;
        validationSetPrecision.resize( useNullRejection ? K+1 : K, 0 );
        validationSetRecall.resize( useNullRejection ? K+1 : K, 0 );
    }
    
    //Flag that the main algorithm has been trained encase we need to trigger any callbacks
    trained = true;
    
    //Train the random forest
    forest.reserve( forestSize );
    
    for(UINT i=0; i<forestSize; i++){
        
        //Get a balanced bootstrapped dataset
        UINT datasetSize = (UINT)floor(trainingData.getNumSamples() * bootstrappedDatasetWeight);
        ClassificationData data = trainingData.getBootstrappedDataset( datasetSize, true );
        
        Timer timer;
        timer.start();
        
        DecisionTree tree;
        tree.setDecisionTreeNode( *decisionTreeNode );
        tree.enableScaling( false ); //We have already scaled the training data so we do not need to scale it again
        tree.setUseValidationSet( useValidationSet );
        tree.setValidationSetSize( validationSetSize );
        tree.setTrainingMode( trainingMode );
        tree.setNumSplittingSteps( numRandomSplits );
        tree.setMinNumSamplesPerNode( minNumSamplesPerNode );
        tree.setMaxDepth( maxDepth );
        tree.enableNullRejection( useNullRejection );
        tree.setRemoveFeaturesAtEachSpilt( removeFeaturesAtEachSpilt );
        
        trainingLog << "Training decision tree " << i+1 << "/" << forestSize << "..." << std::endl;
        
        //Train this tree
        if( !tree.train_( data ) ){
            errorLog << "train_(ClassificationData &trainingData) - Failed to train tree at forest index: " << i << std::endl;
            clear();
            return false;
        }
        
        Float computeTime = timer.getMilliSeconds();
        trainingLog << "Decision tree trained in " << (computeTime*0.001)/60.0 << " minutes" << std::endl;
        
        if( useValidationSet ){
            Float forestNorm = 1.0 / forestSize;
            validationSetAccuracy += tree.getValidationSetAccuracy();
            VectorFloat precision = tree.getValidationSetPrecision();
            VectorFloat recall = tree.getValidationSetRecall();
            
            grt_assert( precision.getSize() == validationSetPrecision.getSize() );
            grt_assert( recall.getSize() == validationSetRecall.getSize() );
            
            for(UINT i=0; i<validationSetPrecision.getSize(); i++){
                validationSetPrecision[i] += precision[i] * forestNorm;
            }
            
            for(UINT i=0; i<validationSetRecall.getSize(); i++){
                validationSetRecall[i] += recall[i] * forestNorm;
            }
        }
        
        //Deep copy the tree into the forest
        forest.push_back( tree.deepCopyTree() );
    }

    //Flag that the models have been trained
    trained = true;

    //Compute the final training stats
    trainingSetAccuracy = 0;

    //If scaling was on, then the data will already be scaled, so turn it off temporially so we can test the model accuracy
    bool scalingState = useScaling;
    useScaling = false;
    if( !computeAccuracy( trainingData, trainingSetAccuracy ) ){
        trained = false;
        errorLog << "Failed to compute training set accuracy! Failed to fully train model!" << std::endl;
        return false;
    }

    trainingLog << "Training set accuracy: " << trainingSetAccuracy << std::endl;

    //Reset the scaling state for future prediction
    useScaling = scalingState;
    
    if( useValidationSet ){
        validationSetAccuracy /= forestSize;
        trainingLog << "Validation set accuracy: " << validationSetAccuracy << std::endl;
        
        trainingLog << "Validation set precision: ";
        for(UINT i=0; i<validationSetPrecision.getSize(); i++){
            trainingLog << validationSetPrecision[i] << " ";
        }
        trainingLog << std::endl;
        
        trainingLog << "Validation set recall: ";
        for(UINT i=0; i<validationSetRecall.getSize(); i++){
            trainingLog << validationSetRecall[i] << " ";
        }
        trainingLog << std::endl;
    }
    
    return true;
}

bool RandomForests::predict_(VectorDouble &inputVector){
    
    predictedClassLabel = 0;
    maxLikelihood = 0;
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - Model Not Trained!" << std::endl;
        return false;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &inputVector) - The size of the input Vector (" << inputVector.getSize() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = grt_scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0.0, 1.0);
        }
    }
    
    if( classLikelihoods.getSize() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.getSize() != numClasses ) classDistances.resize(numClasses,0);
    
    std::fill(classDistances.begin(),classDistances.end(),0);
    
    //Run the prediction for each tree in the forest
    VectorDouble y;
    for(UINT i=0; i<forestSize; i++){
        if( !forest[i]->predict(inputVector, y) ){
            errorLog << "predict_(VectorDouble &inputVector) - Tree " << i << " failed prediction!" << std::endl;
            return false;
        }
        
        for(UINT j=0; j<numClasses; j++){
            classDistances[j] += y[j];
        }
    }
    
    //Use the class distances to estimate the class likelihoods
    bestDistance = 0;
    UINT bestIndex = 0;
    Float classNorm = 1.0 / Float(forestSize);
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
    for(UINT i=0; i<forest.getSize(); i++){
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
    
    std::cout << "RandomForest\n";
    std::cout << "ForestSize: " << forestSize << std::endl;
    std::cout << "NumSplittingSteps: " << numRandomSplits << std::endl;
    std::cout << "MinNumSamplesPerNode: " << minNumSamplesPerNode << std::endl;
    std::cout << "MaxDepth: " << maxDepth << std::endl;
    std::cout << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSpilt << std::endl;
    std::cout << "TrainingMode: " << trainingMode << std::endl;
    std::cout << "ForestBuilt: " << (trained ? 1 : 0) << std::endl;
    
    if( trained ){
        std::cout << "Forest:\n";
        for(UINT i=0; i<forestSize; i++){
            std::cout << "Tree: " << i+1 << std::endl;
            forest[i]->print();
        }
    }
    
    return true;
}

bool RandomForests::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "GRT_RANDOM_FOREST_MODEL_FILE_V1.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    if( decisionTreeNode != NULL ){
        file << "DecisionTreeNodeType: " << decisionTreeNode->getNodeType() << std::endl;
        if( !decisionTreeNode->save( file ) ){
            Classifier::errorLog <<"save(fstream &file) - Failed to save decisionTreeNode settings to file!" << std::endl;
            return false;
        }
    }else{
        file << "DecisionTreeNodeType: " << "NULL" << std::endl;
    }
    
    file << "ForestSize: " << forestSize << std::endl;
    file << "NumSplittingSteps: " << numRandomSplits << std::endl;
    file << "MinNumSamplesPerNode: " << minNumSamplesPerNode << std::endl;
    file << "MaxDepth: " << maxDepth << std::endl;
    file << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSpilt << std::endl;
    file << "TrainingMode: " << trainingMode << std::endl;
    file << "ForestBuilt: " << (trained ? 1 : 0) << std::endl;
    
    if( trained ){
        file << "Forest:\n";
        for(UINT i=0; i<forestSize; i++){
            file << "Tree: " << i+1 << std::endl;
            file << "TreeNodeType: " << forest[i]->getNodeType() << std::endl;
            if( !forest[i]->save( file ) ){
                errorLog << "save(fstream &file) - Failed to save tree " << i << " to file!" << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

bool RandomForests::load( std::fstream &file ){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "load(string filename) - Could not open file to load model" << std::endl;
        return false;
    }
    
    std::string word;
    std::string treeNodeType;
    
    file >> word;
    
    //Find the file type header
    if(word != "GRT_RANDOM_FOREST_MODEL_FILE_V1.0"){
        errorLog << "load(string filename) - Could not find Model File Header" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "DecisionTreeNodeType:"){
        Classifier::errorLog << "load(string filename) - Could not find the DecisionTreeNodeType!" << std::endl;
        return false;
    }
    file >> treeNodeType;
    
    if( treeNodeType != "NULL" ){
        
        decisionTreeNode = dynamic_cast< DecisionTreeNode* >( DecisionTreeNode::createInstanceFromString( treeNodeType ) );
        
        if( decisionTreeNode == NULL ){
            Classifier::errorLog << "load(string filename) - Could not create new DecisionTreeNode from type: " << treeNodeType << std::endl;
            return false;
        }
        
        if( !decisionTreeNode->load( file ) ){
            Classifier::errorLog <<"load(fstream &file) - Failed to load decisionTreeNode settings from file!" << std::endl;
            return false;
        }
    }else{
        Classifier::errorLog <<"load(fstream &file) - Failed to load decisionTreeNode! DecisionTreeNodeType is NULL!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "ForestSize:"){
        errorLog << "load(string filename) - Could not find the ForestSize!" << std::endl;
        return false;
    }
    file >> forestSize;
    
    file >> word;
    if(word != "NumSplittingSteps:"){
        errorLog << "load(string filename) - Could not find the NumSplittingSteps!" << std::endl;
        return false;
    }
    file >> numRandomSplits;
    
    file >> word;
    if(word != "MinNumSamplesPerNode:"){
        errorLog << "load(string filename) - Could not find the MinNumSamplesPerNode!" << std::endl;
        return false;
    }
    file >> minNumSamplesPerNode;
    
    file >> word;
    if(word != "MaxDepth:"){
        errorLog << "load(string filename) - Could not find the MaxDepth!" << std::endl;
        return false;
    }
    file >> maxDepth;
    
    file >> word;
    if(word != "RemoveFeaturesAtEachSpilt:"){
        errorLog << "load(string filename) - Could not find the RemoveFeaturesAtEachSpilt!" << std::endl;
        return false;
    }
    file >> removeFeaturesAtEachSpilt;
    
    file >> word;
    if(word != "TrainingMode:"){
        errorLog << "load(string filename) - Could not find the TrainingMode!" << std::endl;
        return false;
    }
    UINT trainingModeTmp;
    file >> trainingModeTmp;
    trainingMode = (Tree::TrainingMode)trainingModeTmp;
    
    file >> word;
    if(word != "ForestBuilt:"){
        errorLog << "load(string filename) - Could not find the ForestBuilt!" << std::endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        //Find the forest header
        file >> word;
        if(word != "Forest:"){
            errorLog << "load(string filename) - Could not find the Forest!" << std::endl;
            return false;
        }
        
        //Load each tree
        UINT treeIndex;
        forest.reserve( forestSize );
        for(UINT i=0; i<forestSize; i++){
            
            file >> word;
            if(word != "Tree:"){
                errorLog << "load(string filename) - Could not find the Tree Header!" << std::endl;
                std::cout << "WORD: " << word << std::endl;
                std::cout << "Tree i: " << i << std::endl;
                return false;
            }
            file >> treeIndex;
            
            if( treeIndex != i+1 ){
                errorLog << "load(string filename) - Incorrect tree index: " << treeIndex << std::endl;
                return false;
            }
            
            file >> word;
            if(word != "TreeNodeType:"){
                errorLog << "load(string filename) - Could not find the TreeNodeType!" << std::endl;
                std::cout << "WORD: " << word << std::endl;
                std::cout << "i: " << i << std::endl;
                return false;
            }
            file >> treeNodeType;
            
            //Create a new DTree
            DecisionTreeNode *tree = dynamic_cast< DecisionTreeNode* >( DecisionTreeNode::createInstanceFromString( treeNodeType ) );
            
            if( tree == NULL ){
                errorLog << "load(fstream &file) - Failed to create new Tree!" << std::endl;
                return false;
            }
            
            //Load the tree from the file
            tree->setParent( NULL );
            if( !tree->load( file ) ){
                errorLog << "load(fstream &file) - Failed to load tree from file!" << std::endl;
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
        errorLog << "combineModels( const RandomForests &forest ) - This instance has not been trained!" << std::endl;
        return false;
    }
    
    if( !forest.getTrained() ){
        errorLog << "combineModels( const RandomForests &forest ) - This external forest instance has not been trained!" << std::endl;
        return false;
    }
    
    if( this->getNumInputDimensions() != forest.getNumInputDimensions() ) {
        errorLog << "combineModels( const RandomForests &forest ) - The number of input dimensions of the external forest (";
        errorLog << forest.getNumInputDimensions() << ") does not match the number of input dimensions of this instance (";
        errorLog << this->getNumInputDimensions() << ")!" << std::endl;
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

Float RandomForests::getBootstrappedDatasetWeight() const {
    return bootstrappedDatasetWeight;
}

const Vector< DecisionTreeNode* >& RandomForests::getForest() const {
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
            warningLog << "getFeatureWeights( const bool normWeights ) - Failed to compute weights for tree: " << i << std::endl;
            }
    }
    
    //Normalize the weights
    if( normWeights  ){
        Float sum = Util::sum( weights );
        if( sum > 0.0 ){
            const Float norm = 1.0 / sum;
            for(UINT j=0; j<numInputDimensions; j++){
                weights[j] *= norm;
            }
        }
    }
    
    return weights;
}

MatrixDouble RandomForests::getLeafNodeFeatureWeights( const bool normWeights ) const{
    
    if( !trained ) return MatrixDouble();
    
    MatrixDouble weights( getNumClasses(), numInputDimensions );
    weights.setAllValues(0.0);
    
    for(UINT i=0; i<forestSize; i++){
        if( !forest[i]->computeLeafNodeWeights( weights ) ){
            warningLog << "computeLeafNodeWeights( const bool normWeights ) - Failed to compute leaf node weights for tree: " << i << std::endl;
            }
    }
    
    //Normalize the weights
    if( normWeights  ){
        for(UINT j=0; j<weights.getNumCols(); j++){
            Float sum = 0.0;
            for(UINT i=0; i<weights.getNumRows(); i++){
                sum += weights[i][j];
            }
            if( sum != 0.0 ){
                const Float norm = 1.0 / sum;
                for(UINT i=0; i<weights.getNumRows(); i++){
                    weights[i][j] *= norm;
                }
            }
        }
    }
    
    return weights;
}

bool RandomForests::setForestSize(const UINT forestSize){
    if( forestSize > 0 ){
        clear();
        this->forestSize = forestSize;
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

bool RandomForests::setTrainingMode(const Tree::TrainingMode trainingMode){
    if( trainingMode == Tree::BEST_ITERATIVE_SPILT || trainingMode == Tree::BEST_RANDOM_SPLIT ){
        this->trainingMode = trainingMode;
        return true;
    }
    warningLog << "setTrainingMode(const Tree::TrainingMode mode) - Unknown training mode!" << std::endl;
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

bool RandomForests::setBootstrappedDatasetWeight( const Float bootstrappedDatasetWeight ){
    
    if( bootstrappedDatasetWeight > 0.0 && bootstrappedDatasetWeight <= 1.0 ){
        this->bootstrappedDatasetWeight = bootstrappedDatasetWeight;
        return true;
    }
    
    warningLog << "setBootstrappedDatasetWeight(...) - Bad parameter, the weight must be > 0.0 and <= 1.0. Weight: " << bootstrappedDatasetWeight << std::endl;
    return false;
}

GRT_END_NAMESPACE
