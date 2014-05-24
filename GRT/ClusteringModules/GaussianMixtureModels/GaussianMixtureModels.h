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

namespace GRT {

class GaussianMixtureModels : public Clusterer
{
public:
    /**
     Default Constructor.
     */
	GaussianMixtureModels(const UINT numClusters=10,const UINT minNumEpochs=5,const UINT maxNumEpochs=1000,const double minChange=1.0e-5);
    
    /**
     Defines how the data from the rhs instance should be copied to this instance
     
     @param const GaussianMixtureModels &rhs: another instance of a GaussianMixtureModels
     */
    GaussianMixtureModels(const GaussianMixtureModels &rhs);
    
    /**
     Default Destructor
     */
	virtual ~GaussianMixtureModels();
    
    /**
     Defines how the data from the rhs instance should be copied to this instance
     
     @param const GaussianMixtureModels &rhs: another instance of a GaussianMixtureModels
     @return returns a reference to this instance of the GaussianMixtureModels
     */
    GaussianMixtureModels &operator=(const GaussianMixtureModels &rhs);
    
	/**
     This deep copies the variables and models from the Clusterer pointer to this GaussianMixtureModels instance.
     This overrides the base deep copy function for the Clusterer modules.
     
     @param const Clusterer *clusterer: a pointer to the Clusterer base class, this should be pointing to another GaussianMixtureModels instance
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
     This is the main training interface for referenced MatrixDouble data. It overrides the train_ function in the ML base class.
     
     @param MatrixDouble &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(MatrixDouble &data);
    
    /**
     This is the main training interface for reference ClassificationData data. It overrides the train_ function in the ML base class.
     
     @param ClassificationData &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This is the main training interface for reference UnlabelledData data. It overrides the train_ function in the ML base class.
     
     @param UnlabelledData &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
	virtual bool train_(UnlabelledData &trainingData);
    
	/**
     This saves the trained GaussianMixtureModels model to a file.
     This overrides the saveModelToFile function in the base class.
     
     @param string filename: the name of the file to save the GaussianMixtureModels model to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the trained GaussianMixtureModels model to a file.
     This overrides the saveModelToFile function in the base class.
     
     @param fstream &file: a reference to the file the GaussianMixtureModels model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained GaussianMixtureModels model from a file.
     This overrides the loadModelFromFile function in the base class.
     
     @param string filename: the name of the file to load the GaussianMixtureModels model from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads a trained GaussianMixtureModels model from a file.
     This overrides the loadModelFromFile function in the base class.
     
     @param fstream &file: a reference to the file the GaussianMixtureModels model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    /**
     This function returns the mu matrix which is built during the training phase.
     If the GMM model has not been trained, then this function will return an empty MatrixDouble.
     If the GMM model has been trained, then each row in the mu matrix represents a cluster and each column represents an input dimension.
     
     @return returns the mu matrix if the model has been trained, otherwise an empty MatrixDouble will be returned
     */
    MatrixDouble getMu() const { if( trained ){ return mu; } return MatrixDouble(); }
    
    /**
     This function returns the sigma matrix which is built during the training phase.
     If the GMM model has not been trained, then this function will return an empty vector< MatrixDouble >.
     If the GMM model has been trained, then each element in the returned vector represents a cluster.
     Each element is a MatrixDouble, which will have N rows and N columns, where N is the number of input dimensions to the model.
     
     @return returns the sigma matrix if the model has been trained, otherwise an empty vector< MatrixDouble > will be returned
     */
    vector< MatrixDouble > getSigma() const { if( trained ){ return sigma; } return vector< MatrixDouble >(); }
    
    /**
     This function returns the sigma matrix for a specific cluster.
     If the GMM model has not been trained, then this function will return an empty MatrixDouble.
     If the GMM model has been trained, then the returned MatrixDouble will have N rows and N columns, where N is the number of input dimensions to the model.
     
     @return returns the sigma matrix for a specific cluster if the model has been trained, otherwise an empty MatrixDouble will be returned
     */
    MatrixDouble getSigma(const UINT k) const{
        if( k < numClusters && trained ){
            return sigma[k];
        }
        return MatrixDouble();
    }
	
protected:
    bool estep( const MatrixDouble &data, VectorDouble &u, VectorDouble &v, double &change );
	bool mstep( const MatrixDouble &data );
	bool computeInvAndDet();
	inline void SWAP(UINT &a,UINT &b);
	inline double SQR(const double v){ return v*v; }
    
	UINT numTrainingSamples;                    ///< The number of samples in the training data
	double loglike;                             ///< The current loglikelihood value of the models given the data
	MatrixDouble mu;                            ///< A matrix holding the estimated mean values of each Gaussian
	MatrixDouble resp;                          ///< The responsibility matrix
	VectorDouble frac;                          ///< A vector holding the P(k)'s
	VectorDouble lndets;                        ///< A vector holding the log detminants of SIGMA'k
	VectorDouble det;                         
	vector< MatrixDouble > sigma;
	vector< MatrixDouble > invSigma;
    
private:
    static RegisterClustererModule< GaussianMixtureModels > registerModule;
};
    
}//End of namespace GRT

#endif //GRT_GAUSSIAN_MIXTURE_MODELS_HEADER

