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

#include "RegressionData.h"

namespace GRT{

RegressionData::RegressionData(const UINT numInputDimensions,const UINT numTargetDimensions,const string datasetName,const string infoText):totalNumSamples(0){
    this->numInputDimensions = numInputDimensions;
    this->numTargetDimensions = numTargetDimensions;
    this->datasetName = datasetName;
    this->infoText = infoText;
    kFoldValue = 0;
    crossValidationSetup = false;
    useExternalRanges = false;
    debugLog.setProceedingText("[DEBUG LRD]");
    errorLog.setProceedingText("[ERROR LRD]");
    warningLog.setProceedingText("[WARNING LRD]");
}

RegressionData::RegressionData(const RegressionData &rhs){
    *this = rhs;
}

RegressionData::~RegressionData(){}
    
RegressionData& RegressionData::operator=(const RegressionData &rhs){
    if( this != &rhs){
        this->datasetName = rhs.datasetName;
        this->infoText = rhs.infoText;
        this->numInputDimensions = rhs.numInputDimensions;
        this->numTargetDimensions = rhs.numTargetDimensions;
        this->totalNumSamples = rhs.totalNumSamples;
        this->kFoldValue = rhs.kFoldValue;
        this->crossValidationSetup = rhs.crossValidationSetup;
        this->useExternalRanges = rhs.useExternalRanges;
        this->externalInputRanges = rhs.externalInputRanges;
        this->externalTargetRanges = rhs.externalTargetRanges;
        this->data = rhs.data;
        this->crossValidationIndexs = rhs.crossValidationIndexs;
        this->debugLog = rhs.debugLog;
        this->errorLog = rhs.errorLog;
        this->warningLog = rhs.warningLog;
    }
    return *this;
}

void RegressionData::clear(){
    totalNumSamples = 0;
    kFoldValue = 0;
    crossValidationSetup = false;
    data.clear();
    crossValidationIndexs.clear();
}

bool RegressionData::setInputAndTargetDimensions(const UINT numInputDimensions,const UINT numTargetDimensions){
	clear();
    if( numInputDimensions > 0 && numTargetDimensions > 0 ){
        this->numInputDimensions = numInputDimensions;
        this->numTargetDimensions = numTargetDimensions;

        //Clear the external ranges
        useExternalRanges = false;
        externalInputRanges.clear();
        externalTargetRanges.clear();
        return true;
    }
    errorLog << "setInputAndTargetDimensions(UINT numInputDimensions,UINT numTargetDimensions) - The number of input and target dimensions should be greater than zero!" << endl;
    return false;
}

bool RegressionData::setDatasetName(const string &datasetName){

    //Make sure there are no spaces in the string
    if( datasetName.find(" ") == string::npos ){
        this->datasetName = datasetName;
        return true;
    }

    errorLog << "setDatasetName(const string &datasetName) - The dataset name cannot contain any spaces!" << endl;
    return false;
}

bool RegressionData::setInfoText(const string &infoText){
    this->infoText = infoText;
    return true;
}

bool RegressionData::addSample(const VectorDouble &inputVector,const VectorDouble &targetVector){
	if( inputVector.size() == numInputDimensions && targetVector.size() == numTargetDimensions ){
        data.push_back( RegressionSample(inputVector,targetVector) );
        totalNumSamples++;

        //The dataset has changed so flag that any previous cross validation setup will now not work
        crossValidationSetup = false;
        crossValidationIndexs.clear();
        return true;
    }
    errorLog << "addSample(const VectorDouble &inputVector,const VectorDouble &targetVector) - The inputVector size or targetVector size does not match the size of the numInputDimensions or numTargetDimensions" << endl;
    return false;
}

bool RegressionData::removeLastSample(){
	if( totalNumSamples > 0 ){
		//Remove the training example from the buffer
		data.erase(data.end()-1);
		totalNumSamples = (UINT)data.size();

        //The dataset has changed so flag that any previous cross validation setup will now not work
        crossValidationSetup = false;
        crossValidationIndexs.clear();
		return true;
	}
    warningLog << "removeLastSample() - There are no samples to remove!" << endl;
    return false;
}
    
bool RegressionData::reserve(const UINT N){
    
    data.reserve( N );
    
    if( data.capacity() >= N ) return true;
    
    return false;
}

bool RegressionData::setExternalRanges(const vector< MinMax > &externalInputRanges,const vector< MinMax > & externalTargetRanges,const bool useExternalRanges){

    if( externalInputRanges.size() != numInputDimensions ) return false;
    if( externalTargetRanges.size() != numTargetDimensions ) return false;

    this->externalInputRanges = externalInputRanges;
    this->externalTargetRanges = externalTargetRanges;
    this->useExternalRanges = useExternalRanges;

    return true;
}

bool RegressionData::enableExternalRangeScaling(const bool useExternalRanges){
    if( externalInputRanges.size() != numInputDimensions && externalTargetRanges.size() != numTargetDimensions  ){
        this->useExternalRanges = useExternalRanges;
        return true;
    }
    return false;
}

bool RegressionData::scale(const double minTarget,const double maxTarget){
    vector< MinMax > inputRanges = getInputRanges();
    vector< MinMax > targetRanges = getTargetRanges();
    return scale(inputRanges,targetRanges,minTarget,maxTarget);
}

bool RegressionData::scale(const vector< MinMax > &inputVectorRanges,const vector< MinMax > &targetVectorRanges,const double minTarget,const double maxTarget){
    if( inputVectorRanges.size() == numInputDimensions && targetVectorRanges.size() == numTargetDimensions ){

        VectorDouble scaledInputVector(numInputDimensions,0);
        VectorDouble scaledTargetVector(numTargetDimensions,0);
        for(UINT i=0; i<totalNumSamples; i++){

            //Scale the input vector
            for(UINT j=0; j<numInputDimensions; j++){
                scaledInputVector[j] = Util::scale(data[i].getInputVectorValue(j),inputVectorRanges[j].minValue,inputVectorRanges[j].maxValue,minTarget,maxTarget);
            }
            //Scale the target vector
            for(UINT j=0; j<numTargetDimensions; j++){
                scaledTargetVector[j] = Util::scale(data[i].getTargetVectorValue(j),targetVectorRanges[j].minValue,targetVectorRanges[j].maxValue,minTarget,maxTarget);
            }
            //Update the training sample with the scaled data
            data[i].set(scaledInputVector,scaledTargetVector);
        }

        return true;
    }
    return false;
}

vector<MinMax> RegressionData::getInputRanges() const{

    if( useExternalRanges ) return externalInputRanges;

	vector< MinMax > ranges(numInputDimensions);

	if( totalNumSamples > 0 ){
		for(UINT j=0; j<numInputDimensions; j++){
			ranges[j].minValue = data[0].getInputVectorValue(j);
			ranges[j].maxValue = data[0].getInputVectorValue(j);
			for(UINT i=0; i<totalNumSamples; i++){
				if( data[i].getInputVectorValue(j) < ranges[j].minValue ){ ranges[j].minValue = data[i].getInputVectorValue(j); }		//Search for the min value
				else if( data[i].getInputVectorValue(j) > ranges[j].maxValue ){ ranges[j].maxValue = data[i].getInputVectorValue(j); }	//Search for the max value
			}
		}
	}
	return ranges;
}

vector<MinMax> RegressionData::getTargetRanges() const{

    if( useExternalRanges ) return externalTargetRanges;

    vector< MinMax > ranges(numTargetDimensions);

    if( totalNumSamples > 0 ){
        for(UINT j=0; j<numTargetDimensions; j++){
            ranges[j].minValue = data[0].getTargetVectorValue(j);
            ranges[j].maxValue = data[0].getTargetVectorValue(j);
            for(UINT i=0; i<totalNumSamples; i++){
                if( data[i].getTargetVectorValue(j) < ranges[j].minValue ){ ranges[j].minValue = data[i].getTargetVectorValue(j); }		//Search for the min value
                else if( data[i].getTargetVectorValue(j) > ranges[j].maxValue ){ ranges[j].maxValue = data[i].getTargetVectorValue(j); }	//Search for the max value
            }
        }
    }
    return ranges;
}

string RegressionData::getStatsAsString() const{

    string statsText;
    statsText += "DatasetName:\t" + datasetName + "\n";
    statsText += "DatasetInfo:\t" + infoText + "\n";
    statsText += "Number of Input Dimensions:\t" + Util::toString( numInputDimensions ) + "\n";
    statsText += "Number of Target Dimensions:\t" + Util::toString( numTargetDimensions ) + "\n";
    statsText += "Number of Samples:\t" + Util::toString( totalNumSamples ) + "\n";

    vector< MinMax > inputRanges = getInputRanges();

    statsText += "Dataset Input Dimension Ranges:\n";
    for(UINT j=0; j<inputRanges.size(); j++){
        statsText += "[" + Util::toString( j+1 ) + "] Min:\t" + Util::toString( inputRanges[j].minValue ) + "\tMax: " + Util::toString( inputRanges[j].maxValue ) + "\n";
    }

    vector< MinMax > targetRanges = getTargetRanges();

    statsText += "Dataset Target Dimension Ranges:\n";
    for(UINT j=0; j<targetRanges.size(); j++){
        statsText += "[" + Util::toString( j+1 ) + "] Min:\t" + Util::toString( targetRanges[j].minValue ) + "\tMax: " + Util::toString( targetRanges[j].maxValue ) + "\n";
    }
    return statsText;
}

bool RegressionData::printStats() const{
    cout << getStatsAsString();
    return true;
}
    
RegressionData RegressionData::partition(const UINT trainingSizePercentage){

	//Partitions the dataset into a training dataset (which is kept by this instance of the RegressionData) and
	//a testing/validation dataset (which is return as a new instance of the RegressionData).  The trainingSizePercentage
	//therefore sets the size of the data which remains in this instance and the remaining percentage of data is then added to
	//the testing/validation dataset

	const UINT numTrainingExamples = (UINT) floor( double(totalNumSamples) / 100.0 * double(trainingSizePercentage) );

	RegressionData trainingSet(numInputDimensions,numTargetDimensions);
	RegressionData testSet(numInputDimensions,numTargetDimensions);
	vector< UINT > indexs( totalNumSamples );

	//Create the random partion indexs
	Random random;
    UINT randomIndex = 0;
	for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
	for(UINT x=0; x<totalNumSamples; x++){
		randomIndex = random.getRandomNumberInt(0,totalNumSamples);
		SWAP( indexs[ x ] , indexs[ randomIndex ] );
	}

	//Add the data to the training and test sets
	for(UINT i=0; i<numTrainingExamples; i++){
		trainingSet.addSample( data[ indexs[i] ].getInputVector(), data[ indexs[i] ].getTargetVector() );
	}
	for(UINT i=numTrainingExamples; i<totalNumSamples; i++){
		testSet.addSample( data[ indexs[i] ].getInputVector(), data[ indexs[i] ].getTargetVector() );
	}

	//Overwrite the training data in this instance with the training data of the trainingSet
	data = trainingSet.getData();
	totalNumSamples = trainingSet.getNumSamples();

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

	return testSet;
}

bool RegressionData::merge(const RegressionData &regressionData){

    if( regressionData.getNumInputDimensions() != numInputDimensions ){
        errorLog << "merge(RegressionData &regressionData) - The number of input dimensions in the regressionData (" << regressionData.getNumInputDimensions() << ") does not match the number of input dimensions of this dataset (" << numInputDimensions << ")" << endl;
        return false;
    }

    if( regressionData.getNumTargetDimensions() != numTargetDimensions ){
        errorLog << "merge(RegressionData &regressionData) - The number of target dimensions in the regressionData (" << regressionData.getNumTargetDimensions() << ") does not match the number of target dimensions of this dataset (" << numTargetDimensions << ")" << endl;
        return false;
    }

    //Add the data from the labelledData to this instance
    for(UINT i=0; i<regressionData.getNumSamples(); i++){
        addSample(regressionData[i].getInputVector(), regressionData[i].getTargetVector());
    }

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

    return true;
}

bool RegressionData::spiltDataIntoKFolds(const UINT K){

    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //K can not be zero
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(UINT K) - K can not be zero!" << endl;
        return false;
    }

