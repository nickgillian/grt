import GRT
import sys
import math

def main():
    # Create a new instance of an FFT with a window size of 256 and a hop size of 1
    fft = GRT.FFT (256,1)
    
    # Create some varaibles to help generate the signal data
    numSeconds = 10      # The number of seconds of data we want to generate
    t = 0.0              # This keeps track of the time
    tStep = 1.0/1000.0   # This is how much the time will be updated at each iteration in the for loop
    freq = 100.0         # Stores the frequency
    
    # Generate the signal and filter the data
    for i in range(numSeconds*1000):
    
        # Generate the signal
        signal = math.sin( t * math.tau * freq )

        # Compute the FFT of the input signal (and the previous buffer data)
        fft.update( signal )

        # Update the t
        t += tStep

    
    # Take the output of the last FFT and save the values to a file
    fftResults = fft.getFFTResults()

    # The input signal is a 1 dimensional signal, so get the magnitude data for dimension 1 (which is at element 0)
    magnitudeData = fftResults[0].getMagnitudeData()
    
    # Write the magnitude data
    print("Magnitude Data: " + str(magnitudeData))


if __name__ == '__main__':
    main()
    sys.exit(0)