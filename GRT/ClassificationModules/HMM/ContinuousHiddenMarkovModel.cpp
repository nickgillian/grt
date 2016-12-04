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
#include "ContinuousHiddenMarkovModel.h"
#include "HMMEnums.h"

GRT_BEGIN_NAMESPACE

//Init the model with a set number of states and symbols
ContinuousHiddenMarkovModel::ContinuousHiddenMarkovModel(const UINT downsampleFactor,const UINT delta,const bool autoEstimateSigma,const Float sigma) : MLBase( "ContinuousHiddenMarkovModel" )
{
    
    clear();
    this->downsampleFactor = downsampleFactor;
    this->delta = delta;
    this->autoEstimateSigma = autoEstimateSigma;
    this->sigma = sigma;
    modelType = HMMModelTypes::HMM_LEFTRIGHT;
    cThreshold = 0;
    useScaling = false;
}

ContinuousHiddenMarkovModel::ContinuousHiddenMarkovModel(const ContinuousHiddenMarkovModel &rhs) :  MLBase( "ContinuousHiddenMarkovModel" )
{
    this->downsampleFactor = rhs.downsampleFactor;
    this->numStates = rhs.numStates;
    this->classLabel = rhs.classLabel;
    this->timeseriesLength = rhs.timeseriesLength;
    this->sigma = rhs.sigma;
    this->autoEstimateSigma = rhs.autoEstimateSigma;
    this->sigmaStates = rhs.sigmaStates;
    this->a = rhs.a;
    this->b = rhs.b;
    this->pi = rhs.pi;
    this->alpha = rhs.alpha;
    this->c = rhs.c;
    this->observationSequence = rhs.observationSequence;
    this->obsSequence = rhs.obsSequence;
    this->estimatedStates = rhs.estimatedStates;
    this->modelType = rhs.modelType;
    this->delta = rhs.delta;
    this->loglikelihood = rhs.loglikelihood;
    this->cThreshold = rhs.cThreshold;
}

//Default destructor
ContinuousHiddenMarkovModel::~ContinuousHiddenMarkovModel(){
    
}

ContinuousHiddenMarkovModel& ContinuousHiddenMarkovModel::operator=(const ContinuousHiddenMarkovModel &rhs){
    
    if( this != &rhs ){
        this->downsampleFactor = rhs.downsampleFactor;
        this->numStates = rhs.numStates;
        this->classLabel = rhs.classLabel;
        this->timeseriesLength = rhs.timeseriesLength;
        this->sigma = rhs.sigma;
        this->autoEstimateSigma = rhs.autoEstimateSigma;
        this->sigmaStates = rhs.sigmaStates;
        this->a = rhs.a;
        this->b = rhs.b;
        this->pi = rhs.pi;
        this->alpha = rhs.alpha;
        this->c = rhs.c;
        this->observationSequence = rhs.observationSequence;
        this->obsSequence = rhs.obsSequence;
        this->estimatedStates = rhs.estimatedStates;
        this->modelType = rhs.modelType;
        this->delta = rhs.delta;
        this->loglikelihood = rhs.loglikelihood;
        this->cThreshold = rhs.cThreshold;
        
        const MLBase *basePointer = &rhs;
        this->copyMLBaseVariables( basePointer );
    }
    
    return *this;
}

bool ContinuousHiddenMarkovModel::predict_(VectorFloat &x){
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &x) - The model is not trained!" << std::endl;
        return false;
    }
    
    if( x.getSize() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &x) - The input vector size (" << x.getSize() << ") does not match the number of input dimensions (" << numInputDimensions << ")" << std::endl;
        return false;
    }
    
    //Add the new sample to the circular buffer
    observationSequence.push_back( x );
    
    //Convert the circular buffer to MatrixFloat
    for(unsigned int i=0; i<observationSequence.getSize(); i++){
        for(unsigned int j=0; j<numInputDimensions; j++){
            obsSequence[i][j] = observationSequence[i][j];
        }
    }
    
    return predict_( obsSequence );
}

