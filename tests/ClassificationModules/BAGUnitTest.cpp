#include "ClassifierUnitTestHelper.h"

//Unit tests for the GRT BAG module

//Overwrite the generic test functions for BAG
GRT_BEGIN_NAMESPACE
class BAGUnitTestHelper : public ClassifierUnitTestHelper<BAG> {
public:

  virtual bool testBasicTrainPredictFunctions(){

    //Note, the goal here is NOT to test the learning algorithm (i.e., accuracy of model), but instead to test the basic train/predict/getters/setters
  
    //Create a default instance
    CLASSIFIER classifier;

    const std::string modelFilename = classifier.getId() + "_model.grt";

    //Disable the training logging
    EXPECT_TRUE( classifier.setTrainingLoggingEnabled( false ) );
    EXPECT_TRUE( !classifier.getTrainingLoggingEnabled() );

    //Check the module is not trained
    EXPECT_TRUE( !classifier.getTrained() );

    //Generate a basic dataset
    const UINT numSamples = 5000;
    const UINT numClasses = 5;
    const UINT numDimensions = 3;
    ClassificationData trainingData = ClassificationData::generateGaussDataset( numSamples, numClasses, numDimensions, 10, 1 );

    ClassificationData testData = trainingData.split( 50, true );

    //Add an adaptive naive bayes classifier to the ensemble
    EXPECT_TRUE( classifier.addClassifierToEnsemble( ANBC() ) );
    
    //Add a MinDist classifier to the BAG ensemble, using two clusters
    {
      MinDist min_dist_two_clusters;
      min_dist_two_clusters.setNumClusters(2);
      EXPECT_TRUE( classifier.addClassifierToEnsemble( min_dist_two_clusters ) );
    }
    
    //Add a MinDist classifier to the BAG ensemble, using five clusters
    {
      MinDist min_dist_five_clusters;
      min_dist_five_clusters.setNumClusters(5);
      EXPECT_TRUE( classifier.addClassifierToEnsemble( min_dist_five_clusters ) );
    }

    //Train the classifier
    EXPECT_TRUE( classifier.train( trainingData ) );
    EXPECT_TRUE( classifier.getTrained() );
    EXPECT_TRUE( classifier.getNumInputDimensions() == numDimensions );
    EXPECT_TRUE( classifier.getNumOutputDimensions() == numClasses );
    EXPECT_TRUE( classifier.getNumClasses() == numClasses );

    EXPECT_TRUE( classifier.print() );

    //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
    if( classifier.getTrained() ){
      for(UINT i=0; i<testData.getNumSamples(); i++){
        EXPECT_TRUE( classifier.predict( testData[i].getSample() ) );
      }
    }

    EXPECT_TRUE( classifier.save( modelFilename ) );

    EXPECT_TRUE( classifier.clear() );
    EXPECT_TRUE( !classifier.getTrained() );

    EXPECT_TRUE( classifier.load( modelFilename ) );
    EXPECT_TRUE( classifier.getTrained() );

    //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
    if( classifier.getTrained() ){
      for(UINT i=0; i<testData.getNumSamples(); i++){
        EXPECT_TRUE( classifier.predict( testData[i].getSample() ) );
      }
    }

    return true;
  }

