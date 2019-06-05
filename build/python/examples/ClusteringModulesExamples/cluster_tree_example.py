import GRT
import sys
import numpy as np
import argparse

def main():
    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    filename = args.filename

    # Load some training data to train the ClusterTree model
    trainingData = np.loadtxt(filename, delimiter=',')

    # Create a new ClusterTree instance
    ctree = GRT.ClusterTree()
    
    # Set the number of steps that will be used to choose the best splitting values
    # More steps will give you a better model, but will take longer to train
    ctree.setNumSplittingSteps( 100 )
    
    # Set the maximum depth of the tree
    ctree.setMaxDepth( 10 )
    
    # Set the minimum number of samples allowed per node
    ctree.setMinNumSamplesPerNode( 10 )
    
    # Set the minimum RMS error allowed per node
    ctree.setMinRMSErrorPerNode( 0.1 )
    
    # Train a cluster tree model
    if not ctree.train( trainingData ):
        print("Failed to train model!")
        sys.exit(1)

    # if not ctree.save("CTreeModel.grt"): # this fails for some reason
    #     print("Failed to save model!")
    #     sys.exit(1)
    
    # if not ctree.load("CTreeModel.grt"):
    #     print("Failed to train model!")
    #     sys.exit(1)
    
    # Print the tree
    ctree._print()


if __name__ == '__main__':
    main()
    sys.exit(0)