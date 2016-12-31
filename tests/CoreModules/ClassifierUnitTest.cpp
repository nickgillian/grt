#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Classifier base class

// Tests the RegisteredClassifiers function
TEST(Classifier, RegisteredClassifiers) {
  
  //Get a list of the registered classifiers
  Vector< std::string > registeredClassifiers = Classifier::getRegisteredClassifiers();

  //The classifier list should be greater than zero
  EXPECT_TRUE( registeredClassifiers.getSize() > 0 );
}

// Tests the Create function
TEST(Classifier, Create) {
  
  Classifier *knn = Classifier::create( KNN::getId() );

  EXPECT_TRUE( knn != NULL );

  //The classifier ID should match KNN
  EXPECT_TRUE( knn->getId() == KNN::getId() );
}

// Tests the Reset function
TEST(Classifier, Reset) {
  Classifier classifier;
  EXPECT_TRUE( classifier.reset() );
}

// Tests the Clear function
TEST(Classifier, Clear) {
  Classifier classifier;
  EXPECT_TRUE( classifier.clear() );
}

// Tests the EnableNullRejection functions
TEST(Classifier, EnableNullRejection) {
  Classifier classifier;
  {
  	const bool nullRejectionState = true;
  	EXPECT_TRUE( classifier.enableNullRejection(nullRejectionState) );
  	EXPECT_TRUE( classifier.getNullRejectionEnabled() == nullRejectionState );
  }
  {
  	const bool nullRejectionState = false;
  	EXPECT_TRUE( classifier.enableNullRejection(nullRejectionState) );
  	EXPECT_TRUE( classifier.getNullRejectionEnabled() == nullRejectionState );
  }
}

// Tests the GetSetullRejectionCoeff functions
TEST(Classifier, GetSetullRejectionCoeff) {
  Classifier classifier;
  {
  	const Float nullRejectionCoeff = 2.0;
  	EXPECT_TRUE( classifier.setNullRejectionCoeff(nullRejectionCoeff) );
  	EXPECT_TRUE( classifier.getNullRejectionCoeff() == nullRejectionCoeff );
  }
  {
  	const Float nullRejectionCoeff = 5.0;
  	EXPECT_TRUE( classifier.setNullRejectionCoeff(nullRejectionCoeff) );
  	EXPECT_TRUE( classifier.getNullRejectionCoeff() == nullRejectionCoeff );
  	EXPECT_FALSE( classifier.setNullRejectionCoeff(0.0) ); //This should fail, as the null rejection coeff should be greater than zero
  	EXPECT_FALSE( classifier.setNullRejectionCoeff(-2.0) ); //This should fail, as the null rejection coeff should be greater than zero
  	EXPECT_TRUE( classifier.getNullRejectionCoeff() == nullRejectionCoeff );
  }
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
