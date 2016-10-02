#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT AdaBoost module

// Tests the default constructor
TEST(AdaBoost, Constructor) {
  
  AdaBoost adaBoost;

  //Check the id's matches
  EXPECT_TRUE( adaBoost.getId() == AdaBoost::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !adaBoost.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(AdaBoost, TrainBasicDataset) {

  //Note, the goal here is NOT to test the learning algorithm (i.e., accuracy of model), but instead to test the basic train/predict/getters/setters
  
  //Create a default adaboost instance
  AdaBoost adaBoost;

  //Check the module is not trained
  EXPECT_TRUE( !adaBoost.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 100;
  const UINT numClasses = 10;
  const UINT numDimensions = 10;
  ClassificationData trainingData = ClassificationData::generateGaussDataset( numSamples, numClasses, numDimensions, 10, 1 );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( adaBoost.train( trainingData ) );

  EXPECT_TRUE( adaBoost.getTrained() );
  EXPECT_TRUE( adaBoost.getNumInputs() == numDimensions );

  EXPECT_TRUE( adaBoost.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( adaBoost.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( adaBoost.save( "ada_boost_model.grt" ) );

  adaBoost.clear();
  EXPECT_TRUE( !adaBoost.getTrained() );

  EXPECT_TRUE( adaBoost.load( "ada_boost_model.grt" ) );

  EXPECT_TRUE( adaBoost.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( adaBoost.predict( testData[i].getSample() ) );
  }

}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
