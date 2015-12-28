
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
	EXPECT_EQ(vec1.getSize(), vec2.getSize());
}

// Tests the equals operator.
TEST(VectorFloat, EqualsConstructor) {
	const UINT size = 100;
	VectorFloat vec1( size );
	EXPECT_EQ(size, vec1.getSize());
	VectorFloat vec2;
	vec2 = vec1;
	EXPECT_EQ(vec1.getSize(), vec2.getSize());
}

// Tests the Vector< float_t > equals operator.
TEST(VectorFloat, VecFloatEqualsConstructor) {
	const UINT size = 100;
	Vector< GRT::float_t > vec1( size );
	EXPECT_EQ(size, vec1.getSize());
	VectorFloat vec2;
	vec2 = vec1;
	EXPECT_EQ(vec1.getSize(), vec2.getSize());
}

// Tests the save and load methods.
TEST(VectorFloat, SaveLoad) {
	const UINT size = 100;
	VectorFloat vec1( size );
	EXPECT_EQ(size, vec1.getSize());
	for(UINT i=0; i<size; i++){ vec1[i] = i*1.0; }

	//Save the vector to a CSV file
	EXPECT_TRUE( vec1.save("vec.csv") );
	
	//Load the data from the file into another vector
	VectorFloat vec2;
	EXPECT_TRUE( vec2.load("vec.csv") );

	//Vector 2 should now be the same size as vector 1
	EXPECT_EQ(vec1.getSize(), vec2.getSize());

	//Check to make sure the values match
	for(UINT i=0; i<size; i++){ EXPECT_EQ(vec1[i], vec2[i]); }
}

// Tests the scale function
TEST(VectorFloat, Scale) {
	const UINT size = 100;
	VectorFloat vec( size );
	EXPECT_EQ(size, vec.getSize());
	for(UINT i=0; i<size; i++){ vec[i] = i*1.0; }

	//Scale the contents in the vector
	GRT::float_t minTarget = -1.0f;
	GRT::float_t maxTarget = 1.0f;
	EXPECT_TRUE( vec.scale(minTarget,maxTarget) );

	EXPECT_EQ( vec.getMinValue(), minTarget );
	EXPECT_EQ( vec.getMaxValue(), maxTarget );
}

// Tests the min-max scale function
TEST(VectorFloat, MinMaxScale) {
	const UINT size = 100;
	VectorFloat vec( size );
	EXPECT_EQ(size, vec.getSize());
	for(UINT i=0; i<size; i++){ vec[i] = i*1.0; }

	//Scale the contents in the vector
	GRT::float_t minSource = 10; //Deliberately set the min source to 10, even though the min value should be 0
	GRT::float_t maxSource = 90; //Deliberately set the max source to 90, even though the max value should be 100
	GRT::float_t minTarget = -1.0f;
	GRT::float_t maxTarget = 1.0f;
	EXPECT_TRUE( vec.scale(minSource, maxSource, minTarget, maxTarget, true) );
	EXPECT_EQ( vec.getMinValue(), minTarget );
	EXPECT_EQ( vec.getMaxValue(), maxTarget );

	//Reset the data
	for(UINT i=0; i<size; i++){ vec[i] = i*1.0; }
	EXPECT_TRUE( vec.scale(minSource, maxSource, minTarget, maxTarget, false) ); //Re-run the scaling with constraining off
	EXPECT_TRUE( fabs( vec.getMinValue() - minTarget ) > 0.01 ); //The min value should no longer match the min target
	EXPECT_TRUE( fabs( vec.getMaxValue() - maxTarget ) > 0.01 ); //The max value should no longer match the max target
}

// Tests the getMinValue
TEST(VectorFloat, GetMin) {
	const UINT size = 100;
	VectorFloat vec( size );
	EXPECT_EQ(size, vec.getSize());
	for(UINT i=0; i<size; i++){ vec[i] = i*1.0; }
	EXPECT_EQ( vec.getMinValue(), 0.0f );
}

// Tests the getMaxValue
TEST(VectorFloat, GetMax) {
	const UINT size = 100;
	VectorFloat vec( size );
	EXPECT_EQ(size, vec.getSize());
	for(UINT i=0; i<size; i++){ vec[i] = i*1.0; }
	EXPECT_EQ( vec.getMaxValue(), 99.0f );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}