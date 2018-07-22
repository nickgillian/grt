import GRT
import sys
import argparse
import numpy as np


def main():
    '''
    GRT HMM Discrete Example
     This examples demonstrates how to initialize, train, and use the Discrete HMM algorithm for classification.
     
     Hidden Markov Models (HMM) are powerful classifiers that work well on temporal classification problems when you have a large training dataset.
    
     The HMM algorithm in the GRT can be either a discrete or continuous HMM.  A discrete HMM means that the input to the HMM algorithm must be a discrete integer value in the range [0 numSymbols-1].  A continuous HMM means that the input to the HMM algorithm can be an N-dimensional floating point vector.  For the discrete HMM, you can convert N-dimensional continuous data into a 1-dimensional discrete data using one of the GRT Quantization algorithms, such as KMeansQuantizer.
    
     In this example we create an instance of a Discrete HMM algorithm and then train a Discrete HMM model using some pre-recorded training data.
     The trained HMM algorithm is then used to predict the class label of some test data.
     
     This example shows you how to:
     - Create an initialize the Discrete HMM algorithm
     - Load some TimeSeriesClassificationData from a file and partition the training data into a training dataset and a test dataset
     - Train a KMeansQuantizer and quantize the training and test data using the trained quantizer
     - Train the HMM algorithm using the quantized training dataset
     - Test the HMM algorithm using the quantized test dataset
     - Manually compute the accuracy of the classifier
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Load some training data to train the classifier - TimeSeriesClassificationData
    trainingData = GRT.TimeSeriesClassificationData()

    if not trainingData.load(filename):
        print("Failed to load training data!")
        sys.exit(1)

    # Use 20% of the training dataset to create a test dataset
    testData = trainingData.split( 80 )

    # The input to the HMM must be a quantized discrete value
    # We therefore use a KMeansQuantizer to covert the N-dimensional continuous data into 1-dimensional discrete data
    NUM_SYMBOLS = 10
    quantizer = GRT.KMeansQuantizer( NUM_SYMBOLS )

    # Train the quantizer using the training data
    if not quantizer.train( trainingData ):
        print("ERROR: Failed to train quantizer!")
        sys.exit(1)

    # Quantize the training data
    quantizedTrainingData = GRT.TimeSeriesClassificationData( 1 )

    for i in range(trainingData.getNumSamples()):

        classLabel = trainingData.get(i).getClassLabel()
        quantizedSample = np.zeros((0, 1), dtype=np.float32)
        sample = trainingData.get(i).getData()
    
        for j in range(trainingData.get(i).getLength()):
            quantizer.quantize( sample[j] )
            quantizedSample = np.vstack([quantizedSample, quantizer.getFeatureVector()])
    
        if not quantizedTrainingData.addSample(classLabel, quantizedSample):
            print("ERROR: Failed to quantize training data!")
            sys.exit(1)

    # Create a new HMM instance
    hmm = GRT.HMM()

    # Set the HMM as a Discrete HMM
    hmm.setHMMType( GRT.HMM_DISCRETE )

    # Set the number of states in each model
    hmm.setNumStates( 4 )

    # Set the number of symbols in each model, this must match the number of symbols in the quantizer
    hmm.setNumSymbols( NUM_SYMBOLS )

    # Set the HMM model type to LEFTRIGHT with a delta of 1
    hmm.setModelType( GRT.HMM_LEFTRIGHT )
    hmm.setDelta( 1 )

    # Set the training parameters
    hmm.setMinChange( 1.0e-5 )
    hmm.setMaxNumEpochs( 40 )
    hmm.setNumRandomTrainingIterations( 20 )

    # Train the classifier
    if not hmm.train( quantizedTrainingData ):
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

    # Quantize the test data
    quantizedTestData = GRT.TimeSeriesClassificationData( 1 )

    for i in range(testData.getNumSamples()):

        classLabel = testData.get(i).getClassLabel()
        quantizedSample = np.zeros((0, 1), dtype=np.float32)
        sample = testData.get(i).getData()

        for j in range(testData.get(i).getLength()):
            quantizer.quantize( sample[j] )
            quantizedSample = np.vstack([quantizedSample, quantizer.getFeatureVector()])

        if not quantizedTestData.addSample(classLabel, quantizedSample):
            print("ERROR: Failed to quantize training data!")
            sys.exit(1)

    # Use the test dataset to test the decision tree model
    accuracy = 0.0
    for i in range(quantizedTestData.getNumSamples()):
        # Get the i'th test sample
        classLabel = quantizedTestData.get(i).getClassLabel()

        print(quantizedTestData.get(i).getData().shape)
        # Perform a prediction using the classifier
        if not hmm.predict( quantizedTestData.get(i).getData() ):
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
