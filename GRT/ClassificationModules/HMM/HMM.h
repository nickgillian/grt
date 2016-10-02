/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
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

#include "HMMEnums.h"
#include "DiscreteHiddenMarkovModel.h"
#include "ContinuousHiddenMarkovModel.h"
#include "../../CoreModules/Classifier.h"

GRT_BEGIN_NAMESPACE

/**
@brief This class acts as the main interface for using a Hidden Markov Model.

@remark This implementation is based on Rabiner, Lawrence. "A tutorial on hidden Markov models and selected applications in speech recognition." Proceedings of the IEEE 77.2 (1989): 257-286.
*/
class GRT_API HMM : public Classifier
{
public:
    /**
    Default Constructor.
    
    Sets up the HMM instance with default parameters.
    
    @param hmmType: sets the HMM type, this should be either HMM_DISCRETE or HMM_CONTINUOUS. Default: HMM_CONTINUOUS
    @param modelType: sets the model type used by either the discrete or continuous hmm. This should be either HMM_ERGODIC or HMM_LEFTRIGHT. Default: HMM_LEFTRIGHT
    @param delta: sets the number of states a model can transistion to for a HMM_LEFTRIGHT model. Default: 1
    @param useScaling: sets if the training/input data should be scaled to the range [0 1]. Default: false
    @param useNullRejection: sets if the algorithm should use null rejection (i.e., automatically gesture spotting). Default: false
    */
    HMM(const UINT hmmType=HMM_CONTINUOUS,const UINT modelType=HMM_LEFTRIGHT,const UINT delta=1,const bool useScaling = false,const bool useNullRejection = false);
    
    /**
    Default Constructor.
    
    Copies the settings/models from the rhs HMM instance to this instance.
    
    @param rhs: another HMM instance from which the settings/models will be copied to this instance
    */
    HMM(const HMM &rhs);
    
    /**
    Default Destructor.
    */
    virtual ~HMM(void);
    
