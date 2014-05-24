/*
 GRT MIT License
 Copyright (c) <2012> <Nicholas Gillian, Media Lab, MIT>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial
 portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

//Include the GRT header, you may have to change the path of this to match where the GRT is on your system
#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Create a new instance of an FFT with a window size of 256 and a hop size of 1 for a 1 dimensional signal
    //Note, it is important that when you use an FFT with the FFTFeatures module that the FFT module computes the magnitude, but does not compute the phase
    UINT fftWindowSize = 256;
    UINT fftHopSize = 1;
    UINT numInputDimensions = 1;
    bool computeMagnitude = true;
    bool computePhase = false;
    FFT fft(fftWindowSize,fftHopSize,numInputDimensions,FFT::RECTANGULAR_WINDOW,computeMagnitude,computePhase);

    //Create a new fftFeatures instance and pass in the size of the FFT window and the number of input dimensions to the FFT instance
    FFTFeatures fftFeatures(fft.getFFTWindowSize()/2,numInputDimensions);

    //Create some varaibles to help generate the signal data
    const UINT numSeconds = 10;                         //The number of seconds of data we want to generate
    double t = 0;                                       //This keeps track of the time
    double tStep = 1.0/1000.0;                          //This is how much the time will be updated at each iteration in the for loop
    double freq = 100;                                    //Stores the frequency

    //Generate the signal and filter the data
    for(UINT i=0; i<numSeconds*1000; i++){

        //Generate the signal
        double signal = sin( t * TWO_PI*freq );

        //Compute the FFT of the input signal
        fft.update( signal );

        //Compute the features from the FFT
        fftFeatures.computeFeatures( fft.getFeatureVector() );

        //Get the feature vector from the FFT features instance and print the values
        vector< double > featureVector = fftFeatures.getFeatureVector();
        for(UINT j=0; j<featureVector.size(); j++){
                cout << featureVector[j] << "\t";
        }
        cout << "\n";

        //Update the t
        t += tStep;
    }
    
    //Save the settings to a file
    if( !fftFeatures.saveModelToFile( "FFTFeatures.txt" ) ){
        cout << "Error: Failed to save fft features to settings file!" << endl;
        return EXIT_FAILURE;
    }
    
    //Load the setting back from a file again
    if( !fftFeatures.loadModelFromFile( "FFTFeatures.txt" ) ){
        cout << "Error: Failed to load fft features from settings file!" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}