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

//Run this example with some command line options like:
// ./CommandLineParserExample -i 5 -s hi

int main(int argc, char * argv[])
{
    //Create an instance of the parser
    CommandLineParser parser;

    //Add some options and identifiers that can be used to get the results
    parser.addOption( "-i", "test-number" );
    parser.addOption( "-s", "test-string" );

    //Parse the command line
    parser.parse( argc, argv );

    //Retrieve the results you need
    int testNumber = 0;
    string testString = "";

    //Use the 'test-number' identifier to retrieve the number
    parser.get( "test-number", testNumber );

    //Use the 'test-string' identifier to retrieve the string
    parser.get( "test-string", testString );

    cout << "parsed number as: " <<testNumber << endl;
    cout << "parsed string as: " << testString << endl;

    return EXIT_SUCCESS;
}

