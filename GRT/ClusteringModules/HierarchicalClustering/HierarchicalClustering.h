/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a basic Hierarchial Clustering algorithm.
 */

/**
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

#ifndef GRT_HIERARCHICAL_CLUSTERING_HEADER
#define GRT_HIERARCHICAL_CLUSTERING_HEADER

#include "../../CoreModules/Clusterer.h"

GRT_BEGIN_NAMESPACE
    
class GRT_API ClusterInfo{
public:
    ClusterInfo(){
        uniqueClusterID = 0;
        clusterVariance = 0;
    }
    ClusterInfo(const ClusterInfo &rhs){
        *this = rhs;
    }
    ~ClusterInfo(){
        
    }
    
    ClusterInfo& operator=(const ClusterInfo &rhs){
        
        if( this != &rhs ){
            this->uniqueClusterID = rhs.uniqueClusterID;
            this->clusterVariance = rhs.clusterVariance;
            this->indexs = rhs.indexs;
        }
        
        return *this;
    }
    
    UINT& operator[](const UINT &i){
        return indexs[i];
    }
    
    UINT operator[](const UINT &i) const{
        return indexs[i];
    }
    
    void addSampleToCluster(UINT i){
        indexs.push_back( i );
    }
    
    UINT getUniqueClusterID() const{
        return uniqueClusterID;
    }
    
    UINT getNumSamplesInCluster() const{
        return (UINT)indexs.size();
    }
    
    Float getClusterVariance() const{
        return clusterVariance;
    }
    
    unsigned int uniqueClusterID;
    Float clusterVariance;
    Vector< UINT > indexs;
};
    
class GRT_API ClusterLevel{
public:
    ClusterLevel(){
        level = 0;
    }
    
    ClusterLevel(const ClusterLevel &rhs){
        this->level = rhs.level;
        this->clusters = rhs.clusters;
    }
    
    ~ClusterLevel(){
        
    }
    
    ClusterLevel& operator=(const ClusterLevel &rhs){
        
        if( this != &rhs ){
            this->level = rhs.level;
            this->clusters = rhs.clusters;
        }
        
        return *this;
    }
    
    ClusterInfo& operator[](const UINT &i){
        return clusters[i];
    }
    
    ClusterInfo operator[](const UINT &i) const{
        return clusters[i];
    }
    
    UINT getLevel() const{
        return level;
    }
    
    UINT getNumClusters() const{
        return (UINT)clusters.size();
    }
    
    unsigned int level;
    Vector< ClusterInfo > clusters;
};

class GRT_API HierarchicalClustering : public Clusterer {

public:
	/**
     Default Constructor.
     */
	HierarchicalClustering();
    
    /**
     Defines how the data from the rhs instance should be copied to this instance
     
     @param rhs: another instance of a HierarchicalClustering
     */
    HierarchicalClustering(const HierarchicalClustering &rhs);
    
    /**
     Default Destructor
     */
    virtual ~HierarchicalClustering();
    
    /**
     Defines how the data from the rhs instance should be copied to this instance
     
     @param rhs: another instance of a HierarchicalClustering
     @return returns a reference to this instance of the HierarchicalClustering
     */
    HierarchicalClustering &operator=(const HierarchicalClustering &rhs);
    
	/**
     This deep copies the variables and models from the Clusterer pointer to this HierarchicalClustering instance.
     This overrides the base deep copy function for the Clusterer modules.
     
     @param clusterer: a pointer to the Clusterer base class, this should be pointing to another HierarchicalClustering instance
     @return returns true if the clone was successfull, false otherwise
     */
    virtual bool deepCopyFrom(const Clusterer *clusterer);
    
    /**
     This resets the Clusterer.
     This overrides the reset function in the MLBase base class.
     
     @return returns true if the Clusterer was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This function clears the Clusterer module, removing any trained model and setting all the base variables to their default values.
     
     @return returns true if the derived class was cleared succesfully, false otherwise
     */
    virtual bool clear();
    
    /**
     This is the main training interface for referenced MatrixFloat data. It overrides the train_ function in the ML base class.
     
     @param trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(MatrixFloat &trainingData);
    
    /**
     This is the main training interface for reference ClassificationData data. It overrides the train_ function in the ML base class.
     
     @param trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This is the main training interface for reference UnlabelledData data. It overrides the train_ function in the ML base class.
     
     @param trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
	virtual bool train_(UnlabelledData &trainingData);
    
    /**
     This saves the trained HierarchicalClustering model to a file.
     This overrides the saveModelToFile function in the base class.
     
     @param file: a reference to the file the HierarchicalClustering model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile( std::fstream &file ) const;
    
    /**
     This loads a trained HierarchicalClustering model from a file.
     This overrides the loadModelFromFile function in the base class.
     
     @param file: a reference to the file the HierarchicalClustering model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile( std::fstream &file );
    
    bool printModel();
    
    Vector< ClusterLevel > getClusters(){ return clusters; }
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::saveModelToFile;
    using MLBase::loadModelFromFile;

    /**
    Gets a string that represents the HierarchicalClustering class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();

protected:
	inline Float SQR(const Float &a) {return a*a;};
    Float squaredEuclideanDistance(const Float *a,const Float *b);
    Float computeClusterDistance( const ClusterInfo &clusterA, const ClusterInfo &clusterB );
    Float computeClusterVariance( const ClusterInfo &cluster, const MatrixFloat &data );

	UINT M;                             //Number of training examples
	UINT N;                             //Number of dimensions
    Vector< ClusterLevel > clusters;
    MatrixFloat distanceMatrix;                     //The distance matrix

private:
    static RegisterClustererModule< HierarchicalClustering > registerModule;
    static const std::string id;
		
};
    
GRT_END_NAMESPACE

#endif //GRT_HIERARCHICAL_CLUSTERING_HEADER
