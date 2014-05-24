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

#include "ClassificationData.h"

namespace GRT{

ClassificationData::ClassificationData(const UINT numDimensions,const string datasetName,const string infoText){
    this->datasetName = datasetName;
    this->numDimensions = numDimensions;
    this->infoText = infoText;
    totalNumSamples = 0;
    crossValidationSetup = false;
    useExternalRanges = false;
    allowNullGestureClass = false;
    if( numDimensions > 0 ) setNumDimensions( numDimensions );
    debugLog.setProceedingText("[DEBUG LCD]");
    errorLog.setProceedingText("[ERROR LCD]");
    warningLog.setProceedingText("[WARNING LCD]");
}

ClassificationData::ClassificationData(const ClassificationData &rhs){
    *this = rhs;
}

ClassificationData::~ClassificationData(){
}
    
ClassificationData& ClassificationData::operator=(const ClassificationData &rhs){
    if( this != &rhs){
        this->datasetName = rhs.datasetName;
        this->infoText = rhs.infoText;
        this->numDimensions = rhs.numDimensions;
        this->totalNumSamples = rhs.totalNumSamples;
        this->kFoldValue = rhs.kFoldValue;
        this->crossValidationSetup = rhs.crossValidationSetup;
        this->useExternalRanges = rhs.useExternalRanges;
        this->allowNullGestureClass = rhs.allowNullGestureClass;
        this->externalRanges = rhs.externalRanges;
        this->classTracker = rhs.classTracker;
        this->data = rhs.data;
        this->crossValidationIndexs = rhs.crossValidationIndexs;
        this->debugLog = rhs.debugLog;
        this->errorLog = rhs.errorLog;
        this->warningLog = rhs.warningLog;
    }
    return *this;
}

void ClassificationData::clear(){
	totalNumSamples = 0;
	data.clear();
	classTracker.clear();
    crossValidationSetup = false;
    crossValidationIndexs.clear();
}

bool ClassificationData::setNumDimensions(const UINT numDimensions){

    if( numDimensions > 0 ){
        //Clear any previous training data
        clear();

        //Set the dimensionality of the data
        this->numDimensions = numDimensions;

        //Clear the external ranges
        useExternalRanges = false;
        externalRanges.clear();

        return true;
    }

    errorLog << "setNumDimensions(const UINT numDimensions) - The number of dimensions of the dataset must be greater than zero!" << endl;
    return false;
}

bool ClassificationData::setDatasetName(const string datasetName){

    //Make sure there are no spaces in the string
    if( datasetName.find(" ") == string::npos ){
        this->datasetName = datasetName;
        return true;
    }

    errorLog << "setDatasetName(const string datasetName) - The dataset name cannot contain any spaces!" << endl;
    return false;
}

bool ClassificationData::setInfoText(const string infoText){
    this->infoText = infoText;
    return true;
}

bool ClassificationData::setClassNameForCorrespondingClassLabel(const string className,const UINT classLabel){

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            classTracker[i].className = className;
            return true;
        }
    }

	errorLog << "setClassNameForCorrespondingClassLabel(const string className,const UINT classLabel) - Failed to find class with label: " << classLabel << endl;
    return false;
}
    
bool ClassificationData::setAllowNullGestureClass(const bool allowNullGestureClass){
    this->allowNullGestureClass = allowNullGestureClass;
    return true;
}

bool ClassificationData::addSample(const UINT classLabel,const VectorDouble &sample){
    
	if( sample.size() != numDimensions ){
        errorLog << "addSample(const UINT classLabel, VectorDouble &sample) - the size of the new sample (" << sample.size() << ") does not match the number of dimensions of the dataset (" << numDimensions << ")" << endl;
        return false;
    }

    //The class label must be greater than zero (as zero is used for the null rejection class label
    if( classLabel == GRT_DEFAULT_NULL_CLASS_LABEL && !allowNullGestureClass ){
        errorLog << "addSample(const UINT classLabel, VectorDouble &sample) - the class label can not be 0!" << endl;
        return false;
    }

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

	ClassificationSample newSample(classLabel,sample);
	data.push_back( newSample );
	totalNumSamples++;

	if( classTracker.size() == 0 ){
		ClassTracker tracker(classLabel,1);
		classTracker.push_back(tracker);
	}else{
		bool labelFound = false;
		for(UINT i=0; i<classTracker.size(); i++){
			if( classLabel == classTracker[i].classLabel ){
				classTracker[i].counter++;
				labelFound = true;
				break;
			}
		}
		if( !labelFound ){
			ClassTracker tracker(classLabel,1);
			classTracker.push_back(tracker);
		}
	}
	return true;
}

bool ClassificationData::removeLastSample(){

    if( totalNumSamples > 0 ){

        //The dataset has changed so flag that any previous cross validation setup will now not work
        crossValidationSetup = false;
        crossValidationIndexs.clear();

        //Find the corresponding class ID for the last training example
        UINT classLabel = data[ totalNumSamples-1 ].getClassLabel();

        //Remove the training example from the buffer
        data.erase(data.end()-1);

        totalNumSamples = (UINT)data.size();

        //Remove the value from the counter
        for(UINT i=0; i<classTracker.size(); i++){
            if( classTracker[i].classLabel == classLabel ){
                classTracker[i].counter--;
                break;
            }
        }

        return true;

    }else return false;

}

