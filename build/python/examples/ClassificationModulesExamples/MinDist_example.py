import GRT
import sys
import argparse


def main():
    '''
     GRT MinDist Example
     This examples demonstrates how to initialize, train, and use the MinDist algorithm for classification.

     The MinDist (Minimum Distance) algorithm is a simple classifier that works very well on both basic and more complex recognition problems.
     The MinDist algorithm is a very fast classifier and so is a good choice if you need to a classifier with a low-computional overhead.

     In this example we create an instance of a MinDist classifier and then train the algorithm using some pre-recorded training data.
     The trained MinDist algorithm is then used to predict the class label of some test data.
     
     This example shows you how to:
     - Create an initialize the MinDist algorithm
     - Load some ClassificationData from a file and partition the training data into a training dataset and a test dataset
     - Train the MinDist algorithm using the training dataset
     - Test the MinDist algorithm using the test dataset
     - Manually compute the accuracy of the classifier

    You should run this example with one argument pointing to the data you want to load. A good dataset to run this example is acc-orientation.grt, which can be found in the GRT data folder.
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Create a new MinDist instance, using the default parameters
    minDist = GRT.MinDist()

    # Set how many clusters the minDist algorithm will use when training the minDist classification model
    # If your classification problem is more complex, then you might want to use a larger number of clusters
    minDist.setNumClusters( 2 )

    # Load some training data to train the classifier
    trainingData = GRT.ClassificationData()

    if not trainingData.load( filename ):
        print("Failed to load training data: %s" % filename)
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # Train the classifier
    if not minDist.train( trainingData ):
        print("Failed to train classifier!")
        sys.exit(1)

    # Save the model to a file
    if not minDist.save("minDistModel.grt"):
        print("Failed to save the classifier model!")
        sys.exit(1)

    # Load the model from a file
    if not minDist.load("minDistModel.grt"):
        print("Failed to load the classifier model!")
        sys.exit(1)

    # Use the test dataset to test the BAG model
    accuracy = 0.0
    for i in range(testData.getNumSamples()):
        # Get the i'th test sample
        classLabel = testData.get(i).getClassLabel()
        inputVector = testData.get(i).getSample()

        # Perform a prediction using the classifier
        if not minDist.predict( inputVector ):
            print("Failed to perform prediction for test sample: %d" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = minDist.getPredictedClassLabel()
        classLikelihoods = minDist.getClassLikelihoods()
        classDistances = minDist.getClassDistances()
    
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
