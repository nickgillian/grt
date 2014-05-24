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

#include "ANBC_Model.h"

namespace GRT{

bool ANBC_Model::train(UINT classLabel,MatrixDouble &trainingData,VectorDouble &weightsVector){

	//Check to make sure the column sizes match
	if( trainingData.getNumCols() != weightsVector.size() ){
		N = 0;
		return false;
	}
    
	UINT M = trainingData.getNumRows();
	N = trainingData.getNumCols();
    this->classLabel = classLabel;

	//Update the weights buffer
	weights = weightsVector;

	//Resize the buffers
	mu.resize( N );
	sigma.resize( N );

	//Calculate the mean for each dimension
	for(UINT j=0; j<N; j++){
		mu[j] = 0.0;

		for(UINT i=0; i<M; i++){
			mu[j] += trainingData[i][j];
        }

		mu[j] /= double(M);
        
        if( mu[j] == 0 ){
            return false;
        }
	}

	//Calculate the sample standard deviation
	for(UINT j=0; j<N; j++){
		sigma[j] = 0.0;

		for(UINT i=0; i<M; i++){
			sigma[j] += SQR( trainingData[i][j]-mu[j] );
        }

		sigma[j] = sqrt( sigma[j]/double(M-1) );
        
        if( sigma[j] == 0 ){
            return false;
        }
	}

	//Now compute the threshold
    double meanPrediction = 0.0;
	VectorDouble predictions(M);
	for(UINT i=0; i<M; i++){
		//Test the ith training example
		vector<double> testData(N);
		for(UINT j=0; j<N; j++) {
			testData[j] = trainingData[i][j];
        }
        
		predictions[i] = predict(testData);
        meanPrediction += predictions[i];
	}

	//Calculate the mean prediction value
	meanPrediction /= double(M);

	//Calculate the standard deviation
	double stdDev = 0.0;
	for(UINT i=0; i<M; i++) {
		stdDev += SQR( predictions[i]-meanPrediction );
    }
	stdDev = sqrt( stdDev / (double(M)-1.0) );

	threshold = meanPrediction-(stdDev*gamma);

	//Update the training mu and sigma values so the threshold value can be dynamically computed at a later stage
	trainingMu = meanPrediction;
	trainingSigma = stdDev;

	return true;
}

double ANBC_Model::predict(const VectorDouble &x){
	double prediction = 0.0;
	for(UINT j=0; j<N; j++){
		if(weights[j]>0)
			prediction += log(gauss(x[j],mu[j],sigma[j]) * weights[j]);
	}
	return prediction;
}

double ANBC_Model::predictUnnormed(const VectorDouble &x){
	double prediction = 0.0;
	for(UINT j=0; j<N; j++){
		if(weights[j]>0)
			prediction += log(unnormedGauss(x[j],mu[j],sigma[j]) * weights[j]);
	}
	return prediction;
}

inline double ANBC_Model::gauss(const double x,const double mu,const double sigma){
	return ( 1.0/(sigma*sqrt(TWO_PI)) ) * exp( - ( ((x-mu)*(x-mu))/(2*(sigma*sigma)) ) );
}

inline double ANBC_Model::unnormedGauss(const double x,const double mu,const double sigma){
	return exp( - ( ((x-mu)*(x-mu))/(2*(sigma*sigma)) ) );
}

void ANBC_Model::recomputeThresholdValue(const double gamma){
	this->gamma = gamma;
	threshold = trainingMu-(trainingSigma*gamma);
}

} //End of namespace GRT