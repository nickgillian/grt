import GRT
import sys
import numpy as np
import argparse


def main():
    """Logistic Regression Example
     This examples demonstrates how to initialize, train, and use the LinearRegression class for regression.
    
     Linear Regression is a simple regression algorithm that can map an N-dimensional signal to a 1-dimensional signal.
    
     In this example we create an instance of an LinearRegression algorithm and then use the algorithm to train a model using some pre-recorded training data.
     The trained model is then used to perform regression on the test data.
    
     This example shows you how to:
     - Create an initialize the LinearRegression algorithm for regression
     - Create a new instance of a GestureRecognitionPipeline and add the regression instance to the pipeline
     - Load some RegressionData from a file
     - Train a LinearRegression model using the training dataset
     - Test the LinearRegression model using the test dataset
     - Save the output of the LinearRegression algorithm to a file"""
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

    # Add a LinearRegression instance to the pipeline
    pipeline.setRegressifier(GRT.LinearRegression())

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
