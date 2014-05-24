/*
 GRT Custom Feature Extraction Example
 This examples demonstrates how to create your own custom feature extraction module. 
 
 The example creates a simple feature extraction module called an Accumulator, which is probably 
 not very useful as a feature extraction module but is good for demonstrating how to create your 
 own feature extraction.  The Accumulator takes the input, sums up the values, and then adds the 
 last value (which is first multipled by a weight).
 
 The code below tests the Accumulator for both indpendent use (i.e. outside of a pipeline) and also
 within a pipeline. You should look at the Accumulator class to check out how to create your own
 feature extraction module.
*/

#include "Accumulator.h"
using namespace GRT;

bool independentTest();
bool pipelineTest();

int main(int argc, const char * argv[])
{
    //Run the independent test
    if( !independentTest() ){
        cout << "ERROR: Failed to run independent test!\n";
        return EXIT_FAILURE;
    }
    
    //Run the pipeline test
    if( !pipelineTest() ){
        cout << "ERROR: Failed to run pipeline test!\n";
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

bool independentTest(){
    
    const UINT N = 3;
    Random random;
    
    //Create a new accumulator feature extraction module and set the number of dimensions to 3
    Accumulator accumulator( N );
    
    //Save the accumulator to a file to test if this works
    if( !accumulator.saveSettingsToFile( "AccumulatorSettings.txt" ) ){
        cout << "ERROR: Failed to save features!\n";
        return false;
    }
    
    //Load the accumulator from a file to test if this works
    if( !accumulator.loadSettingsFromFile( "AccumulatorSettings.txt" ) ){
        cout << "ERROR: Failed to load features!\n";
        return false;
    }
    
    //Test everything is OK
    if( !accumulator.getInitialized() ){
        cout << "ERROR: The accumulator has not been initialized!\n";
        return false;
    }
    
    //Print the settings to test everything was loaded OK
    UINT numInputDimensions = accumulator.getNumInputDimensions();
    UINT numOutputDimensions = accumulator.getNumOutputDimensions();
    double lastValueWeight = accumulator.getLastValueWeight();
    
    cout << "NumInputDimensions: " << numInputDimensions << endl;
    cout << "NumOutputDimensions: " << numOutputDimensions << endl;
    cout << "LastValueWeight: " << lastValueWeight << endl;
    
    //Run some dummy data through the feature extraction
    for(UINT i=0; i<100; i++){
        VectorDouble inputVector(N);
        
        for(UINT j=0; j<N; j++){
            inputVector[j] = random.getRandomNumberUniform(-1,1);
        }
        
        if( !accumulator.computeFeatures( inputVector ) ){
            cout << "ERROR: Failed to compute features!\n";
            return false;
        }
        
        //Get the features and print the values
        VectorDouble featureData = accumulator.getFeatureVector();
        
        cout << "i: " << i << "\t";
        cout << "input vector: ";
        for(UINT j=0; j<N; j++){
            cout << inputVector[j] << "\t";
        }
        cout << "feature vector: ";
        for(UINT j=0; j<accumulator.getNumOutputDimensions(); j++){
            cout << featureData[j] << "\t";
        }
        cout << endl;
    }
    
    return true;
}

bool pipelineTest(){
    
    const UINT N = 3;
    Random random;
    GestureRecognitionPipeline pipeline;
    
    pipeline.addFeatureExtractionModule( Accumulator( N ) );
    
    //Run some dummy data through the feature extraction
    for(UINT i=0; i<100; i++){
        VectorDouble inputVector(N);
        
        for(UINT j=0; j<N; j++){
            inputVector[j] = random.getRandomNumberUniform(-1,1);
        }
        
        //Pass the input data through the preprocessing modules in the pipeline
        if( !pipeline.preProcessData( inputVector ) ){
            cout << "ERROR: Failed to compute features!\n";
            return false;
        }
        
        //Get the features and print the values
        VectorDouble featureData = pipeline.getFeatureExtractionData();
        
        cout << "i: " << i << "\t";
        cout << "input vector: ";
        for(UINT j=0; j<N; j++){
            cout << inputVector[j] << "\t";
        }
        cout << "feature vector: ";
        for(UINT j=0; j<featureData.size(); j++){
            cout << featureData[j] << "\t";
        }
        cout << endl;
    }
    
    return true;
}



