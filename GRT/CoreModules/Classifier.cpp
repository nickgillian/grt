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
#include "Classifier.h"

GRT_BEGIN_NAMESPACE
    
Classifier::StringClassifierMap* Classifier::stringClassifierMap = NULL;
UINT Classifier::numClassifierInstances = 0;

Classifier* Classifier::createNewInstance() const { return create(); } ///<Legacy function
Classifier* Classifier::createInstanceFromString(const std::string &id) { return create(id); } ///<Legacy function
    
Classifier* Classifier::create(const std::string &id){

    //This function maps the input string and returns a pointer to a new instance 
    
    StringClassifierMap::iterator iter = getMap()->find( id );
    if( iter == getMap()->end() ){
        //If the iterator points to the end of the map, then no match was found so return NULL
        return NULL;
    }

    return iter->second();
}

Classifier* Classifier::create() const{
    return create( MLBase::getId() );
}
    
Classifier* Classifier::deepCopy() const{
    
    Classifier *newInstance = create( MLBase::getId() );
    
    if( newInstance == NULL ) return NULL;
    
    if( !newInstance->deepCopyFrom( this ) ){
        delete newInstance;
        return NULL;
    }
    return newInstance;
}
    
const Classifier* Classifier::getClassifierPointer() const{
    return this;
}

Vector< std::string > Classifier::getRegisteredClassifiers(){
	Vector< std::string > registeredClassifiers;
	
	StringClassifierMap::iterator iter = getMap()->begin();
	while( iter != getMap()->end() ){
		registeredClassifiers.push_back( iter->first );
		++iter; //++iter is faster than iter++ as it does not require a copy/move operator
	}
	return registeredClassifiers;
}
    
Classifier::Classifier( const std::string &id ) : MLBase( id, MLBase::CLASSIFIER )
{
    classifierMode = STANDARD_CLASSIFIER_MODE;
    supportsNullRejection = false;
    useNullRejection = false;
    numInputDimensions = 0;
    numOutputDimensions = 0;
    numClasses = 0;
    predictedClassLabel = 0;
    maxLikelihood = 0;
    bestDistance = 0;
    phase = 0;
    trainingSetAccuracy = 0;
    nullRejectionCoeff = 5;
    numClassifierInstances++;
}
    
Classifier::~Classifier(void){
    if( --numClassifierInstances == 0 ){
        delete stringClassifierMap;
        stringClassifierMap = NULL;
    }
}
    
bool Classifier::copyBaseVariables(const Classifier *classifier){
    
    if( classifier == NULL ){
        errorLog << "copyBaseVariables(const Classifier *classifier) - Classifier is NULL!" << std::endl;
        return false;
    }
    
    if( !this->copyMLBaseVariables( classifier ) ){
        return false;
    }
    
    this->classifierMode = classifier->classifierMode;
    this->supportsNullRejection = classifier->supportsNullRejection;
    this->useNullRejection = classifier->useNullRejection;
    this->numClasses = classifier->numClasses;
    this->predictedClassLabel = classifier->predictedClassLabel;
    this->classifierMode = classifier->classifierMode;
    this->nullRejectionCoeff = classifier->nullRejectionCoeff;
    this->maxLikelihood = classifier->maxLikelihood;
    this->bestDistance = classifier->bestDistance;
    this->phase = classifier->phase;
    this->trainingSetAccuracy = classifier->trainingSetAccuracy;
    this->classLabels = classifier->classLabels;
    this->classLikelihoods = classifier->classLikelihoods;
    this->classDistances = classifier->classDistances;
    this->nullRejectionThresholds = classifier->nullRejectionThresholds;
	this->ranges = classifier->ranges;
    
    return true;
}
    
bool Classifier::reset(){
    
    //Reset the base class
    MLBase::reset();
    
    //Reset the classifier
    predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    maxLikelihood = 0;
    bestDistance = 0;
    phase = 0;
    if( trained ){
        classLikelihoods.clear();
        classDistances.clear();
        classLikelihoods.resize(numClasses,0);
        classDistances.resize(numClasses,0);
    }
    return true;
}
    
bool Classifier::clear(){
    
    //Clear the MLBase variables
    MLBase::clear();
    
    //Clear the classifier variables
    predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    maxLikelihood = 0;
    bestDistance = 0;
    phase = 0;
    trainingSetAccuracy = 0;
    classLikelihoods.clear();
    classDistances.clear();
    nullRejectionThresholds.clear();
    classLabels.clear();
    ranges.clear();
    
    return true;
}

bool Classifier::computeAccuracy( const ClassificationData &data, Float &accuracy ){ 
    return Metrics::computeAccuracy( *this, data, accuracy );
}

std::string Classifier::getClassifierType() const{
    return MLBase::getId(); 
}
    
bool Classifier::getSupportsNullRejection() const{
    return supportsNullRejection;
}
    
bool Classifier::getNullRejectionEnabled() const{ 
    return useNullRejection; 
}

Float Classifier::getNullRejectionCoeff() const{ 
    return nullRejectionCoeff; 
}
    
Float Classifier::getMaximumLikelihood() const{ 
    if( trained ) return maxLikelihood; 
    return DEFAULT_NULL_LIKELIHOOD_VALUE; 
}
  
Float Classifier::getPhase() const{
    return phase;
}

