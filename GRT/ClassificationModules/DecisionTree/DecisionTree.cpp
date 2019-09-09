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
#include "DecisionTree.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string DecisionTree::id = "DecisionTree";
std::string DecisionTree::getId() { return DecisionTree::id; }

//Register the DecisionTree module with the Classifier base class
RegisterClassifierModule< DecisionTree >  DecisionTree::registerModule( DecisionTree::getId() );

DecisionTree::DecisionTree(const DecisionTreeNode &decisionTreeNode,const UINT minNumSamplesPerNode,const UINT maxDepth,const bool removeFeaturesAtEachSplit,const Tree::TrainingMode trainingMode,const UINT numSplittingSteps,const bool useScaling) : Classifier( DecisionTree::getId() )
{
    this->tree = NULL;
    this->decisionTreeNode = dynamic_cast< DecisionTreeNode* >( decisionTreeNode.deepCopy() );
    this->minNumSamplesPerNode = minNumSamplesPerNode;
    this->maxDepth = maxDepth;
    this->removeFeaturesAtEachSplit = removeFeaturesAtEachSplit;
    this->trainingMode = trainingMode;
    this->numSplittingSteps = numSplittingSteps;
    this->useScaling = useScaling;
    this->supportsNullRejection = true;
	this->currentID = 0;
    this->numTrainingIterationsToConverge = 20; //Retrain the model 20 times and pick the best one
    classifierMode = STANDARD_CLASSIFIER_MODE;
}

DecisionTree::DecisionTree(const DecisionTree &rhs) : Classifier( DecisionTree::getId() )
{
    tree = NULL;
    decisionTreeNode = NULL;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    *this = rhs;
}

DecisionTree::~DecisionTree(void)
{
    clear();
    
    if( decisionTreeNode != NULL ){
        delete decisionTreeNode;
        decisionTreeNode = NULL;
    }
}

