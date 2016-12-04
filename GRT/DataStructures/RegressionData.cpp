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

#define GRT_DLL_EXPORTS
#include "RegressionData.h"

GRT_BEGIN_NAMESPACE

RegressionData::RegressionData(const UINT numInputDimensions,const UINT numTargetDimensions,const std::string datasetName,const std::string infoText):totalNumSamples(0){
    this->numInputDimensions = numInputDimensions;
    this->numTargetDimensions = numTargetDimensions;
    this->datasetName = datasetName;
    this->infoText = infoText;
    kFoldValue = 0;
    crossValidationSetup = false;
    useExternalRanges = false;
    debugLog.setKey("[DEBUG RegressionData]");
    errorLog.setKey("[ERROR RegressionData]");
    warningLog.setKey("[WARNING RegressionData]");
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
    errorLog << "setInputAndTargetDimensions(UINT numInputDimensions,UINT numTargetDimensions) - The number of input and target dimensions should be greater than zero!" << std::endl;
    return false;
}

bool RegressionData::setDatasetName(const std::string &datasetName){

    //Make sure there are no spaces in the string
    if( datasetName.find(" ") == std::string::npos ){
        this->datasetName = datasetName;
        return true;
    }

    errorLog << "setDatasetName(const string &datasetName) - The dataset name cannot contain any spaces!" << std::endl;
    return false;
}

bool RegressionData::setInfoText(const std::string &infoText){
    this->infoText = infoText;
    return true;
}

bool RegressionData::addSample(const VectorFloat &inputVector,const VectorFloat &targetVector){
	if( inputVector.getSize() == numInputDimensions && targetVector.getSize() == numTargetDimensions ){
        data.push_back( RegressionSample(inputVector,targetVector) );
        totalNumSamples++;

        //The dataset has changed so flag that any previous cross validation setup will now not work
        crossValidationSetup = false;
        crossValidationIndexs.clear();
        return true;
    }
    errorLog << "addSample(const VectorFloat &inputVector,const VectorFloat &targetVector) - The inputVector size or targetVector size does not match the size of the numInputDimensions or numTargetDimensions" << std::endl;
    return false;
}

bool RegressionData::removeLastSample(){
	if( totalNumSamples > 0 ){
		//Remove the training example from the buffer
		data.erase(data.end()-1);
		totalNumSamples = data.getSize();

        //The dataset has changed so flag that any previous cross validation setup will now not work
        crossValidationSetup = false;
        crossValidationIndexs.clear();
		return true;
	}
    warningLog << "removeLastSample() - There are no samples to remove!" << std::endl;
    return false;
}
    
bool RegressionData::reserve(const UINT N){
    
    data.reserve( N );
    
    if( data.capacity() >= N ) return true;
    
    return false;
}

bool RegressionData::setExternalRanges(const Vector< MinMax > &externalInputRanges,const Vector< MinMax > & externalTargetRanges,const bool useExternalRanges){

    if( externalInputRanges.getSize() != numInputDimensions ) return false;
    if( externalTargetRanges.getSize() != numTargetDimensions ) return false;

    this->externalInputRanges = externalInputRanges;
    this->externalTargetRanges = externalTargetRanges;
    this->useExternalRanges = useExternalRanges;

    return true;
}

bool RegressionData::enableExternalRangeScaling(const bool useExternalRanges){
    if( externalInputRanges.getSize() != numInputDimensions && externalTargetRanges.getSize() != numTargetDimensions  ){
        this->useExternalRanges = useExternalRanges;
        return true;
    }
    return false;
}

bool RegressionData::scale(const Float minTarget,const Float maxTarget){
    Vector< MinMax > inputRanges = getInputRanges();
    Vector< MinMax > targetRanges = getTargetRanges();
    return scale(inputRanges,targetRanges,minTarget,maxTarget);
}

