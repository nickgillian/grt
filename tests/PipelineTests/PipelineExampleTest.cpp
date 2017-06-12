#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for testing the GRT 101 example pipeline.

//Helper function for generating a fake dataset for testing.
bool mockDataset(const std::string filename, const UINT numSamples = 1000, const UINT numClasses = 2, const UINT numDimensions = 10) {
	//Generate a basic dataset
  const Float range = 10;
  const Float sigma = 1;
  ClassificationData data = ClassificationData::generateGaussLinearDataset(numSamples, numClasses, numDimensions, range, sigma);
  return data.save(filename);
}

// Tests the default c'tor.
TEST(GestureRecognitionPipeline, ExamplePipeline) {

	//Mock a fake dataset
	const std::string filename = "basic_dataset.grt";
	EXPECT_TRUE(mockDataset(filename));

 	//Load some training data from a file
  ClassificationData trainingData;
  EXPECT_TRUE(trainingData.load(filename));

  //Print out some stats about the training data
  EXPECT_TRUE(trainingData.printStats());

  //Partition the training data into a training dataset and a test dataset. 80 means that 80%
  //of the data will be used for the training data and 20% will be returned as the test dataset
  ClassificationData testData = trainingData.split(80);
  EXPECT_TRUE(testData.getNumSamples() > 0);

  //Create a new Gesture Recognition Pipeline
  GestureRecognitionPipeline pipeline;

  //Add a Naive Bayes classifier to the pipeline
  pipeline << ANBC();

  //Train the pipeline using the training data
  EXPECT_TRUE(pipeline.train(trainingData));

  //Save the pipeline to a file
  EXPECT_TRUE(pipeline.save("HelloWorldPipeline.grt"));

    //Load the pipeline from a file
  EXPECT_TRUE(pipeline.load("HelloWorldPipeline.grt"));

  //Test the pipeline using the test data
  EXPECT_TRUE(pipeline.test(testData));

  //Print some stats about the testing
  std::cout << "Pipeline Test Accuracy: " << pipeline.getTestAccuracy() << std::endl;

  //Manually project the test dataset through the pipeline
  Float testAccuracy = 0.0;
  for (UINT i=0; i<testData.getNumSamples(); i++) {
    pipeline.predict( testData[i].getSample() );

    if (testData[i].getClassLabel() == pipeline.getPredictedClassLabel()) {
       testAccuracy++;
    }
  }
  testAccuracy = testAccuracy / testData.getNumSamples() * 100.0;
  std::cout << "Manual test accuracy: " << testAccuracy << std::endl;
  //For this basic dataset, the accuracy should be greater than 80%
  EXPECT_TRUE(testAccuracy >= 80);
   
  //Get the vector of class labels from the pipeline
  Vector< UINT > classLabels = pipeline.getClassLabels();

  //Print out the precision
  std::cout << "Precision: ";
  for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
      std::cout << "\t" << pipeline.getTestPrecision( classLabels[k] );
  } std::cout << std::endl;

  //Print out the recall
  std::cout << "Recall: ";
  for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
    std::cout << "\t" << pipeline.getTestRecall( classLabels[k] );
  } std::cout << std::endl;

  //Print out the f-measure
  std::cout << "FMeasure: ";
  for(UINT k=0; k<pipeline.getNumClassesInModel(); k++){
    std::cout << "\t" << pipeline.getTestFMeasure( classLabels[k] );
  } std::cout << std::endl;

  //Print out the confusion matrix
  MatrixFloat confusionMatrix = pipeline.getTestConfusionMatrix();
  std::cout << "ConfusionMatrix: \n";
  for(UINT i=0; i<confusionMatrix.getNumRows(); i++){
    for(UINT j=0; j<confusionMatrix.getNumCols(); j++){
      std::cout << confusionMatrix[i][j] << "\t";
    } std::cout << std::endl;
  }
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
