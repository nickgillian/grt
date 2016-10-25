#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT MinDist module

// Tests the default constructor
TEST(MinDist, Constructor) {
  
  MinDist md;

  //Check the type matches
  EXPECT_TRUE( md.getClassifierType() == MinDist::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !md.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(MinDist, TrainBasicDataset) {
  
  MinDist md;

  //Check the module is not trained
  EXPECT_TRUE( !md.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 1000;
  const UINT numClasses = 10;
  const UINT numDimensions = 10;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( md.train( trainingData ) );

  EXPECT_TRUE( md.getTrained() );

  EXPECT_TRUE( md.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( md.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( md.save( "min_dist_model.grt" ) );

  md.clear();

  EXPECT_TRUE( !md.getTrained() );

  EXPECT_TRUE( md.load( "min_dist_model.grt" ) );

  EXPECT_TRUE( md.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( md.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
