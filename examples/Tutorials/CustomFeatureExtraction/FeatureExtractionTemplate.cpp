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

#include "FeatureExtractionTemplate.h"

GRT_BEGIN_NAMESPACE //This macro places all the following code inside the GRT namespace

const std::string FeatureExtractionTemplate::id = "FeatureExtractionTemplate";
std::string FeatureExtractionTemplate::getId() { return FeatureExtractionTemplate::id; }
    
//Register your module with the FeatureExtraction base class
//The string you pass to the registerModule function must match the string you use for the featureExtractionType
RegisterFeatureExtractionModule< FeatureExtractionTemplate > FeatureExtractionTemplate::registerModule( FeatureExtractionTemplate::getId() );
    
FeatureExtractionTemplate::FeatureExtractionTemplate() : FeatureExtraction( FeatureExtractionTemplate::getId() )
{    
    //Here you should add any specific code to init your custom feature extraction module
    //You might want to add this code to a specific init function (so you can call this from other functions such as the loadSettingsFromFile(..) function for instance)
    //Don't forget to resize the featureVector (which is in the FeatureExtraction base class) to the appropriate size
    //The other variables you need to set are:
    //bool initialized: a flag that indicates if the feature extraction module has been initialized
    //bool featureDataReady: a flag that indicates if the feature data has been computed, this should be set to true in the computeFeatures(...) function)
    //UINT numInputDimensions: sets the size of the input vector (the vector that will be passed through the computeFeatures(...) function)
    //UINT numOutputDimensions: sets the size of the featureVector (the output of the feature extraction module)
}
    
FeatureExtractionTemplate::FeatureExtractionTemplate(const FeatureExtractionTemplate &rhs) : FeatureExtraction( FeatureExtractionTemplate::getId() )
{
    //Invoke the equals operator to copy the data from the rhs instance to this instance
    *this = rhs;
}

FeatureExtractionTemplate::~FeatureExtractionTemplate(){
    //Here you should add any specific code to cleanup your custom feature extraction module if needed
}
    
FeatureExtractionTemplate& FeatureExtractionTemplate::operator=(const FeatureExtractionTemplate &rhs){
    if(this!=&rhs){
        //Here you should copy any class variables from the rhs instance to this instance
        
        //Copy the base variables
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}
    
bool FeatureExtractionTemplate::deepCopyFrom(const FeatureExtraction *featureExtraction){
    
    if( featureExtraction == NULL ) return false;
    
    if( this->getId() == featureExtraction->getId() ){
        
        //Cast the feature extraction pointer to a pointer to your custom feature extraction module
        //Then invoke the equals operator
        *this = *dynamic_cast<const FeatureExtractionTemplate*>( featureExtraction );
        return true;
    }
    
    errorLog << "deepCopyFrom(FeatureExtraction *featureExtraction) -  FeatureExtraction Types Do Not Match!" << std::endl;
    
    return false;
}
    
bool FeatureExtractionTemplate::computeFeatures(const VectorFloat &inputVector){
    
#ifdef GRT_SAFE_CHECKING
    if( !initialized ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - Not initialized!" << std::endl;
        return false;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "computeFeatures(const VectorFloat &inputVector) - The size of the inputVector (" << inputVector.getSize() << ") does not match that of the filter (" << numInputDimensions << ")!" << std::endl;
        return false;
    }
#endif
    
    //Here is where you should add your custom feature extraction code
    //The function should return true unless something goes wrong
    //Make sure you copy the results of your feature extraction into featureVector so other modules in the pipeline can access the results
    //You should set the featureDataReady variable to true to indicate that the feature data has been computed
    
    return true;
}

bool FeatureExtractionTemplate::reset(){
    //Add your own custom reset code here, make sure this function returns true otherwise the pipeline reset will not work!
    return true;
}

bool FeatureExtractionTemplate::save( std::fstream &file ) const{
    
    if( !file.is_open() ){
        errorLog << "saveSettingsToFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Here is where you should add your own custom code to save any settings to a file, this should be done in three steps...
    
    //First, you should add a header (with no spaces) e.g.
    file << "CUSTOM_FEATURE_EXTRACTION_FILE_V1.0" << std::endl;
	
    //Second, you should save the base feature extraction settings to the file
    if( !saveBaseSettingsToFile( file ) ){
        errorLog << "saveSettingsToFile(fstream &file) - Failed to save base feature extraction settings to file!" << std::endl;
        return false;
    }
    
    //Finally, you should add anything needed for your custom feature extraction module
    
    //NOTE: You should not close the file as this will be done by the function that calls this function
    return true;
}

bool FeatureExtractionTemplate::load( std::fstream &file ){
    
    if( !file.is_open() ){
        errorLog << "loadSettingsFromFile(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Here is where you should add your own custom code to load any settings to a file, this should be done in three steps...
    
    //First, you should read and validate the header
    file >> word;
    
    if( word != "CUSTOM_FEATURE_EXTRACTION_FILE_V1.0" ){
        errorLog << "loadSettingsFromFile(fstream &file) - Invalid file format!" << std::endl;
        return false;
    }
    
    //Second, you should load the base feature extraction settings to the file
    if( !loadBaseSettingsFromFile( file ) ){
        errorLog << "loadBaseSettingsFromFile(fstream &file) - Failed to load base feature extraction settings from file!" << std::endl;
        return false;
    }
    
    //Finally, you should load any custom contents    
    
    
    //When you have loaded all your content you should then initialize the module
    
    //NOTE: You should not close the file as this will be done by the function that calls this function
    return true;
}
    
GRT_END_NAMESPACE //This macro places all the code above inside the GRT namespace
