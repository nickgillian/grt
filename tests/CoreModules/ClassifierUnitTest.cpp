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
  EXPECT_TRUE( knn->getClassifierType() == KNN::getId() );
}

// Tests the Create function
TEST(Classifier, CreateFromInstance) {
  
  Classifier *knn = Classifier::create( KNN::getId() );

  EXPECT_TRUE( knn != NULL );

  //The classifier ID should match KNN
  EXPECT_TRUE( knn->getId() == KNN::getId() );
  EXPECT_TRUE( knn->getClassifierType() == KNN::getId() );

  Classifier *knn2 = knn->create();

  //The classifier ID should match KNN
  EXPECT_TRUE( knn2->getId() == KNN::getId() );
  EXPECT_TRUE( knn2->getClassifierType() == KNN::getId() );
}

// Tests the GetClassifierType function
TEST(Classifier, GetClassifierType) {
  Classifier classifier;
  //The classifier type should be empty, as this is the base class
  EXPECT_TRUE( classifier.getClassifierType() == "" );
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

// Tests the GetPhase function
TEST(Classifier, GetPhase) {
  Classifier classifier;
  EXPECT_TRUE( classifier.getPhase() == 0.0 ); //The default phase should be zero
}

// Tests the GetBestDistance function
TEST(Classifier, GetBestDistance) {
  Classifier classifier;
  EXPECT_TRUE( classifier.getBestDistance() == 0.0 ); //The default best distance should be zero
}

// Tests the GetTrainingSetAccuracy function
TEST(Classifier, GetTrainingSetAccuracy) {
  Classifier classifier;
  EXPECT_TRUE( classifier.getTrainingSetAccuracy() == 0.0 ); //The default training set accuracy should be zero
}

// Tests the GetNumClasses function
TEST(Classifier, GetNumClasses) {
  Classifier classifier;
  EXPECT_TRUE( classifier.getNumClasses() == 0 ); //The default number of classes should be zero
}

// Tests the GetClassLabelIndexValue function
TEST(Classifier, GetClassLabelIndexValue) {
  Classifier classifier;
  EXPECT_TRUE( classifier.getClassLabelIndexValue(1) == 0 ); //There are no class labels yet, so the default value (0) should be returned
  EXPECT_TRUE( classifier.getClassLabelIndexValue(-1) == 0 ); //There are no class labels yet, so the default value (0) should be returned
  EXPECT_TRUE( classifier.getClassLabelIndexValue(5) == 0 ); //There are no class labels yet, so the default value (0) should be returned
}

// Tests the GetPredictedClassLabel function
TEST(Classifier, GetPredictedClassLabel) {
  Classifier classifier;
  EXPECT_TRUE( classifier.getPredictedClassLabel() == 0 ); //The default predicted class label should be zero
}

// Tests the GetClassLikelihoods function
TEST(Classifier, GetClassLikelihoods) {
  Classifier classifier;
  VectorFloat classLikelihoods = classifier.getClassLikelihoods();
  EXPECT_TRUE( classLikelihoods.getSize() == 0 ); //The default size should be zero
}

// Tests the GetClassLikelihoods function
TEST(Classifier, GetClassDistances) {
  Classifier classifier;
  VectorFloat classDistances = classifier.getClassDistances();
  EXPECT_TRUE( classDistances.getSize() == 0 ); //The default size should be zero
}

// Tests the GetNullRejectionThresholds function
TEST(Classifier, GetNullRejectionThresholds) {
  Classifier classifier;
  VectorFloat thresholds = classifier.getNullRejectionThresholds();
  EXPECT_TRUE( thresholds.getSize() == 0 ); //The default size should be zero
}

// Tests the GetClassLabels function
TEST(Classifier, GetClassLabels) {
  Classifier classifier;
  Vector<UINT> labels = classifier.getClassLabels();
  EXPECT_TRUE( labels.getSize() == 0 ); //The default size should be zero
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