bool RegressionData::scale(const Vector< MinMax > &inputVectorRanges,const Vector< MinMax > &targetVectorRanges,const Float minTarget,const Float maxTarget){
    if( inputVectorRanges.getSize() == numInputDimensions && targetVectorRanges.getSize() == numTargetDimensions ){

        VectorFloat scaledInputVector(numInputDimensions,0);
        VectorFloat scaledTargetVector(numTargetDimensions,0);
        for(UINT i=0; i<totalNumSamples; i++){

            //Scale the input Vector
            for(UINT j=0; j<numInputDimensions; j++){
                scaledInputVector[j] = grt_scale(data[i].getInputVectorValue(j),inputVectorRanges[j].minValue,inputVectorRanges[j].maxValue,minTarget,maxTarget);
            }
            //Scale the target Vector
            for(UINT j=0; j<numTargetDimensions; j++){
                scaledTargetVector[j] = grt_scale(data[i].getTargetVectorValue(j),targetVectorRanges[j].minValue,targetVectorRanges[j].maxValue,minTarget,maxTarget);
            }
            //Update the training sample with the scaled data
            data[i].set(scaledInputVector,scaledTargetVector);
        }

        return true;
    }
    return false;
}

Vector<MinMax> RegressionData::getInputRanges() const{

    if( useExternalRanges ) return externalInputRanges;

	Vector< MinMax > ranges(numInputDimensions);

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

Vector<MinMax> RegressionData::getTargetRanges() const{

    if( useExternalRanges ) return externalTargetRanges;

    Vector< MinMax > ranges(numTargetDimensions);

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

std::string RegressionData::getStatsAsString() const{

    std::string statsText;
    statsText += "DatasetName:\t" + datasetName + "\n";
    statsText += "DatasetInfo:\t" + infoText + "\n";
    statsText += "Number of Input Dimensions:\t" + Util::toString( numInputDimensions ) + "\n";
    statsText += "Number of Target Dimensions:\t" + Util::toString( numTargetDimensions ) + "\n";
    statsText += "Number of Samples:\t" + Util::toString( totalNumSamples ) + "\n";

    Vector< MinMax > inputRanges = getInputRanges();

    statsText += "Dataset Input Dimension Ranges:\n";
    for(UINT j=0; j<inputRanges.size(); j++){
        statsText += "[" + Util::toString( j+1 ) + "] Min:\t" + Util::toString( inputRanges[j].minValue ) + "\tMax: " + Util::toString( inputRanges[j].maxValue ) + "\n";
    }

    Vector< MinMax > targetRanges = getTargetRanges();

    statsText += "Dataset Target Dimension Ranges:\n";
    for(UINT j=0; j<targetRanges.size(); j++){
        statsText += "[" + Util::toString( j+1 ) + "] Min:\t" + Util::toString( targetRanges[j].minValue ) + "\tMax: " + Util::toString( targetRanges[j].maxValue ) + "\n";
    }
    return statsText;
}

bool RegressionData::printStats() const{
    std::cout << getStatsAsString();
    return true;
}

RegressionData RegressionData::partition(const UINT trainingSizePercentage){
    return split( trainingSizePercentage );
}

RegressionData RegressionData::split(const UINT trainingSizePercentage){

	//Partitions the dataset into a training dataset (which is kept by this instance of the RegressionData) and
	//a testing/validation dataset (which is return as a new instance of the RegressionData).  The trainingSizePercentage
	//therefore sets the size of the data which remains in this instance and the remaining percentage of data is then added to
	//the testing/validation dataset

	const UINT numTrainingExamples = (UINT) floor( Float(totalNumSamples) / 100.0 * Float(trainingSizePercentage) );

	RegressionData trainingSet(numInputDimensions,numTargetDimensions);
	RegressionData testSet(numInputDimensions,numTargetDimensions);
	Vector< UINT > indexs( totalNumSamples );

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
        errorLog << "merge(RegressionData &regressionData) - The number of input dimensions in the regressionData (" << regressionData.getNumInputDimensions() << ") does not match the number of input dimensions of this dataset (" << numInputDimensions << ")" << std::endl;
        return false;
    }

    if( regressionData.getNumTargetDimensions() != numTargetDimensions ){
        errorLog << "merge(RegressionData &regressionData) - The number of target dimensions in the regressionData (" << regressionData.getNumTargetDimensions() << ") does not match the number of target dimensions of this dataset (" << numTargetDimensions << ")" << std::endl;
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
        errorLog << "spiltDataIntoKFolds(UINT K) - K can not be zero!" << std::endl;
        return false;
    }

    //K can not be larger than the number of examples
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(UINT K) - K can not be larger than the total number of samples in the dataset!" << std::endl;
        return false;
    }

    //Setup the dataset for k-fold cross validation
    kFoldValue = K;
    Vector< UINT > indexs( totalNumSamples );

    //Work out how many samples are in each fold, the last fold might have more samples than the others
    UINT numSamplesPerFold = (UINT) floor( totalNumSamples/Float(K) );

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
        errorLog << "getTrainingFoldData(UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) function first before calling this function!" << std::endl;
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
    Float minDist = 1.0e-5;
    Float dist = 0;
    Float totalDimensions = numInputDimensions + numTargetDimensions;
    bool keepSearching = true;
    Vector< RegressionSample >::iterator currentSample = data.begin();
    Vector< RegressionSample >::iterator nextSample = data.begin()+1;

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
            debugLog << "Removing sample with dist: " << dist << std::endl;
        }else{
            currentSample++;
            nextSample++;
        }

        if( currentSample == data.end() ) keepSearching = false;
        if( nextSample == data.end() ) keepSearching = false;
    }

    return numSamplesRemoved;
}
    
