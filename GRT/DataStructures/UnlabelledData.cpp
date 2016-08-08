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
#include "UnlabelledData.h"

GRT_BEGIN_NAMESPACE

UnlabelledData::UnlabelledData(const UINT numDimensions,const std::string datasetName,const std::string infoText):debugLog("[DEBUG ULCD]"),errorLog("[ERROR ULCD]"),warningLog("[WARNING ULCD]"){
    this->datasetName = datasetName;
    this->numDimensions = numDimensions;
    this->infoText = infoText;
    totalNumSamples = 0;
    crossValidationSetup = false;
    useExternalRanges = false;
    if( numDimensions > 0 ) setNumDimensions( numDimensions );
}

UnlabelledData::UnlabelledData(const UnlabelledData &rhs):debugLog("[DEBUG ULCD]"),errorLog("[ERROR ULCD]"),warningLog("[WARNING ULCD]"){
    *this = rhs;
}

UnlabelledData::~UnlabelledData(){}
    
UnlabelledData& UnlabelledData::operator=(const UnlabelledData &rhs){
    if( this != &rhs){
        this->datasetName = rhs.datasetName;
        this->infoText = rhs.infoText;
        this->numDimensions = rhs.numDimensions;
        this->totalNumSamples = rhs.totalNumSamples;
        this->kFoldValue = rhs.kFoldValue;
        this->crossValidationSetup = rhs.crossValidationSetup;
        this->useExternalRanges = rhs.useExternalRanges;
        this->externalRanges = rhs.externalRanges;
        this->data = rhs.data;
        this->crossValidationIndexs = rhs.crossValidationIndexs;
        this->debugLog = rhs.debugLog;
        this->errorLog = rhs.errorLog;
        this->warningLog = rhs.warningLog;
    }
    return *this;
}

void UnlabelledData::clear(){
	totalNumSamples = 0;
	data.clear();
    crossValidationSetup = false;
    crossValidationIndexs.clear();
}

bool UnlabelledData::setNumDimensions(const UINT numDimensions){

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
    return false;
}

bool UnlabelledData::setDatasetName(const std::string datasetName){

    //Make sure there are no spaces in the std::string
    if( datasetName.find(" ") == std::string::npos ){
        this->datasetName = datasetName;
        return true;
    }

    return false;
}

bool UnlabelledData::setInfoText(const std::string infoText){
    this->infoText = infoText;
    return true;
}

bool UnlabelledData::addSample(const VectorFloat &sample){
    
	if( sample.size() != numDimensions ) return false;

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

	data.push_back( sample );
	totalNumSamples++;

	return true;
}

bool UnlabelledData::removeLastSample(){

    if( totalNumSamples > 0 ){

        //The dataset has changed so flag that any previous cross validation setup will now not work
        crossValidationSetup = false;
        crossValidationIndexs.clear();

        //If there is only one sample then we just need to clear the buffer
        if( totalNumSamples == 1 ){
            data.clear();
            return true;
        }
        
        data.erase( data.begin()+data.size()-1 );

        return true;

    }else return false;

}

bool UnlabelledData::reserve(const UINT N){
    
    data.reserve( N );
    
    if( data.capacity() >= N ) return true;
    
    return false;
}
    
bool UnlabelledData::setExternalRanges(const Vector< MinMax > &externalRanges,const bool useExternalRanges){

    if( externalRanges.size() != numDimensions ) return false;

    this->externalRanges = externalRanges;
    this->useExternalRanges = useExternalRanges;

    return true;
}

bool UnlabelledData::enableExternalRangeScaling(const bool useExternalRanges){
    if( externalRanges.size() == numDimensions ){
        this->useExternalRanges = useExternalRanges;
        return true;
    }
    return false;
}

bool UnlabelledData::scale(const Float minTarget,const Float maxTarget){
    Vector< MinMax > ranges = getRanges();
    return scale(ranges,minTarget,maxTarget);
}

