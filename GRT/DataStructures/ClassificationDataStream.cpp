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
#include "ClassificationDataStream.h"

GRT_BEGIN_NAMESPACE

//Constructors and Destructors
ClassificationDataStream::ClassificationDataStream(const UINT numDimensions,const std::string datasetName,const std::string infoText){
    
    this->numDimensions= numDimensions;
    this->datasetName = datasetName;
    this->infoText = infoText;
    
    playbackIndex  = 0;
    trackingClass = false;
    useExternalRanges = false;
    debugLog.setKey("[DEBUG ClassificationDataStream]");
    errorLog.setKey("[ERROR ClassificationDataStream]");
    warningLog.setKey("[WARNING ClassificationDataStream]");
    
    if( numDimensions > 0 ){
        setNumDimensions(numDimensions);
    }
}

ClassificationDataStream::ClassificationDataStream(const ClassificationDataStream &rhs){
    *this = rhs;
}

ClassificationDataStream::~ClassificationDataStream(){}
    
ClassificationDataStream& ClassificationDataStream::operator=(const ClassificationDataStream &rhs){
    if( this != &rhs){
        this->datasetName = rhs.datasetName;
        this->infoText = rhs.infoText;
        this->numDimensions = rhs.numDimensions;
        this->totalNumSamples = rhs.totalNumSamples;
        this->lastClassID = rhs.lastClassID;
        this->playbackIndex = rhs.playbackIndex;
        this->trackingClass = rhs.trackingClass;
        this->useExternalRanges = rhs.useExternalRanges;
        this->externalRanges = rhs.externalRanges;
        this->data = rhs.data;
        this->classTracker = rhs.classTracker;
        this->timeSeriesPositionTracker = rhs.timeSeriesPositionTracker;
        this->debugLog = rhs.debugLog;
        this->warningLog = rhs.warningLog;
        this->errorLog = rhs.errorLog;
        
    }
    return *this;
}

void ClassificationDataStream::clear(){
	totalNumSamples = 0;
	playbackIndex = 0;
	trackingClass = false;
	data.clear();
	classTracker.clear();
	timeSeriesPositionTracker.clear();
}
    
bool ClassificationDataStream::setNumDimensions(const UINT numDimensions){
    if( numDimensions > 0 ){
        //Clear any previous data
        clear();
        
        //Set the dimensionality of the time series data
        this->numDimensions = numDimensions;
        
        return true;
    }
    
    errorLog << "setNumDimensions(const UINT numDimensions) - The number of dimensions of the dataset must be greater than zero!" << std::endl;
    return false;
}
    
    
bool ClassificationDataStream::setDatasetName(const std::string datasetName){
    
    //Make sure there are no spaces in the std::string
    if( datasetName.find(" ") == std::string::npos ){
        this->datasetName = datasetName;
        return true;
    }
    
    errorLog << "setDatasetName(const std::string datasetName) - The dataset name cannot contain any spaces!" << std::endl;
    return false;
}
    
bool ClassificationDataStream::setInfoText(std::string infoText){
    this->infoText = infoText;
    return true;
}
    
bool ClassificationDataStream::setClassNameForCorrespondingClassLabel(const std::string className,const UINT classLabel){
    
    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            classTracker[i].className = className;
            return true;
        }
    }
    
    errorLog << "setClassNameForCorrespondingClassLabel(const std::string className,const UINT classLabel) - Failed to find class with label: " << classLabel << std::endl;
    return false;
}