DecisionTree& DecisionTree::operator=(const DecisionTree &rhs){
    if( this != &rhs ){
        //Clear this tree
        clear();
        
        if( rhs.getTrained() ){
            //Deep copy the tree
            this->tree = (DecisionTreeNode*)rhs.deepCopyTree();
        }
        
        //Deep copy the main node
        if( this->decisionTreeNode != NULL ){
            delete decisionTreeNode;
            decisionTreeNode = NULL;
        }
        this->decisionTreeNode = rhs.deepCopyDecisionTreeNode();
        
        this->minNumSamplesPerNode = rhs.minNumSamplesPerNode;
        this->maxDepth = rhs.maxDepth;
        this->removeFeaturesAtEachSplit = rhs.removeFeaturesAtEachSplit;
        this->trainingMode = rhs.trainingMode;
        this->numSplittingSteps = rhs.numSplittingSteps;
        this->nodeClusters = rhs.nodeClusters;
        
        //Copy the base classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}

bool DecisionTree::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        
        const DecisionTree *ptr = dynamic_cast<const DecisionTree*>( classifier );
        
        //Clear this tree
        this->clear();
        
        if( ptr->getTrained() ){
            //Deep copy the tree
            this->tree = ptr->deepCopyTree();
        }
        
        //Deep copy the main node
        if( this->decisionTreeNode != NULL ){
            delete decisionTreeNode;
            decisionTreeNode = NULL;
        }
        this->decisionTreeNode = ptr->deepCopyDecisionTreeNode();
        
        this->minNumSamplesPerNode = ptr->minNumSamplesPerNode;
        this->maxDepth = ptr->maxDepth;
        this->removeFeaturesAtEachSplit = ptr->removeFeaturesAtEachSplit;
        this->trainingMode = ptr->trainingMode;
        this->numSplittingSteps = ptr->numSplittingSteps;
        this->nodeClusters = ptr->nodeClusters;
        
        //Copy the base classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool DecisionTree::train_(ClassificationData &trainingData){
    
    //Clear any previous model
    clear();
    
    if( decisionTreeNode == NULL ){
        errorLog << __GRT_LOG__ << " The decision tree node has not been set! You must set this first before training a model." << std::endl;
        return false;
    }
    
    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int K = trainingData.getNumClasses();
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << __GRT_LOG__ << " Training data has zero samples!" << std::endl;
        return false;
    }
    
    numInputDimensions = N;
    numOutputDimensions = K;
    numClasses = K;
    classLabels = trainingData.getClassLabels();
    ranges = trainingData.getRanges();
    
    //Get the validation set if needed
    ClassificationData validationData;
    if( useValidationSet ){
        validationData = trainingData.split( validationSetSize );
        validationSetAccuracy = 0;
        validationSetPrecision.resize( useNullRejection ? K+1 : K, 0 );
        validationSetRecall.resize( useNullRejection ? K+1 : K, 0 );
    }

    const unsigned int M = trainingData.getNumSamples();
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }
    
    //If we are using null rejection, then we need a copy of the training dataset for later
    ClassificationData trainingDataCopy;
    if( useNullRejection ){
        trainingDataCopy = trainingData;
    }
    
    //Setup the valid features - at this point all features can be used
    Vector< UINT > features(N);
    for(UINT i=0; i<N; i++){
        features[i] = i;
    }

    numTrainingIterationsToConverge = 1;
    trainingLog << "numTrainingIterationsToConverge " << numTrainingIterationsToConverge << " useValidationSet: " << useValidationSet << std::endl;

    if( useValidationSet ){

        //If we get here, then we are going to train the tree several times and pick the best model
        DecisionTreeNode *bestTree = NULL;
        Float bestValidationSetAccuracy = 0;
        UINT bestTreeIndex = 0;
        for(UINT i=0; i<numTrainingIterationsToConverge; i++){

            trainingLog << "Training tree iteration: " << i+1 << "/" << numTrainingIterationsToConverge << std::endl;

            if( !trainTree( trainingData, trainingDataCopy, validationData, features ) ){
                errorLog << __GRT_LOG__ << " Failed to build tree!" << std::endl;
                //Delete the best tree if it exists
                if( bestTree != NULL ){
                    delete bestTree;
                    bestTree = NULL;
                }
                return false;
            }

            //Keep track of the best tree
            if( bestTree == NULL ){
                //Grab the tree pointer
                bestTree = tree;
                bestValidationSetAccuracy = validationSetAccuracy;
                bestTreeIndex = i;
            }else{
                if( validationSetAccuracy > bestValidationSetAccuracy ){
                    //Grab the tree pointer
                    bestTree = tree;
                    bestValidationSetAccuracy = validationSetAccuracy;
                    bestTreeIndex = i;
                }else{
                    //If we get here then the current tree is not the best so far, so free it
                    if( tree != NULL ){
                        delete tree;
                        tree = NULL;
                    }
                }
            }
        }

        //Use the best tree
        trainingLog << "Best tree index: " << bestTreeIndex+1 << " validation set accuracy: " << bestValidationSetAccuracy << std::endl;
        
        if( bestTree != tree ){
            //Delete the tree if it exists
            if( tree != NULL ){
                delete tree;
                tree = NULL;
            }

            //Swap the pointers
            tree = bestTree;
        }

    }else{
        //If we get here, then we are going to train the tree once
        if( !trainTree( trainingData, trainingDataCopy, validationData, features ) ){
            return false;
        }
    }

    //If we get this far, then a model has been trained
    converged = true;

    //Setup the data for prediction
    predictedClassLabel = 0;
    maxLikelihood = 0;
    classLikelihoods.resize(numClasses);
    classDistances.resize(numClasses);

    //Compute the final training stats
    trainingSetAccuracy = 0;
    validationSetAccuracy = 0;

    //If scaling was on, then the data will already be scaled, so turn it off temporially
    bool scalingState = useScaling;
    useScaling = false;
    for(UINT i=0; i<M; i++){
        if( !predict_( trainingData[i].getSample() ) ){
            trained = false;
            errorLog << __GRT_LOG__ << " Failed to run prediction for training sample: " << i << "! Failed to fully train model!" << std::endl;
            return false;
        }

        if( predictedClassLabel == trainingData[i].getClassLabel() ){
            trainingSetAccuracy++;
        }
    }

    if( useValidationSet ){
        for(UINT i=0; i<validationData.getNumSamples(); i++){
            if( !predict_( validationData[i].getSample() ) ){
                trained = false;
                errorLog << __GRT_LOG__ << " Failed to run prediction for validation sample: " << i << "! Failed to fully train model!" << std::endl;
                return false;
            }

            if( predictedClassLabel == validationData[i].getClassLabel() ){
                validationSetAccuracy++;
            }
        }
    }

    trainingSetAccuracy = trainingSetAccuracy / M * 100.0;

    trainingLog << "Training set accuracy: " << trainingSetAccuracy << std::endl;

    if( useValidationSet ){
        validationSetAccuracy = validationSetAccuracy / validationData.getNumSamples() * 100.0;
        trainingLog << "Validation set accuracy: " << validationSetAccuracy << std::endl;
    }

    //Reset the scaling state for future prediction
    useScaling = scalingState;

    return true;
}

