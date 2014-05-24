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

#include "MultidimensionalRegression.h"

namespace GRT{

//Register the MultidimensionalRegression module with the Classifier base class
RegisterRegressifierModule< MultidimensionalRegression >  MultidimensionalRegression::registerModule("MultidimensionalRegression");

MultidimensionalRegression::MultidimensionalRegression(const Regressifier &regressifier,bool useScaling):regressifier(NULL)
{
    this->useScaling = useScaling;
    classType = "MultidimensionalRegression";
    regressifierType = classType;
    debugLog.setProceedingText("[DEBUG MultidimensionalRegression]");
    errorLog.setProceedingText("[ERROR MultidimensionalRegression]");
    trainingLog.setProceedingText("[TRAINING MultidimensionalRegression]");
    warningLog.setProceedingText("[WARNING MultidimensionalRegression]");
    
    setRegressionModule( regressifier );
}

MultidimensionalRegression::~MultidimensionalRegression(void)
{
	deleteAll();
}
    
MultidimensionalRegression& MultidimensionalRegression::operator=(const MultidimensionalRegression &rhs){
    
	if( this != &rhs ){
        
        //Clean up any previous setup
        deleteAll();
        
        //Set the main regression module
        setRegressionModule( *rhs.getRegressifier() );
        
        //Deep copy the regression modules from the RHS to this instance
        if( !rhs.deepCopyRegressionModules( regressionModules ) ){
            errorLog << "const MultidimensionalRegression &rhs - Failed to deep copy regression modules!" << endl;
        }
        
        //Copy the base variables
        copyBaseVariables( (Regressifier*)&rhs );
	}
	return *this;
}

bool MultidimensionalRegression::deepCopyFrom(const Regressifier *regressifier){
    
    if( regressifier == NULL ) return false;
    
    if( this->getRegressifierType() == regressifier->getRegressifierType() ){
        
        const MultidimensionalRegression *ptr = dynamic_cast<const MultidimensionalRegression*>(regressifier);
        
        //Set the main regression module
        setRegressionModule( *ptr->getRegressifier() );
        
        //Deep copy the regression modules from the RHS to this instance
        if( !ptr->deepCopyRegressionModules( regressionModules ) ){
            errorLog << "deepCopyFrom(const Regressifier *regressifier) - Failed to deep copy regression modules!" << endl;
            return false;
        }
        
        //Copy the base variables
        return copyBaseVariables( regressifier );
    }
    
    return false;
}

bool MultidimensionalRegression::train_(RegressionData &trainingData){
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumInputDimensions();
    const unsigned int K = trainingData.getNumTargetDimensions();
    trained = false;
    trainingResults.clear();
    deleteRegressionModules();
    
    if( !getIsRegressionModuleSet() ){
        errorLog << "train_(RegressionData &trainingData) - The regression module has not been set!" << endl;
        return false;
    }
    
    if( M == 0 ){
        errorLog << "train_(RegressionData &trainingData) - Training data has zero samples!" << endl;
        return false;
    }
    
    numInputDimensions = N;
    numOutputDimensions = K;
    inputVectorRanges.clear();
    targetVectorRanges.clear();
    
    //Scale the training and validation data, if needed
	if( useScaling ){
		//Find the ranges for the input data
        inputVectorRanges = trainingData.getInputRanges();
        
        //Find the ranges for the target data
		targetVectorRanges = trainingData.getTargetRanges();
        
		//Scale the training data
		trainingData.scale(inputVectorRanges,targetVectorRanges,0.0,1.0);
	}
    
    //Setup the regression modules
    regressionModules.resize( K, NULL );
    
    //Any scaling will happpen at the meta level, not the regression module letter, so ensure scaling is turned off for the modules
    regressifier->enableScaling( false );
    
    for(UINT k=0; k<K; k++){
        regressionModules[k] = regressifier->deepCopy();
        if( regressionModules[k] == NULL ){
            errorLog << "train(LabelledRegressionData &trainingData) - Failed to deep copy module " << k << endl;
            return false;
        }
    }
    
    //Train each regression module
    for(UINT k=0; k<K; k++){
        
        trainingLog << "Training regression module: " << k << endl;
        
        //We need to create a 1 dimensional training dataset for the k'th target dimension
        RegressionData data;
        data.setInputAndTargetDimensions(N, 1);
        
        for(UINT i=0; i<M; i++){
            if( !data.addSample(trainingData[i].getInputVector(), VectorDouble(1,trainingData[i].getTargetVector()[k]) ) ){
                errorLog << "train_(RegressionData &trainingData) - Failed to add sample to dataset for regression module " << k << endl;
                return false;
            }
        }
        
        if( !regressionModules[k]->train( data ) ){
            errorLog << "train_(RegressionData &trainingData) - Failed to train regression module " << k << endl;
            return false;
        }
    }
    
    //Flag that the algorithm has been trained
    regressionData.resize(K,0);
    trained = true;
    return trained;
}

bool MultidimensionalRegression::predict_(VectorDouble &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - Model Not Trained!" << endl;
        return false;
    }
    
