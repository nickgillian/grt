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
 GRT SavitzkyGolayFilter Example
 This example demonstrates how to create and use the GRT SavitzkyGolayFilter PreProcessing Module.
 
 The SavitzkyGolayFilter implements a Savitzky-Golay filter average filter. 
 
 In this example we create an instance of a SavitzkyGolayFilter and use this to filter some
 dummy data, generated from a series of sine waves (with increasing frequency ranging from 0.1Hz to 8Hz). 
 The test signal and filtered signals are then printed to std::cout.
 
 This example shows you how to:
 - Create a new SavitzkyGolayFilter instance with a specific left and right hand point size for a 1 dimensional signal
 - Filter some data using the SavitzkyGolayFilter
 - Save the SavitzkyGolayFilter settings to a file
 - Load the SavitzkyGolayFilter settings from a file
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Create a new instance of a SavitzkyGolayFilter filter, setting the number of left hand and right hand points to 15
    SavitzkyGolayFilter sgf(15,15);
    
    //Create some varaibles to help generate the signal data
    const UINT numSeconds = 60;                         //The number of seconds of data we want to generate
    double t = 0;                                       //This keeps track of the time
    double tStep = 1.0/1000.0;                          //This is how much the time will be updated at each iteration in the for loop
    double freq = 0;                                    //Stores the frequency
    map< UINT, double > freqRates;                      //Holds the frequency rates
    map< UINT, double >::iterator iter;                 //An iterator for the frequency rates map
    Random random;
    
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
        double signal = sin( t * TWO_PI*freq ) + random.getRandomNumberGauss(0,0.02);
        
        //Filter the signal
        double filteredValue = sgf.filter( signal );
        
        //Print the signal and the filtered data
        cout << signal << "\t" << filteredValue << endl;
        
        //Update the t
        t += tStep;
    }
    
    //Save the filter settings to a file
    sgf.save("SavitzkyGolayFilterSettings.grt");
    
    //We can then load the settings later if needed
    sgf.load("SavitzkyGolayFilterSettings.grt");
    
    return EXIT_SUCCESS;
}

