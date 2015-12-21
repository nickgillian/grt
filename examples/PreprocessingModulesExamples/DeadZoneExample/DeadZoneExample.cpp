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
 GRT DeadZone Example
 This example demonstrates how to create and use the GRT DeadZone PreProcessing Module.
 
 The DeadZone module sets any values in the input signal that fall within the dead-zone region to zero. 
 Any values outside of the dead-zone region will be offset by the dead zone's lower limit and upper limit. 
 
 In this example we create a DeadZone with a dead-zone region of [-0.2 0.2] for a 1 dimensional signal, we
 then generate some dummy data and run this data through the DeadZone.  The dummy signal and dead-zone filtered
 signal are then printed to std::cout.
 
 This example shows you how to:
 - Create a new DeadZone instance
 - Filter some dummy data using the DeadZone
 - Save the DeadZone instances' settings to a file
 - Load the DeadZone instances' settings from a file
*/

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Create a new DeadZone instance 
    //Set the lower limit to -0.2, and upper limit of 2.0
    //Set the dimensionality of the input signal to 1
    DeadZone deadZone(-0.2,0.2,1);
    
    //Create some dummy data and filter it using the dead zone
    Random random;
    UINT M = 1000;
    UINT signalCounter = 0;
    
    for(UINT i=0; i<M; i++){
        
        //Generate the signal
        double signal = 0;
        if( signalCounter < 50 ){
            signal = random.getRandomNumberUniform(-0.15,0.15);
        }else{
            if( signalCounter >= 50 && signalCounter < 100 ){
                signal = 1.0;
            }else{
                signal = -1.0;
            }
        }
        
        if( ++signalCounter >= 150 ) signalCounter = 0;
        
        //Filter the signal using the dead zone
        double filteredSignal = deadZone.filter( signal );
        
        cout << signal << "\t" << filteredSignal << endl;
    }
    
    //If we need to save the deadzone settings to a file then we can
    deadZone.save("DeadZoneSettings.grt");
    
    //We can then load the settings later if needed
    deadZone.load("DeadZoneSettings.grt");
    
    return EXIT_SUCCESS;
}

