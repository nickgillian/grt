#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Classifier base class

// Tests the square function
TEST(Classifier, RegisteredClassifiers) {
  
  //Get a list of the registered classifiers
  Vector< std::string > registeredClassifiers = Classifier::getRegisteredClassifiers();

  //The classifier list should be great than zero
  EXPECT_TRUE( registeredClassifiers.getSize() > 0 );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
