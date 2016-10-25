#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT SVM module

// Tests the default constructor
TEST(SVM, Constructor) {
  
  SVM svm;

  //Check the type matches
  EXPECT_TRUE( svm.getClassifierType() == SVM::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !svm.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(SVM, TrainBasicDataset) {
  
  SVM svm;

  //Check the module is not trained
  EXPECT_TRUE( !svm.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 1000;
  const UINT numClasses = 10;
  const UINT numDimensions = 10;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( svm.train( trainingData ) );

  EXPECT_TRUE( svm.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( svm.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( svm.save( "svm_model.grt" ) );

  svm.clear();

  EXPECT_TRUE( !svm.getTrained() );

  EXPECT_TRUE( svm.load( "svm_model.grt" ) );

  EXPECT_TRUE( svm.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( svm.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
