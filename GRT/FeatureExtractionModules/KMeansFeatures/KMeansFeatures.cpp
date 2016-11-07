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
#include "KMeansFeatures.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string KMeansFeatures::id = "KMeansFeatures";
std::string KMeansFeatures::getId() { return KMeansFeatures::id; }

//Register your module with the FeatureExtraction base class
RegisterFeatureExtractionModule< KMeansFeatures > KMeansFeatures::registerModule( KMeansFeatures::getId() );

KMeansFeatures::KMeansFeatures(const Vector< UINT > numClustersPerLayer,const Float alpha,const bool useScaling) : FeatureExtraction( KMeansFeatures::getId() )
{
    this->numClustersPerLayer = numClustersPerLayer;
    this->alpha = alpha;
    this->useScaling = useScaling;
    
    if( numClustersPerLayer.size() > 0 ){
        init( numClustersPerLayer );
    }
}

KMeansFeatures::KMeansFeatures(const KMeansFeatures &rhs) : FeatureExtraction( KMeansFeatures::getId() )
{
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

KMeansFeatures::~KMeansFeatures(){
    //Here you should add any specific code to cleanup your custom feature extraction module if needed
}

KMeansFeatures& KMeansFeatures::operator=(const KMeansFeatures &rhs){
    if(this!=&rhs){
        //Here you should copy any class variables from the rhs instance to this instance
        this->numClustersPerLayer = rhs.numClustersPerLayer;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool KMeansFeatures::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //Cast the feature extraction pointer to a pointer to your custom feature extraction module
        //Then invoke the equals operator
        *this = *dynamic_cast<const KMeansFeatures*>(featureExtraction);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool KMeansFeatures::computeFeatures(const VectorFloat &inputVector){
    
    VectorFloat data( numInputDimensions );
    
    //Scale the input data if needed, if not just copy it
    if( useScaling ){
        for(UINT j=0; j<numInputDimensions; j++){
            data[j] = scale(inputVector[j],ranges[j].minValue,ranges[j].maxValue,0,1);
        }
    }else{
        for(UINT j=0; j<numInputDimensions; j++){
            data[j] = inputVector[j];
        }
    }
    
    const UINT numLayers = getNumLayers();
    for(UINT layer=0; layer<numLayers; layer++){
        if( !projectDataThroughLayer(data, featureVector, layer) ){
            errorLog << "computeFeatures(const VectorFloat &inputVector) - Failed to project data through layer: " << layer << std::endl;
            return false;
        }
        
        //The output of the current layer will become the input to the next layer unless it is the last layer
        if( layer+1 < numLayers ){
            data = featureVector;
        }
    }
    
    return true;
}

bool KMeansFeatures::reset(){
    return true;
}

bool KMeansFeatures::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //First, you should add a header (with no spaces) e.g.
    file << "KMEANS_FEATURES_FILE_V1.0" << std::endl;
    
    //Second, you should save the base feature extraction settings to the file
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "save(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    file << "NumLayers: " << getNumLayers() << std::endl;
    file << "NumClustersPerLayer: ";
    for(UINT i=0; i<numClustersPerLayer.getSize(); i++){
        file << " " << numClustersPerLayer[i];
    }
    file << std::endl;
    
    file << "Alpha: " << alpha << std::endl;
    
    if( trained ){
        file << "Ranges: ";
        for(UINT i=0; i<ranges.getSize(); i++){
            file << ranges[i].minValue << " " << ranges[i].maxValue << " ";
        }
        file << std::endl;
        
        file << "Clusters: " << std::endl;
        for(UINT k=0; k<clusters.getSize(); k++){
            file << "NumRows: " << clusters[k].getNumRows() << std::endl;
            file << "NumCols: " << clusters[k].getNumCols() << std::endl;
            for(UINT i=0; i<clusters[k].getNumRows(); i++){
                for(UINT j=0; j<clusters[k].getNumCols(); j++){
                    file << clusters[k][i][j];
                    if( j+1 < clusters[k].getNumCols() )
                    file << "\t";
                }
                file << std::endl;
            }
        }
    }
    
    return true;
}

bool KMeansFeatures::load( std::fstream &file ){
    
    clear();
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    UINT numLayers = 0;
    UINT numRows = 0;
    UINT numCols = 0;
    
    //First, you should read and validate the header
    file >> word;
    if( word != "KMEANS_FEATURES_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    //Second, you should load the base feature extraction settings to the file
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "load(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    //Load the number of layers
    file >> word;
    if( word != "NumLayers:" ){
        errorLog << "load(fstream &file) - Failed to read NumLayers header!" << std::endl;
        return false;
    }
    file >> numLayers;
    numClustersPerLayer.resize( numLayers );
    
    //Load the number clusters per layer
    file >> word;
    if( word != "NumClustersPerLayer:" ){
        errorLog << "load(fstream &file) - Failed to read NumClustersPerLayer header!" << std::endl;
        return false;
    }
    for(UINT i=0; i<numClustersPerLayer.getSize(); i++){
        file >> numClustersPerLayer[i];
    }
    
    //Load the alpha parameter
    file >> word;
    if( word != "Alpha:" ){
        errorLog << "load(fstream &file) - Failed to read Alpha header!" << std::endl;
        return false;
    }
    file >> alpha;
    
    //If the model has been trained then load it
    if( trained ){
        
        //Load the Ranges
        file >> word;
        if( word != "Ranges:" ){
            errorLog << "load(fstream &file) - Failed to read Ranges header!" << std::endl;
            return false;
        }
        ranges.resize(numInputDimensions);
        for(UINT i=0; i<ranges.size(); i++){
            file >> ranges[i].minValue;
            file >> ranges[i].maxValue;
        }
        
        //Load the Clusters
        file >> word;
        if( word != "Clusters:" ){
            errorLog << "load(fstream &file) - Failed to read Clusters header!" << std::endl;
            return false;
        }
        clusters.resize( numLayers );
        
        for(UINT k=0; k<clusters.size(); k++){
            
            //Load the NumRows
            file >> word;
            if( word != "NumRows:" ){
                errorLog << "load(fstream &file) - Failed to read NumRows header!" << std::endl;
                return false;
            }
            file >> numRows;
            
            //Load the NumCols
            file >> word;
            if( word != "NumCols:" ){
                errorLog << "load(fstream &file) - Failed to read NumCols header!" << std::endl;
                return false;
            }
            file >> numCols;
            
            clusters[k].resize(numRows, numCols);
            for(UINT i=0; i<clusters[k].getNumRows(); i++){
                for(UINT j=0; j<clusters[k].getNumCols(); j++){
                    file >> clusters[k][i][j];
                }
            }
        }
    }
    
    return true;
}

bool KMeansFeatures::init( const Vector< UINT > numClustersPerLayer ){
    
    clear();
    
    if( numClustersPerLayer.size() == 0 ) return false;
    
    this->numClustersPerLayer = numClustersPerLayer;
    numInputDimensions = 0; //This will be 0 until the KMeansFeatures has been trained
    numOutputDimensions = 0; //This will be 0 until the KMeansFeatures has been trained
    
    //Flag that the feature extraction has been initialized but not trained
    initialized = true;
    trained = false;
    
    return true;
}

bool KMeansFeatures::train_(ClassificationData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}

bool KMeansFeatures::train_(TimeSeriesClassificationData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}

bool KMeansFeatures::train_(ClassificationDataStream &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}

bool KMeansFeatures::train_(UnlabelledData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}

bool KMeansFeatures::train_(MatrixFloat &trainingData){
    
    if( !initialized ){
        errorLog << "train_(MatrixFloat &trainingData) - The quantizer has not been initialized!" << std::endl;
        return false;
    }
    
    //Reset any previous model
    featureDataReady = false;
    
    const UINT M = trainingData.getNumRows();
    const UINT N = trainingData.getNumCols();
    
    numInputDimensions = N;
    numOutputDimensions = numClustersPerLayer[ numClustersPerLayer.getSize()-1 ];
    
    //Scale the input data if needed
    ranges = trainingData.getRanges();
    if( useScaling ){
        for(UINT i=0; i<M; i++){
            for(UINT j=0; j<N; j++){
                trainingData[i][j] = grt_scale(trainingData[i][j],ranges[j].minValue,ranges[j].maxValue,0.0,1.0);
            }
        }
    }
    
    //Train the KMeans model at each layer
    const UINT K = numClustersPerLayer.getSize();
    for(UINT k=0; k<K; k++){
        KMeans kmeans;
        kmeans.setNumClusters( numClustersPerLayer[k] );
        kmeans.setComputeTheta( true );
        kmeans.setMinChange( minChange );
        kmeans.setMinNumEpochs( minNumEpochs );
        kmeans.setMaxNumEpochs( maxNumEpochs );
        
        trainingLog << "Layer " << k+1 << "/" << K << " NumClusters: " << numClustersPerLayer[k] << std::endl;
        if( !kmeans.train_( trainingData ) ){
            errorLog << "train_(MatrixFloat &trainingData) - Failed to train kmeans model at layer: " << k << std::endl;
            return false;
        }
        
        //Save the clusters
        clusters.push_back( kmeans.getClusters() );
        
        //Project the data through the current layer to use as training data for the next layer
        if( k+1 != K ){
            MatrixFloat data( M, numClustersPerLayer[k] );
            VectorFloat input( trainingData.getNumCols() );
            VectorFloat output( data.getNumCols() );
            
            for(UINT i=0; i<M; i++){
                
                //Copy the data into the sample
                for(UINT j=0; j<input.getSize(); j++){
                    input[j] = trainingData[i][j];
                }
                
                //Project the sample through the current layer
                if( !projectDataThroughLayer( input, output, k ) ){
                    errorLog << "train_(MatrixFloat &trainingData) - Failed to project sample through layer: " << k << std::endl;
                    return false;
                }
                
                //Copy the result into the training data for the next layer
                for(UINT j=0; j<output.getSize(); j++){
                    data[i][j] = output[j];
                }
            }
            
            //Swap the data for the next layer
            trainingData = data;
        }
        
    }
    
    //Flag that the kmeans model has been trained
    trained = true;
    featureVector.resize( numOutputDimensions, 0 );
    
    return true;
}

bool KMeansFeatures::projectDataThroughLayer( const VectorFloat &input, VectorFloat &output, const UINT layer ){
    
    if( layer >= clusters.getSize() ){
        errorLog << "projectDataThroughLayer(...) - Layer out of bounds! It should be less than: " << clusters.getSize() << std::endl;
        return false;
    }
    
    const UINT M = clusters[ layer ].getNumRows();
    const UINT N = clusters[ layer ].getNumCols();
    
    if( input.getSize() != N ){
        errorLog << "projectDataThroughLayer(...) - The size of the input Vector (" << input.getSize() << ") does not match the size: " << N << std::endl;
        return false;
    }
    
    //Make sure the output Vector size is OK
    if( output.getSize() != M ){
        output.resize( M );
    }
    
    UINT i,j = 0;
    //Float gamma = 2.0*SQR(alpha);
    //Float gamma = 2.0*SQR( 1 );
    for(i=0; i<M; i++){
        output[i] = 0;
        for(j=0; j<N; j++){
            output[i] += grt_sqr( input[j] - clusters[layer][i][j] );
            //output[i] += input[j] * clusters[layer][i][j];
        }
        //cout << "i: " << i << " sum: " << output[i] << " output: " << 1.0/(1.0+exp(-output[i])) << std::endl;
        //cout << "i: " << i << " sum: " << output[i] << " output: " << exp( -output[i] / gamma ) << std::endl;
        //output[i] = exp( -output[i] / gamma );
        //output[i] = 1.0/(1.0+exp(-output[i]));
        output[i] = grt_sqrt( output[i] ); //L2 Norm
        
    }
    
    return true;
}

UINT KMeansFeatures::getNumLayers() const{
    return numClustersPerLayer.getSize();
}

UINT KMeansFeatures::getLayerSize(const UINT layerIndex) const{
    if( layerIndex >= numClustersPerLayer.getSize() ){
        warningLog << "LayerIndex is out of bounds. It must be less than the number of layers: " << numClustersPerLayer.getSize() << std::endl;
        return 0;
    }
    return numClustersPerLayer[layerIndex];
}

Vector< MatrixFloat > KMeansFeatures::getClusters() const{
    return clusters;
}

GRT_END_NAMESPACE