bool ClassificationData::reserve(const UINT N){
    
    data.reserve( N );
    
    if( data.capacity() >= N ) return true;
    
    return false;
}
    
UINT ClassificationData::eraseAllSamplesWithClassLabel(const UINT classLabel){
	UINT numExamplesRemoved = 0;
	UINT numExamplesToRemove = 0;

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

	//Find out how many training examples we need to remove
	for(UINT i=0; i<classTracker.size(); i++){
		if( classTracker[i].classLabel == classLabel ){
			numExamplesToRemove = classTracker[i].counter;
			classTracker.erase(classTracker.begin()+i);
			break;
		}
	}

	//Remove the samples with the matching class ID
	if( numExamplesToRemove > 0 ){
		UINT i=0;
		while( numExamplesRemoved < numExamplesToRemove ){
			if( data[i].getClassLabel() == classLabel ){
				data.erase(data.begin()+i);
				numExamplesRemoved++;
			}else if( ++i == data.size() ) break;
		}
	}

	totalNumSamples = (UINT)data.size();

	return numExamplesRemoved;
}
    
bool ClassificationData::addClass(const UINT classLabel){
    
    //Check to make sure the class label does not exist
    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            return false;
        }
    }
    
    //Add the class label to the class tracker
    classTracker.push_back( ClassTracker(classLabel,0) );
    
    //Sort the class labels
    sortClassLabels();
    
    return true;
}

bool ClassificationData::relabelAllSamplesWithClassLabel(const UINT oldClassLabel,const UINT newClassLabel){
    bool oldClassLabelFound = false;
    bool newClassLabelAllReadyExists = false;
    UINT indexOfOldClassLabel = 0;
    UINT indexOfNewClassLabel = 0;

    //Find out how many training examples we need to relabel
    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == oldClassLabel ){
            indexOfOldClassLabel = i;
            oldClassLabelFound = true;
        }
        if( classTracker[i].classLabel == newClassLabel ){
            indexOfNewClassLabel = i;
            newClassLabelAllReadyExists = true;
        }
    }

    //If the old class label was not found then we can't do anything
    if( !oldClassLabelFound ){
        return false;
    }

    //Relabel the old class labels
    for(UINT i=0; i<totalNumSamples; i++){
        if( data[i].getClassLabel() == oldClassLabel ){
            data[i].set(newClassLabel, data[i].getSample());
        }
    }

    //Update the class label counters
    if( newClassLabelAllReadyExists ){
        //Add the old sample count to the new sample count
        classTracker[ indexOfNewClassLabel ].counter += classTracker[ indexOfOldClassLabel ].counter;

        //Erase the old class tracker
        classTracker.erase( classTracker.begin() + indexOfOldClassLabel );
    }else{
        //Create a new class tracker
        classTracker.push_back( ClassTracker(newClassLabel,classTracker[ indexOfOldClassLabel ].counter,classTracker[ indexOfOldClassLabel ].className) );
    }

    return true;
}

bool ClassificationData::setExternalRanges(const vector< MinMax > &externalRanges, const bool useExternalRanges){

    if( externalRanges.size() != numDimensions ) return false;

    this->externalRanges = externalRanges;
    this->useExternalRanges = useExternalRanges;

    return true;
}

bool ClassificationData::enableExternalRangeScaling(const bool useExternalRanges){
    if( externalRanges.size() == numDimensions ){
        this->useExternalRanges = useExternalRanges;
        return true;
    }
    return false;
}

bool ClassificationData::scale(const double minTarget,const double maxTarget){
    vector< MinMax > ranges = getRanges();
    return scale(ranges,minTarget,maxTarget);
}

bool ClassificationData::scale(const vector<MinMax> &ranges,const double minTarget,const double maxTarget){
    if( ranges.size() != numDimensions ) return false;

    //Scale the training data
    for(UINT i=0; i<totalNumSamples; i++){
        for(UINT j=0; j<numDimensions; j++){
            data[i][j] = Util::scale(data[i][j],ranges[j].minValue,ranges[j].maxValue,minTarget,maxTarget);
        }
    }

    return true;
}

