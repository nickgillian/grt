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

#include "ClassLabelTimeoutFilter.h"

namespace GRT{
    
//Register the ClassLabelTimeoutFilter module with the PostProcessing base class
RegisterPostProcessingModule< ClassLabelTimeoutFilter > ClassLabelTimeoutFilter::registerModule("ClassLabelTimeoutFilter");
    
ClassLabelTimeoutFilter::ClassLabelTimeoutFilter(double timeoutDuration,UINT filterMode){
    classType = "ClassLabelTimeoutFilter";
    postProcessingType = classType;
    postProcessingInputMode = INPUT_MODE_PREDICTED_CLASS_LABEL;
    postProcessingOutputMode = OUTPUT_MODE_PREDICTED_CLASS_LABEL;
    debugLog.setProceedingText("[DEBUG ClassLabelTimeoutFilter]");
    errorLog.setProceedingText("[ERROR ClassLabelTimeoutFilter]");
    warningLog.setProceedingText("[WARNING ClassLabelTimeoutFilter]");
    init(timeoutDuration,filterMode);
}
    
ClassLabelTimeoutFilter::ClassLabelTimeoutFilter(const ClassLabelTimeoutFilter &rhs){
    
    classType = "ClassLabelTimeoutFilter";
    postProcessingType = classType;
    postProcessingInputMode = INPUT_MODE_PREDICTED_CLASS_LABEL;
    postProcessingOutputMode = OUTPUT_MODE_PREDICTED_CLASS_LABEL;
    debugLog.setProceedingText("[DEBUG ClassLabelTimeoutFilter]");
    errorLog.setProceedingText("[ERROR ClassLabelTimeoutFilter]");
    warningLog.setProceedingText("[WARNING ClassLabelTimeoutFilter]");
    
    //Copy the classLabelTimeoutFilter values 
    this->filteredClassLabel = rhs.filteredClassLabel;
    this->filterMode = rhs.filterMode;
    this->timeoutDuration = rhs.timeoutDuration;
    this->classLabelTimers = rhs.classLabelTimers;
    
    //Clone the post processing base variables
    copyBaseVariables( (PostProcessing*)&rhs );
}

ClassLabelTimeoutFilter::~ClassLabelTimeoutFilter(){

}
    
ClassLabelTimeoutFilter& ClassLabelTimeoutFilter::operator=(const ClassLabelTimeoutFilter &rhs){
    
    if( this != &rhs ){
        //Copy the classLabelTimeoutFilter values 
        this->filteredClassLabel = rhs.filteredClassLabel;
        this->filterMode = rhs.filterMode;
        this->timeoutDuration = rhs.timeoutDuration;
        this->classLabelTimers = rhs.classLabelTimers;
        
        //Clone the post processing base variables
        copyBaseVariables( (PostProcessing*)&rhs );
    }
        return *this;
}
    
bool ClassLabelTimeoutFilter::deepCopyFrom(const PostProcessing *postProcessing){
    
    if( postProcessing == NULL ) return false;
    
    if( this->getPostProcessingType() == postProcessing->getPostProcessingType() ){
        
        ClassLabelTimeoutFilter *ptr = (ClassLabelTimeoutFilter*)postProcessing;
        
        //Clone the classLabelTimeoutFilter values 
        this->filteredClassLabel = ptr->filterMode;
        this->filterMode = ptr->filterMode;
        this->timeoutDuration = ptr->timeoutDuration;
        this->classLabelTimers = ptr->classLabelTimers;
        
        //Clone the post processing base variables
        return copyBaseVariables( postProcessing );
    }
    return false;
}
    
bool ClassLabelTimeoutFilter::process(const VectorDouble &inputVector){
    
#ifdef GRT_SAFE_CHECKING
    if( !initialized ){
        errorLog << "process(const VectorDouble &inputVector) - Not initialized!" << endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "process(const VectorDouble &inputVector) - The size of the inputVector (" << inputVector.size() << ") does not match that of the filter (" << numInputDimensions << ")!" << endl;
        return false;
    }
#endif
    
    //Use only the first value (as that is the predicted class label)
    processedData[0] = filter( (UINT)inputVector[0] );
    return true;
}
    
bool ClassLabelTimeoutFilter::reset(){
    filteredClassLabel = 0;   
    classLabelTimers.clear();
    processedData.clear();
    processedData.resize(1,0);
    return true;
}
    
bool ClassLabelTimeoutFilter::init(double timeoutDuration,UINT filterMode){

    initialized = false;
    
#ifdef GRT_SAFE_CHECKING
    if( timeoutDuration < 0 ){
        errorLog << "init(double timeoutDuration,UINT filterMode) - TimeoutDuration must be greater or equal to 0!" << endl;
        return false;
    }
    
    if( filterMode != ALL_CLASS_LABELS && filterMode != INDEPENDENT_CLASS_LABELS ){
        errorLog << "init(double timeoutDuration,UINT filterMode) - Unkown filter mode!" << endl;
        return false;
    }
#endif
    
    this->timeoutDuration = timeoutDuration;
    this->filterMode = filterMode;
    numInputDimensions = 1;
    numOutputDimensions = 1;
    initialized = reset();
    return true;
}

UINT ClassLabelTimeoutFilter::filter(UINT predictedClassLabel){
    
    //If we get the NULL class and there are no active timers running then we do not need to do anything
    if( predictedClassLabel == 0 && classLabelTimers.size() == 0 ){
        filteredClassLabel = predictedClassLabel;
        return filteredClassLabel;
    }
    
    bool matchFound = false;
    vector< ClassLabelAndTimer >::iterator iter;
    
    switch( filterMode ){
        case ALL_CLASS_LABELS:
            
            //Have we seen any class label yet, if not then just start the timer and return the current class label
            if( classLabelTimers.size() == 0 ){
                filteredClassLabel = predictedClassLabel;
                classLabelTimers.push_back( ClassLabelAndTimer(predictedClassLabel,timeoutDuration) );
            }else{
                //Otherwise check to see if the timer has timed-out
                if( classLabelTimers[0].timerReached() ){
                    //Clear the timer
                    classLabelTimers.clear();
                    
                    //Check if the current predictedClassLabel is a valid gesture, if so then recursively call this function
                    //to start a new filter
                    filteredClassLabel = 0;
                    if( predictedClassLabel ){
                        filteredClassLabel = filter( predictedClassLabel );
                    }
                    
                }else filteredClassLabel = 0;
            }
            
            break;
        case INDEPENDENT_CLASS_LABELS:
            
            //Search the classLabelTimers buffer to find a matching class label
            if( classLabelTimers.size() > 0 ){
                iter = classLabelTimers.begin();
                
                while( iter != classLabelTimers.end() ){
                    if( iter->getClassLabel() == predictedClassLabel ){
                        //Check to see if the timer for this class has elapsed
                        if( iter->timerReached() ){
                            //Reset the timer for this label
                            iter->set(predictedClassLabel,timeoutDuration);
                            
                            //Signal that a match was found
                            matchFound = true;
                            filteredClassLabel = predictedClassLabel;
                            break;
                        }else filteredClassLabel = 0;
                        
                        //Update the iterator
                        iter++;
                    }else{
                        if( iter->timerReached() ){
                            //Erase the current timer from the buffer
                            iter = classLabelTimers.erase( iter );
                        }else iter++;
                    }
                }
                
            }
            
            //If a match has not been found then create a new timer
            if( !matchFound ){
                classLabelTimers.push_back( ClassLabelAndTimer(predictedClassLabel,timeoutDuration) );
                filteredClassLabel = predictedClassLabel;
            }
                        
            break;
    }
    
	return filteredClassLabel;
}
    
bool ClassLabelTimeoutFilter::isTimeoutActive(){

    for(UINT i=0; i<classLabelTimers.size(); i++){
        if( classLabelTimers[i].timerReached() ){
            return true;
        }
    }
    
    return false;
}
    
bool ClassLabelTimeoutFilter::saveModelToFile(string filename) const{
    
    if( !initialized ){
        errorLog << "saveModelToFile(string filename) - The ClassLabelTimeoutFilter has not been initialized" << endl;
        return false;
    }
    
    std::fstream file; 
    file.open(filename.c_str(), std::ios::out);
    
    if( !saveModelToFile( file ) ){
        file.close();
        return false;
    }
    
    file.close();
    
    return true;
}

bool ClassLabelTimeoutFilter::saveModelToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveModelToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    file << "GRT_CLASS_LABEL_TIMEOUT_FILTER_FILE_V1.0" << endl;
    file << "NumInputDimensions: " << numInputDimensions << endl;
    file << "NumOutputDimensions: " << numOutputDimensions << endl;
    file << "FilterMode: " << filterMode << endl;
    file << "TimeoutDuration: " << timeoutDuration << endl;	
    
