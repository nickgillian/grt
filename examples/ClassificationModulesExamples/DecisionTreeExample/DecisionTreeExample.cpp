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

 @example DecisionTreeExample.cpp
 */

/*
 GRT DecisionTree

 This examples demonstrates how to initialize, train, and use the DecisionTree algorithm for classification.
 
 Decision Trees are conceptually simple classifiers that work well on even complex classification tasks.  
 Decision Trees partition the feature space into a set of rectangular regions, classifying a new datum by 
 finding which region it belongs to.
 
 In this example we create an instance of a DecisionTree algorithm and then train a model using some pre-recorded training data.
 The trained DecisionTree model is then used to predict the class label of some test data.
 
 This example shows you how to:
 - Create and initialize the DecisionTree algorithm
 - Load some ClassificationData from a file and partition the training data into a training dataset and a test dataset
 - Train a DecisionTree model using the training dataset
 - Test the DecisionTree model using the test dataset
 - Manually compute the accuracy of the classifier

You should run this example with one argument pointing to the data you want to load. A good dataset to run this example is acc-orientation.grt, which can be found in the GRT data folder.
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main(int argc, const char * argv[])
{
    //Parse the data filename from the argument list
    if( argc != 2 ){
        cout << "Error: failed to parse data filename from command line. You should run this example with one argument pointing to the data filename!\n";
        return EXIT_FAILURE;
    }
    const string filename = argv[1];

    //Create a new DecisionTree instance
    DecisionTree dTree;
    
    //Set the node that the DecisionTree will use - different nodes may result in different decision boundaries
    //and some nodes may provide better accuracy than others on specific classification tasks
    //The current node options are:
    //- DecisionTreeClusterNode
    //- DecisionTreeThresholdNode
    dTree.setDecisionTreeNode( DecisionTreeClusterNode() );
    
    //Set the number of steps that will be used to choose the best splitting values
    //More steps will give you a better model, but will take longer to train
    dTree.setNumSplittingSteps( 1000 );
    
    //Set the maximum depth of the tree
    dTree.setMaxDepth( 10 );
    
    //Set the minimum number of samples allowed per node
    dTree.setMinNumSamplesPerNode( 10 );
    
    //Load some training data to train the classifier
    ClassificationData trainingData;
    
    if( !trainingData.load( filename ) ){
        cout << "Failed to load training data: " << filename << endl;
        return EXIT_FAILURE;
    }
    
    //Use 20% of the training dataset to create a test dataset
    ClassificationData testData = trainingData.split( 80 );
    
    //Train the classifier
    if( !dTree.train( trainingData ) ){
        cout << "Failed to train classifier!\n";
        return EXIT_FAILURE;
    }
    
    //Print the tree
    dTree.print();
    
    //Save the model to a file
    if( !dTree.save("DecisionTreeModel.grt") ){
        cout << "Failed to save the classifier model!\n";
        return EXIT_FAILURE;
    }
    
    //Load the model from a file
    if( !dTree.load("DecisionTreeModel.grt") ){
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
        bool predictSuccess = dTree.predict( inputVector );
        
        if( !predictSuccess ){
            cout << "Failed to perform prediction for test sampel: " << i <<"\n";
            return EXIT_FAILURE;
        }
        
        //Get the predicted class label
        UINT predictedClassLabel = dTree.getPredictedClassLabel();
        VectorDouble classLikelihoods = dTree.getClassLikelihoods();
        VectorDouble classDistances = dTree.getClassDistances();
        
        //Update the accuracy
        if( classLabel == predictedClassLabel ) accuracy++;
        
        cout << "TestSample: " << i <<  " ClassLabel: " << classLabel << " PredictedClassLabel: " << predictedClassLabel << endl;
    }
    
    cout << "Test Accuracy: " << accuracy/double(testData.getNumSamples())*100.0 << "%" << endl;
    
    return EXIT_SUCCESS;
}
