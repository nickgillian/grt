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
 GRT ZeroCrossingCounter Example
 This example demonstrates how to create and use the GRT ZeroCrossingCounter FeatureExtraction Module.
 
 The ZeroCrossingCounter module counts the number of zero crossings that occur in an N dimensional signal 
 over a given window. The ZeroCrossingCounter class is good for extracting features that describe how much 
 change is occurring in an N dimensional signal. An example application might be to use the ZeroCrossingCounter 
 in combination with one of the GRT classification algorithms to determine if an object is being shaken, and 
 if so, how the object is being shaken.
 
 In this example we create an instance of a ZeroCrossingCounter and use this to extract some features from a
 recording of some acceleration data.  The recording contains a number of different types of the accelerometer
 being shaken, you can see a plot of the raw data here: 
 http://www.nickgillian.com/archive/wiki/grt/reference/ZeroCrossingCounter/ZeroCrossingCounterFeatureExtractionExampleImage1.jpg
 
 The raw data and features are then saved to a file (so you can plot the results in Matlab, Excel, etc. if needed).
 
 This example shows you how to:
 - Create a new ZeroCrossingCounter instance and customize the feature settings
 - Compute some features from a 3 dimensional data signal
 - Save the ZeroCrossingCounter settings to a file
 - Load the ZeroCrossingCounter settings from a file
*/

#include "GRT.h"
using namespace GRT;

int main (int argc, const char * argv[])
{
    
    //Load the example data
    ClassificationData data;
    
    if( !data.loadDatasetFromFile("WiiAccShakeData.txt") ){
        cout << "ERROR: Failed to load data from file!\n";
        return EXIT_FAILURE;
    }

    //The variables used to initialize the zero crossing counter feature extraction
    UINT searchWindowSize = 20;
    double deadZoneThreshold = 0.01;
    UINT numDimensions = data.getNumDimensions();
    UINT featureMode = ZeroCrossingCounter::INDEPENDANT_FEATURE_MODE; //This could also be ZeroCrossingCounter::COMBINED_FEATURE_MODE
    
    //Create a new instance of the ZeroCrossingCounter feature extraction
    ZeroCrossingCounter zeroCrossingCounter(searchWindowSize,deadZoneThreshold,numDimensions,featureMode);
    
    //Loop over the accelerometer data, at each time sample (i) compute the features using the new sample and then write the results to a file
    for(UINT i=0; i<data.getNumSamples(); i++){
        
        //Compute the features using this new sample
        zeroCrossingCounter.computeFeatures( data[i].getSample() );
        
        //Write the data to the file
        cout << "InputVector: ";
        for(UINT j=0; j<data.getNumDimensions(); j++){
           cout << data[i].getSample()[j] << "\t";
        }
        
        //Get the latest feature vector
        VectorDouble featureVector = zeroCrossingCounter.getFeatureVector();
        
        //Write the features to the file
        cout << "FeatureVector: ";
        for(UINT j=0; j<featureVector.size(); j++){
            cout << featureVector[j];
            if( j != featureVector.size()-1 ) cout << "\t";
        }
        cout << endl;
    }
    
    //Save the zero crossing counter settings to a file
    zeroCrossingCounter.saveModelToFile("ZeroCrossingCounterSettings.txt");
    
    //You can then load the settings again if you need them
    zeroCrossingCounter.loadModelFromFile("ZeroCrossingCounterSettings.txt");
    
    return EXIT_SUCCESS;
}