bool UnlabelledData::scale(const Vector<MinMax> &ranges,const Float minTarget,const Float maxTarget){
    if( ranges.size() != numDimensions ) return false;

    //Scale the training data
    for(UINT i=0; i<totalNumSamples; i++){
        for(UINT j=0; j<numDimensions; j++){
            data[i][j] = Util::scale(data[i][j],ranges[j].minValue,ranges[j].maxValue,minTarget,maxTarget);
        }
    }

    return true;
}
    
bool UnlabelledData::save(const std::string &filename) const{
    
    //Check if the file should be saved as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return saveDatasetToCSVFile( filename );
    }
    
    //Otherwise save it as a custom GRT file
    return saveDatasetToFile( filename );
}

bool UnlabelledData::load(const std::string &filename){
    
    //Check if the file should be loaded as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return loadDatasetFromCSVFile( filename );
    }
    
    //Otherwise save it as a custom GRT file
    return loadDatasetFromFile( filename );
}

bool UnlabelledData::saveDatasetToFile(const std::string &filename) const{

	std::fstream file;
	file.open(filename.c_str(), std::ios::out);

	if( !file.is_open() ){
        errorLog << "saveDatasetToFile(const std::string &filename) - Failed to open file!" << std::endl;
		return false;
	}

	file << "GRT_UNLABELLED_DATA_FILE_V1.0\n";
    file << "DatasetName: " << datasetName << std::endl;
    file << "InfoText: " << infoText << std::endl;
	file << "NumDimensions: " << numDimensions << std::endl;
	file << "TotalNumTrainingExamples: " << totalNumSamples << std::endl;

    file << "UseExternalRanges: " << useExternalRanges << std::endl;

    if( useExternalRanges ){
        for(UINT i=0; i<externalRanges.size(); i++){
            file << externalRanges[i].minValue << "\t" << externalRanges[i].maxValue << std::endl;
        }
    }

	file << "UnlabelledTrainingData:\n";

	for(UINT i=0; i<totalNumSamples; i++){
		for(UINT j=0; j<numDimensions; j++){
            if( j != 0 ) file << "\t";
			file << data[i][j];
		}
		file << std::endl;
	}

	file.close();
	return true;
}

