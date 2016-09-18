#include <GRT.h>
#include "gtest/gtest.h"
using namespace GRT;

//Unit tests for the GRT MLP module

// Tests the learning algorithm on the XOR problem
TEST(MLP, XOR) {

  //Disable the training log messages
  TrainingLog::enableLogging( false ); 

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

  x[0] = 0; x[1] = 0; y[0] = 0; data.addSample( x, y );
  x[0] = 1; x[1] = 1; y[0] = 0; data.addSample( x, y );
  x[0] = 0; x[1] = 1; y[0] = 1; data.addSample( x, y );
  x[0] = 1; x[1] = 0; y[0] = 1; data.addSample( x, y );

  const unsigned int numInputNeurons = data.getNumInputDimensions();
  const unsigned int numHiddenNeurons = 2;
  const unsigned int numOutputNeurons = data.getNumTargetDimensions();
  
  //Initialize the MLP
  mlp.init(numInputNeurons, numHiddenNeurons, numOutputNeurons, Neuron::LINEAR, Neuron::TANH, Neuron::LINEAR );

  //Check the module is not trained
  EXPECT_TRUE( !mlp.getTrained() );
  
  //Set the training settings
  mlp.setMaxNumEpochs( 1000 ); //This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
  mlp.setMinChange( 1.0e-10 ); //This sets the minimum change allowed in training error between any two epochs
  mlp.setLearningRate( 0.1 ); //This sets the rate at which the learning algorithm updates the weights of the neural network
  mlp.setMomentum( 0.0 );
  mlp.setNumRandomTrainingIterations( 1000 ); //This sets the number of times the MLP will be trained, each training iteration starts with new random values
  mlp.setUseValidationSet( false ); //This sets aside a small portiion of the training data to be used as a validation set to mitigate overfitting
  mlp.setRandomiseTrainingOrder( true ); //Randomize the order of the training data so that the training algorithm does not bias the training
  mlp.enableScaling( false ); //This test does not use scaling

  //Check the module is not trained
  EXPECT_TRUE( !mlp.getTrained() );

  //Train a model
  EXPECT_TRUE( mlp.train( data ) );

  EXPECT_TRUE( mlp.getTrained() );

  EXPECT_TRUE( mlp.print() );

  EXPECT_TRUE( mlp.getRMSTrainingError() <= 0.25 );

  EXPECT_TRUE( mlp.getRMSValidationError() == 0.0 ); //This should be zero, as there is no validation set

  for(UINT i=0; i<data.getNumSamples(); i++){
    EXPECT_TRUE( mlp.predict( data[i].getInputVector() ) );
  }

  EXPECT_TRUE( mlp.save( "mlp_model.grt" ) );

  mlp.clear();

  EXPECT_TRUE( !mlp.getTrained() );

  EXPECT_TRUE( mlp.load( "mlp_model.grt" ) );

  EXPECT_TRUE( mlp.getTrained() );

  for(UINT i=0; i<data.getNumSamples(); i++){
    EXPECT_TRUE( mlp.predict( data[i].getInputVector() ) );
  }

}

// Tests the learning algorithm on the XOR problem with data that needs to be scaled
TEST(MLP, XORScaled) {

  //Disable the training log messages
  TrainingLog::enableLogging( false ); 

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

  x[0] = -1000; x[1] = -1000; y[0] = -100; data.addSample( x, y );
  x[0] = 1000; x[1] = 1000; y[0] = -100; data.addSample( x, y );
  x[0] = -1000; x[1] = 1; y[0] = 100; data.addSample( x, y );
  x[0] = 1000; x[1] = -1000; y[0] = 100; data.addSample( x, y );

  const unsigned int numInputNeurons = data.getNumInputDimensions();
  const unsigned int numHiddenNeurons = 2;
  const unsigned int numOutputNeurons = data.getNumTargetDimensions();
  
  //Initialize the MLP
  mlp.init(numInputNeurons, numHiddenNeurons, numOutputNeurons, Neuron::LINEAR, Neuron::TANH, Neuron::LINEAR );

  //Check the module is not trained
  EXPECT_TRUE( !mlp.getTrained() );
  
  //Set the training settings
  mlp.setMaxNumEpochs( 1000 ); //This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
  mlp.setMinChange( 1.0e-10 ); //This sets the minimum change allowed in training error between any two epochs
  mlp.setLearningRate( 0.1 ); //This sets the rate at which the learning algorithm updates the weights of the neural network
  mlp.setMomentum( 0.0 );
  mlp.setNumRandomTrainingIterations( 1000 ); //This sets the number of times the MLP will be trained, each training iteration starts with new random values
  mlp.setUseValidationSet( false ); //This sets aside a small portiion of the training data to be used as a validation set to mitigate overfitting
  mlp.setRandomiseTrainingOrder( true ); //Randomize the order of the training data so that the training algorithm does not bias the training
  mlp.enableScaling( true );

  //Check the module is not trained
  EXPECT_TRUE( !mlp.getTrained() );

  //Train a model
  EXPECT_TRUE( mlp.train( data ) );

  EXPECT_TRUE( mlp.getTrained() );

  EXPECT_TRUE( mlp.print() );

  EXPECT_TRUE( mlp.getRMSTrainingError() <= 0.25 );

  EXPECT_TRUE( mlp.getRMSValidationError() == 0.0 ); //This should be zero, as there is no validation set

  for(UINT i=0; i<data.getNumSamples(); i++){
    EXPECT_TRUE( mlp.predict( data[i].getInputVector() ) );
  }

  EXPECT_TRUE( mlp.save( "mlp_model.grt" ) );

  mlp.clear();

  EXPECT_TRUE( !mlp.getTrained() );

  EXPECT_TRUE( mlp.load( "mlp_model.grt" ) );

  EXPECT_TRUE( mlp.getTrained() );

  for(UINT i=0; i<data.getNumSamples(); i++){
    EXPECT_TRUE( mlp.predict( data[i].getInputVector() ) );
  }

}

int main(int argc, char **argv) {
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