bool RegressionData::save(const std::string &filename) const{
    
    //Check if the file should be saved as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return saveDatasetToCSVFile( filename );
    }
    
    //Otherwise save it as a custom GRT file
    return saveDatasetToFile( filename );
}

bool RegressionData::load(const std::string &filename){
    
    //Check if the file should be loaded as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return loadDatasetFromCSVFile( filename, numInputDimensions, numTargetDimensions );
    }
    
    //Otherwise save it as a custom GRT file
    return loadDatasetFromFile( filename );
}

bool RegressionData::saveDatasetToFile(const std::string &filename) const{

	std::fstream file;
	file.open(filename.c_str(), std::ios::out);

	if( !file.is_open() ){
        errorLog << "saveDatasetToFile(const string &filename) - Failed to open file!" << std::endl;
		return false;
	}

	file << "GRT_LABELLED_REGRESSION_DATA_FILE_V1.0\n";
    file << "DatasetName: " << datasetName << std::endl;
    file << "InfoText: " << infoText << std::endl;
	file << "NumInputDimensions: "<<numInputDimensions << std::endl;
	file << "NumTargetDimensions: "<<numTargetDimensions << std::endl;
	file << "TotalNumTrainingExamples: "<<totalNumSamples << std::endl;
    file << "UseExternalRanges: " << useExternalRanges << std::endl;

    if( useExternalRanges ){
        for(UINT i=0; i<externalInputRanges.getSize(); i++){
            file << externalInputRanges[i].minValue << "\t" << externalInputRanges[i].maxValue << std::endl;
        }
        for(UINT i=0; i<externalTargetRanges.getSize(); i++){
            file << externalTargetRanges[i].minValue << "\t" << externalTargetRanges[i].maxValue << std::endl;
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
		file << std::endl;
	}

	file.close();
	return true;
}

bool RegressionData::loadDatasetFromFile(const std::string &filename){

	std::fstream file;
	file.open(filename.c_str(), std::ios::in);
	clear();

	if( !file.is_open() ){
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to open file!" << std::endl;
		return false;
	}

	std::string word;

	//Check to make sure this is a file with the Training File Format
	file >> word;
	if(word != "GRT_LABELLED_REGRESSION_DATA_FILE_V1.0"){
        errorLog << "loadDatasetFromFile(const string &filename) - Unknown file header!" << std::endl;
		file.close();
		return false;
	}

    //Get the name of the dataset
	file >> word;
	if(word != "DatasetName:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find DatasetName!" << std::endl;
		file.close();
		return false;
	}
	file >> datasetName;

    file >> word;
	if(word != "InfoText:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find InfoText!" << std::endl;
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
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to find NumInputDimensions!" << std::endl;
		file.close();
		return false;
	}
	file >> numInputDimensions;

	//Get the number of target dimensions in the training data
	file >> word;
	if(word != "NumTargetDimensions:"){
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to find NumTargetDimensions!" << std::endl;
		file.close();
		return false;
	}
	file >> numTargetDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if(word != "TotalNumTrainingExamples:"){
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to find TotalNumTrainingExamples!" << std::endl;
		file.close();
		return false;
	}
	file >> totalNumSamples;

    //Check if the dataset should be scaled using external ranges
	file >> word;
	if(word != "UseExternalRanges:"){
        errorLog << "loadDatasetFromFile(const string &filename) - failed to find DatasetName!" << std::endl;
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
	if( word != "RegressionData:" && word != "LabelledRegressionData:" ){
        errorLog << "loadDatasetFromFile(const string &filename) - Failed to find RegressionData!" << std::endl;
		file.close();
		return false;
	}

	VectorFloat inputVector(numInputDimensions);
	VectorFloat targetVector(numTargetDimensions);
	data.resize( totalNumSamples, RegressionSample(inputVector,targetVector) );

	for(UINT i=0; i<totalNumSamples; i++){
		//Read the input Vector
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

bool RegressionData::saveDatasetToCSVFile(const std::string &filename) const{

    std::fstream file;
	file.open(filename.c_str(), std::ios::out );

	if( !file.is_open() ){
        errorLog << "saveDatasetToCSVFile(const string &filename) - Failed to open file!" << std::endl;
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
		file << std::endl;
	}

	file.close();

    return true;
}

bool RegressionData::loadDatasetFromCSVFile(const std::string &filename,const UINT numInputDimensions,const UINT numTargetDimensions){

    std::fstream file;
    std::string value;
    clear();
    datasetName = "NOT_SET";
    infoText = "";

    //Clear any previous data
    clear();
    
    //Parse the CSV file
    FileParser parser;
    
    if( !parser.parseCSVFile(filename,true) ){
        errorLog << "loadDatasetFromCSVFile(...) - Failed to parse CSV file!" << std::endl;
        return false;
    }
    
    if( !parser.getConsistentColumnSize() ){
        errorLog << "loadDatasetFromCSVFile(...) - The CSV file does not have a consistent number of columns!" << std::endl;
        return false;
    }
    
    if( parser.getColumnSize() != numInputDimensions+numTargetDimensions ){
        errorLog << "loadDatasetFromCSVFile(...) - The number of columns in the CSV file (" << parser.getColumnSize() << ")";
        errorLog << " does not match the number of input dimensions plus the number of target dimensions (" << numInputDimensions+numTargetDimensions << ")" << std::endl;
        return false;
    }
    
    //Setup the labelled classification data
    setInputAndTargetDimensions(numInputDimensions, numTargetDimensions);
    
    UINT n = 0;
    VectorFloat inputVector(numInputDimensions);
    VectorFloat targetVector(numTargetDimensions);
    for(UINT i=0; i<parser.getRowSize(); i++){
        
        //Reset n
        n = 0;
        
        //Get the input Vector
        for(UINT j=0; j<numInputDimensions; j++){
            inputVector[j] = grt_from_str< Float >( parser[i][n++] );
        }
        
        //Get the target Vector
        for(UINT j=0; j<numTargetDimensions; j++){
            targetVector[j] = grt_from_str< Float >( parser[i][n++] );
        }
        
        //Add the labelled sample to the dataset
        if( !addSample(inputVector, targetVector) ){
            warningLog << "loadDatasetFromCSVFile(string filename) - Could not add sample " << i << " to the dataset!" << std::endl;
        }
    }
    
    return true;
}

GRT_END_NAMESPACE

