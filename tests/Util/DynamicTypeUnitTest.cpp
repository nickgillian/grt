#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Typedefs

// Tests the int type
TEST(DynamicType, IntTest) {
  DynamicType type;
  int value = 1;
  EXPECT_TRUE( type.set( value ) );
  EXPECT_EQ( type.get< int >(), value );
}

// Tests the uint type
TEST(DynamicType, UIntTest) {
  DynamicType type;
  UINT value = 5000;
  EXPECT_TRUE( type.set( value ) );
  EXPECT_EQ( type.get< UINT >(), value );
}

// Tests the float type
TEST(DynamicType, FloatTest) {
  DynamicType type;
  float value = 3.14;
  EXPECT_TRUE( type.set( value ) );
  EXPECT_EQ( type.get< float >(), value );
}

// Tests the VectorFloat type
TEST(DynamicType, VectorFloatTest) {
  DynamicType type;
  VectorFloat a(3);
  a[0] = 1.1; a[1] = 1.2; a[2] = 1.3;
  EXPECT_TRUE( type.set( a ) );
  VectorFloat b = type.get< VectorFloat >();
  EXPECT_EQ( a.getSize(), b.getSize() );
  for(unsigned int i=0; i<a.getSize(); i++){
    EXPECT_EQ( a[i], b[i] );
  }
}

// Tests the MatrixFloat type
TEST(DynamicType, MatrixFloatTest) {
  DynamicType type;
  MatrixFloat a(3,1);
  a[0][0] = 1.1; a[1][0] = 1.2; a[2][0] = 1.3;
  EXPECT_TRUE( type.set( a ) );
  MatrixFloat b = type.get< MatrixFloat >();
  EXPECT_EQ( a.getSize(), b.getSize() );
  EXPECT_EQ( a.getNumRows(), b.getNumRows() );
  EXPECT_EQ( a.getNumCols(), b.getNumCols() );
  for(unsigned int i=0; i<a.getNumRows(); i++){
    for(unsigned int j=0; j<a.getNumCols(); j++){
      EXPECT_EQ( a[i][j], b[i][j] );
    }
  }
}

// Tests the int ref update type
TEST(DynamicType, IntRefTest) {
  DynamicType type;
  int value = 1;
  EXPECT_TRUE( type.set( value ) );
  EXPECT_EQ( type.get< int >(), value );
  value += 1;
  EXPECT_FALSE( type.get< int >() == value );
  int &v = type.get< int >();
  v++;
  EXPECT_EQ( type.get< int >(), value );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