bool ContinuousHiddenMarkovModel::predict_( MatrixFloat &timeseries ){
    
    if( !trained ){
        errorLog << "predict_( MatrixFloat &timeseries ) - The model is not trained!" << std::endl;
        return false;
    }
    
    if( timeseries.getNumCols() != numInputDimensions ){
        errorLog << "predict_( MatrixFloat &timeseries ) - The matrix column size (" << timeseries.getNumCols() << ") does not match the number of input dimensions (" << numInputDimensions << ")" << std::endl;
        return false;
    }
    
    unsigned int t,i,j,k,index = 0;
    Float maxAlpha = 0;
    Float norm = 0;
    
    //Downsample the observation timeseries using the same downsample factor of the training data
    const unsigned int timeseriesLength = (unsigned int)timeseries.getNumRows();
    const unsigned int T = downsampleFactor < timeseriesLength ? (unsigned int)floor( timeseriesLength / Float(downsampleFactor) ) : timeseriesLength;
    const unsigned int K = downsampleFactor < timeseriesLength ? downsampleFactor : 1; //K is used to average over multiple bins
    MatrixFloat obs(T,numInputDimensions);
    for(j=0; j<numInputDimensions; j++){
        index = 0;
        for(i=0; i<T; i++){
            norm = 0;
            obs[i][j] = 0;
            for(k=0; k<K; k++){
                if( index < timeseriesLength ){
                    obs[i][j] += timeseries[index++][j];
                    norm += 1;
                }
            }
            if( norm > 1 )
            obs[i][j] /= norm;
        }
    }
    
    //Resize alpha, c, and the estimated states vector as needed
    if( alpha.getNumRows() != T || alpha.getNumCols() != numStates ) alpha.resize(T,numStates);
    if( (unsigned int)c.size() != T ) c.resize(T);
    if( (unsigned int)estimatedStates.size() != T ) estimatedStates.resize(T);
    
    ////////////////// Run the forward algorithm ////////////////////////
    //Step 1: Init at t=0
    t = 0;
    c[t] = 0;
    maxAlpha = 0;
    for(i=0; i<numStates; i++){
        alpha[t][i] = pi[i]*gauss(b,obs,sigmaStates,i,t,numInputDimensions);
        c[t] += alpha[t][i];
        
        //Keep track of the best state at time t
        if( alpha[t][i] > maxAlpha ){
            maxAlpha = alpha[t][i];
            estimatedStates[t] = i;
        }
    }
    
    //Set the inital scaling coeff
    c[t] = 1.0/c[t];
    
    //Scale alpha
    for(i=0; i<numStates; i++) alpha[t][i] *= c[t];
    
    //Step 2: Induction
    for(t=1; t<T; t++){
        c[t] = 0.0;
        maxAlpha = 0;
        for(j=0; j<numStates; j++){
            alpha[t][j] = 0.0;
            for(i=0; i<numStates; i++){
                alpha[t][j] +=  alpha[t-1][i] * a[i][j];
            }
            alpha[t][j] *= gauss(b,obs,sigmaStates,j,t,numInputDimensions);
            c[t] += alpha[t][j];
            
            //Keep track of the best state at time t
            if( alpha[t][j] > maxAlpha ){
                maxAlpha = alpha[t][j];
                estimatedStates[t] = j;
            }
        }
        
        //Set the scaling coeff
        c[t] = 1.0/c[t];
        
        //Scale Alpha
        for(j=0; j<numStates; j++) alpha[t][j] *= c[t];
    }
    
    //Termination
    loglikelihood = 0.0;
    for(t=0; t<T; t++) loglikelihood += log( c[t] );
    loglikelihood = -loglikelihood; //Store the negative log likelihood
    
    //Set the phase as the last estimated state, this will give a phase between [0 1]
    phase = (estimatedStates[T-1]+1.0)/Float(numStates);
    
    return true;
}

