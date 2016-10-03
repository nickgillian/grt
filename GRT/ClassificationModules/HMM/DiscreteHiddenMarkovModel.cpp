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
#include "DiscreteHiddenMarkovModel.h"

GRT_BEGIN_NAMESPACE

//Default constructor
DiscreteHiddenMarkovModel::DiscreteHiddenMarkovModel() : MLBase( "DiscreteHiddenMarkovModel" )
{
	numStates = 0;
	numSymbols = 0;
	delta = 1;
	numRandomTrainingIterations = 5;
	maxNumEpochs = 100;
	cThreshold = -1000;
	modelType = HMM_LEFTRIGHT;
	logLikelihood = 0.0;
	minChange = 1.0e-5;
}

//Init the model with a set number of states and symbols
DiscreteHiddenMarkovModel::DiscreteHiddenMarkovModel(const UINT numStates,const UINT numSymbols,const UINT modelType,const UINT delta) : MLBase( "DiscreteHiddenMarkovModel" )
{
    this->numStates = numStates;
    this->numSymbols = numSymbols;
	this->modelType = modelType;
	this->delta = delta;
	logLikelihood = 0.0;
	numRandomTrainingIterations = 5;
	cThreshold = -1000;
	logLikelihood = 0.0;
    
	randomizeMatrices(numStates,numSymbols);
}

//Init the model with a pre-trained a, b, and pi matrices
DiscreteHiddenMarkovModel::DiscreteHiddenMarkovModel(const MatrixFloat &a,const MatrixFloat &b,const VectorFloat &pi,const UINT modelType,const UINT delta) : MLBase( "DiscreteHiddenMarkovModel" )
{
    
    numStates = 0;
    numSymbols = 0;
    numRandomTrainingIterations = 5;
	maxNumEpochs = 100;
	cThreshold = -1000;
	logLikelihood = 0.0;
	minChange = 1.0e-5;

    if( a.getNumRows() == a.getNumRows() && a.getNumRows() == b.getNumRows() && a.getNumRows() == pi.size() ){
        this->a = a;
        this->b = b;
        this->pi = pi;
        this->modelType = modelType;
        this->delta = delta;
        numStates = b.getNumRows();
        numSymbols = b.getNumCols();
        trained = true;
    }else{
        errorLog << "DiscreteHiddenMarkovModel(...) - The a,b,pi sizes are invalid!" << std::endl;
    }
}
    
DiscreteHiddenMarkovModel::DiscreteHiddenMarkovModel(const DiscreteHiddenMarkovModel &rhs) : MLBase( "DiscreteHiddenMarkovModel" )
{
    this->numStates = rhs.numStates;
	this->numSymbols = rhs.numSymbols;
	this->delta = rhs.delta;
	this->numRandomTrainingIterations = rhs.numRandomTrainingIterations;
	this->cThreshold = rhs.cThreshold;
	this->modelType = rhs.modelType;
	this->logLikelihood = rhs.logLikelihood;
	this->a = rhs.a;
	this->b = rhs.b;
	this->pi = rhs.pi;
    this->trainingLog = rhs.trainingLog;
}
    
//Default destructor
DiscreteHiddenMarkovModel::~DiscreteHiddenMarkovModel(){
    
}

//This can be called at any time to reset the entire model
bool DiscreteHiddenMarkovModel::resetModel(const UINT numStates,const UINT numSymbols,const UINT modelType,const UINT delta){
	this->numStates = numStates;
    this->numSymbols = numSymbols;
	this->modelType = modelType;
	this->delta = delta;
	return randomizeMatrices(numStates,numSymbols);
}

bool DiscreteHiddenMarkovModel::randomizeMatrices(const UINT numStates,const UINT numSymbols){

	//Set the model as untrained as everything will now be reset
	trained = false;
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

	//Normalize the matrices
	Float sum=0.0;
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
    
Float DiscreteHiddenMarkovModel::predict(const UINT newSample){
    
    if( !trained ){
        return 0;
    }
    
    observationSequence.push_back( newSample );
    
    Vector< UINT > obs = observationSequence.getData();
    
    return predict(obs);
}
  
/*Float predictLogLikelihood(Vector<UINT> &obs)
 - This method computes P(O|A,B,Pi) using the forward algorithm
 */
Float DiscreteHiddenMarkovModel::predict(const Vector<UINT> &obs){
    
	const int N = (int)numStates;
    const int T = (int)obs.size();
	int t,i,j = 0;
    MatrixFloat alpha(T,numStates);
    VectorFloat c(T);
    
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
        Float maxValue = 0;
        for(i=0; i<N; i++){
            if( alpha[t][i] > maxValue ){
                maxValue = alpha[t][i];
                estimatedStates[t] = i;
            }
        }
    }
    
	//Termination
	Float loglikelihood = 0.0;
    for(t=0; t<T; t++) loglikelihood += log( c[t] );
    return -loglikelihood; //Return the negative log likelihood
}

