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

#include "GRT.h"
using namespace GRT;

bool generateDataset( const string filename, const UINT numSamples, const UINT numClasses, const UINT numDimensions  );

int main (int argc, const char * argv[])
{
    //Load some training data from a file
    ClassificationData trainingData;
    
    //Generate a basic dataset with 1000 samples, 5 classes, and 3 dimensions
    cout << "Generating dataset..." << endl;
    generateDataset( "data.csv", 1000, 5, 3 );
    
    cout << "Loading dataset..." << endl;
    if( !trainingData.loadDatasetFromCSVFile( "data.csv" ) ){
        cout << "ERROR: Failed to load training data from file\n";
        return EXIT_FAILURE;
    }
    
    cout << "Data Loaded" << endl;
    
    //Print out some stats about the training data
    trainingData.printStats();
    
    //Partition the training data into a training dataset and a test dataset. 80 means that 80%
    //of the data will be used for the training data and 20% will be returned as the test dataset
    cout << "Splitting data into training/test split..." << endl;
    ClassificationData testData = trainingData.partition(80);
    
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
	if( !pipeline.savePipelineToFile( "HelloWorldPipeline.grt" ) ){
        cout << "ERROR: Failed to save the pipeline!\n";
        return EXIT_FAILURE;
    }
    
	//Load the pipeline from a file
	if( !pipeline.loadPipelineFromFile( "HelloWorldPipeline.grt" ) ){
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
    
    MatrixDouble confusionMatrix = pipeline.getTestConfusionMatrix();
    cout << "ConfusionMatrix: \n";
    for(UINT i=0; i<confusionMatrix.getNumRows(); i++){
        for(UINT j=0; j<confusionMatrix.getNumCols(); j++){
            cout << confusionMatrix[i][j] << "\t";
        }cout << endl;
    }
    
    return EXIT_SUCCESS;
}

/**
 This function generates a very basic dataset and saves it to a file.
 
 @param const string filename: the name of the file you want to save the data to
 @param const UINT numSamples: the number of examples that will be generated
 @param const UINT numClasses: the number of classes in the dataset
 @param const UINT numDimensions: the number of dimensions in the dataset
 @return returns true if the dataset was created successfully, false otherwise
 */
bool generateDataset( const string filename, const UINT numSamples, const UINT numClasses, const UINT numDimensions ){
    
    Random random;
    
    //Generate a simple model that will be used to generate the main dataset
    MatrixDouble model(numClasses,numDimensions);
    for(UINT k=0; k<numClasses; k++){
        for(UINT j=0; j<numDimensions; j++){
            model[k][j] = random.getRandomNumberUniform(-10,10);
        }
    }
    
    //Use the model above to generate the main dataset
    ClassificationData data;
    data.setNumDimensions( numDimensions );
    
    for(UINT i=0; i<numSamples; i++){
        
        //Randomly select which class this sample belongs to
        UINT k = random.getRandomNumberInt( 0, numClasses );
        
        //Generate a sample using the model (+ some Gaussian noise)
        vector< double > sample( numDimensions );
        for(UINT j=0; j<numDimensions; j++){
            sample[j] = model[k][j] + random.getRandomNumberGauss(0,1);
        }
        
        //By default in the GRT, the class label should not be 0, so add 1
        UINT classLabel = k + 1;
        
        //Add the labeled sample to the dataset
        data.addSample( classLabel, sample );
    }
    
    //Save the dataset to a CSV file
    return data.saveDatasetToCSVFile( filename );
}