bool ClassificationData::saveDatasetToFile(const string &filename) const{

	std::fstream file;
	file.open(filename.c_str(), std::ios::out);

	if( !file.is_open() ){
		return false;
	}

	file << "GRT_LABELLED_CLASSIFICATION_DATA_FILE_V1.0\n";
    file << "DatasetName: " << datasetName << endl;
    file << "InfoText: " << infoText << endl;
	file << "NumDimensions: " << numDimensions << endl;
	file << "TotalNumExamples: " << totalNumSamples << endl;
	file << "NumberOfClasses: " << classTracker.size() << endl;
	file << "ClassIDsAndCounters: " << endl;

	for(UINT i=0; i<classTracker.size(); i++){
		file << classTracker[i].classLabel << "\t" << classTracker[i].counter << "\t" << classTracker[i].className << endl;
	}

    file << "UseExternalRanges: " << useExternalRanges << endl;

    if( useExternalRanges ){
        for(UINT i=0; i<externalRanges.size(); i++){
            file << externalRanges[i].minValue << "\t" << externalRanges[i].maxValue << endl;
        }
    }

	file << "Data:\n";

	for(UINT i=0; i<totalNumSamples; i++){
		file << data[i].getClassLabel();
		for(UINT j=0; j<numDimensions; j++){
			file << "\t" << data[i][j];
		}
		file << endl;
	}

	file.close();
	return true;
}

bool ClassificationData::loadDatasetFromFile(const string &filename){

	std::fstream file;
	file.open(filename.c_str(), std::ios::in);
	UINT numClasses = 0;
	clear();

	if( !file.is_open() ){
        errorLog << "loadDatasetFromFile(const string &filename) - could not open file!" << endl;
		return false;
	}

	string word;

	//Check to make sure this is a file with the Training File Format
	file >> word;
	if(word != "GRT_LABELLED_CLASSIFICATION_DATA_FILE_V1.0"){
        errorLog << "loadDatasetFromFile(const string &filename) - could not find file header!" << endl;
		file.close();
		return false;
	}

    //Get the name of the dataset
	file >> word;
	if(word != "DatasetName:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find DatasetName header!" << endl;
        errorLog << word << endl;
		file.close();
		return false;
	}
	file >> datasetName;

    file >> word;
	if(word != "InfoText:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find InfoText header!" << endl;
		file.close();
		return false;
	}

    //Load the info text
    file >> word;
    infoText = "";
    while( word != "NumDimensions:" ){
        infoText += word + " ";
        file >> word;
    }

	//Get the number of dimensions in the training data
	if( word != "NumDimensions:" ){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find NumDimensions header!" << endl;
		file.close();
		return false;
	}
	file >> numDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if( word != "TotalNumTrainingExamples:" && word != "TotalNumExamples:" ){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find TotalNumTrainingExamples header!" << endl;
		file.close();
		return false;
	}
	file >> totalNumSamples;

	//Get the total number of classes in the training data
	file >> word;
	if(word != "NumberOfClasses:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find NumberOfClasses header!" << endl;
		file.close();
		return false;
	}
	file >> numClasses;

	//Resize the class counter buffer and load the counters
	classTracker.resize(numClasses);

	//Get the total number of classes in the training data
	file >> word;
	if(word != "ClassIDsAndCounters:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find ClassIDsAndCounters header!" << endl;
		file.close();
		return false;
	}

	for(UINT i=0; i<classTracker.size(); i++){
		file >> classTracker[i].classLabel;
		file >> classTracker[i].counter;
        file >> classTracker[i].className;
	}

    //Check if the dataset should be scaled using external ranges
	file >> word;
	if(word != "UseExternalRanges:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find UseExternalRanges header!" << endl;
		file.close();
		return false;
	}
    file >> useExternalRanges;

    //If we are using external ranges then load them
    if( useExternalRanges ){
        externalRanges.resize(numDimensions);
        for(UINT i=0; i<externalRanges.size(); i++){
            file >> externalRanges[i].minValue;
            file >> externalRanges[i].maxValue;
        }
    }

	//Get the main training data
	file >> word;
	if( word != "LabelledTrainingData:" && word != "Data:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find LabelledTrainingData header!" << endl;
		file.close();
		return false;
	}

	ClassificationSample tempSample( numDimensions );
	data.resize( totalNumSamples, tempSample );

	for(UINT i=0; i<totalNumSamples; i++){
        UINT classLabel = 0;
        VectorDouble sample(numDimensions,0);
		file >> classLabel;
		for(UINT j=0; j<numDimensions; j++){
			file >> sample[j];
		}
        data[i].set(classLabel, sample);
	}

	file.close();
	
	sortClassLabels();
	
	return true;
}

bool ClassificationData::saveDatasetToCSVFile(const string &filename) const{

    std::fstream file;
	file.open(filename.c_str(), std::ios::out );

	if( !file.is_open() ){
		return false;
	}

    //Write the data to the CSV file
    for(UINT i=0; i<totalNumSamples; i++){
		file << data[i].getClassLabel();
		for(UINT j=0; j<numDimensions; j++){
			file << "," << data[i][j];
		}
		file << endl;
	}

	file.close();

    return true;
}

