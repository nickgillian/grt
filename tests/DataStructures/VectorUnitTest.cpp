#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Vector class

// Tests the default c'tor.
TEST(Vector, DefaultConstructor) {
  Vector< int > vec;
  EXPECT_EQ(0, vec.getSize());
}

// Tests the resize c'tor.
TEST(Vector, ResizeConstructor) {
	const UINT size = 100;
	Vector< int > vec(size);
	EXPECT_EQ(size, vec.getSize());
}

// Tests the copy c'tor.
TEST(Vector, CopyConstructor) {
	const UINT size = 100;
	Vector< int > vec1(size);
	EXPECT_EQ(size, vec1.getSize());
	Vector< int > vec2(vec1);
	EXPECT_EQ(vec1.getSize(), vec2.getSize());
}

// Tests the equals operator.
TEST(Vector, EqualsConstructor) {
	const UINT size = 100;
	Vector< int > vec1(size);
	EXPECT_EQ(size, vec1.getSize());
	Vector< int > vec2;
	vec2 = vec1;
	EXPECT_EQ(vec1.getSize(), vec2.getSize());
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
