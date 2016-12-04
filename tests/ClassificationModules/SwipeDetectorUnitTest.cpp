#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT SwipeDetector module

// Tests the default constructor
TEST(SwipeDetector, TestDefaultConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::SwipeDetector> tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(SwipeDetector, TestCopyConstructor) {
  GRT::ClassifierUnitTestHelper<GRT::SwipeDetector> tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

//TODO: Need to add train unit tests for swipe detector

int main(int argc, char **argv) {
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}