bool ContinuousHiddenMarkovModel::train_(TimeSeriesClassificationSample &trainingData){
    
    //Clear any previous models
    clear();
    
    //The number of states is simply set as the number of samples in the training sample
    timeseriesLength = trainingData.getLength();
    numStates = (unsigned int)floor((double)(timeseriesLength/downsampleFactor));
    numInputDimensions = trainingData.getNumDimensions();
    classLabel = trainingData.getClassLabel();
    
    //a is simply set as the number of 1/numStates
    a.resize(numStates, numStates);
    for(unsigned int i=0; i<numStates; i++){
        for(unsigned int j=0; j<numStates; j++){
            a[i][j] = 1.0/numStates;
        }
    }
    
    //b is simply set as the downsampled training sample
    b.resize(numStates, numInputDimensions);
    
    unsigned int index = 0;
    Float norm = 0;
    for(unsigned int j=0; j<numInputDimensions; j++){
        index = 0;
        for(unsigned int i=0; i<numStates; i++){
            norm = 0;
            b[i][j] = 0;
            for(unsigned int k=0; k<downsampleFactor; k++){
                if( index < trainingData.getLength() ){
                    b[i][j] += trainingData[index++][j];
                    norm += 1;
                }
            }
            if( norm > 1 )
            b[i][j] /= norm;
        }
    }
    
    //Estimate pi
    pi.resize(numStates);
    
    switch( modelType ){
        case(HMM_ERGODIC):
        for(UINT i=0; i<numStates; i++){
            pi[i] = 1.0/numStates;
        }
        break;
        case(HMM_LEFTRIGHT):
        //Set the state transitions constraints
        for(UINT i=0; i<numStates; i++){
            norm = 0;
            for(UINT j=0; j<numStates; j++){
                if((j<i) || (j>i+delta)) a[i][j] = 0.0;
                norm += a[i][j];
            }
            if( norm > 0 ){
                for(UINT j=0; j<numStates; j++){
                    a[i][j] /= norm;
                }
            }
        }
        
        //Set pi to start in state 0
        for(UINT i=0; i<numStates; i++){
            pi[i] = i==0 ? 1 : 0;
        }
        break;
        default:
        throw("HMM_ERROR: Unkown model type!");
        return false;
        break;
    }
    
    //Setup sigma for each state
    sigmaStates.resize( numStates, numInputDimensions );
    
    if( autoEstimateSigma ){
        
        //Estimate the standard dev for each dimension, for each state
        MatrixFloat meanResults( numStates, numInputDimensions );
        for(unsigned int j=0; j<numInputDimensions; j++){
            
            //Estimate the mean for each state
            index = 0;
            for(unsigned int i=0; i<numStates; i++){
                norm = 0;
                meanResults[i][j] = 0;
                for(unsigned int k=0; k<downsampleFactor; k++){
                    if( index < trainingData.getLength() ){
                        meanResults[i][j] += trainingData[index++][j];
                        norm += 1;
                    }
                }
                if( norm > 1 ){
                    meanResults[i][j] /= norm;
                }
            }
            
            //Loop back over the data again and estimate the stddev for each state
            index = 0;
            for(unsigned int i=0; i<numStates; i++){
                norm = 0;
                sigmaStates[i][j] = 0;
                for(unsigned int k=0; k<downsampleFactor; k++){
                    if( index < trainingData.getLength() ){
                        sigmaStates[i][j] += SQR( trainingData[index++][j]-meanResults[i][j] );
                        norm += 1;
                    }
                }
                if( norm > 1 ){
                    sigmaStates[i][j] = sqrt( 1.0/norm * sigmaStates[i][j] );
                }
                
                if( sigmaStates[i][j] < sigma ){
                    sigmaStates[i][j] = sigma;
                }
            }
        }
        
    }else{
        sigmaStates.setAllValues(sigma);
    }
    
    //Setup the observation buffer for prediction
    observationSequence.resize( timeseriesLength, VectorFloat(numInputDimensions,0) );
    obsSequence.resize(timeseriesLength,numInputDimensions);
    estimatedStates.resize( numStates );
    
    //Finally, flag that the model was trained
    trained = true;
    
    return true;
}

bool ContinuousHiddenMarkovModel::reset(){
    
    //Reset the base class
    MLBase::reset();
    
    if( trained ){
        for(unsigned int i=0; i<observationSequence.getSize(); i++){
            observationSequence.push_back( VectorFloat(numInputDimensions,0) );
        }
    }
    
    return true;
}

bool ContinuousHiddenMarkovModel::clear(){
    
    //Clear the base class
    MLBase::clear();
    
    numStates = 0;
    loglikelihood = 0;
    timeseriesLength = 0;
    a.clear();
    b.clear();
    pi.clear();
    alpha.clear();
    c.clear();
    observationSequence.clear();
    obsSequence.clear();
    estimatedStates.clear();
    sigmaStates.clear();
    
    return true;
}

