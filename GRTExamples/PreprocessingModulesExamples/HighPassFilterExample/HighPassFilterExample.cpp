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

/*
 GRT HighPassFilter Example
 This example demonstrates how to create and use the GRT HighPassFilter PreProcessing Module.
 
 The HighPassFilter implements a first-order infinite-impulse-response (IIR) high pass filter. 
 
 In this example we create an instance of a HighPassFilter and use this to filter some
 dummy data, generated from a series of sine waves (with increasing frequency ranging from 0.1Hz to 8Hz). 
 The test signal and filtered signals are then printed to std::cout.
 
 This example shows you how to:
 - Create a new HighPassFilter instance a 1 dimensional signal
 - Set a specific filter cutoff frequency
 - Filter some data using the HighPassFilter
 - Save the HighPassFilter settings to a file
 - Load the HighPassFilter settings from a file
*/

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Create a new instance of a high pass filter, using the default constructor
    HighPassFilter hpf;
    
    //Set the cutoff frequency of the filter to 2.0Hz
    hpf.setCutoffFrequency( 2, 1.0/1000.0);
    
    //Create some varaibles to help generate the signal data
    const UINT numSeconds = 60;                         //The number of seconds of data we want to generate
    double t = 0;                                       //This keeps track of the time
    double tStep = 1.0/1000.0;                          //This is how much the time will be updated at each iteration in the for loop
    double freq = 0;                                    //Stores the frequency
    map< UINT, double > freqRates;                      //Holds the frequency rates
    map< UINT, double >::iterator iter;                 //An iterator for the frequency rates map
    
    //Add the freq rates
    //The first value is the time in seconds and the second value is the frequency that should be set at that time
    freqRates[ 0 ] = 0.1;
    freqRates[ 10 ] = 0.5;
    freqRates[ 20 ] = 1;
    freqRates[ 30 ] = 2;
    freqRates[ 40 ] = 4;
    freqRates[ 50 ] = 8;
    
    //Generate the signal and filter the data
    for(UINT i=0; i<numSeconds*1000; i++){
        
        //Check to see if we should update the freq rate to the next value
        iter = freqRates.find( i/1000 );
        if( iter != freqRates.end() ){
            //Set the new frequency value
            freq = iter->second;
        }
        
        //Generate the signal
        double signal = sin( t * TWO_PI*freq );
        
        //Filter the signal
        double filteredValue = hpf.filter( signal );
        
        //Print the signal and the filtered data
        cout << signal << "\t" << filteredValue << endl;
        
        //Update the t
        t += tStep;
    }
    
    //Save the HighPassFilter settings to a file
    hpf.saveModelToFile("HighPassFilterSettings.txt");
    
    //We can then load the settings later if needed
    hpf.loadModelFromFile("HighPassFilterSettings.txt");
    
    return EXIT_SUCCESS;
}

