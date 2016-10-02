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
#include "HierarchicalClustering.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string HierarchicalClustering::id = "HierarchicalClustering";
std::string HierarchicalClustering::getId() { return HierarchicalClustering::id; }
    
//Register the HierarchicalClustering class with the Clusterer base class
RegisterClustererModule< HierarchicalClustering > HierarchicalClustering::registerModule( HierarchicalClustering::getId() );

HierarchicalClustering::HierarchicalClustering() : Clusterer( HierarchicalClustering::getId() )
{
    M = N = 0;
}
    
HierarchicalClustering::HierarchicalClustering(const HierarchicalClustering &rhs) : Clusterer( HierarchicalClustering::getId() )
{
    *this = rhs;
}

HierarchicalClustering::~HierarchicalClustering(){

}
    
HierarchicalClustering& HierarchicalClustering::operator=(const HierarchicalClustering &rhs){
    
    if( this != &rhs ){
        
        this->M = rhs.M;
        this->N = rhs.N;
        this->clusters = rhs.clusters;
        this->distanceMatrix = rhs.distanceMatrix;
        
        //Clone the Clusterer variables
        copyBaseVariables( (Clusterer*)&rhs );
    }
    
    return *this;
}
    
bool HierarchicalClustering::deepCopyFrom(const Clusterer *clusterer){
    
    if( clusterer == NULL ) return false;
    
    if( this->getId() == clusterer->getId() ){
        //Clone the HierarchicalClustering values
        const HierarchicalClustering *ptr = dynamic_cast<const HierarchicalClustering*>(clusterer);
        
        this->M = ptr->M;
        this->N = ptr->N;
        this->clusters = ptr->clusters;
        this->distanceMatrix = ptr->distanceMatrix;
        
        //Clone the Clusterer variables
        return copyBaseVariables( clusterer );
    }
    return false;
}

bool HierarchicalClustering::reset(){
    
    Clusterer::reset();
    
    return true;
}

bool HierarchicalClustering::clear(){
    
    Clusterer::clear();
    
    M = 0;
    N = 0;
    clusters.clear();
    distanceMatrix.clear();
    
    return true;
}

bool HierarchicalClustering::train_(ClassificationData &trainingData){
	
	if( trainingData.getNumSamples() == 0 ){
		return false;
	}

    //Convert the labelled training data to a training matrix
	M = trainingData.getNumSamples();
    N = trainingData.getNumDimensions();

    MatrixFloat data(M,N);
    for(UINT i=0; i<M; i++){
        for(UINT j=0; j<N; j++){
            data[i][j] = trainingData[i][j];
        }
    }

	return train_( data );
}

bool HierarchicalClustering::train_(UnlabelledData &trainingData){
    
    if( trainingData.getNumSamples() == 0 ){
		return false;
	}

    //Convert the training data into one matrix
	M = trainingData.getNumSamples();
    N = trainingData.getNumDimensions();

    MatrixFloat data(M,N);
    for(UINT i=0; i<M; i++){
        for(UINT j=0; j<N; j++){
            data[i][j] = trainingData[i][j];
        }
    }
	
	return train( data );
}

