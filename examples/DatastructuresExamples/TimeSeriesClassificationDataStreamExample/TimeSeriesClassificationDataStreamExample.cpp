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
    //Create a new instance of the TimeSeriesClassificationDataStream
    TimeSeriesClassificationDataStream trainingData;
    
    //Set the dimensionality of the data (you need to do this before you can add any samples)
    trainingData.setNumDimensions( 3 );
    
    //You can also give the dataset a name (the name should have no spaces)
    trainingData.setDatasetName("DummyData");
    
    //You can also add some info text about the data
    trainingData.setInfoText("This data contains some dummy timeseries data");
    
    //Here you would record a time series, when you have finished recording the time series then add the training sample to the training data
    UINT gestureLabel = 1;
    
    //For now we will just add 10 x 20 random walk data timeseries, each timeseries will be seperated by some noise which represents the null class
    Random random;
    for(UINT k=0; k<10; k++){//For the number of classes
        gestureLabel = k+1;
        
        //Get the init random walk position for this gesture
        VectorFloat startPos( trainingData.getNumDimensions() );
        for(UINT j=0; j<startPos.size(); j++){
            startPos[j] = random.getRandomNumberUniform(-1.0,1.0);
        }
        
        //Generate the 20 time series
        for(UINT x=0; x<20; x++){
            //Generate the random walk
            UINT randomWalkLength = random.getRandomNumberInt(90, 110);
            VectorFloat sample = startPos;
            for(UINT i=0; i<randomWalkLength; i++){
                for(UINT j=0; j<sample.size(); j++){
                    sample[j] += random.getRandomNumberUniform(-0.1,0.1);
                }
                
                //Add the training sample to the dataset
                trainingData.addSample(gestureLabel, sample );
            }
            
            //now add some noise to represent a null class
            for(UINT i=0; i<50; i++){
                for(UINT j=0; j<sample.size(); j++){
                    sample[j] = random.getRandomNumberUniform(-0.01,0.01);
                }
                
                //Add the training sample to the dataset, note that we set the gesture label to 0
                trainingData.addSample(0, sample );
            }
        }
    }
    
    
    //After recording your training data you can then save it to a file
    if( !trainingData.save( "TrainingData.grt" ) ){
        cout << "ERROR: Failed to save dataset to file!\n";
        return EXIT_FAILURE;
    }
    
    //This can then be loaded later
    if( !trainingData.load( "TrainingData.grt" ) ){
        cout << "ERROR: Failed to load dataset from file!\n";
        return EXIT_FAILURE;
    }
    
    //This is how you can get some stats from the training data
    string datasetName = trainingData.getDatasetName();
    string infoText = trainingData.getInfoText();
    UINT numSamples = trainingData.getNumSamples();
    UINT numDimensions = trainingData.getNumDimensions();
    UINT numClasses = trainingData.getNumClasses();
    
    cout << "Dataset Name: " << datasetName << endl;
    cout << "InfoText: " << infoText << endl;
    cout << "NumberOfSamples: " << numSamples << endl;
    cout << "NumberOfDimensions: " << numDimensions << endl;
    cout << "NumberOfClasses: " << numClasses << endl;
    
    //Print the start and end indexs of each time series
    Vector< TimeSeriesPositionTracker > positionTracker = trainingData.getTimeSeriesPositionTracker();
    for(UINT i=0; i<positionTracker.getSize(); i++){
        cout << "Class Label: " << positionTracker[i].getClassLabel() << "\t";
        cout << "Start Index: " << positionTracker[i].getStartIndex() << "\t";
        cout << "End Index: " << positionTracker[i].getEndIndex() << "\t";
        cout << "Length: " << positionTracker[i].getLength() << endl;
    }
    
    //You can also get the minimum and maximum ranges of the data
    Vector< MinMax > ranges = trainingData.getRanges();
    
    cout << "The ranges of the dataset are: \n";
    for(UINT j=0; j<ranges.size(); j++){
        cout << "Dimension: " << j << " Min: " << ranges[j].minValue << " Max: " << ranges[j].maxValue << endl;
    }
    
    //If need you can clear any training data that you have recorded
    trainingData.clear();

    return EXIT_SUCCESS;
}

