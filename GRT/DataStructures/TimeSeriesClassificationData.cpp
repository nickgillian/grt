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
#include "TimeSeriesClassificationData.h"

GRT_BEGIN_NAMESPACE

TimeSeriesClassificationData::TimeSeriesClassificationData(UINT numDimensions,std::string datasetName,std::string infoText){
    debugLog.setKey("[DEBUG TSCD]");
    errorLog.setKey("[ERROR TSCD]");
    warningLog.setKey("[WARNING TSCD]");

    this->numDimensions = numDimensions;
    this->datasetName = datasetName;
    this->infoText = infoText;
    totalNumSamples = 0;
    crossValidationSetup = false;
    useExternalRanges = false;
    allowNullGestureClass = true;
    if( numDimensions > 0 ){
        setNumDimensions(numDimensions);
    }
}

TimeSeriesClassificationData::TimeSeriesClassificationData(const TimeSeriesClassificationData &rhs){
    
    debugLog.setKey("[DEBUG TSCD]");
    errorLog.setKey("[ERROR TSCD]");
    warningLog.setKey("[WARNING TSCD]");
    
    *this = rhs;
}

TimeSeriesClassificationData::~TimeSeriesClassificationData(){}
    
TimeSeriesClassificationData& TimeSeriesClassificationData::operator=(const TimeSeriesClassificationData &rhs){
    if( this != &rhs){
        this->datasetName = rhs.datasetName;
        this->infoText = rhs.infoText;
        this->numDimensions = rhs.numDimensions;
        this->useExternalRanges = rhs.useExternalRanges;
        this->allowNullGestureClass = rhs.allowNullGestureClass;
        this->crossValidationSetup = rhs.crossValidationSetup;
        this->crossValidationIndexs = rhs.crossValidationIndexs;
        this->totalNumSamples = rhs.totalNumSamples;
        this->data = rhs.data;
        this->classTracker = rhs.classTracker;
        this->externalRanges = rhs.externalRanges;
        this->debugLog = rhs.debugLog;
        this->errorLog = rhs.errorLog;
        this->warningLog = rhs.warningLog;
    }
    return *this;
}

void TimeSeriesClassificationData::clear(){
	totalNumSamples = 0;
	data.clear();
	classTracker.clear();
}

bool TimeSeriesClassificationData::setNumDimensions(const UINT numDimensions){
    if( numDimensions > 0 ){
        //Clear any previous training data
        clear();

        //Set the dimensionality of the training data
        this->numDimensions = numDimensions;

        useExternalRanges = false;
        externalRanges.clear();

        return true;
    }

    errorLog << "setNumDimensions(UINT numDimensions) - The number of dimensions of the dataset must be greater than zero!" << std::endl;
    return false;
}

bool TimeSeriesClassificationData::setDatasetName(const std::string datasetName){

    //Make sure there are no spaces in the std::string
    if( datasetName.find(" ") == std::string::npos ){
        this->datasetName = datasetName;
        return true;
    }

    errorLog << "setDatasetName(std::string datasetName) - The dataset name cannot contain any spaces!" << std::endl;
    return false;
}

bool TimeSeriesClassificationData::setInfoText(const std::string infoText){
    this->infoText = infoText;
    return true;
}

bool TimeSeriesClassificationData::setClassNameForCorrespondingClassLabel(const std::string className,const UINT classLabel){

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            classTracker[i].className = className;
            return true;
        }
    }

    return false;
}
    
bool TimeSeriesClassificationData::setAllowNullGestureClass(const bool allowNullGestureClass){
    this->allowNullGestureClass = allowNullGestureClass;
    return true;
}

