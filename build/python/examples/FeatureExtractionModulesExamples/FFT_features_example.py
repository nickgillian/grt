import GRT
import sys
import math

def main():
    # Create a new instance of an FFT with a window size of 256 and a hop size of 1 for a 1 dimensional signal
    # Note, it is important that when you use an FFT with the FFTFeatures module that the FFT module computes the magnitude, but does not compute the phase
    fftWindowSize = 256
    fftHopSize = 1
    numInputDimensions = 1
    computeMagnitude = True
    computePhase = False
    fft = GRT.FFT(fftWindowSize,fftHopSize,numInputDimensions,GRT.FFT.RECTANGULAR_WINDOW,computeMagnitude,computePhase)

    # Create a new fftFeatures instance and pass in the size of the FFT window and the number of input dimensions to the FFT instance
    fftFeatures = GRT.FFTFeatures(int(fft.getFFTWindowSize()/2),numInputDimensions)

    # Create some varaibles to help generate the signal data
    numSeconds = 10          # The number of seconds of data we want to generate
    t = 0.0                  # This keeps track of the time
    tStep = 1.0/1000.0       # This is how much the time will be updated at each iteration in the for loop
    freq = 100.0             # Stores the frequency

    # Generate the signal and filter the data
    for i in range(numSeconds*1000):

        # Generate the signal
        signal = math.sin( t * math.tau * freq )
    
        # Compute the FFT of the input signal
        fft.update( signal )
    
        # Compute the features from the FFT
        fftFeatures.computeFeatures( fft.getFeatureVector() )
    
        # Get the feature vector from the FFT features instance and print the values
        print(fftFeatures.getFeatureVector())

        # Update the t
        t += tStep
        
    
    # Save the settings to a file
    if not fftFeatures.save( "FFTFeatures.grt" ):
        print("Error: Failed to save fft features to settings file!")
        sys.exit(1)
    

    # Load the setting back from a file again
    if not fftFeatures.load( "FFTFeatures.grt" ):
        print("Error: Failed to load fft features from settings file!")
        sys.exit(1)


if __name__ == '__main__':
    main()
    sys.exit(0)