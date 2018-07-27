import GRT
import sys
import numpy as np
import math


def main():
    """GRT MedianFilter Example
     This example demonstrates how to create and use the GRT MedianFilter PreProcessing Module.

     The MedianFilter implements a simple median filter, this will give the value seperating the higher
     half of the most recent data from the lower half.  The filter will automatically store the most
     recent input data for you, the size of the buffer that stores the M most recent samples is controlled
     by the MedianFilter's 'window size' parameter.

     In this example we create an instance of a MedianFilter and use this to filter some dummy data. The test
     signal and filtered signals are then printed to std::cout.

     This example shows you how to:
     - Create a new MedianFilter instance with a specific window size for a 1 dimensional signal
     - Filter some data using the MedianFilter
     - Save the MedianFilter settings to a file
     - Load the MedianFilter settings from a file"""
    # Create a new instance of a median average filter with a window size of 5 for a 1 dimensional signal
    filter = GRT.MedianFilter(10, 1)

    # Generate some data (basic counter) and filter it
    for i in range(100):
        # Filter the current value
        filtered_value = filter.filter(i)

        # Get the current data in the circular buffer
        data = filter.getDataBuffer()

        # Print the results
        print("input value: %d \t filtered value: %.3f\t data: %s" % (i, filtered_value, data))

    # Save the filter settings to a file
    filter.save("MedianFilterSettings.grt")

    # We can then load the settings later if needed
    filter.load("MedianFilterSettings.grt")


if __name__ == '__main__':
    main()
    sys.exit(0)
