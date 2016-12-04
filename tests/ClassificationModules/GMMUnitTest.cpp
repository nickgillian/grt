#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT GMM module

// Tests the default constructor
TEST(GMM, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::GMM> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(GMM, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::GMM> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(GMM, TestBasicTrainPredictFunctions ) {
  GRT::ClassifierUnitTestHelper<GRT::GMM> tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(GMM, TrainGaussDataset) {
  GRT::ClassifierUnitTestHelper<GRT::GMM> tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
