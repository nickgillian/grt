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
#include "SelfOrganizingMap.h"

GRT_BEGIN_NAMESPACE

#define SOM_MIN_TARGET -1.0
#define SOM_MAX_TARGET 1.0

//Define the string that will be used to identify the object
const std::string SelfOrganizingMap::id = "SelfOrganizingMap";
std::string SelfOrganizingMap::getId() { return SelfOrganizingMap::id; }
    
//Register the SelfOrganizingMap class with the Clusterer base class
RegisterClustererModule< SelfOrganizingMap > SelfOrganizingMap::registerModule( SelfOrganizingMap::getId() );

SelfOrganizingMap::SelfOrganizingMap( const UINT networkSize, const UINT networkTypology, const UINT maxNumEpochs, const Float sigmaWeight, const Float alphaStart, const Float alphaEnd ) : Clusterer( SelfOrganizingMap::getId() )
{
    this->numClusters = networkSize;
    this->networkTypology = networkTypology;
    this->maxNumEpochs = maxNumEpochs;
    this->sigmaWeight = sigmaWeight;
    this->alphaStart = alphaStart;
    this->alphaEnd = alphaEnd;
}
    
SelfOrganizingMap::SelfOrganizingMap(const SelfOrganizingMap &rhs) : Clusterer( SelfOrganizingMap::getId() )
{
    if( this != &rhs ){
        
        this->numClusters = rhs.numClusters;
        this->networkTypology = rhs.networkTypology;
        this->sigmaWeight = rhs.sigmaWeight;
        this->alphaStart = rhs.alphaStart;
        this->alphaEnd = rhs.alphaEnd;
        this->neurons = rhs.neurons;
        this->mappedData = rhs.mappedData;
        
        //Clone the Clusterer variables
        copyBaseVariables( (Clusterer*)&rhs );
    }
}

SelfOrganizingMap::~SelfOrganizingMap(){

}
    
SelfOrganizingMap& SelfOrganizingMap::operator=(const SelfOrganizingMap &rhs){
    
    if( this != &rhs ){
        
        this->numClusters = rhs.numClusters;
        this->networkTypology = rhs.networkTypology;
        this->sigmaWeight = rhs.sigmaWeight;
        this->alphaStart = rhs.alphaStart;
        this->alphaEnd = rhs.alphaEnd;
        this->neurons = rhs.neurons;
        this->mappedData = rhs.mappedData;
        
        //Clone the Clusterer variables
        copyBaseVariables( (Clusterer*)&rhs );
    }
    
    return *this;
}
    
