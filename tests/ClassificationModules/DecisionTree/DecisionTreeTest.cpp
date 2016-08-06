#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Decision Tree module

// Tests the square function
TEST(Typedefs, TreeConstructor) {
  
  DecisionTree tree;

  EXPECT_TRUE( tree.getClassifierType() == DecisionTree::getClassifierType() );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