bool HierarchicalClustering::train_(MatrixFloat &data){
	
	trained = false;
    clusters.clear();
    distanceMatrix.clear();
    
    if( data.getNumRows() == 0 || data.getNumCols() == 0 ){
		return false;
	}
	
    //Set the rows and columns
    M = data.getNumRows();
	N = data.getNumCols();
    
    //Build the distance matrix
    distanceMatrix.resize(M,M);

    //Build the distance matrix
    for(UINT i=0; i<M; i++){
        for(UINT j=0; j<M; j++){
            if( i== j ) distanceMatrix[i][j] = grt_numeric_limits< Float >::max();
            else{
                distanceMatrix[i][j] = squaredEuclideanDistance(data[i], data[j]);
            }
        }
    }

    //Build the initial clusters, at the start each sample gets its own cluster
    UINT uniqueClusterID = 0;
    Vector< ClusterInfo > clusterData(M);
    for(UINT i=0; i<M; i++){
        clusterData[i].uniqueClusterID = uniqueClusterID++;
        clusterData[i].addSampleToCluster(i);
    }
    
    trainingLog << "Starting clustering..." << std::endl;
    
    //Create the first cluster level, each sample is it's own cluster
    UINT level = 0;
    ClusterLevel newLevel;
    newLevel.level = level;
    for(UINT i=0; i<M; i++){
        newLevel.clusters.push_back( clusterData[i] );
    }
    clusters.push_back( newLevel );
    
    //Move to level 1 and start the search
    level++;
    bool keepClustering = true;
    
    while( keepClustering ){
        
        //Find the closest two clusters within the cluster data
        Float minDist = grt_numeric_limits< Float >::max();
        Vector< Vector< UINT > > clusterPairs;
        UINT K = (UINT)clusterData.size();
        for(UINT i=0; i<K; i++){
            for(UINT j=0; j<K; j++){
                if( i != j ){
                    Float dist = computeClusterDistance( clusterData[i], clusterData[j]  );
             
                    if( dist < minDist ){
                        minDist = dist;
                        Vector< UINT > clusterPair(2);
                        clusterPair[0] = i;
                        clusterPair[1] = j;
                        clusterPairs.clear();
                        clusterPairs.push_back( clusterPair );
                    }
                    
                }
            }
        }
        
        if( minDist == grt_numeric_limits< Float >::max() ){
            keepClustering = false;
            warningLog << "train_(MatrixFloat &data) - Failed to find any cluster at level: " << level << std::endl;
            return false;
        }else{
        
            //Merge the two closest clusters together and create a new level
            ClusterLevel newLevel;
            newLevel.level = level;
            
            //Create the new cluster
            ClusterInfo newCluster;
            newCluster.uniqueClusterID = uniqueClusterID++;
            
            const UINT numClusterPairs = clusterPairs.getSize();
            
            for(UINT k=0; k<numClusterPairs; k++){
                //Add all the samples in the first cluster to the new cluster
                UINT numSamplesInClusterA = clusterData[ clusterPairs[k][0] ].getNumSamplesInCluster();
                for(UINT i=0; i<numSamplesInClusterA; i++){
                    UINT index = clusterData[ clusterPairs[k][0] ][ i ];
                    newCluster.addSampleToCluster( index );
                }
                
                //Add all the samples in the second cluster to the new cluster
                UINT numSamplesInClusterB = clusterData[ clusterPairs[k][1] ].getNumSamplesInCluster();
                for(UINT i=0; i<numSamplesInClusterB; i++){
                    UINT index = clusterData[ clusterPairs[k][1] ][ i ];
                    newCluster.addSampleToCluster( index );
                }
                
                //Compute the cluster variance
                newCluster.clusterVariance = computeClusterVariance( newCluster, data );
                
                //Remove the two cluster pairs (so they will not be used in the next search
                UINT idA = clusterData[ clusterPairs[k][0] ].getUniqueClusterID();
                UINT idB = clusterData[ clusterPairs[k][1] ].getUniqueClusterID();
                UINT numRemoved = 0;
                Vector< ClusterInfo >::iterator iter = clusterData.begin();
                while( iter != clusterData.end() ){
                    if( iter->getUniqueClusterID() == idA || iter->getUniqueClusterID() == idB ){
                        iter = clusterData.erase( iter );
                        if( ++numRemoved >= 2 ) break;
                    }else iter++;
                }
            }
            
            //Add the merged cluster to the clusterData
            clusterData.push_back( newCluster );
            
            //Add the new level and cluster data to the main cluster buffer
            newLevel.clusters.push_back( newCluster );
            
            clusters.push_back( newLevel );
            
            //Update the level
            level++;
        }
        
        //Check to see if we should stop clustering
        if( level >= M ){
            keepClustering = false;
        }
        
        if( clusterData.size() == 0 ){
            keepClustering = false;
        }
        
        trainingLog << "Cluster level: " << level << " Number of clusters: " << clusters.back().getNumClusters() << std::endl;
    }
    
    //Flag that the model is trained
    trained = true;
    
    //Setup the cluster labels
    clusterLabels.resize(numClusters);
    for(UINT i=0; i<numClusters; i++){
        clusterLabels[i] = i+1;
    }
    clusterLikelihoods.resize(numClusters,0);
    clusterDistances.resize(numClusters,0);

	return true;
}
    
