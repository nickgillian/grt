#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT KNN module

// Tests the default constructor
TEST(KNN, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::KNN> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(KNN, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::KNN> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(KNN, TestBasicTrainPredictFunctions ) {
  GRT::ClassifierUnitTestHelper<GRT::KNN> tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(KNN, TrainGaussDataset) {
  GRT::ClassifierUnitTestHelper<GRT::KNN> tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}