bool ClassificationData::loadDatasetFromCSVFile(const string &filename,const UINT classLabelColumnIndex){

    numDimensions = 0;
    datasetName = "NOT_SET";
    infoText = "";

    //Clear any previous data
    clear();

    //Parse the CSV file
    FileParser parser;
    
    if( !parser.parseCSVFile(filename,true) ){
        errorLog << "loadDatasetFromCSVFile(const string &filename,const UINT classLabelColumnIndex) - Failed to parse CSV file!" << endl;
        return false;
    }
    
    if( !parser.getConsistentColumnSize() ){
        errorLog << "loadDatasetFromCSVFile(const string &filename,const UINT classLabelColumnIndexe) - The CSV file does not have a consistent number of columns!" << endl;
        return false;
    }
    
    if( parser.getColumnSize() <= 1 ){
        errorLog << "loadDatasetFromCSVFile(const string &filename,const UINT classLabelColumnIndex) - The CSV file does not have enough columns! It should contain at least two columns!" << endl;
        return false;
    }
    
    //Set the number of dimensions
    numDimensions = parser.getColumnSize()-1;
    
    //Reserve the memory for the data
    reserve( parser.getRowSize() );
    
    UINT classLabel = 0;
    UINT j = 0;
    UINT n = 0;
    VectorDouble sample(numDimensions);
    for(UINT i=0; i<parser.getRowSize(); i++){
        //Get the class label
        classLabel = Util::stringToInt( parser[i][classLabelColumnIndex] );
        
        //Get the sample data
        j=0;
        n=0;
        while( j != numDimensions ){
            if( n != classLabelColumnIndex ){
                sample[j++] = Util::stringToDouble( parser[i][n] );
            }
            n++;
        }
        
        //Add the labelled sample to the dataset
        if( !addSample(classLabel, sample) ){
            warningLog << "loadDatasetFromCSVFile(const string &filename,const UINT classLabelColumnIndex) - Could not add sample " << i << " to the dataset!" << endl;
        }
    }

	sortClassLabels();
    
    return true;
}
    
bool ClassificationData::printStats() const{

    cout << getStatsAsString();

    return true;
}

bool ClassificationData::sortClassLabels(){
	
	sort(classTracker.begin(),classTracker.end(),ClassTracker::sortByClassLabelAscending);
	
	return true;
}

ClassificationData ClassificationData::partition(const UINT trainingSizePercentage,const bool useStratifiedSampling){

    //Partitions the dataset into a training dataset (which is kept by this instance of the ClassificationData) and
	//a testing/validation dataset (which is return as a new instance of the ClassificationData).  The trainingSizePercentage
	//therefore sets the size of the data which remains in this instance and the remaining percentage of data is then added to
	//the testing/validation dataset

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

    ClassificationData trainingSet(numDimensions);
    ClassificationData testSet(numDimensions);
    trainingSet.setAllowNullGestureClass( allowNullGestureClass );
    testSet.setAllowNullGestureClass( allowNullGestureClass );
    vector< UINT > indexs( totalNumSamples );

	//Create the random partion indexs
	Random random;
    UINT randomIndex = 0;

    if( useStratifiedSampling ){
        //Break the data into seperate classes
        vector< vector< UINT > > classData( getNumClasses() );

        //Add the indexs to their respective classes
        for(UINT i=0; i<totalNumSamples; i++){
            classData[ getClassLabelIndexValue( data[i].getClassLabel() ) ].push_back( i );
        }

        //Randomize the order of the indexs in each of the class index buffers
        for(UINT k=0; k<getNumClasses(); k++){
            UINT numSamples = (UINT)classData[k].size();
            for(UINT x=0; x<numSamples; x++){
                //Pick a random index
                randomIndex = random.getRandomNumberInt(0,numSamples);

                //Swap the indexs
                SWAP(classData[k][ x ], classData[k][ randomIndex ]);
            }
        }
        
        //Reserve the memory
        UINT numTrainingSamples = 0;
        UINT numTestSamples = 0;
        
        for(UINT k=0; k<getNumClasses(); k++){
            UINT numTrainingExamples = (UINT) floor( double(classData[k].size()) / 100.0 * double(trainingSizePercentage) );
            UINT numTestExamples = ((UINT)classData[k].size())-numTrainingExamples;
            numTrainingSamples += numTrainingExamples;
            numTestSamples += numTestExamples;
        }
        
        trainingSet.reserve( numTrainingSamples );
        testSet.reserve( numTestSamples );

        //Loop over each class and add the data to the trainingSet and testSet
        for(UINT k=0; k<getNumClasses(); k++){
            UINT numTrainingExamples = (UINT) floor( double(classData[k].size()) / 100.0 * double(trainingSizePercentage) );

            //Add the data to the training and test sets
            for(UINT i=0; i<numTrainingExamples; i++){
                trainingSet.addSample( data[ classData[k][i] ].getClassLabel(), data[ classData[k][i] ].getSample() );
            }
            for(UINT i=numTrainingExamples; i<classData[k].size(); i++){
                testSet.addSample( data[ classData[k][i] ].getClassLabel(), data[ classData[k][i] ].getSample() );
            }
        }
    }else{

        const UINT numTrainingExamples = (UINT) floor( double(totalNumSamples) / 100.0 * double(trainingSizePercentage) );
        //Create the random partion indexs
        Random random;
        UINT randomIndex = 0;
        for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
        for(UINT x=0; x<totalNumSamples; x++){
            //Pick a random index
            randomIndex = random.getRandomNumberInt(0,totalNumSamples);

            //Swap the indexs
            SWAP(indexs[ x ],indexs[ randomIndex ]);
        }
        
        //Reserve the memory
        trainingSet.reserve( numTrainingExamples );
        testSet.reserve( totalNumSamples-numTrainingExamples );

        //Add the data to the training and test sets
        for(UINT i=0; i<numTrainingExamples; i++){
            trainingSet.addSample( data[ indexs[i] ].getClassLabel(), data[ indexs[i] ].getSample() );
        }
        for(UINT i=numTrainingExamples; i<totalNumSamples; i++){
            testSet.addSample( data[ indexs[i] ].getClassLabel(), data[ indexs[i] ].getSample() );
        }
    }

    //Overwrite the training data in this instance with the training data of the trainingSet
    *this = trainingSet;

	sortClassLabels();
    testSet.sortClassLabels();

	return testSet;
}

