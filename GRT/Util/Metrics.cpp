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
#include "Metrics.h"
#include "../DataStructures/ClassificationData.h"
#include "../CoreModules/Classifier.h"

GRT_BEGIN_NAMESPACE

Metrics::Metrics(){}
    
Metrics::~Metrics(){}

bool Metrics::computeAccuracy( GRT::Classifier &model, const GRT::ClassificationData &data, Float &accuracy ){

    accuracy = 0;

    if( !model.getTrained() ) return false;

    const UINT M = data.getNumSamples();

    UINT predictedClassLabel = 0;
    for(UINT i=0; i<M; i++){
        if( !model.predict( data[i].getSample() ) ){
            accuracy = 0;
            return false;
        }

        if( model.getPredictedClassLabel() == data[i].getClassLabel() ){
            accuracy++;
        }
    }

    //Convert the accuracy to a percentage
    accuracy = accuracy / static_cast<Float>(M) * 100.0;

    return true;
}

GRT_END_NAMESPACE

