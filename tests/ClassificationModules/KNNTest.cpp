#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT KNN module

// Tests the default constructor
TEST(KNN, Constructor) {
  
  KNN knn;

  //Check the type matches
  EXPECT_TRUE( knn.getClassifierType() == KNN::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !knn.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(KNN, TrainBasicDataset) {
  
  KNN knn;

  //Check the module is not trained
  EXPECT_TRUE( !knn.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 1000;
  const UINT numClasses = 10;
  const UINT numDimensions = 10;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( knn.train( trainingData ) );

  EXPECT_TRUE( knn.getTrained() );

  EXPECT_TRUE( knn.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( knn.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( knn.save( "knn_model.grt" ) );

  knn.clear();

  EXPECT_TRUE( !knn.getTrained() );

  EXPECT_TRUE( knn.load( "knn_model.grt" ) );

  EXPECT_TRUE( knn.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( knn.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
