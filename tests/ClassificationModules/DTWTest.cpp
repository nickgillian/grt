#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT DWT module

// Tests the default constructor
TEST(DTW, Constructor) {
  
  DTW dtw;

  //Check the type matches
  EXPECT_TRUE( dtw.getClassifierType() == DTW::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !dtw.getTrained() );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
