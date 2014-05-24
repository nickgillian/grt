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
 GRT Class Label Timeout Filter Example
 This examples demonstrates how to use the Class Label Timeout Filter to filter the predicted class label of any classification module 
 using the gesture recognition pipeline.
 
 The Class Label Timeout Filter is a useful post-processing module which debounces a gesture (i.e. it stops a single gesture from being 
 recognized multiple times over a short time frame). For instance, it is normally the case that whenever a user performs a gesture, such 
 as a swipe gesture for example, that the recognition system may recognize this single gesture several times because the user's movements 
 are being sensed at a high sample rate (i.e. 100Hz). The Class Label Timeout Filter can be used to ensure that a gesture, such as the 
 previous swipe gesture example, is only recognize once within any given timespan.
 
 This example demonstrates how to create a new gesture recognition pipeline and add an ANBC module for classification along with a Class 
 Label Timeout Filter as a post-processing module. The ANBC classifier is trained with some dummy data (which consists of 3 simple classes 
 generated from 3 different Gaussian distributions). Some test data is then used to demonstrate the result of using the Class Label Timeout 
 Filter to post process the predicted class label from the ANBC algorithm. The test data contains a continuous stream of data from class 1, 
 followed by class 2 and then class 3. The time between each prediction is set to 100 milliseconds, with the timeout for the Class Label 
 Timeout Filter set to 1000 milliseconds.
 
 This example shows you how to:
 - Create an initialize a GestureRecognitionPipeline and add an ANBC module and a ClassLabelTimeoutFilter module
 - Load some ClassificationData from a file and train the classifier
 - Use some test data to demonstrate the effect of using a Class Label Filter
 - Print the Processed and Unprocessed Predicted Class Label
 */

//You might need to set the specific path of the GRT header relative to your project
#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Create a new gesture recognition pipeline
    GestureRecognitionPipeline pipeline;
    
    //Add an ANBC module
    pipeline.setClassifier( ANBC() );
    
    //Add a ClassLabelTimeoutFilter as a post processing module with a minCount of 5 and a buffer size of 10
    pipeline.addPostProcessingModule( ClassLabelTimeoutFilter(1000,ClassLabelTimeoutFilter::ALL_CLASS_LABELS) );
    
    //Load some training data to train and test the classifier
    ClassificationData trainingData;
    ClassificationData testData;
    
    if( !trainingData.loadDatasetFromFile("ClassLabelTimeoutFilterTrainingData.txt") ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    if( !testData.loadDatasetFromFile("ClassLabelTimeoutFilterTestData.txt") ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    //Train the classifier
    if( !pipeline.train( trainingData ) ){
        cout << "Failed to train classifier!\n";
        return EXIT_FAILURE;
    }
    
    //Use the test dataset to demonstrate the output of the ClassLabelTimeoutFilter
    for(UINT i=0; i<testData.getNumSamples(); i++){
        vector< double > inputVector = testData[i].getSample();
        
        if( !pipeline.predict( inputVector ) ){
            cout << "Failed to perform prediction for test sampel: " << i <<"\n";
            return EXIT_FAILURE;
        }
        
        //Get the predicted class label (this will be the processed class label)
        UINT predictedClassLabel = pipeline.getPredictedClassLabel();
        
        //Get the unprocessed class label (i.e. the direct output of the classifier)
        UINT unprocessedClassLabel = pipeline.getUnProcessedPredictedClassLabel();
        
        //Also print the results to the screen
        cout << "Processed Class Label: \t" << predictedClassLabel << "\tUnprocessed Class Label: \t" << unprocessedClassLabel << endl;
        
        //Sleep for 100 millisecond
        Util::sleep( 100 );
    }
    
    return EXIT_SUCCESS;
}