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
 GRT DoubleMovingAverageFilter Example
 This example demonstrates how to create and use the GRT DoubleMovingAverageFilter PreProcessing Module.
 
 The DoubleMovingAverageFilter implements a low pass double moving average filter. 
 
 In this example we create an instance of a DoubleMovingAverageFilter and use this to filter some
 dummy data, generated from a sine wave + random noise. The test signal and filtered signals are 
 then printed to std::cout.
 
 This example shows you how to:
 - Create a new DoubleMovingAverageFilter instance with a specific window size for a 1 dimensional signal
 - Filter some data using the DoubleMovingAverageFilter
 - Save the DoubleMovingAverageFilter settings to a file
 - Load the DoubleMovingAverageFilter settings from a file
*/

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Create a new instance of a double moving average filter with a window size of 5 for a 1 dimensional signal
    DoubleMovingAverageFilter filter( 5, 1 );
    
    //Generate some data (sine wave + noise) and filter it
    double x = 0;
    const UINT M = 1000;
    Random random;
    for(UINT i=0; i<M; i++){
        double signal = sin( x ) + random.getRandomNumberUniform(-0.2,0.2);
        
        double filteredValue = filter.filter( signal );
        
        cout << signal << "\t" << filteredValue << endl;
        
        x += TWO_PI/double(M)*10;
    }
    
    //Save the filter settings to a file
    filter.saveModelToFile("DoubleMovingAverageFilterSettings.txt");
    
    //We can then load the settings later if needed
    filter.loadModelFromFile("DoubleMovingAverageFilterSettings.txt");
    
    return EXIT_SUCCESS;
}