bool TimeSeriesClassificationData::addSample(const UINT classLabel,const MatrixFloat &trainingSample){
	
    if( trainingSample.getNumCols() != numDimensions ){
        errorLog << "addSample(UINT classLabel, MatrixFloat trainingSample) - The dimensionality of the training sample (" << trainingSample.getNumCols() << ") does not match that of the dataset (" << numDimensions << ")" << std::endl;
        return false;
    }
    
    //The class label must be greater than zero (as zero is used for the null rejection class label
    if( classLabel == GRT_DEFAULT_NULL_CLASS_LABEL && !allowNullGestureClass ){
        errorLog << "addSample(UINT classLabel, MatrixFloat sample) - the class label can not be 0!" << std::endl;
        return false;
    }

    TimeSeriesClassificationSample newSample(classLabel,trainingSample);
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

UINT TimeSeriesClassificationData::eraseAllSamplesWithClassLabel(const UINT classLabel){
	UINT numExamplesRemoved = 0;
	UINT numExamplesToRemove = 0;

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

bool TimeSeriesClassificationData::removeLastSample(){

	if( totalNumSamples > 0 ){

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

bool TimeSeriesClassificationData::relabelAllSamplesWithClassLabel(const UINT oldClassLabel,const UINT newClassLabel){
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
            data[i].setTrainingSample(newClassLabel, data[i].getData());
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

bool TimeSeriesClassificationData::setExternalRanges(const Vector< MinMax > &externalRanges,const bool useExternalRanges){

    if( externalRanges.size() != numDimensions ) return false;

    this->externalRanges = externalRanges;
    this->useExternalRanges = useExternalRanges;

    return true;
}

bool TimeSeriesClassificationData::enableExternalRangeScaling(const bool useExternalRanges){
    if( externalRanges.size() == numDimensions ){
        this->useExternalRanges = useExternalRanges;
        return true;
    }
    return false;
}

bool TimeSeriesClassificationData::scale(const Float minTarget,const Float maxTarget){
    Vector< MinMax > ranges = getRanges();
    return scale(ranges,minTarget,maxTarget);
}

bool TimeSeriesClassificationData::scale(const Vector<MinMax> &ranges,const Float minTarget,const Float maxTarget){
    if( ranges.size() != numDimensions ) return false;

    //Scale the training data
    for(UINT i=0; i<totalNumSamples; i++){
        for(UINT x=0; x<data[i].getLength(); x++){
            for(UINT j=0; j<numDimensions; j++){
                data[i][x][j] = Util::scale(data[i][x][j],ranges[j].minValue,ranges[j].maxValue,minTarget,maxTarget);
            }
        }
    }

    return true;
}
    
bool TimeSeriesClassificationData::save(const std::string &filename) const{
    
    //Check if the file should be saved as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return saveDatasetToCSVFile( filename );
    }
    
    //Otherwise save it as a custom GRT file
    return saveDatasetToFile( filename );
}

bool TimeSeriesClassificationData::load(const std::string &filename){
    
    //Check if the file should be loaded as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return loadDatasetFromCSVFile( filename );
    }
    
    //Otherwise save it as a custom GRT file
    return loadDatasetFromFile( filename );
}

bool TimeSeriesClassificationData::saveDatasetToFile(const std::string fileName) const{

	std::fstream file;
	file.open(fileName.c_str(), std::ios::out);

	if( !file.is_open() ){
        errorLog << "saveDatasetToFile(std::string fileName) -  Failed to open file!" << std::endl;
		return false;
	}

	file << "GRT_LABELLED_TIME_SERIES_CLASSIFICATION_DATA_FILE_V1.0\n";
    file << "DatasetName: " << datasetName << std::endl;
    file << "InfoText: " << infoText << std::endl;
	file << "NumDimensions: "<<numDimensions << std::endl;
	file << "TotalNumTrainingExamples: "<<totalNumSamples << std::endl;
	file << "NumberOfClasses: "<<classTracker.size() << std::endl;
	file << "ClassIDsAndCounters: " << std::endl;

	for(UINT i=0; i<classTracker.size(); i++){
		file << classTracker[i].classLabel << "\t" << classTracker[i].counter << std::endl;
	}

    file << "UseExternalRanges: " << useExternalRanges << std::endl;

    if( useExternalRanges ){
        for(UINT i=0; i<externalRanges.size(); i++){
            file << externalRanges[i].minValue << "\t" << externalRanges[i].maxValue << std::endl;
        }
    }

	file << "LabelledTimeSeriesTrainingData:\n";

	for(UINT x=0; x<totalNumSamples; x++){
		file << "************TIME_SERIES************\n";
		file << "ClassID: "<<data[x].getClassLabel() << std::endl;
		file << "TimeSeriesLength: "<<data[x].getLength()<< std::endl;
		file << "TimeSeriesData: \n";
		for(UINT i=0; i<data[x].getLength(); i++){
			for(UINT j=0; j<numDimensions; j++){
				file << data[x][i][j];
				if( j<numDimensions-1 ) file << "\t";
			}file << std::endl;
		}
	}

	file.close();
	return true;
}

bool TimeSeriesClassificationData::loadDatasetFromFile(const std::string filename){

	std::fstream file;
	file.open(filename.c_str(), std::ios::in);
	UINT numClasses = 0;
	clear();

	if( !file.is_open() ){
		errorLog << "loadDatasetFromFile(std::string filename) - FILE NOT OPEN!" << std::endl;
		return false;
	}

	std::string word;

	//Check to make sure this is a file with the Training File Format
	file >> word;
	if(word != "GRT_LABELLED_TIME_SERIES_CLASSIFICATION_DATA_FILE_V1.0"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(std::string filename) - Failed to find file header!" << std::endl;
		return false;
	}

    //Get the name of the dataset
	file >> word;
	if(word != "DatasetName:"){
        errorLog << "loadDatasetFromFile(std::string filename) - failed to find DatasetName!" << std::endl;
		file.close();
		return false;
	}
	file >> datasetName;

    file >> word;
	if(word != "InfoText:"){
        errorLog << "loadDatasetFromFile(std::string filename) - failed to find InfoText!" << std::endl;
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
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(std::string filename) - Failed to find NumDimensions!" << std::endl;
		return false;
	}
	file >> numDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if(word != "TotalNumTrainingExamples:"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(std::string filename) - Failed to find TotalNumTrainingExamples!" << std::endl;
		return false;
	}
	file >> totalNumSamples;

	//Get the total number of classes in the training data
	file >> word;
	if(word != "NumberOfClasses:"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(std::string filename) - Failed to find NumberOfClasses!" << std::endl;
		return false;
	}
	file >> numClasses;

	//Resize the class counter buffer and load the counters
	classTracker.resize(numClasses);

	//Get the total number of classes in the training data
	file >> word;
	if(word != "ClassIDsAndCounters:"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(std::string filename) - Failed to find ClassIDsAndCounters!" << std::endl;
		return false;
	}

	for(UINT i=0; i<classTracker.size(); i++){
		file >> classTracker[i].classLabel;
		file >> classTracker[i].counter;
	}

    //Get the UseExternalRanges
	file >> word;
	if(word != "UseExternalRanges:"){
		file.close();
        clear();
        errorLog << "loadDatasetFromFile(std::string filename) - Failed to find UseExternalRanges!" << std::endl;
		return false;
	}

    file >> useExternalRanges;

    if( useExternalRanges ){
        externalRanges.resize(numDimensions);
        for(UINT i=0; i<externalRanges.size(); i++){
            file >> externalRanges[i].minValue;
            file >> externalRanges[i].maxValue;
        }
    }

	//Get the main training data
	file >> word;
	if(word != "LabelledTimeSeriesTrainingData:"){
        file.close();
        clear();
        errorLog << "loadDatasetFromFile(std::string filename) - Failed to find LabelledTimeSeriesTrainingData!" << std::endl;
		return false;
	}

	//Reset the memory
	data.resize( totalNumSamples, TimeSeriesClassificationSample() );

	//Load each of the time series
	for(UINT x=0; x<totalNumSamples; x++){
		UINT classLabel = 0;
		UINT timeSeriesLength = 0;

		file >> word;
		if( word != "************TIME_SERIES************" ){
			file.close();
            clear();
            errorLog << "loadDatasetFromFile(std::string filename) - Failed to find TimeSeries Header!" << std::endl;
            return false;
		}

		file >> word;
		if( word != "ClassID:" ){
			file.close();
            clear();
            errorLog << "loadDatasetFromFile(std::string filename) - Failed to find ClassID!" << std::endl;
            return false;
		}
		file >> classLabel;

		file >> word;
		if( word != "TimeSeriesLength:" ){
			file.close();
            clear();
            errorLog << "loadDatasetFromFile(std::string filename) - Failed to find TimeSeriesLength!" << std::endl;
            return false;
		}
		file >> timeSeriesLength;

		file >> word;
		if( word != "TimeSeriesData:" ){
			file.close();
            clear();
            errorLog << "loadDatasetFromFile(std::string filename) - Failed to find TimeSeriesData!" << std::endl;
            return false;
		}

		//Load the time series data
		MatrixFloat trainingExample(timeSeriesLength,numDimensions);
		for(UINT i=0; i<timeSeriesLength; i++){
			for(UINT j=0; j<numDimensions; j++){
				file >> trainingExample[i][j];
			}
		}

		data[x].setTrainingSample(classLabel,trainingExample);
	}

	file.close();
	return true;
}
    
bool TimeSeriesClassificationData::saveDatasetToCSVFile(const std::string &filename) const{
    
    std::fstream file;
    file.open(filename.c_str(), std::ios::out );
    
    if( !file.is_open() ){
        return false;
    }
    
    //Write the data to the CSV file
    for(UINT x=0; x<totalNumSamples; x++){
        for(UINT i=0; i<data[x].getLength(); i++){
            file << x+1 << ",";
            file << data[x].getClassLabel() << ",";
            for(UINT j=0; j<numDimensions; j++){
                file << data[x][i][j];
                if( j+1 < numDimensions ){
                    file << ",";
                }
            }
            file << std::endl;
        }
    }
    
    file.close();
    
    return true;
}

bool TimeSeriesClassificationData::loadDatasetFromCSVFile(const std::string &filename){
    
    numDimensions = 0;
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
    
    if( parser.getColumnSize() <= 2 ){
        errorLog << "loadDatasetFromCSVFile(const std::string &filename) - The CSV file does not have enough columns! It should contain at least three columns!" << std::endl;
        return false;
    }
    
    //Set the number of dimensions
    numDimensions = parser.getColumnSize()-2;
    
    //Reserve the memory for the data
    data.reserve( parser.getRowSize() );
    
    UINT sampleCounter = 0;
    UINT lastSampleCounter = 0;
    UINT classLabel = 0;
    UINT j = 0;
    UINT n = 0;
    VectorFloat sample(numDimensions);
    MatrixFloat timeseries;
    for(UINT i=0; i<parser.getRowSize(); i++){
        
        sampleCounter = grt_from_str< UINT >( parser[i][0] );
        
        //Check to see if a new timeseries has started, if so then add the previous time series as a sample and start recording the new time series
        if( sampleCounter != lastSampleCounter && i != 0 ){
            //Add the labelled sample to the dataset
            if( !addSample(classLabel, timeseries) ){
                warningLog << "loadDatasetFromCSVFile(const std::string &filename,const UINT classLabelColumnIndex) - Could not add sample " << i << " to the dataset!" << std::endl;
            }
            timeseries.clear();
        }
        lastSampleCounter = sampleCounter;
        
        //Get the class label
        classLabel = grt_from_str< UINT >( parser[i][1] );
        
        //Get the sample data
        j=0;
        n=2;
        while( j != numDimensions ){
            sample[j++] = grt_from_str< Float >( parser[i][n] );
            n++;
        }
        
        //Add the sample to the timeseries
        timeseries.push_back( sample );
    }
	if ( timeseries.getSize() > 0 )
        //Add the labelled sample to the dataset
        if( !addSample(classLabel, timeseries) ){
            warningLog << "loadDatasetFromCSVFile(const std::string &filename,const UINT classLabelColumnIndex) - Could not add sample " << parser.getRowSize()-1 << " to the dataset!" << std::endl;
        }
    
    return true;
}
    
bool TimeSeriesClassificationData::printStats() const {
    
    std::cout << getStatsAsString();
    
    return true;
}
    
std::string TimeSeriesClassificationData::getStatsAsString() const{
    
    std::string stats;
    
    stats += "DatasetName:\t" + datasetName + "\n";
    stats += "DatasetInfo:\t" + infoText + "\n";
    stats += "Number of Dimensions:\t" + Util::toString(numDimensions) + "\n";
    stats += "Number of Samples:\t" + Util::toString(totalNumSamples) + "\n";
    stats += "Number of Classes:\t" + Util::toString(getNumClasses()) + "\n";
    stats += "ClassStats:\n";
    
    for(UINT k=0; k<getNumClasses(); k++){
        stats += "ClassLabel:\t" + Util::toString(classTracker[k].classLabel);
        stats += "\tNumber of Samples:\t" + Util::toString( classTracker[k].counter );
        stats +="\tClassName:\t" + classTracker[k].className + "\n";
    }
    
    Vector< MinMax > ranges = getRanges();
    
    stats += "Dataset Ranges:\n";
    for(UINT j=0; j<ranges.size(); j++){
        stats += "[" + Util::toString( j+1 ) + "] Min:\t" + Util::toString( ranges[j].minValue ) + "\tMax: " + Util::toString( ranges[j].maxValue ) + "\n";
    }
    
    stats += "Timeseries Lengths:\n";
    UINT M = (UINT)data.size();
    for(UINT j=0; j<M; j++){
        stats += "ClassLabel: " + Util::toString( data[j].getClassLabel() ) + " Length:\t" + Util::toString( data[j].getLength() ) + "\n";
    }
    
    return stats;
}

TimeSeriesClassificationData TimeSeriesClassificationData::partition(const UINT trainingSizePercentage,const bool useStratifiedSampling){
    return split( trainingSizePercentage, useStratifiedSampling );
}

TimeSeriesClassificationData TimeSeriesClassificationData::split(const UINT trainingSizePercentage,const bool useStratifiedSampling){

    //Partitions the dataset into a training dataset (which is kept by this instance of the TimeSeriesClassificationData) and
    //a testing/validation dataset (which is return as a new instance of the TimeSeriesClassificationData).  The trainingSizePercentage
    //therefore sets the size of the data which remains in this instance and the remaining percentage of data is then added to
    //the testing/validation dataset

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

    TimeSeriesClassificationData trainingSet(numDimensions);
    TimeSeriesClassificationData testSet(numDimensions);
    trainingSet.setAllowNullGestureClass(allowNullGestureClass);
    testSet.setAllowNullGestureClass(allowNullGestureClass);
    Vector< UINT > indexs( totalNumSamples );

    //Create the random partion indexs
    Random random;
    UINT randomIndex = 0;

    if( useStratifiedSampling ){
        //Break the data into seperate classes
        Vector< Vector< UINT > > classData( getNumClasses() );

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
                SWAP( classData[k][ x ] ,classData[k][ randomIndex ] );
            }
        }

        //Loop over each class and add the data to the trainingSet and testSet
        for(UINT k=0; k<getNumClasses(); k++){
            UINT numTrainingExamples = (UINT) floor( Float(classData[k].size()) / 100.0 * Float(trainingSizePercentage) );

            //Add the data to the training and test sets
            for(UINT i=0; i<numTrainingExamples; i++){
                trainingSet.addSample( data[ classData[k][i] ].getClassLabel(), data[ classData[k][i] ].getData() );
            }
            for(UINT i=numTrainingExamples; i<classData[k].size(); i++){
                testSet.addSample( data[ classData[k][i] ].getClassLabel(), data[ classData[k][i] ].getData() );
            }
        }

        //Overwrite the training data in this instance with the training data of the trainingSet
        data = trainingSet.getClassificationData();
        totalNumSamples = trainingSet.getNumSamples();
    }else{

        const UINT numTrainingExamples = (UINT) floor( Float(totalNumSamples) / 100.0 * Float(trainingSizePercentage) );
        //Create the random partion indexs
        Random random;
        for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
        for(UINT x=0; x<totalNumSamples; x++){
            //Pick a random index
            randomIndex = random.getRandomNumberInt(0,totalNumSamples);

            //Swap the indexs
            SWAP( indexs[ x ] , indexs[ randomIndex ] );
        }

        //Add the data to the training and test sets
        for(UINT i=0; i<numTrainingExamples; i++){
            trainingSet.addSample( data[ indexs[i] ].getClassLabel(), data[ indexs[i] ].getData() );
        }
        for(UINT i=numTrainingExamples; i<totalNumSamples; i++){
            testSet.addSample( data[ indexs[i] ].getClassLabel(), data[ indexs[i] ].getData() );
        }

        //Overwrite the training data in this instance with the training data of the trainingSet
        data = trainingSet.getClassificationData();
        totalNumSamples = trainingSet.getNumSamples();
    }

    return testSet;
}

bool TimeSeriesClassificationData::merge(const TimeSeriesClassificationData &labelledData){

    if( labelledData.getNumDimensions() != numDimensions ){
        errorLog << "merge(TimeSeriesClassificationData &labelledData) - The number of dimensions in the labelledData (" << labelledData.getNumDimensions() << ") does not match the number of dimensions of this dataset (" << numDimensions << ")" << std::endl;
        return false;
    }

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //Add the data from the labelledData to this instance
    for(UINT i=0; i<labelledData.getNumSamples(); i++){
        addSample(labelledData[i].getClassLabel(), labelledData[i].getData());
    }

    //Set the class names from the dataset
    Vector< ClassTracker > classTracker = labelledData.getClassTracker();
    for(UINT i=0; i<classTracker.size(); i++){
        setClassNameForCorrespondingClassLabel(classTracker[i].className, classTracker[i].classLabel);
    }

    return true;
}

bool TimeSeriesClassificationData::spiltDataIntoKFolds(const UINT K,const bool useStratifiedSampling){

    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //K can not be zero
    if( K == 0 ){
        errorLog << "spiltDataIntoKFolds(UINT K) - K can not be zero!" << std::endl;
        return false;
    }

    //K can not be larger than the number of examples
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) - K can not be larger than the total number of samples in the dataset!" << std::endl;
        return false;
    }

    //K can not be larger than the number of examples in a specific class if the stratified sampling option is true
    if( useStratifiedSampling ){
        for(UINT c=0; c<classTracker.size(); c++){
            if( K > classTracker[c].counter ){
                errorLog << "spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) - K can not be larger than the number of samples in any given class!" << std::endl;
                return false;
            }
        }
    }

    //Setup the dataset for k-fold cross validation
    kFoldValue = K;
    Vector< UINT > indexs( totalNumSamples );

    //Work out how many samples are in each fold, the last fold might have more samples than the others
    UINT numSamplesPerFold = (UINT) floor( totalNumSamples/Float(K) );

    //Resize the cross validation indexs buffer
    crossValidationIndexs.resize( K );

    //Create the random partion indexs
    Random random;
    UINT randomIndex = 0;

    if( useStratifiedSampling ){
        //Break the data into seperate classes
        Vector< Vector< UINT > > classData( getNumClasses() );

        //Add the indexs to their respective classes
        for(UINT i=0; i<totalNumSamples; i++){
            classData[ getClassLabelIndexValue( data[i].getClassLabel() ) ].push_back( i );
        }

        //Randomize the order of the indexs in each of the class index buffers
        for(UINT c=0; c<getNumClasses(); c++){
            UINT numSamples = (UINT)classData[c].size();
            for(UINT x=0; x<numSamples; x++){
                //Pick a random index
                randomIndex = random.getRandomNumberInt(0,numSamples);

                //Swap the indexs
                SWAP( classData[c][ x ] , classData[c][ randomIndex ] );
            }
        }

        //Loop over each of the classes and add the data equally to each of the k folds until there is no data left
        Vector< UINT >::iterator iter;
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
    }

    crossValidationSetup = true;
    return true;

}

