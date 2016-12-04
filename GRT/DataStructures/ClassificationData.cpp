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
#include "ClassificationData.h"

GRT_BEGIN_NAMESPACE

ClassificationData::ClassificationData(const UINT numDimensions,const std::string datasetName,const std::string infoText){
    this->datasetName = datasetName;
    this->numDimensions = numDimensions;
    this->infoText = infoText;
    totalNumSamples = 0;
    crossValidationSetup = false;
    useExternalRanges = false;
    allowNullGestureClass = true;
    if( numDimensions > 0 ) setNumDimensions( numDimensions );
    infoLog.setKey("[ClassificationData]");
    debugLog.setKey("[DEBUG ClassificationData]");
    errorLog.setKey("[ERROR ClassificationData]");
    warningLog.setKey("[WARNING ClassificationData]");
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
        this->infoLog = rhs.infoLog;
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

    errorLog << "setNumDimensions(const UINT numDimensions) - The number of dimensions of the dataset must be greater than zero!" << std::endl;
    return false;
}

bool ClassificationData::setDatasetName(const std::string datasetName){

    //Make sure there are no spaces in the std::string
    if( datasetName.find(" ") == std::string::npos ){
        this->datasetName = datasetName;
        return true;
    }

    errorLog << "setDatasetName(const std::string datasetName) - The dataset name cannot contain any spaces!" << std::endl;
    return false;
}

bool ClassificationData::setInfoText(const std::string infoText){
    this->infoText = infoText;
    return true;
}

bool ClassificationData::setClassNameForCorrespondingClassLabel(const std::string className,const UINT classLabel){

    for(UINT i=0; i<classTracker.getSize(); i++){
        if( classTracker[i].classLabel == classLabel ){
            classTracker[i].className = className;
            return true;
        }
    }

	errorLog << "setClassNameForCorrespondingClassLabel(const std::string className,const UINT classLabel) - Failed to find class with label: " << classLabel << std::endl;
    return false;
}
    
bool ClassificationData::setAllowNullGestureClass(const bool allowNullGestureClass){
    this->allowNullGestureClass = allowNullGestureClass;
    return true;
}

bool ClassificationData::addSample(const UINT classLabel,const VectorFloat &sample){
    
	if( sample.getSize() != numDimensions ){
        if( totalNumSamples == 0 ){
            warningLog << "addSample(const UINT classLabel, VectorFloat &sample) - the size of the new sample (" << sample.getSize() << ") does not match the number of dimensions of the dataset (" << numDimensions << "), setting dimensionality to: " << numDimensions << std::endl;
            numDimensions = sample.getSize();
        }else{
            errorLog << "addSample(const UINT classLabel, VectorFloat &sample) - the size of the new sample (" << sample.getSize() << ") does not match the number of dimensions of the dataset (" << numDimensions << ")" << std::endl;
            return false;
        }
    }

    //The class label must be greater than zero (as zero is used for the null rejection class label
    if( classLabel == GRT_DEFAULT_NULL_CLASS_LABEL && !allowNullGestureClass ){
        errorLog << "addSample(const UINT classLabel, VectorFloat &sample) - the class label can not be 0!" << std::endl;
        return false;
    }

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

	ClassificationSample newSample(classLabel,sample);
	data.push_back( newSample );
	totalNumSamples++;

	if( classTracker.getSize() == 0 ){
		ClassTracker tracker(classLabel,1);
		classTracker.push_back(tracker);
	}else{
		bool labelFound = false;
		for(UINT i=0; i<classTracker.getSize(); i++){
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

    //Update the class labels
    sortClassLabels();

	return true;
}
    
bool ClassificationData::removeSample( const UINT index ){
    
    if( totalNumSamples == 0 ){
        warningLog << "removeSample( const UINT index ) - Failed to remove sample, the training dataset is empty!" << std::endl;
        return false;
    }
    
    if( index >= totalNumSamples ){
        warningLog << "removeSample( const UINT index ) - Failed to remove sample, the index is out of bounds! Number of training samples: " << totalNumSamples << " index: " << index << std::endl;
        return false;
    }
    
    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();
    
    //Find the corresponding class ID for the last training example
    UINT classLabel = data[ index ].getClassLabel();
    
    //Remove the training example from the buffer
    data.erase( data.begin()+index );
    
    totalNumSamples = data.getSize();
    
    //Remove the value from the counter
    for(size_t i=0; i<classTracker.getSize(); i++){
        if( classTracker[i].classLabel == classLabel ){
            classTracker[i].counter--;
            break;
        }
    }
    
    return true;
}

bool ClassificationData::removeLastSample(){
    
    if( totalNumSamples == 0 ){
        warningLog << "removeLastSample() - Failed to remove sample, the training dataset is empty!" << std::endl;
        return false;
    }

    return removeSample( totalNumSamples-1 );
}

bool ClassificationData::reserve(const UINT N){
    
    data.reserve( N );
    
    if( data.capacity() >= N ) return true;
    
    return false;
}
    
UINT ClassificationData::eraseAllSamplesWithClassLabel(const UINT classLabel){
    return removeClass( classLabel );
}
    
bool ClassificationData::addClass(const UINT classLabel,const std::string className){
    
    //Check to make sure the class label does not exist
    for(size_t i=0; i<classTracker.getSize(); i++){
        if( classTracker[i].classLabel == classLabel ){
            warningLog << "addClass(const UINT classLabel,const std::string className) - Failed to add class, it already exists! Class label: " << classLabel << std::endl;
            return false;
        }
    }
    
    //Add the class label to the class tracker
    classTracker.push_back( ClassTracker(classLabel,0,className) );
    
    //Sort the class labels
    sortClassLabels();
    
    return true;
}
    
UINT ClassificationData::removeClass(const UINT classLabel){
    
    UINT numExamplesRemoved = 0;
    UINT numExamplesToRemove = 0;
    
    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();
    
    //Find out how many training examples we need to remove
    for(UINT i=0; i<classTracker.getSize(); i++){
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
            }else if( ++i == data.getSize() ) break;
        }
    }
    
    totalNumSamples = data.getSize();
    
    return numExamplesRemoved;
}

