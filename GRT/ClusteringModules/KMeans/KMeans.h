/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements the KMeans clustering algorithm.
 
 @example ClusteringModulesExamples/KMeansExample/KMeansExample.cpp
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

#ifndef GRT_KMEANS_HEADER
#define GRT_KMEANS_HEADER

#include "../../Util/GRTCommon.h"
#include "../../CoreModules/Clusterer.h"
#include "../../DataStructures/ClassificationData.h"
#include "../../DataStructures/UnlabelledData.h"

namespace GRT{

class KMeans : public Clusterer{

public:
	/**
     Default Constructor.
     */
	KMeans(const UINT numClusters=10,const UINT minNumEpochs=5,const UINT maxNumEpochs=1000,const double minChange=1.0e-5,const bool computeTheta=true);

    /**
     Defines how the data from the rhs KMeans should be copied to this KMeans
     
     @param const KMeans &rhs: another instance of a KMeans
     */
    KMeans(const KMeans &rhs);
    
    /**
     Default Destructor
     */
    virtual ~KMeans();
    
    /**
     Defines how the data from the rhs KMeans should be copied to this KMeans
     
     @param const KMeans &rhs: another instance of a KMeans
     @return returns a reference to this instance of the KMeans
     */
    KMeans &operator=(const KMeans &rhs);
    
	/**
     This deep copies the variables and models from the Clusterer pointer to this KMeans instance.
     This overrides the base deep copy function for the Clusterer modules.
     
     @param const Clusterer *clusterer: a pointer to the Clusterer base class, this should be pointing to another KMeans instance
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
     This is the main training algorithm for training a KMeans model. You should only call this function if you have manually set the clusters,
     otherwise you should use any of the train or train_ in functions.
     
     @param MatrixDouble &trainingData: the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    bool trainModel(MatrixDouble &data);
    
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
     This is the main training interface for reference UnlabelledData data. It overrides the trainInplace function in the ML base class.
     
     @param UnlabelledData &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
	virtual bool train_(UnlabelledData &trainingData);
    
	/**
     This saves the trained KMeans model to a file.
     This overrides the saveModelToFile function in the base class.
     
     @param string filename: the name of the file to save the KMeans model to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the trained KMeans model to a file.
     This overrides the saveModelToFile function in the base class.
     
     @param fstream &file: a reference to the file the KMeans model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained KMeans model from a file.
     This overrides the loadModelFromFile function in the base class.
     
     @param string filename: the name of the file to load the KMeans model from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads a trained KMeans model from a file.
     This overrides the loadModelFromFile function in the base class.
     
     @param fstream &file: a reference to the file the KMeans model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    //bool predict(VectorDouble inputVector,UINT &predictedClusterLabel,double &maxLikelihood,VectorDouble &clusterLikelihoods);

    //Getters
	double getTheta(){ return finalTheta; }
	bool getModelTrained(){ return trained; }

    VectorDouble getTrainingThetaLog() const{ return thetaTracker; }
    MatrixDouble getClusters() const{ return clusters; }
    vector< UINT > getClassLabelsVector() const{ return assign; }
    vector< UINT > getClassCountVector() const{ return count; }
    
    //Setters
    bool setComputeTheta(const bool computeTheta);
    
    /**
     This function lets you set the models clusters. You can use this to initalize the cluster values for the training algorithm.
     If you do that, then you should call the trainModel to run the training algorithm so the cluster values do not get reset.
     
     @param const MatrixDouble &clusters: the initial cluster values that will be used to train the KMeans model
     @return returns true if the model was loaded successfully, false otherwise
     */
    bool setClusters(const MatrixDouble &clusters);
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::saveModelToFile;
    using MLBase::loadModelFromFile;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;

protected:
    UINT estep(const MatrixDouble &data);
	void mstep(const MatrixDouble &data);
	double calculateTheta(const MatrixDouble &data);
	inline double SQR(const double a) {return a*a;};

    bool computeTheta;
    UINT numTrainingSamples;            ///<Number of training examples
	UINT nchg;                          ///<Number of values changes
	double finalTheta;
    MatrixDouble clusters;
	vector< UINT > assign, count;
    VectorDouble thetaTracker;
    
private:
    static RegisterClustererModule< KMeans > registerModule;
	
		
};
    
}//End of namespace GRT

#endif //GRT_KMEANS_HEADER