    if( !trained ) return false;
    
	if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorDouble &inputVector) - The size of the input vector (" << int( inputVector.size() ) << ") does not match the num features in the model (" << numInputDimensions << endl;
		return false;
	}
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], inputVectorRanges[n].minValue, inputVectorRanges[n].maxValue, 0, 1);
        }
    }
    
    for(UINT n=0; n<numOutputDimensions; n++){
        if( !regressionModules[ n ]->predict( inputVector ) ){
            errorLog << "predict_(VectorDouble &inputVector) - Failed to predict for regression module " << n << endl;
        }
        regressionData[ n ] = regressionModules[ n ]->getRegressionData()[0];
    }
    
    if( useScaling ){
        for(UINT n=0; n<numOutputDimensions; n++){
            regressionData[n] = scale(regressionData[n], 0, 1, targetVectorRanges[n].minValue, targetVectorRanges[n].maxValue);
        }
    }
    
    return true;
}
    
bool MultidimensionalRegression::saveModelToFile(string filename) const {

    if( !trained ) return false;
    
	std::fstream file; 
	file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        return false;
    }

	file.close();

	return true;
}
    
bool MultidimensionalRegression::saveModelToFile(fstream &file) const {
    
    if(!file.is_open())
	{
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
		return false;
	}
    
	//Write the header info
    file << "GRT_MULTIDIMENSIONAL_REGRESSION_MODEL_FILE_V2.0\n";
    
    //Write the regressifier settings to the file
    if( !Regressifier::saveBaseSettingsToFile(file) ){
        errorLog <<"saveModelToFile(fstream &file) - Failed to save Regressifier base settings to file!" << endl;
		return false;
    }
    
    if( !getIsRegressionModuleSet() ){
        file << "Regressifier: " << "NOT_SET" << endl;
        return true;
    }
    
    //Save the regression 
    file << "Regressifier: " << regressifier->getRegressifierType() << endl;
    
    if( !regressifier->saveModelToFile( file ) ){
        errorLog << "saveModelToFile(fstream &file) - Failed to save regressifier!" << endl;
		return false;
    }
    
    for(UINT i=0; i<regressionModules.size(); i++){
        if( !regressionModules[i]->saveModelToFile( file ) ){
            errorLog << "saveModelToFile(fstream &file) - Failed to save regression module " << i << endl;
            return false;
        }
    }
     
    return true;
}

bool MultidimensionalRegression::loadModelFromFile(string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        return false;
    }
    
    file.close();

	return true;

}
    
bool MultidimensionalRegression::loadModelFromFile(fstream &file){
    
    trained = false;
    numInputDimensions = 0;
    deleteAll();
    
    if(!file.is_open())
    {
        errorLog << "loadModelFromFile(string filename) - Could not open file to load model" << endl;
        return false;
    }
    
    std::string word;
    
    //Find the file type header
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_MULTIDIMENSIONAL_REGRESSION_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    if( word != "GRT_MULTIDIMENSIONAL_REGRESSION_MODEL_FILE_V2.0" ){
        errorLog << "loadModelFromFile( fstream &file ) - Could not find Model File Header" << endl;
        return false;
    }
    
    //Load the regressifier settings from the file
    if( !Regressifier::loadBaseSettingsFromFile(file) ){
        errorLog <<"loadModelFromFile( fstream &file ) - Failed to save Regressifier base settings to file!" << endl;
		return false;
    }
    
    file >> word;
    if( word != "Regressifier:" ){
        errorLog << "loadModelFromFile(string filename) - Failed to find Regressifier!" << endl;
        return false;
    }
    
    //Load the regressifier type
    string regressifierType;
    file >> regressifierType;
    if( regressifierType == "NOT_SET" ){
        return true;
    }
    
    //Create the regressifer
    regressifier = createInstanceFromString( regressifierType );
    
    if( regressifier == NULL ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to create regression instance from string!" << endl;
        return false;
    }
    
    if( !regressifier->loadModelFromFile( file ) ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to load regressifier!" << endl;
        return false;
    }
    
    if( numOutputDimensions > 0 ){
        //Initialize the regression modules
        regressionModules.resize(numOutputDimensions, NULL);
        
        for(UINT i=0; i<regressionModules.size(); i++){
            regressionModules[i] = createInstanceFromString( regressifierType );
            if( !regressionModules[i]->loadModelFromFile( file ) ){
                errorLog << "loadModelFromFile(fstream &file) - Failed to load regression module " << i << endl;
                return false;
            }
        }
    }
    
    return true;
}
    
bool MultidimensionalRegression::getIsRegressionModuleSet() const {
    return regressifier != NULL ? true : false;
}
    