bool DecisionTree::trainTree( ClassificationData trainingData, const ClassificationData &trainingDataCopy, const ClassificationData &validationData, Vector< UINT > features ){

    //Note, this function is only called internally by the decision tree, users should call train_ instead.

    const unsigned int M = trainingData.getNumSamples();

    //Build the tree
    tree = buildTree( trainingData, NULL, features, classLabels );


    if( tree == NULL ){
        clear();
        errorLog << __GRT_LOG__ << " Failed to build tree!" << std::endl;
        return false;
    }

    //Flag that the algorithm has been trained
    trained = true;
    
    //Compute the null rejection thresholds if null rejection is enabled
    if( useNullRejection ){
        VectorFloat classLikelihoods( numClasses );
        Vector< UINT > predictions(M);
        VectorFloat distances(M);
        VectorFloat classCounter( numClasses, 0 );
        
        //Run over the training dataset and compute the distance between each training sample and the predicted node cluster
        VectorFloat sample;
        for(UINT i=0; i<M; i++){
            //Run the prediction for this sample
            sample = trainingDataCopy[i].getSample();
            if( !tree->predict_( sample, classLikelihoods ) ){
                errorLog << __GRT_LOG__ << " Failed to predict training sample while building null rejection model!" << std::endl;
                return false;
            }
            
            //Store the predicted class index and cluster distance
            predictions[i] = Util::getMaxIndex( classLikelihoods );
            distances[i] = getNodeDistance(sample, tree->getPredictedNodeID() );
            
            classCounter[ predictions[i] ]++;
        }
        
        //Compute the average distance for each class between the training data and the node clusters
        classClusterMean.clear();
        classClusterStdDev.clear();
        classClusterMean.resize( numClasses, 0 );
        classClusterStdDev.resize( numClasses, 0.01 ); //we start the std dev with a small value to ensure it is not zero
        
        for(UINT i=0; i<M; i++){
            classClusterMean[ predictions[i] ] += distances[ i ];
        }
        for(UINT k=0; k<numClasses; k++){
            classClusterMean[k] /= grt_max( classCounter[k], 1 );
        }
        
        //Compute the std deviation
        for(UINT i=0; i<M; i++){
            classClusterStdDev[ predictions[i] ] += MLBase::SQR( distances[ i ] - classClusterMean[ predictions[i] ] );
        }
        for(UINT k=0; k<numClasses; k++){
            classClusterStdDev[k] = sqrt( classClusterStdDev[k] / grt_max( classCounter[k], 1 ) );
        }
        
        //Compute the null rejection thresholds using the class mean and std dev
        recomputeNullRejectionThresholds();
    }
    
    if( useValidationSet ){
        const UINT numTestSamples = validationData.getNumSamples();
        double numCorrect = 0;
        UINT testLabel = 0;
        VectorFloat testSample;
        VectorFloat validationSetPrecisionCounter( validationSetPrecision.size(), 0.0 );
        VectorFloat validationSetRecallCounter( validationSetRecall.size(), 0.0 );
        trainingLog << "Testing model with validation set..." << std::endl;
        for(UINT i=0; i<numTestSamples; i++){
            testLabel = validationData[i].getClassLabel();
            testSample = validationData[i].getSample();
            predict_( testSample );
            if( predictedClassLabel == testLabel ){
                numCorrect++;
                validationSetPrecision[ getClassLabelIndexValue( testLabel ) ]++;
                validationSetRecall[ getClassLabelIndexValue( testLabel ) ]++;
            }
            validationSetPrecisionCounter[ getClassLabelIndexValue( predictedClassLabel ) ]++;
            validationSetRecallCounter[ getClassLabelIndexValue( testLabel ) ]++;
        }
        
        validationSetAccuracy = (numCorrect / numTestSamples) * 100.0;
        for(UINT i=0; i<validationSetPrecision.getSize(); i++){
            validationSetPrecision[i] /= validationSetPrecisionCounter[i] > 0 ? validationSetPrecisionCounter[i] : 1;
        }
        for(UINT i=0; i<validationSetRecall.getSize(); i++){
            validationSetRecall[i] /= validationSetRecallCounter[i] > 0 ? validationSetRecallCounter[i] : 1;
        }
        
        Classifier::trainingLog << "Validation set accuracy: " << validationSetAccuracy << std::endl;
        
        Classifier::trainingLog << "Validation set precision: ";
        for(UINT i=0; i<validationSetPrecision.getSize(); i++){
            Classifier::trainingLog << validationSetPrecision[i] << " ";
        }
        Classifier::trainingLog << std::endl;
        
        Classifier::trainingLog << "Validation set recall: ";
        for(UINT i=0; i<validationSetRecall.getSize(); i++){
            Classifier::trainingLog << validationSetRecall[i] << " ";
        }
        Classifier::trainingLog << std::endl;
    }
    
    return true;
}

bool DecisionTree::predict_(VectorFloat &inputVector){
    
    predictedClassLabel = 0;
    maxLikelihood = 0;
    
    //Validate the input is OK and the model is trained properly
    if( !trained ){
        errorLog << __GRT_LOG__ << " Model Not Trained!" << std::endl;
        return false;
    }
    
    if( tree == NULL ){
        errorLog << __GRT_LOG__ << " DecisionTree pointer is null!" << std::endl;
        return false;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << __GRT_LOG__ << " The size of the input Vector (" << inputVector.getSize() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    //Scale the input data if needed
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = grt_scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0.0, 1.0);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    //Run the decision tree prediction
    if( !tree->predict_( inputVector, classLikelihoods ) ){
        errorLog << __GRT_LOG__ << " Failed to predict!" << std::endl;
        return false;
    }
    
    //Find the maximum likelihood
    //The tree automatically returns proper class likelihoods so we don't need to do anything else
    UINT maxIndex = 0;
    maxLikelihood = 0;
    for(UINT k=0; k<numClasses; k++){
        if( classLikelihoods[k] > maxLikelihood ){
            maxLikelihood = classLikelihoods[k];
            maxIndex = k;
        }
    }
    
    //Run the null rejection
    if( useNullRejection ){
        
        //Get the distance between the input and the leaf mean
        Float leafDistance = getNodeDistance( inputVector, tree->getPredictedNodeID() );
        
        if( grt_isnan(leafDistance) ){
            errorLog << __GRT_LOG__ << " Failed to match leaf node ID to compute node distance!" << std::endl;
            return false;
        }
        
        //Set the predicted class distance as the leaf distance, all other classes will have a distance of zero
        classDistances.setAll(0.0);
        classDistances[ maxIndex ] = leafDistance;
        
        //Use the distance to check if the class label should be rejected or not
        if( leafDistance <= nullRejectionThresholds[ maxIndex ] ){
            predictedClassLabel = classLabels[ maxIndex ];
        }else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
        
    }else {
        //Set the predicated class label
        predictedClassLabel = classLabels[ maxIndex ];
    }
    
    return true;
}