Float Classifier::getTrainingSetAccuracy() const{
    return trainingSetAccuracy;
}
    
Float Classifier::getBestDistance() const{ 
    if( trained ) return bestDistance; 
    return DEFAULT_NULL_DISTANCE_VALUE; 
}

UINT Classifier::getNumClasses() const{  
    return numClasses; 
}
    
UINT Classifier::getClassLabelIndexValue(UINT classLabel) const{
    for(UINT i=0; i<classLabels.size(); i++){
        if( classLabel == classLabels[i] )
            return i;
    }
    return 0;
}

UINT Classifier::getPredictedClassLabel() const{ 
    if( trained ) return predictedClassLabel; 
    return 0; 
}

VectorFloat Classifier::getClassLikelihoods() const{ 
    if( trained ) return classLikelihoods;
    return VectorFloat(); 
}

VectorFloat Classifier::getClassDistances() const{ 
    if( trained ) return classDistances; 
    return VectorFloat(); 
}

VectorFloat Classifier::getNullRejectionThresholds() const{ 
    if( trained ) return nullRejectionThresholds;
    return VectorFloat(); 
}

Vector< UINT > Classifier::getClassLabels() const{ 
    return classLabels;
}
    
Vector<MinMax> Classifier::getRanges() const{
    return ranges;
}

bool Classifier::enableNullRejection(bool useNullRejection){ 
    this->useNullRejection = useNullRejection; 
    return true;
}

bool Classifier::setNullRejectionCoeff(Float nullRejectionCoeff){ 
    if( nullRejectionCoeff > 0 ){ 
        this->nullRejectionCoeff = nullRejectionCoeff; 
        return true; 
    } 
    return false; 
}

bool Classifier::setNullRejectionThresholds(VectorFloat newRejectionThresholds){
	if( newRejectionThresholds.size() == getNumClasses() ){
		nullRejectionThresholds = newRejectionThresholds;
		return true;
	}
	return false;
}
    
const Classifier& Classifier::getBaseClassifier() const{
    return *this;
}
    
bool Classifier::saveBaseSettingsToFile( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "saveBaseSettingsToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    file << "UseNullRejection: " << useNullRejection << std::endl;
    file << "ClassifierMode: " << classifierMode << std::endl;
    file << "NullRejectionCoeff: " << nullRejectionCoeff << std::endl;
    
    if( trained ){
        
        file << "NumClasses: " << numClasses << std::endl;
        
        file << "NullRejectionThresholds: ";
		if (useNullRejection && nullRejectionThresholds.size()){
            for(UINT i=0; i<nullRejectionThresholds.size(); i++){
                file << " " << nullRejectionThresholds[i];
            }
            file << std::endl;
        }else{
            for(UINT i=0; i<numClasses; i++){
                file << " " << 0.0;
            }
            file << std::endl;
        }
        
        file << "ClassLabels: ";
        for(UINT i=0; i<classLabels.size(); i++){
            file << " " << classLabels[i];
        }
        file << std::endl;
        
        if( useScaling ){
            file << "Ranges: " << std::endl;
            for(UINT i=0; i<ranges.size(); i++){
                file << ranges[i].minValue << "\t" << ranges[i].maxValue << std::endl;
            }
        }
    }
    
    return true;
}

bool Classifier::loadBaseSettingsFromFile( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Try and load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile( file ) ){
        return false;
    }
    
    std::string word;
    
    //Load if the number of clusters
    file >> word;
    if( word != "UseNullRejection:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read UseNullRejection header!" << std::endl;
        clear();
        return false;
    }
    file >> useNullRejection;
    
    //Load if the classifier mode
    file >> word;
    if( word != "ClassifierMode:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read ClassifierMode header!" << std::endl;
        clear();
        return false;
    }
    file >> classifierMode;
    
    //Load if the null rejection coeff
    file >> word;
    if( word != "NullRejectionCoeff:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NullRejectionCoeff header!" << std::endl;
        clear();
        return false;
    }
    file >> nullRejectionCoeff;
    
    //If the model is trained then load the model settings
    if( trained ){
        
        //Load the number of classes
        file >> word;
        if( word != "NumClasses:" ){
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NumClasses header!" << std::endl;
            clear();
            return false;
        }
        file >> numClasses;
        
        //Load the null rejection thresholds
        file >> word;
        if( word != "NullRejectionThresholds:" ){
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NullRejectionThresholds header!" << std::endl;
            clear();
            return false;
        }
        nullRejectionThresholds.resize(numClasses);
        for(UINT i=0; i<nullRejectionThresholds.size(); i++){
            file >> nullRejectionThresholds[i];
        }
        
        //Load the class labels
        file >> word;
        if( word != "ClassLabels:" ){
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read ClassLabels header!" << std::endl;
            clear();
            return false;
        }
        classLabels.resize( numClasses );
        for(UINT i=0; i<classLabels.size(); i++){
            file >> classLabels[i];
        }
        
        if( useScaling ){
            //Load if the Ranges
            file >> word;
            if( word != "Ranges:" ){
                errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read Ranges header!" << std::endl;
                clear();
                return false;
            }
            ranges.resize(numInputDimensions);
            
            for(UINT i=0; i<ranges.size(); i++){
                file >> ranges[i].minValue;
                file >> ranges[i].maxValue;
            }
        }
    }
    
    return true;
}

GRT_END_NAMESPACE

