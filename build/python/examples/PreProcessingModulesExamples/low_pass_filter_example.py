import GRT
import sys
import numpy as np
import math


def main():
    """GRT LowPassFilter Example
     This example demonstrates how to create and use the GRT LowPassFilter PreProcessing Module.

     The LowPassFilter implements a first-order infinite-impulse-response (IIR) low pass filter.

     In this example we create an instance of a LowPassFilter and use this to filter some
     dummy data, generated from a series of sine waves (with increasing frequency ranging from 0.1Hz to 8Hz).
     The test signal and filtered signals are then printed to std::cout.

     This example shows you how to:
     - Create a new LowPassFilter instance a 1 dimensional signal
     - Set a specific filter cutoff frequency
     - Filter some data using the LowPassFilter
     - Save the LowPassFilter settings to a file
     - Load the LowPassFilter settings from a file"""
    # Create a new instance of a high pass filter, using the default constructor
    lpf = GRT.LowPassFilter()

    # Set the cutoff frequency of the filter to 2.0Hz
    lpf.setCutoffFrequency(2.0, 1.0 / 1000.0)

    # Create some variables to help generate the signal data
    num_seconds = 6  # The number of seconds of data we want to generate
    t = 0  # This keeps track of the time
    t_step = 1.0 / 1000.0  # This is how much the time will be updated at each iteration in the for loop

    # Add the freq rates
    # The first value is the time in seconds and the second value is the frequency that should be set at that time
    freq_rates = {0: 0.1, 1: 0.5, 2: 1, 3: 2, 4: 4, 5: 8, 6: 32}

    # Generate the signal and filter the data
    for i in range(num_seconds * 1000):
        # Check to see if we should update the freq rate to the next value
        # Set the new frequency value
        freq = [v for (k, v) in freq_rates.items() if k > (i / 1000)][0]

        # Generate the signal
        signal = math.sin(t * math.tau * freq)

        # Filter the signal
        filtered_value = lpf.filter(signal)

        # Print the signal and the filtered data
        print("f=%.3f: %.3f \t %.3f" % (freq, signal, filtered_value))

        # Update the t
        t += t_step

    # Save the HighPassFilter settings to a file
    lpf.save("LowPassFilterSettings.grt")

    # We can then load the settings later if needed
    lpf.load("LowPassFilterSettings.grt")


if __name__ == '__main__':
    main()
    sys.exit(0)