bool DecisionTree::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the node clusters
    nodeClusters.clear();
    
    //Delete the tree if it exists
    if( tree != NULL ){
        tree->clear();
        delete tree;
        tree = NULL;
    }
    
    //NOTE: We do not want to clean up the decisionTreeNode here as we need to keep track of this, this is only delete in the destructor
    
    return true;
}

bool DecisionTree::recomputeNullRejectionThresholds(){
    
    if( !trained ){
        Classifier::warningLog << __GRT_LOG__ << " Failed to recompute null rejection thresholds, the model has not been trained!" << std::endl;
        return false;
    }
    
    if( !useNullRejection ){
        Classifier::warningLog << __GRT_LOG__ << " Failed to recompute null rejection thresholds, null rejection is not enabled!" << std::endl;
        return false;
    }
    
    nullRejectionThresholds.resize( numClasses );
    
    //Compute the rejection threshold for each class using the mean and std dev
    for(UINT k=0; k<numClasses; k++){
        nullRejectionThresholds[k] = classClusterMean[k] + (classClusterStdDev[k]*nullRejectionCoeff);
    }
    
    return true;
}

bool DecisionTree::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog << __GRT_LOG__ << " The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "GRT_DECISION_TREE_MODEL_FILE_V4.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog << __GRT_LOG__ << " Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    if( decisionTreeNode != NULL ){
        file << "DecisionTreeNodeType: " << decisionTreeNode->getNodeType() << std::endl;
        if( !decisionTreeNode->save( file ) ){
            errorLog << __GRT_LOG__ << " Failed to save decisionTreeNode settings to file!" << std::endl;
            return false;
        }
    }else{
        file << "DecisionTreeNodeType: " << "NULL" << std::endl;
    }
    
    file << "MinNumSamplesPerNode: " << minNumSamplesPerNode << std::endl;
    file << "MaxDepth: " << maxDepth << std::endl;
    file << "RemoveFeaturesAtEachSpilt: " << removeFeaturesAtEachSplit << std::endl;
    file << "TrainingMode: " << trainingMode << std::endl;
    file << "NumSplittingSteps: " << numSplittingSteps << std::endl;
    file << "TreeBuilt: " << (tree != NULL ? 1 : 0) << std::endl;
    
    if( tree != NULL ){
        file << "Tree:\n";
        if( !tree->save( file ) ){
            errorLog << __GRT_LOG__ << " Failed to save tree to file!" << std::endl;
            return false;
        }
        
        //Save the null rejection data if needed
        if( useNullRejection ){
            
            file << "ClassClusterMean:";
            for(UINT k=0; k<numClasses; k++){
                file << " " << classClusterMean[k];
            }
            file << std::endl;
            
            file << "ClassClusterStdDev:";
            for(UINT k=0; k<numClasses; k++){
                file << " " << classClusterStdDev[k];
            }
            file << std::endl;
            
            file << "NumNodes: " << nodeClusters.size() << std::endl;
            file << "NodeClusters:\n";
            
            std::map< UINT, VectorFloat >::const_iterator iter = nodeClusters.begin();
            
            while( iter != nodeClusters.end() ){
                
                //Write the nodeID
                file << iter->first;
                
                //Write the node cluster
                for(UINT j=0; j<numInputDimensions; j++){
                    file << " " << iter->second[j];
                }
                file << std::endl;
                
                iter++;
            }
        }
        
    }
    
    return true;
}

