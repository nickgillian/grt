#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT AdaBoost module

// Tests the default constructor
TEST(AdaBoost, TestDefaultConstructor) {
  
  AdaBoost adaBoost;

  //Check the id's matches
  EXPECT_TRUE( adaBoost.getId() == AdaBoost::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !adaBoost.getTrained() );
}

// Tests the copy constructor
TEST(AdaBoost, TestCopyConstructor) {
  
  AdaBoost adaBoost;

  //Check the id's matches
  EXPECT_TRUE( adaBoost.getId() == AdaBoost::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !adaBoost.getTrained() );

  AdaBoost adaBoost2 = adaBoost;

  //Check the id's matches
  EXPECT_TRUE( adaBoost2.getId() == AdaBoost::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !adaBoost2.getTrained() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(AdaBoost, TestBasicTrainPredictFunctions ) {

  //Note, the goal here is NOT to test the learning algorithm (i.e., accuracy of model), but instead to test the basic train/predict/getters/setters
  
  //Create a default adaboost instance
  AdaBoost adaBoost;

  //Disable the training logging
  EXPECT_TRUE( adaBoost.setTrainingLoggingEnabled( false ) );
  EXPECT_TRUE( !adaBoost.getTrainingLoggingEnabled() );

  //Check the module is not trained
  EXPECT_TRUE( !adaBoost.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 20;
  const UINT numClasses = 5;
  const UINT numDimensions = 3;
  ClassificationData trainingData = ClassificationData::generateGaussDataset( numSamples, numClasses, numDimensions, 10, 1 );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( adaBoost.train( trainingData ) );

  std::cerr << "adaBoost.getNumClasses(): " << adaBoost.getNumClasses() << " numClasses: " << numClasses << std::endl;

  EXPECT_TRUE( adaBoost.getTrained() );
  EXPECT_TRUE( adaBoost.getNumInputDimensions() == numDimensions );
  EXPECT_TRUE( adaBoost.getNumOutputDimensions() == numClasses );
  EXPECT_TRUE( adaBoost.getNumClasses() == numClasses );

  EXPECT_TRUE( adaBoost.print() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( adaBoost.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( adaBoost.save( "ada_boost_model.grt" ) );

  EXPECT_TRUE( adaBoost.clear() );
  EXPECT_TRUE( !adaBoost.getTrained() );

  EXPECT_TRUE( adaBoost.load( "ada_boost_model.grt" ) );
  EXPECT_TRUE( adaBoost.getTrained() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( adaBoost.predict( testData[i].getSample() ) );
  }
}

// Tests the learning algorithm on a basic dataset
TEST(AdaBoost, TrainGaussDataset) {

  //Note, the goal here IS to test the learning algorithm (i.e., accuracy of model), the training data below is selected in a 
  //way that is biased so the algorithm should be able to train a model....if it can't then this indicates there is a bug
  
  //Create a default adaboost instance
  AdaBoost adaBoost;

  //Disable the training logging
  EXPECT_TRUE( adaBoost.setTrainingLoggingEnabled( false ) );
  EXPECT_TRUE( !adaBoost.getTrainingLoggingEnabled() );

  //Check the module is not trained
  EXPECT_TRUE( !adaBoost.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 1000;
  const UINT numClasses = 10;
  const UINT numDimensions = 10;
  ClassificationData trainingData = ClassificationData::generateGaussDataset( numSamples, numClasses, numDimensions, 10, 1 );

  ClassificationData testData = trainingData.split( 50 );

  //Turn off the validation set for training
  EXPECT_TRUE( adaBoost.setUseValidationSet( false ) );
  EXPECT_TRUE( !adaBoost.getUseValidationSet() ); //This should now be false

  //Train the classifier
  EXPECT_TRUE( adaBoost.train( trainingData ) );

  EXPECT_TRUE( adaBoost.getTrained() );
  EXPECT_TRUE( adaBoost.getNumInputDimensions() == numDimensions );
  EXPECT_TRUE( adaBoost.getNumOutputDimensions() == numClasses );
  std::cout << "adaBoost.getNumClasses(): " << adaBoost.getNumClasses() << " numClasses: " << numClasses << std::endl;
  EXPECT_TRUE( adaBoost.getNumClasses() == numClasses );
  EXPECT_TRUE( adaBoost.getTrainingSetAccuracy() >= 75.0 ); //On this basic dataset we expect to get at least 75% accuracy
  EXPECT_TRUE( adaBoost.getValidationSetAccuracy() == 0.0 ); //Validation is off, so the validation accuracy should be zero

  EXPECT_TRUE( adaBoost.print() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( adaBoost.predict( testData[i].getSample() ) );
  }

  //Save the model to a file
  EXPECT_TRUE( adaBoost.save( "ada_boost_model.grt" ) );

  //Clear the model
  EXPECT_TRUE( adaBoost.clear() );
  EXPECT_TRUE( !adaBoost.getTrained() );

  //Load it from a file again
  EXPECT_TRUE( adaBoost.load( "ada_boost_model.grt" ) );
  EXPECT_TRUE( adaBoost.getTrained() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( adaBoost.predict( testData[i].getSample() ) );
  }

  //Test that the model can be copied to another instance
  AdaBoost adaBoost2( adaBoost );

  EXPECT_TRUE( adaBoost2.getTrained() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( adaBoost2.predict( testData[i].getSample() ) );
  }
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
