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

/*
 GRT HMM Continuous Example
 This examples demonstrates how to initialize, train, and use the Continuous HMM algorithm for classification.
 
 Hidden Markov Models (HMM) are powerful classifiers that work well on temporal classification problems when you have a large training dataset.

 The HMM algorithm in the GRT can be either a discrete or continuous HMM.  A discrete HMM means that the input to the HMM algorithm must be a 
 discrete integer value in the range [0 numSymbols-1].  A continuous HMM means that the input to the HMM algorithm can be an N-dimensional 
 floating point vector.  For the discrete HMM, you can convert N-dimensional continuous data into a 1-dimensional discrete data using one of 
 the GRT Quantization algorithms, such as KMeansQuantizer.

 In this example we create an instance of a Continuous HMM algorithm and then train a Continuous HMM model using some pre-recorded training data.
 The trained HMM algorithm is then used to predict the class label of some test data.
 
 This example shows you how to:
 - Create an initialize the Continuous HMM algorithm
 - Load some TimeSeriesClassificationData from a file and partition the training data into a training dataset and a test dataset
 - Train the HMM algorithm using the training dataset
 - Test the HMM algorithm using the test dataset
 - Manually compute the accuracy of the classifier
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main(int argc, const char * argv[]){
    
    //Load the training data
    TimeSeriesClassificationData trainingData;
    
    if( !trainingData.load("HMMTrainingData.grt") ){
        cout << "ERROR: Failed to load training data!\n";
        return false;
    }
    
    //Remove 20% of the training data to use as test data
    TimeSeriesClassificationData testData = trainingData.split( 80 );
    
    //Create a new HMM instance
    HMM hmm;
    
    //Set the HMM as a Continuous HMM
    hmm.setHMMType( HMM_CONTINUOUS );
    
    //Set the downsample factor, a higher downsample factor will speed up the prediction time, but might reduce the classification accuracy
    hmm.setDownsampleFactor( 5 );
    
    //Set the committee size, this sets the (top) number of models that will be used to make a prediction
    hmm.setCommitteeSize( 10 );
    
    //Tell the hmm algorithm that we want it to estimate sigma from the training data
    hmm.setAutoEstimateSigma( true );
    
    //Set the minimum value for sigma, you might need to adjust this based on the range of your data
    //If you set setAutoEstimateSigma to false, then all sigma values will use the value below
    hmm.setSigma( 20.0 );
    
    //Set the HMM model type to LEFTRIGHT with a delta of 1, this means the HMM can only move from the left-most state to the right-most state
    //in steps of 1
    hmm.setModelType( HMM_LEFTRIGHT );
    hmm.setDelta( 1 );
    
    //Train the HMM model
    if( !hmm.train( trainingData ) ){
        cout << "ERROR: Failed to train the HMM model!\n";
        return false;
    }
    
    //Save the HMM model to a file
    if( !hmm.save( "HMMModel.grt" ) ){
        cout << "ERROR: Failed to save the model to a file!\n";
        return false;
    }
    
    //Load the HMM model from a file
    if( !hmm.load( "HMMModel.grt" ) ){
        cout << "ERROR: Failed to load the model from a file!\n";
        return false;
    }

    //Compute the accuracy of the HMM models using the test data
    double numCorrect = 0;
    double numTests = 0;
    for(UINT i=0; i<testData.getNumSamples(); i++){
        
        UINT classLabel = testData[i].getClassLabel();
        hmm.predict( testData[i].getData() );
        
        if( classLabel == hmm.getPredictedClassLabel() ) numCorrect++;
        numTests++;
        
        VectorFloat classLikelihoods = hmm.getClassLikelihoods();
        VectorFloat classDistances = hmm.getClassDistances();
        
        cout << "ClassLabel: " << classLabel;
        cout << " PredictedClassLabel: " << hmm.getPredictedClassLabel();
        cout << " MaxLikelihood: " << hmm.getMaximumLikelihood();
        
        cout << "  ClassLikelihoods: ";
        for(UINT k=0; k<classLikelihoods.size(); k++){
            cout << classLikelihoods[k] << "\t";
        }
        
        cout << "ClassDistances: ";
        for(UINT k=0; k<classDistances.size(); k++){
            cout << classDistances[k] << "\t";
        }
        cout << endl;
    }
    
    cout << "Test Accuracy: " << numCorrect/numTests*100.0 << endl;
    
    return true;
}