bool ClassificationData::merge(const ClassificationData &labelledData){

    if( labelledData.getNumDimensions() != numDimensions ){
        errorLog << "merge(const ClassificationData &labelledData) - The number of dimensions in the labelledData (" << labelledData.getNumDimensions() << ") does not match the number of dimensions of this dataset (" << numDimensions << ")" << endl;
        return false;
    }

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();
    
    //Reserve the memory
    reserve( getNumSamples() + labelledData.getNumSamples() );

    //Add the data from the labelledData to this instance
    for(UINT i=0; i<labelledData.getNumSamples(); i++){
        addSample(labelledData[i].getClassLabel(), labelledData[i].getSample());
    }

    //Set the class names from the dataset
    vector< ClassTracker > classTracker = labelledData.getClassTracker();
    for(UINT i=0; i<classTracker.size(); i++){
        setClassNameForCorrespondingClassLabel(classTracker[i].className, classTracker[i].classLabel);
    }

	sortClassLabels();

    return true;
}

bool ClassificationData::spiltDataIntoKFolds(const UINT K,const bool useStratifiedSampling){

    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //K can not be zero
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(const UINT K,const bool useStratifiedSampling) - K can not be zero!" << endl;
        return false;
    }

    //K can not be larger than the number of examples
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(const UINT K,const bool useStratifiedSampling) - K can not be larger than the total number of samples in the dataset!" << endl;
        return false;
    }

    //K can not be larger than the number of examples in a specific class if the stratified sampling option is true
    if( useStratifiedSampling ){
        for(UINT c=0; c<classTracker.size(); c++){
            if( K > classTracker[c].counter ){
                errorLog << "spiltDataIntoKFolds(const UINT K,const bool useStratifiedSampling) - K can not be larger than the number of samples in any given class!" << endl;
                return false;
            }
        }
    }

    //Setup the dataset for k-fold cross validation
    kFoldValue = K;
    vector< UINT > indexs( totalNumSamples );

    //Work out how many samples are in each fold, the last fold might have more samples than the others
    UINT numSamplesPerFold = (UINT) floor( totalNumSamples/double(K) );

    //Add the random indexs to each fold
    crossValidationIndexs.resize(K);

	//Create the random partion indexs
	Random random;
    UINT randomIndex = 0;

    if( useStratifiedSampling ){
        //Break the data into seperate classes
        vector< vector< UINT > > classData( getNumClasses() );

        //Add the indexs to their respective classes
        for(UINT i=0; i<totalNumSamples; i++){
            classData[ getClassLabelIndexValue( data[i].getClassLabel() ) ].push_back( i );
        }

        //Randomize the order of the indexs in each of the class index buffers
        for(UINT c=0; c<getNumClasses(); c++){
            UINT numSamples = (UINT)classData[c].size();
            for(UINT x=0; x<numSamples; x++){
                //Pick a random indexs
                randomIndex = random.getRandomNumberInt(0,numSamples);

                //Swap the indexs
                SWAP(classData[c][ x ] , classData[c][ randomIndex ]);
            }
        }

        //Loop over each of the k folds, at each fold add a sample from each class
        vector< UINT >::iterator iter;
        for(UINT c=0; c<getNumClasses(); c++){
            iter = classData[ c ].begin();
            UINT k = 0;
            while( iter != classData[c].end() ){
                crossValidationIndexs[ k ].push_back( *iter );
                iter++;
                k++;
                k = k % K;
            }
        }

    }else{
        //Randomize the order of the data
        for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
        for(UINT x=0; x<totalNumSamples; x++){
            //Pick a random index
            randomIndex = random.getRandomNumberInt(0,totalNumSamples);

            //Swap the indexs
            SWAP(indexs[ x ] , indexs[ randomIndex ]);
        }

        UINT counter = 0;
        UINT foldIndex = 0;
        for(UINT i=0; i<totalNumSamples; i++){
            //Add the index to the current fold
            crossValidationIndexs[ foldIndex ].push_back( indexs[i] );

            //Move to the next fold if ready
            if( ++counter == numSamplesPerFold && foldIndex < K-1 ){
                foldIndex++;
                counter = 0;
            }
        }
    }

    crossValidationSetup = true;
    return true;

}

