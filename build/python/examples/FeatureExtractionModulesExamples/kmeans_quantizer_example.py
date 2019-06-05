import GRT
import sys
import argparse

def main():
    '''GRT KMeansQuantizer Example
     This examples demonstrates how to use the KMeansQuantizer module. 
    
     The KMeansQuantizer module quantizes the N-dimensional input vector to a 1-dimensional discrete
     value. This value will be between [0 K-1], where K is the number of clusters used to create the 
     quantization model. Before you use the KMeansQuantizer, you need to train a quantization model.
     To do this, you select the number of clusters you want your quantizer to have and then give it
     any training data in the following formats:
     - ClassificationData
     - TimeSeriesClassificationData
     - TimeSeriesClassificationDataStream
     - UnlabelledClassificationData
     - MatrixDouble
     
     The example loads a basic dataset and uses this to train a quantization model. After the model 
     is trained, the data is then run through the quantizer and the quantized values are printed for
     demonstration.'''

    # Parse the data filename from the argument list
    parser = argparse.ArgumentParser(description='Process some data.')
    parser.add_argument('filename', help='A data file')
    args = parser.parse_args()

    # Load a basic dataset from a file
    data = GRT.ClassificationData()

    if not data.load(args.filename):
        print("ERROR: Failed to load training data!\n")
        sys.exit(1)

    # Create a new KMeansQuantizer instance
    quantizer = GRT.KMeansQuantizer ( 5 )

    # Train the quantization model
    if not quantizer.train( data ) :
        print("ERROR: Failed to train quantizer!\n")
        sys.exit(1)

    # Save the model and settings to a file
    if not quantizer.save("KMeansQuantizerSettings.grt") :
        print("ERROR: Failed to save settings to file!\n")
        sys.exit(1)

    # Load the model and settings from a file
    if not quantizer.load("KMeansQuantizerSettings.grt") :
        print("ERROR: Failed to load settings from file!\n")
        sys.exit(1)

    # Run the data through the quantizer and print out the quantized value
    for i in range(data.getNumSamples()):
        sample = data.get(i).getSample()

        # Quantize the i'th sample
        quantizer.quantize( sample )

        # Print out the sample values and the quantized value
        print("Index: %d Sample: %s QuantizedValue: %d" % (i, str(sample), quantizer.getQuantizedValue()))


if __name__ == '__main__':
    main()
    sys.exit(0)