bool DecisionTree::load( std::fstream &file ){
    
    clear();

    UINT tempTrainingMode = 0;
    
    if( decisionTreeNode != NULL ){
        delete decisionTreeNode;
        decisionTreeNode = NULL;
    }
    
    if( !file.is_open() )
    {
        errorLog << __GRT_LOG__ << " Could not open file to load model" << std::endl;
        return false;
    }
    
    std::string word;
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_DECISION_TREE_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile_v1( file );
    }
    
    if( word == "GRT_DECISION_TREE_MODEL_FILE_V2.0" ){
        return loadLegacyModelFromFile_v2( file );
    }
    
    if( word == "GRT_DECISION_TREE_MODEL_FILE_V3.0" ){
        return loadLegacyModelFromFile_v3( file );
    }
    
    //Find the file type header
    if( word != "GRT_DECISION_TREE_MODEL_FILE_V4.0" ){
        errorLog << __GRT_LOG__ << " Could not find Model File Header" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << __GRT_LOG__ << " Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "DecisionTreeNodeType:"){
        errorLog << __GRT_LOG__ << " Could not find the DecisionTreeNodeType!" << std::endl;
        return false;
    }
    file >> word;
    
    if( word != "NULL" ){
        
        decisionTreeNode = dynamic_cast< DecisionTreeNode* >( DecisionTreeNode::createInstanceFromString( word ) );
        
        if( decisionTreeNode == NULL ){
            errorLog << __GRT_LOG__ << " Could not create new DecisionTreeNode from type: " << word << std::endl;
            return false;
        }
        
        if( !decisionTreeNode->load( file ) ){
            errorLog << __GRT_LOG__ << " Failed to load decisionTreeNode settings from file!" << std::endl;
            return false;
        }
    }else{
        errorLog << __GRT_LOG__ << " Failed to load decisionTreeNode! DecisionTreeNodeType is NULL!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "MinNumSamplesPerNode:"){
        errorLog << __GRT_LOG__ << " Could not find the MinNumSamplesPerNode!" << std::endl;
        return false;
    }
    file >> minNumSamplesPerNode;
    
    file >> word;
    if(word != "MaxDepth:"){
        errorLog << __GRT_LOG__ << " Could not find the MaxDepth!" << std::endl;
        return false;
    }
    file >> maxDepth;
    
    file >> word;
    if(word != "RemoveFeaturesAtEachSpilt:"){
        errorLog << __GRT_LOG__ << " Could not find the RemoveFeaturesAtEachSpilt!" << std::endl;
        return false;
    }
    file >> removeFeaturesAtEachSplit;
    
    file >> word;
    if(word != "TrainingMode:"){
        errorLog << __GRT_LOG__ << " Could not find the TrainingMode!" << std::endl;
        return false;
    }
    file >> tempTrainingMode;
    trainingMode = static_cast<Tree::TrainingMode>(tempTrainingMode);
    
    file >> word;
    if(word != "NumSplittingSteps:"){
        errorLog << __GRT_LOG__ << " Could not find the NumSplittingSteps!" << std::endl;
        return false;
    }
    file >> numSplittingSteps;
    
    file >> word;
    if(word != "TreeBuilt:"){
        errorLog << __GRT_LOG__ << " Could not find the TreeBuilt!" << std::endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        file >> word;
        if(word != "Tree:"){
            errorLog << __GRT_LOG__ << " Could not find the Tree!" << std::endl;
            return false;
        }
        
        //Create a new DTree
        tree = dynamic_cast< DecisionTreeNode* >( decisionTreeNode->createNewInstance() );
        
        if( tree == NULL ){
            clear();
            errorLog << __GRT_LOG__ << " Failed to create new DecisionTreeNode!" << std::endl;
            return false;
        }
        
        tree->setParent( NULL );
        if( !tree->load( file ) ){
            clear();
            errorLog << __GRT_LOG__ << " Failed to load tree from file!" << std::endl;
            return false;
        }
        
        //Load the null rejection data if needed
        if( useNullRejection ){
            
            UINT numNodes = 0;
            classClusterMean.resize( numClasses );
            classClusterStdDev.resize( numClasses );
            
            file >> word;
            if(word != "ClassClusterMean:"){
                errorLog << __GRT_LOG__ << " Could not find the ClassClusterMean header!" << std::endl;
                return false;
            }
            for(UINT k=0; k<numClasses; k++){
                file >> classClusterMean[k];
            }
            
            file >> word;
            if(word != "ClassClusterStdDev:"){
                errorLog << __GRT_LOG__ << " Could not find the ClassClusterStdDev header!" << std::endl;
                return false;
            }
            for(UINT k=0; k<numClasses; k++){
                file >> classClusterStdDev[k];
            }
            
            file >> word;
            if(word != "NumNodes:"){
                errorLog << __GRT_LOG__ << " Could not find the NumNodes header!" << std::endl;
                return false;
            }
            file >> numNodes;
            
            file >> word;
            if(word != "NodeClusters:"){
                errorLog << __GRT_LOG__ << " Could not find the NodeClusters header!" << std::endl;
                return false;
            }
            
            UINT nodeID = 0;
            VectorFloat cluster( numInputDimensions );
            for(UINT i=0; i<numNodes; i++){
                
                //load the nodeID
                file >> nodeID;
                
                for(UINT j=0; j<numInputDimensions; j++){
                    file >> cluster[j];
                }
                
                //Add the cluster to the cluster nodes map
                nodeClusters[ nodeID ] = cluster;
            }
            
            //Recompute the null rejection thresholds
            recomputeNullRejectionThresholds();
        }
        
        //Resize the prediction results to make sure it is setup for realtime prediction
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}

bool DecisionTree::getModel( std::ostream &stream ) const{
    
    if( tree != NULL )
    return tree->getModel( stream );
    return false;
    
}

DecisionTreeNode* DecisionTree::deepCopyTree() const{
    
    if( tree == NULL ){
        return NULL;
    }

    return dynamic_cast< DecisionTreeNode* >( tree->deepCopy() );
}

