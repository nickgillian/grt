import GRT
import sys
import argparse
import inspect
def main():
    # Parse the training data filename from the command line
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Load some training data from a file
    training_data = GRT.ClassificationData()

    print("Loading dataset...")
    if not training_data.load(filename):
        print("ERROR: Failed to load training data from file\n")
        sys.exit(1)

    print("Data Loaded")

    # Print out some stats about the training data
    training_data.printStats()

    # Partition the training data into a training dataset and a test dataset. 80 means that 80%
    # of the data will be used for the training data and 20% will be returned as the test dataset
    print("Splitting data into training/test split...")
    test_data = training_data.split(80)

    # Create a new AdaBoost instance
    ada_boost = GRT.AdaBoost()

    # Set the weak classifier you want to use
    ada_boost.setWeakClassifier( GRT.DecisionStump() )


    # Train the classifier
    if not ada_boost.train( training_data ):
        print("Failed to train classifier!\n")
        sys.exit(1)

    # Save the model to a file
    if not ada_boost.save("AdaBoostModel.grt"):
        print("Failed to save the classifier model!\n")
        sys.exit(1)

    # Load the model from a file
    if not ada_boost.load("AdaBoostModel.grt"):
        print("Failed to load the classifier model!\n")
        sys.exit(1)

    # Use the test dataset to test the AdaBoost model
    accuracy = 0.0
    for i in range(test_data.getNumSamples()):
        # Get the i'th test sample
        classLabel = test_data.get(i).getClassLabel()
        inputVector = test_data.get(i).getSample()

        # Perform a prediction using the classifier
        if not ada_boost.predict( inputVector ):
            print("Failed to perform prediction for test sample %d\n" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = ada_boost.getPredictedClassLabel()
        maximumLikelhood = ada_boost.getMaximumLikelihood()
        classLikelihoods = ada_boost.getClassLikelihoods()
        classDistances = ada_boost.getClassDistances()

        # Update the accuracy
        if classLabel == predictedClassLabel:
            accuracy += 1

        print("TestSample: %d ClassLabel: %d" % (i, classLabel))
        print(" PredictedClassLabel: %d Likelihood: %.3f" % (predictedClassLabel , maximumLikelhood))

    print("Test Accuracy: %.3f%%" % (accuracy/float(test_data.getNumSamples())*100.0))

    print("DONE")

if __name__ == '__main__':
    main()
    sys.exit(0)