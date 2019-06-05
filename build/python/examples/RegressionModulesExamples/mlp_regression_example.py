import GRT
import sys
import numpy as np
import argparse


def main():
    """GRT MLP Regression Example
     This examples demonstrates how to initialize, train, and use the MLP algorithm for regression.
     
     The Multi Layer Perceptron (MLP) algorithm is a powerful form of an Artificial Neural Network that is commonly used for regression.
     
     In this example we create an instance of an MLP algorithm and then train the algorithm using some pre-recorded training data.
     The trained MLP algorithm is then used to perform regression on the test data.
     
     This example shows you how to:
     - Create and initialize the MLP algorithm for regression
     - Create a new instance of a GestureRecognitionPipeline and add the regression instance to the pipeline
     - Load some RegressionData from a file
     - Train the MLP algorithm using the training dataset
     - Test the MLP algorithm using the test dataset
     - Save the output of the MLP algorithm to a file
    
     To use this example, run the example and pass in two arguments pointing to two files, one for training the model and one for testing the model. You 
     can find example datasets in the main GRT data directory, a good example dataset to use here is the xor.grt file (you can use the same file for training
     and testing if needed)."""
    # Parse the training data filename from the command line
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('trainingDataFilename', help='A data file')
    parser.add_argument('testDataFilename', help='A data file')
    args = parser.parse_args()

    # Load the training data
    training_data = GRT.RegressionData()
    test_data = GRT.RegressionData()

    if not training_data.load(args.trainingDataFilename):
        print("ERROR: Failed to load training data: " + args.trainingDataFilename)
        sys.exit(1)

    if not test_data.load(args.testDataFilename):
        print("ERROR: Failed to load test data: " + args.testDataFilename)
        sys.exit(1)

    print("Training and Test datasets loaded")

    # Print the stats of the datasets
    print("Training data stats:")
    training_data.printStats()

    print("Test data stats:")
    test_data.printStats()

    # Create a new gesture recognition pipeline
    pipeline = GRT.GestureRecognitionPipeline()

    # Setup the MLP, the number of input and output neurons must match the dimensionality of the training/test datasets
    num_input_neurons = training_data.getNumInputDimensions()
    num_hidden_neurons = 2
    num_output_neurons = training_data.getNumTargetDimensions()
    input_activation_function = GRT.Neuron.LINEAR
    hidden_activation_function = GRT.Neuron.TANH
    output_activation_function = GRT.Neuron.LINEAR

    # Initialize the MLP
    mlp = GRT.MLP()
    mlp.init(num_input_neurons, num_hidden_neurons, num_output_neurons, input_activation_function, hidden_activation_function, output_activation_function )

    # Set the training settings
    # This sets the maximum number of epochs (1 epoch is 1 complete iteration of the training data) that are allowed
    mlp.setMaxNumEpochs(1000)
    # This sets the minimum change allowed in training error between any two epochs
    mlp.setMinChange(1.0e-10)
    # This sets the rate at which the learning algorithm updates the weights of the neural network
    mlp.setLearningRate(0.1)
    mlp.setMomentum(0.5)
    # This sets the number of times the MLP will be trained, each training iteration starts with new random values
    mlp.setNumRestarts(1)
    # This sets aside a small portiion of the training data to be used as a validation set to mitigate overfitting
    mlp.setUseValidationSet(False)
    # Use 20% of the training data for validation during the training phase
    mlp.setValidationSetSize(20)
    # Randomize the order of the training data so that the training algorithm does not bias the training
    mlp.setRandomiseTrainingOrder(False)

    # The MLP generally works much better if the training and prediction data is first scaled to a common range (i.e.
    #  [0.0 1.0])
    mlp.enableScaling( True )

    # Add the LogisticRegression instance to the pipeline
    pipeline.setRegressifier(mlp)

    # Train the LinearRegression model
    print("Training LogisticRegression model...")
    if not pipeline.train(training_data):
        print("ERROR: Failed to train LinearRegression model!")
        sys.exit(1)

    print("Model trained.")

    # Test the model
    print("Testing LinearRegression model...")
    if not pipeline.test(test_data):
        print("ERROR: Failed to test LinearRegression model!")
        sys.exit(1)

    print("Test complete. Test RMS error: %.3f" % pipeline.getTestRMSError())

    for i in range(test_data.getNumSamples()):
        input_vector = test_data.get(i).getInputVector()
        target_vector = test_data.get(i).getTargetVector()

        # Map the input vector using the trained regression model
        if not pipeline.predict(input_vector):
            print("ERROR: Failed to map test sample %d" % i)
            sys.exit(1)

        # Get the mapped regression data
        output_vector = pipeline.getRegressionData()

        # Write the mapped value and also the target value to the file
        print(output_vector, target_vector)

    pass


if __name__ == '__main__':
    main()
    sys.exit(0)
