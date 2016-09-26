#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT HMM module

// Tests the default constructor
TEST(HMM, Constructor) {
  
  HMM hmm;

  //Check the type matches
  EXPECT_TRUE( hmm.getClassifierType() == HMN::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !hmm.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(HMM, TrainBasicDataset) {
  
  HMM hmm;

  //Check the module is not trained
  EXPECT_TRUE( !hmm.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( hmm.train( trainingData ) );

  EXPECT_TRUE( hmm.getTrained() );

  EXPECT_TRUE( hmm.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( hmm.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( hmm.save( "hmm_model.grt" ) );

  hmm.clear();

  EXPECT_TRUE( !hmm.getTrained() );

  EXPECT_TRUE( hmm.load( "hmm_model.grt" ) );

  EXPECT_TRUE( hmm.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( hmm.predict( hmm[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
