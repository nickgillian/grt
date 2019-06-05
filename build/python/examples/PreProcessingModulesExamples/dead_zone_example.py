import GRT
import sys
import numpy as np


def main():
    """GRT DeadZone Example
     This example demonstrates how to create and use the GRT DeadZone PreProcessing Module.

     The DeadZone module sets any values in the input signal that fall within the dead-zone region to zero.
     Any values outside of the dead-zone region will be offset by the dead zone's lower limit and upper limit.

     In this example we create a DeadZone with a dead-zone region of [-0.2 0.2] for a 1 dimensional signal, we
     then generate some dummy data and run this data through the DeadZone.  The dummy signal and dead-zone filtered
     signal are then printed to std::cout.

     This example shows you how to:
     - Create a new DeadZone instance
     - Filter some dummy data using the DeadZone
     - Save the DeadZone instances' settings to a file
     - Load the DeadZone instances' settings from a file"""
    # Create a new DeadZone instance
    # Set the lower limit to -0.2, and upper limit of 2.0
    # Set the dimensionality of the input signal to 1
    dead_zone = GRT.DeadZone(-0.2, 0.2, 1)

    # Create some dummy data and filter it using the dead zone
    signal_counter = 0

    for i in range(1000):

        # Generate the signal
        if signal_counter < 50:
            signal = np.random.uniform(-0.15, 0.15)
        else:
            if 50 <= signal_counter < 100:
                signal = 1.0
            else:
                signal = -1.0

        signal_counter += 1
        if signal_counter >= 150: signal_counter = 0

        # Filter the signal using the dead zone
        filtered_signal = dead_zone.filter(signal)

        print("%.3f \t %.3f" % (signal, filtered_signal))

    # If we need to save the deadzone settings to a file then we can
    dead_zone.save("DeadZoneSettings.grt")

    # We can then load the settings later if needed
    dead_zone.load("DeadZoneSettings.grt")


if __name__ == '__main__':
    main()
    sys.exit(0)