Regressifier* MultidimensionalRegression::getRegressifier() const {
    return regressifier;
}
    
bool MultidimensionalRegression::setRegressionModule( const Regressifier &regressifier ){
    
    if( !deleteRegressionModules() ){
        return false;
    }
    
    trained = false;
    
    if( this->regressifier != NULL ) delete this->regressifier;
    
    this->regressifier = regressifier.deepCopy();
    
    if( this->regressifier == NULL ) return false;
    
    return true;
}

bool MultidimensionalRegression::deepCopyRegressionModules( vector< Regressifier* > &newModules ) const{
	
	const UINT N = (UINT)regressionModules.size();
	
	//The newModules vector should be empty
	if( newModules.size() > 0 ) return false;
	
	//Return true if there are no modules to copy
	if( N == 0 ) return true;
	
	//Perform the copy
	newModules.resize( N );
	
	for(UINT i=0; i<N; i++){
        //Deep copy the i'th module into the i'th regressionModules
        newModules[i] = regressionModules[i]->deepCopy();
		if( newModules[i] == NULL ){
			for(UINT j=0; j<i; j++){
				delete newModules[j];
				newModules[j] = NULL;
			}
			newModules.clear();
			return false;
		}
	}
	
	return true;
}

bool MultidimensionalRegression::deleteAll(){
    if( regressifier != NULL ){
        delete regressifier;
        regressifier = NULL;
    }
    return deleteRegressionModules();
}
    
bool MultidimensionalRegression::deleteRegressionModules(){
	
	const UINT N = (UINT)regressionModules.size();
	
	if( N == 0 ) return true;
	
	for(UINT i=0; i<N; i++){
		delete regressionModules[i];
		regressionModules[i] = NULL;
	}
	regressionModules.clear();
	return true;
}
    
bool MultidimensionalRegression::loadLegacyModelFromFile( fstream &file ){
    
    string word;
    
    file >> word;
    if(word != "NumFeatures:"){
        errorLog << "loadModelFromFile(string filename) - Could not find NumFeatures!" << endl;
        return false;
    }
    file >> numInputDimensions;
    
    file >> word;
    if(word != "NumOutputDimensions:"){
        errorLog << "loadModelFromFile(string filename) - Could not find NumOutputDimensions!" << endl;
        return false;
    }
    file >> numOutputDimensions;
    
    file >> word;
    if(word != "UseScaling:"){
        errorLog << "loadModelFromFile(string filename) - Could not find UseScaling!" << endl;
        return false;
    }
    file >> useScaling;
    
    ///Read the ranges if needed
    if( useScaling ){
        //Resize the ranges buffer
        inputVectorRanges.resize(numInputDimensions);
        targetVectorRanges.resize(numOutputDimensions);
        
        //Load the ranges
        file >> word;
        if(word != "InputVectorRanges:"){
            file.close();
            errorLog << "loadModelFromFile(string filename) - Failed to find InputVectorRanges!" << endl;
            return false;
        }
        for(UINT j=0; j<inputVectorRanges.size(); j++){
            file >> inputVectorRanges[j].minValue;
            file >> inputVectorRanges[j].maxValue;
        }
        
        file >> word;
        if(word != "OutputVectorRanges:"){
            file.close();
            errorLog << "loadModelFromFile(string filename) - Failed to find OutputVectorRanges!" << endl;
            return false;
        }
        for(UINT j=0; j<targetVectorRanges.size(); j++){
            file >> targetVectorRanges[j].minValue;
            file >> targetVectorRanges[j].maxValue;
        }
    }
    
    file >> word;
    if( word != "Regressifier:" ){
        errorLog << "loadModelFromFile(string filename) - Failed to find Regressifier!" << endl;
        return false;
    }
    
    //Load the regressifier type
    string regressifierType;
    file >> regressifierType;
    if( regressifierType == "NOT_SET" ){
        return true;
    }
    
    //Create the regressifer
    regressifier = createInstanceFromString( regressifierType );
    
    if( regressifier == NULL ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to create regression instance from string!" << endl;
        return false;
    }
    
    if( !regressifier->loadModelFromFile( file ) ){
        errorLog <<"loadModelFromFile(fstream &file) - Failed to load regressifier!" << endl;
        return false;
    }
    
    if( numOutputDimensions > 0 ){
        //Initialize the regression modules
        regressionModules.resize(numOutputDimensions, NULL);
        
        for(UINT i=0; i<regressionModules.size(); i++){
            regressionModules[i] = createInstanceFromString( regressifierType );
            if( !regressionModules[i]->loadModelFromFile( file ) ){
                errorLog << "loadModelFromFile(fstream &file) - Failed to load regression module " << i << endl;
                return false;
            }
        }
        
        //Resize the regression data vector
        regressionData.resize(numOutputDimensions,0);
        
        //Flag that the model has been trained
        trained = true;
    }

    return true;
}

} //End of namespace GRT

