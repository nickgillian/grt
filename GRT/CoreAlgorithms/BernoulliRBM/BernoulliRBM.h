
/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief This class implements a Bernoulli Restricted Boltzmann machine.

@remark This implementation is based on the RBM algorithm in the DeepLearnToolbox: https://github.com/rasmusbergpalm/DeepLearnToolbox

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

#ifndef GRT_BERNOULLI_RBM_HEADER
#define GRT_BERNOULLI_RBM_HEADER

#include "../../Util/GRTTypedefs.h"
#include "../../DataStructures/MatrixFloat.h"
#include "../../CoreModules/MLBase.h"

GRT_BEGIN_NAMESPACE

class GRT_API BernoulliRBM : public MLBase{
    
    public:
    BernoulliRBM(const UINT numHiddenUnits = 100,const UINT maxNumEpochs = 1000,const Float learningRate = 1,const Float learningRateUpdate = 1,const Float momentum = 0.5,const bool useScaling = true,const bool randomiseTrainingOrder = true);
    
    virtual ~BernoulliRBM();
    
    /**
    This is the prediction interface for referenced VectorFloat data, it calls the main prediction interface below.
    The RBM should be trained first before you use this function.
    The size of the input data must match the number of visible units.
    
    @param inputData: a reference to the input data that will be used to train the RBM model
    @return returns true if the prediction was successful, false otherwise
    */
    bool predict_(VectorFloat &inputData);
    
    /**
    This is the main prediction interface for referenced VectorFloat data.  It propagates the input data up through the RBM.
    The RBM should be trained first before you use this function.
    The size of the input data must match the number of visible units.
    
    @param inputData: a reference to the input data that will be used to train the RBM model
    @param outputData: a reference to the output data that will be used to train the RBM model
    @return returns true if the prediction was successful, false otherwise
    */
    bool predict_(VectorFloat &inputData,VectorFloat &outputData);
    
    /**
    This function is used during the training phase to propagate the input data up through the RBM, this gives P( h_j = 1 | input )
    If you are using this function then you should make sure the RBM is trained first before you use it.
    The size of the matrices must match the size of the model.
    
    @param inputData: a reference to the input data
    @param outputData: a reference to the output data that will store the results of the propagation
    @param rowIndex: the row in the inputData/outputData that should be used for the propagation
    @return returns true if the prediction was successful, false otherwise
    */
    bool predict_(const MatrixFloat &inputData,MatrixFloat &outputData,const UINT rowIndex);
    
    /**
    This is the main training interface for referenced MatrixFloat data.
    
    @param trainingData: a reference to the training data that will be used to train the RBM model
    @return returns true if the model was successfully trained, false otherwise
    */
    virtual bool train_(MatrixFloat &data);
    
    /**
    This function will reset the model (i.e. set all values back to default settings). If you want to completely clear the model
    (i.e. clear any learned weights or values) then you should use the clear function.
    
    @return returns true if the instance was reset succesfully, false otherwise
    */
    virtual bool reset();
    
    /**
    This function will completely clear the RBM instance, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the instance was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    This saves the trained model to a file.
    
    @param file: a reference to the file the model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained model from a file.
    
    @param file: a reference to the file the model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    bool reconstruct(const VectorFloat &input,VectorFloat &output);
    
    virtual bool print() const;
    
    bool getRandomizeWeightsForTraining() const;
    UINT getNumVisibleUnits() const;
    UINT getNumHiddenUnits() const;
    VectorFloat getOutputData() const;
    const MatrixFloat& getWeights() const;
    
    bool setNumHiddenUnits(const UINT numHiddenUnits);
    bool setMomentum(const Float momentum);
    bool setLearningRateUpdate(const Float learningRateUpdate);
    bool setRandomizeWeightsForTraining(const bool randomizeWeightsForTraining);
    bool setBatchSize(const UINT batchSize);
    bool setBatchStepSize(const UINT batchStepSize);
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train; ///<Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::predict; ///<Tell the compiler we are using the base class predict method to stop hidden virtual function warnings
    using MLBase::train_; ///<Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::predict_; ///<Tell the compiler we are using the base class predict method to stop hidden virtual function warnings
    
protected:
    bool loadLegacyModelFromFile( std::fstream &file );
    
    inline Float sigmoidRandom(const Float &x){
        return (1.0 / (1.0 + exp(-x)) > rand.getRandomNumberUniform(0.0,1.0)) ? 1.0 : 0.0;
    }
    
    bool randomizeWeightsForTraining;
    UINT numVisibleUnits;
    UINT numHiddenUnits;
    UINT batchSize;
    UINT batchStepSize;
    Float momentum;
    Float learningRateUpdate;
    MatrixFloat weightsMatrix;
    VectorFloat visibleLayerBias;
    VectorFloat hiddenLayerBias;
    VectorFloat ph_mean;
    VectorFloat ph_sample;
    VectorFloat nv_means;
    VectorFloat nv_samples;
    VectorFloat nh_means;
    VectorFloat nh_samples;
    VectorFloat outputData;
    Vector<MinMax> ranges;
    Random rand;
    
    struct BatchIndexs{
        UINT startIndex;
        UINT endIndex;
        UINT batchSize;
    };
    typedef struct BatchIndexs BatchIndexs;
    
};

GRT_END_NAMESPACE

#endif //GRT_BERNOULLI_RBM_HEADER
