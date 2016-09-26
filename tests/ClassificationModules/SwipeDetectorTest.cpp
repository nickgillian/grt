#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT SwipeDetector module

// Tests the default constructor
TEST(SVM, Constructor) {
  
  SwipeDetector sd;

  //Check the type matches
  EXPECT_TRUE( sd.getClassifierType() == SVM::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !sd.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(SwipeDetector, TrainBasicDataset) {
  
  SwipeDetector sd;

  //Check the module is not trained
  EXPECT_TRUE( !sd.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( sd.train( trainingData ) );

  EXPECT_TRUE( sd.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( svm.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( sd.save( "sd_model.grt" ) );

  sd.clear();

  EXPECT_TRUE( !sd.getTrained() );

  EXPECT_TRUE( sd.load( "sd_model.grt" ) );

  EXPECT_TRUE( sd.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( sd.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
