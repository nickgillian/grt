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
 You should run this example with one argument pointing to the data you want to load.
 A good dataset to run this example is acc-orientation.grt, which can be found in the GRT data folder.
 */

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