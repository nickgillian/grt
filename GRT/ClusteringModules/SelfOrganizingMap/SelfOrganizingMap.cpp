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

#include "SelfOrganizingMap.h"

namespace GRT{
    
//Register the SelfOrganizingMap class with the Clusterer base class
RegisterClustererModule< SelfOrganizingMap > SelfOrganizingMap::registerModule("SelfOrganizingMap");

SelfOrganizingMap::SelfOrganizingMap( const UINT networkSize, const UINT networkTypology, const UINT maxNumEpochs, const double alphaStart, const double alphaEnd ){
    
    this->numClusters = networkSize;
    this->networkTypology = networkTypology;
    this->maxNumEpochs = maxNumEpochs;
    this->alphaStart = alphaStart;
    this->alphaEnd = alphaEnd;
    
    classType = "SelfOrganizingMap";
    clustererType = classType;
    debugLog.setProceedingText("[DEBUG SelfOrganizingMap]");
    errorLog.setProceedingText("[ERROR SelfOrganizingMap]");
    trainingLog.setProceedingText("[TRAINING SelfOrganizingMap]");
    warningLog.setProceedingText("[WARNING SelfOrganizingMap]");
}
    
SelfOrganizingMap::SelfOrganizingMap(const SelfOrganizingMap &rhs){
    
    classType = "SelfOrganizingMap";
    clustererType = classType;
    debugLog.setProceedingText("[DEBUG KMeans]");
    errorLog.setProceedingText("[ERROR KMeans]");
    trainingLog.setProceedingText("[TRAINING KMeans]");
    warningLog.setProceedingText("[WARNING KMeans]");
    
    if( this != &rhs ){
        
        this->networkTypology = rhs.networkTypology;
        this->alphaStart = rhs.alphaStart;
        this->alphaEnd = rhs.alphaEnd;
        
        //Clone the Clusterer variables
        copyBaseVariables( (Clusterer*)&rhs );
    }
}

SelfOrganizingMap::~SelfOrganizingMap(){

}
    
SelfOrganizingMap& SelfOrganizingMap::operator=(const SelfOrganizingMap &rhs){
    
    if( this != &rhs ){
        
        this->networkTypology = rhs.networkTypology;
        this->alphaStart = rhs.alphaStart;
        this->alphaEnd = rhs.alphaEnd;
        
        //Clone the Clusterer variables
        copyBaseVariables( (Clusterer*)&rhs );
    }
    
    return *this;
}
    
bool SelfOrganizingMap::deepCopyFrom(const Clusterer *clusterer){
    
    if( clusterer == NULL ) return false;
    
    if( this->getClustererType() == clusterer->getClustererType() ){
        //Clone the SelfOrganizingMap values
        SelfOrganizingMap *ptr = (SelfOrganizingMap*)clusterer;
        
        this->networkTypology = ptr->networkTypology;
        this->alphaStart = ptr->alphaStart;
        this->alphaEnd = ptr->alphaEnd;
        
        //Clone the Clusterer variables
        return copyBaseVariables( clusterer );
    }
    
    return false;
}
    
bool SelfOrganizingMap::reset(){
    
    //Reset the base class
    Clusterer::reset();
    
    return true;
}

bool SelfOrganizingMap::clear(){
    
    //Reset the base class
    Clusterer::clear();
    
    //Clear the SelfOrganizingMap models
    neurons.clear();
    networkWeights.clear();
    
    return true;
}

bool SelfOrganizingMap::train_( MatrixDouble &data ){
    
    //Clear any previous models
    clear();
    
    const UINT M = data.getNumRows();
    const UINT N = data.getNumCols();
    numInputDimensions = N;
    numOutputDimensions = numClusters;
    Random rand;
    
    //Setup the neurons
    neurons.resize( numClusters );
    
    if( neurons.size() != numClusters ){
        errorLog << "train_( MatrixDouble &data ) - Failed to resize neurons vector, there might not be enough memory!" << endl;
        return false;
    }
    
    for(UINT j=0; j<numClusters; j++){
        
        //Init the neuron
        neurons[j].init( N, 0.5 );
        
        //Set the weights as a random training example
        neurons[j].weights = data.getRowVector( rand.getRandomNumberInt(0, M) );
    }
    
    //Setup the network weights
    switch( networkTypology ){
        case RANDOM_NETWORK:
            networkWeights.resize(numClusters, numClusters);
            
            //Set the diagonal weights as 1 (as i==j)
            for(UINT i=0; i<numClusters; i++){
                networkWeights[i][i] = 1;
            }
            
            //Randomize the other weights
            UINT indexA = 0;
            UINT indexB = 0;
            double weight = 0;
            for(UINT i=0; i<numClusters*numClusters; i++){
                indexA = rand.getRandomNumberInt(0, numClusters);
                indexB = rand.getRandomNumberInt(0, numClusters);
                
                //Make sure the two random indexs are the same (as this is a diagonal and should be 1)
                if( indexA != indexB ){
                    //Pick a random weight between these two neurons
                    weight = rand.getRandomNumberUniform(0,1);
                    
                    //The weight betwen neurons a and b is the mirrored
                    networkWeights[indexA][indexB] = weight;
                    networkWeights[indexB][indexA] = weight;
                }
            }
            break;
    }
    
    //Scale the data if needed
    ranges = data.getRanges();
    if( useScaling ){
        for(UINT i=0; i<M; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                data[i][j] = scale(data[i][j],ranges[j].minValue,ranges[j].maxValue,0,1);
            }
        }
    }
    