bool ClassificationData::relabelAllSamplesWithClassLabel(const UINT oldClassLabel,const UINT newClassLabel){
    bool oldClassLabelFound = false;
    bool newClassLabelAllReadyExists = false;
    UINT indexOfOldClassLabel = 0;
    UINT indexOfNewClassLabel = 0;

    //Find out how many training examples we need to relabel
    for(UINT i=0; i<classTracker.getSize(); i++){
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
            data[i].setClassLabel(newClassLabel);
        }
    }

    //Update the class tracler
    if( newClassLabelAllReadyExists ){
        //Add the old sample count to the new sample count
        classTracker[ indexOfNewClassLabel ].counter += classTracker[ indexOfOldClassLabel ].counter;
    }else{
        //Create a new class tracker
        classTracker.push_back( ClassTracker(newClassLabel,classTracker[ indexOfOldClassLabel ].counter,classTracker[ indexOfOldClassLabel ].className) );
    }

    //Erase the old class tracker
    classTracker.erase( classTracker.begin() + indexOfOldClassLabel );

    //Sort the class labels
    sortClassLabels();

    return true;
}

bool ClassificationData::setExternalRanges(const Vector< MinMax > &externalRanges, const bool useExternalRanges){

    if( externalRanges.size() != numDimensions ) return false;

    this->externalRanges = externalRanges;
    this->useExternalRanges = useExternalRanges;

    return true;
}

bool ClassificationData::enableExternalRangeScaling(const bool useExternalRanges){
    if( externalRanges.getSize() == numDimensions ){
        this->useExternalRanges = useExternalRanges;
        return true;
    }
    return false;
}

bool ClassificationData::scale(const Float minTarget,const Float maxTarget){
    Vector< MinMax > ranges = getRanges();
    return scale(ranges,minTarget,maxTarget);
}

bool ClassificationData::scale(const Vector<MinMax> &ranges,const Float minTarget,const Float maxTarget){
    if( ranges.getSize() != numDimensions ) return false;

    //Scale the training data
    for(UINT i=0; i<totalNumSamples; i++){
        for(UINT j=0; j<numDimensions; j++){
            data[i][j] = grt_scale(data[i][j],ranges[j].minValue,ranges[j].maxValue,minTarget,maxTarget);
        }
    }

    return true;
}
    
bool ClassificationData::save(const std::string &filename) const{
    
    //Check if the file should be saved as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return saveDatasetToCSVFile( filename );
    }
    
    //Otherwise save it as a custom GRT file
    return saveDatasetToFile( filename );
}

bool ClassificationData::load(const std::string &filename){
    
    //Check if the file should be loaded as a csv file
    if( Util::stringEndsWith( filename, ".csv" )  ){
        return loadDatasetFromCSVFile( filename );
    }
    
    //Otherwise save it as a custom GRT file
    return loadDatasetFromFile( filename );
}

