import GRT
import sys
import argparse


def main():
    '''
    GRT HMM Continuous Example
     This examples demonstrates how to initialize, train, and use the Continuous HMM algorithm for classification.
     
     Hidden Markov Models (HMM) are powerful classifiers that work well on temporal classification problems when you have a large training dataset.
    
     The HMM algorithm in the GRT can be either a discrete or continuous HMM.  A discrete HMM means that the input to the HMM algorithm must be a 
     discrete integer value in the range [0 numSymbols-1].  A continuous HMM means that the input to the HMM algorithm can be an N-dimensional 
     floating point vector.  For the discrete HMM, you can convert N-dimensional continuous data into a 1-dimensional discrete data using one of 
     the GRT Quantization algorithms, such as KMeansQuantizer.
    
     In this example we create an instance of a Continuous HMM algorithm and then train a Continuous HMM model using some pre-recorded training data.
     The trained HMM algorithm is then used to predict the class label of some test data.
     
     This example shows you how to:
     - Create an initialize the Continuous HMM algorithm
     - Load some TimeSeriesClassificationData from a file and partition the training data into a training dataset and a test dataset
     - Train the HMM algorithm using the training dataset
     - Test the HMM algorithm using the test dataset
     - Manually compute the accuracy of the classifier
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Create a new HMM instance
    hmm = GRT.HMM()

    # Set the HMM as a Continuous HMM
    hmm.setHMMType( GRT.HMM_CONTINUOUS )

    # Set the downsample factor, a higher downsample factor will speed up the prediction time, but might reduce the classification accuracy
    hmm.setDownsampleFactor( 5 )

    # Set the committee size, this sets the (top) number of models that will be used to make a prediction
    hmm.setCommitteeSize( 10 )

    # Tell the hmm algorithm that we want it to estimate sigma from the training data
    hmm.setAutoEstimateSigma( True )

    # Set the minimum value for sigma, you might need to adjust this based on the range of your data
    # If you set setAutoEstimateSigma to false, then all sigma values will use the value below
    hmm.setSigma( 20.0 )

    # Set the HMM model type to LEFTRIGHT with a delta of 1, this means the HMM can only move from the left-most state to the right-most state
    # in steps of 1
    hmm.setModelType( GRT.HMM_LEFTRIGHT )
    hmm.setDelta( 1 )

    # Load some training data to train the classifier - TimeSeriesClassificationData
    trainingData = GRT.TimeSeriesClassificationData()

    if not trainingData.load(filename):
        print("Failed to load training data!")
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # Train the classifier
    if not hmm.train( trainingData ):
        print("Failed to train classifier!")
        sys.exit(1)

    # Save the decision tree model to a file
    if not hmm.save("HMMModel.grt"):
        print("Failed to save the classifier model!")
        sys.exit(1)

    # Load the decision tree model from a file
    if not hmm.load("HMMModel.grt"):
        print("Failed to load the classifier model!")
        sys.exit(1)

    # Use the test dataset to test the decision tree model
    accuracy = 0.0
    for i in range(testData.getNumSamples()):
        # Get the i'th test sample
        classLabel = testData.get(i).getClassLabel()
        timeseries = testData.get(i).getData()

        # Perform a prediction using the classifier
        if not hmm.predict( timeseries ):
            print("Failed to perform prediction for test sample: %d" % i)
            sys.exit(1)

        # Get the predicted class label
        predictedClassLabel = hmm.getPredictedClassLabel()
        classLikelihoods = hmm.getClassLikelihoods()
        classDistances = hmm.getClassDistances()

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
