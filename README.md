# Gesture Recognition Toolkit (GRT)

The Gesture Recognition Toolkit (GRT) is a cross-platform, open-source, C++ machine learning library designed for real-time gesture recognition.

Build Status:
* Master branch: 
  * ![Master Build Status](https://travis-ci.org/nickgillian/grt.svg?branch=master)
* Dev branch: 
  * ![Dev Build Status](https://travis-ci.org/nickgillian/grt.svg?branch=dev)

Current version: [0.2.5](http://nickgillian.com/grt/api/0.2.5/)

Key things to know about the GRT:
* The toolkit consists of two parts: a comprehensive [C++ API](http://nickgillian.com/grt/api/0.2.5) and a front-end [graphical user interface (GUI)](http://www.nickgillian.com/wiki/pmwiki.php/GRT/GUI). You can access the source code for both the C++ API and GUI in this repository, a precompiled version of the GUI can be downloaded [here](http://www.nickgillian.com/wiki/pmwiki.php/GRT/Download)
* Both the C++ API and GUI are designed to work with real-time sensor data, but they can also be used for more conventional offline machine-learning tasks 
* The input to the GRT can be any *N*-dimensional floating-point vector - this means you can use the GRT with Cameras, Kinect, Leap Motion, accelerometers, or any other custom sensor you might have built
* The toolkit defines a generic [Float](#grt-floating-point-precision) type, this defaults to double precision float, but can easily be changed to single precision via the main GRT Typedefs header
* The precision of the GRT [VectorFloat](#vectorfloat-and-matrixfloat-data-structures) and [MatrixFloat](#vectorfloat-and-matrixfloat-data-structures) classes is automatically updated based on the main Float precision
* The toolkit reserves the class label value of zero as a special **null gesture** class label for automatic gesture spotting, so if you want to use gesture spotting avoid labeling any of your gestures with the class label of zero
* Training data and models are saved as custom **.grt** files.  These consist of a simple header followed by the main dataset.  In addition to the grt files, you can also import/export data via CSV files by using the *.csv* file extension when saving/loading files
* Almost all the GRT classes support the following functions: 
  * **predict( ... )**: uses the input data (...) and a pre-trained model to perform a prediction, such as classification or regression
  * **train( ... )**: uses the input data (...) to train a new model that can then be used for real-time prediction
  * **save( ... )**: saves a model or dataset to a file.  The file format can be a custom GRT file (.grt) or a CSV file (.csv)
  * **load( ... )**: loads a pre-trained model or dataset from a file. The file format can be a custom GRT file (.grt) or a CSV file (.csv)
  * **reset()**: resets a module, for example resetting a filter module would clear the values in its history buffer and sets them to zero
  * **clear()**: clears a module, removing all pre-trained models, weights, etc.. For example, clearing a filter module would delete the filter coefficients, history buffer, etc.
* Functions with an underscore, such as **train_( ... )**, pass the input arguments as references and are therefore more efficient to use with very large datasets

## Core Resources
* GRT Website: [http://www.nickgillian.com/grt](http://www.nickgillian.com/grt)
* GRT Wiki: [https://github.com/nickgillian/grt/wiki](https://github.com/nickgillian/grt/wiki)
* GRT Forum: [http://www.nickgillian.com/forum](http://www.nickgillian.com/forum)
* GRT API Reference: [http://nickgillian.com/grt/api/0.2.5/](http://nickgillian.com/grt/api/0.2.5)
* GRT Source Code: [https://github.com/nickgillian/grt](https://github.com/nickgillian/grt)
* GRT GUI Download: [http://www.nickgillian.com/wiki/pmwiki.php/GRT/Download](http://www.nickgillian.com/wiki/pmwiki.php/GRT/Download)
* GRT Journal of Machine Learning Research paper: [grt.pdf](http://jmlr.csail.mit.edu/papers/volume15/gillian14a/gillian14a.pdf)

## Core Algorithms
The GRT supports a wide number of supervised and unsupervised machine learning algorithms for classification, regression, and clustering, including:
* **Classification:** [Adaboost](https://github.com/nickgillian/grt/wiki/adaboost), [Decision Tree](https://github.com/nickgillian/grt/wiki/decision_tree), [Dynamic Time Warping](https://github.com/nickgillian/grt/wiki/dtw), [Gaussian Mixture Models](https://github.com/nickgillian/grt/wiki/gmm), [Hidden Markov Models](http://www.nickgillian.com/wiki/pmwiki.php/GRT/HMM), [k-nearest neighbor](https://github.com/nickgillian/grt/wiki/knn), [Naive Bayes](https://github.com/nickgillian/grt/wiki/anbc), [Random Forests](https://github.com/nickgillian/grt/wiki/random_forests), [Support Vector Machine](https://github.com/nickgillian/grt/wiki/svm), [Softmax](https://github.com/nickgillian/grt/wiki/softmax), and [more...](https://github.com/nickgillian/grt/wiki/reference#classifiers)

* **Regression:** [Linear Regression](https://github.com/nickgillian/grt/wiki/linear_regression), [Logistic Regression](https://github.com/nickgillian/grt/wiki/logistic_regression), [Neural Networks (Multilayer Perceptron)](https://github.com/nickgillian/grt/wiki/mlp)

* **Clustering:** [k-means](https://github.com/nickgillian/grt/blob/master/examples/ClusteringModulesExamples/KMeansExample/KMeansExample.cpp), [cluster tree](https://github.com/nickgillian/grt/blob/master/examples/ClusteringModulesExamples/ClusterTreeExample/ClusterTreeExample.cpp), [Gaussian Mixture Models](https://github.com/nickgillian/grt/blob/master/examples/ClusteringModulesExamples/GaussianMixtureModelsExample/GaussianMixtureModelsExample.cpp)

In addition to the machine learning algorithms above, the toolkit also includes a large number of algorithms for [preprocessing](https://github.com/nickgillian/grt/wiki/reference#pre-processing), [feature extraction](https://github.com/nickgillian/grt/wiki/reference#feature-extraction), and [post processing](https://github.com/nickgillian/grt/wiki/reference#post-processing).

See the [wiki](https://github.com/nickgillian/grt/wiki) for more details.

## GRT Extensions
There are now several extensions and third-party applications that use the GRT as the backend machine learning system, these include:

* [ofGrt](https://github.com/nickgillian/ofxGrt): an extension of the GRT for [openFrameworks](http://openframeworks.cc)
* [ml-lib](https://github.com/cmuartfab/ml-lib), by [Ali Momeni](http://alimomeni.net/) and [Jamie Bullock](http://jamiebullock.com): ml-lib is a library of machine learning externals for Max and Pure Data, designed to work on a variety of platforms including OS X, Windows, Linux, on Intel and ARM architectures.
* [ESP](https://github.com/damellis/ESP), by [David A. Mellis](https://github.com/damellis) and [Ben Zhang](https://www.benzhang.name): An interactive application that aims to help novices make sophisticated use of sensors in interactive projects through the application of machine learning.  The system is built using [openFrameworks](http://openframeworks.cc) and has several interesting examples built for [Arduino sensor modules](https://create.arduino.cc/projecthub/mellis/gesture-recognition-using-accelerometer-and-esp-71faa1) and more generic input data streams (e.g., network data).
* [Android Port](http://hollyhook.de/wp/grt-for-android): you can find a specific Android port of the GRT [here](http://hollyhook.de/wp/grt-for-android).

## GRT Architecture
To support flexibility while maintaining consistency, the GRT uses an object-oriented modular architecture. This architecture is built around a set of core **modules** and a central **gesture recognition pipeline**.

The input to both the modules and pipeline consists of a **N-dimensional floating-point vector**, making the toolkit flexible to the type of input signal. 
The algorithms in each module can be used as standalone classes; alternatively, a pipeline can be used to chain modules together to create a more sophisticated gesture-recognition system. The GRT includes modules for preprocessing, feature extraction, clustering, classification, regression and post processing.

The toolkit's source code is structured as follows:
* **ClassificationModules:** Contains all the GRT classification algorithms, such as AdaBoost, Naive Bayes, K-Nearest Neighbor, Support Vector Machines, and more.
* **ClusteringModules:** Contains all the GRT clustering algorithms, including K-Means, Gaussian Mixture Models, and Self-Organizing Maps.
* **ContextModules:** Contains all the GRT context modules, these are modules that can be connected to a gesture recognition pipeline to input additional context to a real-time classification system.
* **CoreAlgorithms:** Contains a number of algorithms that are used across the GRT, such as Particle Filters, Principal Component Analysis, and Restricted Boltzmann Machines.
* **CoreModules:** Contains all the GRT base classes, such as MLBase, Classifier, FeatureExtraction, etc..
* **DataStructures:** Contains all the GRT classes for recording, saving and loading datasets.
* **FeatureExtractionModules:** Contains all the GRT feature extraction modules.  These include FFT, Quantizers, and TimeDomainFeatures.
* **PostProcessingModules:** Contains all the GRT post-processing modules, including ClassLabelFilter and ClassLabelTimeoutFilter.
* **PreProcessingModules:** Contains all the GRT pre-processing modules, including LowPassFilter, HighPassFilter, DeadZone, and much more.
* **RegressionModules:** Contains all the GRT regression modules, such as MLP Neural Networks, Linear Regression, and Logistic Regression.
* **Util:** Contains a wide range of supporting classes, such as Logging, Util, TimeStamp, Random, and Matrix.

## Getting Started Example
This example demonstrates a few key components of the GRT, such as:
* how to load a dataset from a file (e.g., a CSV file)
* how to split a dataset into a training and test dataset
* how to set up a new Gesture Recognition Pipeline and add a classification algorithm to the pipeline
* how to use a training dataset to train a new classification model
* how to save/load a trained pipeline to/from a file
* how to use an automatically test dataset to test the accuracy of a classification model
* how to use a manually test dataset to test the accuracy of a classification model
* how to print detailed test results, such as precision, recall, and the confusion matrix

You can find this source code and a large number of other examples and tutorials in the GRT examples folder.

You should run this example with one argument, pointing to the file you want to load, for example:

```
 ./example my_data.csv
```

You can find several examples CSV files and other datasets in the main GRT data directory.

```C++
//Include the main GRT header
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[]) {
  //Parse the training data filename from the command line
  if (argc != 2) {
    cout << "Error: failed to parse data filename from command line. ";
    cout << "You should run this example with one argument pointing to a data file\n";
    return EXIT_FAILURE;
  }
  const string filename = argv[1];

  //Load some training data from a file
  ClassificationData trainingData;

  cout << "Loading dataset..." << endl;
  if (!trainingData.load(filename)) {
    cout << "ERROR: Failed to load training data from file\n";
    return EXIT_FAILURE;
  }

  cout << "Data Loaded" << endl;

  //Print out some stats about the training data
  trainingData.printStats();

  //Partition the training data into a training dataset and a test dataset. 80 means that 80%
  //of the data will be used for the training data and 20% will be returned as the test dataset
  cout << "Splitting data into training/test split..." << endl;
  ClassificationData testData = trainingData.split(80);

  //Create a new Gesture Recognition Pipeline
  GestureRecognitionPipeline pipeline;

  //Add a KNN classifier to the pipeline with a K value of 10
  pipeline << KNN(10);

  //Train the pipeline using the training data
  cout << "Training model..." << endl;
  if (!pipeline.train(trainingData)) {
    cout << "ERROR: Failed to train the pipeline!\n";
    return EXIT_FAILURE;
  }

  //Save the pipeline to a file
  if (!pipeline.save("HelloWorldPipeline.grt")) {
    cout << "ERROR: Failed to save the pipeline!\n";
    return EXIT_FAILURE;
  }

  //Load the pipeline from a file
  if (!pipeline.load("HelloWorldPipeline.grt")) {
    cout << "ERROR: Failed to load the pipeline!\n";
    return EXIT_FAILURE;
  }

  //Test the pipeline using the test data
  cout << "Testing model..." << endl;
  if (!pipeline.test(testData)) {
    cout << "ERROR: Failed to test the pipeline!\n";
    return EXIT_FAILURE;
  }

  //Print some stats about the testing
  cout << "Pipeline Test Accuracy: " << pipeline.getTestAccuracy() << endl;

  //Manually project the test dataset through the pipeline
  Float testAccuracy = 0.0;
  for (UINT i=0; i<testData.getNumSamples(); i++) {
    pipeline.predict(testData[i].getSample());

    if (testData[i].getClassLabel() == pipeline.getPredictedClassLabel()) {
      testAccuracy++;
    }
  }
  cout << "Manual test accuracy: " << testAccuracy / testData.getNumSamples() * 100.0 << endl;
   
  //Get the vector of class labels from the pipeline
  Vector< UINT > classLabels = pipeline.getClassLabels();

  //Print out the precision
  cout << "Precision: ";
  for (UINT k=0; k<pipeline.getNumClassesInModel(); k++) {
    cout << "\t" << pipeline.getTestPrecision(classLabels[k]);
  }cout << endl;

  //Print out the recall
  cout << "Recall: ";
  for (UINT k=0; k<pipeline.getNumClassesInModel(); k++) {
    cout << "\t" << pipeline.getTestRecall(classLabels[k]);
  }cout << endl;

  //Print out the f-measure
  cout << "FMeasure: ";
  for (UINT k=0; k<pipeline.getNumClassesInModel(); k++) {
    cout << "\t" << pipeline.getTestFMeasure(classLabels[k]);
  }cout << endl;

  //Print out the confusion matrix
  MatrixFloat confusionMatrix = pipeline.getTestConfusionMatrix();
  cout << "ConfusionMatrix: \n";
  for (UINT i=0; i<confusionMatrix.getNumRows(); i++) {
    for (UINT j=0; j<confusionMatrix.getNumCols(); j++) {
      cout << confusionMatrix[i][j] << "\t";
    }cout << endl;
  }

  return EXIT_SUCCESS;
}
```
## Tutorials and Examples

You can find a large number of tutorials and examples in the examples folder.  You can also find a
wide range of examples and references on the main GRT wiki:

http://www.nickgillian.com/wiki/pmwiki.php?n=GRT.GestureRecognitionToolkit

If you build the GRT using CMake, an examples folder will automatically be generated in the build directory after you successfully build the main GRT library. Example applications can
then be directly run from this example directory.  To run any of the examples, open terminal in the grt/build/examples directory and run:

    ./ExampleName

where *ExampleName* is the name of the example application you want to run.

## Forum

Note, at the moment the forum server is currently broken, we are working to resolve this.  In the meantime, use GitHub issues and pull requests.

You can find the link for the old forum at: [http://www.nickgillian.com/forum/](http://www.nickgillian.com/forum/)

## Bugs

Please submit bugs to the [github bug tracker](https://github.com/nickgillian/grt/issues).

## Contributions

All contributions are welcome, there are several ways in which users can contribute to the toolkit:

* improving the doxygen generated [API](http://nickgillian.com/grt/api/0.2.5) (by improving coverage and quality of the current documents in the existing code) 
* improving the higher-level documentation that can be found in the [wiki](https://github.com/nickgillian/grt/wiki)
* adding new examples or tutorials, or improving the existing ones
* adding new [unit tests](https://github.com/nickgillian/grt/tree/master/tests), to help ensure the quality of the current functions and catch potential bugs in the future

Please submit [pull requests](https://help.github.com/articles/about-pull-requests/) for any contribution.

## GRT Floating Point Precision
The GRT defaults to double precision floating point values.  The precision of the toolkit is defined by the following **Float** typedef:

```C++
typedef double Float; ///<This typedef is used to set floating-point precision throughout the GRT
```

This can easily be changed to single precision accuracy if needed by modifying the main GRT **Float** typedef value, defined in GRT/Util/GRTTypedefs.h header.

## VectorFloat and MatrixFloat Data Structures
The GRT uses two main data structures throughout the toolkit: *Vector* and *Matrix*.  These are templates and can, therefore, generalize to any C++ class.  The main things to know about these data types are:

- **Vector:** this inherits from the [STL vector class](http://www.cplusplus.com/reference/vector/vector/)
```C++
//Create an integer vector with a size of 3 elements
Vector< int > vec1(3);

//Create a string vector with a size of 2 elements
Vector< string > vec2(2);

//Create a Foo vector with a size of 5 elements
Vector< Foo > vec3(5);
```
- **Matrix:** this provides the base class for storing two dimensional arrays:
```C++
//Create an integer matrix with a size of 3x2
Matrix< int > mat1(3,2);

//Create a string matrix with a size of 2x2
Matrix< string > mat2(2,2);

//Create a Foo matrix with a size of 5x3
Matrix< Foo > mat3(5,3);
```
- **VectorFloat:** this provides the main data structure for storing floating point vector data. The precision of VectorFloat will automatically match that of GRT Float.
```C++
//Create a new vector with 10 elements
VectorFloat vector( 10 );
for(UINT i=0; i<vector.getSize(); i++){ 
    vector[i] = i*1.0; 
}
```
- **MatrixFloat:** this provides the main data structure for storing floating point matrix data. The precision of MatrixFloat will automatically match that of GRT Float.
```C++
//Create a [5x2] floating point matrix
MatrixFloat matrix(5,2);

//Loop over the data and set the values to a basic incrementing value
UINT counter = 0;
for(UINT i=0; i<matrix.getNumRows(); i++){
    for(UINT j=0; j<matrix.getNumCols(); j++){
        matrix[i][j] = counter++;
    }
}
```

## Building the GRT

You can find a CMakeLists file in the build folder that you can use to auto generate a makefile for your machine.

Read the readme file in the build folder to see how to build the GRT as a static library for Linux, OS X, or Windows.

## Installing and using the GRT in your C++ projects

See the build directory for details on how to build, install, and use the GRT in your C++ projects.

## License

The Gesture Recognition Toolkit is available under an MIT license.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
