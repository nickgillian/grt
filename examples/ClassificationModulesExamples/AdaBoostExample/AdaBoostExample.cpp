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

 @example AdaBoostExample.cpp
 */

/*
 GRT AdaBoostExample

 This examples demonstrates how to initialize, train, and use the AdaBoost algorithm for classification.
 
 AdaBoost (Adaptive Boosting) is a powerful classifier that works well on both basic and more complex recognition problems.
 AdaBoost works by combining several relatively weak classifiers together to form a highly accurate strong classifier.  AdaBoost
 therefore acts as a meta algorithm, which allows you to use it as a wrapper for other classifiers.  In the GRT, these classifiers
 are called WeakClassifiers such as a DecisionStump (which is just one node of a DecisionTree).
 
 In this example we create an instance of an AdaBoost algorithm and then train the algorithm using some pre-recorded training data.
 The trained AdaBoost algorithm is then used to predict the class label of some test data.
 This example uses the DecisionStump as the WeakClassifer, although AdaBoost works with any GRT WeakClassifier (including any you write yourself).
 
 This example shows you how to:
 - Create an initialize the AdaBoost algorithm
 - Set a DecisionStump as the WeakClassifer
 - Load some ClassificationData from a file and partition the data into a training dataset and a test dataset
 - Train the AdaBoost algorithm using the training dataset
 - Test the AdaBoost algorithm using the test dataset
 - Manually compute the accuracy of the classifier

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

    //Create a new AdaBoost instance
    AdaBoost adaBoost;

    //Set the weak classifier you want to use
    adaBoost.setWeakClassifier( DecisionStump() );

    //Load some training data to train the classifier
    ClassificationData trainingData;

    if( !trainingData.load( filename ) ){
        cout << "Failed to load training data: " << filename << endl;
        return EXIT_FAILURE;
    }

    //Use 20% of the training dataset to create a test dataset
    ClassificationData testData = trainingData.split( 80 );

    //Train the classifier
    if( !adaBoost.train( trainingData ) ){
        cout << "Failed to train classifier!\n";
        return EXIT_FAILURE;
    }

    //Save the model to a file
    if( !adaBoost.save("AdaBoostModel.grt") ){
        cout << "Failed to save the classifier model!\n";
        return EXIT_FAILURE;
    }

    //Load the model from a file
    if( !adaBoost.load("AdaBoostModel.grt") ){
        cout << "Failed to load the classifier model!\n";
        return EXIT_FAILURE;
    }

    //Use the test dataset to test the AdaBoost model
    double accuracy = 0;
    for(UINT i=0; i<testData.getNumSamples(); i++){
        //Get the i'th test sample
        UINT classLabel = testData[i].getClassLabel();
        VectorFloat inputVector = testData[i].getSample();

        //Perform a prediction using the classifier
        if( !adaBoost.predict( inputVector ) ){
            cout << "Failed to perform prediction for test sampel: " << i <<"\n";
            return EXIT_FAILURE;
        }

        //Get the predicted class label
        UINT predictedClassLabel = adaBoost.getPredictedClassLabel();
        double maximumLikelhood = adaBoost.getMaximumLikelihood();
        VectorFloat classLikelihoods = adaBoost.getClassLikelihoods();
        VectorFloat classDistances = adaBoost.getClassDistances();

        //Update the accuracy
        if( classLabel == predictedClassLabel ) accuracy++;

        cout << "TestSample: " << i <<  " ClassLabel: " << classLabel;
        cout << " PredictedClassLabel: " << predictedClassLabel << " Likelihood: " << maximumLikelhood;
        cout << endl;
    }

    cout << "Test Accuracy: " << accuracy/double(testData.getNumSamples())*100.0 << "%" << endl;

    return EXIT_SUCCESS;
}
