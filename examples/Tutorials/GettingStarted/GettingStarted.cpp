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

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Generate a basic dummy dataset with 1000 samples, 5 classes, and 3 dimensions
    cout << "Generating dataset..." << endl;
    ClassificationData::generateGaussDataset( "data.csv", 1000, 5, 3 );
    
    //Load some training data from a file
    ClassificationData trainingData;
    
    cout << "Loading dataset..." << endl;
    if( !trainingData.load( "data.csv" ) ){
        cout << "ERROR: Failed to load training data from file\n";
        return EXIT_FAILURE;
    }
    
    cout << "Data Loaded" << endl;
    
    //Print out some stats about the training data
    trainingData.printStats();
    
    //Partition the training data into a training dataset and a test dataset. 80 means that 80%
    //of the data will be used for the training data and 20% will be returned as the test dataset
    cout << "Splitting data into training/test split..." << endl;
    ClassificationData testData = trainingData.split(80);
    
    //Create a new Gesture Recognition Pipeline using an Adaptive Naive Bayes Classifier
    GestureRecognitionPipeline pipeline;
    pipeline.setClassifier( ANBC() );
    
    //Train the pipeline using the training data
    cout << "Training model..." << endl;
    if( !pipeline.train( trainingData ) ){
        cout << "ERROR: Failed to train the pipeline!\n";
        return EXIT_FAILURE;
    }
    
    //Save the pipeline to a file
    if( !pipeline.save( "HelloWorldPipeline" ) ){
        cout << "ERROR: Failed to save the pipeline!\n";
        return EXIT_FAILURE;
    }
    
    //Load the pipeline from a file
    if( !pipeline.load( "HelloWorldPipeline" ) ){
        cout << "ERROR: Failed to load the pipeline!\n";
        return EXIT_FAILURE;
    }
    
    //Test the pipeline using the test data
    cout << "Testing model..." << endl;
    if( !pipeline.test( testData ) ){
        cout << "ERROR: Failed to test the pipeline!\n";
        return EXIT_FAILURE;
    }
    
    //Print some stats about the testing
    cout << "Test Accuracy: " << pipeline.getTestAccuracy() << endl;
    
    cout << "Precision: ";
    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        UINT classLabel = pipeline.getClassLabels()[k];
        cout << "\t" << pipeline.getTestPrecision(classLabel);
    }cout << endl;
    
    cout << "Recall: ";
    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        UINT classLabel = pipeline.getClassLabels()[k];
        cout << "\t" << pipeline.getTestRecall(classLabel);
    }cout << endl;
    
    cout << "FMeasure: ";
    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        UINT classLabel = pipeline.getClassLabels()[k];
        cout << "\t" << pipeline.getTestFMeasure(classLabel);
    }cout << endl;
    
    MatrixFloat confusionMatrix = pipeline.getTestConfusionMatrix();
    cout << "ConfusionMatrix: \n";
    for(UINT i=0; i<confusionMatrix.getNumRows(); i++){
        for(UINT j=0; j<confusionMatrix.getNumCols(); j++){
            cout << confusionMatrix[i][j] << "\t";
        }cout << endl;
    }
    
    return EXIT_SUCCESS;
}
