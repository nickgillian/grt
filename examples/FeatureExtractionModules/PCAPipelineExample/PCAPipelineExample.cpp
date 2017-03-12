/*
 This example demonstrates how to use the PCA Feature Extraction module with a GestureRecognitionPipeline.

 The PCA Feature Extraction module provides an interface for the GRT PrincipalComponentAnalysis algorithm, 
 enabling the PrincipalComponentAnalysis algorithm to be used within a Gesture Recognition Pipeline.

 This example shows how to:
 - Load an existing classification data from a file.
 - Create a new PCA feature extraction instance and add it to a GestureRecognitionPipeline.
 - Add a GRT classifier (KNN) to the pipeline.
 - Train the PCA model.
 - Train the KNN model, using the PCA for feature extraction and input to the KNN classifier.
 - Print out the results.

 Run this example with one argument, pointing to the input classification data file you want to load. 
 For example, running this from the grt build directory: ./PCAPipelineExample ../../data/IrisData.grt
 */

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[]) {

  InfoLog info("[PCA Pipeline Example]");

  // Parse the filename from the input
  if (argc != 2) {
    info << "Failed to parse filename from input, run this example with the path to the file you want to parse." << std::endl;
    return EXIT_FAILURE;
  }
  const std::string filename = argv[1];

  ClassificationData data;

  info << "Loading data..." << std::endl;
  if (!data.load(filename)) {
    info << "Failed to load data from file: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  //Create a new pipeline
  GestureRecognitionPipeline pipeline;

  const UINT numSamples = data.getNumSamples();
  const UINT numInputDimensions = data.getNumDimensions();
  const UINT numOutputDimensions = 3;  // This is the number of principal components

  info << "Data loaded. Num Samples: " << numSamples;
  info << " Num Input Dimensions: " << numInputDimensions;
  info << " Num Output Dimensions: " << numOutputDimensions << std::endl;

  // Create an instance of the PCA feature extraction module and add it to the pipeline
  pipeline << PCA(numInputDimensions, numOutputDimensions);

  // Add a KNN Classifier to the pipeline
  pipeline << KNN(10);

  // Get a pointer to the PCA module we just added to the pipeline so we can train it
  // This is a little bit of a hack until the pipeline supports automatic training of the feature modules
  {
    PCA *pca = pipeline.getFeatureExtractionModule<PCA>(0);
    if (!pca->train(data)) {
      info << "Failed to train PCA model!" << std::endl;
      return EXIT_FAILURE;
    }
  }

  // Train the model, this first project the data through the trained PCA module before inputting it
  // into the classifier
  info << "Training model..." << std::endl;
  if (!pipeline.train(data)) {
    info << "Failed to train pipeline model!" << std::endl;
    return EXIT_FAILURE;
  }

  info << "Model trained." << std::endl;

  return EXIT_SUCCESS;
}