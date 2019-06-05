import GRT
import sys
import numpy as np
import math


def main():
    """GRT SavitzkyGolayFilter Example
     This example demonstrates how to create and use the GRT SavitzkyGolayFilter PreProcessing Module.

     The SavitzkyGolayFilter implements a Savitzky-Golay filter average filter.

     In this example we create an instance of a SavitzkyGolayFilter and use this to filter some
     dummy data, generated from a series of sine waves (with increasing frequency ranging from 0.1Hz to 8Hz).
     The test signal and filtered signals are then printed to std::cout.

     This example shows you how to:
     - Create a new SavitzkyGolayFilter instance with a specific left and right hand point size for a 1 dimensional signal
     - Filter some data using the SavitzkyGolayFilter
     - Save the SavitzkyGolayFilter settings to a file
     - Load the SavitzkyGolayFilter settings from a file"""
    # Create a new instance of a SavitzkyGolayFilter filter, setting the number of left hand and right hand points to 15
    sgf = GRT.SavitzkyGolayFilter(15, 15)

    # Create some variables to help generate the signal data
    num_seconds = 6  # The number of seconds of data we want to generate
    t = 0  # This keeps track of the time
    t_step = 1.0 / 1000.0  # This is how much the time will be updated at each iteration in the for loop

    # Add the freq rates
    # The first value is the time in seconds and the second value is the frequency that should be set at that time
    freq_rates = {0: 0.1, 1: 0.5, 2: 1, 3: 2, 4: 4, 5: 8, 6: 16}

    # Generate the signal and filter the data
    for i in range(num_seconds * 1000):
        # Check to see if we should update the freq rate to the next value
        # Set the new frequency value
        freq = [v for (k, v) in freq_rates.items() if k > (i / 1000)][0]

        # Generate the signal
        signal = math.sin(t * math.tau * freq)

        # Filter the signal
        filtered_value = sgf.filter(signal)

        # Print the signal and the filtered data
        print("%.3f %.3f %.3f" % (freq, signal, filtered_value))

        # Update the t
        t += t_step

    # Save the HighPassFilter settings to a file
    sgf.save("SavitzkyGolayFilterSettings.grt")

    # We can then load the settings later if needed
    sgf.load("SavitzkyGolayFilterSettings.grt")


if __name__ == '__main__':
    main()
    sys.exit(0)
