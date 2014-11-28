# Gesture Recognition Toolkit (GRT)

The Gesture Recognition Toolkit (GRT) is a cross-platform, open-source, C++ machine learning library designed for real-time gesture recognition.

You can find the official git repository for the GRT here: [https://github.com/nickgillian/grt](https://github.com/nickgillian/grt) 

It is also mirrored on googlecode: [https://code.google.com/p/gesture-recognition-toolkit/](https://code.google.com/p/gesture-recognition-toolkit/) 

Key things to know about the GRT:
* The toolkit consists of two parts: a comprehensive **C++ API** and a front-end **graphical user interface (GUI)**. You can access the source code for both the C++ API and GUI in this repository, a precompiled version of the GUI can be downloaded here: [download GUI](http://www.nickgillian.com/wiki/pmwiki.php/GRT/Download)
* Both the C++ API and GUI are designed to work with real-time sensor data, but they can also be used for more conventional offline machine-learning tasks 
* The input to the GRT can be any *N*-dimensional floating-point vector - this means you can use the GRT with Cameras, Kinect, Leap Motion, accelerometers, or any other custom sensor you might have built
* The toolkit reserves the class label value of zero as a special **null gesture** class label - you should therefore avoid using the gesture label of zero in your training data
* Training data and models are saved as custom **.grt** files.  These consist of a simple header followed by the main dataset.  In addition to the grt files, you can also import/export data via CSV files by using the *.csv* file extension when saving/loading files
* Almost all the GRT classes support the following functions: **predict( ... )**, **train( ... )**, **save( ... )**, **load( ... )**, **reset()** and **clear()**.  Functions with an underscore, such as **train_( ... )**, pass the input arguments as references and are therefore more efficient to use with very large datasets

You can find out more about the GRT in our Journal of Machine Learning Research paper:  [grt.pdf](http://jmlr.csail.mit.edu/papers/volume15/gillian14a/gillian14a.pdf)

##GRT Architecture
To support flexibility while maintaining consistency, the GRT uses an object-oriented modular architecture. This architecture is built around a set 
of core **modules** and a central **gesture recognition pipeline**.

The input to both the modules and pipeline consists of an **N-dimensional double-precision vector**, making the toolkit flexible to the type of input signal. 
The algorithms in each module can be used as standalone classes; alternatively a pipeline can be used to chain modules together to create a more sophisticated gesture-recognition system. The GRT includes modules for preprocessing, feature extraction, clustering, classification, regression and post processing.

The toolkit's source code is structured as following:
* **ClassificationModules:** Contains all the GRT classification algorithms, such as AdaBoost, Naive Bayes, K-Nearest Neighbor, Support Vector Machines, and more.
* **ClusteringModules:** Contains all the GRT clustering algorithms, including K-Means, Gaussian Mixture Models and Self-Organizing Maps.
* **ContextModules:** Contains all the GRT context modules, these are modules that can be connected to a gesture recognition pipeline to input additional context to a real-time classification system.
* **CoreAlgorithms:** Contains a number of algorithms that are used across the GRT, such as Particle Filters, Principal Component Analysis and Restricted Boltzmann Machines.
* **CoreModules:** Contains all the GRT base classes, such as MLBase, Classifier, FeatureExtraction, etc..
* **DataStructures:** Contains all the GRT classes for recording, saving and loading datasets.
* **FeatureExtractionModules:** Contains all the GRT feature extraction modules.  These include FFT, Quantizers and TimeDomainFeatures.
* **PostProcessingModules:** Contains all the GRT post processing modules, including ClassLabelFilter and ClassLabelTimeoutFilter.
* **PreProcessingModules:** Contains all the GRT pre processing modules, including LowPassFilter, HighPassFilter, DeadZone, and many more.
* **RegressionModules:** Contains all the GRT regression modules, such as MLP Neural Networks, Linear Regression, and Logistic Regression.
* **Util:** Contains a wide range of supporting classes, such as Logging, Util, TimeStamp, Random and Matrix.

##Getting Started Example
This example demonstrates a few key components of the GRT, such as:
* how to generate and save a basic labeled dataset
* how to save/load this data to/from a CSV file
* how to split a dataset into a training and test partition
* how to setup a new Gesture Recognition Pipeline and add a classification algorithm to the pipeline
* how to use a training dataset to train a new classification model
* how to save/load a trained pipeline to/from a file
* how to use a test dataset to test the accuracy of a classification model
* how to print the test results

You can find this source code and a large number of other examples and tutorials in the GRT examples folder.

```C++
//Include the main GRT header
#include "GRT.h"
using namespace GRT;

//A helper function to generate a dummy dataset
bool generateDataset( const string filename, const UINT numSamples, const UINT numClasses, const UINT numDimensions  );

int main (int argc, const char * argv[])
{
    //Load some training data from a file
    ClassificationData trainingData;

    //Generate a basic dataset with 1000 samples, 5 classes, and 3 dimensions
    cout << "Generating dataset..." << endl;
    generateDataset( "data.csv", 1000, 5, 3 );

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

    MatrixDouble confusionMatrix = pipeline.getTestConfusionMatrix();
    cout << "ConfusionMatrix: \n";
    for(UINT i=0; i<confusionMatrix.getNumRows(); i++){
        for(UINT j=0; j<confusionMatrix.getNumCols(); j++){
            cout << confusionMatrix[i][j] << "\t";
        }cout << endl;
    }

    return EXIT_SUCCESS;
}

//This function generates a dummy dataset and saves it to a CSV file
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
    return data.save( filename );
}
```
##Tutorials and Examples

You can find a large number of tutorials and examples in the examples folder.  You can also find a
wide range of examples and references on the main GRT wiki:

http://www.nickgillian.com/wiki/pmwiki.php?n=GRT.GestureRecognitionToolkit

##Forum

You can find the main GRT forum at: [http://www.nickgillian.com/forum/](http://www.nickgillian.com/forum/)

##Building the GRT

You can find a CMakeLists file in the build folder that you can use to autogenerate a makefile for your machine.

Read the readme file in the build folder to see how to build the GRT as a static library for Linux, OS X, or Windows.

##Installing and using the GRT in your C++ projects

To use the GRT you simply need to add all of the code contained in the GRT folder to your c++ project.  
If you are using an IDE such as XCode or VisualStudio then you can either add the files in the GRT 
folder to your project using "File -> Add Files to project", or in most IDEs you can simply drag the 
GRT folder (from Finder or Windows Explorer) into your IDE which should add all of the files in the GRT 
folder to your project.

After you have added the code in the GRT folder to your project, you can now use any of the GRT 
functions or classes by adding the following two lines of code to the top of any header file in your 
project in which you want to use the GRT code:

    #include "GRT/GRT.h"
    using namespace GRT;

The first line of code above adds the main GRT header file (GRT.h) to your project, this header file 
contains all of the GRT module header files which means you do not need to manually include any other 
GRT header files.  The second line of code states that you are using the GRT namespace, this means
that you don't have to write GRT::WhatEverClass every time you want to use a GRT class and can instead
simply write WhatEverClass.

Note that, depending on your IDE, you may have to specify the physical path that 
you placed the GRT folder in on your hard drive, for instance you might have to write something like:

    #include "/Users/Nick/Documents/gesture-recognition-toolkit/GRT/GRT.h"

##License

The Gesture Recognition Toolkit is available under a MIT license.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