TimeSeriesClassificationData TimeSeriesClassificationData::getTrainingFoldData(const UINT foldIndex) const {

    TimeSeriesClassificationData trainingData;

    if( !crossValidationSetup ){
        errorLog << "getTrainingFoldData(UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) function first before calling this function!" << std::endl;
        return trainingData;
    }

    if( foldIndex >= kFoldValue ) return trainingData;

    trainingData.setNumDimensions( numDimensions );

    //Add the data to the training set, this will consist of all the data that is NOT in the foldIndex
    UINT index = 0;
    for(UINT k=0; k<kFoldValue; k++){
        if( k != foldIndex ){
            for(UINT i=0; i<crossValidationIndexs[k].size(); i++){

                index = crossValidationIndexs[k][i];
                trainingData.addSample( data[ index ].getClassLabel(), data[ index ].getData() );
            }
        }
    }

    return trainingData;
}

TimeSeriesClassificationData TimeSeriesClassificationData::getTestFoldData(const UINT foldIndex) const {
    TimeSeriesClassificationData testData;

    if( !crossValidationSetup ) return testData;

    if( foldIndex >= kFoldValue ) return testData;

    //Add the data to the training
    testData.setNumDimensions( numDimensions );

    UINT index = 0;
    for(UINT i=0; i<crossValidationIndexs[ foldIndex ].size(); i++){

        index = crossValidationIndexs[ foldIndex ][i];
        testData.addSample( data[ index ].getClassLabel(), data[ index ].getData() );
    }

    return testData;
}

