/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a Neuron that is used by the Multilayer Perceptron.
 
 @example RegressionModulesExamples/MLPRegressionExample/MLPRegressionExample.cpp
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

#ifndef GRT_NEURON_HEADER
#define GRT_NEURON_HEADER

#include "../../../Util/GRTCommon.h"

GRT_BEGIN_NAMESPACE

class GRT_API Neuron{
public:
    enum Type{LINEAR=0,SIGMOID,BIPOLAR_SIGMOID,TANH,NUMBER_OF_ACTIVATION_FUNCTIONS};
    
    Neuron();

    Neuron( const Neuron &rhs );

    ~Neuron();

    Neuron& operator=(const Neuron &rhs);
    
    bool init(const UINT numInputs,const Type actvationFunction,const Float minWeightRange = -0.1, const Float maxWeightRange = 0.1);
    void clear();
    Float fire(const VectorFloat &x);
	Float getDerivative(const Float &y);
    
    static bool validateActivationFunction(const Type activationFunction);
    
	Float gamma;
    Float bias;
    Float previousBiasUpdate;
    VectorFloat weights;
	VectorFloat previousUpdate;
    UINT numInputs;
    UINT activationFunction;
};

GRT_END_NAMESPACE

#endif //GRT_NEURON_HEADER


