/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This file implements a container for a Softmax model.
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

#ifndef GRT_SOFTMAX_MODEL_HEADER
#define GRT_SOFTMAX_MODEL_HEADER

#include "../../CoreModules/Classifier.h"

GRT_BEGIN_NAMESPACE
    
class SoftmaxModel{
public:
    SoftmaxModel(){
        classLabel = 0;
        N = 0;
    }
    ~SoftmaxModel(){
        
    }
    
    bool init(UINT classLabel,UINT N){
        this->classLabel = classLabel;
        this->N = N;
        
        //Resize the weights
        w.clear();
        w.resize(N);
        
        //Randomize the weights
        Random rand;
        for(UINT i=0; i<N; i++){
            w[i] = rand.getRandomNumberUniform(-0.1,0.1);
        }
        w0 = rand.getRandomNumberUniform(-0.1,0.1);
        
        return true;
    }
    
    Float compute(const VectorFloat &x){
        Float sum = w0;
        for(UINT i=0; i<N; i++){
            sum += x[i]*w[i];
        }
        return 1.0 / (1.0+exp(-sum));
    }
    
    UINT classLabel;
    UINT N; //The number of dimensions
    VectorFloat w; //The coefficents
    Float w0;
    
};

GRT_END_NAMESPACE

#endif //SoftmaxModel

