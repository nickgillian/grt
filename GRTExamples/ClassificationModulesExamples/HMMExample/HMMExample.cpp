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
 GRT HMM Example
 This examples demonstrates how to initialize, train, and use the HMM algorithm for classification. 
 
 Hidden Markov Models (HMM) are powerful classifiers that work well on temporal classification problems when you have a large training dataset.

 The HMM algorithm in the GRT is a discrete HMM, which means that the input to the HMM algorithm must be a discrete integer value in the range [0 numSymbols-1].  You can
 convert N-dimensional continuous data into a 1-dimensional discrete data using one of the GRT Quantization algorithms, such as KMeansQuantizer.

 In this example we create an instance of a HMM algorithm and then train a HMM model using some pre-recorded training data.
 The trained HMM algorithm is then used to predict the class label of some test data.
 
 This example shows you how to:
 - Create an initialize the HMM algorithm 
 - Load some LabelledTimeSeriesClassificationData from a file and partition the training data into a training dataset and a test dataset
 - Train a KMeansQuantizer and quantize the training and test data using the trained quantizer
 - Train the HMM algorithm using the quantized training dataset
 - Test the HMM algorithm using the quantized test dataset
 - Manually compute the accuracy of the classifier
*/

//You might need to set the specific path of the GRT header relative to your project
#include "GRT.h"
using namespace GRT;

int main(int argc, const char * argv[]){
    
    //Load the training data
    TimeSeriesClassificationData trainingData;
    
    if( !trainingData.loadDatasetFromFile("HMMTrainingData.txt") ){
        cout << "ERROR: Failed to load training data!\n";
        return false;
    }
    
    //Remove 20% of the training data to use as test data
    TimeSeriesClassificationData testData = trainingData.partition( 80 );
    
    //The input to the HMM must be a quantized discrete value
    //We therefore use a KMeansQuantizer to covert the N-dimensional continuous data into 1-dimensional discrete data
    const UINT NUM_SYMBOLS = 10;
    KMeansQuantizer quantizer( NUM_SYMBOLS );
    
    //Train the quantizer using the training data
    if( !quantizer.train( trainingData ) ){
        cout << "ERROR: Failed to train quantizer!\n";
        return false;
    }
    
    //Quantize the training data
    TimeSeriesClassificationData quantizedTrainingData( 1 );
    
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        
        UINT classLabel = trainingData[i].getClassLabel();
        MatrixDouble quantizedSample;
        
        for(UINT j=0; j<trainingData[i].getLength(); j++){
            quantizer.quantize( trainingData[i].getData().getRowVector(j) );
            
            quantizedSample.push_back( quantizer.getFeatureVector() );
        }
        
        if( !quantizedTrainingData.addSample(classLabel, quantizedSample) ){
            cout << "ERROR: Failed to quantize training data!\n";
            return false;
        }
        
    }
    
    //Create a new HMM instance
    HMM hmm;
    
    //Set the number of states in each model
    hmm.setNumStates( 4 );
    
    //Set the number of symbols in each model, this must match the number of symbols in the quantizer
    hmm.setNumSymbols( NUM_SYMBOLS );
    
    //Set the HMM model type to LEFTRIGHT with a delta of 1
    hmm.setModelType( HiddenMarkovModel::LEFTRIGHT );
    hmm.setDelta( 1 );
    
    //Set the training parameters
    hmm.setMinImprovement( 1.0e-5 );
    hmm.setMaxNumIterations( 100 );
    hmm.setNumRandomTrainingIterations( 20 );
    
    //Train the HMM model
    if( !hmm.train( quantizedTrainingData ) ){
        cout << "ERROR: Failed to train the HMM model!\n";
        return false;
    }
    
    //Save the HMM model to a file
    if( !hmm.saveModelToFile( "HMMModel.txt" ) ){
        cout << "ERROR: Failed to save the model to a file!\n";
        return false;
    }
    
    //Load the HMM model from a file
    if( !hmm.loadModelFromFile( "HMMModel.txt" ) ){
        cout << "ERROR: Failed to load the model from a file!\n";
        return false;
    }
    
    //Quantize the test data
    TimeSeriesClassificationData quantizedTestData( 1 );
    
    for(UINT i=0; i<testData.getNumSamples(); i++){
        
        UINT classLabel = testData[i].getClassLabel();
        MatrixDouble quantizedSample;
        
        for(UINT j=0; j<testData[i].getLength(); j++){
            quantizer.quantize( testData[i].getData().getRowVector(j) );
            
            quantizedSample.push_back( quantizer.getFeatureVector() );
        }
        
        if( !quantizedTestData.addSample(classLabel, quantizedSample) ){
            cout << "ERROR: Failed to quantize training data!\n";
            return false;
        }
    }
    
    //Compute the accuracy of the HMM models using the test data
    double numCorrect = 0;
    double numTests = 0;
    for(UINT i=0; i<quantizedTestData.getNumSamples(); i++){
        
        UINT classLabel = quantizedTestData[i].getClassLabel();
        hmm.predict( quantizedTestData[i].getData() );
        
        if( classLabel == hmm.getPredictedClassLabel() ) numCorrect++;
        numTests++;
        
        VectorDouble classLikelihoods = hmm.getClassLikelihoods();
        VectorDouble classDistances = hmm.getClassDistances();
        
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
