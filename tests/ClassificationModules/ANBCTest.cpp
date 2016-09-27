#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT ANBC Classifier

// Tests the default constructor
TEST(ANBC, Constructor) {
  
  ANBC anbc;

  //Check the type matches
  EXPECT_TRUE( anbc.getClassifierType() == ANBC::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !anbc.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(ANBC, TrainBasicDataset) {
  
  ANBC anbc;

  //Check the module is not trained
  EXPECT_TRUE( !anbc.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10 * 1000;
  const UINT numClasses = 10;
  const UINT numDimensions = 1000;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( anbc.train( trainingData ) );

  EXPECT_TRUE( anbc.getTrained() );

  EXPECT_TRUE( anbc.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( anbc.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( anbc.save( "anbc_model.grt" ) );

  anbc.clear();

  EXPECT_TRUE( !anbc.getTrained() );

  EXPECT_TRUE( anbc.load( "anbc_model.grt" ) );

  EXPECT_TRUE( anbc.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( anbc.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
