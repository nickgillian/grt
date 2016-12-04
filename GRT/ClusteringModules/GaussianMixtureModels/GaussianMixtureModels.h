/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a Gaussian Miture Model clustering algorithm.  The code is based on the GMM code from Numerical Recipes (3rd Edition)
 
 @example ClusteringModulesExamples/GaussianMixtureModelsExample/GaussianMixtureModelsExample.cpp
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
#ifndef GRT_GAUSSIAN_MIXTURE_MODELS_HEADER
#define GRT_GAUSSIAN_MIXTURE_MODELS_HEADER

#include "../../CoreModules/Clusterer.h"

GRT_BEGIN_NAMESPACE

class GRT_API GaussianMixtureModels : public Clusterer
{
public:
    /**
     Default Constructor.
     */
	GaussianMixtureModels(const UINT numClusters=10,const UINT minNumEpochs=5,const UINT maxNumEpochs=1000,const Float minChange=1.0e-5,const UINT numRestarts=5);
    
    /**
     Defines how the data from the rhs instance should be copied to this instance
     
     @param rhs: another instance of a GaussianMixtureModels
     */
    GaussianMixtureModels(const GaussianMixtureModels &rhs);
    
    /**
     Default Destructor
     */
	virtual ~GaussianMixtureModels();
    
    /**
     Defines how the data from the rhs instance should be copied to this instance
     
     @param rhs: another instance of a GaussianMixtureModels
     @return returns a reference to this instance of the GaussianMixtureModels
     */
    GaussianMixtureModels &operator=(const GaussianMixtureModels &rhs);
    
	/**
     This deep copies the variables and models from the Clusterer pointer to this GaussianMixtureModels instance.
     This overrides the base deep copy function for the Clusterer modules.
     
     @param clusterer: a pointer to the Clusterer base class, this should be pointing to another GaussianMixtureModels instance
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
    
    
    virtual bool predict_(VectorDouble &inputVector);
    
    /**
     This saves the trained GaussianMixtureModels model to a file.
     This overrides the saveModelToFile function in the base class.
     
     @param fstream &file: a reference to the file the GaussianMixtureModels model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile( std::fstream &file ) const;
    
    /**
     This loads a trained GaussianMixtureModels model from a file.
     This overrides the loadModelFromFile function in the base class.
     
     @param fstream &file: a reference to the file the GaussianMixtureModels model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile( std::fstream &file );
    
    /**
     This function returns the mu matrix which is built during the training phase.
     If the GMM model has not been trained, then this function will return an empty MatrixFloat.
     If the GMM model has been trained, then each row in the mu matrix represents a cluster and each column represents an input dimension.
     
     @return returns the mu matrix if the model has been trained, otherwise an empty MatrixFloat will be returned
     */
    MatrixFloat getMu() const { if( trained ){ return mu; } return MatrixFloat(); }
    
    /**
     This function returns the sigma matrix which is built during the training phase.
     If the GMM model has not been trained, then this function will return an empty vector< MatrixFloat >.
     If the GMM model has been trained, then each element in the returned vector represents a cluster.
     Each element is a MatrixFloat, which will have N rows and N columns, where N is the number of input dimensions to the model.
     
     @return returns the sigma matrix if the model has been trained, otherwise an empty vector< MatrixFloat > will be returned
     */
    Vector< MatrixFloat > getSigma() const { if( trained ){ return sigma; } return Vector< MatrixFloat >(); }
    
    /**
     This function returns the sigma matrix for a specific cluster.
     If the GMM model has not been trained, then this function will return an empty MatrixFloat.
     If the GMM model has been trained, then the returned MatrixFloat will have N rows and N columns, where N is the number of input dimensions to the model.
     
     @return returns the sigma matrix for a specific cluster if the model has been trained, otherwise an empty MatrixFloat will be returned
     */
    MatrixFloat getSigma(const UINT k) const;

    /**
    Gets a string that represents the GaussianMixtureModels class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();

    /**
     The Gaussian Mixture Model learning algorithm can fail if the algorithm starts at a bad initial starting point (which is picked at random).
     To improve the robustness of the learning algorithm, the learning algorithm can be restarted from a new random location if it fails on the previous attempt. 
     The numRestarts parameter controls the maximum number of times the learning algorithm can reattempt to converge, a value of 0 indicates that the learning
     algorithm must converge on the first attempt, otherwise training will fail.
     
     @return returns true if the parameter was updated successfully, false otherwise
     */
    bool setNumRestarts(const UINT numRestarts);

    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::saveModelToFile;
    using MLBase::loadModelFromFile;
	
protected:
    bool train_( const UINT numTrainingSamples, const MatrixFloat &data );
    bool estep( const MatrixFloat &data, VectorDouble &u, VectorDouble &v, Float &change );
	bool mstep( const MatrixFloat &data );
	bool computeInvAndDet();
	inline void SWAP(UINT &a,UINT &b);
	inline Float SQR(const Float v){ return v*v; }
    
    Float gauss(const VectorDouble &x,const UINT clusterIndex,const VectorDouble &det,const MatrixFloat &mu,const Vector< MatrixFloat > &invSigma){
        
        Float y = 0;
        Float sum = 0;
        UINT i,j = 0;
        const UINT N = (UINT)x.size();
        VectorDouble temp(N,0);
        
        //Compute the first part of the equation
        y = (1.0/pow(TWO_PI,N/2.0)) * (1.0/pow(det[clusterIndex],0.5));
        
        //Compute the later half
        for(i=0; i<N; i++){
            for(j=0; j<N; j++){
                temp[i] += (x[j]-mu[clusterIndex][j]) * invSigma[clusterIndex][j][i];
            }
            sum += (x[i]-mu[clusterIndex][i]) * temp[i];
        }
        
        return ( y*exp( -0.5*sum ) );
    }
    
	UINT numTrainingSamples;                    ///< The number of samples in the training data
    UINT numRestarts;                           ///<The number of times the learning algorithm can reattempt to train a model
	Float loglike;                             ///< The current loglikelihood value of the models given the data
	MatrixFloat mu;                            ///< A matrix holding the estimated mean values of each Gaussian
	MatrixFloat resp;                          ///< The responsibility matrix
	VectorDouble frac;                          ///< A vector holding the P(k)'s
	VectorDouble lndets;                        ///< A vector holding the log detminants of SIGMA'k
	VectorDouble det;                         
	Vector< MatrixFloat > sigma;
	Vector< MatrixFloat > invSigma;
    
private:
    static RegisterClustererModule< GaussianMixtureModels > registerModule;
    static const std::string id;
};
    
GRT_END_NAMESPACE

#endif //GRT_GAUSSIAN_MIXTURE_MODELS_HEADER