    /**
    Custom Equals Operator.
    
    Copies the settings/models from the rhs HMM instance to this instance.
    
    @param rhs: another HMM instance from which the settings/models will be copied to this instance
    @return returns a reference to this instance
    */
    HMM& operator=(const HMM &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an HMM instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another HMM instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This overrides the train function in the Classifier base class. It simply prints a warning message stating that the
    bool train(LabelledTimeSeriesClassificationData trainingData) function should be used to train the HMM model.
    
    @param trainingData: a reference to the training data
    @return returns true if the HMM model was trained, false otherwise
    */
    virtual bool train(ClassificationData trainingData);
    
    /**
    This trains the HMM model, using the labelled timeseries classification data.
    This is the main training function for the HMM class.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the HMM model was trained, false otherwise
    */
    virtual bool train_(TimeSeriesClassificationData &trainingData);
    
    /**
    This predicts the class of the inputVector.
    This overrides the predict function in the Classifier base class.
    
    @param inputVector: the input Vector to classify
    @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
    This predicts the class of the timeseries.
    This overrides the predict function in the Classifier base class.
    
    @param timeSeries: the input timeseries to classify
    @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(MatrixFloat &timeseries);
    
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
    
    virtual bool print() const;
    
    /**
    This saves the trained HMM model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the HMM model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained HMM model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the HMM model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    This function gets the HMM type.  This will be either HMM_DISCRETE or HMM_CONTINUOUS.
    
    @return returns the current HMM type
    */
    UINT getHMMType() const;
    
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
    This function gets the number of states in each HMM.  This is only relevant if the HMM model type is HMM_DISCRETE.
    
    @return returns the number of states in each discrete HMM
    */
    UINT getNumStates() const;
    
    /**
    This function gets the number of symbols in each HMM.  This is only relevant if the HMM model type is HMM_DISCRETE.
    
    @return returns the number of symbols in each discrete HMM
    */
    UINT getNumSymbols() const;
    
    /**
    This function gets the number of random training iterations used to train each discrete HMM.
    
    The accuracy of the Baum Welch algorithm can be effected by the starting values of the A, B, and PI matrices.  The HMM
    algorithm therefore tries several different starting values and then continues to train the algorithm with the best settings.
    
    The number of random training iterations parameter therefore controls the number of different starting values.
    
    @return returns the maximum number of iterations used to trained each HMM
    */
    UINT getNumRandomTrainingIterations() const;
    
    /**
    This function gets returns a Vector of trained DiscreteHiddenMarkovModels.  There will be one HiddenMarkovModel for each class in
    the training data. This is only relevant if the HMM model type is HMM_DISCRETE.
    
    @return returns the trained DiscreteHiddenMarkovModels
    */
    Vector< DiscreteHiddenMarkovModel > getDiscreteModels() const;
    
    /**
    This function gets returns a Vector of trained ContinuousHiddenMarkovModels.  There will be one HiddenMarkovModel for each sample in
    the training data. This is only relevant if the HMM model type is HMM_CONTINUOUS.
    
    @return returns the trained ContinuousHiddenMarkovModels
    */
    Vector< ContinuousHiddenMarkovModel > getContinuousModels() const;
    
    /**
    This function sets the hmmType.  This should be either a HMM_DISCRETE, or HMM_CONTINUOUS.
    
    This will clear any trained model.
    
    @param hmmType: the new hmmType
    @return returns true if the parameter was set correctly, false otherwise
    */
    bool setHMMType(const UINT hmmType);
    
    /**
    This function sets the modelType used for each HMM.  This should be one of the HMM modelType enums.
    
    This will clear any trained model.
    
    @param modelType: the modelType in each HMM
    @return returns true if the parameter was set correctly, false otherwise
    */
    bool setModelType(const UINT modelType);
    
    /**
    This function sets the delta parameter in each HMM.
    
    The delta value controls how many states a model can transition to if the LEFTRIGHT model type is used.
    
    The parameter must be greater than zero.
    
    This will clear any trained model.
    
    @param delta: the delta parameter used for each HMM
    @return returns true if the parameter was set correctly, false otherwise
    */
    bool setDelta(const UINT delta);
    
    /**
    This function sets the downsample factor used for a HMM_CONTINUOUS.  The downsample factor controls the resampling of each training
    timeseries for the continuous HMM.  A downsample factor of 5, for instance, will result in each timeseries being resized (smaller) by a factor of 5.
    Increasing the downsample factor will significantly increase the realtime prediction time for each model, however, setting this value too high may
    reduce the overall accuracy of the model.
    
    The parameter must be greater than zero. This will clear any trained model.
    
    @param downsampleFactor: the downsample factor used for a continuous HMM
    @return returns true if the parameter was set correctly, false otherwise
    */
    bool setDownsampleFactor(const UINT downsampleFactor);
    
    /**
    This function sets the committeeSize used for a HMM_CONTINUOUS.  The commitee size controls the number of votes used to make a prediction for a
    continuous HMM.  For example, if the committeeSize is 5, then the top 5 estimations will be combined togethere
    
    The parameter must be greater than zero. This will NOT clear any trained model.
    
    @param committeeSize: the committeeSize used for a continuous HMM
    @return returns true if the parameter was set correctly, false otherwise
    */
    bool setCommitteeSize(const UINT committeeSize);
    
    /**
    This function sets the number of states for a HMM_DISCRETE.
    
    The parameter must be greater than zero. This will clear any trained model.
    
    @param numStates: the number of states in each HMM
    @return returns true if the parameter was set correctly, false otherwise
    */
    bool setNumStates(const UINT numStates);
    
    /**
    This function sets the number of symbols for a HMM_DISCRETE.  The parameter must be greater than zero. This will clear any trained model.
    
    @param symbols: the number of symbols in each HMM
    @return returns true if the parameter was set correctly, false otherwise
    */
    bool setNumSymbols(const UINT numStates);
    
    /**
    This function sets the number of random training iterations used to train each HMM.
    
    The accuracy of the Baum Welch algorithm can be effected by the starting values of the A, B, and PI matrices.  The HMM
    algorithm therefore tries several different starting values and then continues to train the algorithm with the best settings.
    The number of random training iterations parameter therefore controls the number of different starting values.
    
    This will clear any trained model.
    
    @param numRandomTrainingIterations: the number of random training iterations used to train each HMM
    @return returns true if the parameter was set correctly, false otherwise
    */
    bool setNumRandomTrainingIterations(const UINT numRandomTrainingIterations);
    
    /**
    This function sets the sigma parameter used for HMM_CONTINUOUS.
    
    Sigma controls the width of the Gaussian distance function that estimates the similarity between the input sample and the model.
    
    Modifing sigma will NOT clear the trained model, enabling the user to tune this parameter when needed.
    
    Sigma must be greater than zero.
    
    @param sigma: the new sigma parameter
    @return returns true if the parameter was set correctly, false otherwise
    */
    bool setSigma(const Float sigma);
    
    bool setAutoEstimateSigma(const bool autoEstimateSigma);

    /**
    Gets a string that represents the KNN class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict_;
    
protected:
    bool train_discrete(TimeSeriesClassificationData &trainingData);
    bool train_continuous(TimeSeriesClassificationData &trainingData);
    bool predict_discrete( VectorFloat &inputVector );
    bool predict_continuous( VectorFloat &inputVector );
    bool predict_discrete(MatrixFloat &timeseries);
    bool predict_continuous(MatrixFloat &timeseries);
    bool convertDataToObservationSequence( TimeSeriesClassificationData &classData, Vector< Vector< UINT > > &observationSequences );
    bool loadLegacyModelFromFile( std::fstream &file );
    
    UINT hmmType;           ///<Controls if this is a HMM_DISCRETE or a HMM_CONTINUOUS
    UINT modelType;         //Set if the model is ERGODIC or LEFTRIGHT
    UINT delta;             //The number of states a model can move to in a LeftRight model
    
    //Discrete HMM variables
    UINT numStates;         //The number of states for each model
    UINT numSymbols;        //The number of symbols for each model
    UINT numRandomTrainingIterations;
    Vector< DiscreteHiddenMarkovModel > discreteModels;
    
    //Continuous HMM variables
    UINT downsampleFactor;
    UINT committeeSize;
    Float sigma;
    bool autoEstimateSigma;
    Vector< ContinuousHiddenMarkovModel > continuousModels;
    
private:
    static RegisterClassifierModule< HMM > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_HMM_HEADER
