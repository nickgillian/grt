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

/**
 GRT Util Example.

 This example demonstrates how to use some of the GRT Util functions.  

 The Util class is a wrapper for a number of generic functions that are used throughout the GRT.  This includes 
 functions for scaling data, finding the minimum or maximum values in a double or UINT vector, etc.  Many of these 
 functions are static functions, which enables you to use them without having to create a new Util instance, for 
 instance, you can directly call: Util::sleep( 1000 ); to use the sleep function.  
*/

#include "GRT.h"
using namespace GRT;

void print(const string &info,const vector< double > &d){
	cout << info << " ";
	for(size_t i=0; i<d.size(); i++)
		cout << d[i] << "\t";
	cout << endl;
}

int main (int argc, const char * argv[])
{
	//Test the sleep function
	cout << "Sleeping for 1 second...\n";
	Util::sleep( 1000 );
	cout << "Done.\n";

	//Scale the value from the input range to the source range
	double scaleResult = Util::scale(0.5,0,1,0,1000);
	cout << "Scaled value: " << scaleResult << endl;
	
	//Convert the values to a string
	cout << "Value as string: " << Util::toString( 1 ) << endl;
	cout << "Value as string: " << Util::toString( 1.2345 ) << endl;
	cout << "Value as string: " << Util::toString( true ) << endl;
	
	//Convert the strings back to values
	cout << "String as value: " << Util::stringToInt( "1" ) << endl;
	cout << "String as value: " << Util::stringToDouble( "1.2345" ) << endl;
	cout << "String as value: " << Util::stringToBool( "true" ) << endl;
	
	//Perform some functions on the following vector
	vector< double > data;
	data.push_back(1.0); 
	data.push_back(1.4); 
	data.push_back(2.5); 
	data.push_back(4.9); 
	data.push_back(5.2);
	
	//Scale the data 
	vector< double > scaledData = Util::scale(data,1,5,0,1);
	print( "ScaledData:", scaledData);
	
	//Normalize the data
	vector< double > normData = Util::normalize(data);
	print( "NormData:", normData);
	
	//Limit the data
	vector< double > limitData = Util::scale(data,2,4);
	print( "LimitData:", limitData);
	
	//Get the sum, min and max values
	double sumValue = Util::sum(data);
	double minValue = Util::getMin(data);
	double maxValue = Util::getMax(data);
	cout << "Sum: " << sumValue << " MinValue: " << minValue << " MaxValue: " << maxValue << endl;
	
    return EXIT_SUCCESS;
}