    return true;
}

bool ClassLabelTimeoutFilter::loadModelFromFile(string filename){
    
    std::fstream file; 
    file.open(filename.c_str(), std::ios::in);
    
    if( !loadModelFromFile( file ) ){
        file.close();
        initialized = false;
        return false;
    }
    
    file.close();
    
    return true;
}

bool ClassLabelTimeoutFilter::loadModelFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadModelFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    string word;
    
    //Load the header
    file >> word;
    
    if( word != "GRT_CLASS_LABEL_TIMEOUT_FILTER_FILE_V1.0" ){
        errorLog << "loadModelFromFile(fstream &file) - Invalid file format!" << endl;
        return false;     
    }
    
    file >> word;
    if( word != "NumInputDimensions:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumInputDimensions header!" << endl;
        return false;     
    }
    file >> numInputDimensions;
    
    //Load the number of output dimensions
    file >> word;
    if( word != "NumOutputDimensions:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read NumOutputDimensions header!" << endl;
        return false;     
    }
    file >> numOutputDimensions;
    
    //Load the filterMode
    file >> word;
    if( word != "FilterMode:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read FilterMode header!" << endl;
        return false;     
    }
    file >> filterMode;
    
    file >> word;
    if( word != "TimeoutDuration:" ){
        errorLog << "loadModelFromFile(fstream &file) - Failed to read TimeoutDuration header!" << endl;
        return false;     
    }
    file >> timeoutDuration;
    
    //Init the classLabelTimeoutFilter module to ensure everything is initialized correctly
    return init(timeoutDuration,filterMode);
}
    
bool ClassLabelTimeoutFilter::setTimeoutDuration(double timeoutDuration){
    if( timeoutDuration < 0 ) return false;
    this->timeoutDuration = timeoutDuration;
    if( initialized ){
        return reset();
    }
    return true;
}

bool ClassLabelTimeoutFilter::setFilterMode(UINT filterMode){
    if( filterMode != ALL_CLASS_LABELS && filterMode != INDEPENDENT_CLASS_LABELS ) return false;
    this->filterMode = filterMode;
    if( initialized ){
        return reset();
    }
    return true;
}

}//End of namespace GRT