bool UnlabelledData::loadDatasetFromFile(const std::string &filename){

	std::fstream file;
	file.open(filename.c_str(), std::ios::in);
	clear();

	if( !file.is_open() ){
        errorLog << "loadDatasetFromFile(const std::string &filename) - could not open file!" << std::endl;
		return false;
	}

	std::string word;

	//Check to make sure this is a file with the Training File Format
	file >> word;
	if( word != "GRT_UNLABELLED_DATA_FILE_V1.0" && word != "GRT_UNLABELLED_CLASSIFICATION_DATA_FILE_V1.0" ){
        errorLog << "loadDatasetFromFile(const std::string &filename) - could not find file header!" << std::endl;
		file.close();
		return false;
	}

    //Get the name of the dataset
	file >> word;
	if(word != "DatasetName:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find DatasetName!" << std::endl;
		file.close();
		return false;
	}
	file >> datasetName;

    file >> word;
	if(word != "InfoText:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find InfoText!" << std::endl;
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
	if(word != "NumDimensions:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find DatasetName!" << std::endl;
		file.close();
		return false;
	}
	file >> numDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if(word != "TotalNumTrainingExamples:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find DatasetName!" << std::endl;
		file.close();
		return false;
	}
	file >> totalNumSamples;

    //Check if the dataset should be scaled using external ranges
	file >> word;
	if(word != "UseExternalRanges:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find DatasetName!" << std::endl;
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
	if(word != "UnlabelledTrainingData:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find DatasetName!" << std::endl;
		file.close();
		return false;
	}
	data.resize( totalNumSamples, VectorFloat(numDimensions) );

	for(UINT i=0; i<totalNumSamples; i++){
		for(UINT j=0; j<numDimensions; j++){
			file >> data[i][j];
		}
	}

	file.close();
	return true;
}


bool UnlabelledData::saveDatasetToCSVFile(const std::string &filename) const{

    std::fstream file;
	file.open(filename.c_str(), std::ios::out );

	if( !file.is_open() ){
        errorLog << "saveDatasetToCSVFile(const std::string &filename) - Failed to open file!" << std::endl;
		return false;
	}

    //Write the data to the CSV file
    for(UINT i=0; i<totalNumSamples; i++){
		for(UINT j=0; j<numDimensions; j++){
            if( j != 0 ) file << ",";
			file << data[i][j];
		}
		file << std::endl;
	}

	file.close();

    return true;
}

bool UnlabelledData::loadDatasetFromCSVFile(const std::string &filename){

    std::string value;
    datasetName = "NOT_SET";
    infoText = "";

    //Clear any previous data
    clear();
    
    //Parse the CSV file
    FileParser parser;
    
    if( !parser.parseCSVFile(filename,true) ){
        errorLog << "loadDatasetFromCSVFile(const std::string &filename) - Failed to parse CSV file!" << std::endl;
        return false;
    }
    
    if( !parser.getConsistentColumnSize() ){
        errorLog << "loadDatasetFromCSVFile(const std::string &filename) - The CSV file does not have a consistent number of columns!" << std::endl;
        return false;
    }
    
    const UINT rows = parser.getRowSize();
    const UINT cols = parser.getColumnSize();
    
    //Setup the labelled classification data
    numDimensions = cols;
    
    //Reserve the data so we do not have to continually resize the memory
    data.reserve( rows );

    VectorFloat sample(numDimensions);
    for(UINT i=0; i<rows; i++){
        
        //Get the input vector
        for(UINT j=0; j<numDimensions; j++){
            sample[j] = Util::stringToFloat( parser[i][j] );
        }
        
        //Add the labelled sample to the dataset
        if( !addSample(sample) ){
            warningLog << "loadDatasetFromCSVFile(const std::string &filename) - Could not add sample " << i << " to the dataset!" << std::endl;
        }
    }
    
    return true;
}

UnlabelledData UnlabelledData::partition(const UINT trainingSizePercentage){
    return split( trainingSizePercentage );
}

UnlabelledData UnlabelledData::split(const UINT trainingSizePercentage){

    //Partitions the dataset into a training dataset (which is kept by this instance of the UnlabelledData) and
	//a testing/validation dataset (which is return as a new instance of the UnlabelledData).  The trainingSizePercentage
	//therefore sets the size of the data which remains in this instance and the remaining percentage of data is then added to
	//the testing/validation dataset

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

	const UINT numTrainingExamples = (UINT) floor( Float(totalNumSamples) / 100.0 * Float(trainingSizePercentage) );

	UnlabelledData trainingSet(numDimensions);
	UnlabelledData testSet(numDimensions);
	Vector< UINT > indexs( totalNumSamples );

	//Create the random partion indexs
	Random random;
    UINT randomIndex = 0;
	for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
	for(UINT x=0; x<totalNumSamples; x++){
        //Pick a random index
		randomIndex = random.getRandomNumberInt(0,totalNumSamples);

        //Swap the indexs
        SWAP( indexs[ x ] , indexs[ randomIndex ] );
	}
    
    trainingSet.reserve( numTrainingExamples );
    testSet.reserve( totalNumSamples-numTrainingExamples );

	//Add the data to the training and test sets
	for(UINT i=0; i<numTrainingExamples; i++){
		trainingSet.addSample( data[ indexs[i] ] );
	}
	for(UINT i=numTrainingExamples; i<totalNumSamples; i++){
		testSet.addSample( data[ indexs[i] ]  );
	}

	//Overwrite the training data in this instance with the training data of the trainingSet
	*this = trainingSet;

	return testSet;
}

bool UnlabelledData::merge(const UnlabelledData &unlabelledData){

    if( unlabelledData.getNumDimensions() != numDimensions ){
        errorLog << "merge(const UnlabelledData &unlabelledData) - The number of dimensions in the unlabelledData (" << unlabelledData.getNumDimensions() << ") does not match the number of dimensions of this dataset (" << numDimensions << ")" << std::endl;
        return false;
    }

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();
    
    reserve( getNumSamples() + unlabelledData.getNumSamples() );

    //Add the data from the labelledData to this instance
    for(UINT i=0; i<unlabelledData.getNumSamples(); i++){
        addSample( unlabelledData[i] );
    }

    return true;
}

bool UnlabelledData::spiltDataIntoKFolds(const UINT K){

    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //K can not be zero
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(const UINT K) - K can not be zero!" << std::endl;
        return false;
    }

    //K can not be larger than the number of examples
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(const UINT K) - K can not be larger than the total number of samples in the dataset!" << std::endl;
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
        grt_swap( indexs[ x ] , indexs[ randomIndex ] );
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

UnlabelledData UnlabelledData::getTrainingFoldData(const UINT foldIndex) const{
    UnlabelledData trainingData;

    if( !crossValidationSetup ){
        errorLog << "getTrainingFoldData(const UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K) function first before calling this function!" << std::endl;
       return trainingData;
    }

    if( foldIndex >= kFoldValue ) return trainingData;

    trainingData.setNumDimensions( numDimensions );
    
    //Work out how many samples will be in this fold
    UINT numSamples = 0;
    for(UINT k=0; k<kFoldValue; k++){
        if( k != foldIndex ){
            numSamples += (UINT)crossValidationIndexs[k].size();
        }
    }
    trainingData.reserve( numSamples );

    //Add the data to the training set, this will consist of all the data that is NOT in the foldIndex
    UINT index = 0;
    for(UINT k=0; k<kFoldValue; k++){
        if( k != foldIndex ){
            for(UINT i=0; i<crossValidationIndexs[k].size(); i++){

                index = crossValidationIndexs[k][i];
                trainingData.addSample( data[ index ] );
            }
        }
    }

    return trainingData;
}

UnlabelledData UnlabelledData::getTestFoldData(const UINT foldIndex) const{
    UnlabelledData testData;

    if( !crossValidationSetup ) return testData;

    if( foldIndex >= kFoldValue ) return testData;

    //Add the data to the training
    testData.setNumDimensions( numDimensions );
    
    //Work out how many samples will be in this fold
    UINT numSamples = (UINT)crossValidationIndexs[ foldIndex ].size();

    testData.reserve( numSamples );

    UINT index = 0;
	for(UINT i=0; i<crossValidationIndexs[ foldIndex ].size(); i++){

        index = crossValidationIndexs[ foldIndex ][i];
		testData.addSample( data[ index ] );
	}

    return testData;
}

std::string UnlabelledData::getStatsAsString() const{
    std::string statsText;
    statsText += "DatasetName:\t" + datasetName + "\n";
    statsText += "DatasetInfo:\t" + infoText + "\n";
    statsText += "Number of Dimensions:\t" + Util::toString( numDimensions ) + "\n";
    statsText += "Number of Samples:\t" + Util::toString( totalNumSamples ) + "\n";

    Vector< MinMax > ranges = getRanges();

    statsText += "Dataset Ranges:\n";
    for(UINT j=0; j<ranges.size(); j++){
        statsText += "[" + Util::toString( j+1 ) + "] Min:\t" + Util::toString( ranges[j].minValue ) + "\tMax: " + Util::toString( ranges[j].maxValue ) + "\n";
    }

    return statsText;
}

Vector<MinMax> UnlabelledData::getRanges() const{

    //If the dataset should be scaled using the external ranges then return the external ranges
    if( useExternalRanges ) return externalRanges;
    
    Vector< MinMax > ranges(numDimensions);

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
    
Vector< VectorFloat > UnlabelledData::getData() const{
    return data;
}

MatrixDouble UnlabelledData::getDataAsMatrixDouble() const{
    const UINT rows = getNumSamples();
    const UINT cols = getNumDimensions();
    MatrixDouble d(rows,cols);
    
    for(UINT i=0; i<rows; i++){
        for(UINT j=0; j<cols; j++){
            d[i][j] = data[i][j];
        }
    }
    
    return d;
}

MatrixFloat UnlabelledData::getDataAsMatrixFloat() const {
    const UINT rows = getNumSamples();
    const UINT cols = getNumDimensions();
    MatrixFloat d(rows,cols);
    
    for(UINT i=0; i<rows; i++){
        for(UINT j=0; j<cols; j++){
            d[i][j] = data[i][j];
        }
    }
    
    return d;
}

GRT_END_NAMESPACE

