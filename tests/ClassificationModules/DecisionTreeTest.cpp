#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT DecisionTree module
typedef DecisionTree CLASSIFIER;
const std::string modelFilename = "dtree_model.grt";

// Tests the default constructor
TEST(CLASSIFIER, TestDefaultConstructor) {
  
  CLASSIFIER classifier;

  //Check the id's matches
  EXPECT_TRUE( classifier.getId() == CLASSIFIER::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !classifier.getTrained() );
}

// Tests the copy constructor
TEST(CLASSIFIER, TestCopyConstructor) {
  
  CLASSIFIER classifier;

  //Check the id's matches
  EXPECT_TRUE( classifier.getId() == CLASSIFIER::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !classifier.getTrained() );

  CLASSIFIER classifier2 = classifier;

  //Check the id's matches
  EXPECT_TRUE( classifier2.getId() == CLASSIFIER::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !classifier2.getTrained() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(CLASSIFIER, TestBasicTrainPredictFunctions ) {

  //Note, the goal here is NOT to test the learning algorithm (i.e., accuracy of model), but instead to test the basic train/predict/getters/setters
  
  //Create a default adaboost instance
  CLASSIFIER classifier;

  //Disable the training logging
  EXPECT_TRUE( classifier.setTrainingLoggingEnabled( false ) );
  EXPECT_TRUE( !classifier.getTrainingLoggingEnabled() );

  //Check the module is not trained
  EXPECT_TRUE( !classifier.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 1000;
  const UINT numClasses = 5;
  const UINT numDimensions = 3;
  ClassificationData trainingData = ClassificationData::generateGaussDataset( numSamples, numClasses, numDimensions, 10, 1 );

  ClassificationData testData = trainingData.split( 50, true );

  //Train the classifier
  EXPECT_TRUE( classifier.train( trainingData ) );
  EXPECT_TRUE( classifier.getTrained() );
  EXPECT_TRUE( classifier.getNumInputDimensions() == numDimensions );
  EXPECT_TRUE( classifier.getNumOutputDimensions() == numClasses );
  EXPECT_TRUE( classifier.getNumClasses() == numClasses );

  EXPECT_TRUE( classifier.print() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( classifier.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( classifier.save( modelFilename ) );

  EXPECT_TRUE( classifier.clear() );
  EXPECT_TRUE( !classifier.getTrained() );

  EXPECT_TRUE( classifier.load( modelFilename ) );
  EXPECT_TRUE( classifier.getTrained() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( classifier.predict( testData[i].getSample() ) );
  }
}

// Tests the learning algorithm on a basic dataset
TEST(CLASSIFIER, TrainGaussDataset) {

  //Note, the goal here IS to test the learning algorithm (i.e., accuracy of model), the training data below is selected in a 
  //way that is biased so the algorithm should be able to train a model....if it can't then this indicates there is a bug
  
  //Create a default adaboost instance
  CLASSIFIER classifier;

  //Disable the training logging
  EXPECT_TRUE( classifier.setTrainingLoggingEnabled( false ) );
  EXPECT_TRUE( !classifier.getTrainingLoggingEnabled() );

  //Check the module is not trained
  EXPECT_TRUE( !classifier.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 1000;
  const UINT numClasses = 10;
  const UINT numDimensions = 10;
  ClassificationData trainingData = ClassificationData::generateGaussDataset( numSamples, numClasses, numDimensions, 10, 1 );

  ClassificationData testData = trainingData.split( 50, true );

  //Turn off the validation set for training
  EXPECT_TRUE( classifier.setUseValidationSet( false ) );
  EXPECT_TRUE( !classifier.getUseValidationSet() ); //This should now be false

  //Train the classifier
  EXPECT_TRUE( classifier.train( trainingData ) );

  EXPECT_TRUE( classifier.getTrained() );
  EXPECT_TRUE( classifier.getNumInputDimensions() == numDimensions );
  EXPECT_TRUE( classifier.getNumOutputDimensions() == numClasses );
  EXPECT_TRUE( classifier.getNumClasses() == numClasses );
  EXPECT_TRUE( classifier.getTrainingSetAccuracy() >= 75.0 ); //On this basic dataset we expect to get at least 75% accuracy
  EXPECT_TRUE( classifier.getValidationSetAccuracy() == 0.0 ); //Validation is off, so the validation accuracy should be zero

  EXPECT_TRUE( classifier.print() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( classifier.predict( testData[i].getSample() ) );
  }

  //Save the model to a file
  EXPECT_TRUE( classifier.save( modelFilename ) );

  //Clear the model
  EXPECT_TRUE( classifier.clear() );
  EXPECT_TRUE( !classifier.getTrained() );

  //Load it from a file again
  EXPECT_TRUE( classifier.load( modelFilename ) );
  EXPECT_TRUE( classifier.getTrained() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( classifier.predict( testData[i].getSample() ) );
  }

  //Test that the model can be copied to another instance
  CLASSIFIER classifier2( classifier );

  EXPECT_TRUE( classifier2.getTrained() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( classifier2.predict( testData[i].getSample() ) );
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
