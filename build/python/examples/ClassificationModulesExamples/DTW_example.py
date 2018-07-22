import GRT
import sys
import argparse


def main():
    '''
    GRT DTW Example
     This examples demonstrates how to initialize, train, and use the DTW algorithm for classification.
     
     The Dynamic Time Warping (DTW) algorithm is a powerful classifier that works very well for recognizing temporal gestures.
     
     In this example we create an instance of an DTW algorithm and then train the algorithm using some pre-recorded training data.
     The trained DTW algorithm is then used to predict the class label of some test data.
     
     This example shows you how to:
     - Create an initialize the DTW algorithm
     - Load some LabelledTimeSeriesClassificationData from a file and partition the training data into a training dataset and a test dataset
     - Trim any periods of non-movement from the start and end of each timeseries recording 
     - Train the DTW algorithm using the training dataset
     - Test the DTW algorithm using the test dataset
     - Manually compute the accuracy of the classifier
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Create a new DTW instance, using the default parameters
    dtw = GRT.DTW()

    # Load some training data to train the classifier - the DTW uses TimeSeriesClassificationData
    trainingData = GRT.TimeSeriesClassificationData()

    if not trainingData.load(filename):
        print("Failed to load training data!")
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # Trim the training data for any sections of non-movement at the start or end of the recordings
    dtw.enableTrimTrainingData(True, 0.1, 90)

    # Train the classifier
    if not dtw.train( trainingData ):
        print("Failed to train classifier!")
        sys.exit(1)

    # Save the decision tree model to a file
    if not dtw.save("DTWModel.grt"):
        print("Failed to save the classifier model!")
        sys.exit(1)

    # Load the decision tree model from a file
    if not dtw.load("DTWModel.grt"):
        print("Failed to load the classifier model!")
        sys.exit(1)

    # Use the test dataset to test the decision tree model
    accuracy = 0.0
    for i in range(testData.getNumSamples()):
        # Get the i'th test sample
        classLabel = testData.get(i).getClassLabel()
        timeseries = testData.get(i).getData()

        # Perform a prediction using the classifier
        if not dtw.predict( timeseries ):
            print("Failed to perform prediction for test sample: %d" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = dtw.getPredictedClassLabel()
        classLikelihoods = dtw.getClassLikelihoods()
        classDistances = dtw.getClassDistances()

        # Update the accuracy
        if classLabel == predictedClassLabel:
            accuracy += 1

        # Print out the results
        print("TestSample: %d  ClassLabel: %d PredictedClassLabel: %d" % (i, classLabel, predictedClassLabel))

        print(" ClassLikelihoods: " + str(classLikelihoods))
        print(" ClassDistances: " + str(classDistances))

    print("Test Accuracy: %.3f%%" % (accuracy/float(testData.getNumSamples())*100.0))

    return 0


if __name__ == '__main__':
    sys.exit(main())