ClassificationData ClassificationData::getTrainingFoldData(const UINT foldIndex) const{
   
    ClassificationData trainingData;
    trainingData.setNumDimensions( numDimensions );
    trainingData.setAllowNullGestureClass( allowNullGestureClass );

    if( !crossValidationSetup ){
        errorLog << "getTrainingFoldData(const UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) function first before calling this function!" << endl;
       return trainingData;
    }

    if( foldIndex >= kFoldValue ) return trainingData;

    //Add the data to the training set, this will consist of all the data that is NOT in the foldIndex
    UINT index = 0;
    for(UINT k=0; k<kFoldValue; k++){
        if( k != foldIndex ){
            for(UINT i=0; i<crossValidationIndexs[k].size(); i++){

                index = crossValidationIndexs[k][i];
                trainingData.addSample( data[ index ].getClassLabel(), data[ index ].getSample() );
            }
        }
    }

	trainingData.sortClassLabels();

    return trainingData;
}

ClassificationData ClassificationData::getTestFoldData(const UINT foldIndex) const{
    
    ClassificationData testData;
    testData.setNumDimensions( numDimensions );
    testData.setAllowNullGestureClass( allowNullGestureClass );

    if( !crossValidationSetup ) return testData;

    if( foldIndex >= kFoldValue ) return testData;
    
    testData.reserve( (UINT)crossValidationIndexs[ foldIndex ].size() );

    //Add the data to the test fold
    UINT index = 0;
	for(UINT i=0; i<crossValidationIndexs[ foldIndex ].size(); i++){

        index = crossValidationIndexs[ foldIndex ][i];
		testData.addSample( data[ index ].getClassLabel(), data[ index ].getSample() );
	}
	
	testData.sortClassLabels();

    return testData;
}

ClassificationData ClassificationData::getClassData(const UINT classLabel) const{
    
    ClassificationData classData;
    classData.setNumDimensions( this->numDimensions );
    classData.setAllowNullGestureClass( allowNullGestureClass );
    
    //Reserve the memory for the class data
    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            classData.reserve( classTracker[i].counter );
            break;
        }
    }

    for(UINT i=0; i<totalNumSamples; i++){
        if( data[i].getClassLabel() == classLabel ){
            classData.addSample(classLabel, data[i].getSample());
        }
    }

    return classData;
}
    
ClassificationData ClassificationData::getBootstrappedDataset(UINT numSamples) const{
    
    Random rand;
    ClassificationData newDataset;
    newDataset.setNumDimensions( getNumDimensions() );
    newDataset.setAllowNullGestureClass( allowNullGestureClass );
    newDataset.setExternalRanges( externalRanges, useExternalRanges );
    
    if( numSamples == 0 ) numSamples = totalNumSamples;
    
    newDataset.reserve( numSamples );
    
    //Add all the class labels to the new dataset to ensure the dataset has a list of all the labels
    for(UINT k=0; k<getNumClasses(); k++){
        newDataset.addClass( classTracker[k].classLabel );
    }
    
    //Randomly select the training samples to add to the new data set
    UINT randomIndex;
    for(UINT i=0; i<numSamples; i++){
        randomIndex = rand.getRandomNumberInt(0, totalNumSamples);
        newDataset.addSample(data[randomIndex].getClassLabel(), data[randomIndex].getSample());
    }

    //Sort the class labels so they are in order
	newDataset.sortClassLabels();
    
    return newDataset;
}

RegressionData ClassificationData::reformatAsRegressionData() const{

    //Turns the classification into a regression data to enable regression algorithms like the MLP to be used as a classifier
    //This sets the number of targets in the regression data equal to the number of classes in the classification data
    //The output of each regression training sample will then be all 0's, except for the index matching the classLabel, which will be 1
    //For this to work, the labelled classification data cannot have any samples with a classLabel of 0!
    RegressionData regressionData;

    if( totalNumSamples == 0 ){
        return regressionData;
    }

    const UINT numInputDimensions = numDimensions;
    const UINT numTargetDimensions = getNumClasses();
    regressionData.setInputAndTargetDimensions(numInputDimensions, numTargetDimensions);

    for(UINT i=0; i<totalNumSamples; i++){
        VectorDouble targetVector(numTargetDimensions,0);

        //Set the class index in the target vector to 1 and all other values in the target vector to 0
        UINT classLabel = data[i].getClassLabel();

        if( classLabel > 0 ){
            targetVector[ classLabel-1 ] = 1;
        }else{
            regressionData.clear();
            return regressionData;
        }

        regressionData.addSample(data[i].getSample(),targetVector);
    }

    return regressionData;
}

