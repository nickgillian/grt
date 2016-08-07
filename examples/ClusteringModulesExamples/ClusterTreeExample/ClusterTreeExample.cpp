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
    InfoLog log("ClusterTreeExample");
    
    //Load some training data to train the ClusterTree model
    MatrixDouble trainingData;
    if( !trainingData.load("ClusterTreeData.csv") ){
        log << "Failed to load training data!" << endl;
        return EXIT_FAILURE;
    }
    
    //Create a new ClusterTree instance
    ClusterTree ctree;
    
    //Set the number of steps that will be used to choose the best splitting values
    //More steps will give you a better model, but will take longer to train
    ctree.setNumSplittingSteps( 100 );
    
    //Set the maximum depth of the tree
    ctree.setMaxDepth( 10 );
    
    //Set the minimum number of samples allowed per node
    ctree.setMinNumSamplesPerNode( 10 );
    
    //Set the minimum RMS error allowed per node
    ctree.setMinRMSErrorPerNode( 0.1 );
	
    //Train a cluster tree model
    if( !ctree.train( trainingData ) ){
        log << "Failed to train model!" << endl;
        return EXIT_FAILURE;
    }
    
    if( !ctree.save("Model.grt") ){
        log << "Failed to train model!" << endl;
        return EXIT_FAILURE;
    }
	
    if( !ctree.load("Model.grt") ){
        log << "Failed to train model!" << endl;
        return EXIT_FAILURE;
    }
    
    //Print the tree
    ctree.print();
	
    return EXIT_SUCCESS;
}