    //K can not be larger than the number of examples
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(UINT K) - K can not be larger than the total number of samples in the dataset!" << endl;
        return false;
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

    //Randomize the order of the data
    for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
    for(UINT x=0; x<totalNumSamples; x++){
        //Pick a random index
        randomIndex = random.getRandomNumberInt(0,totalNumSamples);

        //Swap the indexs
        SWAP( indexs[ x ] , indexs[ randomIndex ] );
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

    crossValidationSetup = true;
    return true;

}

RegressionData RegressionData::getTrainingFoldData(const UINT foldIndex) const{
    RegressionData trainingData;

    if( !crossValidationSetup ){
        errorLog << "getTrainingFoldData(UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) function first before calling this function!" << endl;
        return trainingData;
    }

    if( foldIndex >= kFoldValue ) return trainingData;

    trainingData.setInputAndTargetDimensions(numInputDimensions, numTargetDimensions);

    //Add the data to the training set, this will consist of all the data that is NOT in the foldIndex
    UINT index = 0;
    for(UINT k=0; k<kFoldValue; k++){
        if( k != foldIndex ){
            for(UINT i=0; i<crossValidationIndexs[k].size(); i++){

                index = crossValidationIndexs[k][i];
                trainingData.addSample( data[ index ].getInputVector(), data[ index ].getTargetVector() );
            }
        }
    }

    return trainingData;
}

RegressionData RegressionData::getTestFoldData(const UINT foldIndex) const{
    RegressionData testData;

    if( !crossValidationSetup ) return testData;

    if( foldIndex >= kFoldValue ) return testData;

    //Add the data to the training
    testData.setInputAndTargetDimensions(numInputDimensions, numTargetDimensions);

    UINT index = 0;
    for(UINT i=0; i<crossValidationIndexs[ foldIndex ].size(); i++){

        index = crossValidationIndexs[ foldIndex ][i];
        testData.addSample( data[ index ].getInputVector(), data[ index ].getTargetVector() );
    }

    return testData;
}

UINT RegressionData::removeDuplicateSamples(){

    UINT numSamplesRemoved = 0;

    //Sort the data
    sort(data.begin(),data.end(),RegressionSample::sortByInputVectorAscending );

    //Remove any samples that are very close to each other
    double minDist = 1.0e-5;
    double dist = 0;
    double totalDimensions = numInputDimensions + numTargetDimensions;
    bool keepSearching = true;
    vector< RegressionSample >::iterator currentSample = data.begin();
    vector< RegressionSample >::iterator nextSample = data.begin()+1;

    if( currentSample == data.end() ) keepSearching = false;
    if( nextSample == data.end() ) keepSearching = false;

    while( keepSearching ){
        dist = 0;
        for(UINT i=0; i<numInputDimensions; i++){
            dist += SQR( currentSample->getInputVectorValue(i) - nextSample->getInputVectorValue(i) );
        }
        for(UINT i=0; i<numTargetDimensions; i++){
            dist += SQR( currentSample->getTargetVectorValue(i) - nextSample->getTargetVectorValue(i) );
        }
        dist /= totalDimensions;
        if( dist <= minDist ){
            //Remove the next sample
            currentSample = data.erase( nextSample );
            nextSample = currentSample + 1;
            numSamplesRemoved++;
            debugLog << "Removing sample with dist: " << dist << endl;
        }else{
            currentSample++;
            nextSample++;
        }

        if( currentSample == data.end() ) keepSearching = false;
        if( nextSample == data.end() ) keepSearching = false;
    }

    return numSamplesRemoved;
}

bool RegressionData::saveDatasetToFile(const string &filename) const{

	std::fstream file;
	file.open(filename.c_str(), std::ios::out);

	if( !file.is_open() ){
        errorLog << "saveDatasetToFile(const string &filename) - Failed to open file!" << endl;
		return false;
	}

	file << "GRT_LABELLED_REGRESSION_DATA_FILE_V1.0\n";
    file << "DatasetName: " << datasetName << endl;
    file << "InfoText: " << infoText << endl;
	file << "NumInputDimensions: "<<numInputDimensions<<endl;
	file << "NumTargetDimensions: "<<numTargetDimensions<<endl;
	file << "TotalNumTrainingExamples: "<<totalNumSamples<<endl;
    file << "UseExternalRanges: " << useExternalRanges << endl;

    if( useExternalRanges ){
        for(UINT i=0; i<externalInputRanges.size(); i++){
            file << externalInputRanges[i].minValue << "\t" << externalInputRanges[i].maxValue << endl;
        }
        for(UINT i=0; i<externalTargetRanges.size(); i++){
            file << externalTargetRanges[i].minValue << "\t" << externalTargetRanges[i].maxValue << endl;
        }
    }

	file << "RegressionData:\n";

	for(UINT i=0; i<totalNumSamples; i++){
		for(UINT j=0; j<numInputDimensions; j++){
			file << data[i].getInputVectorValue(j) << "\t";
		}
		for(UINT j=0; j<numTargetDimensions; j++){
			file << data[i].getTargetVectorValue(j);
			if( j!= numTargetDimensions-1 ) file << "\t";
		}
		file << endl;
	}

	file.close();
	return true;
}

bool RegressionData::loadDatasetFromFile(const string &filename){

	std::fstream file;
	file.open(filename.c_str(), std::ios::in);
	clear();

	if( !file.is_open() ){
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to open file!" << endl;
		return false;
	}

	string word;

	//Check to make sure this is a file with the Training File Format
	file >> word;
	if(word != "GRT_LABELLED_REGRESSION_DATA_FILE_V1.0"){
        errorLog << "loadDatasetFromFile(const string &filename) - Unknown file header!" << endl;
		file.close();
		return false;
	}

    //Get the name of the dataset
	file >> word;
	if(word != "DatasetName:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find DatasetName!" << endl;
		file.close();
		return false;
	}
	file >> datasetName;

    file >> word;
	if(word != "InfoText:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find InfoText!" << endl;
		file.close();
		return false;
	}

    //Load the info text
    file >> word;
    infoText = "";
    while( word != "NumInputDimensions:" ){
        infoText += word + " ";
        file >> word;
    }

	//Get the number of input dimensions in the training data
	if(word != "NumInputDimensions:"){
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to find NumInputDimensions!" << endl;
		file.close();
		return false;
	}
	file >> numInputDimensions;

	//Get the number of target dimensions in the training data
	file >> word;
	if(word != "NumTargetDimensions:"){
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to find NumTargetDimensions!" << endl;
		file.close();
		return false;
	}
	file >> numTargetDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if(word != "TotalNumTrainingExamples:"){
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to find TotalNumTrainingExamples!" << endl;
		file.close();
		return false;
	}
	file >> totalNumSamples;

    //Check if the dataset should be scaled using external ranges
	file >> word;
	if(word != "UseExternalRanges:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find DatasetName!" << endl;
		file.close();
		return false;
	}
    file >> useExternalRanges;

    //If we are using external ranges then load them
    if( useExternalRanges ){
        externalInputRanges.resize(numInputDimensions);
        externalTargetRanges.resize(numTargetDimensions);
        for(UINT i=0; i<externalInputRanges.size(); i++){
            file >> externalInputRanges[i].minValue;
            file >> externalInputRanges[i].maxValue;
        }
        for(UINT i=0; i<externalTargetRanges.size(); i++){
            file >> externalTargetRanges[i].minValue;
            file >> externalTargetRanges[i].maxValue;
        }
    }

	//Get the main training data
	file >> word;
	if(word != "RegressionData:"){
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to find RegressionData!" << endl;
		file.close();
		return false;
	}

	VectorDouble inputVector(numInputDimensions);
	VectorDouble targetVector(numTargetDimensions);
	data.resize( totalNumSamples, RegressionSample(inputVector,targetVector) );

	for(UINT i=0; i<totalNumSamples; i++){
		//Read the input vector
		for(UINT j=0; j<numInputDimensions; j++){
			file >> inputVector[j];
		}
		for(UINT j=0; j<numTargetDimensions; j++){
			file >> targetVector[j];
		}
        data[i].set(inputVector, targetVector);
	}

	file.close();
	return true;
}

bool RegressionData::saveDatasetToCSVFile(const string &filename) const{

    std::fstream file;
	file.open(filename.c_str(), std::ios::out );

	if( !file.is_open() ){
        errorLog << "saveDatasetToCSVFile(const string &filename) - Failed to open file!" << endl;
		return false;
	}

    //Write the data to the CSV file
    for(UINT i=0; i<totalNumSamples; i++){
		for(UINT j=0; j<numInputDimensions; j++){
			file << data[i].getInputVector()[j] << ",";
		}
        for(UINT j=0; j<numTargetDimensions; j++){
			file << data[i].getTargetVector()[j];
            if( j != numTargetDimensions-1 ) file << ",";
		}
		file << endl;
	}

	file.close();

    return true;
}

bool RegressionData::loadDatasetFromCSVFile(const string &filename,const UINT numInputDimensions,const UINT numTargetDimensions){

    fstream file;
    string value;
    clear();
    datasetName = "NOT_SET";
    infoText = "";

    //Clear any previous data
    clear();
    
    //Parse the CSV file
    FileParser parser;
    
    if( !parser.parseCSVFile(filename,true) ){
        errorLog << "loadDatasetFromCSVFile(...) - Failed to parse CSV file!" << endl;
        return false;
    }
    
    if( !parser.getConsistentColumnSize() ){
        errorLog << "loadDatasetFromCSVFile(...) - The CSV file does not have a consistent number of columns!" << endl;
        return false;
    }
    
    if( parser.getColumnSize() != numInputDimensions+numTargetDimensions ){
        errorLog << "loadDatasetFromCSVFile(...) - The number of columns in the CSV file (" << parser.getColumnSize() << ")";
        errorLog << " does not match the number of input dimensions plus the number of target dimensions (" << numInputDimensions+numTargetDimensions << ")" << endl;
        return false;
    }
    
    //Setup the labelled classification data
    setInputAndTargetDimensions(numInputDimensions, numTargetDimensions);
    
    UINT n = 0;
    VectorDouble inputVector(numInputDimensions);
    VectorDouble targetVector(numTargetDimensions);
    for(UINT i=0; i<parser.getRowSize(); i++){
        
        //Reset n
        n = 0;
        
        //Get the input vector
        for(UINT j=0; j<numInputDimensions; j++){
            inputVector[j] = Util::stringToDouble( parser[i][n++] );
        }
        
        //Get the target vector
        for(UINT j=0; j<numTargetDimensions; j++){
            targetVector[j] = Util::stringToDouble( parser[i][n++] );
        }
        
        //Add the labelled sample to the dataset
        if( !addSample(inputVector, targetVector) ){
            warningLog << "loadDatasetFromCSVFile(string filename) - Could not add sample " << i << " to the dataset!" << endl;
        }
    }
    
    return true;
}

} //End of namespace GRT

