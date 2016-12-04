#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT Softmax module

// Tests the default constructor
TEST(Softmax, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::Softmax> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(Softmax, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::Softmax> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(Softmax, TestBasicTrainPredictFunctions ) {
  GRT::ClassifierUnitTestHelper<GRT::Softmax> tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(Softmax, TrainGaussDataset) {
  GRT::ClassifierUnitTestHelper<GRT::Softmax> tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}