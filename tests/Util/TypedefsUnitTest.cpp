#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Typedefs

// Tests the square function
TEST(Typedefs, Sqr) {
  Float a = 0.0;
  Float b = 1.0;
  Float c = 2.0;
  Float d = -2.0;
  Float e = 1000.0;

  Float expectedA = 0.0 * 0.0;
  Float expectedB = 1.0 * 1.0;
  Float expectedC = 2.0 * 2.0;
  Float expectedD = -2.0 * -2.0;
  Float expectedE = 1000.0 * 1000.0;

  EXPECT_EQ(grt_sqr(a), expectedA);
  EXPECT_EQ(grt_sqr(b), expectedB);
  EXPECT_EQ(grt_sqr(c), expectedC);
  EXPECT_EQ(grt_sqr(d), expectedD);
  EXPECT_EQ(grt_sqr(e), expectedE);
}

// Tests the square root function
TEST(Typedefs, Sqrt) {
  Float a = 0.0;
  Float b = 1.0;
  Float c = 2.0;
  Float d = 5.0;
  Float e = 1000.0;

  Float expectedA = sqrt( a );
  Float expectedB = sqrt( b );
  Float expectedC = sqrt( c );
  Float expectedD = sqrt( d );
  Float expectedE = sqrt( e );

  EXPECT_EQ(grt_sqrt(a), expectedA);
  EXPECT_EQ(grt_sqrt(b), expectedB);
  EXPECT_EQ(grt_sqrt(c), expectedC);
  EXPECT_EQ(grt_sqrt(d), expectedD);
  EXPECT_EQ(grt_sqrt(e), expectedE);
}

// Tests the to string function
TEST(Typedefs, ToStr) {
  Float a = 3.1415;
  int b = 6;

  EXPECT_TRUE(grt_to_str(a) == "3.1415");
  EXPECT_TRUE(grt_to_str(b) == "6");
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
