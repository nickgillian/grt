#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT Logistic Regression module

// Tests the learning algorithm on a basic regression task
TEST(LogisticRegression, BasicRegressionTest) {

  const bool scaleData = true;
  const Float learningRate = 0.01;
  const Float minChange = 1.0e-5;
  const UINT batchSize = 1;
  const UINT maxNumEpochs = 1000;

  LogisticRegression regression(scaleData,learningRate,minChange,batchSize,maxNumEpochs);

  EXPECT_TRUE( regression.getLearningRate() == learningRate );
  EXPECT_TRUE( regression.getBatchSize() == batchSize );

  const std::string modelFilename = regression.getId() + "_model.grt";

  //Disable the training logging
  EXPECT_TRUE( regression.setTrainingLoggingEnabled( false ) );
  EXPECT_TRUE( !regression.getTrainingLoggingEnabled() );

  //Check the module is not trained
  EXPECT_TRUE( !regression.getTrained() );

  //Generate a basic dataset
  const UINT numSamples = 10 * 1000;
  const UINT numDimensions = 10;
  const Float errorTolerance = 0.25;

  //Generate a basic training dataset
  RegressionData trainingData;
  EXPECT_TRUE( trainingData.setInputAndTargetDimensions( numDimensions, 1 ) );
  {
    Random random;
    VectorFloat inputVector(numDimensions);
    VectorFloat targetVector(1);
    for(UINT i=0; i<numSamples; i++){
    
      Float randomTarget = random.getUniform(0.0,1.0) > 0.5 ? 1.0 : 0.0;
      for(UINT j=0; j<numDimensions; j++){
        inputVector[j] = random.getGauss( randomTarget, 0.25 );
      }
      targetVector[0] = randomTarget;

      EXPECT_TRUE( trainingData.addSample( inputVector, targetVector ) );
    }
  }

  RegressionData testData = trainingData.split( 50 );

  //Turn off the validation set for training
  EXPECT_TRUE( regression.setUseValidationSet( false ) );
  EXPECT_TRUE( !regression.getUseValidationSet() ); //This should now be false

  //Train the classifier
  EXPECT_TRUE( regression.train( trainingData ) );

  EXPECT_TRUE( regression.getTrained() );
  EXPECT_TRUE( regression.getNumInputDimensions() == numDimensions );
  EXPECT_TRUE( regression.getNumOutputDimensions() == 1 );
  EXPECT_TRUE( regression.getRMSTrainingError() <= errorTolerance ); //This should equal zero, as we only get accuracy for classifiers
  EXPECT_TRUE( regression.getRMSValidationError() == 0.0 ); //Validation is off, so the validation accuracy should be zero

  EXPECT_TRUE( regression.print() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  if( regression.getTrained() ){
    for(UINT i=0; i<testData.getNumSamples(); i++){
      EXPECT_TRUE( regression.predict( testData[i].getInputVector() ) );
    }
  }
  
  //Save the model to a file
  EXPECT_TRUE( regression.save( modelFilename ) );

  //Clear the model
  EXPECT_TRUE( regression.clear() );
  EXPECT_TRUE( !regression.getTrained() );

  //Load it from a file again
  EXPECT_TRUE( regression.load( modelFilename ) );
  EXPECT_TRUE( regression.getTrained() );

  //Test the prediction, we don't care about accuracy here, just that the prediction is successfull
  if( regression.getTrained() ){
    for(UINT i=0; i<testData.getNumSamples(); i++){
      EXPECT_TRUE( regression.predict( testData[i].getInputVector() ) );
    }
  }
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
