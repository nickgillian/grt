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

    # Create a new ANBC instance
    anbc = GRT.ANBC()
    anbc.setNullRejectionCoeff( 10 )
    anbc.enableScaling( True )
    anbc.enableNullRejection( True )

    # Load some training data to train the classifier
    trainingData = GRT.ClassificationData()

    if not trainingData.load( filename ):
        print("Failed to load training data: %s" % filename)
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # Train the classifier
    if not anbc.train( trainingData ):
        print("Failed to train classifier!")
        sys.exit(1)

    # Save the ANBC model to a file
    if not anbc.save("ANBCModel.grt"):
        print("Failed to save the classifier model!")
        sys.exit(1)

    # Load the ANBC model from a file
    if not anbc.load("ANBCModel.grt"):
        print("Failed to load the classifier model!")
        sys.exit(1)

    # Use the test dataset to test the ANBC model
    accuracy = 0.0
    for i in range(testData.getNumSamples()):
        # Get the i'th test sample
        classLabel = testData.get(i).getClassLabel()
        inputVector = testData.get(i).getSample()

        # Perform a prediction using the classifier
        predictSuccess = anbc.predict( inputVector )

        if not predictSuccess:
            print("Failed to perform prediction for test sampel: %d" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = anbc.getPredictedClassLabel()
        classLikelihoods = anbc.getClassLikelihoods()
        classDistances = anbc.getClassDistances()

        # Update the accuracy
        if classLabel == predictedClassLabel:
            accuracy+=1

        print("TestSample: %d ClassLabel: %d PredictedClassLabel: %d" % (i, classLabel, predictedClassLabel))

    print("Test Accuracy: %.3f%%" % (accuracy/float(testData.getNumSamples())*100.0))

    return 0


if __name__ == '__main__':
    sys.exit(main())