bool ClassificationData::saveDatasetToFile(const std::string &filename) const{

	std::fstream file;
	file.open(filename.c_str(), std::ios::out);

	if( !file.is_open() ){
		return false;
	}

	file << "GRT_LABELLED_CLASSIFICATION_DATA_FILE_V1.0\n";
    file << "DatasetName: " << datasetName << std::endl;
    file << "InfoText: " << infoText << std::endl;
	file << "NumDimensions: " << numDimensions << std::endl;
	file << "TotalNumExamples: " << totalNumSamples << std::endl;
	file << "NumberOfClasses: " << classTracker.size() << std::endl;
	file << "ClassIDsAndCounters: " << std::endl;

	for(UINT i=0; i<classTracker.size(); i++){
		file << classTracker[i].classLabel << "\t" << classTracker[i].counter << "\t" << classTracker[i].className << std::endl;
	}

    file << "UseExternalRanges: " << useExternalRanges << std::endl;

    if( useExternalRanges ){
        for(UINT i=0; i<externalRanges.size(); i++){
            file << externalRanges[i].minValue << "\t" << externalRanges[i].maxValue << std::endl;
        }
    }

	file << "Data:\n";

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

bool ClassificationData::loadDatasetFromFile(const std::string &filename){

	std::fstream file;
	file.open(filename.c_str(), std::ios::in);
	UINT numClasses = 0;
	clear();

	if( !file.is_open() ){
        errorLog << "loadDatasetFromFile(const std::string &filename) - could not open file!" << std::endl;
		return false;
	}

	std::string word;

	//Check to make sure this is a file with the Training File Format
	file >> word;
	if(word != "GRT_LABELLED_CLASSIFICATION_DATA_FILE_V1.0"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - could not find file header!" << std::endl;
		file.close();
		return false;
	}

    //Get the name of the dataset
	file >> word;
	if(word != "DatasetName:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find DatasetName header!" << std::endl;
        errorLog << word << std::endl;
		file.close();
		return false;
	}
	file >> datasetName;

    file >> word;
	if(word != "InfoText:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find InfoText header!" << std::endl;
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
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find NumDimensions header!" << std::endl;
		file.close();
		return false;
	}
	file >> numDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if( word != "TotalNumTrainingExamples:" && word != "TotalNumExamples:" ){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find TotalNumTrainingExamples header!" << std::endl;
		file.close();
		return false;
	}
	file >> totalNumSamples;

	//Get the total number of classes in the training data
	file >> word;
	if(word != "NumberOfClasses:"){
        errorLog << "loadDatasetFromFile(string filename) - failed to find NumberOfClasses header!" << std::endl;
		file.close();
		return false;
	}
	file >> numClasses;

	//Resize the class counter buffer and load the counters
	classTracker.resize(numClasses);

	//Get the total number of classes in the training data
	file >> word;
	if(word != "ClassIDsAndCounters:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find ClassIDsAndCounters header!" << std::endl;
		file.close();
		return false;
	}

	for(UINT i=0; i<classTracker.getSize(); i++){
		file >> classTracker[i].classLabel;
		file >> classTracker[i].counter;
        file >> classTracker[i].className;
	}

    //Check if the dataset should be scaled using external ranges
	file >> word;
	if(word != "UseExternalRanges:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find UseExternalRanges header!" << std::endl;
		file.close();
		return false;
	}
    file >> useExternalRanges;

    //If we are using external ranges then load them
    if( useExternalRanges ){
        externalRanges.resize(numDimensions);
        for(UINT i=0; i<externalRanges.getSize(); i++){
            file >> externalRanges[i].minValue;
            file >> externalRanges[i].maxValue;
        }
    }

	//Get the main training data
	file >> word;
	if( word != "LabelledTrainingData:" && word != "Data:"){
        errorLog << "loadDatasetFromFile(const std::string &filename) - failed to find LabelledTrainingData header!" << std::endl;
		file.close();
		return false;
	}

	ClassificationSample tempSample( numDimensions );
	data.resize( totalNumSamples, tempSample );

	for(UINT i=0; i<totalNumSamples; i++){
        UINT classLabel = 0;
        VectorFloat sample(numDimensions,0);
		file >> classLabel;
		for(UINT j=0; j<numDimensions; j++){
			file >> sample[j];
		}
        data[i].set(classLabel, sample);
	}

	file.close();
	
    //Sort the class labels
    sortClassLabels();
	
	return true;
}

bool ClassificationData::saveDatasetToCSVFile(const std::string &filename) const{

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
		file << std::endl;
	}

	file.close();

    return true;
}

