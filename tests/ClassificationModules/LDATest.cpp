#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT LDA module

// Tests the default constructor
TEST(LDA, Constructor) {
  
  LDA lda;

  //Check the type matches
  EXPECT_TRUE( lda.getClassifierType() == LDA::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !lda.getTrained() );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
