import GRT
import sys
import numpy as np
import math


def main():
    """GRT LeakyIntegrator Example
     This example demonstrates how to create and use the GRT LeakyIntegrator PreProcessing Module.
     
     The LeakyIntegrator implements a basic leaky integrator filter that computes: y = y*z + x
     where y is the output of the filter, x is the input and z is the leak rate.
     
     The leak rate sets how quickly the filter will leak the last input, with a value very close to 1.0 resulting 
     in a slow leak time and a value less than 1.0 resulting in a faster leak time.
     
     The leak rate value should be in the range of [0 1], with values of 0.99 being common for most purposes.
     
     In this example we create an instance of a LeakyIntegrator and use this to filter some
     dummy data, generated from a simple impluse generator. 
     The test signal and filtered signals are then printed to std::cout.
     
     This example shows you how to:
     - Create a new LeakyIntegrator instance a 1 dimensional signal
     - Set a specific leak rate
     - Filter some data using the LeakyIntegrator
     - Save the LeakyIntegrator settings to a file
     - Load the LeakyIntegrator settings from a file"""

    # Create a new instance of a LeakyIntegrator, using the default constructor
    leaky_integrator = GRT.LeakyIntegrator()

    # Set the cutoff frequency of the filter to 0.99
    leaky_integrator.setLeakRate(0.99)

    # Generate the signal and filter the data
    for i in range(100):
        # Generate the impluse if i == 10
        signal = 1.0 if i == 10 else 0

        # Filter the signal
        filtered_value = leaky_integrator.update(signal)

        # Print the signal and the filtered data
        print("%.3f %.3f" % (signal, filtered_value))

    # Save the HighPassFilter settings to a file
    leaky_integrator.save("LeakyIntegratorSettings.grt")

    # We can then load the settings later if needed
    leaky_integrator.load("LeakyIntegratorSettings.grt")


if __name__ == '__main__':
    main()
    sys.exit(0)
