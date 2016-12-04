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
#include "Neuron.h"

GRT_BEGIN_NAMESPACE

Neuron::Neuron(){
	activationFunction = LINEAR;
	numInputs = 0;
	gamma = 2.0;
	bias = 0;
    previousBiasUpdate = 0;
}

Neuron::Neuron( const Neuron &rhs ){
    this->gamma = rhs.gamma;
    this->bias = rhs.bias;
    this->previousBiasUpdate = rhs.previousBiasUpdate;
    this->weights = rhs.weights;
    this->previousUpdate = rhs.previousUpdate;
    this->numInputs = rhs.numInputs;
    this->activationFunction = rhs.activationFunction;
}

Neuron::~Neuron(){}

Neuron& Neuron::operator=(const Neuron &rhs){
    if( this != &rhs ){
        this->gamma = rhs.gamma;
        this->bias = rhs.bias;
        this->previousBiasUpdate = rhs.previousBiasUpdate;
        this->weights = rhs.weights;
        this->previousUpdate = rhs.previousUpdate;
        this->numInputs = rhs.numInputs;
        this->activationFunction = rhs.activationFunction;
    }

    return *this;
}

bool Neuron::init(const UINT numInputs,const Type activationFunction,const Float minWeightRange, const Float maxWeightRange){
    
    if( !validateActivationFunction(activationFunction) ){
        return false;
    }
    
    this->numInputs = numInputs;
    this->activationFunction = activationFunction;
    
    weights.resize(numInputs);
	previousUpdate.resize(numInputs);
    
    //Set the random seed
    Random random;
    //random.setSeed( (unsigned long long)time(NULL) );
    
    //Randomise the weights between [-0.1 0.1]
    //Note, it's better to set the random values using small weights rather than [-1.0 1.0]
    for(unsigned int i=0; i<numInputs; i++){
        weights[i] = random.getRandomNumberUniform(minWeightRange,maxWeightRange);
		previousUpdate[i] = 0;
    }

	//Randomise the bias between [-0.1 0.1]
    bias = random.getRandomNumberUniform(minWeightRange,maxWeightRange);
    
    return true;
}

void Neuron::clear(){
    numInputs = 0;
	bias = 0;
    previousBiasUpdate = 0;
    weights.clear();
	previousUpdate.clear();
}

Float Neuron::fire(const VectorFloat &x){
    
    Float y = 0;
    UINT i=0;
    
    switch( activationFunction ){
        case(LINEAR):
            y = bias;
            for(i=0; i<numInputs; i++){
                y += x[i] * weights[i];
            }
            break;
        case(SIGMOID):
            y = bias;
            for(i=0; i<numInputs; i++){
                y += x[i] * weights[i];
            }
            
            //Trick for stopping overflow
            /*
			if( y < -45.0 ){ y = 0; }
			else if( y > 45.0 ){ y = 1.0; }
			else{
				y = 1.0/(1.0+exp(-y));
			}
            */
            y = 1.0/(1.0+exp(-y));
            break;
        case(BIPOLAR_SIGMOID):
            y = bias;
            for(i=0; i<numInputs; i++){
                y += x[i] * weights[i];
            }
	
            /*
            if( y < -45.0 ){ y = 0; }
			else if( y > 45.0 ){ y = 1.0; }
			else{
				y = (2.0 / (1.0 + exp(-gamma * y))) - 1.0;
			}
            */
            y = (2.0 / (1.0 + exp(-gamma * y))) - 1.0;
            break;
        case(TANH):
            y = bias;
            for(i=0; i<numInputs; i++){
                y += x[i] * weights[i];
            }
            y = tanh( y );
            break;
    }
    return y;
    
}

Float Neuron::getDerivative(const Float &y){

	Float yy = 0;
	switch( activationFunction ){
        case(LINEAR):
			yy = 1.0;
            break;
        case(SIGMOID):
			yy = y * (1.0 - y);
            break;
        case(BIPOLAR_SIGMOID):
			yy = (gamma * (1.0 - (y*y))) / 2.0;
            break;
        case(TANH):
            yy = 1.0 - (y*y);
            break;
    }
    return yy;
}
    
bool Neuron::validateActivationFunction(const Type actvationFunction){
    if( actvationFunction >= LINEAR && actvationFunction < NUMBER_OF_ACTIVATION_FUNCTIONS ) return true;
    return false;
}

GRT_END_NAMESPACE
