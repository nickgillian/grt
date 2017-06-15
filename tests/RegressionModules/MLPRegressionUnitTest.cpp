#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT MLP module

//Main function for testing the MLP for the XOR dataset
bool train_mlp_xor(const bool enableScaling) {

  //Disable the training log messages
  TrainingLog::setLoggingEnabled( false );

  //Create a new MLP module
  MLP mlp;

  //Check the module is not trained
  EXPECT_TRUE( !mlp.getTrained() );

  //Create a dataset for XOR
  RegressionData data;

  //The XOR problem has two inputs and 1 output
  data.setInputAndTargetDimensions( 2, 1 );

  //Add the training/test data
  VectorFloat x(2);
  VectorFloat y(1);

  if( !enableScaling ){
    x[0] = 0; x[1] = 0; y[0] = 0; data.addSample( x, y );
    x[0] = 1; x[1] = 1; y[0] = 0; data.addSample( x, y );
    x[0] = 0; x[1] = 1; y[0] = 1; data.addSample( x, y );
    x[0] = 1; x[1] = 0; y[0] = 1; data.addSample( x, y );
  }else{
    x[0] = -1000; x[1] = -1000; y[0] = -100; data.addSample( x, y );
    x[0] = 1000; x[1] = 1000; y[0] = -100; data.addSample( x, y );
    x[0] = -1000; x[1] = 1; y[0] = 100; data.addSample( x, y );
    x[0] = 1000; x[1] = -1000; y[0] = 100; data.addSample( x, y );
  }

  const unsigned int numInputNeurons = data.getNumInputDimensions();
  const unsigned int numHiddenNeurons = 2;
  const unsigned int numOutputNeurons = data.getNumTargetDimensions();
  
  //Initialize the MLP
  EXPECT_TRUE( mlp.init(numInputNeurons, numHiddenNeurons, numOutputNeurons, Neuron::LINEAR, Neuron::TANH, Neuron::LINEAR ) );

  //Check the module is not trained
  EXPECT_TRUE( !mlp.getTrained() );
  
  //Set the training settings
  EXPECT_TRUE( mlp.setMaxNumEpochs( 1000 ) ); //This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
  EXPECT_TRUE( mlp.setMinChange( 1.0e-10 ) ); //This sets the minimum change allowed in training error between any two epochs
  EXPECT_TRUE( mlp.setLearningRate( 0.01 ) ); //This sets the rate at which the learning algorithm updates the weights of the neural network
  EXPECT_TRUE( mlp.setMomentum( 0.5 ) ); //This sets the amount of smoothing between weight updates
  EXPECT_TRUE( mlp.setNumRestarts( 1 ) ); //This sets the number of times the MLP will be trained, we manually do the re-try training below as needed
  EXPECT_TRUE( mlp.setUseValidationSet( false ) ); //Don't use a validation set for this case (as we only have 4 samples)
  EXPECT_TRUE( mlp.setRandomiseTrainingOrder( false ) ); //We don't need to randomize the training data for this XOR case
  EXPECT_TRUE( mlp.enableScaling( enableScaling ) ); //Turn training on/off

  //Check the module is not trained
  EXPECT_TRUE( !mlp.getTrained() );

  //Train a model, for MLP we may need to train it multiple times due to the
  //random nature of the learning algorithm
  const Float passRmsError = 0.25;
  const UINT maxNumTrys = 5000;
  UINT i=0;
  do{
    EXPECT_TRUE( mlp.train( data ) );

    EXPECT_TRUE( mlp.getTrained() );

    EXPECT_TRUE( mlp.print() );
  
    EXPECT_TRUE( mlp.getRMSValidationError() == 0.0 ); //This should be zero, as there is no validation set
    if( mlp.getRMSTrainingError() <= passRmsError ) break;
  }while( ++i <= maxNumTrys );
  
  EXPECT_TRUE( mlp.getRMSTrainingError() <= passRmsError );

  //Project the data through the trained model, we are not looking for accuracy
  //here, just that the prediction can be run successfully
  for(UINT i=0; i<data.getNumSamples(); i++){
    EXPECT_TRUE( mlp.predict( data[i].getInputVector() ) );
  }

  EXPECT_TRUE( mlp.save( "mlp_model.grt" ) );

  EXPECT_TRUE( mlp.clear() );

  EXPECT_TRUE( !mlp.getTrained() );

  EXPECT_TRUE( mlp.load( "mlp_model.grt" ) );

  EXPECT_TRUE( mlp.getTrained() );

  //After loading the model, project the data back through the trained model, we are not looking for accuracy
  //here, just that the prediction can be run successfully
  for(UINT i=0; i<data.getNumSamples(); i++){
    EXPECT_TRUE( mlp.predict( data[i].getInputVector() ) );
  }
  
  //If we get this far then the test must be have passed
  return true;
}

// Tests the learning algorithm on the XOR problem
TEST(MLP, XOR) {
  EXPECT_TRUE( train_mlp_xor(false) );
}

// Tests the learning algorithm on the XOR problem with data that needs to be scaled
TEST(MLP, XORScaled) {
  EXPECT_TRUE( train_mlp_xor(true) );
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