DecisionTreeNode* DecisionTree::deepCopyDecisionTreeNode() const{
    
    if( decisionTreeNode == NULL ){
        return NULL;
    }
    
    return dynamic_cast< DecisionTreeNode* >(decisionTreeNode->deepCopy());
}

const DecisionTreeNode* DecisionTree::getTree() const{
    return tree;
}

bool DecisionTree::setDecisionTreeNode( const DecisionTreeNode &node ){
    
    if( decisionTreeNode != NULL ){
        decisionTreeNode->clear();
        delete decisionTreeNode;
        decisionTreeNode = NULL;
    }
    this->decisionTreeNode = dynamic_cast< DecisionTreeNode* >(node.deepCopy());
    
    return true;
}

DecisionTreeNode* DecisionTree::buildTree(ClassificationData &trainingData,DecisionTreeNode *parent,Vector< UINT > features,const Vector< UINT > &classLabels ){
    
    const UINT M = trainingData.getNumSamples();
    const UINT N = trainingData.getNumDimensions();
    
    //Update the nodeID
	this->currentID++;

    //Get the depth
    UINT depth = 0;
    
    if( parent != NULL )
    depth = parent->getDepth() + 1;
    
    //If there are no training data then return NULL
    if( trainingData.getNumSamples() == 0 )
    return NULL;
    
    //Create the new node
    DecisionTreeNode *node = dynamic_cast< DecisionTreeNode* >( decisionTreeNode->createNewInstance() );
    
    if( node == NULL )
    return NULL;
    
    //Get the class probabilities
    VectorFloat classProbs = trainingData.getClassProbabilities( classLabels );
    
    //Set the parent
    node->initNode( parent, depth, this->currentID );
    
    //If all the training data belongs to the same class or there are no features left then create a leaf node and return
    if( trainingData.getNumClasses() == 1 || features.size() == 0 || M < minNumSamplesPerNode || depth >= maxDepth ){
        
        //Set the node
        node->setLeafNode( trainingData.getNumSamples(), classProbs );
        
        //Build the null cluster if null rejection is enabled
        if( useNullRejection ){
            nodeClusters[ this->currentID ] = trainingData.getMean();
        }
        
        std::string info = "Reached leaf node.";
        if( trainingData.getNumClasses() == 1 ) info = "Reached pure leaf node.";
        else if( features.size() == 0 ) info = "Reached leaf node, no remaining features.";
        else if( M < minNumSamplesPerNode ) info = "Reached leaf node, hit min-samples-per-node limit.";
        else if( depth >= maxDepth ) info = "Reached leaf node, max depth reached.";
        
        trainingLog << info << " Depth: " << depth << " NumSamples: " << trainingData.getNumSamples();
        
        trainingLog << " Class Probabilities: ";
        for(UINT k=0; k<classProbs.getSize(); k++){
            trainingLog << classProbs[k] << " ";
        }
        trainingLog << std::endl;
        
        return node;
    }
    
    //Compute the best spilt point
    UINT featureIndex = 0;
    Float minError = 0;
    
    if( !node->computeBestSplit( trainingMode, numSplittingSteps, trainingData, features, classLabels, featureIndex, minError ) ){
        delete node;
        return NULL;
    }
    
    trainingLog << "Depth: " << depth << " FeatureIndex: " << featureIndex << " MinError: " << minError;
    trainingLog << " Class Probabilities: ";
    for(UINT k=0; k<classProbs.getSize(); k++){
        trainingLog << classProbs[k] << " ";
    }
    trainingLog << std::endl;
    
    //Remove the selected feature so we will not use it again
    if( removeFeaturesAtEachSplit ){
        for(UINT i=0; i<features.getSize(); i++){
            if( features[i] == featureIndex ){
                features.erase( features.begin()+i );
                break;
            }
        }
    }
    
    //Split the data into a left and right dataset
    ClassificationData lhs(N);
    ClassificationData rhs(N);
    
    //Reserve the memory to speed up the allocation of large datasets
    lhs.reserve( M );
    rhs.reserve( M );
    
    for(UINT i=0; i<M; i++){
        if( node->predict_( trainingData[i].getSample() ) ){
            rhs.addSample(trainingData[i].getClassLabel(), trainingData[i].getSample());
        }else lhs.addSample(trainingData[i].getClassLabel(), trainingData[i].getSample());
    }
    
    //Clear the parent dataset so we do not run out of memory with very large datasets (with very deep trees)
    trainingData.clear();   

    //Run the recursive tree building on the children
    node->setLeftChild( buildTree( lhs, node, features, classLabels ) );
    node->setRightChild( buildTree( rhs, node, features, classLabels ));
   
    return node;
}

Float DecisionTree::getNodeDistance( const VectorFloat &x, const UINT nodeID ){
    
    //Use the node ID to find the node cluster
    std::map< UINT,VectorFloat >::iterator iter = nodeClusters.find( nodeID );
    
    //If we failed to find a match, return NAN
    if( iter == nodeClusters.end() ) return NAN;
    
    //Compute the distance between the input and the node cluster
    return getNodeDistance( x, iter->second );
}

