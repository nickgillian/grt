/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a discrete Hidden Markov Model.
 
 @example ClassificationModulesExamples/HMMExample/HMMExample.cpp
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

#ifndef GRT_DISCRETE_HIDDEN_MARKOV_MODEL_HEADER
#define GRT_DISCRETE_HIDDEN_MARKOV_MODEL_HEADER

#include "HMMEnums.h"
#include "../../Util/GRTCommon.h"
#include "../../CoreModules/MLBase.h"

namespace GRT {

//This class is used for the HMM batch training
class HMMTrainingObject{
public:
	HMMTrainingObject(){
		pk = 0.0;
	}
	~HMMTrainingObject(){}
	MatrixDouble alpha;     //The forward estimate matrix
	MatrixDouble beta;      //The backward estimate matrix
	VectorDouble c;         //The scaling coefficient vector
	double pk;				//P( O | Model )
};

class DiscreteHiddenMarkovModel : public MLBase {

public:
	DiscreteHiddenMarkovModel();

	DiscreteHiddenMarkovModel(const UINT numStates,const UINT numSymbols,const UINT modelType,const UINT delta);
    
	DiscreteHiddenMarkovModel(const MatrixDouble &a,const MatrixDouble &b,const VectorDouble &pi,const UINT modelType,const UINT delta);
    
    DiscreteHiddenMarkovModel(const DiscreteHiddenMarkovModel &rhs);
    
    virtual ~DiscreteHiddenMarkovModel();
    
    double predict(const UINT newSample);
    double predict(const vector<UINT> &obs);
    
    bool resetModel(const UINT numStates,const UINT numSymbols,const UINT modelType,const UINT delta);
    bool train(const vector< vector<UINT> > &trainingData);
    
    virtual bool reset();
    
    /**
     This saves the trained model to a file.
     
     @param fstream &file: a reference to the file the model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained model from a file.
     
     @param fstream &file: a reference to the file the model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);

    bool randomizeMatrices(const UINT numStates,const UINT numSymbols);
	double predictLogLikelihood(const vector<UINT> &obs);
	bool forwardBackward(HMMTrainingObject &trainingObject,const vector<UINT> &obs);
    bool train_(const vector< vector<UINT> > &obs,const UINT maxIter, UINT &currentIter,double &newLoglikelihood);
    virtual bool print() const;
    
    VectorDouble getTrainingIterationLog() const;
    
	UINT numStates;             //The number of states for this model
	UINT numSymbols;            //The number of symbols for this model
	MatrixDouble a;             //The transitions probability matrix
	MatrixDouble b;             //The emissions probability matrix
	VectorDouble pi;            //The state start probability vector
    VectorDouble trainingIterationLog;   //Stores the loglikelihood at each iteration the BaumWelch algorithm

	UINT modelType;
	UINT delta;				//The number of states a model can move to in a LeftRight model
	UINT numRandomTrainingIterations;		//The number of training loops to find the best starting values
	double logLikelihood;	//The log likelihood of an observation sequence given the modal, calculated by the forward method
	double cThreshold;		//The classification threshold for this model
    CircularBuffer<UINT> observationSequence;
    vector< UINT > estimatedStates;
};

}//end of namespace GRT

#endif //GRT_HIDDEN_MARKOV_MODEL_HEADER
