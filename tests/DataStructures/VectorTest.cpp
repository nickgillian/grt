
#include <GRT.h>
#include "gtest/gtest.h"

//Unit tests for the GRT Vector class

// Tests the default c'tor.
TEST(Vector, DefaultConstructor) {
  GRT::Vector< int > vec;

  EXPECT_EQ(0, vec.getSize());
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}