bool HierarchicalClustering::printModel(){
    
    UINT K = (UINT)clusters.size();
    
    std::cout << "Hierarchical Clustering Model\n\n";
    for(UINT k=0; k<K; k++){
        UINT numClusters = clusters[k].getNumClusters();
        UINT numSamples = 0;
        for(UINT i=0; i<numClusters; i++){
            numSamples += clusters[k][i].getNumSamplesInCluster();
        }
        
        std::cout << "Level: " << clusters[k].level << "\tNumClusters: " << numClusters << "\tNumSamples: " << numSamples << std::endl;
        for(UINT i=0; i<numClusters; i++){
             std::cout << "ClusterVariance: " << clusters[k][i].clusterVariance << std::endl;
             std::cout << "Indexs: ";
            UINT numSamplesInCluster = clusters[k][i].getNumSamplesInCluster();
            for(UINT j=0; j<numSamplesInCluster; j++){
                 std::cout << clusters[k][i][j] << "\t";
            }
             std::cout << std::endl;
        }
    }

    return true;
}
    
Float HierarchicalClustering::squaredEuclideanDistance(const Float *a,const Float *b){
    Float dist = 0;
    for(UINT i=0; i<N; i++){
        dist += SQR( a[i] - b[i] );
    }
    return dist;
}
    
Float HierarchicalClustering::computeClusterDistance( const ClusterInfo &clusterA, const ClusterInfo &clusterB ){
    
    Float minDist = grt_numeric_limits< Float >::max();
    const UINT numSamplesA = clusterA.getNumSamplesInCluster();
    const UINT numSamplesB = clusterB.getNumSamplesInCluster();
    
    //Find the minimum distance between the two clusters
    for(UINT i=0; i<numSamplesA; i++){
        for(UINT j=0; j<numSamplesB; j++){
            if( distanceMatrix[ clusterA[i] ][ clusterB[j] ] < minDist ){
                minDist = distanceMatrix[ clusterA[i] ][ clusterB[j] ];
            }
        }
    }
    
    return minDist;
}
    
Float HierarchicalClustering::computeClusterVariance( const ClusterInfo &cluster, const MatrixFloat &data ){
    
    VectorFloat mean(N,0);
    VectorFloat std(N,0);
    
    //Compute the mean
    UINT numSamples = cluster.getNumSamplesInCluster();
    for(UINT j=0; j<N; j++){
        for(UINT i=0; i<numSamples; i++){
            UINT index = cluster[i];
            mean[j] += data[ index ][j];
        }
        mean[j] /= Float( numSamples );
    }
    
    //Compute the std dev
    for(UINT j=0; j<N; j++){
        for(UINT i=0; i<numSamples; i++){
            std[j] += grt_sqr( data[ cluster[i] ][j] - mean[j] );
        }
        std[j] = grt_sqrt( std[j] / Float( numSamples-1 ) );
    }
    
    Float variance = 0;
    for(UINT j=0; j<N; j++){
        variance += std[j];
    }
    return variance/N;
}

bool HierarchicalClustering::saveModelToFile( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(string filename) - Failed to open file!" << std::endl;
        return false;
    }
    
    file << "GRT_HIERARCHICAL_CLUSTERING_FILE_V1.0\n";
    
    if( !saveClustererSettingsToFile( file ) ){
        errorLog << "saveModelToFile(fstream &file) - Failed to save cluster settings to file!" << std::endl;
        return false;
    }
    
    if( trained ){
        file << "M: " << M << std::endl;
        file << "N: " << N << std::endl;
        file << "NumLevels: " << clusters.getSize() << std::endl;
        
        for(UINT i=0; i<clusters.getSize(); i++){
            file << "Level: " << clusters[i].getLevel() << std::endl;
            file << "NumClusters: " << clusters[i].getNumClusters() << std::endl;
        }
    }
    
    return true;
    
}

bool HierarchicalClustering::loadModelFromFile( std::fstream &file ){
    
    std::string word;
    
    //Clear any previous model
    clear();
    
    file >> word;
    if( word != "GRT_HIERARCHICAL_CLUSTERING_FILE_V1.0" ){
        return false;
    }
    
    if( !loadClustererSettingsFromFile( file ) ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to load cluster settings from file!" << std::endl;
        return false;
    }
        
    return true;
}

GRT_END_NAMESPACE
