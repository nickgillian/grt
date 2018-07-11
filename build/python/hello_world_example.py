import GRT
import sys
import argparse


def main():
    # Parse the training data filename from the command line
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Load some training data from a file
    training_data = GRT.ClassificationData()
    
    print("Loading dataset...")
    if not training_data.load(filename):
        print("ERROR: Failed to load training data from file\n")
        sys.exit(1)

    print("Data Loaded")
    
    # Print out some stats about the training data
    training_data.printStats()
    
    # Partition the training data into a training dataset and a test dataset. 80 means that 80%
    # of the data will be used for the training data and 20% will be returned as the test dataset
    print("Splitting data into training/test split...")
    test_data = training_data.split(80)
    
    # Create a new Gesture Recognition Pipeline
    pipeline = GRT.GestureRecognitionPipeline()
    
    # Add a KNN classifier to the pipeline with a K value of 10
    knn = GRT.KNN(10)
    pipeline.setClassifier(knn)

    # Train the pipeline using the training data
    print("Training model...")
    if not pipeline.train(training_data):
        print("ERROR: Failed to train the pipeline!\n")
        sys.exit(1)

    # Save the pipeline to a file
    if not pipeline.save("HelloWorldPipeline.grt"):
        print("ERROR: Failed to save the pipeline!\n")
        sys.exit(1)

    # Load the pipeline from a file
    if not pipeline.load("HelloWorldPipeline.grt"):
        print("ERROR: Failed to load the pipeline!\n")
        sys.exit(1)

    # Test the pipeline using the test data
    print("Testing model...")
    if not pipeline.test(test_data):
        print("ERROR: Failed to test the pipeline!\n")
        sys.exit(1)

    # Print some stats about the testing
    print("Pipeline Test Accuracy: %.3f"%(pipeline.getTestAccuracy()))

    # Manually project the test dataset through the pipeline
    test_accuracy = 0.0

    for i in range(test_data.getNumSamples()):
        pipeline.predict(test_data.get(i).getSample())

    if test_data.get(i).getClassLabel() == pipeline.getPredictedClassLabel():
        test_accuracy += 1

    print("Manual test accuracy: %.3f" % (float(test_accuracy) / float(test_data.getNumSamples()) * 100.0))

    # Get the vector of class labels from the pipeline
    class_labels = pipeline.getClassLabels()

    # Print out the precision
    print("Precision: " + str([pipeline.getTestPrecision(c) for c in class_labels]))

    # Print out the recall
    print("Recall: " + str([pipeline.getTestRecall(c) for c in class_labels]))

    # Print out the f-measure
    print("FMeasure: " + str([pipeline.getTestFMeasure(c) for c in class_labels]))

    # Print out the confusion matrix
    confusion_matrix = pipeline.getTestConfusionMatrix()
    print("ConfusionMatrix: \n")
    print(confusion_matrix)

    print("Done")


if __name__ == '__main__':
    main()
    sys.exit(0)
