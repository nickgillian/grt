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

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{
    //Create a new Random instance, this will set the random seed as the current system time
	Random random;

	//Create a random integer in the range [0 999], using a uniform distribution
	int valueInt = random.getRandomNumberInt( 0, 1000 );
	cout << "Random Int: " << valueInt << endl;

	//Create a random double in the range [1.2 3.5], using a uniform distribution
	double valueDouble = random.getRandomNumberUniform( 1.2, 3.5 );
	cout << "Random Double: " << valueDouble << endl;

	//Generate a random double, using a gaussian distribution with Mu 0 and Sigma 1.0
	double valueGauss = random.getRandomNumberGauss( 0, 1 );
	cout << "Random Gaussian: " << valueGauss << endl;

	//Generate a random weighted value
	Vector< int > values(3);
	VectorFloat weights(3);

	//Set the possible values that can be picked
	values[0] = 1;
	values[1] = 2;
	values[2] = 3;

	//Set the corresponding weights (note that they do not need to sum to 1)
	weights[0] = 0.1;
	weights[1] = 0.3;
	weights[2] = 0.5;

	//Generate the weighted random value
	int randomValue = random.getRandomNumberWeighted( values, weights );
	cout << "Random Weighted Int: " << randomValue << endl;
    
    return EXIT_SUCCESS;
}

