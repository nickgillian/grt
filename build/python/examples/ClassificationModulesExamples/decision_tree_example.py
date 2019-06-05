import GRT
import sys
import argparse


def main():
    '''
    GRT DecisionTree

     This examples demonstrates how to initialize, train, and use the DecisionTree algorithm for classification.

     Decision Trees are conceptually simple classifiers that work well on even complex classification tasks.
     Decision Trees partition the feature space into a set of rectangular regions, classifying a new datum by
     finding which region it belongs to.

     In this example we create an instance of a DecisionTree algorithm and then train a model using some pre-recorded training data.
     The trained DecisionTree model is then used to predict the class label of some test data.

     This example shows you how to:
     - Create and initialize the DecisionTree algorithm
     - Load some ClassificationData from a file and partition the training data into a training dataset and a test dataset
     - Train a DecisionTree model using the training dataset
     - Test the DecisionTree model using the test dataset
     - Manually compute the accuracy of the classifier

    You should run this example with one argument pointing to the data you want to load. A good dataset to run this example is acc-orientation.grt, which can be found in the GRT data folder.
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Create a new DecisionTree instance
    dTree = GRT.DecisionTree()

    # Set the node that the DecisionTree will use - different nodes may result in different decision boundaries
    # and some nodes may provide better accuracy than others on specific classification tasks
    # The current node options are:
    # - DecisionTreeClusterNode
    # - DecisionTreeThresholdNode
    dTree.setDecisionTreeNode( GRT.DecisionTreeClusterNode() )

    # Set the number of steps that will be used to choose the best splitting values
    # More steps will give you a better model, but will take longer to train
    dTree.setNumSplittingSteps( 1000 )

    # Set the maximum depth of the tree
    dTree.setMaxDepth( 10 )

    # Set the minimum number of samples allowed per node
    dTree.setMinNumSamplesPerNode( 10 )

    # Load some training data to train the classifier
    trainingData = GRT.ClassificationData()

    if not trainingData.load( filename ):
        print("Failed to load training data: %s" % filename)
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # Train the classifier
    if not dTree.train( trainingData ):
        print("Failed to train classifier!")
        sys.exit(1)

    # Save the decision tree model to a file
    if not dTree.save("DecisionTreeModel.grt"):
        print("Failed to save the classifier model!")
        sys.exit(1)

    # Load the decision tree model from a file
    if not dTree.load("DecisionTreeModel.grt"):
        print("Failed to load the classifier model!")
        sys.exit(1)

    # Use the test dataset to test the decision tree model
    accuracy = 0.0
    for i in range(testData.getNumSamples()):
        # Get the i'th test sample
        classLabel = testData.get(i).getClassLabel()
        inputVector = testData.get(i).getSample()

        # Perform a prediction using the classifier
        if not dTree.predict( inputVector ):
            print("Failed to perform prediction for test sample: %d" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = dTree.getPredictedClassLabel()
        classLikelihoods = dTree.getClassLikelihoods()
        classDistances = dTree.getClassDistances()

        # Update the accuracy
        if classLabel == predictedClassLabel:
            accuracy += 1

        # Print out the results
        print("TestSample: %d  ClassLabel: %d PredictedClassLabel: %d" % (i, classLabel, predictedClassLabel))

        print(" ClassLikelihoods: " + str(classLikelihoods))
        print(" ClassDistances: " + str(classDistances))

    return 0

if __name__ == '__main__':
    sys.exit(main())