Float DecisionTree::getNodeDistance( const VectorFloat &x, const VectorFloat &y ){
    
    Float distance = 0;
    const UINT N = x.getSize();
    
    for(UINT i=0; i<N; i++){
        distance += MLBase::SQR( x[i] - y[i] );
    }
    
    //Return the squared Euclidean distance instead of actual Euclidean distance as this is faster and just as useful
    return distance;
}

Tree::TrainingMode DecisionTree::getTrainingMode() const{
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

UINT DecisionTree::getPredictedNodeID()const{
    
    if( tree == NULL ){
        return 0;
    }
    
    return tree->getPredictedNodeID();
}

bool DecisionTree::getRemoveFeaturesAtEachSplit() const{
    return removeFeaturesAtEachSplit;
}

bool DecisionTree::setTrainingMode(const Tree::TrainingMode trainingMode){ 
    if( trainingMode >= Tree::BEST_ITERATIVE_SPILT && trainingMode < Tree::NUM_TRAINING_MODES ){
        this->trainingMode = trainingMode;
        return true;
    }
    warningLog << __GRT_LOG__ << " Unknown trainingMode: " << trainingMode << std::endl;
    return false;
}

bool DecisionTree::setNumSplittingSteps(const UINT numSplittingSteps){
    if( numSplittingSteps > 0 ){
        this->numSplittingSteps = numSplittingSteps;
        return true;
    }
    warningLog << __GRT_LOG__ <<  " The number of splitting steps must be greater than zero!" << std::endl;
    return false;
}

bool DecisionTree::setMinNumSamplesPerNode(const UINT minNumSamplesPerNode){
    if( minNumSamplesPerNode > 0 ){
        this->minNumSamplesPerNode = minNumSamplesPerNode;
        return true;
    }
    warningLog << __GRT_LOG__ << " The minimum number of samples per node must be greater than zero!" << std::endl;
    return false;
}

bool DecisionTree::setMaxDepth(const UINT maxDepth){
    if( maxDepth > 0 ){
        this->maxDepth = maxDepth;
        return true;
    }
    warningLog << __GRT_LOG__ << " The maximum depth must be greater than zero!" << std::endl;
    return false;
}

bool DecisionTree::setRemoveFeaturesAtEachSplit(const bool removeFeaturesAtEachSplit){
    this->removeFeaturesAtEachSplit = removeFeaturesAtEachSplit;
    return true;
}

bool DecisionTree::setRemoveFeaturesAtEachSpilt(const bool removeFeaturesAtEachSpilt){
    return setRemoveFeaturesAtEachSplit(removeFeaturesAtEachSpilt);
}

bool DecisionTree::loadLegacyModelFromFile_v1( std::fstream &file ){
    
    std::string word;
    UINT tempTrainingMode = 0;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << __GRT_LOG__ << " Could not find NumFeatures!" << std::endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumClasses:"){
        errorLog << __GRT_LOG__ << " Could not find NumClasses!" << std::endl;
        return false;
    }
    file >> numClasses;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << __GRT_LOG__ << " Could not find UseScaling!" << std::endl;
        return false;
    }
    file >> useScaling;
    
    file >> word;
    if(word != "UseNullRejection:"){
        errorLog << __GRT_LOG__ << " Could not find UseNullRejection!" << std::endl;
        return false;
    }
    file >> useNullRejection;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        ranges.resize( numInputDimensions );
        
        file >> word;
        if(word != "Ranges:"){
            errorLog << __GRT_LOG__ << " Could not find the Ranges!" << std::endl;
            return false;
        }
        for(UINT n=0; n<ranges.size(); n++){
            file >> ranges[n].minValue;
            file >> ranges[n].maxValue;
        }
    }
    
    file >> word;
    if(word != "NumSplittingSteps:"){
        errorLog << __GRT_LOG__ << " Could not find the NumSplittingSteps!" << std::endl;
        return false;
    }
    file >> numSplittingSteps;
    
    file >> word;
    if(word != "MinNumSamplesPerNode:"){
        errorLog << __GRT_LOG__ << " Could not find the MinNumSamplesPerNode!" << std::endl;
        return false;
    }
    file >> minNumSamplesPerNode;
    
    file >> word;
    if(word != "MaxDepth:"){
        errorLog << __GRT_LOG__ << " Could not find the MaxDepth!" << std::endl;
        return false;
    }
    file >> maxDepth;
    
    file >> word;
    if(word != "RemoveFeaturesAtEachSpilt:"){
        errorLog << __GRT_LOG__ << " Could not find the RemoveFeaturesAtEachSpilt!" << std::endl;
        return false;
    }
    file >> removeFeaturesAtEachSplit;
    
    file >> word;
    if(word != "TrainingMode:"){
        errorLog << __GRT_LOG__ << " Could not find the TrainingMode!" << std::endl;
        return false;
    }
    file >> tempTrainingMode;
    trainingMode = static_cast<Tree::TrainingMode>(tempTrainingMode);
    
    file >> word;
    if(word != "TreeBuilt:"){
        errorLog << __GRT_LOG__ << " Could not find the TreeBuilt!" << std::endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        file >> word;
        if(word != "Tree:"){
            errorLog << __GRT_LOG__ << " Could not find the Tree!" << std::endl;
            return false;
        }
        
        //Create a new DTree
        tree = new DecisionTreeNode;
        
        if( tree == NULL ){
            clear();
            errorLog << __GRT_LOG__ << " Failed to create new DecisionTreeNode!" << std::endl;
            return false;
        }
        
        tree->setParent( NULL );
        if( !tree->load( file ) ){
            clear();
            errorLog << __GRT_LOG__ << " Failed to load tree from file!" << std::endl;
            return false;
        }
    }
    
    return true;
}

