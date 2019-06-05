import GRT
import sys
import argparse


def main():
    '''
     GRT Random Forests Example

     This examples demonstrates how to initialize, train, and use the RandomForests algorithm for classification.

     RandomForests are an ensemble learning method that operate by building a number of decision trees at training
     time and outputting the class with the majority vote over all the trees in the ensemble.

     In this example we create an instance of a RandomForests algorithm and then train a model using some pre-recorded training data.
     The trained RandomForests model is then used to predict the class label of some test data.

     This example shows you how to:
     - Create and initialize the RandomForests algorithm
     - Load some ClassificationData from a file and partition the training data into a training dataset and a test dataset
     - Train a RandomForests model using the training dataset
     - Test the RandomForests model using the test dataset
     - Manually compute the accuracy of the classifier

    You should run this example with one argument pointing to the data you want to load. A good dataset to run this example is acc-orientation.grt, which can be found in the GRT data folder.
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Create a new RandomForests instance
    forest = GRT.RandomForests()

    # Set the number of trees in the forest
    forest.setForestSize( 10 )

    # Set the number of random candidate splits that will be used to choose the best splitting values
    # More steps will give you a better model, but will take longer to train
    forest.setNumRandomSplits( 100 )

    # Set the maximum depth of the tree
    forest.setMaxDepth( 10 )

    # Set the minimum number of samples allowed per node
    forest.setMinNumSamplesPerNode( 10 )

    # Load some training data to train the classifier
    trainingData = GRT.ClassificationData()

    if not trainingData.load( filename ):
        print("Failed to load training data: %s" % filename)
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # Train the classifier
    if not forest.train( trainingData ):
        print("Failed to train classifier!")
        sys.exit(1)

    # Print the forest
    forest._print()

    # Save the model to a file
    if not forest.save("RandomForestsModel.grt"):
        print("Failed to save the classifier model!")
        sys.exit(1)

    # Load the model from a file
    if not forest.load("RandomForestsModel.grt"):
        print("Failed to load the classifier model!")
        sys.exit(1)

    # Use the test dataset to test the BAG model
    accuracy = 0.0
    for i in range(testData.getNumSamples()):
        # Get the i'th test sample
        classLabel = testData.get(i).getClassLabel()
        inputVector = testData.get(i).getSample()

        # Perform a prediction using the classifier
        if not forest.predict( inputVector ):
            print("Failed to perform prediction for test sample: %d" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = forest.getPredictedClassLabel()
        classLikelihoods = forest.getClassLikelihoods()
        classDistances = forest.getClassDistances()
    
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
