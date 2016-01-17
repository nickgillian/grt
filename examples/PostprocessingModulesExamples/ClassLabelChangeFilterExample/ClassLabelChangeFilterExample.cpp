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
 GRT Class Label Change Filter Example
 This examples demonstrates how to use the Class Label Change Filter to filter the predicted class label of any classification module 
 using the gesture recognition pipeline.
 
 The Class Label Change Filter is a useful post-processing module which signals when the predicted output of a classifier changes. 
 For instance, if the output stream of a classifier was {1,1,1,1,2,2,2,2,3,3}, then the output of the filter would be {1,0,0,0,2,0,0,0,3,0}.
 This module is useful if you want to debounce a gesture and only care about when the gesture label changes.
 
 In this example we create a new gesture recognition pipeline and add an ANBC module for classification along with a Class Label Change 
 Filter as a post-processing module.  The ANBC classifier is then trained with some dummy data (which consists of 3 simple classes generated 
 from 3 different Gaussian distributions).  Some test data is then used to demonstrate the result of using the Class Label Change Filter to
 post process the predicted class label from the ANBC algorithm.  The test data contains a continuous stream of data from class 1, followed 
 by class 2 and then class 3. 
 
 This example shows you how to:
 - Create an initialize a GestureRecognitionPipeline and add an ANBC module and a ClassLabelChangeFilter module
 - Load some ClassificationData from a file and train the classifier
 - Use some test data to demonstrate the effect of using a Class Label Change Filter
 - Print the Processed and Unprocessed Predicted Class Label
 */

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Create a new gesture recognition pipeline
    GestureRecognitionPipeline pipeline;
    
    //Add an ANBC module
    pipeline.setClassifier( ANBC() );
    
    //Add a ClassLabelChangeFilter as a post processing module
    pipeline.addPostProcessingModule( ClassLabelChangeFilter() );
    
    //Load some training data to train and test the classifier
    ClassificationData trainingData;
    ClassificationData testData;
    
    if( !trainingData.load("ClassLabelChangeFilterTrainingData.grt") ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    if( !testData.load("ClassLabelChangeFilterTestData.grt") ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    //Train the classifier
    if( !pipeline.train( trainingData ) ){
        cout << "Failed to train classifier!\n";
        return EXIT_FAILURE;
    }
    
    //Use the test dataset to demonstrate the output of the ClassLabelChangeFilter
    for(UINT i=0; i<testData.getNumSamples(); i++){
        VectorFloat inputVector = testData[i].getSample();
        
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
    }
        
    return EXIT_SUCCESS;
}