    double error = 0;
    double lastError = 0;
    double trainingSampleError = 0;
    double delta = 0;
    double minChange = 0;
    double weightUpdate = 0;
    double weightUpdateSum = 0;
    double alpha = 1.0;
    double neuronDiff = 0;
    UINT iter = 0;
    bool keepTraining = true;
    VectorDouble trainingSample;
    vector< UINT > randomTrainingOrder(M);
    
    //In most cases, the training data is grouped into classes (100 samples for class 1, followed by 100 samples for class 2, etc.)
    //This can cause a problem for stochastic gradient descent algorithm. To avoid this issue, we randomly shuffle the order of the
    //training samples. This random order is then used at each epoch.
    for(UINT i=0; i<M; i++){
        randomTrainingOrder[i] = i;
    }
    std::random_shuffle(randomTrainingOrder.begin(), randomTrainingOrder.end());
    
    //Enter the main training loop
    while( keepTraining ){
        
        //Update alpha based on the current iteration
        alpha = Util::scale(iter,0,maxNumEpochs,alphaStart,alphaEnd);
        
        //Run one epoch of training using the online best-matching-unit algorithm
        error = 0;
        for(UINT i=0; i<M; i++){
            
            trainingSampleError = 0;
            
            //Get the i'th random training sample
            trainingSample = data.getRowVector( randomTrainingOrder[i] );
            
            //Find the best matching unit
            double dist = 0;
            double bestDist = numeric_limits<double>::max();
            UINT bestIndex = 0;
            for(UINT j=0; j<numClusters; j++){
                dist = neurons[j].getSquaredWeightDistance( trainingSample );
                if( dist < bestDist ){
                    bestDist = dist;
                    bestIndex = j;
                }
            }
            
            //Update the weights based on the distance to the winning neuron
            //Neurons closer to the winning neuron will have their weights update more
            for(UINT j=0; j<numClusters; j++){
                
                //Update the weights for the j'th neuron
                weightUpdateSum = 0;
                neuronDiff = 0;
                for(UINT n=0; n<N; n++){
                    neuronDiff = trainingSample[n] - neurons[j][n];
                    weightUpdate = networkWeights[bestIndex][j] * alpha * neuronDiff;
                    neurons[j][n] += weightUpdate;
                    weightUpdateSum += neuronDiff;
                }
                
                trainingSampleError += SQR( weightUpdateSum );
            }
            
            error += sqrt( trainingSampleError / numClusters );
        }
        
        //Compute the error
        delta = fabs( error-lastError );
        lastError = error;
        
        //Check to see if we should stop
        if( delta <= minChange ){
            converged = true;
            keepTraining = false;
        }
        
        if( std::isinf( error ) ){
            errorLog << "train_(MatrixDouble &data) - Training failed! Error is NAN!" << endl;
            return false;
        }
        
        if( ++iter >= maxNumEpochs ){
            keepTraining = false;
        }
        
        trainingLog << "Epoch: " << iter << " Squared Error: " << error << " Delta: " << delta << " Alpha: " << alpha << endl;
    }
    
    numTrainingIterationsToConverge = iter;
    trained = true;
    
    return true;
}
    
bool SelfOrganizingMap::train_(ClassificationData &trainingData){
    MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train_(data);
}

bool SelfOrganizingMap::train_(UnlabelledData &trainingData){
    MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train_(data);
}
    
bool SelfOrganizingMap::map_( VectorDouble &x ){
    
    if( !trained ){
        return false;
    }
    
    if( useScaling ){
        for(UINT i=0; i<numInputDimensions; i++){
            x[i] = scale(x[i], ranges[i].minValue, ranges[i].maxValue, 0, 1);
        }
    }
    
    if( mappedData.size() != numClusters )
        mappedData.resize( numClusters );
    
    for(UINT i=0; i<numClusters; i++){
        mappedData[i] = neurons[i].fire( x );
    }
    
    return true;
}
    
bool SelfOrganizingMap::saveModelToFile(string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(string filename) - Failed to open file!" << endl;
        return false;
    }
    
    if( !saveModelToFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
}

