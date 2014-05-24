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

#include "Classifier.h"
namespace GRT{
    
Classifier::StringClassifierMap* Classifier::stringClassifierMap = NULL;
UINT Classifier::numClassifierInstances = 0;
    
Classifier* Classifier::createInstanceFromString(string const &classifierType){
    
    StringClassifierMap::iterator iter = getMap()->find( classifierType );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}
Classifier* Classifier::createNewInstance() const{
    return createInstanceFromString( classifierType );
}
    
Classifier* Classifier::deepCopy() const{
    
    Classifier *newInstance = createInstanceFromString( classifierType );
    
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

vector< string > Classifier::getRegisteredClassifiers(){
	vector< string > registeredClassifiers;
	
	StringClassifierMap::iterator iter = getMap()->begin();
	while( iter != getMap()->end() ){
		registeredClassifiers.push_back( iter->first );
		iter++;
	}
	return registeredClassifiers;
}
    
Classifier::Classifier(void){
    baseType = MLBase::CLASSIFIER;
    classifierMode = STANDARD_CLASSIFIER_MODE;
    classifierType = "NOT_SET";
    useNullRejection = false;
    numInputDimensions = 0;
    numOutputDimensions = 1;
    numClasses = 0;
    predictedClassLabel = 0;
    maxLikelihood = 0;
    bestDistance = 0;
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
        errorLog << "copyBaseVariables(const Classifier *classifier) - Classifier is NULL!" << endl;
        return false;
    }
    
    if( !this->copyMLBaseVariables( classifier ) ){
        return false;
    }
    
    this->classifierType = classifier->classifierType;
    this->classifierMode = classifier->classifierMode;
    this->useNullRejection = classifier->useNullRejection;
    this->numClasses = classifier->numClasses;
    this->predictedClassLabel = classifier->predictedClassLabel;
    this->nullRejectionCoeff = classifier->nullRejectionCoeff;
    this->maxLikelihood = classifier->maxLikelihood;
    this->bestDistance = classifier->bestDistance;
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
    maxLikelihood = 0;
    bestDistance = 0;
    classLikelihoods.clear();
    classDistances.clear();
    nullRejectionThresholds.clear();
    classLabels.clear();
    ranges.clear();
    
    return true;
}

string Classifier::getClassifierType() const{
    return classifierType; 
}
    
bool Classifier::getNullRejectionEnabled() const{ 
    return useNullRejection; 
}

double Classifier::getNullRejectionCoeff() const{ 
    return nullRejectionCoeff; 
}
    
double Classifier::getMaximumLikelihood() const{ 
    if( trained ) return maxLikelihood; 
    return DEFAULT_NULL_LIKELIHOOD_VALUE; 
}
    
double Classifier::getBestDistance() const{ 
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

VectorDouble Classifier::getClassLikelihoods() const{ 
    if( trained ) return classLikelihoods;
    return VectorDouble(); 
}

VectorDouble Classifier::getClassDistances() const{ 
    if( trained ) return classDistances; 
    return VectorDouble(); 
}

VectorDouble Classifier::getNullRejectionThresholds() const{ 
    if( trained ) return nullRejectionThresholds;
    return VectorDouble(); 
}

vector< UINT > Classifier::getClassLabels() const{ 
    return classLabels;
}
    
vector<MinMax> Classifier::getRanges() const{
    return ranges;
}

bool Classifier::enableNullRejection(bool useNullRejection){ 
    this->useNullRejection = useNullRejection; 
    return true;
}

bool Classifier::setNullRejectionCoeff(double nullRejectionCoeff){ 
    if( nullRejectionCoeff > 0 ){ 
        this->nullRejectionCoeff = nullRejectionCoeff; 
        return true; 
    } 
    return false; 
}

bool Classifier::setNullRejectionThresholds(VectorDouble newRejectionThresholds){
	if( newRejectionThresholds.size() == getNumClasses() ){
		nullRejectionThresholds = newRejectionThresholds;
		return true;
	}
	return false;
}
    
const Classifier& Classifier::getBaseClassifier() const{
    return *this;
}
    
bool Classifier::saveBaseSettingsToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveBaseSettingsToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    file << "UseNullRejection: " << useNullRejection << endl;
    file << "ClassifierMode: " << classifierMode << endl;
    file << "NullRejectionCoeff: " << nullRejectionCoeff << endl;
    
    if( trained ){
        
        file << "NumClasses: " << numClasses << endl;
        
        file << "NullRejectionThresholds: ";
        if( useNullRejection ){
            for(UINT i=0; i<nullRejectionThresholds.size(); i++){
                file << " " << nullRejectionThresholds[i];
            }
            file << endl;
        }else{
            for(UINT i=0; i<numClasses; i++){
                file << " " << 0.0;
            }
            file << endl;
        }
        
        file << "ClassLabels: ";
        for(UINT i=0; i<classLabels.size(); i++){
            file << " " << classLabels[i];
        }
        file << endl;
        
        if( useScaling ){
            file << "Ranges: " << endl;
            for(UINT i=0; i<ranges.size(); i++){
                file << ranges[i].minValue << "\t" << ranges[i].maxValue << endl;
            }
        }
    }
    
    return true;
}

bool Classifier::loadBaseSettingsFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Try and load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile( file ) ){
        return false;
    }
    
    string word;
    
    //Load if the number of clusters
    file >> word;
    if( word != "UseNullRejection:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read UseNullRejection header!" << endl;
        clear();
        return false;
    }
    file >> useNullRejection;
    
    //Load if the classifier mode
    file >> word;
    if( word != "ClassifierMode:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read ClassifierMode header!" << endl;
        clear();
        return false;
    }
    file >> classifierMode;
    
    //Load if the null rejection coeff
    file >> word;
    if( word != "NullRejectionCoeff:" ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NullRejectionCoeff header!" << endl;
        clear();
        return false;
    }
    file >> nullRejectionCoeff;
    
    //If the model is trained then load the model settings
    if( trained ){
        
        //Load the number of classes
        file >> word;
        if( word != "NumClasses:" ){
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NumClasses header!" << endl;
            clear();
            return false;
        }
        file >> numClasses;
        
        //Load the null rejection thresholds
        file >> word;
        if( word != "NullRejectionThresholds:" ){
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read NullRejectionThresholds header!" << endl;
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
            errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to read ClassLabels header!" << endl;
            clear();
            return false;
        }
        classLabels.resize(numClasses);
        for(UINT i=0; i<classLabels.size(); i++){
            file >> classLabels[i];
        }
        
        if( useScaling ){
            //Load if the Ranges
            file >> word;
            if( word != "Ranges:" ){
                errorLog << "loadClustererSettingsFromFile(fstream &file) - Failed to read Ranges header!" << endl;
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

} //End of namespace GRT

