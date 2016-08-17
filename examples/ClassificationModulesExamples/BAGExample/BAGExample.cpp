/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @section LICENSE
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

 @example BAGExample.cpp
 */

/*
 GRT BAG Example
 This examples demonstrates how to initialize, train, and use the BAG algorithm for classification.
 
 The BAG classifier implements bootstrap aggregating (bagging), a machine learning ensemble meta-algorithm 
 designed to improve the stability and accuracy of other machine learning algorithms.  Bagging also reduces 
 variance and helps to avoid overfitting. Although it is usually applied to decision tree methods, the BAG 
 class can be used with any type of GRT classifier.
 
 In this example we create an instance of a BAG algorithm, add several classifiers to the BAG ensemble and then
 train a BAG model using some pre-recorded training data.  The trained BAG model is then used to predict the 
 class label of some test data.
 
 This example shows you how to:
 - Create an initialize the BAG algorithm
 - Add several classifiers to the BAG ensemble
 - Load some ClassificationData from a file and partition the training data into a training dataset and a test dataset
 - Train a BAG model using the training dataset
 - Test the model using the test dataset
 - Compute the accuracy of the classifier

You should run this example with one argument pointing to the data you want to load. A good dataset to run this example is acc-orientation.grt, which can be found in the GRT data folder.
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Parse the data filename from the argument list
    if( argc != 2 ){
        cout << "Error: failed to parse data filename from command line. You should run this example with one argument pointing to the data filename!\n";
        return EXIT_FAILURE;
    }
    const string filename = argv[1];

    //Create a new BAG instance
    BAG bag;
    
    //Add an adaptive naive bayes classifier to the BAG ensemble
    bag.addClassifierToEnsemble( ANBC() );
    
    //Add a MinDist classifier to the BAG ensemble, using two clusters
    MinDist min_dist_two_clusters;
    min_dist_two_clusters.setNumClusters(2);
    bag.addClassifierToEnsemble( min_dist_two_clusters );
    
    //Add a MinDist classifier to the BAG ensemble, using five clusters
    MinDist min_dist_five_clusters;
    min_dist_five_clusters.setNumClusters(5);
    bag.addClassifierToEnsemble( min_dist_five_clusters );
    
    //Load some training data to train the classifier
    ClassificationData trainingData;
    
    if( !trainingData.load( filename ) ){
        cout << "Failed to load training data: " << filename << endl;
        return EXIT_FAILURE;
    }
    
    //Use 50% of the training dataset to create a test dataset
    ClassificationData testData = trainingData.split( 50 );
    
    //Train the classifier
    if( !bag.train( trainingData ) ){
        cout << "Failed to train classifier!\n";
        return EXIT_FAILURE;
    }
    
    //Save the BAG model to a file
    if( !bag.save("BAGModel.grt") ){
        cout << "Failed to save the classifier model!\n";
        return EXIT_FAILURE;
    }
    
    //Load the BAG model from a file
    if( !bag.load("BAGModel.grt") ){
        cout << "Failed to load the classifier model!\n";
        return EXIT_FAILURE;
    }
    
    //Use the test dataset to test the BAG model
    double accuracy = 0;
    for(UINT i=0; i<testData.getNumSamples(); i++){
        //Get the i'th test sample
        UINT classLabel = testData[i].getClassLabel();
        VectorFloat inputVector = testData[i].getSample();
        
        //Perform a prediction using the classifier
        if( !bag.predict( inputVector ) ){
            cout << "Failed to perform prediction for test sampel: " << i <<"\n";
            return EXIT_FAILURE;
        }
        
        //Get the predicted class label
        UINT predictedClassLabel = bag.getPredictedClassLabel();
        VectorFloat classLikelihoods = bag.getClassLikelihoods();
        VectorFloat classDistances = bag.getClassDistances();
        
        //Update the accuracy
        if( classLabel == predictedClassLabel ) accuracy++;
        
        //Print out the results
        cout << "TestSample: " << i;
        cout <<" ClassLabel: " << classLabel;
        cout << " PredictedClassLabel: " << predictedClassLabel;
        
        cout <<" ClassLikelihoods: ";
        for(UINT j=0; j<classLikelihoods.size(); j++){
            cout << classLikelihoods[j] << " ";
        }
        cout <<" ClassDistances: ";
        for(UINT j=0; j<classLikelihoods.size(); j++){
            cout << classDistances[j] << " ";
        }
        cout << endl;
    }
    
    cout << "Test Accuracy: " << accuracy/double(testData.getNumSamples())*100.0 << "%" << endl;
    
    return EXIT_SUCCESS;
}
