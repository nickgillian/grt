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
 GRT MedianFilter Example
 This example demonstrates how to create and use the GRT MedianFilter PreProcessing Module.
 
 The MedianFilter implements a simple median filter, this will give the value seperating the higher
 half of the most recent data from the lower half.  The filter will automatically store the most
 recent input data for you, the size of the buffer that stores the M most recent samples is controlled
 by the MedianFilter's 'window size' parameter.
 
 In this example we create an instance of a MedianFilter and use this to filter some dummy data. The test 
 signal and filtered signals are then printed to std::cout.
 
 This example shows you how to:
 - Create a new MedianFilter instance with a specific window size for a 1 dimensional signal
 - Filter some data using the MedianFilter
 - Save the MedianFilter settings to a file
 - Load the MedianFilter settings from a file
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Create a new instance of a median average filter with a window size of 5 for a 1 dimensional signal
    MedianFilter filter( 10, 1 );
    
    //Generate some data (basic counter) and filter it
    const UINT M = 100;
    Random random;
    for(UINT i=0; i<M; i++){
        
        //Filter the current value
        double filteredValue = filter.filter( i );
        
        //Get the current data in the circular buffer
        Vector< VectorFloat > data = filter.getDataBuffer();
        
        //Print the results
        cout << "input value: " << i;
        cout << "\t filtered value: " << filteredValue;
        
        cout << "\t data: [ ";
        for(unsigned int j=0; j<data[0].size(); j++){
            cout << data[0][j] << " ";
        }
        cout << "]" << endl;
        
    }
    
    //Save the filter settings to a file
    filter.save("MedianFilterSettings.grt");
    
    //We can then load the settings later if needed
    filter.load("MedianFilterSettings.grt");
    
    return EXIT_SUCCESS;
}

