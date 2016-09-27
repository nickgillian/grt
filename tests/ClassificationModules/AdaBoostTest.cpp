#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT AdaBoost module

// Tests the default constructor
TEST(AdaBoost, Constructor) {
  
  AdaBoost adaBoost;

  //Check the type matches
  EXPECT_TRUE( adaBoost.getClassifierType() == AdaBoost::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !adaBoost.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(AdaBoost, TrainBasicDataset) {
  
  AdaBoost adaBoost;

  //Check the module is not trained
  EXPECT_TRUE( !adaBoost.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( adaBoost.train( trainingData ) );

  EXPECT_TRUE( adaBoost.getTrained() );

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
