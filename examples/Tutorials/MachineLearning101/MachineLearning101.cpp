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
 MachineLearning101 Example
 
 This example shows you how to:
 - load a dataset from a file
 - partition the dataset into a training dataset and a test dataset
 - setup a gesture recognition pipeline
 - train a KNN classification algorithm using the training dataset
 - test the classification abilities of the KNN algorithm using the test dataset
 - get some test metrics (accuracy, precision, recall, confusion matrix) about how successful the classification was
 */

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{

    //We are going to use the Iris dataset, you can find more about the orginal dataset at: http://en.wikipedia.org/wiki/Iris_flower_data_set
    
    //Create a new instance of ClassificationData to hold the training data
    ClassificationData trainingData;
    
    //Load the training dataset from a file, the file should be in the same directory as this program
    if( !trainingData.loadDatasetFromFile("IrisData.txt") ){
        cout << "Failed to load Iris data from file!\n";
        return EXIT_FAILURE;
    }
    
    //Print some basic stats about the dataset we have loaded
    trainingData.printStats();
    
    //Partition the training dataset into a training dataset and test dataset
    //We will use 60% of the data to train the algorithm and 40% of the data to test it
    //The true parameter flags that we want to use stratified sampling, which means there 
    //should be an equal class distribution between the training and test datasets
    ClassificationData testData = trainingData.partition( 60, true );
    
    //Setup the gesture recognition pipeline
    GestureRecognitionPipeline pipeline;
    
    //Add a KNN classification algorithm as the main classifier with a K value of 10
    pipeline.setClassifier( KNN(10) );
    
    //Train the KNN algorithm using the training dataset
    if( !pipeline.train( trainingData ) ){
        cout << "Failed to train the pipeline!\n";
        return EXIT_FAILURE;
    }
    
    //Test the KNN model using the test dataset
    if( !pipeline.test( testData ) ){
        cout << "Failed to test the pipeline!\n";
        return EXIT_FAILURE;
    }
    
    //Print some metrics about how successful the classification was
    //Print the accuracy
    cout << "The classification accuracy was: " << pipeline.getTestAccuracy() << "%\n" << endl;
    
    //Print the precision for each class
    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        UINT classLabel = pipeline.getClassLabels()[k];
        double classPrecision = pipeline.getTestPrecision( classLabel );
        cout << "The precision for class " << classLabel << " was " << classPrecision << endl;
    }
    cout << endl;
    
    //Print the recall for each class
    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        UINT classLabel = pipeline.getClassLabels()[k];
        double classRecall = pipeline.getTestRecall( classLabel );
        cout << "The recall for class " << classLabel << " was " << classRecall << endl;
    }
    cout << endl;
    
    //Print the confusion matrix
    Matrix< double > confusionMatrix = pipeline.getTestConfusionMatrix();
    cout << "Confusion Matrix: \n";
    for(UINT i=0; i<confusionMatrix.getNumRows(); i++){
        for(UINT j=0; j<confusionMatrix.getNumCols(); j++){
            cout << confusionMatrix[i][j] << "\t";
        }
        cout << endl;
        
    }
    cout << endl;

    return EXIT_SUCCESS;
}

