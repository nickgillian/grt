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
 GRT Random Forests Example
 
 This examples demonstrates how to initialize, train, and use the RandomForests algorithm for classification.
 
 RandomForests are an ensemble learning method that operate by building a number of decision trees at training 
 time and outputting the class with the majority vote over all the trees in the ensemble.

 In this example we create an instance of a RandomForests algorithm and then train a model using some pre-recorded training data.
 The trained RandomForests model is then used to predict the class label of some test data.
 
 This example shows you how to:
 - Create and initialize the RandomForests algorithm
 - Load some ClassificationData from a file and partition the training data into a training dataset and a test dataset
 - Train a RandomForests model using the training dataset
 - Test the RandomForests model using the test dataset
 - Manually compute the accuracy of the classifier
 */

#include "GRT.h"
using namespace GRT;

int main(int argc, const char * argv[])
{
    //Create a new RandomForests instance
    RandomForests forest;
    
    //Set the number of trees in the forest
    forest.setForestSize( 10 );
    
    //Set the number of random candidate splits that will be used to choose the best splitting values
    //More steps will give you a better model, but will take longer to train
    forest.setNumRandomSpilts( 100 );
    
    //Set the maximum depth of the tree
    forest.setMaxDepth( 10 );
    
    //Set the minimum number of samples allowed per node
    forest.setMinNumSamplesPerNode( 10 );
    
    //Load some training data to train the classifier
    ClassificationData trainingData;
    
    cout << "Loading Training Data\n";
    if( !trainingData.loadDatasetFromFile("RandomForestsTrainingData.txt") ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    //Use 20% of the training dataset to create a test dataset
    ClassificationData testData = trainingData.partition( 80 );
    
    //Train the classifier
    if( !forest.train( trainingData ) ){
        cout << "Failed to train classifier!\n";
        return EXIT_FAILURE;
    }
    
    //Print the forest
    forest.print();
    
    //Save the model to a file
    if( !forest.saveModelToFile("RandomForestsModel.txt") ){
        cout << "Failed to save the classifier model!\n";
        return EXIT_FAILURE;
    }
    
    //Load the model from a file
    if( !forest.loadModelFromFile("RandomForestsModel.txt") ){
        cout << "Failed to load the classifier model!\n";
        return EXIT_FAILURE;
    }
    
    //Test the accuracy of the model on the test data
    double accuracy = 0;
    for(UINT i=0; i<testData.getNumSamples(); i++){
        //Get the i'th test sample
        UINT classLabel = testData[i].getClassLabel();
        VectorDouble inputVector = testData[i].getSample();
        
        //Perform a prediction using the classifier
        bool predictSuccess = forest.predict( inputVector );
        
        if( !predictSuccess ){
            cout << "Failed to perform prediction for test sampel: " << i <<"\n";
            return EXIT_FAILURE;
        }
        
        //Get the predicted class label
        UINT predictedClassLabel = forest.getPredictedClassLabel();
        VectorDouble classLikelihoods = forest.getClassLikelihoods();
        VectorDouble classDistances = forest.getClassDistances();
        
        //Update the accuracy
        if( classLabel == predictedClassLabel ) accuracy++;
        
        cout << "TestSample: " << i <<  " ClassLabel: " << classLabel << " PredictedClassLabel: " << predictedClassLabel << endl;
    }
    
    cout << "Test Accuracy: " << accuracy/double(testData.getNumSamples())*100.0 << "%" << endl;
    
    return EXIT_SUCCESS;
}
