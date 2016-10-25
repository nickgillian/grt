#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT RandomForest module

// Tests the default constructor
TEST(RandomForests, Constructor) {
  
  RandomForests rf;

  //Check the type matches
  EXPECT_TRUE( rf.getClassifierType() == RandomForests::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !rf.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(RandomForests, TrainBasicDataset) {
  
  RandomForests rf;

  //Check the module is not trained
  EXPECT_TRUE( !rf.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 1000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( rf.train( trainingData ) );

  EXPECT_TRUE( rf.getTrained() );

  EXPECT_TRUE( rf.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( rf.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( rf.save( "rf_model.grt" ) );

  rf.clear();

  EXPECT_TRUE( !rf.getTrained() );

  EXPECT_TRUE( rf.load( "rf_model.grt" ) );

  EXPECT_TRUE( rf.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( rf.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
