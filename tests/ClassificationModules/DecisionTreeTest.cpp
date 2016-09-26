#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Decision Tree module

// Tests the default constructor
TEST(DecisionTree, Constructor) {
  
  DecisionTree tree;

  //Check the type matches
  EXPECT_TRUE( tree.getClassifierType() == DecisionTree::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !tree.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(DecisionTree, TrainBasicDataset) {
  
  DecisionTree tree;

  //Check the module is not trained
  EXPECT_TRUE( !tree.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Train the classifier
  EXPECT_TRUE( tree.train( trainingData ) );

  EXPECT_TRUE( tree.getTrained() );

  EXPECT_TRUE( tree.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( tree.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( tree.save( "tree_model.grt" ) );

  tree.clear();

  EXPECT_TRUE( !tree.getTrained() );

  EXPECT_TRUE( tree.load( "tree_model.grt" ) );

  EXPECT_TRUE( tree.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( tree.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
