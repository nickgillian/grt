import GRT
import sys
import argparse

def main():
    '''GRT Class Label Filter Example
     This examples demonstrates how to use the Class Label Filter to filter the predicted class label of any classification module
     using the gesture recognition pipeline.

     The Class Label Filter is a useful post-processing module which can remove erroneous or sporadic prediction spikes that may be
     made by a classifier on a continuous input stream of data.  For instance, imagine a classifier that correctly outputs the predicted
     class label of 1 for a large majority of the time that a user is performing gesture 1, but every so often (perhaps due to sensor noise),
     the classifier outputs the class label of 2.  In this instance the class label filter can be used to remove these sporadic prediction
     values, with the output of the class label filter in this instance being 1.

     In this example we create a new gesture recognition pipeline and add an ANBC module for classification along with a Class Label Filter
     as a post-processing module.  The ANBC classifier is then trained with some dummy data (which consists of 3 simple classes generated
     from 3 different Gaussian distributions).  Some test data is then used to demonstrate the result of using the Class Label Filter to
     post process the predicted class label from the ANBC algorithm.  The test data contains a continuous stream of data from class 1, with
     a few sporadic class 2 and 3 labels added randomly to the stream.

     This example shows you how to:
     - Create an initialize a GestureRecognitionPipeline and add an ANBC module and a ClassLabelFilter module
     - Load some ClassificationData from a file and train the classifier
     - Use some test data to demonstrate the effect of using a Class Label Filter
     - Print the Processed and Unprocessed Predicted Class Label'''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('train_file', help='A training data file')
    parser.add_argument('test_file', help='A testing data file')
    args = parser.parse_args()

    # Create a new gesture recognition pipeline
    pipeline = GRT.GestureRecognitionPipeline()

    # Add an ANBC module
    pipeline.setClassifier( GRT.ANBC() )

    # Add a ClassLabelFilter as a post processing module with a minCount of 5 and a buffer size of 10
    pipeline.addPostProcessingModule( GRT.ClassLabelFilter(5,10) )

    # Load some training data to train and test the classifier
    trainingData = GRT.ClassificationData()
    testData = GRT.ClassificationData()

    if not trainingData.load(args.train_file):
        print("Failed to load training data!\n")
        sys.exit(1)

    if not testData.load(args.test_file):
        print("Failed to load training data!\n")
        sys.exit(1)

    # Train the classifier
    if not pipeline.train( trainingData ):
        print("Failed to train classifier!\n")
        sys.exit(1)

    # Use the test dataset to demonstrate the output of the ClassLabelChangeFilter
    for i in range(testData.getNumSamples()):
        inputVector = testData.get(i).getSample()

        if not pipeline.predict( inputVector ):
            print("Failed to perform prediction for test sample %d" % i)
            sys.exit(1)


        # Get the predicted class label (this will be the processed class label)
        predictedClassLabel = pipeline.getPredictedClassLabel()

        # Get the unprocessed class label (i.e. the direct output of the classifier)
        unprocessedClassLabel = pipeline.getUnProcessedPredictedClassLabel()

        # Also print the results to the screen
        print("Processed Class Label: \t%d\tUnprocessed Class Label: \t%d" % (predictedClassLabel, unprocessedClassLabel))


if __name__ == '__main__':
    main()
    sys.exit(0)
