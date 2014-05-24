/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a container for an ANBC model.
 
 @example ClassificationModulesExamples/ANBCExample/ANBCExample.cpp
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

#ifndef GRT_ANBC_MODEL_HEADER
#define GRT_ANBC_MODEL_HEADER

#include "../../Util/GRTCommon.h"

namespace GRT{

class ANBC_Model{
public:
	ANBC_Model(void){ N=0; classLabel = 0; gamma=2.0; threshold=0.0; trainingMu=0.0; trainingSigma=0.0;};
	~ANBC_Model(void){};

	bool train(UINT classLabel,MatrixDouble &trainingData,VectorDouble &weightsVector);
	double predict(const VectorDouble &observation);
	double predictUnnormed(const VectorDouble &x);
	inline double gauss(const double x,const double mu,const double sigma);
	inline double unnormedGauss(const double x,const double mu,const double sigma);
	void recomputeThresholdValue(const double gamma);

public:
    inline double SQR(double x){ return x*x; }
    
	UINT	N;					//The number of dimensions in the problem
    UINT classLabel;            //The label of the class this model represents
	double threshold;			//The classification threshold value
	double gamma;				//The number of standard deviations to use for the threshold
	double trainingMu;			//The average confidence value in the training data
	double trainingSigma;		//The simga confidence value in the training data
	VectorDouble mu;			//A vector to hold the mean values for each dimension
	VectorDouble sigma;		//A vector to hold the sigma values for each dimension
	VectorDouble weights;		//A vector to hold the weights for each dimension
};

} //End of namespace GRT

#endif //GRT_ANBC_MODEL_HEADER