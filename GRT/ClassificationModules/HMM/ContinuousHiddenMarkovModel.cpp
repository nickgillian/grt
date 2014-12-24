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

#include "ContinuousHiddenMarkovModel.h"

using namespace std;

namespace GRT {


//Init the model with a set number of states and symbols
ContinuousHiddenMarkovModel::ContinuousHiddenMarkovModel(const UINT downsampleFactor,const UINT delta){

    clear();
	this->downsampleFactor = downsampleFactor;
	this->delta = delta;
    modelType = HMM_LEFTRIGHT;
	cThreshold = -1000;
    useScaling = false;
    
    debugLog.setProceedingText("[DEBUG ContinuousHiddenMarkovModel]");
    errorLog.setProceedingText("[ERROR ContinuousHiddenMarkovModel]");
    warningLog.setProceedingText("[WARNING ContinuousHiddenMarkovModel]");
    trainingLog.setProceedingText("[TRAINING ContinuousHiddenMarkovModel]");
}
 
ContinuousHiddenMarkovModel::ContinuousHiddenMarkovModel(const ContinuousHiddenMarkovModel &rhs){
    this->numStates = rhs.numStates;
	this->numInputDimensions = rhs.numInputDimensions;
	this->delta = rhs.delta;
	this->cThreshold = rhs.cThreshold;
	this->modelType = rhs.modelType;
	this->loglikelihood = rhs.loglikelihood;
	this->a = rhs.a;
	this->b = rhs.b;
	this->pi = rhs.pi;
    this->trainingLog = rhs.trainingLog;

    debugLog.setProceedingText("[DEBUG ContinuousHiddenMarkovModel]");
    errorLog.setProceedingText("[ERROR ContinuousHiddenMarkovModel]");
    warningLog.setProceedingText("[WARNING ContinuousHiddenMarkovModel]");
    trainingLog.setProceedingText("[TRAINING ContinuousHiddenMarkovModel]");
}
    
//Default destructor
ContinuousHiddenMarkovModel::~ContinuousHiddenMarkovModel(){
    
}
    
double ContinuousHiddenMarkovModel::predict(const VectorDouble &x){
    
    if( !trained ){
        return 0;
    }
    
    if( x.size() != numInputDimensions ){
        return 0;
    }
    
    //Add the new sample to the circular buffer
    observationSequence.push_back( x );
    
    //Convert the circular buffer to MatrixDouble
    for(unsigned int i=0; i<numStates; i++){
        for(unsigned int j=0; j<numInputDimensions; j++){
            obsSequence[i][j] = observationSequence[i][j];
        }
    }

    return predict( obsSequence );
}
  
/*double predictLogLikelihood(Vector<UINT> &obs)
 - This method computes P(O|A,B,Pi) using the forward algorithm
 */
double ContinuousHiddenMarkovModel::predict(const MatrixDouble &obs){
    
    const int T = (int)obs.getNumRows();
	int t,i,j = 0;
    MatrixDouble alpha(T,numStates);
    VectorDouble c(T);
    
	////////////////// Run the forward algorithm ////////////////////////
	//Step 1: Init at t=0
	t = 0;
	c[t] = 0.0;
	for(i=0; i<numStates; i++){
		alpha[t][i] = pi[i]*gauss(b,obs,i,t,numInputDimensions,sigma);
		c[t] += alpha[t][i];
	}
    
	//Set the inital scaling coeff
	c[t] = 1.0/c[t];
    
	//Scale alpha
    for(i=0; i<numStates; i++) alpha[t][i] *= c[t];
    
	//Step 2: Induction
	for(t=1; t<T; t++){
		c[t] = 0.0;
		for(j=0; j<numStates; j++){
			alpha[t][j] = 0.0;
			for(i=0; i<numStates; i++){
				alpha[t][j] +=  alpha[t-1][i] * a[i][j];
			}
            alpha[t][j] *= gauss(b,obs,j,t,numInputDimensions,sigma);
            c[t] += alpha[t][j];
		}
        
		//Set the scaling coeff
		c[t] = 1.0/c[t];
        
		//Scale Alpha
        for(j=0; j<numStates; j++) alpha[t][j] *= c[t];
	}
    
    if( int(estimatedStates.size()) != T ) estimatedStates.resize(T);
    for(t=0; t<T; t++){
        double maxValue = 0;
        for(i=0; i<numStates; i++){
            if( alpha[t][i] > maxValue ){
                maxValue = alpha[t][i];
                estimatedStates[t] = i;
            }
        }
    }
    
	//Termination
	loglikelihood = 0.0;
    for(t=0; t<T; t++) loglikelihood += log( c[t] );
    return -loglikelihood; //Return the negative log likelihood
}

bool ContinuousHiddenMarkovModel::train_(TimeSeriesClassificationSample &trainingData){

    //Clear any previous models
    clear();

    //The number of states is simply set as the number of samples in the training sample
    numStates = (unsigned int)floor(trainingData.getLength()/downsampleFactor);
    numInputDimensions = trainingData.getNumDimensions();
    classLabel = trainingData.getClassLabel();
    
    //a is simply set as the number of 1/numStates
    a.resize(numStates, numStates);
    for(unsigned int i=0; i<numStates; i++){
        for(unsigned int j=0; j<numStates; j++){
            a[i][j] = 1.0/numStates;
        }
    }
    
	//b is simply set as the training sample
    b.resize(numStates, numInputDimensions);
    
    unsigned int index = 0;
    double norm = 0;
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
    pi[0] = 1;
    
    switch( modelType ){
		case(HMM_ERGODIC):
			//Don't need todo anything
			break;
		case(HMM_LEFTRIGHT):
			//Set the state transitions constraints
			for(UINT i=0; i<numStates; i++)
				for(UINT j=0; j<numStates; j++)
					if((j<i) || (j>i+delta)) a[i][j] = 0.0;
            
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
    
    //Estimate sigma, this is just the average standard deviation across all dimensions
    sigma = 0;
    VectorDouble sig = trainingData.getData().getStdDev();
    for(UINT i=0; i<numInputDimensions; i++){
        sigma += sig[i];
    }
    sigma /= numInputDimensions;

    //Setup the observation buffer for prediction
    observationSequence.resize( numStates );
    obsSequence.resize(numStates,numInputDimensions);
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
            observationSequence.push_back( VectorDouble(numInputDimensions,0) );
        }
    }
    
