#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT SwipeDetector module
/*
// Tests the default constructor
TEST(SwipeDetector, Constructor) {
  
  SwipeDetector sd;

  //Check the type matches
  EXPECT_TRUE( sd.getClassifierType() == SwipeDetector::getId() );

  //Check the module is trained, swipe detector is trained already
  EXPECT_TRUE( sd.getTrained() );
}
*/
int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