/*Float predictLogLikelihood(Vector<UINT> &obs)
- This method computes P(O|A,B,Pi) using the forward algorithm
*/
Float DiscreteHiddenMarkovModel::predictLogLikelihood(const Vector<UINT> &obs){

	const UINT T = (unsigned int)obs.size();
	UINT t,i,j,minState = 0;
	MatrixFloat alpha(T,numStates);
    Float minWeight = 0;
    Float weight = 0;

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

/*Float forwardBackward(Vector<UINT> &obs)
- This method runs one pass of the forward backward algorithm, the hmm training object needs to be resized BEFORE calling this function!
*/
bool DiscreteHiddenMarkovModel::forwardBackward(HMMTrainingObject &hmm,const Vector<UINT> &obs){

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
    
    if( grt_isinf(hmm.pk) ){
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
bool DiscreteHiddenMarkovModel::train(const Vector< Vector<UINT> > &trainingData){

    //Clear any previous models
    trained = false;
    observationSequence.clear();
    estimatedStates.clear();
    trainingIterationLog.clear();
    
	UINT n,currentIter, bestIndex = 0;
	Float newLoglikelihood, bestLogValue = 0;
    
    if( numRandomTrainingIterations > 1 ){

        //A buffer to keep track each AB matrix
        Vector< MatrixFloat > aTracker( numRandomTrainingIterations );
        Vector< MatrixFloat > bTracker( numRandomTrainingIterations );
        Vector< Float > loglikelihoodTracker( numRandomTrainingIterations );
        
        UINT maxNumTestIter = maxNumEpochs > 10 ? 10 : maxNumEpochs;

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
    if( !train_(trainingData,maxNumEpochs,currentIter,newLoglikelihood) ){
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
    
    averageObsLength = (UINT)floor( averageObsLength/Float(numObs) );
    observationSequence.resize( averageObsLength );
    estimatedStates.resize( averageObsLength );
    
    //Finally, flag that the model was trained
    trained = true;

	return true;
}

bool DiscreteHiddenMarkovModel::train_(const Vector< Vector<UINT> > &obs,const UINT maxIter, UINT &currentIter,Float &newLoglikelihood){
    
    const UINT numObs = (unsigned int)obs.size();
    UINT i,j,k,t = 0;
    Float num,denom,oldLoglikelihood = 0;
    bool keepTraining = true;
    trainingIterationLog.clear();
    
    //Create the array to hold the data for each training instance
    Vector< HMMTrainingObject > hmms( numObs );
    
    //Create epislon and gamma to hold the re-estimation variables
    Vector< Vector< MatrixFloat > > epsilon( numObs );
    Vector< MatrixFloat > gamma( numObs );
    
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
        
        if( ++currentIter >= maxIter ){ keepTraining = false; trainingLog << "Max Iter Reached! Stopping Training" << std::endl; }
        if( fabs(newLoglikelihood-oldLoglikelihood) < minChange && currentIter > 1 ){ keepTraining = false; trainingLog << "Min Improvement Reached! Stopping Training" << std::endl; }
        //if( newLoglikelihood < oldLoglikelihood ){ cout<<"Warning: Inverted Training!\n";}
        
        trainingLog << "Iter: " << currentIter << " logLikelihood: " << newLoglikelihood << " change: " << oldLoglikelihood - newLoglikelihood << std::endl;
        
        print();
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
                    errorLog << "Denom is zero for A!" << std::endl;
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
                        errorLog << "Denominator is zero for B!" << std::endl;
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
                Float sum;
                for (UINT i=0; i<numStates; i++) {
                    sum = 0.;
                    for (UINT k=0; k<numSymbols; k++){
                        b[i][k] += 1.0/numSymbols; //Add a small value to B to make sure the value will not be zero
                        sum += b[i][k];
                    }
                    for (UINT k=0; k<numSymbols; k++) b[i][k] /= sum;
                }
            }
            
            //Re-estimate Pi - only if the model type is HMM_ERGODIC, otherwise Pi[0] == 1 and everything else is 0
            if (modelType==HMM_ERGODIC ){
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
                
                Float sum = 0;
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
    
bool DiscreteHiddenMarkovModel::reset(){

    for(UINT i=0; i<observationSequence.getSize(); i++){
        observationSequence.push_back( 0 );
    }
    
    return true;
}
    
bool DiscreteHiddenMarkovModel::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog << "save( fstream &file ) - File is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "DISCRETE_HMM_MODEL_FILE_V1.0\n";
    
    //Write the base settings to the file
    if( !MLBase::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    file << "NumStates: " << numStates << std::endl;
    file << "NumSymbols: " << numSymbols << std::endl;
    file << "ModelType: " << modelType << std::endl;
    file << "Delta: " << delta << std::endl;
    file << "Threshold: " << cThreshold << std::endl;
    file << "NumRandomTrainingIterations: " << numRandomTrainingIterations << std::endl;
    
    file << "A:\n";
    for(UINT i=0; i<numStates; i++){
        for(UINT j=0; j<numStates; j++){
            file << a[i][j];
            if( j+1 < numStates ) file << "\t";
        }file << std::endl;
    }
    
    file << "B:\n";
    for(UINT i=0; i<numStates; i++){
        for(UINT j=0; j<numSymbols; j++){
            file << b[i][j];
            if( j+1 < numSymbols ) file << "\t";
        }file << std::endl;
    }
    
    file<<"Pi:\n";
    for(UINT i=0; i<numStates; i++){
        file << pi[i];
        if( i+1 < numStates ) file << "\t";
    }
    file << std::endl;
    
    return true;
}

bool DiscreteHiddenMarkovModel::load( std::fstream &file ){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "load( fstream &file ) - File is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    file >> word;
    
    //Find the file type header
    if(word != "DISCRETE_HMM_MODEL_FILE_V1.0"){
        errorLog << "load( fstream &file ) - Could not find Model File Header!" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    file >> word;
    if(word != "NumStates:"){
        errorLog << "load( fstream &file ) - Could not find the NumStates header." << std::endl;
        return false;
    }
    file >> numStates;
    
    file >> word;
    if(word != "NumSymbols:"){
        errorLog << "load( fstream &file ) - Could not find the NumSymbols header." << std::endl;
        return false;
    }
    file >> numSymbols;
    
    file >> word;
    if(word != "ModelType:"){
        errorLog << "load( fstream &file ) - Could not find the modelType for the header." << std::endl;
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
    
    file >> word;
    if(word != "NumRandomTrainingIterations:"){
        errorLog << "load( fstream &file ) - Could not find the numRandomTrainingIterations header." << std::endl;
        return false;
    }
    file >> numRandomTrainingIterations;
    
    a.resize(numStates,numStates);
    b.resize(numStates,numSymbols);
    pi.resize(numStates);
    
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
        for(UINT j=0; j<numSymbols; j++){
            file >> b[i][j];
        }
    }
    
    file >> word;
    if(word != "Pi:"){
        errorLog << "load( fstream &file ) - Could not find the Pi matrix header." << std::endl;
        return false;
    }
    
    //Load Pi
    for(UINT i=0; i<numStates; i++){
        file >> pi[i];
    }
    
    return true;
}

bool DiscreteHiddenMarkovModel::print() const{

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
	for(UINT i=0; i<pi.size(); i++){
        trainingLog << pi[i] << "\t";
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

        for(UINT i=0; i<b.getNumRows(); i++){
          sum=0.0;
          for(UINT j=0; j<b.getNumCols(); j++) sum += b[i][j];
          if( sum <= 0.99 || sum >= 1.01 ) warningLog << "WARNING: B Row " << i << " Sum: " << sum << std::endl;
        }
    }

    return true;
}
    
VectorFloat DiscreteHiddenMarkovModel::getTrainingIterationLog() const{
    return trainingIterationLog;
}

GRT_END_NAMESPACE