bool SelfOrganizingMap::deepCopyFrom(const Clusterer *clusterer){
    
    if( clusterer == NULL ) return false;
    
    if( this->getId() == clusterer->getId() ){
        //Clone the SelfOrganizingMap values
        const SelfOrganizingMap *ptr = dynamic_cast<const SelfOrganizingMap*>(clusterer);
        
        this->numClusters = ptr->numClusters;
        this->networkTypology = ptr->networkTypology;
        this->sigmaWeight = ptr->sigmaWeight;
        this->alphaStart = ptr->alphaStart;
        this->alphaEnd = ptr->alphaEnd;
        this->neurons = ptr->neurons;
        this->mappedData = ptr->mappedData;
        
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
    
    return true;
}

bool SelfOrganizingMap::train_( MatrixFloat &data ){
    
    //Clear any previous models
    clear();
    
    const UINT M = data.getNumRows();
    const UINT N = data.getNumCols();
    numInputDimensions = N;
    numOutputDimensions = numClusters*numClusters;
    Random rand;
    
    //Setup the neurons
    neurons.resize( numClusters, numClusters );
    
    if( neurons.getSize() != numClusters*numClusters ){
        errorLog << "train_( MatrixFloat &data ) - Failed to resize neurons matrix, there might not be enough memory!" << std::endl;
        return false;
    }
    
    //Init the neurons
    for(UINT i=0; i<numClusters; i++){
        for(UINT j=0; j<numClusters; j++){
            neurons[i][j].init( N, 0.5, SOM_MIN_TARGET, SOM_MAX_TARGET );
        }
    }
    
    //Scale the data if needed
    ranges = data.getRanges();
    if( useScaling ){
        for(UINT i=0; i<M; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                data[i][j] = scale(data[i][j],ranges[j].minValue,ranges[j].maxValue,SOM_MIN_TARGET,SOM_MAX_TARGET);
            }
        }
    }
    
    Float error = 0;
    Float lastError = 0;
    Float trainingSampleError = 0;
    Float delta = 0;
    Float minChange = 0;
    Float weightUpdate = 0;
    Float weightUpdateSum = 0;
    Float alpha = 1.0;
    Float neuronDiff = 0;
    Float neuronWeightFunction = 0;
    Float gamma = 0;
    UINT iter = 0;
    bool keepTraining = true;
    VectorFloat trainingSample;
    Vector< UINT > randomTrainingOrder(M);
    
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
        for(UINT m=0; m<M; m++){
            
            trainingSampleError = 0;
            
            //Get the i'th random training sample
            trainingSample = data.getRowVector( randomTrainingOrder[m] );
            
            //Find the best matching unit
            Float dist = 0;
            Float bestDist = grt_numeric_limits< Float >::max();
            UINT bestIndexRow = 0;
            UINT bestIndexCol = 0;
            for(UINT i=0; i<numClusters; i++){
                for(UINT j=0; j<numClusters; j++){
                    dist = neurons[i][j].getSquaredWeightDistance( trainingSample );
                    if( dist < bestDist ){
                        bestDist = dist;
                        bestIndexRow = i;
                        bestIndexCol = j;
                    }
                }
            }
            error += bestDist;
            
            //Update the weights based on the distance to the winning neuron
            //Neurons closer to the winning neuron will have their weights update more
            const Float bir = bestIndexRow;
            const Float bic = bestIndexCol;
            for(UINT i=0; i<numClusters; i++){  
                for(UINT j=0; j<numClusters; j++){
                
                    //Update the weights for all the neurons, pulling them a little closer to the input example
                    neuronDiff = 0;
                    gamma = 2.0 * grt_sqr( numClusters * sigmaWeight );
                    neuronWeightFunction = exp( -grt_sqr(bir-i)/gamma ) * exp( -grt_sqr(bic-j)/gamma );
                    //std::cout << "best index: " << bestIndexRow << " " << bestIndexCol << " bestDist: " << bestDist << " pos: " << i << " " << j << " neuronWeightFunction: " << neuronWeightFunction << std::endl;
                    for(UINT n=0; n<N; n++){
                        neuronDiff = trainingSample[n] - neurons[i][j][n];
                        weightUpdate = neuronWeightFunction * alpha * neuronDiff;
                        neurons[i][j][n] += weightUpdate;
                    }
                }
            }
        }

        error = error / M;

        trainingLog << "iter: " << iter << " average error: " << error << std::endl;
        
        //Compute the error
        delta = fabs( error-lastError );
        lastError = error;
        
        //Check to see if we should stop
        if( delta <= minChange && false ){
            converged = true;
            keepTraining = false;
        }
        
        if( grt_isinf( error ) ){
            errorLog << "train_(MatrixFloat &data) - Training failed! Error is NAN!" << std::endl;
            return false;
        }
        
        if( ++iter >= maxNumEpochs ){
            keepTraining = false;
        }
        
        trainingLog << "Epoch: " << iter << " Squared Error: " << error << " Delta: " << delta << " Alpha: " << alpha << std::endl;
    }
    
    numTrainingIterationsToConverge = iter;
    trained = true;
    
    return true;
}
    
bool SelfOrganizingMap::train_(ClassificationData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_(data);
}

bool SelfOrganizingMap::train_(UnlabelledData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_(data);
}
    
bool SelfOrganizingMap::map_( VectorFloat &x ){
    
    if( !trained ){
        return false;
    }
    
    if( useScaling ){
        for(UINT i=0; i<numInputDimensions; i++){
            x[i] = scale(x[i], ranges[i].minValue, ranges[i].maxValue, SOM_MIN_TARGET, SOM_MAX_TARGET);
        }
    }
    
    if( mappedData.getSize() != numClusters*numClusters )
        mappedData.resize( numClusters*numClusters );
    
    UINT index = 0;
    for(UINT i=0; i<numClusters; i++){
        for(UINT j=0; j<numClusters; j++){
            mappedData[index++] = neurons[i][j].fire( x );
        }
    }
    
    return true;
}
    
bool SelfOrganizingMap::save( std::fstream &file ) const{
    
    if( !trained ){
        errorLog << "save(fstream &file) - Can't save model to file, the model has not been trained!" << std::endl;
        return false;
    }
    
    file << "GRT_SELF_ORGANIZING_MAP_MODEL_FILE_V1.0\n";
    
    if( !saveClustererSettingsToFile( file ) ){
        errorLog << "save(fstream &file) - Failed to save cluster settings to file!" << std::endl;
        return false;
    }
    
    file << "NetworkTypology: " << networkTypology << std::endl;
    file << "AlphaStart: " << alphaStart << std::endl;
    file << "AlphaEnd: " << alphaEnd << std::endl;
    
    if( trained ){        
        file << "Neurons: \n";
        for(UINT i=0; i<neurons.getNumRows(); i++){
            for(UINT j=0; j<neurons.getNumCols(); j++){
                if( !neurons[i][j].save( file ) ){
                    errorLog << "save(fstream &file) - Failed to save neuron to file!" << std::endl;
                    return false;
                }
            }
        }
    }
    
    return true;
    
}

