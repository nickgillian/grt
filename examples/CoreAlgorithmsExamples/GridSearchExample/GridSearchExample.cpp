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
 This example demonstrates how to use the GridSearch module, this example requires the GRT to be built with C++ 11 support.
 */

//You might need to set the specific path of the GRT header relative to your project
#include <GRT/GRT.h>
using namespace GRT;
using namespace std;

int main (int argc, const char * argv[])
{

#ifdef GRT_CXX11_ENABLED

    //Create a new classifier which we can tune using GridSearch
    GestureRecognitionPipeline pipeline;
    pipeline.setClassifier( RandomForests() );
    RandomForests *rf = pipeline.getClassifier< RandomForests >();

    //Generate some dummy training data that can be used for training the model
    ClassificationData::generateGaussDataset( "data.csv", 100000, 10, 10, 1.0, 0.3 );
    ClassificationData trainingData, testData;
    trainingData.load( "data.csv" );
    testData = trainingData.split( 50 );

    //Setup grid search
    GridSearch< GestureRecognitionPipeline > gridSearch;

    //Add the ranges that will be searched
    GridSearchRange<unsigned int> forestSizeRange(1,100,5); //Range will iterate between 1 and 100 in steps of 5
    GridSearchRange<unsigned int> maxDepthRange(1,10,1); //Max depth will iterate between 1 and 10 in steps of 1
    GridSearchRange<unsigned int> minSamplesPerNodeRange(100,1000,100); //Min samples per node will iterate between 100 and 1000 in steps of 100

    //Add the parameters we want to tune
    gridSearch.addParameter( std::bind( &RandomForests::setForestSize, rf, std::placeholders::_1 ), forestSizeRange );
    gridSearch.addParameter( std::bind( &RandomForests::setMaxDepth, rf, std::placeholders::_1 ), maxDepthRange );
    gridSearch.addParameter( std::bind( &RandomForests::setMinNumSamplesPerNode, rf, std::placeholders::_1 ), minSamplesPerNodeRange );

    //Setup the evaluation function that will be used to tune the parameters
    //The evalution function must return the value that will be used for optimization
    //For this grid search, we use the accuracy on the test dataset
    gridSearch.setEvaluationFunction( [&](){ 
        cout << "- forest size: " << rf->getForestSize() << endl;
        cout << "- max depth: " << rf->getMaxDepth() << endl;
        pipeline.train( trainingData ); 
        pipeline.test( testData ); 
        gridSearch.setModel( pipeline );
        return pipeline.getTestAccuracy(); 
    } );
    
    //Start the search
    gridSearch.search();

    //Get the search results
    double bestResult = gridSearch.getBestResult();

    //Get the best pipeline
    pipeline = gridSearch.getBestModel();

    //Get the random forest classifier from the pipeline and print the parameters we were tuning
    rf = pipeline.getClassifier< RandomForests >();
    cout << "Best Result: " << bestResult << endl;
    cout << "- forest size: " << rf->getForestSize() << endl;
    cout << "- max depth: " << rf->getMaxDepth() << endl;

    return EXIT_SUCCESS;

#endif // GRT_CXX11_ENABLED
   
    return EXIT_FAILURE;
}
