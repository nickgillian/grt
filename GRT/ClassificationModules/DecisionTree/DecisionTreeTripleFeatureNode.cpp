
#include "DecisionTreeTripleFeatureNode.h"

using namespace GRT;
    
//Register the DecisionTreeTripleFeatureNode module with the Node base class
RegisterNode< DecisionTreeTripleFeatureNode > DecisionTreeTripleFeatureNode::registerModule("DecisionTreeTripleFeatureNode");
    
DecisionTreeTripleFeatureNode::DecisionTreeTripleFeatureNode(){
    nodeType = "DecisionTreeTripleFeatureNode";
    parent = NULL;
    leftChild = NULL;
    rightChild = NULL;
    clear();
}

DecisionTreeTripleFeatureNode::~DecisionTreeTripleFeatureNode(){
    clear();
}

bool DecisionTreeTripleFeatureNode::predict(const VectorDouble &x) {

    if( (x[ featureIndexA ] - x[ featureIndexB ]) >= (x[ featureIndexC ] - x[ featureIndexB ]) ) return true;

    return false;
}

bool DecisionTreeTripleFeatureNode::clear(){
    
    //Call the base class clear function
    DecisionTreeNode::clear();
    
    featureIndexA = 0;
    featureIndexB = 0;
    featureIndexC = 0;
  
    return true;
}

bool DecisionTreeTripleFeatureNode::print() const{
    
    ostringstream stream;
    
    if( getModel( stream ) ){
        cout << stream.str();
        return true;
    }
    
    return false;
}
    
bool DecisionTreeTripleFeatureNode::getModel(ostream &stream) const{

    string tab = "";
    for(UINT i=0; i<depth; i++) tab += "\t";
    
    stream << tab << "depth: " << depth;
    stream << " nodeSize: " << nodeSize;
    stream << " featureIndexA: " << featureIndexA;
    stream << " featureIndexB: " << featureIndexB;
    stream << " featureIndexC: " << featureIndexC;
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

Node* DecisionTreeTripleFeatureNode::deepCopyNode() const{
    
    DecisionTreeTripleFeatureNode *node = new DecisionTreeTripleFeatureNode;
    
    if( node == NULL ){
        return NULL;
    }
    
    //Copy this node into the node
    node->depth = depth;
    node->isLeafNode = isLeafNode;
    node->nodeID = nodeID;
    node->predictedNodeID = predictedNodeID;
    node->nodeSize = nodeSize;
    node->featureIndexA = featureIndexA;
    node->featureIndexB = featureIndexB;
    node->featureIndexC = featureIndexC;
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
    
    return dynamic_cast< DecisionTreeTripleFeatureNode* >( node );
}

DecisionTreeTripleFeatureNode* DecisionTreeTripleFeatureNode::deepCopy() const{
    return dynamic_cast< DecisionTreeTripleFeatureNode* >( deepCopyNode() );
}

UINT DecisionTreeTripleFeatureNode::getFeatureIndexA() const{
    return featureIndexA;
}

UINT DecisionTreeTripleFeatureNode::getFeatureIndexB() const{
    return featureIndexB;
}

UINT DecisionTreeTripleFeatureNode::getFeatureIndexC() const{
    return featureIndexC;
}

bool DecisionTreeTripleFeatureNode::set(const UINT nodeSize,const UINT featureIndexA,const UINT featureIndexB,const UINT featureIndexC,const VectorDouble &classProbabilities){
    this->nodeSize = nodeSize;
    this->featureIndexA = featureIndexA;
    this->featureIndexB = featureIndexB;
    this->featureIndexC = featureIndexC;
    this->classProbabilities = classProbabilities;
    return true;
}

bool DecisionTreeTripleFeatureNode::computeBestSpiltBestIterativeSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){
    
    return computeBestSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError);
}

bool DecisionTreeTripleFeatureNode::computeBestSpiltBestRandomSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){

    return computeBestSpilt( numSplittingSteps, trainingData, features, classLabels, featureIndex, minError);
}

