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
 GRT Derivative Example
 This example demonstrates how to create and use the GRT Derivative PreProcessing Module.
 
 The Derivative module computes either the first or second order derivative of the input signal. 
 
 In this example we create two Derivative instances, one to compute the first derivative of the
 input signal and another to compute the second derivative of a 1-dimensional input signal.
 
 We then create a sine-wave signal as the input signal to both Derivative instances, the sine signal
 and first and second derivative signals are then output to std::cout.
 
 This example shows you how to:
 - Create a new Derivative instance to compute either the first or second derivative
 - Compute the (1st and 2nd) derivative of a sine-wave signal
 - Save the Derivative settings to a file
 - Load the Derivative settings from a file
*/

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{

    //Create a new Derivative instance and set it to compute the first order derivative
    Derivative deriv1(Derivative::FIRST_DERIVATIVE);
    
    //Create another new Derivative instance and set it to compute the second order derivative
    Derivative deriv2(Derivative::SECOND_DERIVATIVE);
    
    //Generate some data (sine wave) and compute the derivative of this
    double x = 0;
    const UINT M = 1000;
    Random random;
    for(UINT i=0; i<M; i++){
        double signal = sin( x );
        
        double deriv1Value = deriv1.computeDerivative( signal );
        double deriv2Value = deriv2.computeDerivative( signal );
        
        cout << signal << "\t" <<deriv1Value << "\t" << deriv2Value << endl;
        
        x += TWO_PI/double(M)*10;
    }
    
    //Save the derivative settings to a file
    deriv1.saveModelToFile("Derivative1Settings.txt");
    deriv2.saveModelToFile("Derivative2Settings.txt");
    
    //We can then load the settings later if needed
    deriv1.loadModelFromFile("Derivative1Settings.txt");
    deriv2.loadModelFromFile("Derivative2Settings.txt");

    return EXIT_SUCCESS;
}
