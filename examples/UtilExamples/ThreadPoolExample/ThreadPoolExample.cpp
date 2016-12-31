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

 This example demonstrates how to the GRT ThreadPool class to run multiple tasks in parrell. To use this example,
 you need to compile the GRT with C++11 support.
*/

#include "GRT.h"
using namespace GRT;

InfoLog infoLog;

void task( const unsigned int numSteps, double &bestValue ){
	infoLog << "starting task..." << std::endl;
	Random random;
	bestValue = 0;
	double value = 0;
	for(unsigned int i=0; i<numSteps; i++){
		value = random.getRandomNumberUniform( 0.0, 1.0 );
		if( value > bestValue ) bestValue = value;
	}
	infoLog << "end of task, maxValue: " << bestValue << std::endl;
}

int main (int argc, const char * argv[])
{
	//Create a new thread pool
	ThreadPool pool;

	//Get the number of threads, this defaults to the number of threads on your machine
	const unsigned int numThreads = pool.getThreadPoolSize();

	infoLog << "num threads: " << numThreads << std::endl;

	//Set the number of tasks that we want to pass to the thread pool
	const unsigned int numTasks = 100;

	//Setup a vector to store the results
	std::vector< double > results( numTasks );
	std::vector< std::future< void > > status( numTasks );

	//Add some tasks to the pool
	for(unsigned int i=0; i<numTasks; i++){
		status[i] = pool.enqueue( task, 99999, results[i] );
	}

	//Wait for the tasks to complex
	for(unsigned int i=0; i<numTasks; i++){
		status[i].wait();
	}

	//Get the maximum value across all the task results
	double maxValue = results[0];
	for(unsigned int i=1; i<numTasks; i++){
		if( results[i] > maxValue ) maxValue = results[i];
	}

	infoLog << "maximum value: " << maxValue << std::endl;
	
    return EXIT_SUCCESS;
}

