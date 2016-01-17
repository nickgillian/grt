#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Matrix class

// Tests the default c'tor.
TEST(Matrix, DefaultConstructor) {
  Matrix< int > mat;

  EXPECT_EQ(0, mat.getSize());
  EXPECT_EQ(0, mat.getNumRows());
  EXPECT_EQ(0, mat.getNumCols());
}

// Tests the resize c'tor.
TEST(Matrix, ResizeConstructor) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat( numRows, numCols );
	EXPECT_EQ(numRows*numCols, mat.getSize());
  	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
}

// Tests the copy c'tor.
TEST(Matrix, CopyConstructor) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat1( numRows, numCols );
	EXPECT_EQ(numRows, mat1.getNumRows());
  	EXPECT_EQ(numCols, mat1.getNumCols());
	Matrix< int > mat2( mat1 );
	EXPECT_EQ(numRows, mat2.getNumRows());
  	EXPECT_EQ(numCols, mat2.getNumCols());
}

// Tests the equals operator.
TEST(Matrix, EqualsConstructor) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat1( numRows, numCols );
	EXPECT_EQ(numRows, mat1.getNumRows());
  	EXPECT_EQ(numCols, mat1.getNumCols());
	Matrix< int > mat2;
	mat2 = mat1;
	EXPECT_EQ(numRows, mat2.getNumRows());
  	EXPECT_EQ(numCols, mat2.getNumCols());
}

// Tests the Vector c'tor.
TEST(Matrix, VectorConstructor) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Vector< Vector< int > > vec( numRows, Vector< int >( numCols ) );
	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			vec[i][j] = 1;
		}
	}

	Matrix< int > mat( vec );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ(mat[i][j],vec[i][j]);
		}
	}
}

// Tests the [] operator
TEST(Matrix, AccessOperator) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = i*j;
		}
	}
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ(mat[i][j],i*j);
		}
	}
}

// Tests the getRowVector
TEST(Matrix, GetRowVector) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = i*j;
		}
	}
  	for(UINT i=0; i<numRows; i++){
  		Vector< int > rowVector = mat.getRowVector( i );
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ(rowVector[j],i*j);
		}
	}
}

// Tests the getColVector
TEST(Matrix, GetColVector) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = i*j;
		}
	}
	for(UINT j=0; j<numCols; j++){
		Vector< int > colVector = mat.getColVector( j );
  		for(UINT i=0; i<numRows; i++){
			EXPECT_EQ(colVector[i],i*j);
		}
	}
}

// Tests the resize
TEST(Matrix, Resize) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	const UINT newRows = 200;
	const UINT newCols = 100;
	Matrix< int > mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
	EXPECT_TRUE( mat.resize( newRows, newCols ) );
	EXPECT_EQ(newRows, mat.getNumRows());
  	EXPECT_EQ(newCols, mat.getNumCols());
}

// Tests the copy
TEST(Matrix, Copy) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat1( numRows, numCols );
	EXPECT_EQ(numRows, mat1.getNumRows());
  	EXPECT_EQ(numCols, mat1.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat1[i][j] = i*j;
		}
	}
	Matrix< int > mat2;
	EXPECT_TRUE( mat2.copy( mat1 ) );
	EXPECT_EQ(numRows, mat2.getNumRows());
  	EXPECT_EQ(numCols, mat2.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ( mat1[i][j], mat2[i][j] );
		}
	}
}

// Tests the setAllValues
TEST(Matrix, SetAll) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = i*j;
		}
	}
	EXPECT_TRUE( mat.setAll( 0 ) );
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ( mat[i][j], 0 );
		}
	}
}

// Tests the setRowVector
TEST(Matrix, SetRowVector) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = 0;
		}
	}
	Vector< int > vec( numCols );
	EXPECT_TRUE( vec.setAll( 1000 ) );
	EXPECT_TRUE( mat.setRowVector( vec, 0 ) );
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ( mat[i][j], i == 0 ? 1000 : 0 );
		}
	}
}

// Tests the setColVector
TEST(Matrix, SetColVector) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Matrix< int > mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = 0;
		}
	}
	Vector< int > vec( numRows );
	EXPECT_TRUE( vec.setAll( 1000 ) );
	EXPECT_TRUE( mat.setColVector( vec, 0 ) );
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ( mat[i][j], j == 0 ? 1000 : 0 );
		}
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
