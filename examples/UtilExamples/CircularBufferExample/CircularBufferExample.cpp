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
	//Create a new circular buffer for unsigned integers
	CircularBuffer< UINT > buffer;

	//Resize the buffer
	buffer.resize( 10 );
	
	//Add some values to the buffer so we fill it
	for(UINT i=0; i<buffer.getSize(); i++){
		cout << "Adding " << i << " to buffer\n";
		buffer.push_back( i );
		
		//Print the values in the buffer
		cout << "Values: \t\t";
		for(UINT j=0; j<buffer.getSize(); j++){
			cout << buffer[j] << "\t";
		}cout << endl;
		
		//Print the raw values in the buffer
		cout << "RawValues: \t\t";
		for(UINT j=0; j<buffer.getSize(); j++){
			cout << buffer(j) << "\t";
		}cout << endl;
	}
	
	//Get all the data in the buffer as a vector
	Vector< UINT > data = buffer.getData();
	cout << "Data: \t\t\t";
	for(UINT j=0; j<data.size(); j++){
		cout << data[j] << "\t";
	}
	cout << endl;
	
    return EXIT_SUCCESS;
}

