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
#include "Clusterer.h"

GRT_BEGIN_NAMESPACE
    
Clusterer::StringClustererMap* Clusterer::stringClustererMap = NULL;
UINT Clusterer::numClustererInstances = 0;

Clusterer* Clusterer::createNewInstance() const { return create(); } ///<Legacy function
Clusterer* Clusterer::createInstanceFromString(const std::string &id) { return create(id); } ///<Legacy function
    
Clusterer* Clusterer::create( std::string const &id ){
    
    StringClustererMap::iterator iter = getMap()->find( id );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}

Clusterer* Clusterer::create() const{
    return create( MLBase::getId() );
}
    
Clusterer* Clusterer::deepCopy() const{
    
    Clusterer *newInstance = create( MLBase::getId() );
    
    if( newInstance == NULL ) return NULL;
    
    if( !newInstance->deepCopyFrom( this ) ){
        delete newInstance;
        newInstance = NULL;
        return NULL;
    }
    return newInstance;
}

Vector< std::string > Clusterer::getRegisteredClusterers(){
	Vector< std::string > registeredClusterers;
	
	StringClustererMap::iterator iter = getMap()->begin();
	while( iter != getMap()->end() ){
		registeredClusterers.push_back( iter->first );
		iter++;
	}
	return registeredClusterers;
}
    
Clusterer::Clusterer( const std::string &id ) : MLBase( id, MLBase::CLUSTERER )
{
    numClusters = 10;
    predictedClusterLabel = 0;
    maxLikelihood = 0;
    bestDistance = 0;
    minNumEpochs = 1;
    maxNumEpochs = 1000;
    minChange = 1.0e-5;
    converged = false;
    numClustererInstances++;
}
    
Clusterer::~Clusterer(void){
    if( --numClustererInstances == 0 ){
        delete stringClustererMap;
        stringClustererMap = NULL;
    }
}
    
bool Clusterer::copyBaseVariables(const Clusterer *clusterer){
    
    if( clusterer == NULL ){
        errorLog << "copyBaseVariables(const Clusterer *clusterer) - clusterer is NULL!" << std::endl;
        return false;
    }
    
    if( !this->copyMLBaseVariables( clusterer ) ){
        return false;
    }
    
    //Copy the clusterer base variables
    this->numClusters = clusterer->numClusters;
    this->predictedClusterLabel = clusterer->predictedClusterLabel;
    this->predictedClusterLabel = clusterer->predictedClusterLabel;
    this->maxLikelihood = clusterer->maxLikelihood;
    this->bestDistance = clusterer->bestDistance;
    this->clusterLikelihoods = clusterer->clusterLikelihoods;
    this->clusterDistances = clusterer->clusterDistances;
    this->clusterLabels = clusterer->clusterLabels;
    this->converged = clusterer->converged;
    this->ranges = clusterer->ranges;
    
    return true;
}

bool Clusterer::train_(MatrixFloat &trainingData){
    return false;
}
    
bool Clusterer::train_(ClassificationData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}
    
bool Clusterer::train_(UnlabelledData &trainingData){
    MatrixFloat data = trainingData.getDataAsMatrixFloat();
    return train_( data );
}
    
bool Clusterer::reset(){
    
    //Reset the base class
    MLBase::reset();
    
    predictedClusterLabel = 0;
    maxLikelihood = 0;
    bestDistance = 0;
    std::fill(clusterLikelihoods.begin(),clusterLikelihoods.end(),0);
    std::fill(clusterDistances.begin(),clusterDistances.end(),0);
    
    return true;
}
    
bool Clusterer::clear(){
    
    //Clear the MLBase variables
    MLBase::clear();
    
    predictedClusterLabel = 0;
    maxLikelihood = 0;
    bestDistance = 0;
    clusterLikelihoods.clear();
    clusterDistances.clear();
    clusterLabels.clear();
    
    return true;
}
    
bool Clusterer::saveClustererSettingsToFile( std::fstream &file ) const{
	
    if( !file.is_open() ){
        errorLog << "saveClustererSettingsToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    file << "NumClusters: " << numClusters << std::endl;
	
	if( trained ){
    	file << "Ranges: " << std::endl;
    
    	for(UINT i=0; i<ranges.size(); i++){
        	file << ranges[i].minValue << "\t" << ranges[i].maxValue << std::endl;
    	}
	}
    
    return true;
}

bool Clusterer::loadClustererSettingsFromFile( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "loadClustererSettingsFromFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Try and load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile( file ) ){
        return false;
    }
    
    std::string word;
    
    //Load if the number of clusters
    file >> word;
    if( word != "NumClusters:" ){
        errorLog << "loadClustererSettingsFromFile(fstream &file) - Failed to read NumClusters header!" << std::endl;
        clear();
        return false;
    }
    file >> numClusters;
    
    //Load if the Ranges (if the model has been trained)
	if( trained ){
    	file >> word;
    	if( word != "Ranges:" ){
        	errorLog << "loadClustererSettingsFromFile(fstream &file) - Failed to read Ranges header!" << std::endl;
        	clear();
        	return false;
    	}
    	ranges.resize(numInputDimensions);
    
    	for(UINT i=0; i<ranges.size(); i++){
        	file >> ranges[i].minValue;
        	file >> ranges[i].maxValue;
    	}
        
        clusterLabels.resize(numClusters);
        for(UINT i=0; i<numClusters; i++){
            clusterLabels[i] = i+1;
        }
        
        clusterLikelihoods.resize(numClusters,0);
        clusterDistances.resize(numClusters,0);
        
	}
    
    return true;
}

std::string Clusterer::getClustererType() const { return MLBase::getId(); } //Legacy
    
bool Clusterer::getConverged() const{
    if( !trained ) return false;
    return converged;
}
    
UINT Clusterer::getNumClusters() const { return numClusters; }
    
UINT Clusterer::getPredictedClusterLabel() const { return predictedClusterLabel; }
    

Float Clusterer::getMaximumLikelihood() const{
    return maxLikelihood;
}

Float Clusterer::getBestDistance() const{
    return bestDistance;
}

VectorFloat Clusterer::getClusterLikelihoods() const{
    return clusterLikelihoods;
}

VectorFloat Clusterer::getClusterDistances() const{
    return clusterDistances;
}
    
Vector< UINT > Clusterer::getClusterLabels() const{
    return clusterLabels;
}
    
const Clusterer& Clusterer::getBaseClusterer() const{
    return *this;
}
    
bool Clusterer::setNumClusters(const UINT numClusters){
    if( numClusters == 0 ) return false;
    clear();
    this->numClusters = numClusters;
    return true;
}

GRT_END_NAMESPACE