bool SelfOrganizingMap::load( std::fstream &file ){
    
    //Clear any previous model
    clear();
    
    std::string word;
    file >> word;
    if( word != "GRT_SELF_ORGANIZING_MAP_MODEL_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Failed to load file header!" << std::endl;
        return false;
    }
    
    if( !loadClustererSettingsFromFile( file ) ){
        errorLog << "load(fstream &file) - Failed to load cluster settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if( word != "NetworkTypology:" ){
        errorLog << "load(fstream &file) - Failed to load NetworkTypology header!" << std::endl;
        return false;
    }
    file >> networkTypology;
    
    file >> word;
    if( word != "AlphaStart:" ){
        errorLog << "load(fstream &file) - Failed to load AlphaStart header!" << std::endl;
        return false;
    }
    file >> alphaStart;
    
    file >> word;
    if( word != "AlphaEnd:" ){
        errorLog << "load(fstream &file) - Failed to load alphaEnd header!" << std::endl;
        return false;
    }
    file >> alphaEnd;
    
    //Load the model if it has been trained
    if( trained ){
        file >> word;
        if( word != "Neurons:" ){
            errorLog << "load(fstream &file) - Failed to load Neurons header!" << std::endl;
            return false;
        }
        
        neurons.resize(numClusters,numClusters);
        for(UINT i=0; i<neurons.getNumRows(); i++){
            for(UINT j=0; j<neurons.getNumCols(); j++){
                if( !neurons[i][j].load( file ) ){
                    errorLog << "load(fstream &file) - Failed to save neuron to file!" << std::endl;
                    return false;
                }
            }
        }
    }
    
    return true;
}
    
bool SelfOrganizingMap::validateNetworkTypology( const UINT networkTypology ){
    if( networkTypology == RANDOM_NETWORK ) return true;
    
    warningLog << "validateNetworkTypology(const UINT networkTypology) - Unknown networkTypology!" << std::endl;
    
    return false;
}
    
UINT SelfOrganizingMap::getNetworkSize() const{
    return numClusters;
}
    
Float SelfOrganizingMap::getAlphaStart() const{
    return alphaStart;
}

Float SelfOrganizingMap::getAlphaEnd() const{
    return alphaEnd;
}
    
VectorFloat SelfOrganizingMap::getMappedData() const{
    return mappedData;
}
    
Matrix< GaussNeuron > SelfOrganizingMap::getNeurons() const{
    return neurons;
}
    
const Matrix< GaussNeuron >& SelfOrganizingMap::getNeuronsRef() const{
    return neurons;
}

Matrix< VectorFloat > SelfOrganizingMap::getWeightsMatrix() const {
    if( !trained ) return Matrix< VectorFloat >();

    Matrix< VectorFloat > weights( numClusters, numClusters, VectorFloat(numInputDimensions) );
    for(UINT i=0; i<numClusters; i++){
        for(UINT j=0; j<numClusters; j++){
            for(UINT n=0; n<numInputDimensions; n++){
                weights[i][j][n] = neurons[i][j][n];
            }
        }
    }
    return weights;
}

bool SelfOrganizingMap::setNetworkSize( const UINT networkSize ){
    if( networkSize > 0 ){
        this->numClusters = networkSize;
        return true;
    }
    
    warningLog << "setNetworkSize(const UINT networkSize) - The networkSize must be greater than 0!" << std::endl;
    
    return false;
}
    
bool SelfOrganizingMap::setNetworkTypology( const UINT networkTypology ){
    if( validateNetworkTypology( networkTypology ) ){
        this->networkTypology = networkTypology;
        return true;
    }
    return false;
}
    
bool SelfOrganizingMap::setAlphaStart( const Float alphaStart ){
    
    if( alphaStart > 0 ){
        this->alphaStart = alphaStart;
        return true;
    }
    
    warningLog << "setAlphaStart(const Float alphaStart) - AlphaStart must be greater than zero!" << std::endl;
    
    return false;
}
    
bool SelfOrganizingMap::setAlphaEnd( const Float alphaEnd ){
    
    if( alphaEnd > 0 ){
        this->alphaEnd = alphaEnd;
        return true;
    }
    
    warningLog << "setAlphaEnd(const Float alphaEnd) - AlphaEnd must be greater than zero!" << std::endl;
    
    return false;
}

bool SelfOrganizingMap::setSigmaWeight( const Float sigmaWeight ){

    if( sigmaWeight > 0 ){
        this->sigmaWeight = sigmaWeight;
        return true;
    }
    
    warningLog << "setSigmaWeight(const Float sigmaWeight) - sigmaWeight must be greater than zero!" << std::endl;
    
    return false;
}

GRT_END_NAMESPACE

