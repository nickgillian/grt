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

// Tests the default constructor
TEST(DecisionTree, TrainBasicDataset) {
  
  DecisionTree tree;

  //Check the module is not trained
  EXPECT_TRUE( !tree.getTrained() );

  //Generate a basic dataset
  ClassificationData::generateGaussDataset( "gauss_data.csv", 10000, 10, 100, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 80 );

  //Train the classifier
  EXPECT_TRUE( tree.train( trainingData ) );

}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