bool ClassificationDataStream::addSample(const UINT classLabel,const VectorFloat &sample){

	if( numDimensions != sample.size() ){
		errorLog << "addSample(const UINT classLabel, VectorFloat sample) - the size of the new sample (" << sample.size() << ") does not match the number of dimensions of the dataset (" << numDimensions << ")" << std::endl;
        return false;
	}

	bool searchForNewClass = true;
	if( trackingClass ){
		if( classLabel != lastClassID ){
			//The class ID has changed so update the time series tracker
			timeSeriesPositionTracker[ timeSeriesPositionTracker.size()-1 ].setEndIndex( totalNumSamples-1 );
		}else searchForNewClass = false;
	}
	
	if( searchForNewClass ){
		bool newClass = true;
		//Search to see if this class has been found before
		for(UINT k=0; k<classTracker.size(); k++){
			if( classTracker[k].classLabel == classLabel ){
				newClass = false;
				classTracker[k].counter++;
			}
		}
		if( newClass ){
			ClassTracker newCounter(classLabel,1);
			classTracker.push_back( newCounter );
		}

		//Set the timeSeriesPositionTracker start position
		trackingClass = true;
		lastClassID = classLabel;
		TimeSeriesPositionTracker newTracker(totalNumSamples,0,classLabel);
		timeSeriesPositionTracker.push_back( newTracker );
	}

	ClassificationSample labelledSample(classLabel,sample);
	data.push_back( labelledSample );
	totalNumSamples++;
	return true;
}

bool ClassificationDataStream::addSample(const UINT classLabel,const MatrixFloat &sample){

    if( numDimensions != sample.getNumCols() ){
        errorLog << "addSample(const UINT classLabel, const MatrixFloat &sample) - the number of columns in the sample (" << sample.getNumCols() << ") does not match the number of dimensions of the dataset (" << numDimensions << ")" << std::endl;
        return false;
    }

    bool searchForNewClass = true;
    if( trackingClass ){
        if( classLabel != lastClassID ){
            //The class ID has changed so update the time series tracker
            timeSeriesPositionTracker[ timeSeriesPositionTracker.size()-1 ].setEndIndex( totalNumSamples-1 );
        }else searchForNewClass = false;
    }
    
    if( searchForNewClass ){
        bool newClass = true;
        //Search to see if this class has been found before
        for(UINT k=0; k<classTracker.size(); k++){
            if( classTracker[k].classLabel == classLabel ){
                newClass = false;
                classTracker[k].counter += sample.getNumRows();
            }
        }
        if( newClass ){
            ClassTracker newCounter(classLabel,1);
            classTracker.push_back( newCounter );
        }

        //Set the timeSeriesPositionTracker start position
        trackingClass = true;
        lastClassID = classLabel;
        TimeSeriesPositionTracker newTracker(totalNumSamples,0,classLabel);
        timeSeriesPositionTracker.push_back( newTracker );
    }

    ClassificationSample labelledSample( numDimensions );
    for(UINT i=0; i<sample.getNumRows(); i++){
        data.push_back( labelledSample );
        data.back().setClassLabel( classLabel );
        for(UINT j=0; j<numDimensions; j++){
            data.back()[j] = sample[i][j];
        }
    }
    totalNumSamples += sample.getNumRows();
    return true;

}
    
bool ClassificationDataStream::removeLastSample(){
    
    if( totalNumSamples > 0 ){
        
        //Find the corresponding class ID for the last training example
        UINT classLabel = data[ totalNumSamples-1 ].getClassLabel();
        
        //Remove the training example from the buffer
        data.erase( data.end()-1 );
        
        totalNumSamples = (UINT)data.size();
        
        //Remove the value from the counter
        for(UINT i=0; i<classTracker.size(); i++){
            if( classTracker[i].classLabel == classLabel ){
                classTracker[i].counter--;
                break;
            }
        }	
        
        //If we are not tracking a class then decrement the end index of the timeseries position tracker
        if( !trackingClass ){
            UINT endIndex = timeSeriesPositionTracker[ timeSeriesPositionTracker.size()-1 ].getEndIndex();
            timeSeriesPositionTracker[ timeSeriesPositionTracker.size()-1 ].setEndIndex( endIndex-1 );
        }
        
        return true;
        
    }else return false;
    
}
    
