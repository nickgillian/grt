import GRT
import sys
import argparse


def main():
    '''GRT ZeroCrossingCounter Example
     This example demonstrates how to create and use the GRT ZeroCrossingCounter FeatureExtraction Module.

     The ZeroCrossingCounter module counts the number of zero crossings that occur in an N dimensional signal
     over a given window. The ZeroCrossingCounter class is good for extracting features that describe how much
     change is occurring in an N dimensional signal. An example application might be to use the ZeroCrossingCounter
     in combination with one of the GRT classification algorithms to determine if an object is being shaken, and
     if so, how the object is being shaken.

     In this example we create an instance of a ZeroCrossingCounter and use this to extract some features from a
     recording of some acceleration data.  The recording contains a number of different types of the accelerometer
     being shaken, you can see a plot of the raw data here:
     http://www.nickgillian.com/archive/wiki/grt/reference/ZeroCrossingCounter/ZeroCrossingCounterFeatureExtractionExampleImage1.jpg

     The raw data and features are then saved to a file (so you can plot the results in Matlab, Excel, etc. if needed).

     This example shows you how to:
     - Create a new ZeroCrossingCounter instance and customize the feature settings
     - Compute some features from a 3 dimensional data signal
     - Save the ZeroCrossingCounter settings to a file
     - Load the ZeroCrossingCounter settings from a file'''
    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    # Load the example data
    data = GRT.ClassificationData() 

    if not data.load(args.filename):
        print("ERROR: Failed to load data from file!")
        sys.exit(1)

    # The variables used to initialize the zero crossing counter feature extraction
    searchWindowSize = 20
    deadZoneThreshold = 0.01
    numDimensions = data.getNumDimensions()
    # This could also be ZeroCrossingCounter::COMBINED_FEATURE_MODE
    featureMode = GRT.ZeroCrossingCounter.INDEPENDANT_FEATURE_MODE

    # Create a new instance of the ZeroCrossingCounter feature extraction
    zeroCrossingCounter = GRT.ZeroCrossingCounter(searchWindowSize,deadZoneThreshold,numDimensions,featureMode)

    # Loop over the accelerometer data, at each time sample (i) compute the features
    # using the new sample and then write the results to a file
    for i in range(data.getNumSamples()):

        # Compute the features using this new sample
        zeroCrossingCounter.computeFeatures( data.get(i).getSample() )

        # Write the data
        print("InputVector: " + str(data.get(i).getSample()))

        # Get the latest feature vector
        print("FeatureVector: " + str(zeroCrossingCounter.getFeatureVector()))
    
    # Save the MovementIndex settings to a file
    zeroCrossingCounter.save("MovementIndexSettings.grt")
    
    # You can then load the settings again if you need them
    zeroCrossingCounter.load("MovementIndexSettings.grt")


if __name__ == '__main__':
    main()
    sys.exit(0)
