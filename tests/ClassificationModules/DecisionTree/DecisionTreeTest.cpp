#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Decision Tree module

// Tests the square function
TEST(Typedefs, TreeConstructor) {
  
  DecisionTree tree;

  //Check the type matches
  EXPECT_TRUE( tree.getClassifierType() == DecisionTree::getType() );

  //Check the module is not trained
  EXPECT_TRUE( !tree.getTrained() );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