UINT ClassificationDataStream::eraseAllSamplesWithClassLabel(const UINT classLabel){
    UINT numExamplesRemoved = 0;
    UINT numExamplesToRemove = 0;
    
    //Find out how many training examples we need to remove    
    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            numExamplesToRemove = classTracker[i].counter;
            classTracker.erase(classTracker.begin()+i);
            break; //There should only be one class with this classLabel so break
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
    
    //Update the time series position tracker
    Vector< TimeSeriesPositionTracker >::iterator iter = timeSeriesPositionTracker.begin();
    
    while( iter != timeSeriesPositionTracker.end() ){
        if( iter->getClassLabel() == classLabel ){
            UINT length = iter->getLength();
            //Update the start and end positions of all the following position trackers
            Vector< TimeSeriesPositionTracker >::iterator updateIter = iter + 1;
            
            while( updateIter != timeSeriesPositionTracker.end() ){
                updateIter->setStartIndex( updateIter->getStartIndex() - length );
                updateIter->setEndIndex( updateIter->getEndIndex() - length );
                updateIter++;
            }
            
            //Erase the current position tracker
            iter = timeSeriesPositionTracker.erase( iter );
        }else iter++;
    }
    
    totalNumSamples = (UINT)data.size();
    
    return numExamplesRemoved;
}
    
bool ClassificationDataStream::relabelAllSamplesWithClassLabel(const UINT oldClassLabel,const UINT newClassLabel){
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
    
    //Update the timeseries position tracker
    for(UINT i=0; i<timeSeriesPositionTracker.size(); i++){
        if( timeSeriesPositionTracker[i].getClassLabel() == oldClassLabel ){
            timeSeriesPositionTracker[i].setClassLabel( newClassLabel );
        }
    }
    
    return true;
}
    
bool ClassificationDataStream::setExternalRanges(const Vector< MinMax > &externalRanges, const bool useExternalRanges){
    
    if( externalRanges.size() != numDimensions ) return false;
    
    this->externalRanges = externalRanges;
    this->useExternalRanges = useExternalRanges;
    
    return true;
}

bool ClassificationDataStream::enableExternalRangeScaling(const bool useExternalRanges){
    if( externalRanges.size() == numDimensions ){
        this->useExternalRanges = useExternalRanges;
        return true;
    }
    return false;
}

bool ClassificationDataStream::scale(const Float minTarget,const Float maxTarget){
    Vector< MinMax > ranges = getRanges();
    return scale(ranges,minTarget,maxTarget);
}

bool ClassificationDataStream::scale(const Vector<MinMax> &ranges,const Float minTarget,const Float maxTarget){
    if( ranges.size() != numDimensions ) return false;
    
    //Scale the training data
    for(UINT i=0; i<totalNumSamples; i++){
        for(UINT j=0; j<numDimensions; j++){
            data[i][j] = Util::scale(data[i][j],ranges[j].minValue,ranges[j].maxValue,minTarget,maxTarget);
        }
    }
    return true;
}

bool ClassificationDataStream::resetPlaybackIndex(const UINT playbackIndex){
    if( playbackIndex < totalNumSamples ){
        this->playbackIndex = playbackIndex;
        return true;
    }
    return false;
}

ClassificationSample ClassificationDataStream::getNextSample(){
    if( totalNumSamples == 0 ) return ClassificationSample();
    
    UINT index = playbackIndex++ % totalNumSamples;
    return data[ index ];
}
    
TimeSeriesClassificationData ClassificationDataStream::getAllTrainingExamplesWithClassLabel(const UINT classLabel) const {
	TimeSeriesClassificationData classData(numDimensions);
	for(UINT x=0; x<timeSeriesPositionTracker.size(); x++){
		if( timeSeriesPositionTracker[x].getClassLabel() == classLabel && timeSeriesPositionTracker[x].getEndIndex() > 0){
			Matrix<Float> timeSeries;
			for(UINT i=timeSeriesPositionTracker[x].getStartIndex(); i<timeSeriesPositionTracker[x].getEndIndex(); i++){
				timeSeries.push_back( data[ i ].getSample() );
			}
			classData.addSample(classLabel,timeSeries);
		}
	}
	return classData;
}

UINT ClassificationDataStream::getMinimumClassLabel() const {
    UINT minClassLabel = 99999;
    
    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel < minClassLabel ){
            minClassLabel = classTracker[i].classLabel;
        }
    }
    
    return minClassLabel;
}


UINT ClassificationDataStream::getMaximumClassLabel() const {
    UINT maxClassLabel = 0;
    
    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel > maxClassLabel ){
            maxClassLabel = classTracker[i].classLabel;
        }
    }
    
    return maxClassLabel;
}