TimeSeriesClassificationData TimeSeriesClassificationData::getClassData(const UINT classLabel) const {
    TimeSeriesClassificationData classData(numDimensions);
    for(UINT x=0; x<totalNumSamples; x++){
        if( data[x].getClassLabel() == classLabel ){
            classData.addSample( classLabel, data[x].getData() );
        }
    }
    return classData;
}

UnlabelledData TimeSeriesClassificationData::reformatAsUnlabelledData() const {

    UnlabelledData unlabelledData;

    if( totalNumSamples == 0 ){
        return unlabelledData;
    }

    unlabelledData.setNumDimensions( numDimensions );

    for(UINT i=0; i<totalNumSamples; i++){
        for(UINT x=0; x<data[i].getLength(); x++){
            unlabelledData.addSample( data[i].getData().getRow( x ) );
        }
    }

    return unlabelledData;
}

UINT TimeSeriesClassificationData::getMinimumClassLabel() const {
    UINT minClassLabel = 99999;

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel < minClassLabel ){
            minClassLabel = classTracker[i].classLabel;
        }
    }

    return minClassLabel;
}


UINT TimeSeriesClassificationData::getMaximumClassLabel() const {
    UINT maxClassLabel = 0;

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel > maxClassLabel ){
            maxClassLabel = classTracker[i].classLabel;
        }
    }

    return maxClassLabel;
}

