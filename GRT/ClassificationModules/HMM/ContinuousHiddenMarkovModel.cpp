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
    sigma = 10;
	cThreshold = -1000;
    useScaling = false;
    
    debugLog.setProceedingText("[DEBUG ContinuousHiddenMarkovModel]");
    errorLog.setProceedingText("[ERROR ContinuousHiddenMarkovModel]");
    warningLog.setProceedingText("[WARNING ContinuousHiddenMarkovModel]");
    trainingLog.setProceedingText("[TRAINING ContinuousHiddenMarkovModel]");
}
 
ContinuousHiddenMarkovModel::ContinuousHiddenMarkovModel(const ContinuousHiddenMarkovModel &rhs){
    
    this->downsampleFactor = rhs.downsampleFactor;
	this->numStates = rhs.numStates;
    this->classLabel = rhs.classLabel;
    this->timeseriesLength = rhs.timeseriesLength;
    this->sigma = rhs.sigma;
	this->a = rhs.a;
	this->b = rhs.b;
	this->pi = rhs.pi;
    this->observationSequence = rhs.observationSequence;
    this->obsSequence = rhs.obsSequence;
    this->estimatedStates = rhs.estimatedStates;
    this->modelType = rhs.modelType;
	this->delta = rhs.delta;
	this->loglikelihood = rhs.loglikelihood;
	this->cThreshold = rhs.cThreshold;
    
    const MLBase *basePointer = &rhs;
    this->copyMLBaseVariables( basePointer );

    debugLog.setProceedingText("[DEBUG ContinuousHiddenMarkovModel]");
    errorLog.setProceedingText("[ERROR ContinuousHiddenMarkovModel]");
    warningLog.setProceedingText("[WARNING ContinuousHiddenMarkovModel]");
    trainingLog.setProceedingText("[TRAINING ContinuousHiddenMarkovModel]");
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
    
bool ContinuousHiddenMarkovModel::predict_(VectorDouble &x){
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &x) - The model is not trained!" << endl;
        return false;
    }
    
    if( x.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &x) - The input vector size (" << x.size() << ") does not match the number of input dimensions (" << numInputDimensions << ")" << endl;
        return false;
    }
    
    //Add the new sample to the circular buffer
    observationSequence.push_back( x );
    
    //Convert the circular buffer to MatrixDouble
    for(unsigned int i=0; i<observationSequence.getSize(); i++){
        for(unsigned int j=0; j<numInputDimensions; j++){
            obsSequence[i][j] = observationSequence[i][j];
        }
    }

    return predict_( obsSequence );
}
  
bool ContinuousHiddenMarkovModel::predict_(MatrixDouble &obs){
    
    if( !trained ){
        errorLog << "predict_(MatrixDouble &obs) - The model is not trained!" << endl;
        return false;
    }
    
    if( obs.getNumCols() != numInputDimensions ){
        errorLog << "predict_(MatrixDouble &obs) - The matrix column size (" << obs.getNumCols() << ") does not match the number of input dimensions (" << numInputDimensions << ")" << endl;
        return false;
    }
    
    const int T = (int)obs.getNumRows();
	int t,i,j = 0;
    double maxValue = 0;
    alpha.resize(T,numStates);
    c.resize(T);
    
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
        maxValue = 0;
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
    loglikelihood = -loglikelihood; //Store the negative log likelihood
    
    //Set the phase as the last estimated state, this will give a phase between [0 1]
    phase = (estimatedStates[T-1]+1.0)/double(numStates);

    return true;
}

