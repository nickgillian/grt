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
 GRT KMeans Example
 This examples demonstrates how to use the KMeans module for unsupervised clustering.

 The LabelledClusterData.csv data contains 1000 training samples, randomly generated from 3 clusters.
 The 3 clusters are centered at:
 1: 2 6
 2: 6 4
 3: 7 8

 
*/

//You might need to set the specific path of the GRT header relative to your project
#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    //Create a new KMeans instance
    KMeans kmeans;
    kmeans.setComputeTheta( true );
    kmeans.setMinChange( 1.0e-10 );
    kmeans.setMinNumEpochs( 10 );
	kmeans.setMaxNumEpochs( 10000 );

	//There are a number of ways of training the KMeans algorithm, depending on what you need the KMeans for
	//These are:
	//- with labelled training data (in the ClassificationData format)
	//- with unlablled training data (in the UnlabelledData format)
	//- with unlabelled training data (in a simple MatrixDouble format)
	
	//This example shows you how to train the algorithm with ClassificationData
	
	//Load some training data to train the KMeans algorithm
    ClassificationData trainingData;
    
    if( !trainingData.loadDatasetFromCSVFile("LabelledClusterData.csv") ){
        cout << "Failed to load training data!\n";
        return EXIT_FAILURE;
    }
	
    //Train the KMeans algorithm - K will automatically be set to the number of classes in the training dataset
    if( !kmeans.train( trainingData ) ){
        cout << "Failed to train model!\n";
        return EXIT_FAILURE;
    }
	
	//Get the K clusters from the KMeans instance and print them
	cout << "\nClusters:\n";
	Matrix< double > clusters = kmeans.getClusters();
    for(unsigned int k=0; k<clusters.getNumRows(); k++){
		for(unsigned int n=0; n<clusters.getNumCols(); n++){
			cout << clusters[k][n] << "\t";
		}cout << endl;
	}
	
    return EXIT_SUCCESS;
}
