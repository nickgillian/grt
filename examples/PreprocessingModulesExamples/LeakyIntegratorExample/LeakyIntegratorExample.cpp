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
 GRT LeakyIntegrator Example
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
 - Load the LeakyIntegrator settings from a file
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Create a new instance of a LeakyIntegrator, using the default constructor
    LeakyIntegrator leakyIntegrator;
    
    //Set the cutoff frequency of the filter to 0.99
    leakyIntegrator.setLeakRate( 0.99 );
    
    //Create some varaibles to help generate the signal data
    double signal = 0;
    const UINT M = 100;
    
    //Generate the signal and filter the data
    for(UINT i=0; i<M; i++){
        
        //Generate the impluse if i == 10
        if( i == 10 ) signal = 1.0;
        else signal = 0;
        
        //Filter the signal
        double filteredValue = leakyIntegrator.update( signal );
        
        //Print the signal and the filtered data
        cout << signal << "\t" << filteredValue << endl;
    }
    
    //Save the HighPassFilter settings to a file
    leakyIntegrator.save("LeakyIntegratorSettings.grt");
    
    //We can then load the settings later if needed
    leakyIntegrator.load("LeakyIntegratorSettings.grt");
    
    return EXIT_SUCCESS;
}

