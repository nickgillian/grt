# Gesture Recognition Toolkit (GRT)

The Gesture Recognition Toolkit (GRT) is a cross-platform, open-source, C++ machine learning library designed for real-time gesture recognition.

Build Status:
* Master branch: 
  * ![Master Build Status](https://travis-ci.org/nickgillian/grt.svg?branch=master)
* Dev branch: 
  * ![Dev Build Status](https://travis-ci.org/nickgillian/grt.svg?branch=dev)

Key things to know about the GRT:
* The toolkit consists of two parts: a comprehensive **C++ API** and a front-end **graphical user interface (GUI)**. You can access the source code for both the C++ API and GUI in this repository, a precompiled version of the GUI can be downloaded [here](http://www.nickgillian.com/wiki/pmwiki.php/GRT/Download)
* Both the C++ API and GUI are designed to work with real-time sensor data, but they can also be used for more conventional offline machine-learning tasks 
* The input to the GRT can be any *N*-dimensional floating-point vector - this means you can use the GRT with Cameras, Kinect, Leap Motion, accelerometers, or any other custom sensor you might have built
* The toolkit reserves the class label value of zero as a special **null gesture** class label - you should therefore avoid using the gesture label of zero in your training data
* Training data and models are saved as custom **.grt** files.  These consist of a simple header followed by the main dataset.  In addition to the grt files, you can also import/export data via CSV files by using the *.csv* file extension when saving/loading files
* Almost all the GRT classes support the following functions: 
  * **predict( ... )**: uses the input data (...) and a pre-trained model to perform a prediction, such as classification or regression
  * **train( ... )**: uses the input data (...) to train a new model that can then be used for real-time prediction
  * **save( ... )**: saves a model or dataset to a file.  The file format can be a custom GRT file (.grt) or a CSV file (.csv)
  * **load( ... )**: loads a pre-trained model or dataset from a file. The file format can be a custom GRT file (.grt) or a CSV file (.csv)
  * **reset()**: resets a module, for example resetting a filter module would clear the values in it's history buffer and sets them to zero
  * **clear()**: clears a module, removing all pre-trained models, weights, etc.. For example, clearing a filter module would delete the filter coefficients, history buffer, etc.
* Functions with an underscore, such as **train_( ... )**, pass the input arguments as references and are therefore more efficient to use with very large datasets

##Core Resources
* GRT Wiki: [http://www.nickgillian.com/wiki](http://www.nickgillian.com/wiki)
* GRT Forum: [http://www.nickgillian.com/forum](http://www.nickgillian.com/forum)
* GRT API Reference: [http://www.nickgillian.com/archive/wiki/grt/doxygen/index.html](http://www.nickgillian.com/archive/wiki/grt/doxygen/index.html)
* GRT Source Code: [https://github.com/nickgillian/grt](https://github.com/nickgillian/grt)
* GRT GUI Download: [http://www.nickgillian.com/wiki/pmwiki.php/GRT/Download](http://www.nickgillian.com/wiki/pmwiki.php/GRT/Download)
* GRT Journal of Machine Learning Research paper: [grt.pdf](http://jmlr.csail.mit.edu/papers/volume15/gillian14a/gillian14a.pdf)

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
#include <GRT/GRT.h>
using namespace GRT;

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
   
    vector< UINT > classLabels = pipeline.getClassLabels();

    cout << "Precision: ";
    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        cout << "\t" << pipeline.getTestPrecision( classLabels[k] );
    }cout << endl;

    cout << "Recall: ";
    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
         cout << "\t" << pipeline.getTestRecall( classLabels[k] );
    }cout << endl;

    cout << "FMeasure: ";
    for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
        cout << "\t" << pipeline.getTestFMeasure( classLabels[k] );
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
```
##Tutorials and Examples

You can find a large number of tutorials and examples in the examples folder.  You can also find a
wide range of examples and references on the main GRT wiki:

http://www.nickgillian.com/wiki/pmwiki.php?n=GRT.GestureRecognitionToolkit

If you build the GRT using CMake, an examples folder will automatically be generated in the build directory after you successfully build the main GRT library. Example applications can
then be directly run from this example directory.  To run any of the examples, open terminal in the grt/build/examples directory and run:

    ./ExampleName

where *ExampleName* is the name of the example application you want to run.

##Forum

You can find the main GRT forum at: [http://www.nickgillian.com/forum/](http://www.nickgillian.com/forum/)

##Building the GRT

You can find a CMakeLists file in the build folder that you can use to autogenerate a makefile for your machine.

Read the readme file in the build folder to see how to build the GRT as a static library for Linux, OS X, or Windows.

##Installing and using the GRT in your C++ projects

See the build directory for details on how to build, install, and use the GRT in your C++ projects.

##Android port of the Gesture Recognition Toolkit

as described [http://hollyhook.de/wp/grt-for-android/] extended and fixed

##License

The Gesture Recognition Toolkit is available under a MIT license.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
