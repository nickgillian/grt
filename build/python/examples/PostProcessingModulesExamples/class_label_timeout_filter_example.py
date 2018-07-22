import GRT
import sys
import argparse

def main():
    '''GRT Class Label Timeout Filter Example
     This examples demonstrates how to use the Class Label Timeout Filter to filter the predicted class label of any classification module
     using the gesture recognition pipeline.

     The Class Label Timeout Filter is a useful post-processing module which debounces a gesture (i.e. it stops a single gesture from being
     recognized multiple times over a short time frame). For instance, it is normally the case that whenever a user performs a gesture, such
     as a swipe gesture for example, that the recognition system may recognize this single gesture several times because the user's movements
     are being sensed at a high sample rate (i.e. 100Hz). The Class Label Timeout Filter can be used to ensure that a gesture, such as the
     previous swipe gesture example, is only recognize once within any given timespan.

     This example demonstrates how to create a new gesture recognition pipeline and add an ANBC module for classification along with a Class
     Label Timeout Filter as a post-processing module. The ANBC classifier is trained with some dummy data (which consists of 3 simple classes
     generated from 3 different Gaussian distributions). Some test data is then used to demonstrate the result of using the Class Label Timeout
     Filter to post process the predicted class label from the ANBC algorithm. The test data contains a continuous stream of data from class 1,
     followed by class 2 and then class 3. The time between each prediction is set to 100 milliseconds, with the timeout for the Class Label
     Timeout Filter set to 1000 milliseconds.

     This example shows you how to:
     - Create an initialize a GestureRecognitionPipeline and add an ANBC module and a ClassLabelTimeoutFilter module
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
    pipeline.addPostProcessingModule( GRT.ClassLabelTimeoutFilter(1000,GRT.ClassLabelTimeoutFilter.ALL_CLASS_LABELS) )

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
