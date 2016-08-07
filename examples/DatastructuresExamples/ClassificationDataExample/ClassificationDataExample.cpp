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

    //Create a new instance of the ClassificationData
    ClassificationData trainingData;
    
    //Set the dimensionality of the data (you need to do this before you can add any samples)
    trainingData.setNumDimensions( 3 );
    
    //You can also give the dataset a name (the name should have no spaces)
    trainingData.setDatasetName("DummyData");
    
    //You can also add some info text about the data
    trainingData.setInfoText("This data contains some dummy data");
    
    //Here you would grab some data from your sensor and label it with the corresponding gesture it belongs to
    UINT gestureLabel = 1;
    VectorFloat sample(3);
    
    //For now we will just add some random data
    Random random;
    for(UINT i=0; i<100; i++){
        sample[0] = random.getRandomNumberUniform(-1.0,1.0);
        sample[1] = random.getRandomNumberUniform(-1.0,1.0); 
        sample[2] = random.getRandomNumberUniform(-1.0,1.0); 
        
        //Add the sample to the training data
        trainingData.addSample( gestureLabel, sample );
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
    
    //You can also save and load the training data to a CSV file
    //Each row will contain a sample, with the first column containing the class label and the remaining columns containing the data
    if( !trainingData.save( "TrainingData.csv" ) ){
		cout << "ERROR: Failed to save dataset to csv file!\n";
		return EXIT_FAILURE;
	}
	
    //The data structure will automatically detect the csv extension and parse the file accordingly
    if( !trainingData.load( "TrainingData.csv" ) ){
		cout << "ERROR: Failed to load dataset from csv file!\n";
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
    
    //You can also get the minimum and maximum ranges of the data
    Vector< MinMax > ranges = trainingData.getRanges();
    
    cout << "The ranges of the dataset are: \n";
    for(UINT j=0; j<ranges.size(); j++){
        cout << "Dimension: " << j << " Min: " << ranges[j].minValue << " Max: " << ranges[j].maxValue << endl;
    }
    
    //If you want to split the dataset into a training dataset and a test dataset then you can use the split function
    //A value of 80 means that 80% of the original data will remain in the training dataset and 20% will be returned as the test dataset
    ClassificationData testData = trainingData.split( 80 );
    
    //If you have multiple datasets that you want to merge together then use the merge function
    if( !trainingData.merge( testData ) ){
		cout << "ERROR: Failed to save merge datasets!\n";
		return EXIT_FAILURE;
	}
    
    //If you want to run K-Fold cross validation using the dataset then you should first spilt the dataset into K-Folds
    //A value of 10 splits the dataset into 10 folds and the true parameter signals that stratified sampling should be used
    if( !trainingData.spiltDataIntoKFolds( 10, true ) ){
		cout << "ERROR: Failed to spiltDataIntoKFolds!\n";
		return EXIT_FAILURE;
	}
    
    //After you have called the spilt function you can then get the training and test sets for each fold
    for(UINT foldIndex=0; foldIndex<10; foldIndex++){
        ClassificationData foldTrainingData = trainingData.getTrainingFoldData( foldIndex );
        ClassificationData foldTestingData = trainingData.getTestFoldData( foldIndex );
    }
    
    //If need you can clear any training data that you have recorded
    trainingData.clear();
    
    return EXIT_SUCCESS;
}