bool ContinuousHiddenMarkovModel::print() const{
    
    if( trained ){
        trainingLog << "A: " << std::endl;
        for(UINT i=0; i<a.getNumRows(); i++){
            for(UINT j=0; j<a.getNumCols(); j++){
                trainingLog << a[i][j] << "\t";
            }
            trainingLog << std::endl;
        }
        
        trainingLog << "B: " << std::endl;
        for(UINT i=0; i<b.getNumRows(); i++){
            for(UINT j=0; j<b.getNumCols(); j++){
                trainingLog << b[i][j] << "\t";
            }
            trainingLog << std::endl;
        }
        
        trainingLog << "Pi: ";
        for(size_t i=0; i<pi.size(); i++){
            trainingLog << pi[i] << "\t";
        }
        trainingLog << std::endl;
        
        trainingLog << "SigmaStates: ";
        for(UINT i=0; i<sigmaStates.getNumRows(); i++){
            for(UINT j=0; j<sigmaStates.getNumCols(); j++){
                trainingLog << sigmaStates[i][j] << "\t";
            }
            trainingLog << std::endl;
        }
        trainingLog << std::endl;
        
        //Check the weights all sum to 1
        if( true ){
            Float sum=0.0;
            for(UINT i=0; i<a.getNumRows(); i++){
                sum=0.0;
                for(UINT j=0; j<a.getNumCols(); j++) sum += a[i][j];
                if( sum <= 0.99 || sum >= 1.01 ) warningLog << "WARNING: A Row " << i <<" Sum: "<< sum << std::endl;
            }
        }
    }
    
    return true;
    
}

bool ContinuousHiddenMarkovModel::setDownsampleFactor(const UINT downsampleFactor){
    if( downsampleFactor > 0 ){
        clear();
        this->downsampleFactor = downsampleFactor;
        return true;
    }
    warningLog << "setDownsampleFactor(const UINT downsampleFactor) - Failed to set downsample factor, it must be greater than zero!" << std::endl;
    return false;
}

bool ContinuousHiddenMarkovModel::setModelType(const UINT modelType){
    if( modelType == HMM_ERGODIC || modelType == HMM_LEFTRIGHT ){
        clear();
        this->modelType = modelType;
        return true;
    }
    warningLog << "setModelType(const UINT modelType) - Failed to set model type, unknown type!" << std::endl;
    return false;
}

bool ContinuousHiddenMarkovModel::setDelta(const UINT delta){
    if( delta > 0 ){
        clear();
        this->delta = delta;
        return true;
    }
    warningLog << "setDelta(const UINT delta) - Failed to set delta, it must be greater than zero!" << std::endl;
    return false;
}

bool ContinuousHiddenMarkovModel::setSigma(const Float sigma){
    if( sigma > 0 ){
        this->sigma = sigma;
        
        if( !autoEstimateSigma && trained ){
            sigmaStates.setAllValues(sigma);
        }
        return true;
    }
    warningLog << "setSigma(const Float sigma) - Failed to set sigma, it must be greater than zero!" << std::endl;
    return false;
}

bool ContinuousHiddenMarkovModel::setAutoEstimateSigma(const bool autoEstimateSigma){
    
    clear();
    
    this->autoEstimateSigma = autoEstimateSigma;
    
    return true;
}

Float ContinuousHiddenMarkovModel::gauss( const MatrixFloat &x, const MatrixFloat &y, const MatrixFloat &sigma, const unsigned int i,const unsigned int j,const unsigned int N ){
    Float z = 1;
    for(unsigned int n=0; n<N; n++){
        z *= (1.0/( sigma[i][n] * SQRT_TWO_PI )) * exp( - SQR(x[i][n]-y[j][n])/(2.0*SQR(sigma[i][n])) );
    }
    return z;
}

bool ContinuousHiddenMarkovModel::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog << "save( fstream &file ) - File is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "CONTINUOUS_HMM_MODEL_FILE_V1.0\n";
    
    //Write the base settings to the file
    if( !MLBase::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    file << "DownsampleFactor: " << downsampleFactor << std::endl;
    file << "NumStates: " << numStates << std::endl;
    file << "ClassLabel: " << classLabel << std::endl;
    file << "TimeseriesLength: " << timeseriesLength << std::endl;
    file << "Sigma: " << sigma << std::endl;
    file << "AutoEstimateSigma: " << autoEstimateSigma << std::endl;
    file << "ModelType: " << modelType << std::endl;
    file << "Delta: " << delta << std::endl;
    file << "Threshold: " << cThreshold << std::endl;
    
    if( trained ){
        file << "A:\n";
        for(UINT i=0; i<numStates; i++){
            for(UINT j=0; j<numStates; j++){
                file << a[i][j];
                if( j+1 < numStates ) file << "\t";
            }file << std::endl;
        }
        
        file << "B:\n";
        for(UINT i=0; i<numStates; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                file << b[i][j];
                if( j+1 < numInputDimensions ) file << "\t";
            }file << std::endl;
        }
        
        file<<"Pi: ";
        for(UINT i=0; i<numStates; i++){
            file << pi[i];
            if( i+1 < numStates ) file << "\t";
        }
        file << std::endl;
        
        file << "SigmaStates: ";
        for(UINT i=0; i<numStates; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                file << sigmaStates[i][j];
                if( j+1 < numInputDimensions ) file << "\t";
            }file << std::endl;
        }
        file << std::endl;
        
    }
    
    return true;
}

