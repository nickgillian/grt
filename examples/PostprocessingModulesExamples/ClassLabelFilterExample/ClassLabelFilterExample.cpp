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
 GRT Class Label Filter Example
 This examples demonstrates how to use the Class Label Filter to filter the predicted class label of any classification module 
 using the gesture recognition pipeline.
 
 The Class Label Filter is a useful post-processing module which can remove erroneous or sporadic prediction spikes that may be 
 made by a classifier on a continuous input stream of data.  For instance, imagine a classifier that correctly outputs the predicted
 class label of 1 for a large majority of the time that a user is performing gesture 1, but every so often (perhaps due to sensor noise),
 the classifier outputs the class label of 2.  In this instance the class label filter can be used to remove these sporadic prediction 
 values, with the output of the class label filter in this instance being 1.
 
 In this example we create a new gesture recognition pipeline and add an ANBC module for classification along with a Class Label Filter
 as a post-processing module.  The ANBC classifier is then trained with some dummy data (which consists of 3 simple classes generated 
 from 3 different Gaussian distributions).  Some test data is then used to demonstrate the result of using the Class Label Filter to
 post process the predicted class label from the ANBC algorithm.  The test data contains a continuous stream of data from class 1, with
 a few sporadic class 2 and 3 labels added randomly to the stream.
 
 This example shows you how to:
 - Create an initialize a GestureRecognitionPipeline and add an ANBC module and a ClassLabelFilter module
 - Load some ClassificationData from a file and train the classifier
 - Use some test data to demonstrate the effect of using a Class Label Filter
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
    
    //Add a ClassLabelFilter as a post processing module with a minCount of 5 and a buffer size of 10
    pipeline.addPostProcessingModule( ClassLabelFilter(5,10) );
    
    //Load some training data to train and test the classifier
    ClassificationData trainingData;
    ClassificationData testData;
    
    if( !trainingData.load("ClassLabelFilterTrainingData.grt") ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    if( !testData.load("ClassLabelFilterTestData.grt") ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }
    
    //Train the classifier
    if( !pipeline.train( trainingData ) ){
        cout << "Failed to train classifier!\n";
        return EXIT_FAILURE;
    }
    
    //Use the test dataset to demonstrate the output of the ClassLabelFilter    
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