bool DecisionTree::loadLegacyModelFromFile_v2( std::fstream &file ){
    
    std::string word;
    UINT tempTrainingMode = 0;
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "NumSplittingSteps:"){
        errorLog << "load(string filename) - Could not find the NumSplittingSteps!" << std::endl;
        return false;
    }
    file >> numSplittingSteps;
    
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
    file >> removeFeaturesAtEachSplit;
    
    file >> word;
    if(word != "TrainingMode:"){
        errorLog << "load(string filename) - Could not find the TrainingMode!" << std::endl;
        return false;
    }
    file >> tempTrainingMode;
    trainingMode = static_cast<Tree::TrainingMode>(tempTrainingMode);
    
    file >> word;
    if(word != "TreeBuilt:"){
        errorLog << "load(string filename) - Could not find the TreeBuilt!" << std::endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        file >> word;
        if(word != "Tree:"){
            errorLog << "load(string filename) - Could not find the Tree!" << std::endl;
            return false;
        }
        
        //Create a new DTree
        tree = dynamic_cast<DecisionTreeNode*>( new DecisionTreeNode );
        
        if( tree == NULL ){
            clear();
            errorLog << "load(fstream &file) - Failed to create new DecisionTreeNode!" << std::endl;
            return false;
        }
        
        tree->setParent( NULL );
        if( !tree->load( file ) ){
            clear();
            errorLog << "load(fstream &file) - Failed to load tree from file!" << std::endl;
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

bool DecisionTree::loadLegacyModelFromFile_v3( std::fstream &file ){
    
    std::string word;
    UINT tempTrainingMode = 0;
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "NumSplittingSteps:"){
        errorLog << "load(string filename) - Could not find the NumSplittingSteps!" << std::endl;
        return false;
    }
    file >> numSplittingSteps;
    
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
    file >> removeFeaturesAtEachSplit;
    
    file >> word;
    if(word != "TrainingMode:"){
        errorLog << "load(string filename) - Could not find the TrainingMode!" << std::endl;
        return false;
    }
    file >> tempTrainingMode;
    trainingMode = static_cast<Tree::TrainingMode>(tempTrainingMode);
    
    file >> word;
    if(word != "TreeBuilt:"){
        errorLog << "load(string filename) - Could not find the TreeBuilt!" << std::endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        file >> word;
        if(word != "Tree:"){
            errorLog << "load(string filename) - Could not find the Tree!" << std::endl;
            return false;
        }
        
        //Create a new DTree
        tree = new DecisionTreeNode;
        
        if( tree == NULL ){
            clear();
            errorLog << "load(fstream &file) - Failed to create new DecisionTreeNode!" << std::endl;
            return false;
        }
        
        tree->setParent( NULL );
        if( !tree->load( file ) ){
            clear();
            errorLog << "load(fstream &file) - Failed to load tree from file!" << std::endl;
            return false;
        }
        
        //Load the null rejection data if needed
        if( useNullRejection ){
            
            UINT numNodes = 0;
            classClusterMean.resize( numClasses );
            classClusterStdDev.resize( numClasses );
            
            file >> word;
            if(word != "ClassClusterMean:"){
                errorLog << "load(string filename) - Could not find the ClassClusterMean header!" << std::endl;
                return false;
            }
            for(UINT k=0; k<numClasses; k++){
                file >> classClusterMean[k];
            }
            
            file >> word;
            if(word != "ClassClusterStdDev:"){
                errorLog << "load(string filename) - Could not find the ClassClusterStdDev header!" << std::endl;
                return false;
            }
            for(UINT k=0; k<numClasses; k++){
                file >> classClusterStdDev[k];
            }
            
            file >> word;
            if(word != "NumNodes:"){
                errorLog << "load(string filename) - Could not find the NumNodes header!" << std::endl;
                return false;
            }
            file >> numNodes;
            
            file >> word;
            if(word != "NodeClusters:"){
                errorLog << "load(string filename) - Could not find the NodeClusters header!" << std::endl;
                return false;
            }
            
            UINT nodeID = 0;
            VectorFloat cluster( numInputDimensions );
            for(UINT i=0; i<numNodes; i++){
                
                //load the nodeID
                file >> nodeID;
                
                for(UINT j=0; j<numInputDimensions; j++){
                    file >> cluster[j];
                }
                
                //Add the cluster to the cluster nodes map
                nodeClusters[ nodeID ] = cluster;
            }
            
            //Recompute the null rejection thresholds
            recomputeNullRejectionThresholds();
        }
        
        //Resize the prediction results to make sure it is setup for realtime prediction
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}

GRT_END_NAMESPACE