UINT ClassificationDataStream::getClassLabelIndexValue(const UINT classLabel) const {
    for(UINT k=0; k<classTracker.size(); k++){
        if( classTracker[k].classLabel == classLabel ){
            return k;
        }
    }
    warningLog << "getClassLabelIndexValue(const UINT classLabel) - Failed to find class label: " << classLabel << " in class tracker!" << std::endl;
    return 0;
}

std::string ClassificationDataStream::getClassNameForCorrespondingClassLabel(const UINT classLabel){
    
    for(UINT i=0; i<classTracker.size(); i++){
        if( classTracker[i].classLabel == classLabel ){
            return classTracker[i].className;
        }
    }
    return "CLASS_LABEL_NOT_FOUND";
}

Vector<MinMax> ClassificationDataStream::getRanges() const {
    
    Vector< MinMax > ranges(numDimensions);
    
    //If the dataset should be scaled using the external ranges then return the external ranges
    if( useExternalRanges ) return externalRanges;
    
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
    
bool ClassificationDataStream::save(const std::string &filename){
    
    //Check if the file should be saved as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return saveDatasetToCSVFile( filename );
    }
    
    //Otherwise save it as a custom GRT file
    return saveDatasetToFile( filename );
}

bool ClassificationDataStream::load(const std::string &filename){
    
    //Check if the file should be loaded as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return loadDatasetFromCSVFile( filename );
    }
    
    //Otherwise save it as a custom GRT file
    return loadDatasetFromFile( filename );
}

bool ClassificationDataStream::saveDatasetToFile(const std::string &filename) {

    std::fstream file;
    file.open(filename.c_str(), std::ios::out);

    if( !file.is_open() ){
        errorLog << "saveDatasetToFile(const std::string &filename) - Failed to open file!" << std::endl;
        return false;
    }

    if( trackingClass ){
        //The class tracker was not stopped so assume the last sample is the end
        trackingClass = false;
        timeSeriesPositionTracker[ timeSeriesPositionTracker.size()-1 ].setEndIndex( totalNumSamples-1 );
    }

    file << "GRT_LABELLED_CONTINUOUS_TIME_SERIES_CLASSIFICATION_FILE_V1.0\n";
    file << "DatasetName: " << datasetName << std::endl;
    file << "InfoText: " << infoText << std::endl;
    file << "NumDimensions: " << numDimensions << std::endl;
    file << "TotalNumSamples: " << totalNumSamples << std::endl;
    file << "NumberOfClasses: " << classTracker.size() << std::endl;
    file << "ClassIDsAndCounters: " << std::endl;
    for(UINT i=0; i<classTracker.size(); i++){
        file << classTracker[i].classLabel << "\t" << classTracker[i].counter << std::endl;
    }

    file << "NumberOfPositionTrackers: " << timeSeriesPositionTracker.size() << std::endl;
    file << "TimeSeriesPositionTrackers: " << std::endl;
    for(UINT i=0; i<timeSeriesPositionTracker.size(); i++){
        file << timeSeriesPositionTracker[i].getClassLabel() << "\t" << timeSeriesPositionTracker[i].getStartIndex() << "\t" << timeSeriesPositionTracker[i].getEndIndex() << std::endl;
    }
    
    file << "UseExternalRanges: " << useExternalRanges << std::endl;
    
    if( useExternalRanges ){
        for(UINT i=0; i<externalRanges.size(); i++){
            file << externalRanges[i].minValue << "\t" << externalRanges[i].maxValue << std::endl;
        }
    }

    file << "LabelledContinuousTimeSeriesClassificationData:\n";
    for(UINT i=0; i<totalNumSamples; i++){
        file << data[i].getClassLabel();
        for(UINT j=0; j<numDimensions; j++){
            file << "\t" << data[i][j];
        }
        file << std::endl;
    }

    file.close();
    return true;
}

