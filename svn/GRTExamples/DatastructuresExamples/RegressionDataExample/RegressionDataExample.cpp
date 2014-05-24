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

int main (int argc, const char * argv[])
{

    const UINT N = 1;       //The number of dimensions in an input vector
    const UINT T = 1;       //The number of dimensions in a target vector
    const UINT M = 1000;    //The number of training examples we will generate
    RegressionData trainingData;
    
    trainingData.setInputAndTargetDimensions(N, T);
    
    trainingData.setDatasetName("DummyData");
    
    trainingData.setInfoText("This dataset was created using a sine wave and random noise");
    
    //Generate an input and target signal, the input signal will consist of a sine wave with noise
    cout << "Generating " << M << " training datum";
    double x = 0;
    Random random;
    for(UINT i=0; i<M; i++){
        if( i % (M/10) == 0 ) cout << ". ";
        VectorDouble inputVector(N);
        VectorDouble targetVector(T);
        
        //Generate the input vector
        for(UINT j=0; j<N; j++){
            inputVector[j] = sin( x ) + random.getRandomNumberUniform(-0.1,0.1);
        }
        
        //Generate the target vector
        for(UINT j=0; j<T; j++){
            targetVector[j] = sin( x );
        }
        
        //Add the sample
        trainingData.addSample(inputVector, targetVector);
        
        //Update x
        x += TWO_PI/double(M)*10.0;
    }
    cout << "\n";
    
    //Save the training data to a custom file
    cout << "Saving data to file\n";
    if( !trainingData.saveDatasetToFile("RegressionData.txt") ){
        cout << "Failed to save training data to file!\n";
        return EXIT_FAILURE;
    }
    
    //You can also save the training data to a CSV file
    cout << "Saving data to file\n";
    if( !trainingData.saveDatasetToCSVFile("RegressionData.csv") ){
        cout << "Failed to save training data to CSV file!\n";
        return EXIT_FAILURE;
    }
    
    //You can also load data from a CSV file, you need to specify the number of input dimensions and number of target dimensions
    if( !trainingData.loadDatasetFromCSVFile("RegressionData.csv",N,T) ){
        cout << "Failed to save training data to CSV file!\n";
        return EXIT_FAILURE;
    }
    
    //Load the data back from a custom file
    cout << "Loading data from file\n";
    if( !trainingData.loadDatasetFromFile("RegressionData.txt") ){
        cout << "Failed to load training data from file!\n";
        return EXIT_FAILURE;
    }
    
    //Print some info about the dataset
    cout << "Dataset Name: " << trainingData.getDatasetName() << endl;
    cout << "NumInputDimensions: " << trainingData.getNumInputDimensions() << endl;
    cout << "NumTargetDimensions: " << trainingData.getNumTargetDimensions() << endl;
    cout << "NumSamples: " << trainingData.getNumSamples() << endl;
    cout << "Info: " << trainingData.getInfoText() << endl;
    
    //Partition the dataset into a training dataset and a test dataset
    //This will keep 80% of the data in the training dataset and return 20% of the data as the test dataset
    RegressionData testData = trainingData.partition( 80 );
    
    //Datasets can also be merged together, this will add all the data in the test dataset to the training dataset
    trainingData.merge( testData );
    
    //If you want to use one dataset for cross validation then first call the spilt function
    //This will set the dataset up for 10 fold cross validation
    trainingData.spiltDataIntoKFolds( 10 );
    
    //After you have called the spilt function you can then get the training and test sets for each fold
    for(UINT foldIndex=0; foldIndex<10; foldIndex++){
        RegressionData trainingFoldData = trainingData.getTrainingFoldData( foldIndex );
        RegressionData testFoldData = trainingData.getTestFoldData( foldIndex );
    }
    
    //Finally, if you need to clear the data at any point then call the clear function
    trainingData.clear();
    
    return EXIT_SUCCESS;
}

