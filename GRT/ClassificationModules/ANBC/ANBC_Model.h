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

#include "../../DataStructures/VectorFloat.h"
#include "../../DataStructures/MatrixFloat.h"

GRT_BEGIN_NAMESPACE

class GRT_API ANBC_Model{
public:
	ANBC_Model(void){ N=0; classLabel = 0; gamma=2.0; threshold=0.0; trainingMu=0.0; trainingSigma=0.0;};
	~ANBC_Model(void){};
	
	bool train( const UINT classLabel, const MatrixDouble &trainingData, const VectorFloat &weightsVector );
	Float predict( const VectorFloat &x );
	Float predictUnnormed( const VectorFloat &x );
	inline Float gauss(const Float x,const Float mu,const Float sigma);
	inline Float unnormedGauss(const Float x,const Float mu,const Float sigma);
	void recomputeThresholdValue(const Float gamma);
	
public:
	UINT	N;					//The number of dimensions in the problem
	UINT classLabel;            //The label of the class this model represents
	Float threshold;			//The classification threshold value
	Float gamma;				//The number of standard deviations to use for the threshold
	Float trainingMu;			//The average confidence value in the training data
	Float trainingSigma;		//The simga confidence value in the training data
	VectorFloat mu;			//A vector to hold the mean values for each dimension
	VectorFloat sigma;		//A vector to hold the sigma values for each dimension
	VectorFloat weights;		//A vector to hold the weights for each dimension
};

GRT_END_NAMESPACE

#endif //GRT_ANBC_MODEL_HEADER
	