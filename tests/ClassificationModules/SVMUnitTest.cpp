#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT SVM module

// Tests the default constructor
TEST(SVM, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::SVM> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(SVM, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::SVM> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(SVM, TestBasicTrainPredictFunctions ) {
  GRT::ClassifierUnitTestHelper<GRT::SVM> tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(SVM, TrainGaussDataset) {
  GRT::ClassifierUnitTestHelper<GRT::SVM> tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}