UnlabelledData ClassificationData::reformatAsUnlabelledData() const{

    UnlabelledData unlabelledData;

    if( totalNumSamples == 0 ){
        return unlabelledData;
    }

    unlabelledData.setNumDimensions( numDimensions );

    for(UINT i=0; i<totalNumSamples; i++){
        unlabelledData.addSample( data[i].getSample() );
    }

    return unlabelledData;
}

UINT ClassificationData::getMinimumClassLabel() const{
    UINT minClassLabel = numeric_limits< UINT >::max();

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel < minClassLabel ){
            minClassLabel = classTracker[i].classLabel;
        }
    }

    return minClassLabel;
}


UINT ClassificationData::getMaximumClassLabel() const{
    UINT maxClassLabel = 0;

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel > maxClassLabel ){
            maxClassLabel = classTracker[i].classLabel;
        }
    }

    return maxClassLabel;
}

UINT ClassificationData::getClassLabelIndexValue(UINT classLabel) const{
    for(UINT k=0; k<classTracker.size(); k++){
        if( classTracker[k].classLabel == classLabel ){
            return k;
        }
    }
    warningLog << "getClassLabelIndexValue(UINT classLabel) - Failed to find class label: " << classLabel << " in class tracker!" << endl;
    return 0;
}

string ClassificationData::getClassNameForCorrespondingClassLabel(UINT classLabel) const{

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            return classTracker[i].className;
        }
    }

    return "CLASS_LABEL_NOT_FOUND";
}

string ClassificationData::getStatsAsString() const{
    string statsText;
    statsText += "DatasetName:\t" + datasetName + "\n";
    statsText += "DatasetInfo:\t" + infoText + "\n";
    statsText += "Number of Dimensions:\t" + Util::toString( numDimensions ) + "\n";
    statsText += "Number of Samples:\t" + Util::toString( totalNumSamples ) + "\n";
    statsText += "Number of Classes:\t" + Util::toString( getNumClasses() ) + "\n";
    statsText += "ClassStats:\n";

    for(UINT k=0; k<getNumClasses(); k++){
        statsText += "ClassLabel:\t" + Util::toString( classTracker[k].classLabel );
        statsText += "\tNumber of Samples:\t" + Util::toString(classTracker[k].counter);
        statsText += "\tClassName:\t" + classTracker[k].className + "\n";
    }

    vector< MinMax > ranges = getRanges();

    statsText += "Dataset Ranges:\n";
    for(UINT j=0; j<ranges.size(); j++){
        statsText += "[" + Util::toString( j+1 ) + "] Min:\t" + Util::toString( ranges[j].minValue ) + "\tMax: " + Util::toString( ranges[j].maxValue ) + "\n";
    }

    return statsText;
}

vector<MinMax> ClassificationData::getRanges() const{

    //If the dataset should be scaled using the external ranges then return the external ranges
    if( useExternalRanges ) return externalRanges;

	vector< MinMax > ranges(numDimensions);

    //Otherwise return the min and max values for each column in the dataset
    if( totalNumSamples > 0 ){
        for(UINT j=0; j<numDimensions; j++){
            ranges[j].minValue = data[0][0];
            ranges[j].maxValue = data[0][0];
            for(UINT i=0; i<totalNumSamples; i++){
                if( data[i][j] < ranges[j].minValue ){ ranges[j].minValue = data[i][j]; }		//Search for the min value
                else if( data[i][j] > ranges[j].maxValue ){ ranges[j].maxValue = data[i][j]; }	//Search for the max value
            }
        }
    }
    return ranges;
}

vector< UINT > ClassificationData::getClassLabels() const{
    vector< UINT > classLabels( getNumClasses(), 0 );

    if( getNumClasses() == 0 ) return classLabels;

    for(UINT i=0; i<getNumClasses(); i++){
        classLabels[i] = classTracker[i].classLabel;
    }

    return classLabels;
}

vector< UINT > ClassificationData::getNumSamplesPerClass() const{
    vector< UINT > classSampleCounts( getNumClasses(), 0 );

    if( getNumSamples() == 0 ) return classSampleCounts;

    for(UINT i=0; i<getNumClasses(); i++){
        classSampleCounts[i] = classTracker[i].counter;
    }

    return classSampleCounts;
}

VectorDouble ClassificationData::getMean() const{
	
	VectorDouble mean(numDimensions,0);
	
	for(UINT j=0; j<numDimensions; j++){
		for(UINT i=0; i<totalNumSamples; i++){
			mean[j] += data[i][j];
		}
		mean[j] /= double(totalNumSamples);
	}
	
	return mean;
}

VectorDouble ClassificationData::getStdDev() const{
	
	VectorDouble mean = getMean();
	VectorDouble stdDev(numDimensions,0);
	
	for(UINT j=0; j<numDimensions; j++){
		for(UINT i=0; i<totalNumSamples; i++){
			stdDev[j] += SQR(data[i][j]-mean[j]);
		}
		stdDev[j] = sqrt( stdDev[j] / double(totalNumSamples-1) );
	}
	
	return stdDev;
}