    return true;
}
    
bool ContinuousHiddenMarkovModel::clear(){
    
    //Clear the base class
    MLBase::clear();
    
    trained = false;
    numStates = 0;
    loglikelihood = 0;
	a.clear();
	b.clear();
	pi.clear();
    observationSequence.clear();
    estimatedStates.clear();
    
    return true;
}

bool ContinuousHiddenMarkovModel::print() const{

	trainingLog << "A: " << endl;
	for(UINT i=0; i<a.getNumRows(); i++){
		for(UINT j=0; j<a.getNumCols(); j++){
			trainingLog << a[i][j] << "\t";
		}
        trainingLog << endl;
	}

	trainingLog << "B: " << endl;
	for(UINT i=0; i<b.getNumRows(); i++){
		for(UINT j=0; j<b.getNumCols(); j++){
			trainingLog << b[i][j] << "\t";
		}
        trainingLog << endl;
	}
    
    trainingLog << "Pi: ";
	for(UINT i=0; i<pi.size(); i++){
        trainingLog << pi[i] << "\t";
    }
    trainingLog<<endl;

    //Check the weights all sum to 1
    if( true ){
        double sum=0.0;
        for(UINT i=0; i<a.getNumRows(); i++){
          sum=0.0;
          for(UINT j=0; j<a.getNumCols(); j++) sum += a[i][j];
          if( sum <= 0.99 || sum >= 1.01 ) warningLog << "WARNING: A Row " << i <<" Sum: "<< sum << endl;
        }

        for(UINT i=0; i<b.getNumRows(); i++){
          sum=0.0;
          for(UINT j=0; j<b.getNumCols(); j++) sum += b[i][j];
          if( sum <= 0.99 || sum >= 1.01 ) warningLog << "WARNING: B Row " << i << " Sum: " << sum << endl;
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
    return false;
}
    
bool ContinuousHiddenMarkovModel::setModelType(const UINT modelType){
    if( modelType == HMM_ERGODIC || modelType == HMM_LEFTRIGHT ){
        clear();
        this->modelType = modelType;
        return true;
    }
    return false;
}

bool ContinuousHiddenMarkovModel::setDelta(const UINT delta){
    if( delta > 0 ){
        clear();
        this->delta = delta;
        return true;
    }
    return false;
}
    
double ContinuousHiddenMarkovModel::gauss( const MatrixDouble &x, const MatrixDouble &y,const unsigned int i,const unsigned int j,const unsigned int N, const double sigma ){
    double sum = 0;
    for(unsigned int n=0; n<N; n++){
        sum += SQR(  x[i][n] - y[j][n] );
    }
    return exp( -SQR(sqrt( sum )/sigma) );
}
    
bool ContinuousHiddenMarkovModel::saveModelToFile(fstream &file) const{
    
    return true;
}

bool ContinuousHiddenMarkovModel::loadModelFromFile(fstream &file){
    
    return true;
}

}
