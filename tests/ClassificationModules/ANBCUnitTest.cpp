#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT ANBC module

// Tests the default constructor
TEST(ANBC, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::ANBC> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(ANBC, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::ANBC> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(ANBC, TestBasicTrainPredictFunctions ) {
  GRT::ClassifierUnitTestHelper<GRT::ANBC> tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(ANBC, TrainGaussDataset) {
  GRT::ClassifierUnitTestHelper<GRT::ANBC> tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