  virtual bool testTrainGaussLinearDataset(){

    //Note, the goal here IS to test the learning algorithm (i.e., accuracy of model), the training data below is selected in a 
    //way that is biased so the algorithm should be able to train a model....if it can't then this indicates there is a bug
    
    //Create a default instance
    CLASSIFIER classifier;

    const std::string modelFilename = classifier.getId() + "_model.grt";

    //Disable the training logging
    EXPECT_TRUE( classifier.setTrainingLoggingEnabled( false ) );
    EXPECT_TRUE( !classifier.getTrainingLoggingEnabled() );

    //Check the module is not trained
    EXPECT_TRUE( !classifier.getTrained() );

    //Generate a basic dataset
    const UINT numSamples = 5000;
    const UINT numClasses = 5;
    const UINT numDimensions = 3;
    ClassificationData trainingData = ClassificationData::generateGaussLinearDataset( numSamples, numClasses, numDimensions, 10, 1 );

    ClassificationData testData = trainingData.split( 50, true );

    //Add an adaptive naive bayes classifier to the BAG ensemble
    EXPECT_TRUE( classifier.addClassifierToEnsemble( ANBC() ) );
    
    //Add a MinDist classifier to the BAG ensemble, using two clusters
    {
      MinDist min_dist_two_clusters;
      min_dist_two_clusters.setNumClusters(2);
      EXPECT_TRUE( classifier.addClassifierToEnsemble( min_dist_two_clusters ) );
    }
    
    //Add a MinDist classifier to the BAG ensemble, using five clusters
    {
      MinDist min_dist_five_clusters;
      min_dist_five_clusters.setNumClusters(5);
      EXPECT_TRUE( classifier.addClassifierToEnsemble( min_dist_five_clusters ) );
    }
    
    //Turn off the validation set for training
    EXPECT_TRUE( classifier.setUseValidationSet( false ) );
    EXPECT_TRUE( !classifier.getUseValidationSet() ); //This should now be false

    //Train the classifier
    EXPECT_TRUE( classifier.train( trainingData ) );

    EXPECT_TRUE( classifier.getTrained() );
    EXPECT_TRUE( classifier.getNumInputDimensions() == numDimensions );
    EXPECT_TRUE( classifier.getNumOutputDimensions() == numClasses );
    EXPECT_TRUE( classifier.getNumClasses() == numClasses );
    EXPECT_TRUE( classifier.getTrainingSetAccuracy() >= 75.0 ); //On this basic dataset we expect to get at least 75% accuracy
    EXPECT_TRUE( classifier.getValidationSetAccuracy() == 0.0 ); //Validation is off, so the validation accuracy should be zero

    EXPECT_TRUE( classifier.print() );

    //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
    if( classifier.getTrained() ){
      for(UINT i=0; i<testData.getNumSamples(); i++){
        EXPECT_TRUE( classifier.predict( testData[i].getSample() ) );
      }
    }

    //Save the model to a file
    EXPECT_TRUE( classifier.save( modelFilename ) );

    //Clear the model
    EXPECT_TRUE( classifier.clear() );
    EXPECT_TRUE( !classifier.getTrained() );

    //Load it from a file again
    EXPECT_TRUE( classifier.load( modelFilename ) );
    EXPECT_TRUE( classifier.getTrained() );

    //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
    if( classifier.getTrained() ){
      for(UINT i=0; i<testData.getNumSamples(); i++){
        EXPECT_TRUE( classifier.predict( testData[i].getSample() ) );
      }
    }

    //Test that the model can be copied to another instance
    CLASSIFIER classifier2( classifier );

    EXPECT_TRUE( classifier2.getTrained() );

    //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
    if( classifier.getTrained() ){
      for(UINT i=0; i<testData.getNumSamples(); i++){
        EXPECT_TRUE( classifier2.predict( testData[i].getSample() ) );
      }
    }

    return true;
  }

};
GRT_END_NAMESPACE

// Tests the default constructor
TEST(BAG, TestDefaultConstructor) {
  GRT::BAGUnitTestHelper tester;
  EXPECT_TRUE( tester.testDefaultConstructor() );
}

// Tests the copy constructor
TEST(BAG, TestCopyConstructor) {
  GRT::BAGUnitTestHelper tester;
  EXPECT_TRUE( tester.testCopyConstructor() );
}

// Tests the main train/prediction functions using a basic dataset
TEST(BAG, TestBasicTrainPredictFunctions ) {
  GRT::BAGUnitTestHelper tester;
  EXPECT_TRUE( tester.testBasicTrainPredictFunctions() );
}

// Tests the learning algorithm on a basic dataset
TEST(BAG, TrainGaussDataset) {
  GRT::BAGUnitTestHelper tester;
  EXPECT_TRUE( tester.testTrainGaussLinearDataset() );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
