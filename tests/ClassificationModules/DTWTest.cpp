#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT DWT module

// Tests the default constructor
TEST(DTW, Constructor) {
  
  DTW dtw;

  //Check the type matches
  EXPECT_TRUE( dtw.getClassifierType() == DecisionTree::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !dtw.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(DecisionTree, TrainBasicDataset) {
  
  DecisionTree dtw;

  //Check the module is not trained
  EXPECT_TRUE( !dtw.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( dtw.train( trainingData ) );

  EXPECT_TRUE( dtw.getTrained() );

  EXPECT_TRUE( dtw.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( dtw.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( dtw.save( "dtw_model.grt" ) );

  dtw.clear();

  EXPECT_TRUE( !dtw.getTrained() );

  EXPECT_TRUE( dtw.load( "dtw.grt" ) );

  EXPECT_TRUE( dtw.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( dtw.predict( dtw[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
