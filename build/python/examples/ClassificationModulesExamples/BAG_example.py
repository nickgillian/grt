import GRT
import sys
import argparse


def main():
    '''
    GRT BAG Example
     This examples demonstrates how to initialize, train, and use the BAG algorithm for classification.

     The BAG classifier implements bootstrap aggregating (bagging), a machine learning ensemble meta-algorithm
     designed to improve the stability and accuracy of other machine learning algorithms.  Bagging also reduces
     variance and helps to avoid overfitting. Although it is usually applied to decision tree methods, the BAG
     class can be used with any type of GRT classifier.

     In this example we create an instance of a BAG algorithm, add several classifiers to the BAG ensemble and then
     train a BAG model using some pre-recorded training data.  The trained BAG model is then used to predict the
     class label of some test data.

     This example shows you how to:
     - Create an initialize the BAG algorithm
     - Add several classifiers to the BAG ensemble
     - Load some ClassificationData from a file and partition the training data into a training dataset and a test dataset
     - Train a BAG model using the training dataset
     - Test the model using the test dataset
     - Compute the accuracy of the classifier

    You should run this example with one argument pointing to the data you want to load. A good dataset to run this example is acc-orientation.grt, which can be found in the GRT data folder.
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Create a new BAG instance
    bag = GRT.BAG()

    # Add an adaptive naive bayes classifier to the BAG ensemble
    bag.addClassifierToEnsemble( GRT.ANBC() )

    # Add a MinDist classifier to the BAG ensemble, using two clusters
    min_dist_two_clusters = GRT.MinDist() 
    min_dist_two_clusters.setNumClusters(2)
    bag.addClassifierToEnsemble( min_dist_two_clusters )

    # Add a MinDist classifier to the BAG ensemble, using five clusters
    min_dist_five_clusters = GRT.MinDist()
    min_dist_five_clusters.setNumClusters(5);
    bag.addClassifierToEnsemble( min_dist_five_clusters )

    # Load some training data to train the classifier
    trainingData = GRT.ClassificationData()

    if not trainingData.load( filename ):
        print("Failed to load training data: %s" % filename)
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # Train the classifier
    if not bag.train( trainingData ):
        print("Failed to train classifier!")
        sys.exit(1)

    # Save the BAG model to a file
    if not bag.save("BAGModel.grt"):
        print("Failed to save the classifier model!")
        sys.exit(1)

    # Load the BAG model from a file
    if not bag.load("BAGModel.grt"):
        print("Failed to load the classifier model!")
        sys.exit(1)

    # Use the test dataset to test the BAG model
    accuracy = 0.0
    for i in range(testData.getNumSamples()):
        # Get the i'th test sample
        classLabel = testData.get(i).getClassLabel()
        inputVector = testData.get(i).getSample()

        # Perform a prediction using the classifier
        if not bag.predict( inputVector ):
            print("Failed to perform prediction for test sample: %d" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = bag.getPredictedClassLabel()
        classLikelihoods = bag.getClassLikelihoods()
        classDistances = bag.getClassDistances()
    
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
