/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class acts as the main interface for using a Hidden Markov Model.
 
 @remark This implementation is based on Rabiner, Lawrence. "A tutorial on hidden Markov models and selected applications in speech recognition." Proceedings of the IEEE 77.2 (1989): 257-286.
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

#ifndef GRT_HMM_HEADER
#define GRT_HMM_HEADER

#include "HiddenMarkovModel.h"
#include "../../CoreModules/Classifier.h"

namespace GRT{

class HMM : public Classifier
{
public:
	HMM(UINT numStates=5,UINT numSymbols=10,UINT modelType=HiddenMarkovModel::LEFTRIGHT,UINT delta=1,UINT maxNumIter=100,double minImprovement=1.0e-2,bool useNullRejection = false);
    
    HMM(const HMM &rhs);
    
	virtual ~HMM(void);
    
    HMM& operator=(const HMM &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.  
     It clones the data from the Base Class Classifier pointer (which should be pointing to an HMM instance) into this instance
     
     @param Classifier *classifier: a pointer to the Classifier Base Class, this should be pointing to another HMM instance
     @return returns true if the clone was successfull, false otherwise
     */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
     This overrides the train function in the Classifier base class. It simply prints a warning message stating that the 
     bool train(LabelledTimeSeriesClassificationData trainingData) function should be used to train the HMM model.
     
     @param ClassificationData trainingData: a reference to the training data
     @return returns true if the HMM model was trained, false otherwise
     */
    virtual bool train(ClassificationData trainingData);
    
    /**
     This trains the HMM model, using the labelled timeseries classification data.
     This overrides the train function in the Classifier base class.
     
     @param TimeSeriesClassificationData trainingData: a reference to the training data
     @return returns true if the HMM model was trained, false otherwise
     */
    virtual bool train_(TimeSeriesClassificationData &trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the Classifier base class.
     
     @param VectorDouble inputVector: the input vector to classify
     @return returns true if the prediction was performed, false otherwise
     */
    virtual bool predict_(VectorDouble &inputVector);
    
    /**
     This predicts the class of the timeseries.
     This overrides the predict function in the Classifier base class.
     
     @param MatrixDouble timeSeries: the input timeseries to classify
     @return returns true if the prediction was performed, false otherwise
     */
    virtual bool predict_(MatrixDouble &timeseries);
    
    /**
     This resets the HMM classifier.
     
     @return returns true if the HMM model was successfully reset, false otherwise.
     */
    virtual bool reset();
    
    /**
     This overrides the clear function in the Classifier base class.
     It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
     
     @return returns true if the module was cleared succesfully, false otherwise
     */
    virtual bool clear();
    
    /**
     This saves the trained HMM model to a file.
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param string filename: the name of the file to save the HMM model to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the trained HMM model to a file.
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the HMM model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained HMM model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param string filename: the name of the file to load the HMM model from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads a trained HMM model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the HMM model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    /**
     This function gets the number of states in each HMM.
     
     @return returns the number of states in each HMM
     */
    UINT getNumStates() const;
    
    /**
     This function gets the number of symbols in each HMM.
     
     @return returns the number of symbols in each HMM
     */
    UINT getNumSymbols() const;
    
    /**
     This function gets the model type for each HMM, this will be one of the HMM enum ModelTypes.
     
     @return returns the model type for each HMM
     */
    UINT getModelType() const;
    
    /**
     This function gets the delta value used for each HMM.  The delta value controls how many states a model can transition to
     if the LEFTRIGHT model type is used.
     
     @return returns the delta parameter for each HMM
     */
    UINT getDelta() const;
    
    /**
     This function gets the maximum number of iterations used to train each HMM.  
     
     @return returns the maximum number of iterations used to trained each HMM
     */
    UINT getMaxNumIterations() const;
    
    /**
     This function gets the number of random training iterations used to train each HMM.
     
     The accuracy of the Baum Welch algorithm can be effected by the starting values of the A, B, and PI matrices.  The HMM
     algorithm therefore tries several different starting values and then continues to train the algorithm with the best settings.
     
     The number of random training iterations parameter therefore controls the number of different starting values.
     
     @return returns the maximum number of iterations used to trained each HMM
     */
    UINT getNumRandomTrainingIterations() const;
    
    /**
     This function gets the minimum improvement parameter which controls when the HMM training algorithm should stop.
     
     @return returns the minimum improvment parameter
     */
    double getMinImprovement() const;
    
    /**
     This function gets returns a vector of trained HiddenMarkovModels.  There will be one HiddenMarkovModel for each class in
     the training data.
     
     @return returns the trained HiddenMarkovModels
     */
    vector< HiddenMarkovModel > getModels() const;
    
    /**
     This function sets the number of states in each HMM.  The parameter must be greater than zero.
     
     This will clear any trained model.
     
     @param const UINT numStates: the number of states in each HMM
     @return returns true if the parameter was set correctly, false otherwise
     */
    bool setNumStates(const UINT numStates);
    
    /**
     This function sets the number of symbols in each HMM.  The parameter must be greater than zero.
     
     This will clear any trained model.
     
     @param const UINT symbols: the number of symbols in each HMM
     @return returns true if the parameter was set correctly, false otherwise
     */
    bool setNumSymbols(const UINT numStates);
    
    /**
     This function sets the modelType used for each HMM.  This should be one of the HMM modelType enums.
     
     This will clear any trained model.
     
     @param const UINT modelType: the modelType in each HMM
     @return returns true if the parameter was set correctly, false otherwise
     */
    bool setModelType(const UINT modelType);
    
    /**
     This function sets the delta parameter in each HMM.
     
     The delta value controls how many states a model can transition to if the LEFTRIGHT model type is used.
     
     The parameter must be greater than zero.
     
     This will clear any trained model.
     
     @param const UINT delta: the delta parameter used for each HMM
     @return returns true if the parameter was set correctly, false otherwise
     */
    bool setDelta(const UINT delta);
    
    /**
     This function sets the maximum number of iterations used to train each HMM.  The parameter must be greater than zero.
     
     This will clear any trained model.
     
     @param const UINT maxNumIter: the maximum number of iterations used to train each HMM
     @return returns true if the parameter was set correctly, false otherwise
     */
    bool setMaxNumIterations(const UINT maxNumIter);
    
    /**
     This function sets the number of random training iterations used to train each HMM.
     
     The accuracy of the Baum Welch algorithm can be effected by the starting values of the A, B, and PI matrices.  The HMM
     algorithm therefore tries several different starting values and then continues to train the algorithm with the best settings.
     The number of random training iterations parameter therefore controls the number of different starting values.
     
     This will clear any trained model.
     
     @param const UINT numRandomTrainingIterations: the number of random training iterations used to train each HMM
     @return returns true if the parameter was set correctly, false otherwise
     */
    bool setNumRandomTrainingIterations(const UINT numRandomTrainingIterations);
    
    /**
     This function sets the minimum improvement parameter which controls when the HMM training algorithm should stop.
     
     This will clear any trained model.
     
     @param const double minImprovement: the minimum improvement parameter which controls when the HMM training algorithm should stop
     @return returns true if the parameter was set correctly, false otherwise
     */
    bool setMinImprovement(const double minImprovement);
    
    using MLBase::train; ///<Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::predict; ///<Tell the compiler we are using the base class predict method to stop hidden virtual function warnings

protected:
    bool convertDataToObservationSequence( TimeSeriesClassificationData &classData, vector< vector< UINT > > &observationSequences );
    bool loadLegacyModelFromFile( fstream &file );

	//Variables for all the HMMs
	UINT numStates;			//The number of states for each model
	UINT numSymbols;		//The number of symbols for each model
	UINT modelType;         //Set if the model is ERGODIC or LEFTRIGHT
	UINT delta;				//The number of states a model can move to in a LeftRight model
	UINT maxNumIter;		//The maximum number of iter allowed during the full training
    UINT numRandomTrainingIterations; 
	double minImprovement;  //The minimum improvement value for each model during training
    
    vector< HiddenMarkovModel > models;
    
    static RegisterClassifierModule< HMM > registerModule;
    
public:
    enum ModelTypes{ERGODIC=0,LEFTRIGHT=1};
};
    
}//End of namespace GRT

#endif //GRT_HMM_HEADER
