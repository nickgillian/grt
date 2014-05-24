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

RandomForests::RandomForests(const UINT forestSize,const UINT numRandomSplits,const UINT minNumSamplesPerNode,const UINT maxDepth,const bool useScaling)
{
    this->forestSize = forestSize;
    this->numRandomSplits = numRandomSplits;
    this->minNumSamplesPerNode = minNumSamplesPerNode;
    this->maxDepth = maxDepth;
    this->useScaling = useScaling;
    classType = "RandomForests";
    classifierType = classType;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    debugLog.setProceedingText("[DEBUG RandomForests]");
    errorLog.setProceedingText("[ERROR RandomForests]");
    trainingLog.setProceedingText("[TRAINING RandomForests]");
    warningLog.setProceedingText("[WARNING RandomForests]");
}
    
RandomForests::RandomForests(const RandomForests &rhs){
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
}
    
RandomForests& RandomForests::operator=(const RandomForests &rhs){
	if( this != &rhs ){
        //Clear this tree
        clear();
        
        if( rhs.getTrained() ){
            //Deep copy the forest
            for(UINT i=0; i<rhs.forest.size(); i++){
                this->forest.push_back( rhs.forest[i]->deepCopyTree() );
            }
        }
        
        this->forestSize = rhs.forestSize;
        this->numRandomSplits = rhs.numRandomSplits;
        this->minNumSamplesPerNode = rhs.minNumSamplesPerNode;
        this->maxDepth = rhs.maxDepth;

        //Copy the base classifier variables
        copyBaseVariables( (Classifier*)&rhs );
	}
	return *this;
}

bool RandomForests::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        
        RandomForests *ptr = (RandomForests*)classifier;
        
        //Clear this tree
        this->clear();
        
        if( ptr->getTrained() ){
            //Deep copy the forest
            for(UINT i=0; i<ptr->forest.size(); i++){
                this->forest.push_back( ptr->forest[i]->deepCopyTree() );
            }
        }
        
        this->forestSize = ptr->forestSize;
        this->numRandomSplits = ptr->numRandomSplits;
        this->minNumSamplesPerNode = ptr->minNumSamplesPerNode;
        this->maxDepth = ptr->maxDepth;
        
        //Copy the base classifier variables
        return copyBaseVariables( classifier );
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
        errorLog << "train_(ClassificationData &labelledTrainingData) - Training data has zero samples!" << endl;
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
    DecisionTree tree;
    tree.enableScaling( false ); //We have already scaled the training data so we do not need to scale it again
    tree.setTrainingMode( DecisionTree::BEST_RANDOM_SPLIT );
    tree.setNumSplittingSteps( numRandomSplits );
    tree.setMinNumSamplesPerNode( minNumSamplesPerNode );
    tree.setMaxDepth( maxDepth );
    
    for(UINT i=0; i<forestSize; i++){
        ClassificationData data = trainingData.getBootstrappedDataset();
        
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
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - Model Not Trained!" << endl;
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
    
    for(UINT j=0; j<numClasses; j++){
        classDistances[j] = 0;
    }
    
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
    
    maxLikelihood = 0;
    bestDistance = 0;
    UINT bestIndex = 0;
    for(UINT k=0; k<numClasses; k++){
        classLikelihoods[k] = classDistances[k] / double(forestSize);
        
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
    for(UINT i=0; i<forest.size(); i++){
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
    cout << "ForestBuilt: " << (trained ? 1 : 0) << endl;
    
    cout << "Forest:\n";
    for(UINT i=0; i<forestSize; i++){
        cout << "Tree: " << i+1 << endl;
        forest[i]->print();
    }
    
    return true;
}
    
bool RandomForests::saveModelToFile(string filename) const{

    if( !trained ) return false;
    
	std::fstream file; 
	file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }

	file.close();

	return true;
}
    
bool RandomForests::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
	{
		errorLog <<"saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
	file << "GRT_RANDOM_FOREST_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
		return false;
    }
    
    file << "ForestSize: " << forestSize << endl;
    file << "NumSplittingSteps: " << numRandomSplits << endl;
    file << "MinNumSamplesPerNode: " << minNumSamplesPerNode << endl;
    file << "MaxDepth: " << maxDepth << endl;
    file << "ForestBuilt: " << (trained ? 1 : 0) << endl;
    
    if( trained ){
        file << "Forest:\n";
        for(UINT i=0; i<forestSize; i++){
            file << "Tree: " << i+1 << endl;
            if( !forest[i]->saveToFile( file ) ){
                errorLog << "saveModelToFile(fstream &file) - Failed to save tree " << i << " to file!" << endl;
                return false;
            }
        }
    }
    
    return true;
}

bool RandomForests::loadModelFromFile(string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    file.close();

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
    
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_RANDOM_FOREST_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Find the file type header
    if(word != "GRT_RANDOM_FOREST_MODEL_FILE_V2.0"){
        errorLog << "loadModelFromFile(string filename) - Could not find Model File Header" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
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
    if(word != "ForestBuilt:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the ForestBuilt!" << endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        file >> word;
        if(word != "Forest:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the Forest!" << endl;
            return false;
        }
        
        UINT treeIndex;
        for(UINT i=0; i<forestSize; i++){
            
            file >> word;
            if(word != "Tree:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the Tree Header!" << endl;
                return false;
            }
            file >> treeIndex;
            
            if( treeIndex != i+1 ){
                errorLog << "loadModelFromFile(string filename) - Incorrect tree index: " << treeIndex << endl;
                return false;
            }
            
            //Create a new DTree
            DecisionTreeNode *tree = new DecisionTreeNode;
            
            if( tree == NULL ){
                errorLog << "loadModelFromFile(fstream &file) - Failed to create new Tree!" << endl;
                return false;
            }
            
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
    
UINT RandomForests::getForestSize()const{
    return forestSize;
}
    
UINT RandomForests::getNumRandomSpilts()const{
    return numRandomSplits;
}

UINT RandomForests::getMinNumSamplesPerNode()const{
    return minNumSamplesPerNode;
}

UINT RandomForests::getMaxDepth()const{
    return maxDepth;
}
    
bool RandomForests::setForestSize(const UINT forestSize){
    if( forestSize > 0 ){
        this->forestSize = forestSize;
        clear();
        return true;
    }
    return false;
}
    
bool RandomForests::setNumRandomSpilts(const UINT numRandomSplits){
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
    
bool RandomForests::loadLegacyModelFromFile( fstream &file ){
    
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
        ranges.resize(numInputDimensions);
        
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
    if(word != "ForestBuilt:"){
        errorLog << "loadModelFromFile(string filename) - Could not find the ForestBuilt!" << endl;
        return false;
    }
    file >> trained;
    
    if( trained ){
        file >> word;
        if(word != "Forest:"){
            errorLog << "loadModelFromFile(string filename) - Could not find the Forest!" << endl;
            return false;
        }
        
        UINT treeIndex;
        for(UINT i=0; i<forestSize; i++){
            
            file >> word;
            if(word != "Tree:"){
                errorLog << "loadModelFromFile(string filename) - Could not find the Tree Header!" << endl;
                return false;
            }
            file >> treeIndex;
            
            if( treeIndex != i+1 ){
                errorLog << "loadModelFromFile(string filename) - Incorrect tree index: " << treeIndex << endl;
                return false;
            }
            
            //Create a new DTree
            DecisionTreeNode *tree = new DecisionTreeNode;
            
            if( tree == NULL ){
                errorLog << "loadModelFromFile(fstream &file) - Failed to create new Tree!" << endl;
                return false;
            }
            
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

} //End of namespace GRT

