/*
 This example demonstrates how to use the PCA Feature Extraction module.

 The PCA Feature Extraction module provides an interface for the GRT PrincipalComponentAnalysis algorithm, 
 enabling the PrincipalComponentAnalysis algorithm to be used within a Gesture Recognition Pipeline.

 This example shows how to:
 - Load an existing classification data from a file
 - Create a new PCA feature extraction instance.
 - Train the PCA model.
 - Project the data through the trained feature extraction model.
 - Print out the results.

 Run this example with one argument, pointing to the input classification data file you want to load. 
 For example, running this from the grt build directory: ./PCAExample ../../data/IrisData.grt
 */

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[]) {

  InfoLog info("[PCA Example]");

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

  const UINT numSamples = data.getNumSamples();
  const UINT numInputDimensions = data.getNumDimensions();
  const UINT numOutputDimensions = 2;  // This is the number of principal components

  info << "Data loaded. Num Samples: " << numSamples;
  info << " Num Input Dimensions: " << numInputDimensions;
  info << " Num Output Dimensions: " << numOutputDimensions << std::endl;

  // Create an instance of the PCA feature extraction module
  PCA pca(numInputDimensions, numOutputDimensions);

  // Train the PCA model
  info << "Training PCA model..." << std::endl;
  if (!pca.train(data)) {
    info << "Failed to train PCA model!" << std::endl;
    return EXIT_FAILURE;
  }

  info << "Model trained." << std::endl;

  // Get the internal PrincipalComponentAnalysis instance and plot the trained mean vector
  PrincipalComponentAnalysis *pcomp = pca.getPCA();
  VectorFloat meanVector = pcomp->getMeanVector();
  info << "pca mean vector: ";
  for (UINT j=0; j<meanVector.getSize(); j++) {
    info << meanVector[j] << " ";
  }
  info << std::endl;

  // Project the data through the feature extraction module
  for (UINT i=0; i<numSamples; i++) {

    info << "label: " << data[i].getClassLabel();

    // Print the input data
    const VectorFloat input = data[i].getSample();
    info << " input: ";
    for (UINT j=0; j<input.getSize(); j++) {
      info << input[j] << " ";
    }

    // Project the input through the feature extraction module
    if (!pca.predict(input)) {
      info << "Failed to predict sample: " << i << std::endl;
      return EXIT_FAILURE;
    }

    // Print the output feature data
    const VectorFloat &features = pca.getFeatureVector();
    info << " output: ";
    for (UINT j=0; j<features.getSize(); j++) {
      info << features[j] << " ";
    }
    info << std::endl;
  }

  return EXIT_SUCCESS;
}