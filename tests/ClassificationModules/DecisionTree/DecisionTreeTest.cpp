#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Decision Tree module

// Tests the square function
TEST(Typedefs, TreeConstructor) {
  
  

  EXPECT_EQ(grt_sqr(a), expectedA);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