bool ClassificationData::loadDatasetFromCSVFile(const std::string &filename,const UINT classLabelColumnIndex){

    numDimensions = 0;
    datasetName = "NOT_SET";
    infoText = "";

    //Clear any previous data
    clear();

    //Parse the CSV file
    FileParser parser;

    Timer timer;

    timer.start();
    
    if( !parser.parseCSVFile(filename,true) ){
        errorLog << "loadDatasetFromCSVFile(const std::string &filename,const UINT classLabelColumnIndex) - Failed to parse CSV file!" << std::endl;
        return false;
    }
    
    if( !parser.getConsistentColumnSize() ){
        errorLog << "loadDatasetFromCSVFile(const std::string &filename,const UINT classLabelColumnIndexe) - The CSV file does not have a consistent number of columns!" << std::endl;
        return false;
    }
    
    if( parser.getColumnSize() <= 1 ){
        errorLog << "loadDatasetFromCSVFile(const std::string &filename,const UINT classLabelColumnIndex) - The CSV file does not have enough columns! It should contain at least two columns!" << std::endl;
        return false;
    }
    
    //Set the number of dimensions
    numDimensions = parser.getColumnSize()-1;

    timer.start();

    //Reserve the memory for the data
    data.resize( parser.getRowSize(), ClassificationSample(numDimensions) );

    timer.start();
   
    //Loop over the samples and add them to the data set
    UINT classLabel = 0;
    UINT j = 0;
    UINT n = 0;
    totalNumSamples = parser.getRowSize();
    for(UINT i=0; i<totalNumSamples; i++){
        //Get the class label
        classLabel = grt_from_str< UINT >( parser[i][classLabelColumnIndex] );
        
        //Set the class label
        data[i].setClassLabel( classLabel );
        
        //Get the sample data
        j=0;
        n=0;
        while( j != numDimensions ){
            if( n != classLabelColumnIndex ){
                data[i][j++] = grt_from_str< Float >( parser[i][n] );
            }
            n++;
        }
        
        //Update the class tracker
        if( classTracker.size() == 0 ){
            ClassTracker tracker(classLabel,1);
            classTracker.push_back(tracker);
        }else{
            bool labelFound = false;
            const size_t numClasses = classTracker.size();
            for(size_t i=0; i<numClasses; i++){
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
    }

    //Sort the class labels
    sortClassLabels();
    
    return true;
}
    
bool ClassificationData::printStats() const{

    std::cout << getStatsAsString();

    return true;
}

bool ClassificationData::sortClassLabels(){
	
	sort(classTracker.begin(),classTracker.end(),ClassTracker::sortByClassLabelAscending);

	return true;
}

ClassificationData ClassificationData::partition(const UINT trainingSizePercentage,const bool useStratifiedSampling){
    return split(trainingSizePercentage, useStratifiedSampling);
}

ClassificationData ClassificationData::split(const UINT trainingSizePercentage,const bool useStratifiedSampling){

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

	//Create the random partion indexs
	Random random;
    UINT K = getNumClasses();

    //Make sure both datasets get all the class labels, even if they have no samples in each
    trainingSet.classTracker.resize( K );
    testSet.classTracker.resize( K );
    for(UINT k=0; k<K; k++){
        trainingSet.classTracker[k].classLabel = classTracker[k].classLabel;
        testSet.classTracker[k].classLabel = classTracker[k].classLabel;
        trainingSet.classTracker[k].counter = 0;
        testSet.classTracker[k].counter = 0;
    }

    if( useStratifiedSampling ){
        //Break the data into seperate classes
        Vector< Vector< UINT > > classData( K );

        //Add the indexs to their respective classes
        for(UINT i=0; i<totalNumSamples; i++){
            classData[ getClassLabelIndexValue( data[i].getClassLabel() ) ].push_back( i );
        }

        //Randomize the order of the indexs in each of the class index buffers
        for(UINT k=0; k<K; k++){
            std::random_shuffle(classData[k].begin(), classData[k].end());
        }
        
        //Reserve the memory
        UINT numTrainingSamples = 0;
        UINT numTestSamples = 0;
        
        for(UINT k=0; k<K; k++){
            UINT numTrainingExamples = (UINT) floor( Float(classData[k].size()) / 100.0 * Float(trainingSizePercentage) );
            UINT numTestExamples = ((UINT)classData[k].size())-numTrainingExamples;
            numTrainingSamples += numTrainingExamples;
            numTestSamples += numTestExamples;
        }
        
        trainingSet.reserve( numTrainingSamples );
        testSet.reserve( numTestSamples );

        //Loop over each class and add the data to the trainingSet and testSet
        for(UINT k=0; k<K; k++){
            UINT numTrainingExamples = (UINT) floor( Float(classData[k].getSize()) / 100.0 * Float(trainingSizePercentage) );

            //Add the data to the training and test sets
            for(UINT i=0; i<numTrainingExamples; i++){
                trainingSet.addSample( data[ classData[k][i] ].getClassLabel(), data[ classData[k][i] ].getSample() );
            }
            for(UINT i=numTrainingExamples; i<classData[k].getSize(); i++){
                testSet.addSample( data[ classData[k][i] ].getClassLabel(), data[ classData[k][i] ].getSample() );
            }
        }
    }else{

        const UINT numTrainingExamples = (UINT) floor( Float(totalNumSamples) / 100.0 * Float(trainingSizePercentage) );

        //Create the random partion indexs
        Vector< UINT > indexs( totalNumSamples );
        for(UINT i=0; i<totalNumSamples; i++) indexs[i] = i;
        std::random_shuffle(indexs.begin(), indexs.end());
        
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

    //The training and test datasets MUST have the same number of classes as the original data
    grt_assert( trainingSet.getNumClasses() == K );
    grt_assert( testSet.getNumClasses() == K );

    //Overwrite the training data in this instance with the training data of the trainingSet
    *this = trainingSet;

    //Sort the class labels in this dataset
    sortClassLabels();

    //Sort the class labels of the test dataset
    testSet.sortClassLabels();

	return testSet;
}

bool ClassificationData::merge(const ClassificationData &otherData){

    if( otherData.getNumDimensions() != numDimensions ){
        errorLog << "merge(const ClassificationData &labelledData) - The number of dimensions in the labelledData (" << otherData.getNumDimensions() << ") does not match the number of dimensions of this dataset (" << numDimensions << ")" << std::endl;
        return false;
    }

    //The dataset has changed so flag that any previous cross validation setup will now not work
    crossValidationSetup = false;
    crossValidationIndexs.clear();

    const UINT M = otherData.getNumSamples();
    
    //Reserve the memory
    reserve( getNumSamples() + M );

    //Add the data from the labelledData to this instance
    
    for(UINT i=0; i<M; i++){
        addSample(otherData[i].getClassLabel(), otherData[i].getSample());
    }

    //Set the class names from the dataset
    Vector< ClassTracker > classTracker = otherData.getClassTracker();
    for(UINT i=0; i<classTracker.getSize(); i++){
        setClassNameForCorrespondingClassLabel(classTracker[i].className, classTracker[i].classLabel);
    }

    //Sort the class labels
    sortClassLabels();

    return true;
}

bool ClassificationData::spiltDataIntoKFolds(const UINT K,const bool useStratifiedSampling){

    crossValidationSetup = false;
    crossValidationIndexs.clear();

    //K can not be zero
    if( K == 0 ){
        errorLog << "spiltDataIntoKFolds(const UINT K,const bool useStratifiedSampling) - K can not be zero!" << std::endl;
        return false;
    }

    //K can not be larger than the number of examples
    if( K > totalNumSamples ){
        errorLog << "spiltDataIntoKFolds(const UINT K,const bool useStratifiedSampling) - K can not be larger than the total number of samples in the dataset!" << std::endl;
        return false;
    }

    //K can not be larger than the number of examples in a specific class if the stratified sampling option is true
    if( useStratifiedSampling ){
        for(UINT c=0; c<classTracker.getSize(); c++){
            if( K > classTracker[c].counter ){
                errorLog << "spiltDataIntoKFolds(const UINT K,const bool useStratifiedSampling) - K can not be larger than the number of samples in any given class!" << std::endl;
                return false;
            }
        }
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
                //Pick a random indexs
                randomIndex = random.getRandomNumberInt(0,numSamples);

                //Swap the indexs
                SWAP(classData[c][ x ] , classData[c][ randomIndex ]);
            }
        }

        //Loop over each of the k folds, at each fold add a sample from each class
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
        errorLog << "getTrainingFoldData(const UINT foldIndex) - Cross Validation has not been setup! You need to call the spiltDataIntoKFolds(UINT K,bool useStratifiedSampling) function first before calling this function!" << std::endl;
       return trainingData;
    }

    if( foldIndex >= kFoldValue ) return trainingData;

    //Add the class labels to make sure they all exist
    for(UINT k=0; k<getNumClasses(); k++){
        trainingData.addClass( classTracker[k].classLabel, classTracker[k].className );
    }

    //Add the data to the training set, this will consist of all the data that is NOT in the foldIndex
    UINT index = 0;
    for(UINT k=0; k<kFoldValue; k++){
        if( k != foldIndex ){
            for(UINT i=0; i<crossValidationIndexs[k].getSize(); i++){

                index = crossValidationIndexs[k][i];
                trainingData.addSample( data[ index ].getClassLabel(), data[ index ].getSample() );
            }
        }
    }

    //Sort the class labels
    trainingData.sortClassLabels();

    return trainingData;
}

ClassificationData ClassificationData::getTestFoldData(const UINT foldIndex) const{
    
    ClassificationData testData;
    testData.setNumDimensions( numDimensions );
    testData.setAllowNullGestureClass( allowNullGestureClass );

    if( !crossValidationSetup ) return testData;

    if( foldIndex >= kFoldValue ) return testData;

    //Add the class labels to make sure they all exist
    for(UINT k=0; k<getNumClasses(); k++){
        testData.addClass( classTracker[k].classLabel, classTracker[k].className );
    }
    
    testData.reserve( crossValidationIndexs[ foldIndex ].getSize() );

    //Add the data to the test fold
    UINT index = 0;
	for(UINT i=0; i<crossValidationIndexs[ foldIndex ].getSize(); i++){

        index = crossValidationIndexs[ foldIndex ][i];
		testData.addSample( data[ index ].getClassLabel(), data[ index ].getSample() );
	}
	
    //Sort the class labels
	testData.sortClassLabels();

    return testData;
}

ClassificationData ClassificationData::getClassData(const UINT classLabel) const{
    
    ClassificationData classData;
    classData.setNumDimensions( this->numDimensions );
    classData.setAllowNullGestureClass( allowNullGestureClass );
    
    //Reserve the memory for the class data
    for(UINT i=0; i<classTracker.getSize(); i++){
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
    
ClassificationData ClassificationData::getBootstrappedDataset(const UINT numSamples_,const bool balanceDataset) const{
    
    Random rand;
    ClassificationData newDataset;
    newDataset.setNumDimensions( getNumDimensions() );
    newDataset.setAllowNullGestureClass( allowNullGestureClass );
    newDataset.setExternalRanges( externalRanges, useExternalRanges );
    
    const UINT numBootstrapSamples = numSamples_ > 0 ? numSamples_ : totalNumSamples;

    grt_assert( numBootstrapSamples > 0 );
    
    newDataset.reserve( numBootstrapSamples );

    const UINT K = getNumClasses(); 
    
    //Add all the class labels to the new dataset to ensure the dataset has a list of all the labels
    for(UINT k=0; k<K; k++){
        newDataset.addClass( classTracker[k].classLabel );
    }

    if( balanceDataset ){
        //Group the class indexs
        Vector< Vector< UINT > > classIndexs( K );
        for(UINT i=0; i<totalNumSamples; i++){
            classIndexs[ getClassLabelIndexValue( data[i].getClassLabel() ) ].push_back( i );
        }

        //Get the class with the minimum number of examples
        UINT numSamplesPerClass = (UINT)floor( numBootstrapSamples / Float(K) );

        //Randomly select the training samples from each class
        UINT classIndex = 0;
        UINT classCounter = 0;
        UINT randomIndex = 0;
        for(UINT i=0; i<numBootstrapSamples; i++){
            randomIndex = rand.getRandomNumberInt(0, (UINT)classIndexs[ classIndex ].size() );
            randomIndex = classIndexs[ classIndex ][ randomIndex ];
            newDataset.addSample(data[ randomIndex ].getClassLabel(), data[ randomIndex ].getSample());
            if( classCounter++ >= numSamplesPerClass && classIndex+1 < K ){
                classCounter = 0;
                classIndex++;
            }
        }

    }else{
        //Randomly select the training samples to add to the new data set
        UINT randomIndex;
        for(UINT i=0; i<numBootstrapSamples; i++){
            randomIndex = rand.getRandomNumberInt(0, totalNumSamples);
            newDataset.addSample( data[randomIndex].getClassLabel(), data[randomIndex].getSample() );
        }
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
        VectorFloat targetVector(numTargetDimensions,0);

        //Set the class index in the target Vector to 1 and all other values in the target Vector to 0
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
    UINT minClassLabel = grt_numeric_limits< UINT >::max();

    for(UINT i=0; i<classTracker.getSize(); i++){
        if( classTracker[i].classLabel < minClassLabel ){
            minClassLabel = classTracker[i].classLabel;
        }
    }

    return minClassLabel;
}


UINT ClassificationData::getMaximumClassLabel() const{
    UINT maxClassLabel = 0;

    for(UINT i=0; i<classTracker.getSize(); i++){
        if( classTracker[i].classLabel > maxClassLabel ){
            maxClassLabel = classTracker[i].classLabel;
        }
    }

    return maxClassLabel;
}

UINT ClassificationData::getClassLabelIndexValue(const UINT classLabel) const{
    for(UINT k=0; k<classTracker.getSize(); k++){
        if( classTracker[k].classLabel == classLabel ){
            return k;
        }
    }
    warningLog << "getClassLabelIndexValue(UINT classLabel) - Failed to find class label: " << classLabel << " in class tracker!" << std::endl;
    return 0;
}

std::string ClassificationData::getClassNameForCorrespondingClassLabel(const UINT classLabel) const{

    for(UINT i=0; i<classTracker.getSize(); i++){
        if( classTracker[i].classLabel == classLabel ){
            return classTracker[i].className;
        }
    }

    return "CLASS_LABEL_NOT_FOUND";
}

std::string ClassificationData::getStatsAsString() const{
    std::string statsText;
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

    Vector< MinMax > ranges = getRanges();

    statsText += "Dataset Ranges:\n";
    for(UINT j=0; j<ranges.size(); j++){
        statsText += "[" + Util::toString( j+1 ) + "] Min:\t" + Util::toString( ranges[j].minValue ) + "\tMax: " + Util::toString( ranges[j].maxValue ) + "\n";
    }

    return statsText;
}

Vector<MinMax> ClassificationData::getRanges() const{

    //If the dataset should be scaled using the external ranges then return the external ranges
    if( useExternalRanges ) return externalRanges;

	Vector< MinMax > ranges(numDimensions);

    //Otherwise return the min and max values for each column in the dataset
    if( totalNumSamples > 0 ){
        for(UINT j=0; j<numDimensions; j++){
            ranges[j].minValue = data[0][j];
            ranges[j].maxValue = data[0][j];
            for(UINT i=0; i<totalNumSamples; i++){
                if( data[i][j] < ranges[j].minValue ){ ranges[j].minValue = data[i][j]; }		//Search for the min value
                else if( data[i][j] > ranges[j].maxValue ){ ranges[j].maxValue = data[i][j]; }	//Search for the max value
            }
        }
    }
    return ranges;
}

Vector< UINT > ClassificationData::getClassLabels() const{
    Vector< UINT > classLabels( getNumClasses(), 0 );

    if( getNumClasses() == 0 ) return classLabels;

    for(UINT i=0; i<getNumClasses(); i++){
        classLabels[i] = classTracker[i].classLabel;
    }

    return classLabels;
}

Vector< UINT > ClassificationData::getNumSamplesPerClass() const{
    Vector< UINT > classSampleCounts( getNumClasses(), 0 );

    if( getNumSamples() == 0 ) return classSampleCounts;

    for(UINT i=0; i<getNumClasses(); i++){
        classSampleCounts[i] = classTracker[i].counter;
    }

    return classSampleCounts;
}

VectorFloat ClassificationData::getMean() const{
	
	VectorFloat mean(numDimensions,0);
	
	for(UINT j=0; j<numDimensions; j++){
		for(UINT i=0; i<totalNumSamples; i++){
			mean[j] += data[i][j];
		}
		mean[j] /= Float(totalNumSamples);
	}
	
	return mean;
}

VectorFloat ClassificationData::getStdDev() const{
	
	VectorFloat mean = getMean();
	VectorFloat stdDev(numDimensions,0);
	
	for(UINT j=0; j<numDimensions; j++){
		for(UINT i=0; i<totalNumSamples; i++){
			stdDev[j] += SQR(data[i][j]-mean[j]);
		}
		stdDev[j] = sqrt( stdDev[j] / Float(totalNumSamples-1) );
	}
	
	return stdDev;
}

MatrixFloat ClassificationData::getClassHistogramData(const UINT classLabel,const UINT numBins) const{

    const UINT M = getNumSamples();
    const UINT N = getNumDimensions();

    Vector< MinMax > ranges = getRanges();
    VectorFloat binRange(N);
    for(UINT i=0; i<ranges.size(); i++){
        binRange[i] = (ranges[i].maxValue-ranges[i].minValue)/Float(numBins);
    }

    MatrixFloat histData(N,numBins);
    histData.setAllValues(0);

    Float norm = 0;
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

MatrixFloat ClassificationData::getClassMean() const{
	
	MatrixFloat mean(getNumClasses(),numDimensions);
	VectorFloat counter(getNumClasses(),0);
	
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
			mean[k][j] = counter[k] > 0 ? mean[k][j]/counter[k] : 0;
		}
	}
	
	return mean;
}

MatrixFloat ClassificationData::getClassStdDev() const{

	MatrixFloat mean = getClassMean();
	MatrixFloat stdDev(getNumClasses(),numDimensions);
	VectorFloat counter(getNumClasses(),0);
	
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
			stdDev[k][j] = sqrt( stdDev[k][j] / Float(counter[k]-1) );
		}
	}
	
	return stdDev;
}

MatrixFloat ClassificationData::getCovarianceMatrix() const{
	
	VectorFloat mean = getMean();
	MatrixFloat covariance(numDimensions,numDimensions);
	
	for(UINT j=0; j<numDimensions; j++){
		for(UINT k=0; k<numDimensions; k++){
			for(UINT i=0; i<totalNumSamples; i++){
				covariance[j][k] += (data[i][j]-mean[j]) * (data[i][k]-mean[k]) ;
			}
			covariance[j][k] /= Float(totalNumSamples-1);
		}
	}
	
	return covariance;
}

Vector< MatrixFloat > ClassificationData::getHistogramData(const UINT numBins) const{
    const UINT K = getNumClasses();
    Vector< MatrixFloat > histData(K);

    for(UINT k=0; k<K; k++){
        histData[k] = getClassHistogramData( classTracker[k].classLabel, numBins );
    }

    return histData;
}
   
VectorFloat ClassificationData::getClassProbabilities() const {
    return getClassProbabilities( getClassLabels() );
}
    
VectorFloat ClassificationData::getClassProbabilities( const Vector< UINT > &classLabels ) const {
    const UINT K = (UINT)classLabels.size();
    const UINT N = getNumClasses();
    Float sum = 0;
    VectorFloat x(K,0);
    for(UINT k=0; k<K; k++){
        for(UINT n=0; n<N; n++){
            if( classLabels[k] == classTracker[n].classLabel ){
                x[k] = classTracker[n].counter;
                sum += classTracker[n].counter;
                break;
            }
        }
    }
    
    //Normalize the class probabilities
    if( sum > 0 ){
        for(UINT k=0; k<K; k++){
            x[k] /= sum;
        }
    }
    
    return x;
}

Vector< UINT > ClassificationData::getClassDataIndexes(const UINT classLabel) const{

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
    Vector< UINT > classIndexes(N);
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

MatrixFloat ClassificationData::getDataAsMatrixFloat() const {
    const UINT M = getNumSamples();
    const UINT N = getNumDimensions();
    MatrixFloat d(M,N);

    for(UINT i=0; i<M; i++){
        for(UINT j=0; j<N; j++){
            d[i][j] = data[i][j];
        }
    }

    return d;
}

bool ClassificationData::generateGaussDataset( const std::string filename, const UINT numSamples, const UINT numClasses, const UINT numDimensions, const Float range, const Float sigma ){
    
    //Generate the dataset
    ClassificationData data = generateGaussDataset( numSamples, numClasses, numDimensions, range, sigma );
    
    //Save the dataset to a CSV file
    return data.save( filename );
}

ClassificationData ClassificationData::generateGaussDataset( const UINT numSamples, const UINT numClasses, const UINT numDimensions, const Float range, const Float sigma ){
    
    Random random;
    
    //Generate a simple model that will be used to generate the main dataset
    MatrixFloat model(numClasses,numDimensions);
    for(UINT k=0; k<numClasses; k++){
        for(UINT j=0; j<numDimensions; j++){
            model[k][j] = random.getRandomNumberUniform(-range,range);
        }
    }
    
    //Use the model above to generate the main dataset
    ClassificationData data;
    data.setNumDimensions( numDimensions );
    data.reserve( numSamples );
    
    for(UINT i=0; i<numSamples; i++){
        
        //Randomly select which class this sample belongs to
        UINT k = random.getRandomNumberInt( 0, numClasses );
        
        //Generate a sample using the model (+ some Gaussian noise)
        VectorFloat sample( numDimensions );
        for(UINT j=0; j<numDimensions; j++){
            sample[j] = model[k][j] + random.getRandomNumberGauss(0,sigma);
        }
        
        //By default in the GRT, the class label should not be 0, so add 1
        UINT classLabel = k + 1;
        
        //Add the labeled sample to the dataset
        data.addSample( classLabel, sample );
    }
    
    //Return the datset
    return data;
}

ClassificationData ClassificationData::generateGaussLinearDataset( const UINT numSamples, const UINT numClasses, const UINT numDimensions, const Float range, const Float sigma){

    Random random;
    
    //Generate a simple model that will be used to generate the main dataset
    //Enforce the gaussian clusters to be linearly separable by setting each model centroid on a regular spaced grid
    MatrixFloat model(numClasses,numDimensions);
    for(UINT k=0; k<numClasses; k++){
        for(UINT j=0; j<numDimensions; j++){
            model[k][j] = Util::scale(k,0,numClasses-1,-range,range,true);
        }
    }
    
    //Use the model above to generate the main dataset
    ClassificationData data;
    data.setNumDimensions( numDimensions );
    data.reserve( numSamples );
    
    for(UINT i=0; i<numSamples; i++){
        
        //Randomly select which class this sample belongs to
        UINT k = random.getRandomNumberInt( 0, numClasses );
        
        //Generate a sample using the model (+ some Gaussian noise)
        VectorFloat sample( numDimensions );
        for(UINT j=0; j<numDimensions; j++){
            sample[j] = model[k][j] + random.getRandomNumberGauss(0,sigma);
        }
        
        //By default in the GRT, the class label should not be 0, so add 1
        UINT classLabel = k + 1;
        
        //Add the labeled sample to the dataset
        data.addSample( classLabel, sample );
    }

    return data;
}

GRT_END_NAMESPACE

