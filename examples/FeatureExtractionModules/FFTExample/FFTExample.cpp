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

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Create a new instance of an FFT with a window size of 256 and a hop size of 1
    FFT fft(256,1);
    
    //Create some varaibles to help generate the signal data
    const UINT numSeconds = 10;                         //The number of seconds of data we want to generate
    double t = 0;                                       //This keeps track of the time
    double tStep = 1.0/1000.0;                          //This is how much the time will be updated at each iteration in the for loop
    double freq = 100;                                  //Stores the frequency
    
    //Generate the signal and filter the data
    for(UINT i=0; i<numSeconds*1000; i++){
        
        //Generate the signal
        double signal = sin( t * TWO_PI*freq );
        
        //Compute the FFT of the input signal (and the previous buffer data)
        fft.update( signal );
        
        //Update the t
        t += tStep;
    }
    
    //Take the output of the last FFT and save the values to a file
    vector<FastFourierTransform> fftResults = fft.getFFTResults();
    
    //The input signal is a 1 dimensional signal, so get the magnitude data for dimension 1 (which is at element 0)
    VectorDouble magnitudeData = fftResults[0].getMagnitudeData();
    
    //Write the magnitude data to a file
    cout << "Magnitude Data:\n";
    for(UINT i=0; i<magnitudeData.size(); i++){
        cout << magnitudeData[i] << endl;
    }
    
    return EXIT_SUCCESS;
    
}

