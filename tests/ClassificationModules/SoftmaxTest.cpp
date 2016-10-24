#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Softmax module

// Tests the default constructor
TEST(Softmax, Constructor) {
  
  Softmax sm;

  //Check the type matches
  EXPECT_TRUE( sm.getClassifierType() == Softmax::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !sm.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(Softmax, TrainBasicDataset) {
  
  Softmax sm;

  //Check the module is not trained
  EXPECT_TRUE( !sm.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 1000;
  const UINT numClasses = 10;
  const UINT numDimensions = 10;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( sm.train( trainingData ) );

  EXPECT_TRUE( sm.getTrained() );

  EXPECT_TRUE( sm.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( sm.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( sm.save( "sm_model.grt" ) );

  sm.clear();

  EXPECT_TRUE( !sm.getTrained() );

  EXPECT_TRUE( sm.load( "sm_model.grt" ) );

  EXPECT_TRUE( sm.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( sm.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
