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

#include "HiddenMarkovModel.h"

namespace GRT {

//Default constructor
HiddenMarkovModel::HiddenMarkovModel(){
	numStates = 0;
	numSymbols = 0;
	delta = 1;
	numRandomTrainingIterations = 5;
	maxNumIter = 100;
	cThreshold = -1000;
	modelTrained = false;
	modelType = ERGODIC;
	logLikelihood = 0.0;
	minImprovement = 1.0e-5;
    
    debugLog.setProceedingText("[DEBUG HiddenMarkovModel]");
    errorLog.setProceedingText("[ERROR HiddenMarkovModel]");
    warningLog.setProceedingText("[WARNING HiddenMarkovModel]");
    trainingLog.setProceedingText("[TRAINING HiddenMarkovModel]");
}

//Init the model with a set number of states and symbols
HiddenMarkovModel::HiddenMarkovModel(const UINT numStates,const UINT numSymbols,const UINT modelType,const UINT delta){
    this->numStates = numStates;
    this->numSymbols = numSymbols;
	this->modelType = modelType;
	this->delta = delta;
	logLikelihood = 0.0;
	numRandomTrainingIterations = 5;
	maxNumIter = 100;
	cThreshold = -1000;
	logLikelihood = 0.0;
	minImprovement = 1.0e-5;
    
    debugLog.setProceedingText("[DEBUG HiddenMarkovModel]");
    errorLog.setProceedingText("[ERROR HiddenMarkovModel]");
    warningLog.setProceedingText("[WARNING HiddenMarkovModel]");
    trainingLog.setProceedingText("[TRAINING HiddenMarkovModel]");
    
	randomizeMatrices(numStates,numSymbols);
}

//Init the model with a pre-trained a, b, and pi matrices
HiddenMarkovModel::HiddenMarkovModel(const MatrixDouble &a,const MatrixDouble &b,const VectorDouble &pi,const UINT modelType,const UINT delta){
    
    numStates = 0;
    numSymbols = 0;
    numRandomTrainingIterations = 5;
	maxNumIter = 100;
	cThreshold = -1000;
	logLikelihood = 0.0;
	minImprovement = 1.0e-5;
	modelTrained = false;
    
    debugLog.setProceedingText("[DEBUG HiddenMarkovModel]");
    errorLog.setProceedingText("[ERROR HiddenMarkovModel]");
    warningLog.setProceedingText("[WARNING HiddenMarkovModel]");
    trainingLog.setProceedingText("[TRAINING HiddenMarkovModel]");

    if( a.getNumRows() == a.getNumRows() && a.getNumRows() == b.getNumRows() && a.getNumRows() == pi.size() ){
        this->a = a;
        this->b = b;
        this->pi = pi;
        this->modelType = modelType;
        this->delta = delta;
        numStates = b.getNumRows();
        numSymbols = b.getNumCols();
        modelTrained = true;
    }else{
        errorLog << "HiddenMarkovModel(...) - The a,b,pi sizes are invalid!" << endl;
    }
}
    
HiddenMarkovModel::HiddenMarkovModel(const HiddenMarkovModel &rhs){
    this->numStates = rhs.numStates;
	this->numSymbols = rhs.numSymbols;
	this->delta = rhs.delta;
	this->numRandomTrainingIterations = rhs.numRandomTrainingIterations;
	this->maxNumIter = rhs.maxNumIter;
	this->cThreshold = rhs.cThreshold;
	this->modelTrained = rhs.modelTrained;
	this->modelType = rhs.modelType;
	this->logLikelihood = rhs.logLikelihood;
	this->minImprovement = rhs.minImprovement;
	this->a = rhs.a;
	this->b = rhs.b;
	this->pi = rhs.pi;
    this->trainingLog = rhs.trainingLog;

    debugLog.setProceedingText("[DEBUG HiddenMarkovModel]");
    errorLog.setProceedingText("[ERROR HiddenMarkovModel]");
    warningLog.setProceedingText("[WARNING HiddenMarkovModel]");
    trainingLog.setProceedingText("[TRAINING HiddenMarkovModel]");
}
    
//Default destructor
HiddenMarkovModel::~HiddenMarkovModel(){
    
}

//This can be called at any time to reset the entire model
bool HiddenMarkovModel::resetModel(const UINT numStates,const UINT numSymbols,const UINT modelType,const UINT delta){
	this->numStates = numStates;
    this->numSymbols = numSymbols;
	this->modelType = modelType;
	this->delta = delta;
	return randomizeMatrices(numStates,numSymbols);
}

bool HiddenMarkovModel::randomizeMatrices(const UINT numStates,const UINT numSymbols){

	//Set the model as untrained as everything will now be reset
	modelTrained = false;
	logLikelihood = 0.0;

	//Set the new state and symbol size
	this->numStates = numStates;
	this->numSymbols = numSymbols;
	a.resize(numStates,numStates);
	b.resize(numStates,numSymbols);
	pi.resize(numStates);

	//Fill Transition and Symbol Matrices randomly
    //It's best to choose values in the range [0.9 1.1] rather than [0 1]
    //That way, no single value will get too large or too small a weight when the values are normalized
	Random random;
	for(UINT i=0; i<a.getNumRows(); i++)
		for(UINT j=0; j<a.getNumCols(); j++)
			a[i][j] = random.getRandomNumberUniform(0.9,1);

	for(UINT i=0; i<b.getNumRows(); i++)
		for(UINT j=0; j<b.getNumCols(); j++)
			b[i][j] = random.getRandomNumberUniform(0.9,1);
    
    //Randomise pi
	for(UINT i=0; i<numStates; i++)
        pi[i] = random.getRandomNumberUniform(0.9,1);
    
    //Set any constraints on the model
	switch( modelType ){
		case(ERGODIC):
			//Don't need todo anything
			break;
		case(LEFTRIGHT):
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

	//Normalize the matrices
	double sum=0.0;
	for (UINT i=0; i<numStates; i++) {
		sum = 0.;
		for (UINT j=0; j<numStates; j++) sum += a[i][j];
		for (UINT j=0; j<numStates; j++) a[i][j] /= sum;
	}
	for (UINT i=0; i<numStates; i++) {
		sum = 0.;
		for (UINT k=0; k<numSymbols; k++) sum += b[i][k];
		for (UINT k=0; k<numSymbols; k++) b[i][k] /= sum;
	}
    
	//Normalise pi
	sum = 0.0;
	for (UINT i=0; i<numStates; i++) sum += pi[i];
	for (UINT i=0; i<numStates; i++) pi[i] /= sum;
    
    return true;
}
    
double HiddenMarkovModel::predict(const UINT newSample){
    
    if( !modelTrained ){
        return 0;
    }
    
    observationSequence.push_back( newSample );
    
    vector< UINT > obs = observationSequence.getDataAsVector();
    
    return predict(obs);
}
  
/*double predictLogLikelihood(Vector<UINT> &obs)
 - This method computes P(O|A,B,Pi) using the forward algorithm
 */
double HiddenMarkovModel::predict(const vector<UINT> &obs){
    
	const int N = (int)numStates;
    const int T = (int)obs.size();
	int t,i,j = 0;
    MatrixDouble alpha(T,numStates);
    VectorDouble c(T);
    
	////////////////// Run the forward algorithm ////////////////////////
	//Step 1: Init at t=0
	t = 0;
	c[t] = 0.0;
	for(i=0; i<N; i++){
		alpha[t][i] = pi[i]*b[i][ obs[t] ];
		c[t] += alpha[t][i];
	}
    
	//Set the inital scaling coeff
	c[t] = 1.0/c[t];
    
	//Scale alpha
    for(i=0; i<N; i++) alpha[t][i] *= c[t];
    
	//Step 2: Induction
	for(t=1; t<T; t++){
		c[t] = 0.0;
		for(j=0; j<N; j++){
			alpha[t][j] = 0.0;
			for(i=0; i<N; i++){
				alpha[t][j] +=  alpha[t-1][i] * a[i][j];
			}
            alpha[t][j] *= b[j][obs[t]];
            c[t] += alpha[t][j];
		}
        
		//Set the scaling coeff
		c[t] = 1.0/c[t];
        
		//Scale Alpha
        for(j=0; j<N; j++) alpha[t][j] *= c[t];
	}
    
    if( int(estimatedStates.size()) != T ) estimatedStates.resize(T);
    for(t=0; t<T; t++){
        double maxValue = 0;
        for(i=0; i<N; i++){
            if( alpha[t][i] > maxValue ){
                maxValue = alpha[t][i];
                estimatedStates[t] = i;
            }
        }
    }
    
	//Termination
	double loglikelihood = 0.0;
    for(t=0; t<T; t++) loglikelihood += log( c[t] );
    return -loglikelihood; //Return the negative log likelihood
}

/*double predictLogLikelihood(Vector<UINT> &obs)
- This method computes P(O|A,B,Pi) using the forward algorithm
*/
double HiddenMarkovModel::predictLogLikelihood(const vector<UINT> &obs){

	const UINT T = (unsigned int)obs.size();
	UINT t,i,j,minState = 0;
	MatrixDouble alpha(T,numStates);
    double minWeight = 0;
    double weight = 0;

    // Base
	t = 0;
    for(i=0; i<numStates; i++){
        alpha[t][i] = (-1.0 * log(pi[i])) - log(b[i][ obs[t] ]);
    }

    // Induction
    for (t=1; t<T; t++){
        for (j=0; j<numStates; j++){
            minState = 0;
            minWeight = alpha[t-1][j] - log(a[0][j]);

            for (i=1; i<numStates; i++){
                weight = alpha[t-1][i] - log(a[i][j]);

                if (weight < minWeight)
                {
                    minState = i;
                    minWeight = weight;
                }
           }

            alpha[t][j] = minWeight - log(b[j][ obs[t] ]);
        }
    }

    // Find minimum value for time T-1
    minState = 1;
    minWeight = alpha[T - 1][0];

    for(i=1; i<numStates; i++)
    {
        if (alpha[T-1][i] < minWeight)
        {
            minState = i;
            minWeight = alpha[T-1][i];
        }
    }

    // Returns the sequence probability
    return exp(-minWeight);
}

/*double forwardBackward(Vector<UINT> &obs)
- This method runs one pass of the forward backward algorithm, the hmm training object needs to be resized BEFORE calling this function!
*/
bool HiddenMarkovModel::forwardBackward(HMMTrainingObject &hmm,const vector<UINT> &obs){

	const int N = (int)numStates;
	const int T = (int)obs.size();
	int t,i,j = 0;

	////////////////// Run the forward algorithm ////////////////////////
	//Step 1: Init at t=0
	t = 0;
	hmm.c[t] = 0.0;
	for(i=0; i<N; i++){
		hmm.alpha[t][i] = pi[i]*b[i][ obs[t] ];
		hmm.c[t] += hmm.alpha[t][i];
	}

	//Set the inital scaling coeff
	hmm.c[t] = 1.0/hmm.c[t];

	//Scale alpha
    for(i=0; i<N; i++) hmm.alpha[t][i] *= hmm.c[t];
    
	//Step 2: Induction
	for(t=1; t<T; t++){
		hmm.c[t] = 0.0;
		for(j=0; j<N; j++){
			hmm.alpha[t][j] = 0.0;
			for(i=0; i<N; i++){
				hmm.alpha[t][j] +=  hmm.alpha[t-1][i] * a[i][j];
			}
            hmm.alpha[t][j] *= b[j][obs[t]];
            hmm.c[t] += hmm.alpha[t][j];
		}

		//Set the scaling coeff
		hmm.c[t] = 1.0/hmm.c[t];

		//Scale Alpha
        for(j=0; j<N; j++) hmm.alpha[t][j] *= hmm.c[t];
	}

	//Termination
	hmm.pk = 0.0;
    for(t=0; t<T; t++) hmm.pk += log( hmm.c[t] );
    //hmm.pk = - hmm.pk; //We don't really need to minus here
    
    if( std::isinf(hmm.pk) ){
        return false;
    }
    
	////////////////// Run the backward algorithm ////////////////////////
	//Step 1: Init at time t=T (T-1 as everything is zero based)
	t = T-1;
	for(i=0; i<N; i++) hmm.beta[t][i] = 1.0;
    
	//Scale beta, using the same coeff as A
    for(i=0; i<N; i++) hmm.beta[t][i] *= hmm.c[t];

	//Step 2: Induction, from T-1 until 1 (T-2 until 0 as everything is zero based)
	for(t=T-2; t>=0; t--){
		for(i=0; i<N; i++){
			//Calculate the backward step for t, using the scaled beta
			hmm.beta[t][i]=0.0;
			for(j=0; j<N; j++)
				hmm.beta[t][i] += a[i][j] * b[j][ obs[t+1] ] * hmm.beta[t+1][j];

            //Scale B using the same coeff as A
            hmm.beta[t][i] *= hmm.c[t];
		}
	}
    
    return true;
}

/*bool batchTrain(Vector<UINT> &obs)
- This method 
*/
bool HiddenMarkovModel::train(const vector< vector<UINT> > &trainingData){

    //Clear any previous models
    modelTrained = false;
    observationSequence.clear();
    estimatedStates.clear();
    trainingIterationLog.clear();
    
	UINT n,currentIter, bestIndex = 0;
	double newLoglikelihood, bestLogValue = 0;
    
    if( numRandomTrainingIterations > 1 ){

        //A buffer to keep track each AB matrix
        vector< MatrixDouble > aTracker( numRandomTrainingIterations );
        vector< MatrixDouble > bTracker( numRandomTrainingIterations );
        vector< double > loglikelihoodTracker( numRandomTrainingIterations );
        
        UINT maxNumTestIter = maxNumIter > 10 ? 10 : maxNumIter;

        //Try and find the best starting point
        for(n=0; n<numRandomTrainingIterations; n++){
            //Reset the model to a new random starting values
            randomizeMatrices(numStates,numSymbols);

            if( !train_(trainingData,maxNumTestIter,currentIter,newLoglikelihood) ){
                return false;
            }
            aTracker[n] = a;
            bTracker[n] = b;
            loglikelihoodTracker[n] = newLoglikelihood;
        }

        //Get the best result and set it as the a and b starting values
        bestIndex = 0;
        bestLogValue = loglikelihoodTracker[0];
        for(n=1; n<numRandomTrainingIterations; n++){
            if(bestLogValue < loglikelihoodTracker[n]){
                bestLogValue = loglikelihoodTracker[n];
                bestIndex = n;
            }
        }

        //Set a and b
        a = aTracker[bestIndex];
        b = bTracker[bestIndex];
        
    }else{
        randomizeMatrices(numStates,numSymbols);
    }

	//Perform the actual training
    if( !train_(trainingData,maxNumIter,currentIter,newLoglikelihood) ){
        return false;
    }

	//Calculate the observationSequence buffer length
	const UINT numObs = (unsigned int)trainingData.size();
	UINT k = 0;
    UINT averageObsLength = 0;
	for(k=0; k<numObs; k++){
		const UINT T = (unsigned int)trainingData[k].size();
		averageObsLength += T;
	}
    
    averageObsLength = (UINT)floor( averageObsLength/double(numObs) );
    observationSequence.resize( averageObsLength );
    estimatedStates.resize( averageObsLength );
    
    //Finally, flag that the model was trained
    modelTrained = true;

	return true;
}

bool HiddenMarkovModel::train_(const vector< vector<UINT> > &obs,const UINT maxIter, UINT &currentIter,double &newLoglikelihood){
    
    const UINT numObs = (unsigned int)obs.size();
    UINT i,j,k,t = 0;
    double num,denom,oldLoglikelihood = 0;
    bool keepTraining = true;
    trainingIterationLog.clear();
    
    //Create the array to hold the data for each training instance
    vector< HMMTrainingObject > hmms( numObs );
    
    //Create epislon and gamma to hold the re-estimation variables
    vector< vector< MatrixDouble > > epsilon( numObs );
    vector< MatrixDouble > gamma( numObs );
    
    //Resize the hmms, epsilon and gamma matrices so they are ready to be filled
    for(k=0; k<numObs; k++){
        const UINT T = (UINT)obs[k].size();
        gamma[k].resize(T,numStates);
        epsilon[k].resize(T);
        for(t=0; t<T; t++) epsilon[k][t].resize(numStates,numStates);
        
        //Resize alpha, beta and phi
        hmms[k].alpha.resize(T,numStates);
        hmms[k].beta.resize(T,numStates);
        hmms[k].c.resize(T);
    }
    
    //For each training seq, run one pass of the forward backward
    //algorithm then reestimate a and b using the Baum-Welch
    oldLoglikelihood = 0;
    newLoglikelihood = 0;
    currentIter = 0;
    
    do{
        newLoglikelihood = 0.0;
        
        //Run the forwardbackward algorithm for each training example
        for(k=0; k<numObs; k++){
            if( !forwardBackward(hmms[k],obs[k]) ){
                return false;
            }
            newLoglikelihood += hmms[k].pk;
        }
        
        //Set the new log likelihood as the average of the observations
        newLoglikelihood /= numObs;
        
        trainingIterationLog.push_back( newLoglikelihood );
        
        if( ++currentIter >= maxIter ){ keepTraining = false; trainingLog << "Max Iter Reached! Stopping Training" << endl; }
        if( fabs(newLoglikelihood-oldLoglikelihood) < minImprovement && currentIter > 1 ){ keepTraining = false; trainingLog << "Min Improvement Reached! Stopping Training" << endl; }
        //if( newLoglikelihood < oldLoglikelihood ){ cout<<"Warning: Inverted Training!\n";}
        
        trainingLog << "Iter: "<<currentIter<<" logLikelihood: "<<newLoglikelihood<<" change: "<<oldLoglikelihood - newLoglikelihood<<endl;
        
        printMatrices();
        //PAUSE;
        
        oldLoglikelihood = newLoglikelihood;
        
        //Only update A, B, and Pi if needed
        if( keepTraining ){

            //Re-estimate A
            for(i=0; i<numStates; i++){
                
                //Compute the denominator of A (which is independent of j)
                denom = 0;
                for(k=0; k<numObs; k++){
                    for(t=0; t<obs[k].size()-1; t++){
                        denom += hmms[k].alpha[t][i] * hmms[k].beta[t][i] / hmms[k].c[t];
                    }
                }
                
                //Compute the numerator and also update a[i][j]
                if( denom > 0 ){
                    for(j=0; j<numStates; j++){
                        num = 0;
                        for(k=0; k<numObs; k++){
                            for(t=0; t<obs[k].size()-1; t++){
                                num += hmms[k].alpha[t][i] * a[i][j] * b[j][ obs[k][t+1] ] * hmms[k].beta[t+1][j];
                            }
                        }
                    
                        //Update a[i][j]
                        a[i][j] = num/denom;
                    }
                }else{
                    errorLog << "Denom is zero for A!" << endl;
                    return false;
                }
            }
            
            //Re-estimate B
            bool renormB = false;
            for(i=0; i<numStates; i++){
                for(j=0; j<numSymbols; j++){
                    num=0.0;
                    denom = 0.0;
                    for(k=0; k<numObs; k++){
                        const UINT T = (unsigned int)obs[k].size();
                        for(t=0; t<T; t++){
                            if( obs[k][t] == j ){
                                num += hmms[k].alpha[t][i] * hmms[k].beta[t][i] / hmms[k].c[t];
                            }
                            denom += hmms[k].alpha[t][i] * hmms[k].beta[t][i] / hmms[k].c[t];
                        }
                    }
                    
                    if( denom == 0 ){
                        errorLog << "Denominator is zero for B!" << endl;
                        return false;
                    }
                    //Update b[i][j]
                    //If there are no observations at all for a state then the probabilities will be zero which is bad
                    //So instead we flag that B needs to be renormalized later
                    if( num > 0 ) b[i][j] = denom > 0 ? num/denom : 1.0e-5;
                    else{ b[i][j] = 0; renormB = true; }
                }
            }
            
            if( renormB ){
                double sum;
                for (UINT i=0; i<numStates; i++) {
                    sum = 0.;
                    for (UINT k=0; k<numSymbols; k++){
                        b[i][k] += 1.0/numSymbols; //Add a small value to B to make sure the value will not be zero
                        sum += b[i][k];
                    }
                    for (UINT k=0; k<numSymbols; k++) b[i][k] /= sum;
                }
            }
            
            //Re-estimate Pi - only if the model type is ERGODIC, otherwise Pi[0] == 1 and everything else is 0
            if (modelType==ERGODIC ){
                for(k=0; k<numObs; k++){
                    const UINT T = (unsigned int)obs[k].size();
                    //Compute epsilon
                    for(t=0; t<T-1; t++){
                        denom = 0.0;
                        for(i=0; i<numStates; i++){
                            for(j=0; j<numStates; j++){
                                epsilon[k][t][i][j] = hmms[k].alpha[t][i] * a[i][j] * b[j][ obs[k][t+1] ] * hmms[k].beta[t+1][j];
                                denom += epsilon[k][t][i][j];
                            }
                        }
                        //Normalize Epsilon
                        for(i=0; i<numStates; i++){
                            for(j=0; j<numStates; j++){
                                if(denom!=0) epsilon[k][t][i][j] /= denom;
                                else{ epsilon[k][t][i][j] = 0; }
                            }
                        }
                    }
                    
                    //Compute gamma
                    for(t=0; t<T-1; t++){
                        for(i=0; i<numStates; i++){
                            gamma[k][t][i]= 0.0;
                            for(j=0; j<numStates; j++)
                                gamma[k][t][i] += epsilon[k][t][i][j];
                        }
                    }
                }
                
                double sum = 0;
                for(i=0; i<numStates; i++){
                    sum=0.0;
                    for(k=0; k<numObs; k++){
                        sum += gamma[k][0][i];
                    }
                    pi[i] = sum / numObs;
                }
            }
        }
        
    }while(keepTraining);
    
    return true;
    
}
    
bool HiddenMarkovModel::reset(){

    for(UINT i=0; i<observationSequence.getSize(); i++){
        observationSequence.push_back( 0 );
    }
    
    return true;
}

void HiddenMarkovModel::printMatrices(){

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

}
    
VectorDouble HiddenMarkovModel::getTrainingIterationLog() const{
    return trainingIterationLog;
}

}
