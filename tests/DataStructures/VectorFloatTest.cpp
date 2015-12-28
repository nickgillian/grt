
#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Vector class

// Tests the default c'tor.
TEST(VectorFloat, DefaultConstructor) {
	VectorFloat vec;
	EXPECT_EQ(0, vec.getSize());
}

// Tests the resize c'tor.
TEST(VectorFloat, ResizeConstructor) {
	const UINT size = 100;
	VectorFloat vec( size );
	EXPECT_EQ(size, vec.getSize());
}

// Tests the copy c'tor.
TEST(VectorFloat, CopyConstructor) {
	const UINT size = 100;
	VectorFloat vec1( size );
	EXPECT_EQ(size, vec1.getSize());
	VectorFloat vec2( vec1 );
	EXPECT_EQ(size, vec2.getSize());
}

// Tests the equals operator.
TEST(VectorFloat, EqualsConstructor) {
	const UINT size = 100;
	VectorFloat vec1( size );
	EXPECT_EQ(size, vec1.getSize());
	VectorFloat vec2;
	vec2 = vec1;
	EXPECT_EQ(size, vec2.getSize());
}

// Tests the Vector< float_t > equals operator.
TEST(VectorFloat, VecFloatEqualsConstructor) {
	const UINT size = 100;
	Vector< GRT::float_t > vec1( size );
	EXPECT_EQ(size, vec1.getSize());
	VectorFloat vec2;
	vec2 = vec1;
	EXPECT_EQ(size, vec2.getSize());
}

// Tests the save method.
TEST(VectorFloat, Save) {
	const UINT size = 100;
	VectorFloat vec1( size );
	EXPECT_EQ(size, vec1.getSize());
	for(UINT i=0; i<size; i++){ vec1[i] = i*1.0; std::cout << "value: " << vec1[i] << std::endl; }
	EXPECT_TRUE( vec1.save("vec.csv") );
	
	//Load the data into another vector
	VectorFloat vec2;
	EXPECT_TRUE( vec2.load("vec.csv") );

	//Vector 2 should now be the same size as vector 1
	EXPECT_EQ(vec1.getSize(), vec2.getSize());

	//Check to make sure the values match
	for(UINT i=0; i<size; i++){ EXPECT_EQ(vec1[i], vec2[i]); }
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}