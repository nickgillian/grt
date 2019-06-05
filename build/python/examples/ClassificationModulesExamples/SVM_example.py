import GRT
import sys
import argparse


def main():
    '''
     GRT SVM Example
     his examples demonstrates how to initialize, train, and use the SVM algorithm for classification. 
     
     The Support Vector Machine (SVM) classifier is a powerful classifier that works well on a wide range of classification problems, even problems in high dimensions and that are not linearly separable. 
     
     In this example we create an instance of a SVM algorithm and then train the algorithm using some pre-recorded training data.
     The trained SVM algorithm is then used to predict the class label of some test data.
     
     This example shows you how to:
     - Create an initialize the SVM algorithm using a LINEAR kernel
     - Load some ClassificationData from a file and partition the training data into a training dataset and a test dataset
     - Train the SVM algorithm using the training dataset
     - Test the SVM algorithm using the test dataset
     - Manually compute the accuracy of the classifier

    You should run this example with one argument pointing to the data you want to load. A good dataset to run this example is acc-orientation.grt, which can be found in the GRT data folder.
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Create a new SVM classifier with a linear kernel
    # Other kernel options you could choose are: POLY_KERNEL, RBF_KERNEL, SIGMOID_KERNEL, PRECOMPUTED_KERNEL
    svm = GRT.SVM(GRT.SVM.LINEAR_KERNEL)

    # The SVM will typically work much better if we scale the training and prediction data, so turn scaling on
    svm.enableScaling( True )

    # Load some training data to train the classifier
    trainingData = GRT.ClassificationData()

    if not trainingData.load( filename ):
        print("Failed to load training data: %s" % filename)
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # Train the classifier
    if not svm.train( trainingData ):
        print("Failed to train classifier!")
        sys.exit(1)

    # Save the model to a file
    if not svm.save("SoftmaxModel.grt"):
        print("Failed to save the classifier model!")
        sys.exit(1)

    # Load the model from a file
    if not svm.load("SoftmaxModel.grt"):
        print("Failed to load the classifier model!")
        sys.exit(1)

    # Use the test dataset to test the BAG model
    accuracy = 0.0
    for i in range(testData.getNumSamples()):
        # Get the i'th test sample
        classLabel = testData.get(i).getClassLabel()
        inputVector = testData.get(i).getSample()

        # Perform a prediction using the classifier
        if not svm.predict( inputVector ):
            print("Failed to perform prediction for test sample: %d" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = svm.getPredictedClassLabel()
        classLikelihoods = svm.getClassLikelihoods()
        classDistances = svm.getClassDistances()
    
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
