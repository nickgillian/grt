import GRT
import sys
import numpy as np
import math


def main():
    """GRT MovingAverageFilter Example
     This example demonstrates how to create and use the GRT MovingAverageFilter PreProcessing Module.
     
     The MovingAverageFilter implements a low pass moving average filter. 
     
     In this example we create an instance of a MovingAverageFilter and use this to filter some
     dummy data, generated from a sine wave + random noise. The test signal and filtered signals are 
     then printed to std::cout.
     
     This example shows you how to:
     - Create a new MovingAverageFilter instance with a specific window size for a 1 dimensional signal
     - Filter some data using the MovingAverageFilter
     - Save the MovingAverageFilter settings to a file
     - Load the MovingAverageFilter settings from a file"""
    # Create a new instance of a moving average filter with a window size of 5 for a 1 dimensional signal
    filter = GRT.MovingAverageFilter(5, 1)

    # Generate some data (sine wave + noise) and filter it
    x = 0
    for i in range(1000):
        signal = math.sin(x) + np.random.uniform(-0.2, 0.2)

        filtered_value = filter.filter(signal)

        print("%.3f %.3f" % (signal, filtered_value))

        x += math.tau / 1000.0 * 10.0

    # Save the filter settings to a file
    filter.save("MovingAverageFilterSettings.grt")

    # We can then load the settings later if needed
    filter.load("MovingAverageFilterSettings.grt")


if __name__ == '__main__':
    main()
    sys.exit(0)
