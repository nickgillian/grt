#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT BAG module

// Tests the default constructor
TEST(BAG, Constructor) {
  
  BAG bag;

  //Check the type matches
  EXPECT_TRUE( bag.getClassifierType() == BAG::getId() );

  //Check the module is not trained
  EXPECT_TRUE( !bag.getTrained() );
}

// Tests the learning algorithm on a basic dataset
TEST(BAG, TrainBasicDataset) {
  
  BAG bag;

  //Check the module is not trained
  EXPECT_TRUE( !bag.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10000;
  const UINT numClasses = 10;
  const UINT numDimensions = 100;
  ClassificationData::generateGaussDataset( "gauss_data.csv", numSamples, numClasses, numDimensions, 10, 1 );
  ClassificationData trainingData;
  EXPECT_TRUE( trainingData.load( "gauss_data.csv" ) );

  ClassificationData testData = trainingData.split( 50 );

  //Add an adaptive naive bayes classifier to the BAG ensemble
  bag.addClassifierToEnsemble( ANBC() );
  
  //Add a MinDist classifier to the BAG ensemble, using two clusters
  MinDist min_dist_two_clusters;
  min_dist_two_clusters.setNumClusters(2);
  bag.addClassifierToEnsemble( min_dist_two_clusters );
  
  //Add a MinDist classifier to the BAG ensemble, using five clusters
  MinDist min_dist_five_clusters;
  min_dist_five_clusters.setNumClusters(5);
  bag.addClassifierToEnsemble( min_dist_five_clusters );

  //Train the classifier
  EXPECT_TRUE( bag.train( trainingData ) );

  EXPECT_TRUE( bag.getTrained() );

  EXPECT_TRUE( bag.print() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( bag.predict( testData[i].getSample() ) );
  }

  EXPECT_TRUE( bag.save( "bag_model.grt" ) );

  bag.clear();
  EXPECT_TRUE( !bag.getTrained() );

  EXPECT_TRUE( bag.load( "bag_model.grt" ) );

  EXPECT_TRUE( bag.getTrained() );

  for(UINT i=0; i<testData.getNumSamples(); i++){
    EXPECT_TRUE( bag.predict( testData[i].getSample() ) );
  }


}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
