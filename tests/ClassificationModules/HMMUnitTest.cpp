#include "ClassifierUnitTestHelper.h"

// Tests the default constructor
TEST(HMM, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::HMM> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(HMM, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::HMM> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

//TODO: Need to implement temporal train unit tests

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}