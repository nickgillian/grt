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
 This example demonstrates how to hook into the GRT training log callbacks. A good dataset to use this with can be found in grt/data/basic_linear_dataset.grt
 For example, run this from the temporary build directory via: /TrainingLogCallbacks ../../data/basic_linear_dataset.grt 
 */

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

class CallbackListener : public Observer< TrainingLogMessage >{
public:
    CallbackListener(){
        //Register the callback with the training log listener
        GRT::TrainingLog::registerObserver( *this );
    }
private:
    virtual void notify(const TrainingLogMessage &log){
        std::cout << "Custom Listener: " << log.getMessage() << std::endl;
    }
};

int main (int argc, const char * argv[])
{
    //Parse the data filename from the argument list
    if( argc != 2 ){
        cout << "Error: failed to parse data filename from command line. You should run this example with one argument pointing to the data filename!\n";
        return EXIT_FAILURE;
    }
    const string filename = argv[1];

    //Load some training data from a file
    RegressionData trainingData;
    
    if( !trainingData.load( filename ) ){
        cout << "ERROR: Failed to load training data from file\n";
        return EXIT_FAILURE;
    }

    //Create an instance of the callback listener
    CallbackListener callback;

    //Create a logistic regression instance
    LogisticRegression regression;
    
    //Train a model using the training data
    if( !regression.train( trainingData ) ){
        cout << "ERROR: Failed to train a model!\n";
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

