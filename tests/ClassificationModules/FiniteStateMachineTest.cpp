#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT FiniteStateMachine module
/*
// Tests the default constructor
TEST(FiniteStateMachine, Constructor) {
  
  FiniteStateMachine fsm;

  //Check the type matches
  EXPECT_TRUE( fsm.getClassifierType() == FiniteStateMachine::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !fsm.getTrained() );
}
*/
int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
