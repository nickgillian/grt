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
 This example demonstrates how to use the GridSearch module. 
 */

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{

    //Create a new classifier which we can tune using GridSearch
    GestureRecognitionPipeline pipeline;
    pipeline.setClassifier( RandomForests() );
    RandomForests *rf = pipeline.getClassifier< RandomForests >();

    //Generate some dummy training data that can be used for training the model
    ClassificationData::generateGaussDataset( "data.csv", 100000, 10, 10, 1.0, 0.3 );
    ClassificationData trainingData, testData;
    trainingData.load( "data.csv" );
    testData = trainingData.partition( 50 );

    //Setup grid search
    GridSearch< GestureRecognitionPipeline > gridSearch;

    gridSearch.addParameter( std::bind( &RandomForests::setForestSize, rf, std::placeholders::_1 ), GridSearchRange<unsigned int>(1,100,5) );
    gridSearch.addParameter( std::bind( &RandomForests::setMaxDepth, rf, std::placeholders::_1 ), GridSearchRange<unsigned int>(1,10,1) );
    gridSearch.addParameter( std::bind( &RandomForests::setMinNumSamplesPerNode, rf, std::placeholders::_1 ), GridSearchRange<unsigned int>(100,1000,100) );

    gridSearch.setEvaluationFunction( [&](){ 
        cout << "- forest size: " << rf->getForestSize() << endl;
        cout << "- max depth: " << rf->getMaxDepth() << endl;
        pipeline.train( trainingData ); 
        pipeline.test( testData ); 
        gridSearch.setModel( pipeline );
        return pipeline.getTestAccuracy(); 
    } );
    
    gridSearch.search();

    double bestResult = gridSearch.getBestResult();
    pipeline = gridSearch.getBestModel();
    rf = pipeline.getClassifier< RandomForests >();
    cout << "Best Result: " << bestResult << endl;
    cout << "- forest size: " << rf->getForestSize() << endl;
    cout << "- max depth: " << rf->getMaxDepth() << endl;
   
    return EXIT_SUCCESS;
}