UINT TimeSeriesClassificationData::getClassLabelIndexValue(const UINT classLabel) const {
    for(UINT k=0; k<classTracker.size(); k++){
        if( classTracker[k].classLabel == classLabel ){
            return k;
        }
    }
    warningLog << "getClassLabelIndexValue(UINT classLabel) - Failed to find class label: " << classLabel << " in class tracker!" << std::endl;
    return 0;
}

std::string TimeSeriesClassificationData::getClassNameForCorrespondingClassLabel(const UINT classLabel) const {

    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            return classTracker[i].className;
        }
    }
    return "CLASS_LABEL_NOT_FOUND";
}

Vector<MinMax> TimeSeriesClassificationData::getRanges() const {

    if( useExternalRanges ) return externalRanges;

    Vector<MinMax> ranges(numDimensions);

    if( totalNumSamples > 0 ){
        for(UINT j=0; j<numDimensions; j++){
            ranges[j].minValue = data[0][0][0];
            ranges[j].maxValue = data[0][0][0];
            for(UINT x=0; x<totalNumSamples; x++){
                for(UINT i=0; i<data[x].getLength(); i++){
                    if( data[x][i][j] < ranges[j].minValue ){ ranges[j].minValue = data[x][i][j]; }		//Search for the min value
                    else if( data[x][i][j] > ranges[j].maxValue ){ ranges[j].maxValue = data[x][i][j]; }	//Search for the max value
                }
            }
        }
    }
    return ranges;
}
    
MatrixFloat TimeSeriesClassificationData::getDataAsMatrixFloat() const {
    
    //Count how many samples are in the entire dataset
    UINT M = 0;
    UINT index = 0;
    for(UINT x=0; x<totalNumSamples; x++){
        M += data[x].getLength();
    }
    
    if( M == 0 ) MatrixFloat();
    
    //Get all the data and concatenate it into 1 matrix
    MatrixFloat matrixData(M,numDimensions);
    for(UINT x=0; x<totalNumSamples; x++){
        for(UINT i=0; i<data[x].getLength(); i++){
            for(UINT j=0; j<numDimensions; j++){
                matrixData[index][j] = data[x][i][j];
            }
            index++;
        }
    }
    return matrixData;
}

GRT_END_NAMESPACE

