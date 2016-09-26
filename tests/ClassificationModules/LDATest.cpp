#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT LDA module

// Tests the default constructor
TEST(LDA, Constructor) {
  
  LDA lda;

  //Check the type matches
  EXPECT_TRUE( lda.getClassifierType() == KNN::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !lda.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(LDA, TrainBasicDataset) {
  
  LDA lda;

  //Check the module is not trained
  EXPECT_TRUE( !knn.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( lda.train( trainingData ) );

  EXPECT_TRUE( lda.getTrained() );

  EXPECT_TRUE( lda.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( knn.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( lda.save( "lda_model.grt" ) );

  lda.clear();

  EXPECT_TRUE( !lda.getTrained() );

  EXPECT_TRUE( lda.load( "lda_model.grt" ) );

  EXPECT_TRUE( lda.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( lda.predict( lda[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
