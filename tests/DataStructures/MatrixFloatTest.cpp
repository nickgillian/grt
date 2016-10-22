#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT MatrixFloatFloat class

// Tests the default c'tor.
TEST(MatrixFloat, DefaultConstructor) {
  MatrixFloat mat;

  EXPECT_EQ(0, mat.getSize());
  EXPECT_EQ(0, mat.getNumRows());
  EXPECT_EQ(0, mat.getNumCols());
}

// Tests the resize c'tor.
TEST(MatrixFloat, ResizeConstructor) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat( numRows, numCols );
	EXPECT_EQ(numRows*numCols, mat.getSize());
  	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
}

// Tests the copy c'tor.
TEST(MatrixFloat, CopyConstructor) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat1( numRows, numCols );
	EXPECT_EQ(numRows, mat1.getNumRows());
  	EXPECT_EQ(numCols, mat1.getNumCols());
	MatrixFloat mat2( mat1 );
	EXPECT_EQ(numRows, mat2.getNumRows());
  	EXPECT_EQ(numCols, mat2.getNumCols());
}

// Tests the equals operator.
TEST(MatrixFloat, EqualsConstructor) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat1( numRows, numCols );
	EXPECT_EQ(numRows, mat1.getNumRows());
  	EXPECT_EQ(numCols, mat1.getNumCols());
	MatrixFloat mat2;
	mat2 = mat1;
	EXPECT_EQ(numRows, mat2.getNumRows());
  	EXPECT_EQ(numCols, mat2.getNumCols());
}

// Tests the Vector c'tor.
TEST(MatrixFloat, VectorConstructor) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	Vector< VectorFloat > vec( numRows, VectorFloat( numCols ) );
	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			vec[i][j] = 1;
		}
	}

	MatrixFloat mat( vec );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ(mat[i][j],vec[i][j]);
		}
	}
}

// Tests the [] operator
TEST(MatrixFloat, AccessOperator) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat( numRows, numCols );
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
TEST(MatrixFloat, GetRow) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = i*j;
		}
	}
  	for(UINT i=0; i<numRows; i++){
  		VectorFloat rowVector = mat.getRow( i );
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ(rowVector[j],i*j);
		}
	}
}

// Tests the getColVector
TEST(MatrixFloat, GetCol) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = i*j;
		}
	}
	for(UINT j=0; j<numCols; j++){
		VectorFloat colVector = mat.getCol( j );
  		for(UINT i=0; i<numRows; i++){
			EXPECT_EQ(colVector[i],i*j);
		}
	}
}

// Tests the resize
TEST(MatrixFloat, Resize) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	const UINT newRows = 200;
	const UINT newCols = 100;
	MatrixFloat mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
	EXPECT_TRUE( mat.resize( newRows, newCols ) );
	EXPECT_EQ(newRows, mat.getNumRows());
  	EXPECT_EQ(newCols, mat.getNumCols());
}

// Tests the copy
TEST(MatrixFloat, Copy) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat1( numRows, numCols );
	EXPECT_EQ(numRows, mat1.getNumRows());
  	EXPECT_EQ(numCols, mat1.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat1[i][j] = i*j;
		}
	}
	MatrixFloat mat2;
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
TEST(MatrixFloat, SetAll) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat( numRows, numCols );
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
TEST(MatrixFloat, SetRowVector) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = 0;
		}
	}
	VectorFloat vec( numCols );
	EXPECT_TRUE( vec.setAll( 1000 ) );
	EXPECT_TRUE( mat.setRowVector( vec, 0 ) );
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ( mat[i][j], i == 0 ? 1000 : 0 );
		}
	}
}

// Tests the setColVector
TEST(MatrixFloat, SetColVector) {
	const UINT numRows = 100;
	const UINT numCols = 50;
	MatrixFloat mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = 0;
		}
	}
	VectorFloat vec( numRows );
	EXPECT_TRUE( vec.setAll( 1000 ) );
	EXPECT_TRUE( mat.setColVector( vec, 0 ) );
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			EXPECT_EQ( mat[i][j], j == 0 ? 1000 : 0 );
		}
	}
}

// Tests the CSV save and load functions
TEST(MatrixFloat, SaveLoad) {
	const UINT numRows = 100;
	const UINT numCols = 1000;
	MatrixFloat mat( numRows, numCols );
	EXPECT_EQ(numRows, mat.getNumRows());
  	EXPECT_EQ(numCols, mat.getNumCols());
  	for(UINT i=0; i<numRows; i++){
		for(UINT j=0; j<numCols; j++){
			mat[i][j] = i*j;
		}
	}

	//Save the data
	EXPECT_TRUE( mat.save( "matrix_float.csv") );

	EXPECT_TRUE( mat.clear() );

	//Load the data
	{
		MatrixFloat mat2;
		EXPECT_TRUE( mat2.load( "matrix_float.csv") );
		EXPECT_EQ(numRows, mat2.getNumRows());
  		EXPECT_EQ(numCols, mat2.getNumCols());
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
