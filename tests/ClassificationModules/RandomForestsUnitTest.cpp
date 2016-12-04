#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT RandomForests module

// Tests the default constructor
TEST(RandomForests, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::RandomForests> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(RandomForests, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::RandomForests> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(RandomForests, TestBasicTrainPredictFunctions ) {
  GRT::ClassifierUnitTestHelper<GRT::RandomForests> tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(RandomForests, TrainGaussDataset) {
  GRT::ClassifierUnitTestHelper<GRT::RandomForests> tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}