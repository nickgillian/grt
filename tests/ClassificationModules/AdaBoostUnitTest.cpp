#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT AdaBoost module

// Tests the default constructor
TEST(AdaBoost, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::AdaBoost> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(AdaBoost, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::AdaBoost> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(AdaBoost, TestBasicTrainPredictFunctions ) {
  GRT::ClassifierUnitTestHelper<GRT::AdaBoost> tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(AdaBoost, TrainGaussDataset) {
  GRT::ClassifierUnitTestHelper<GRT::AdaBoost> tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
