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

#include "Clusterer.h"
namespace GRT{
    
Clusterer::StringClustererMap* Clusterer::stringClustererMap = NULL;
UINT Clusterer::numClustererInstances = 0;
    
Clusterer* Clusterer::createInstanceFromString(string const &clustererType){
    
    StringClustererMap::iterator iter = getMap()->find( clustererType );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}
Clusterer* Clusterer::createNewInstance() const{
    return createInstanceFromString( clustererType );
}
    
Clusterer* Clusterer::deepCopy() const{
    
    Clusterer *newInstance = createInstanceFromString( clustererType );
    
    if( newInstance == NULL ) return NULL;
    
    if( !newInstance->deepCopyFrom( this ) ){
        delete newInstance;
        return NULL;
    }
    return newInstance;
}

vector< string > Clusterer::getRegisteredClusterers(){
	vector< string > registeredClusterers;
	
	StringClustererMap::iterator iter = getMap()->begin();
	while( iter != getMap()->end() ){
		registeredClusterers.push_back( iter->first );
		iter++;
	}
	return registeredClusterers;
}
    
Clusterer::Clusterer(void){
    baseType = MLBase::CLUSTERER;
    clustererType = "NOT_SET";
    numClusters = 10;
    minNumEpochs = 1;
    maxNumEpochs = 1000;
    minChange = 1.0e-5;
    converged = false;
    numClustererInstances++;
}
    
Clusterer::~Clusterer(void){
    if( --numClustererInstances == 0 ){
        delete stringClustererMap;
        stringClustererMap = NULL;
    }
}
    
bool Clusterer::copyBaseVariables(const Clusterer *clusterer){
    
    if( clusterer == NULL ){
        errorLog << "copyBaseVariables(const Clusterer *clusterer) - clusterer is NULL!" << endl;
        return false;
    }
    
    if( !this->copyMLBaseVariables( clusterer ) ){
        return false;
    }
    
    //Copy the clusterer base variables
    this->clustererType = clusterer->clustererType;
    this->numClusters = clusterer->numClusters;
    this->converged = clusterer->converged;
    this->ranges = clusterer->ranges;
    
    return true;
}
    
bool Clusterer::reset(){
    
    //Reset the base class
    MLBase::reset();
    
    return true;
}
    
bool Clusterer::clear(){
    
    //Clear the MLBase variables
    MLBase::clear();
    
    return true;
}
    
bool Clusterer::saveClustererSettingsToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveClustererSettingsToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    file << "NumClusters: " << numClusters << endl;
    file << "Ranges: " << endl;
    
    for(UINT i=0; i<ranges.size(); i++){
        file << ranges[i].minValue << "\t" << ranges[i].maxValue << endl;
    }
    
    return true;
}

bool Clusterer::loadClustererSettingsFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadClustererSettingsFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Try and load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile( file ) ){
        return false;
    }
    
    string word;
    
    //Load if the number of clusters
    file >> word;
    if( word != "NumClusters:" ){
        errorLog << "loadClustererSettingsFromFile(fstream &file) - Failed to read NumClusters header!" << endl;
        clear();
        return false;
    }
    file >> numClusters;
    
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
    
    return true;
}
    
bool Clusterer::getConverged() const{
    if( !trained ) return false;
    return converged;
}
    
UINT Clusterer::getNumClusters() const { return numClusters; }

string Clusterer::getClustererType() const{ return clustererType; }
    
const Clusterer& Clusterer::getBaseClusterer() const{
    return *this;
}
    
bool Clusterer::setNumClusters(const UINT numClusters){
    if( numClusters == 0 ) return false;
    clear();
    this->numClusters = numClusters;
    return true;
}

} //End of namespace GRT

