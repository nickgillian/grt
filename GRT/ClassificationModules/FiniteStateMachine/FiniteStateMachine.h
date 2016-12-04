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

#ifndef GRT_FINITE_STATE_MACHINE_HEADER
#define GRT_FINITE_STATE_MACHINE_HEADER

#include "../../CoreModules/Classifier.h"
#include "FSMParticleFilter.h"
#include "../../ClusteringModules/KMeans/KMeans.h"

GRT_BEGIN_NAMESPACE

class GRT_API FiniteStateMachine : public Classifier
{
public:
    
    /**
    Default constructor.
    
    */
    FiniteStateMachine(const UINT numParticles = 200,const UINT numClustersPerState = 20,const Float stateTransitionSmoothingCoeff = 0.0,const Float measurementNoise = 10.0);
    
    /**
    Defines the copy constructor.
    
    @param rhs: the instance from which all the data will be copied into this instance
    */
    FiniteStateMachine(const FiniteStateMachine &rhs);
    
    /**
    Default Destructor
    */
    virtual ~FiniteStateMachine(void);
    
    /**
    Defines how the data from the rhs FPSM should be copied to this FPSM
    
    @param rhs: another instance of a FPSM
    @return returns a pointer to this instance of the FPSM
    */
    FiniteStateMachine &operator=(const FiniteStateMachine &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an FiniteStateMachine instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another FiniteStateMachine instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the FiniteStateMachine model, using the labelled classification data.
    This overrides the train function in the Classifier base class.
    It converts the data into a TimeSeriesClassificationDataStream format and calls that train_ function.
    
    @param trainingData: a reference to the training data
    @return returns true if the FiniteStateMachine model was trained, false otherwise
    */
    virtual bool train_( ClassificationData &trainingData );
    
    /**
    This trains the FiniteStateMachine model, using the labelled timeseries classification data.
    This overrides the train function in the Classifier base class.
    It converts the data into a TimeSeriesClassificationDataStream format and calls that train_ function.
    
    @param trainingData: a reference to the training data
    @return returns true if the FiniteStateMachine model was trained, false otherwise
    */
    virtual bool train_( TimeSeriesClassificationData &trainingData );
    
    /**
    This is the main training function for the FiniteStateMachine model, using the TimeSeriesClassificationDataStream data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the FiniteStateMachine model was trained, false otherwise
    */
    virtual bool train_( TimeSeriesClassificationDataStream &data );
    
    /**
    This predicts the class of the inputVector.
    This overrides the predict function in the Classifier base class.
    
    @param inputVector: the input vector to classify
    @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
    This resets the FiniteStateMachine.
    
    If a model has been trained then this will randomize the current state and reset all the particles.
    
    @return returns true if the FPSM model was successfully reset, false otherwise.
    */
    virtual bool reset();
    
    /**
    This overrides the clear function in the Classifier base class.
    It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the module was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    This overrides the print function in the Classifier base class.
    It will print the current model and settings to the infoLog.
    
    @return returns true if the module was cleared succesfully, false otherwise
    */
    virtual bool print() const;
    
    /**
    This saves the trained FiniteStateMachine model to a file.
    This overrides the save function in the Classifier base class.
    
    @param file: a reference to the file the FiniteStateMachine model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained FiniteStateMachine model from a file.
    This overrides the load function in the Classifier base class.
    
    @param file: a reference to the file the FiniteStateMachine model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    
    bool setNumParticles(const UINT numParticles);
    bool setNumClustersPerState(const UINT numClustersPerState);
    bool setStateTransitionSmoothingCoeff(const Float stateTransitionSmoothingCoeff);
    bool setMeasurementNoise(const Float measurementNoise);
    
    /**
    Gets a string that represents the FiniteStateMachine class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();

    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_;
    using MLBase::predict_;
    
protected:
    bool recomputePT();
    bool recomputePE();
    bool initParticles();
    
    UINT numParticles;
    UINT numClustersPerState;
    Float stateTransitionSmoothingCoeff;
    Float measurementNoise;
    FSMParticleFilter particles;
    MatrixFloat stateTransitions;
    Vector< MatrixFloat > stateEmissions;
    Vector< Vector< IndexedDouble > > pt;   ///<This stores the stateTransitions matrix in a format more efficient for the particle filter
    Vector< Vector< VectorFloat > > pe;    ///<This stores the stateEmissions model in a format more efficient for the particle filter
    
private:
    static RegisterClassifierModule< FiniteStateMachine > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif
