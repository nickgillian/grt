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
 GRT MLP Regression Example
 This examples demonstrates how to initialize, train, and use the MLP algorithm for regression.
 
 The Multi Layer Perceptron (MLP) algorithm is a powerful form of an Artificial Neural Network that is commonly used for regression.
 
 In this example we create an instance of an MLP algorithm and then train the algorithm using some pre-recorded training data.
 The trained MLP algorithm is then used to perform regression on the test data.
 
 This example shows you how to:
 - Create an initialize the MLP algorithm for regression
 - Create a new instance of a GestureRecognitionPipeline and add the regression instance to the pipeline
 - Load some LabelledRegressionData from a file
 - Train the MLP algorithm using the training dataset
 - Test the MLP algorithm using the test dataset
 - Save the output of the MLP algorithm to a file
*/

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Turn on the training log so we can print the training status of the MLP to the screen
    TrainingLog::enableLogging( true ); 

    //Load the training data
    RegressionData trainingData;
    RegressionData testData;
    
    if( !trainingData.loadDatasetFromFile("MLPRegressionTrainingData.txt") ){
        cout << "ERROR: Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    if( !testData.loadDatasetFromFile("MLPRegressionTestData.txt") ){
        cout << "ERROR: Failed to load test data!\n";
        return EXIT_FAILURE;
    }
    
    //Make sure the dimensionality of the training and test data matches
    if( trainingData.getNumInputDimensions() != testData.getNumInputDimensions() ){
        cout << "ERROR: The number of input dimensions in the training data (" << trainingData.getNumInputDimensions() << ")";
        cout << " does not match the number of input dimensions in the test data (" << testData.getNumInputDimensions() << ")\n";
        return EXIT_FAILURE;
    }
    
    if( testData.getNumTargetDimensions() != testData.getNumTargetDimensions() ){
        cout << "ERROR: The number of target dimensions in the training data (" << testData.getNumTargetDimensions() << ")";
        cout << " does not match the number of target dimensions in the test data (" << testData.getNumTargetDimensions() << ")\n";
        return EXIT_FAILURE;
    }
    
    cout << "Training and Test datasets loaded\n";
    
    //Print the stats of the datasets
    cout << "Training data stats:\n";
    trainingData.printStats();
    
    cout << "Test data stats:\n";
    testData.printStats();
    
    //Create a new gesture recognition pipeline
    GestureRecognitionPipeline pipeline;
    
    //Setup the MLP, the number of input and output neurons must match the dimensionality of the training/test datasets
    MLP mlp;
    unsigned int numInputNeurons = trainingData.getNumInputDimensions();
    unsigned int numHiddenNeurons = 5;
    unsigned int numOutputNeurons = trainingData.getNumTargetDimensions();
    
    //Initialize the MLP
    mlp.init(numInputNeurons, numHiddenNeurons, numOutputNeurons);
    
    //Set the training settings
    mlp.setMaxNumEpochs( 500 ); //This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
    mlp.setMinChange( 1.0e-5 ); //This sets the minimum change allowed in training error between any two epochs
    mlp.setNumRandomTrainingIterations( 20 ); //This sets the number of times the MLP will be trained, each training iteration starts with new random values
    mlp.setUseValidationSet( true ); //This sets aside a small portiion of the training data to be used as a validation set to mitigate overfitting
    mlp.setValidationSetSize( 20 ); //Use 20% of the training data for validation during the training phase
    mlp.setRandomiseTrainingOrder( true ); //Randomize the order of the training data so that the training algorithm does not bias the training
    
    //The MLP generally works much better if the training and prediction data is first scaled to a common range (i.e. [0.0 1.0])
    mlp.enableScaling( true );
    
    //Add the MLP to the pipeline
    pipeline.setRegressifier( mlp );
    
    //Train the MLP model
    cout << "Training MLP model...\n";
    if( !pipeline.train( trainingData ) ){
        cout << "ERROR: Failed to train MLP model!\n";
        return EXIT_FAILURE;
    }
    
    cout << "Model trained.\n";
    
    //Test the model
    cout << "Testing MLP model...\n";
    if( !pipeline.test( testData ) ){
        cout << "ERROR: Failed to test MLP model!\n";
        return EXIT_FAILURE;
    }
    
    cout << "Test complete. Test RMS error: " << pipeline.getTestRMSError() << endl;
    
    //Run back over the test data again and output the results to a file 
    fstream file;
    file.open("MLPResultsData.txt", fstream::out);
    
    for(UINT i=0; i<testData.getNumSamples(); i++){
        vector< double > inputVector = testData[i].getInputVector();
        vector< double > targetVector = testData[i].getTargetVector();
        
        //Map the input vector using the trained regression model
        if( !pipeline.predict( inputVector ) ){
            cout << "ERROR: Failed to map test sample " << i << endl;
            return EXIT_FAILURE;
        }
        
        //Get the mapped regression data
        vector< double > outputVector = pipeline.getRegressionData();
        
        //Write the mapped value and also the target value to the file
        for(UINT j=0; j<outputVector.size(); j++){
            file << outputVector[j] << "\t";
        }
        for(UINT j=0; j<targetVector.size(); j++){
            file << targetVector[j] << "\t";
        }
        file << endl;
    }
    
    //Close the file
    file.close();
    
    return EXIT_SUCCESS;
}