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

#include "KMeansQuantizer.h"

namespace GRT{
    
//Register your module with the FeatureExtraction base class
RegisterFeatureExtractionModule< KMeansQuantizer > KMeansQuantizer::registerModule("KMeansQuantizer");
    
KMeansQuantizer::KMeansQuantizer(const UINT numClusters){
    
    this->numClusters = numClusters;
    classType = "KMeansQuantizer";
    featureExtractionType = classType;
    
    debugLog.setProceedingText("[DEBUG KMeansQuantizer]");
    errorLog.setProceedingText("[ERROR KMeansQuantizer]");
    warningLog.setProceedingText("[WARNING KMeansQuantizer]");
}
    
KMeansQuantizer::KMeansQuantizer(const KMeansQuantizer &rhs){
    
    classType = "KMeansQuantizer";
    featureExtractionType = classType;
    
    debugLog.setProceedingText("[DEBUG KMeansQuantizer]");
    errorLog.setProceedingText("[ERROR KMeansQuantizer]");
    warningLog.setProceedingText("[WARNING KMeansQuantizer]");
    
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
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}
    
bool KMeansQuantizer::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getFeatureExtractionType() == featureExtraction->getFeatureExtractionType() ){
        
        //Cast the feature extraction pointer to a pointer to your custom feature extraction module
        //Then invoke the equals operator
        *this = *(KMeansQuantizer*)featureExtraction;
        
        return true;
    }
    
    errorLog << "clone(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << endl;
    
    return false;
}
    
bool KMeansQuantizer::computeFeatures(const VectorDouble &inputVector){
    
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
    
    return true;
}
    
bool KMeansQuantizer::saveModelToFile(const string filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }
    
    file.close();
    
    return true;
}

bool KMeansQuantizer::loadModelFromFile(const string filename){
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool KMeansQuantizer::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //First, you should add a header (with no spaces) e.g.
    file << "KMEANS_QUANTIZER_FILE_V1.0" << endl;
	
    //Second, you should save the base feature extraction settings to the file
    if( !saveBaseSettingsToFile( file ) ){
        errorLog << "saveModelToFile(fstream &file) - Failed to save base feature extraction settings to file!" << endl;
        return false;
    }
    
    file << "QuantizerTrained: " << trained << endl;
    file << "NumClusters: " << numClusters << endl;
    
    if( trained ){
        file << "Clusters: \n";
        for(UINT k=0; k<numClusters; k++){
            for(UINT j=0; j<numInputDimensions; j++){
                file << clusters[k][j];
                if( j != numInputDimensions-1 ) file << "\t";
                else file << endl;
            }
        }
    }
    
    return true;
}

bool KMeansQuantizer::loadModelFromFile(fstream &file){
    
    initialized = false;
    numClusters = 0;
    clusters.clear();
    quantizationDistances.clear();
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //First, you should read and validate the header
    file >> word;
    if( word != "KMEANS_QUANTIZER_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;
    }
    
    //Second, you should load the base feature extraction settings to the file
    if( !loadBaseSettingsFromFile( file ) ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if( word != "QuantizerTrained:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load QuantizerTrained!" << endl;
        return false;
    }
    file >> trained;
    
    file >> word;
    if( word != "NumClusters:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load NumClusters!" << endl;
        return false;
    }
    file >> numClusters;
    
    if( trained ){
        clusters.resize(numClusters, numInputDimensions);
        file >> word;
        if( word != "Clusters:" ){
            errorLog << "loadModelFromFile(fstream &file) - Failed to load Clusters!" << endl;
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
    MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train( data );
}
    
bool KMeansQuantizer::train_(TimeSeriesClassificationData &trainingData){
    MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train( data );
}
   
bool KMeansQuantizer::train_(TimeSeriesClassificationDataStream &trainingData){
    MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train( data );
}

bool KMeansQuantizer::train_(UnlabelledData &trainingData){
	MatrixDouble data = trainingData.getDataAsMatrixDouble();
    return train( data );
}
    
bool KMeansQuantizer::train_(MatrixDouble &trainingData){
    
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
        errorLog << "train_(MatrixDouble &trainingData) - Failed to train quantizer!" << endl;
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

UINT KMeansQuantizer::quantize(double inputValue){
	return quantize( VectorDouble(1,inputValue) );
}

UINT KMeansQuantizer::quantize(const VectorDouble &inputVector){
	
    if( !trained ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - The quantizer has not been trained!" << endl;
        return 0;
    }

    if( inputVector.size() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return 0;
    }

	//Find the minimum cluster
    double minDist = numeric_limits<double>::max();
    UINT quantizedValue = 0;
    
    for(UINT k=0; k<numClusters; k++){
        //Compute the squared Euclidean distance
        quantizationDistances[k] = 0;
        for(UINT i=0; i<numInputDimensions; i++){
            quantizationDistances[k] += SQR( inputVector[i]-clusters[k][i] );
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
    
}//End of namespace GRT