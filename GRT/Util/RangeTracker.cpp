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
#include "RangeTracker.h"

GRT_BEGIN_NAMESPACE

RangeTracker::RangeTracker(){
    trackData = true;
    numDimensions = 0;
    totalNumSamplesViewed = 0;
}

RangeTracker::RangeTracker(UINT numDimensions){
    trackData = true;
    setNumDimensions( numDimensions );
}

RangeTracker::RangeTracker(const RangeTracker &rhs){
    this->trackData = rhs.trackData;
    this->numDimensions = rhs.numDimensions;
    this->totalNumSamplesViewed = rhs.totalNumSamplesViewed;
    this->ranges = rhs.ranges;
}

RangeTracker::~RangeTracker(){};

void RangeTracker::clear(){
	totalNumSamplesViewed = 0;
	ranges.clear();
    if( numDimensions > 0 )
        ranges.resize(numDimensions,MinMax(BIG_POSITIVE_VALUE,BIG_NEGATIVE_VALUE));
}
    
bool RangeTracker::setNumDimensions(UINT numDimensions){
    
    if( numDimensions > 0 ){
        //Set the dimensionality of the data
        this->numDimensions = numDimensions;
        
        //Reset the ranges values
        clear();
        
        return true;
    }
    return false;
}

bool RangeTracker::update(VectorFloat sample){
	if( sample.size() != numDimensions ) return false;
    
    if( !trackData ) return true;
    
    totalNumSamplesViewed++;
    for(UINT j=0; j<numDimensions; j++){
        if( sample[j] < ranges[j].minValue ){ ranges[j].minValue = sample[j]; }
        else if( sample[j] > ranges[j].maxValue ){ ranges[j].maxValue = sample[j]; }
    }
	
	return true;
}

bool RangeTracker::saveRangeDataToFile(std::string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::out);

	if( !file.is_open() ){
		return false;
	}

	file << "GRT_RANGE_TRACKER_DATA_FILE_V1.0\n";
	file << "NumDimensions: " << numDimensions << std::endl;
	file << "TotalNumSamplesViewed: " << totalNumSamplesViewed << std::endl;
    file << "Ranges: " << std::endl;
    
    for(UINT i=0; i<ranges.getSize(); i++){
        file << ranges[i].minValue << "\t" << ranges[i].maxValue << std::endl;
    }
    
   	file.close();
	return true;
}

bool RangeTracker::loadRangeDataFromFile(std::string filename){

	std::fstream file; 
	file.open(filename.c_str(), std::ios::in);
	clear();

	if( !file.is_open() ){
        std::cout << "FILE NOT FOUND\n";
		return false;
	}

	std::string word;

	//Check to make sure this is a file with the correct file format
	file >> word;
	if(word != "GRT_RANGE_TRACKER_DATA_FILE_V1.0"){
		file.close();
		return false;
	}
    
   	//Get the number of dimensions in the data
	file >> word;
	if(word != "NumDimensions:"){
		file.close();
		return false;
	}
	file >> numDimensions;

	//Get the total number of training examples in the training data
	file >> word;
	if(word != "TotalNumSamplesViewed:"){
		file.close();
		return false;
	}
	file >> totalNumSamplesViewed;


    //Load the ranges
	file >> word;
	if(word != "Ranges:"){
		file.close();
		return false;
	}

    ranges.resize(numDimensions);
    for(UINT i=0; i<ranges.size(); i++){
        file >> ranges[i].minValue;
        file >> ranges[i].maxValue;
    }
    
	file.close();
	return true;
}
    
Vector<MinMax> RangeTracker::getRanges(){
    return ranges;
}
    
GRT_END_NAMESPACE