bool DecisionTreeTripleFeatureNode::computeBestSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const vector< UINT > &features, const vector< UINT > &classLabels, UINT &featureIndex, double &minError ){

    const UINT M = trainingData.getNumSamples();
    const UINT N = (UINT)features.size();
    const UINT K = (UINT)classLabels.size();
    
    if( N == 0 ) return false;
    
    minError = numeric_limits<double>::max();
    Random random;
    UINT bestFeatureIndexA = 0;
    UINT bestFeatureIndexB = 0;
    UINT bestFeatureIndexC = 0;
    double error = 0;
    double giniIndexL = 0;
    double giniIndexR = 0;
    double weightL = 0;
    double weightR = 0;
    vector< UINT > groupIndex(M);
    VectorDouble groupCounter(2,0);
    vector< MinMax > ranges = trainingData.getRanges();
    MatrixDouble classProbabilities(K,2);
    MatrixDouble data(M,1); //This will store our temporary data for each dimension
    
    //Randomly select which features we want to use
    UINT numRandomFeatures = numSplittingSteps > N ? N : numSplittingSteps;
    vector< UINT > randomFeatures = random.getRandomSubset( 0, N, numRandomFeatures );

    //Loop over each random feature and try and find the best split point
    for(UINT n=0; n<numRandomFeatures; n++){
        
	//Randomly select 3 features to use
        featureIndexB = features[ randomFeatures[n] ]; //B is the central feature
        featureIndexA = features[ randomFeatures[ random.getRandomNumberInt(0,numRandomFeatures) ] ];
        featureIndexC = features[ randomFeatures[ random.getRandomNumberInt(0,numRandomFeatures) ] ];
        
        //Iterate over each sample and work out if it should be in the lhs (0) or rhs (1) group based on the current threshold
        groupCounter[0] = groupCounter[1] = 0;
        classProbabilities.setAllValues(0);
        for(UINT i=0; i<M; i++){
            groupIndex[i] = predict( trainingData[i].getSample() ) ? 1 : 0;
            groupCounter[ groupIndex[i] ]++;
            classProbabilities[ getClassLabelIndexValue(trainingData[i].getClassLabel(),classLabels) ][ groupIndex[i] ]++;
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
            bestFeatureIndexA = featureIndexA;
            bestFeatureIndexB = featureIndexB;
            bestFeatureIndexC = featureIndexC;
        }
     }

     trainingLog << "Best features indexs: [" << bestFeatureIndexA << "," << bestFeatureIndexB << "," << bestFeatureIndexC << "] Min Error: " << minError << endl;
     
     //Set the best feature index that will be returned to the DecisionTree that called this function
     featureIndex = bestFeatureIndexB;
     
     //Store the node size, feature indexs and class probabilities for this node
     set(M,bestFeatureIndexA,bestFeatureIndexB,bestFeatureIndexC,trainingData.getClassProbabilities(classLabels));

     return true;
}

bool DecisionTreeTripleFeatureNode::saveParametersToFile(fstream &file) const{
    
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
    file << "FeatureIndexA: " << featureIndexA << endl;
    file << "FeatureIndexB: " << featureIndexB << endl;
    file << "FeatureIndexC: " << featureIndexC << endl;
    
    return true;
}

bool DecisionTreeTripleFeatureNode::loadParametersFromFile(fstream &file){
    
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
    if( word != "FeatureIndexA:" ){
        errorLog << "loadParametersFromFile(fstream &file) - Failed to find FeatureIndexA header!" << endl;
        return false;
    }
    file >> featureIndexA;
    
    file >> word;
    if( word != "FeatureIndexB:" ){
        errorLog << "loadParametersFromFile(fstream &file) - Failed to find FeatureIndexB header!" << endl;
        return false;
    }
    file >> featureIndexB;

    file >> word;
    if( word != "FeatureIndexC:" ){
        errorLog << "loadParametersFromFile(fstream &file) - Failed to find FeatureIndexC header!" << endl;
        return false;
    }
    file >> featureIndexC;
 
    return true;
}

