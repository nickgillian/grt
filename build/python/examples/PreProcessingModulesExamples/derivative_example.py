import GRT
import sys
import numpy as np
import math

def main():
    """GRT Derivative Example
     This example demonstrates how to create and use the GRT Derivative PreProcessing Module.
     
     The Derivative module computes either the first or second order derivative of the input signal. 
     
     In this example we create two Derivative instances, one to compute the first derivative of the
     input signal and another to compute the second derivative of a 1-dimensional input signal.
     
     We then create a sine-wave signal as the input signal to both Derivative instances, the sine signal
     and first and second derivative signals are then output to std::cout.
     
     This example shows you how to:
     - Create a new Derivative instance to compute either the first or second derivative
     - Compute the (1st and 2nd) derivative of a sine-wave signal
     - Save the Derivative settings to a file
     - Load the Derivative settings from a file"""
    # Create a new Derivative instance and set it to compute the first order derivative
    deriv1 = GRT.Derivative(GRT.Derivative.FIRST_DERIVATIVE)
    
    # Create another new Derivative instance and set it to compute the second order derivative
    deriv2 = GRT.Derivative(GRT.Derivative.SECOND_DERIVATIVE)
    
    # Generate some data (sine wave) and compute the derivative of this
    x = 0

    for i in range(1000):
        signal = math.sin( x )
        deriv1_value = deriv1.computeDerivative( signal )
        deriv2_value = deriv2.computeDerivative( signal )

        print("%.3f \t %.3f \t %.3f" % (signal, deriv1_value, deriv2_value))

        x += (math.tau / 1000.0) * 10.0

    # Save the derivative settings to a file
    deriv1.save("Derivative1Settings.grt")
    deriv2.save("Derivative2Settings.grt")
    
    # We can then load the settings later if needed
    deriv1.load("Derivative1Settings.grt")
    deriv2.load("Derivative2Settings.grt")


if __name__ == '__main__':
    main()
    sys.exit(0)
