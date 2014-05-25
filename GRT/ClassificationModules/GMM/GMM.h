/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements the Gaussian Mixture Model Classifier algorithm. The Gaussian Mixture Model 
 Classifier (GMM) is basic but useful classification algorithm that can be used to classify an N-dimensional signal.
 
 @remark This implementation is based on Duda, Richard O., and Peter E. Hart. Pattern classification and scene analysis. Vol. 3. New York: Wiley, 1973.
 
 @example ClassificationModulesExamples/GMMExample/GMMExample.cpp
 
 @note The GMM algorithm can fail to train on some occasions, if this happens just try and run the training algorithm
 again and it should eventially converge.
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

#ifndef GRT_GMM_HEADER
#define GRT_GMM_HEADER

#include "../../CoreModules/Classifier.h"
#include "../../ClusteringModules/GaussianMixtureModels/GaussianMixtureModels.h"
#include "MixtureModel.h"

#define GMM_MIN_SCALE_VALUE 0.0001
#define GMM_MAX_SCALE_VALUE 1

namespace GRT {

class GMM : public Classifier
{
public:
    /**
     Default Constructor. Sets the number of mixture models to use for each model. 
     */
	GMM(UINT numMixtureModels = 2,bool useScaling=false,bool useNullRejection=false,double nullRejectionCoeff=1.0,UINT maxIter=100,double minChange=1.0e-5);
    
    /**
     Defines the copy constructor.
     
     @param const GMM &rhs: the instance from which all the data will be copied into this instance
     */
    GMM(const GMM &rhs);
    
    /**
     Default destructor.
     */
	virtual ~GMM(void);
    
    /**
     Defines how the data from the rhs GMM should be copied to this GMM
     
     @param const GMM &rhs: another instance of a GMM
     @return returns a pointer to this instance of the GMM
     */
	GMM &operator=(const GMM &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier method is called.  
     It clones the data from the Base Class GRT::Classifier pointer (which should be pointing to an GMM instance) into this instance
     
     @param Classifier *classifier: a pointer to the GRT::Classifier Base Class, this should be pointing to another GMM instance
     @return returns true if the clone was successfull, false otherwise
     */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
     This trains the GMM model, using the labelled classification data.
     This overrides the train function in the GRT::Classifier base class.
     The GMM is an unsupervised learning algorithm, it will therefore NOT use any class labels provided
     
     @param ClassificationData trainingData: a reference to the training data
     @return returns true if the GMM model was trained, false otherwise
     */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the GRT::Classifier base class.
     
     @param VectorDouble inputVector: the input vector to classify
     @return returns true if the prediction was performed, false otherwise
     */
    virtual bool predict_(VectorDouble &inputVector);
    
    /**
     This overrides the clear function in the Classifier base class.
     It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
     
     @return returns true if the module was cleared succesfully, false otherwise
     */
    virtual bool clear();
    
    /**
     This saves the trained GMM model to a file.
     This overrides the saveModelToFile function in the GRT::Classifier base class.
     
     @param string filename: the name of the file to save the GMM model to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the trained GMM model to a file.
     This overrides the saveModelToFile function in the GRT::Classifier base class.
     
     @param fstream &file: a reference to the file the GMM model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained GMM model from a file.
     This overrides the loadModelFromFile function in the GRT::Classifier base class.
     
     @param string filename: the name of the file to load the GMM model from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads a trained GMM model from a file.
     This overrides the loadModelFromFile function in the GRT::Classifier base class.
     
     @param fstream &file: a reference to the file the GMM model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
   /**
    This function recomputes the null rejection thresholds for each model.
    This overrides the recomputeNullRejectionThresholds function in the GRT::Classifier base class.
    
    @return returns true if the nullRejectionThresholds were updated successfully, false otherwise
    */
    virtual bool recomputeNullRejectionThresholds();
    
    /**
     This function returns the number of mixture models.
     
     @return returns the number of mixture models in the GMM
     */
	UINT getNumMixtureModels();
    
    /**
     This function returns a copy of the MixtureModels estimated during the training phase. Each element in the
     vector represents a MixtureModel for one class.
     
     @return returns a vector of GRT::MixtureModel, an empty vector will be returned if the GRT::GMM has not been trained
     */
    vector< MixtureModel > getModels();
    
    /**
     This function sets the number of mixture models used for class. You should call this function before you train the GMM model.
     The number of mixture models must be greater than 0.
     
     @param UINT K: the number of mixture models
     @return returns true if the number of mixture models was successfully updated, false otherwise
     */
    bool setNumMixtureModels(UINT K);
    
    /**
     This function sets the minChange parameter which controls when the GMM train function should stop. MinChange must be greater than zero.
     
     @param double minChange: the new minChange value
     @return returns true if the number of minChange was successfully updated, false otherwise
     */
    bool setMinChange(double minChange);
    
    /**
     This function sets the maxIter parameter which controls when the maximum number of iterations parameter that controls when the GMM train 
     function should stop. MaxIter must be greater than zero.
     
     @param double maxIter: the new maxIter value
     @return returns true if the number of maxIter was successfully updated, false otherwise
     */
    bool setMaxIter(UINT maxIter);
    
    using MLBase::train; ///<Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::predict; ///<Tell the compiler we are using the base class predict method to stop hidden virtual function warnings
    
protected:
    double computeMixtureLikelihood(const VectorDouble &x,UINT k);
    bool loadLegacyModelFromFile( fstream &file );
    
    UINT numMixtureModels;
    UINT maxIter;
    double minChange;
    vector< MixtureModel > models;
    
    DebugLog debugLog;
    ErrorLog errorLog;
    WarningLog warningLog;
    
    static RegisterClassifierModule< GMM > registerModule;
	
};
    
}//End of namespace GRT

#endif //GRT_GMM_HEADER
