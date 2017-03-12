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

#define GRT_DLL_EXPORTS
#include "PCA.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
std::string PCA::id = "PCA";
std::string PCA::getId() { return PCA::id; }

//Register your module with the FeatureExtraction base class
RegisterFeatureExtractionModule< PCA > PCA::registerModule(PCA::getId());

PCA::PCA(const UINT numDimensions, const UINT numPrincipalComponents) : FeatureExtraction(PCA::getId(), true) {
  if (numDimensions > 0 && numPrincipalComponents > 0) {
    init(numDimensions, numPrincipalComponents);
  }
}

PCA::PCA(const PCA &rhs) : FeatureExtraction(PCA::getId(), true) {
  //Invoke the equals operator to copy the data from the rhs instance to this instance
  *this = rhs;
}

PCA::~PCA() {}

PCA& PCA::operator=(const PCA &rhs) {  
  if (this!=&rhs) {
    this->pca = rhs.pca;
    
    //Copy the base variables
    copyBaseVariables(dynamic_cast<const FeatureExtraction*>(&rhs));
  }
  return *this;
}

bool PCA::deepCopyFrom(const FeatureExtraction *featureExtraction) {
    
  if (featureExtraction == NULL) return false;
  
  if (this->getId() == featureExtraction->getId()) {
      
    //Cast the feature extraction pointer to a pointer to your custom feature extraction module
    //Then invoke the equals operator
    *this = *dynamic_cast<const PCA*>(featureExtraction);
    
    return true;
  }
  
  errorLog << __GRT_LOG__ << " FeatureExtraction Types Do Not Match!" << std::endl;
  return false;
}

bool PCA::computeFeatures(const VectorFloat &inputVector) {

  featureDataReady = false;
    
  if (!initialized) return false;

  if (!trained) return false;

  if (inputVector.getSize() != numInputDimensions) return false;

  if (!pca.project(inputVector, featureVector)) {
    return false;
  }
  
  //Flag that the feature data is ready
  featureDataReady = true;
  
  return true;
}

bool PCA::computeFeatures(const MatrixFloat &inputMatrix) {

  featureDataReady = false;
    
  if (!initialized) return false;

  if (!trained) return false;

  if (inputMatrix.getNumCols() != numInputDimensions) return false;

  if (!pca.project(inputMatrix, featureMatrix)) {
    return false;
  }
  
  //Flag that the feature data is ready
  featureDataReady = true;
  
  return true;
}

bool PCA::reset() {
  return pca.reset();
}

bool PCA::save(std::fstream &file) const {
    
  if (!file.is_open()) {
    errorLog << __GRT_LOG__ << " The file is not open!" << std::endl;
    return false;
  }
  
  //First, you should add a header (with no spaces) e.g.
  file << "PCA_FILE_V1.0" << std::endl;
  
  //Second, you should save the base feature extraction settings to the file
  if (!saveFeatureExtractionSettingsToFile(file)) {
    errorLog << __GRT_LOG__ << " Failed to save base feature extraction settings to file!" << std::endl;
    return false;
  }
  
  return true;
}

bool PCA::load(std::fstream &file) {
    
  clear();
  
  if (!file.is_open()) {
    errorLog << __GRT_LOG__ << " The file is not open!" << std::endl;
    return false;
  }
  
  std::string word;
  
  //First, you should read and validate the header
  file >> word;
  if (word != "PCA_V1.0") {
    errorLog << __GRT_LOG__ << " Invalid file format! " << word << std::endl;
    return false;
  }
  
  //Second, you should load the base feature extraction settings to the file
  if (!loadFeatureExtractionSettingsFromFile(file)) {
    errorLog << __GRT_LOG__ << " Failed to load base feature extraction settings from file!" << std::endl;
    return false;
  }
  
  return pca.save(file);
}

bool PCA::init(const UINT numDimensions, const UINT numPrincipalComponents) {
    
  clear();
  
  if (numDimensions == 0) return false;
  
  numInputDimensions = numDimensions;
  numOutputDimensions = numPrincipalComponents;
  
  // Call the feature extraction base class init function to setup the feature extraction buffers
  return FeatureExtraction::init();
}

bool PCA::train_(MatrixFloat &data) {

  // Clear the previous model
  trained = false;
  pca.clear();

  if (data.getNumCols() != numInputDimensions) {
    errorLog << __GRT_LOG__ << " The number of columns in the input data (";
    errorLog << data.getNumCols() << ") does not match the expected number of inputs (";
    errorLog << numInputDimensions << "). " << std::endl;
    return false;
  }

  const UINT numPrincipalComponents = numOutputDimensions;
  const bool normData = true;
  if (!pca.computeFeatureVector(data, numPrincipalComponents, normData)) {
    errorLog << __GRT_LOG__ << " Failed to train PCA module!" << std::endl;
    return false;
  }

  // Flag that the model has been trained
  trained = true;

  // Reset the module so it's ready for the next input
  reset();

  return true;
}

bool PCA::train_(ClassificationData &data) {

  const UINT numSamples = data.getNumSamples();
  const UINT numDimensions = data.getNumDimensions();
  MatrixFloat mat(numSamples, numDimensions);

  for (UINT i=0; i<numSamples; i++) {
    for (UINT j=0; j<numDimensions; j++) {
      mat[i][j] = data[i][j];
    }
  }

  return train_(mat);
}

PrincipalComponentAnalysis* PCA::getPCA() {
  return &pca;
}

GRT_END_NAMESPACE
