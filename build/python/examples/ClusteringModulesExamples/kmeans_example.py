import GRT
import sys
import argparse

def main():
    '''
    GRT KMeans Example
    This examples demonstrates how to use the KMeans module for unsupervised clustering.
    
    The ClusterData.csv data contains 1000 training samples, randomly generated from 3 clusters.
    The 3 clusters are centered at:
    1: 2 6
    2: 6 4
    3: 7 8
    '''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    # Create a new KMeans instance
    kmeans = GRT.KMeans()
    kmeans.setComputeTheta( True )
    kmeans.setMinChange( 1.0e-10 )
    kmeans.setMinNumEpochs( 10 )
    kmeans.setMaxNumEpochs( 10000 )
    
    # There are a number of ways of training the KMeans algorithm, depending on what you need the KMeans for
    # These are:
    # - with labelled training data (in the ClassificationData format)
    # - with unlablled training data (in the UnlabelledData format)
    # - with unlabelled training data (in a simple MatrixDouble format)
    
    # This example shows you how to train the algorithm with ClassificationData
    
    # Load some training data to train the KMeans algorithm
    trainingData = GRT.ClassificationData()
    
    if not trainingData.load(args.filename):
        print("Failed to load training data!")
        sys.exit(1)

    
    # Train the KMeans algorithm - K will automatically be set to the number of classes in the training dataset
    if not kmeans.train( trainingData ):
        print("Failed to train model!")
        sys.exit(1)

    
    # Get the K clusters from the KMeans instance and print them
    print("Clusters:\n" + str(kmeans.getClusters()))


if __name__ == '__main__':
    main()
    sys.exit(0)
