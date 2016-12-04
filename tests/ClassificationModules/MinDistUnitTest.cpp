#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT MinDist module

// Tests the default constructor
TEST(MinDist, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::MinDist> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(MinDist, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::MinDist> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(MinDist, TestBasicTrainPredictFunctions ) {
  GRT::ClassifierUnitTestHelper<GRT::MinDist> tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(MinDist, TrainGaussDataset) {
  GRT::ClassifierUnitTestHelper<GRT::MinDist> tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}