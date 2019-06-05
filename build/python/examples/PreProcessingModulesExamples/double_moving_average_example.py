import GRT
import sys
import numpy as np
import math


def main():
    """GRT DoubleMovingAverageFilter Example
     This example demonstrates how to create and use the GRT DoubleMovingAverageFilter PreProcessing Module.
     
     The DoubleMovingAverageFilter implements a low pass double moving average filter. 
     
     In this example we create an instance of a DoubleMovingAverageFilter and use this to filter some
     dummy data, generated from a sine wave + random noise. The test signal and filtered signals are 
     then printed to std::cout.
     
     This example shows you how to:
     - Create a new DoubleMovingAverageFilter instance with a specific window size for a 1 dimensional signal
     - Filter some data using the DoubleMovingAverageFilter
     - Save the DoubleMovingAverageFilter settings to a file
     - Load the DoubleMovingAverageFilter settings from a file"""
    # Create a new instance of a double moving average filter with a window size of 5 for a 1 dimensional signal
    dma_filter = GRT.DoubleMovingAverageFilter(5, 1)

    # Generate some data (sine wave + noise) and filter it
    x = 0

    for i in range(1000):
        signal = math.sin(x) + np.random.uniform(-0.2, 0.2)

        filtered_value = dma_filter.filter(signal)

        print("%.3f %.3f" % (signal, filtered_value))

        x += math.tau / 1000.0 * 10.0

    # Save the filter settings to a file
    dma_filter.save("DoubleMovingAverageFilterSettings.grt")

    # We can then load the settings later if needed
    dma_filter.load("DoubleMovingAverageFilterSettings.grt")


if __name__ == '__main__':
    main()
    sys.exit(0)