bool SelfOrganizingMap::saveModelToFile(fstream &file) const{
    
    if( !trained ){
        errorLog << "saveModelToFile(fstream &file) - Can't save model to file, the model has not been trained!" << endl;
        return false;
    }
    
    file << "GRT_SELF_ORGANIZING_MAP_MODEL_FILE_V1.0\n";
    
    if( !saveClustererSettingsToFile( file ) ){
        errorLog << "saveModelToFile(fstream &file) - Failed to save cluster settings to file!" << endl;
        return false;
    }
    
    file << "NetworkTypology: " << networkTypology << endl;
    file << "AlphaStart: " << alphaStart <<endl;
    file << "AlphaEnd: " << alphaEnd <<endl;
    
    if( trained ){
        file << "NetworkWeights: \n";
        for(UINT i=0; i<networkWeights.getNumRows(); i++){
            for(UINT j=0; j<networkWeights.getNumCols(); j++){
                file << networkWeights[i][j];
                if( j<networkWeights.getNumCols()-1 ) file << "\t";
            }
            file << "\n";
        }
        
        file << "Neurons: \n";
        for(UINT i=0; i<neurons.size(); i++){
            if( !neurons[i].saveNeuronToFile( file ) ){
                errorLog << "saveModelToFile(fstream &file) - Failed to save neuron to file!" << endl;
                return false;
            }
        }
    }
    
    return true;
    
}

bool SelfOrganizingMap::loadModelFromFile(string fileName){
    
    std::fstream file;
    string word;
    file.open(fileName.c_str(), std::ios::in);
    
    if(!file.is_open()){
        errorLog << "loadModelFromFile(string filename) - Failed to open file!" << endl;
        return false;
    }
    
    if( !loadModelFromFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
    
}

bool SelfOrganizingMap::loadModelFromFile(fstream &file){
    
    //Clear any previous model
    clear();
    
    string word;
    file >> word;
    if( word != "GRT_SELF_ORGANIZING_MAP_MODEL_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load file header!" << endl;
        return false;
    }
    
    if( !loadClustererSettingsFromFile( file ) ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load cluster settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if( word != "NetworkTypology:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load NetworkTypology header!" << endl;
        return false;
    }
    file >> networkTypology;
    
    file >> word;
    if( word != "AlphaStart:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load AlphaStart header!" << endl;
        return false;
    }
    file >> alphaStart;
    
    file >> word;
    if( word != "AlphaEnd:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load alphaEnd header!" << endl;
        return false;
    }
    file >> alphaEnd;
    
    //Load the model if it has been trained
    if( trained ){
        file >> word;
        if( word != "NetworkWeights:" ){
            errorLog << "loadModelFromFile(fstream &file) - Failed to load NetworkWeights header!" << endl;
            return false;
        }
        
        networkWeights.resize(numClusters, numClusters);
        for(UINT i=0; i<networkWeights.getNumRows(); i++){
            for(UINT j=0; j<networkWeights.getNumCols(); j++){
                file >> networkWeights[i][j];
            }
        }
        
        file >> word;
        if( word != "Neurons:" ){
            errorLog << "loadModelFromFile(fstream &file) - Failed to load Neurons header!" << endl;
            return false;
        }
        
        neurons.resize(numClusters);
        for(UINT i=0; i<neurons.size(); i++){
            if( !neurons[i].loadNeuronFromFile( file ) ){
                errorLog << "loadModelFromFile(fstream &file) - Failed to save neuron to file!" << endl;
                return false;
            }
        }
    }
    
    return true;
}
    
bool SelfOrganizingMap::validateNetworkTypology( const UINT networkTypology ){
    if( networkTypology == RANDOM_NETWORK ) return true;
    
    warningLog << "validateNetworkTypology(const UINT networkTypology) - Unknown networkTypology!" << endl;
    
    return false;
}
    
UINT SelfOrganizingMap::getNetworkSize() const{
    return numClusters;
}
    
double SelfOrganizingMap::getAlphaStart() const{
    return alphaStart;
}

double SelfOrganizingMap::getAlphaEnd() const{
    return alphaEnd;
}
    
VectorDouble SelfOrganizingMap::getMappedData() const{
    return mappedData;
}
    
vector< GaussNeuron > SelfOrganizingMap::getNeurons() const{
    return neurons;
}
    
const vector< GaussNeuron >& SelfOrganizingMap::getNeuronsRef() const{
    return neurons;
}

MatrixDouble SelfOrganizingMap::getNetworkWeights() const{
    return networkWeights;
}
    
bool SelfOrganizingMap::setNetworkSize( const UINT networkSize ){
    if( networkSize > 0 ){
        this->numClusters = networkSize;
        return true;
    }
    
    warningLog << "setNetworkSize(const UINT networkSize) - The networkSize must be greater than 0!" << endl;
    
    return false;
}
    
bool SelfOrganizingMap::setNetworkTypology( const UINT networkTypology ){
    if( validateNetworkTypology( networkTypology ) ){
        this->networkTypology = networkTypology;
        return true;
    }
    return false;
}
    
bool SelfOrganizingMap::setAlphaStart( const double alphaStart ){
    
    if( alphaStart > 0 ){
        this->alphaStart = alphaStart;
        return true;
    }
    
    warningLog << "setAlphaStart(const double alphaStart) - AlphaStart must be greater than zero!" << endl;
    
    return false;
}
    
bool SelfOrganizingMap::setAlphaEnd( const double alphaEnd ){
    
    if( alphaEnd > 0 ){
        this->alphaEnd = alphaEnd;
        return true;
    }
    
    warningLog << "setAlphaEnd(const double alphaEnd) - AlphaEnd must be greater than zero!" << endl;
    
    return false;
}

} //End of namespace GRT
