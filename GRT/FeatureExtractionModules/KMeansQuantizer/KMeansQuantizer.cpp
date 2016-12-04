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
#include "KMeansQuantizer.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string KMeansQuantizer::id = "KMeansQuantizer";
std::string KMeansQuantizer::getId() { return KMeansQuantizer::id; }

//Register your module with the FeatureExtraction base class
RegisterFeatureExtractionModule< KMeansQuantizer > KMeansQuantizer::registerModule( KMeansQuantizer::getId() );

KMeansQuantizer::KMeansQuantizer(const UINT numClusters) : FeatureExtraction( KMeansQuantizer::getId() )
{
    this->numClusters = numClusters;
}

KMeansQuantizer::KMeansQuantizer(const KMeansQuantizer &rhs) : FeatureExtraction( KMeansQuantizer::getId() )
{
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

KMeansQuantizer::~KMeansQuantizer(){
}

KMeansQuantizer& KMeansQuantizer::operator=(const KMeansQuantizer &rhs){
    if(this!=&rhs){
        //Copy any class variables from the rhs instance to this instance
        this->numClusters = rhs.numClusters;
        this->clusters = rhs.clusters;
        this->quantizationDistances = rhs.quantizationDistances;
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool KMeansQuantizer::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //Cast the feature extraction pointer to a pointer to your custom feature extraction module
        //Then invoke the equals operator
        *this = *dynamic_cast<const KMeansQuantizer*>(featureExtraction);
        
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}

bool KMeansQuantizer::computeFeatures(const VectorFloat &inputVector){
    
    //Run the quantize algorithm
    quantize( inputVector );
    
    return true;
}

bool KMeansQuantizer::reset(){
    
    FeatureExtraction::reset();
    
    if( trained ){
        std::fill(quantizationDistances.begin(),quantizationDistances.end(),0);
    }
    
    return true;
}

bool KMeansQuantizer::clear(){
    
    FeatureExtraction::clear();
    
    clusters.clear();
    quantizationDistances.clear();
    quantizationDistances.clear();
    
    return true;
}

bool KMeansQuantizer::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Save the header
    file << "KMEANS_QUANTIZER_FILE_V1.0" << std::endl;
    
    //Save the feature extraction base class settings
    if( !saveFeatureExtractionSettingsToFile( file ) ){
        errorLog << "save(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    //Save the KMeansQuantizer settings
    file << "QuantizerTrained: " << trained << std::endl;
    file << "NumClusters: " << numClusters << std::endl;
    
    if( trained ){
        file << "Clusters: \n";
        for(UINT k=0; k<numClusters; k++){
            for(UINT j=0; j<numInputDimensions; j++){
                file << clusters[k][j];
                if( j != numInputDimensions-1 ) file << "\t";
                else file << std::endl;
                }
        }
    }
    
    return true;
}

bool KMeansQuantizer::load( std::fstream &file ){
    
    //Clear any previouly built model and settings
    clear();
    
    if( !file.is_open() ){
        errorLog << "load(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //First, you should read and validate the header
    file >> word;
    if( word != "KMEANS_QUANTIZER_FILE_V1.0" ){
        errorLog << "load(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    //Second, you should load the base feature extraction settings to the file
    if( !loadFeatureExtractionSettingsFromFile( file ) ){
        errorLog << "loadFeatureExtractionSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if( word != "QuantizerTrained:" ){
        errorLog << "load(fstream &file) - Failed to load QuantizerTrained!" << std::endl;
        return false;
    }
    file >> trained;
    
    file >> word;
    if( word != "NumClusters:" ){
        errorLog << "load(fstream &file) - Failed to load NumClusters!" << std::endl;
        return false;
    }
    file >> numClusters;
    
    if( trained ){
        clusters.resize(numClusters, numInputDimensions);
        file >> word;
        if( word != "Clusters:" ){
            errorLog << "load(fstream &file) - Failed to load Clusters!" << std::endl;
            return false;
        }
        
        for(UINT k=0; k<numClusters; k++){
            for(UINT j=0; j<numInputDimensions; j++){
                file >> clusters[k][j];
            }
        }
        
        initialized = true;
        featureDataReady = false;
        quantizationDistances.resize(numClusters,0);
    }
    
    return true;
}

bool KMeansQuantizer::train_(ClassificationData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train( data );
}

bool KMeansQuantizer::train_(TimeSeriesClassificationData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train( data );
}

bool KMeansQuantizer::train_(ClassificationDataStream &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train( data );
}

bool KMeansQuantizer::train_(UnlabelledData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train( data );
}

bool KMeansQuantizer::train_(MatrixFloat &trainingData){
    
    //Clear any previous model
    clear();
    
    //Train the KMeans model
    KMeans kmeans;
    kmeans.setNumClusters(numClusters);
    kmeans.setComputeTheta( true );
    kmeans.setMinChange( minChange );
    kmeans.setMinNumEpochs( minNumEpochs );
    kmeans.setMaxNumEpochs( maxNumEpochs );
    
    if( !kmeans.train_(trainingData) ){
        errorLog << "train_(MatrixFloat &trainingData) - Failed to train quantizer!" << std::endl;
        return false;
    }
    
    trained = true;
    initialized = true;
    numInputDimensions = trainingData.getNumCols();
    numOutputDimensions = 1; //This is always 1 for the KMeansQuantizer
    featureVector.resize(numOutputDimensions,0);
    clusters = kmeans.getClusters();
    quantizationDistances.resize(numClusters,0);
    
    return true;
}

UINT KMeansQuantizer::quantize(const Float inputValue){
    return quantize( VectorFloat(1,inputValue) );
}

UINT KMeansQuantizer::quantize(const VectorFloat &inputVector){
    
    if( !trained ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - The quantizer has not been trained!" << std::endl;
        return 0;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.getSize() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return 0;
    }
    
    //Find the minimum cluster
    Float minDist = grt_numeric_limits< Float >::max();
    UINT quantizedValue = 0;
    
    for(UINT k=0; k<numClusters; k++){
        //Compute the squared Euclidean distance
        quantizationDistances[k] = 0;
        for(UINT i=0; i<numInputDimensions; i++){
            quantizationDistances[k] += grt_sqr( inputVector[i]-clusters[k][i] );
        }
        if( quantizationDistances[k] < minDist ){
            minDist = quantizationDistances[k];
            quantizedValue = k;
        }
    }
    
    featureVector[0] = quantizedValue;
    featureDataReady = true;
    
    return quantizedValue;
}

UINT KMeansQuantizer::getNumClusters() const{
    return numClusters;
}

bool KMeansQuantizer::setNumClusters(const UINT numClusters){
    clear();
    this->numClusters = numClusters;
    return true;
}

GRT_END_NAMESPACE
