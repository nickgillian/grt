#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT FiniteStateMachine module

// Tests the default constructor
TEST(FiniteStateMachine, Constructor) {
  
  FiniteStateMachine fsm;

  //Check the type matches
  EXPECT_TRUE( fsm.getClassifierType() == SVM::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !fsm.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(SVM, TrainBasicDataset) {
  
  SVM fsm;

  //Check the module is not trained
  EXPECT_TRUE( !fsm.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( fsm.train( trainingData ) );

  EXPECT_TRUE( fsm.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( fsm.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( fsm.save( "fsm_model.grt" ) );

  fsm.clear();

  EXPECT_TRUE( !fsm.getTrained() );

  EXPECT_TRUE( fsm.load( "fsm_model.grt" ) );

  EXPECT_TRUE( fsm.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( fsm.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
