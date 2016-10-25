#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT GMM module

// Tests the default constructor
TEST(GMM, Constructor) {
  
  GMM gmm;

  //Check the type matches
  EXPECT_TRUE( gmm.getClassifierType() == GMM::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !gmm.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(GMM, TrainBasicDataset) {
  
  GMM gmm;

  //Check the module is not trained
  EXPECT_TRUE( !gmm.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( gmm.train( trainingData ) );

  EXPECT_TRUE( gmm.getTrained() );

  EXPECT_TRUE( gmm.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( gmm.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( gmm.save( "gmm_model.grt" ) );

  gmm.clear();

  EXPECT_TRUE( !gmm.getTrained() );

  EXPECT_TRUE( gmm.load( "gmm_model.grt" ) );

  EXPECT_TRUE( gmm.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( gmm.predict( testData[i].getSample() ) );
  }

}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
