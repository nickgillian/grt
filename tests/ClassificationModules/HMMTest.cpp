#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT HMM module

// Tests the default constructor
TEST(HMM, Constructor) {
  
  HMM hmm;

  //Check the type matches
  EXPECT_TRUE( hmm.getClassifierType() == HMM::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !hmm.getTrained() );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
