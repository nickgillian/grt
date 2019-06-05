import GRT
import sys
import argparse


def main():
    '''
     GRT GMM Example
     This examples demonstrates how to initialize, train, and use the GMM algorithm for classification.
     
     The Gaussian Mixture Model Classifier (GMM) is basic but useful classification algorithm that can be used to
     classify an N-dimensional signal.
     
     In this example we create an instance of a GMM classifier and then train the algorithm using some pre-recorded training data.
     The trained GMM algorithm is then used to predict the class label of some test data.
     
     This example shows you how to:
     - Create an initialize the GMM algorithm
     - Load some ClassificationData from a file and partition the training data into a training dataset and a test dataset
     - Train the GMM algorithm using the training dataset
     - Test the GMM algorithm using the test dataset
     - Manually compute the accuracy of the classifier
    
    You should run this example with one argument pointing to the data you want to load. A good dataset to run this example is acc-orientation.grt, which can be found in the GRT data folder.
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Create a new GMM instance and set the number of mixture models to 2
    gmm = GRT.GMM()
    gmm.setNumMixtureModels(2)

    # Load some training data to train the classifier
    trainingData = GRT.ClassificationData()

    if not trainingData.load( filename ):
        print("Failed to load training data: %s" % filename)
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # Train the classifier
    if not gmm.train( trainingData ):
        print("Failed to train classifier!")
        sys.exit(1)

    # Save the model to a file
    if not gmm.save("GMMModel.grt"):
        print("Failed to save the classifier model!")
        sys.exit(1)

    # Load the model from a file
    if not gmm.load("GMMModel.grt"):
        print("Failed to load the classifier model!")
        sys.exit(1)

    # Use the test dataset to test the BAG model
    accuracy = 0.0
    for i in range(testData.getNumSamples()):
        # Get the i'th test sample
        classLabel = testData.get(i).getClassLabel()
        inputVector = testData.get(i).getSample()

        # Perform a prediction using the classifier
        if not gmm.predict( inputVector ):
            print("Failed to perform prediction for test sample: %d" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = gmm.getPredictedClassLabel()
        classLikelihoods = gmm.getClassLikelihoods()
        classDistances = gmm.getClassDistances()
    
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
