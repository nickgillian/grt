#include "ClassifierUnitTestHelper.h"

// Tests the default constructor
TEST(DecisionTree, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::DecisionTree> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(DecisionTree, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::DecisionTree> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(DecisionTree, TestBasicTrainPredictFunctions ) {
  GRT::ClassifierUnitTestHelper<GRT::DecisionTree> tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(DecisionTree, TrainGaussDataset) {
  GRT::ClassifierUnitTestHelper<GRT::DecisionTree> tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}