bool ContinuousHiddenMarkovModel::train_(TimeSeriesClassificationSample &trainingData){

    //Clear any previous models
    clear();

    //The number of states is simply set as the number of samples in the training sample
    timeseriesLength = trainingData.getLength();
    numStates = (unsigned int)floor(timeseriesLength/downsampleFactor);
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
    
    //TODO: Estimate sigma

    //Setup the observation buffer for prediction
    observationSequence.resize( timeseriesLength, VectorDouble(numInputDimensions,0) );
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
            observationSequence.push_back( VectorDouble(numInputDimensions,0) );
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
    
bool ContinuousHiddenMarkovModel::setSigma(const double sigma){
    if( sigma > 0 ){
        this->sigma = sigma;
        return true;
    }
    return false;
}
    
double ContinuousHiddenMarkovModel::gauss( const MatrixDouble &x, const MatrixDouble &y,const unsigned int i,const unsigned int j,const unsigned int N, const double sigma ){
    double sum = 0;
    for(unsigned int n=0; n<N; n++){
        sum += SQR(  x[i][n] - y[j][n] );
    }
    return exp( -SQR(sqrt( sum )/(2*SQR(sigma))) );
}
    
bool ContinuousHiddenMarkovModel::saveModelToFile(fstream &file) const{
    
    if(!file.is_open())
    {
        errorLog << "saveModelToFile( fstream &file ) - File is not open!" << endl;
        return false;
    }
    
    //Write the header info
    file << "CONTINUOUS_HMM_MODEL_FILE_V1.0\n";
    
    //Write the base settings to the file
    if( !MLBase::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save classifier base settings to file!" << endl;
        return false;
    }
    
    file << "DownsampleFactor: " << downsampleFactor << endl;
    file << "NumStates: " << numStates << endl;
    file << "ClassLabel: " << classLabel << endl;
    file << "TimeseriesLength: " << timeseriesLength << endl;
    file << "Sigma: " << sigma << endl;
    file << "ModelType: " << modelType << endl;
    file << "Delta: " << delta << endl;
    file << "Threshold: " << cThreshold << endl;
    
    if( trained ){
        file << "A:\n";
        for(UINT i=0; i<numStates; i++){
            for(UINT j=0; j<numStates; j++){
                file << a[i][j];
                if( j+1 < numStates ) file << "\t";
            }file << endl;
        }
        
        file << "B:\n";
        for(UINT i=0; i<numStates; i++){
            for(UINT j=0; j<numInputDimensions; j++){
                file << b[i][j];
                if( j+1 < numInputDimensions ) file << "\t";
            }file << endl;
        }
        
        file<<"Pi:\n";
        for(UINT i=0; i<numStates; i++){
            file << pi[i];
            if( i+1 < numStates ) file << "\t";
        }
        file << endl;
    }
    
    return true;
}

bool ContinuousHiddenMarkovModel::loadModelFromFile(fstream &file){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile( fstream &file ) - File is not open!" << endl;
        return false;
    }
    
    std::string word;
    
    file >> word;
    
    cout << "WORD: " << word;
    
    //Find the file type header
    if(word != "CONTINUOUS_HMM_MODEL_FILE_V1.0"){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find Model File Header!" << endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile(file) ){
        errorLog << "loadModelFromFile(string filename) - Failed to load base settings from file!" << endl;
        return false;
    }
    
    file >> word;
    if(word != "DownsampleFactor:"){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find the DownsampleFactor header." << endl;
        return false;
    }
    file >> downsampleFactor;
    
    file >> word;
    if(word != "NumStates:"){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find the NumStates header." << endl;
        return false;
    }
    file >> numStates;
    
    file >> word;
    if(word != "ClassLabel:"){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find the ClassLabel header." << endl;
        return false;
    }
    file >> classLabel;
    
    file >> word;
    if(word != "TimeseriesLength:"){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find the TimeseriesLength header." << endl;
        return false;
    }
    file >> timeseriesLength;
    
    file >> word;
    if(word != "Sigma:"){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find the Sigma for the header." << endl;
        return false;
    }
    file >> sigma;
    
    file >> word;
    if(word != "ModelType:"){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find the ModelType for the header." << endl;
        return false;
    }
    file >> modelType;
    
    file >> word;
    if(word != "Delta:"){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find the Delta for the header." << endl;
        return false;
    }
    file >> delta;
    
    file >> word;
    if(word != "Threshold:"){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find the Threshold for the header." << endl;
        return false;
    }
    file >> cThreshold;
    
    if( trained ){
        a.resize(numStates,numStates);
        b.resize(numStates,numInputDimensions);
        pi.resize(numStates);
        
        //Load the A, B and Pi matrices
        file >> word;
        if(word != "A:"){
            errorLog << "loadModelFromFile( fstream &file ) - Could not find the A matrix header." << endl;
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
            errorLog << "loadModelFromFile( fstream &file ) - Could not find the B matrix header." << endl;
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
            errorLog << "loadModelFromFile( fstream &file ) - Could not find the Pi matrix header." << endl;
            return false;
        }
        
        //Load Pi
        for(UINT i=0; i<numStates; i++){
            file >> pi[i];
        }

        
        //Setup the observation buffer for prediction
        observationSequence.resize( timeseriesLength, VectorDouble(numInputDimensions,0) );
        obsSequence.resize(timeseriesLength,numInputDimensions);
        estimatedStates.resize( numStates );
    }

    return true;
}

}