bool ClassificationDataStream::loadDatasetFromFile(const std::string &filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
	UINT numClasses = 0;
	UINT numTrackingPoints = 0;
	clear();

	if( !file.is_open() ){
		errorLog<< "loadDatasetFromFile(string fileName) - Failed to open file!" << std::endl;
		return false;
	}

	std::string word;

	//Check to make sure this is a file with the Training File Format
	file >> word;
	if(word != "GRT_LABELLED_CONTINUOUS_TIME_SERIES_CLASSIFICATION_FILE_V1.0"){
		file.close();
        errorLog<< "loadDatasetFromFile(string fileName) - Failed to find file header!" << std::endl;
		return false;
	}
    
    //Get the name of the dataset
	file >> word;
	if(word != "DatasetName:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find DatasetName!" << std::endl;
		file.close();
		return false;
	}
	file >> datasetName;
    
    file >> word;
	if(word != "InfoText:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find InfoText!" << std::endl;
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
        errorLog<< "loadDatasetFromFile(string fileName) - Failed to find NumDimensions!" << std::endl;
		file.close();
		return false;
	}
	file >> numDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if(word != "TotalNumSamples:"){
        errorLog<< "loadDatasetFromFile(string fileName) - Failed to find TotalNumSamples!" << std::endl;
		file.close();
		return false;
	}
	file >> totalNumSamples;

	//Get the total number of classes in the training data
	file >> word;
	if(word != "NumberOfClasses:"){
        errorLog<< "loadDatasetFromFile(string fileName) - Failed to find NumberOfClasses!" << std::endl;
		file.close();
		return false;
	}
	file >> numClasses;

	//Resize the class counter buffer and load the counters
	classTracker.resize(numClasses);

	//Get the total number of classes in the training data
	file >> word;
	if(word != "ClassIDsAndCounters:"){
        errorLog<< "loadDatasetFromFile(string fileName) - Failed to find ClassIDsAndCounters!" << std::endl;
		file.close();
		return false;
	}

	for(UINT i=0; i<classTracker.size(); i++){
		file >> classTracker[i].classLabel;
		file >> classTracker[i].counter;
	}

	//Get the NumberOfPositionTrackers
	file >> word;
	if(word != "NumberOfPositionTrackers:"){
        errorLog<< "loadDatasetFromFile(string fileName) - Failed to find NumberOfPositionTrackers!" << std::endl;
		file.close();
		return false;
	}
	file >> numTrackingPoints;
	timeSeriesPositionTracker.resize( numTrackingPoints );

	//Get the TimeSeriesPositionTrackers
	file >> word;
	if(word != "TimeSeriesPositionTrackers:"){
        errorLog<< "loadDatasetFromFile(string fileName) - Failed to find TimeSeriesPositionTrackers!" << std::endl;
		file.close();
		return false;
	}

	for(UINT i=0; i<timeSeriesPositionTracker.size(); i++){
		UINT classLabel;
		UINT startIndex;
		UINT endIndex;
		file >> classLabel;
		file >> startIndex;
		file >> endIndex;
		timeSeriesPositionTracker[i].setTracker(startIndex,endIndex,classLabel);
	}
    
    //Check if the dataset should be scaled using external ranges
	file >> word;
	if(word != "UseExternalRanges:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find DatasetName!" << std::endl;
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
	
	//Get the main time series data
	file >> word;
	if(word != "LabelledContinuousTimeSeriesClassificationData:"){
        errorLog<< "loadDatasetFromFile(string fileName) - Failed to find LabelledContinuousTimeSeriesClassificationData!" << std::endl;
		file.close();
		return false;
	}

	//Reset the memory
	data.resize( totalNumSamples, ClassificationSample() );

	//Load each sample
    UINT classLabel = 0;
    VectorFloat sample(numDimensions);
	for(UINT i=0; i<totalNumSamples; i++){

		file >> classLabel;
		for(UINT j=0; j<numDimensions; j++){
			file >> sample[j];
		}

		data[i].set(classLabel,sample);
	}

	file.close();
	return true;
}
    
bool ClassificationDataStream::saveDatasetToCSVFile(const std::string &filename) {
    std::fstream file; 
    file.open(filename.c_str(), std::ios::out );
    
    if( !file.is_open() ){
        return false;
    }
    
    //Write the data to the CSV file

    for(UINT i=0; i<data.size(); i++){
        file << data[i].getClassLabel();
        for(UINT j=0; j<numDimensions; j++){
            file << "," << data[i][j];
        }
        file << std::endl;
    }
    
    file.close();
    
    return true;
}

bool ClassificationDataStream::loadDatasetFromCSVFile(const std::string &filename,const UINT classLabelColumnIndex){

    datasetName = "NOT_SET";
    infoText = "";
    
    //Clear any previous data
    clear();
    
    //Parse the CSV file
    FileParser parser;
    
    if( !parser.parseCSVFile(filename,true) ){
        errorLog << "loadDatasetFromCSVFile(const std::string filename,const UINT classLabelColumnIndex) - Failed to parse CSV file!" << std::endl;
        return false;
    }
    
    if( !parser.getConsistentColumnSize() ){
        errorLog << "loadDatasetFromCSVFile(const std::string filename,const UINT classLabelColumnIndex) - The CSV file does not have a consistent number of columns!" << std::endl;
        return false;
    }
    
    if( parser.getColumnSize() <= 1 ){
        errorLog << "loadDatasetFromCSVFile(const std::string filename,const UINT classLabelColumnIndex) - The CSV file does not have enough columns! It should contain at least two columns!" << std::endl;
        return false;
    }
    
    //Set the number of dimensions
    numDimensions = parser.getColumnSize()-1;
    UINT classLabel = 0;
    UINT j = 0;
    UINT n = 0;
    VectorFloat sample(numDimensions);
    for(UINT i=0; i<parser.getRowSize(); i++){
        //Get the class label
        classLabel = Util::stringToInt( parser[i][classLabelColumnIndex] );
        
        //Get the sample data
        j=0;
        n=0;
        while( j != numDimensions ){
            if( n != classLabelColumnIndex ){
                sample[j++] = Util::stringToFloat( parser[i][n] );
            }
            n++;
        }
        
        //Add the labelled sample to the dataset
        if( !addSample(classLabel, sample) ){
            warningLog << "loadDatasetFromCSVFile(const std::string filename,const UINT classLabelColumnIndex) - Could not add sample " << i << " to the dataset!" << std::endl;
        }
    }

    return true;
}
    
bool ClassificationDataStream::printStats() const {
    
    std::cout << "DatasetName:\t" << datasetName << std::endl;
    std::cout << "DatasetInfo:\t" << infoText << std::endl;
    std::cout << "Number of Dimensions:\t" << numDimensions << std::endl;
    std::cout << "Number of Samples:\t" << totalNumSamples << std::endl;
    std::cout << "Number of Classes:\t" << getNumClasses() << std::endl;
    std::cout << "ClassStats:\n";
    
    for(UINT k=0; k<getNumClasses(); k++){
        std::cout << "ClassLabel:\t" << classTracker[k].classLabel;
        std::cout << "\tNumber of Samples:\t" << classTracker[k].counter;
        std::cout << "\tClassName:\t" << classTracker[k].className << std::endl;
    }
    
    std::cout << "TimeSeriesMarkerStats:\n";
    for(UINT i=0; i<timeSeriesPositionTracker.size(); i++){
        std::cout << "ClassLabel: " << timeSeriesPositionTracker[i].getClassLabel();
        std::cout << "\tStartIndex: " << timeSeriesPositionTracker[i].getStartIndex();
        std::cout << "\tEndIndex: " << timeSeriesPositionTracker[i].getEndIndex();
        std::cout << "\tLength: " << timeSeriesPositionTracker[i].getLength() << std::endl;
    }
    
    Vector< MinMax > ranges = getRanges();
    
    std::cout << "Dataset Ranges:\n";
    for(UINT j=0; j<ranges.size(); j++){
        std::cout << "[" << j+1 << "] Min:\t" << ranges[j].minValue << "\tMax: " << ranges[j].maxValue << std::endl;
    }
    
    return true;
}
    
ClassificationDataStream ClassificationDataStream::getSubset(const UINT startIndex,const UINT endIndex) const {
    
    ClassificationDataStream subset;
    
    if( endIndex >= totalNumSamples ){
        warningLog << "getSubset(const UINT startIndex,const UINT endIndex) - The endIndex is greater than or equal to the number of samples in the current dataset!" << std::endl;
        return subset;
    }
    
    if( startIndex >= endIndex ){
        warningLog << "getSubset(const UINT startIndex,const UINT endIndex) - The startIndex is greater than or equal to the endIndex!" << std::endl;
        return subset;
    }
    
    //Set the header info
    subset.setNumDimensions( getNumDimensions() );
    subset.setDatasetName( getDatasetName() );
    subset.setInfoText( getInfoText() );
    
    //Add the data
    for(UINT i=startIndex; i<=endIndex; i++){
        subset.addSample(data[i].getClassLabel(), data[i].getSample());
    }
    
    return subset;
}
    
TimeSeriesClassificationData ClassificationDataStream::getTimeSeriesClassificationData( const bool includeNullGestures ) const {
    
    TimeSeriesClassificationData tsData;
    
    tsData.setNumDimensions( getNumDimensions() );
    tsData.setAllowNullGestureClass( includeNullGestures );
    
    bool addSample = false;
    const UINT numTimeseries = (UINT)timeSeriesPositionTracker.size();
    for(UINT i=0; i<numTimeseries; i++){
        addSample = includeNullGestures ? true : timeSeriesPositionTracker[i].getClassLabel() != GRT_DEFAULT_NULL_CLASS_LABEL;
        if( addSample ){
            tsData.addSample(timeSeriesPositionTracker[i].getClassLabel(), getTimeSeriesData( timeSeriesPositionTracker[i] ) );
        }
    }
    
    return tsData;
}
    
ClassificationData ClassificationDataStream::getClassificationData( const bool includeNullGestures ) const {
    
    ClassificationData classificationData;
    
    classificationData.setNumDimensions( getNumDimensions() );
    classificationData.setAllowNullGestureClass( includeNullGestures );

    bool addSample = false;
    for(UINT i=0; i<timeSeriesPositionTracker.size(); i++){
        addSample = includeNullGestures ? true : timeSeriesPositionTracker[i].getClassLabel() != GRT_DEFAULT_NULL_CLASS_LABEL;
        if( addSample ){
            MatrixFloat dataSegment = getTimeSeriesData( timeSeriesPositionTracker[i] );
            for(UINT j=0; j<dataSegment.getNumRows(); j++){
                classificationData.addSample(timeSeriesPositionTracker[i].getClassLabel(), dataSegment.getRow(j) );
            }
        }
    }
    
    return classificationData;
}
    
MatrixFloat ClassificationDataStream::getTimeSeriesData( const TimeSeriesPositionTracker &trackerInfo ) const {
    
    if( trackerInfo.getStartIndex() >= totalNumSamples || trackerInfo.getEndIndex() > totalNumSamples ){
        warningLog << "getTimeSeriesData(TimeSeriesPositionTracker trackerInfo) - Invalid tracker indexs!" << std::endl;
        return MatrixFloat();
    }

    UINT startIndex = trackerInfo.getStartIndex();
    UINT endIndex = trackerInfo.getEndIndex();
    UINT M = endIndex > 0 ? trackerInfo.getLength() : totalNumSamples - startIndex;
    UINT N = getNumDimensions();

    MatrixFloat tsData(M,N);
    for(UINT i=0; i<M; i++){
        for(UINT j=0; j<N; j++){
            tsData[i][j] = data[ i+startIndex ][j];
        }
    }
    return tsData;
}
    
MatrixFloat ClassificationDataStream::getDataAsMatrixFloat() const {
    UINT M = getNumSamples();
    UINT N = getNumDimensions();
    MatrixFloat matrixData(M,N);
    for(UINT i=0; i<M; i++){
        for(UINT j=0; j<N; j++){
            matrixData[i][j] = data[i][j];
        }
    }
    return matrixData;
}
    
Vector< UINT > ClassificationDataStream::getClassLabels() const{
    const UINT K = (UINT)classTracker.size();
    Vector< UINT > classLabels( K );
    
    for(UINT i=0; i<K; i++){
        classLabels[i] = classTracker[i].classLabel;
    }
    
    return classLabels;
}

GRT_END_NAMESPACE