MatrixDouble ClassificationData::getClassHistogramData(UINT classLabel,UINT numBins) const{

    const UINT M = getNumSamples();
    const UINT N = getNumDimensions();

    vector< MinMax > ranges = getRanges();
    vector< double > binRange(N);
    for(UINT i=0; i<ranges.size(); i++){
        binRange[i] = (ranges[i].maxValue-ranges[i].minValue)/double(numBins);
    }

    MatrixDouble histData(N,numBins);
    histData.setAllValues(0);

    double norm = 0;
    for(UINT i=0; i<M; i++){
        if( data[i].getClassLabel() == classLabel ){
            for(UINT j=0; j<N; j++){
                UINT binIndex = 0;
                bool binFound = false;
                for(UINT k=0; k<numBins-1; k++){
                    if( data[i][j] >= ranges[i].minValue + (binRange[j]*k) && data[i][j] >= ranges[i].minValue + (binRange[j]*(k+1)) ){
                        binIndex = k;
                        binFound = true;
                        break;
                    }
                }
                if( !binFound ) binIndex = numBins-1;
                histData[j][binIndex]++;
            }
            norm++;
        }
    }

    if( norm == 0 ) return histData;

    //Is this the best way to normalize a multidimensional histogram???
    for(UINT i=0; i<histData.getNumRows(); i++){
        for(UINT j=0; j<histData.getNumCols(); j++){
            histData[i][j] /= norm;
        }
    }

    return histData;
}

MatrixDouble ClassificationData::getClassMean() const{
	
	MatrixDouble mean(getNumClasses(),numDimensions);
	VectorDouble counter(getNumClasses(),0);
	
	mean.setAllValues( 0 );
	
	for(UINT i=0; i<totalNumSamples; i++){
		UINT classIndex = getClassLabelIndexValue( data[i].getClassLabel() );
		for(UINT j=0; j<numDimensions; j++){
			mean[classIndex][j] += data[i][j];
		}
		counter[ classIndex ]++;
	}
	
	for(UINT k=0; k<getNumClasses(); k++){
		for(UINT j=0; j<numDimensions; j++){
			mean[k][j] = counter[j] > 0 ? mean[k][j]/counter[j] : 0;
		}
	}
	
	return mean;
}

MatrixDouble ClassificationData::getClassStdDev() const{

	MatrixDouble mean = getClassMean();
	MatrixDouble stdDev(getNumClasses(),numDimensions);
	VectorDouble counter(getNumClasses(),0);
	
	stdDev.setAllValues( 0 );
	
	for(UINT i=0; i<totalNumSamples; i++){
		UINT classIndex = getClassLabelIndexValue( data[i].getClassLabel() );
		for(UINT j=0; j<numDimensions; j++){
			stdDev[classIndex][j] += SQR(data[i][j]-mean[classIndex][j]);
		}
		counter[ classIndex  ]++;
	}
	
	for(UINT k=0; k<getNumClasses(); k++){
		for(UINT j=0; j<numDimensions; j++){
			stdDev[k][j] = sqrt( stdDev[k][j] / double(counter[k]-1) );
		}
	}
	
	return stdDev;
}

MatrixDouble ClassificationData::getCovarianceMatrix() const{
	
	VectorDouble mean = getMean();
	MatrixDouble covariance(numDimensions,numDimensions);
	
	for(UINT j=0; j<numDimensions; j++){
		for(UINT k=0; k<numDimensions; k++){
			for(UINT i=0; i<totalNumSamples; i++){
				covariance[j][k] += (data[i][j]-mean[j]) * (data[i][k]-mean[k]) ;
			}
			covariance[j][k] /= double(totalNumSamples-1);
		}
	}
	
	return covariance;
}

vector< MatrixDouble > ClassificationData::getHistogramData(UINT numBins) const{
    const UINT K = getNumClasses();
    vector< MatrixDouble > histData(K);

    for(UINT k=0; k<K; k++){
        histData[k] = getClassHistogramData( classTracker[k].classLabel, numBins );
    }

    return histData;
}

vector< UINT > ClassificationData::getClassDataIndexes(UINT classLabel) const{

    const UINT M = getNumSamples();
    const UINT K = getNumClasses();
    UINT N = 0;

    //Get the number of samples in the class
    for(UINT k=0; k<K; k++){
        if( classTracker[k].classLabel == classLabel){
            N = classTracker[k].counter;
            break;
        }
    }

    UINT index = 0;
    vector< UINT > classIndexes(N);
    for(UINT i=0; i<M; i++){
        if( data[i].getClassLabel() == classLabel ){
            classIndexes[index++] = i;
        }
    }

    return classIndexes;
}

MatrixDouble ClassificationData::getDataAsMatrixDouble() const{

    const UINT M = getNumSamples();
    const UINT N = getNumDimensions();
    MatrixDouble d(M,N);

    for(UINT i=0; i<M; i++){
        for(UINT j=0; j<N; j++){
            d[i][j] = data[i][j];
        }
    }

    return d;
}

}; //End of namespace GRT