bool ContinuousHiddenMarkovModel::load( std::fstream &file ){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "load( fstream &file ) - File is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    file >> word;
    
    //Find the file type header
    if(word != "CONTINUOUS_HMM_MODEL_FILE_V1.0"){
        errorLog << "load( fstream &file ) - Could not find Model File Header!" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "DownsampleFactor:"){
        errorLog << "load( fstream &file ) - Could not find the DownsampleFactor header." << std::endl;
        return false;
    }
    file >> downsampleFactor;
    
    file >> word;
    if(word != "NumStates:"){
        errorLog << "load( fstream &file ) - Could not find the NumStates header." << std::endl;
        return false;
    }
    file >> numStates;
    
    file >> word;
    if(word != "ClassLabel:"){
        errorLog << "load( fstream &file ) - Could not find the ClassLabel header." << std::endl;
        return false;
    }
    file >> classLabel;
    
    file >> word;
    if(word != "TimeseriesLength:"){
        errorLog << "load( fstream &file ) - Could not find the TimeseriesLength header." << std::endl;
        return false;
    }
    file >> timeseriesLength;
    
    file >> word;
    if(word != "Sigma:"){
        errorLog << "load( fstream &file ) - Could not find the Sigma for the header." << std::endl;
            return false;
    }
    file >> sigma;
    
    file >> word;
    if(word != "AutoEstimateSigma:"){
        errorLog << "load( fstream &file ) - Could not find the AutoEstimateSigma for the header." << std::endl;
            return false;
    }
    file >> autoEstimateSigma;
    
    file >> word;
    if(word != "ModelType:"){
        errorLog << "load( fstream &file ) - Could not find the ModelType for the header." << std::endl;
            return false;
    }
    file >> modelType;
    
    file >> word;
    if(word != "Delta:"){
        errorLog << "load( fstream &file ) - Could not find the Delta for the header." << std::endl;
            return false;
    }
    file >> delta;
    
    file >> word;
    if(word != "Threshold:"){
        errorLog << "load( fstream &file ) - Could not find the Threshold for the header." << std::endl;
            return false;
    }
    file >> cThreshold;
    
    if( trained ){
        a.resize(numStates,numStates);
        b.resize(numStates,numInputDimensions);
        pi.resize(numStates);
        sigmaStates.resize(numStates,numInputDimensions);
        
        //Load the A, B and Pi matrices
        file >> word;
        if(word != "A:"){
            errorLog << "load( fstream &file ) - Could not find the A matrix header." << std::endl;
            return false;
        }
        
        //Load A
        for(UINT i=0; i<numStates; i++){
            for(UINT j=0; j<numStates; j++){
                file >> a[i][j];
            }
        }
        
        file >> word;
        if(word != "B:"){
            errorLog << "load( fstream &file ) - Could not find the B matrix header." << std::endl;
            return false;
        }
        
        //Load B
        for(UINT i=0; i<numStates; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                file >> b[i][j];
            }
        }
        
        file >> word;
        if(word != "Pi:"){
            errorLog << "load( fstream &file ) - Could not find the Pi header." << std::endl;
            return false;
        }
        
        //Load Pi
        for(UINT i=0; i<numStates; i++){
            file >> pi[i];
        }
        
        file >> word;
        if(word != "SigmaStates:"){
            errorLog << "load( fstream &file ) - Could not find the SigmaStates header." << std::endl;
            return false;
        }
        
        //Load sigmaStates
        for(UINT i=0; i<numStates; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                file >> sigmaStates[i][j];
            }
        }
        
        //Setup the observation buffer for prediction
        observationSequence.resize( timeseriesLength, VectorFloat(numInputDimensions,0) );
        obsSequence.resize(timeseriesLength,numInputDimensions);
        estimatedStates.resize( numStates );
    }
    
    return true;
}

GRT_END_NAMESPACE
