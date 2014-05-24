/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This is the main base class that all GRT Feature Extraction algorithms should inherit from.
 
 A large number of the functions in this class are virtual and simply return false as these functions must be overwridden by the inheriting class.
 */

/**
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

#ifndef GRT_FEATURE_EXTRACTION_HEADER
#define GRT_FEATURE_EXTRACTION_HEADER

#include "MLBase.h"

namespace GRT{

class FeatureExtraction : public MLBase
{
public:
    /**
     Default FeatureExtraction Constructor
     */
	FeatureExtraction();
    
    /**
     Default FeatureExtraction Destructor
     */
	virtual ~FeatureExtraction();
    
    /**
     This is the base deepCopyFrom function for the FeatureExtraction modules. This function should be overwritten by the derived class.
     
     @param const FeatureExtraction *featureExtraction: a pointer to the FeatureExtraction base class, this should be pointing to another instance of a matching derived class
     @return returns true if the deep copy was successfull, false otherwise (the FeatureExtraction base class will always return flase)
     */
    virtual bool deepCopyFrom(const FeatureExtraction *rhs){ return false; };
    
    /**
     This copies the FeatureExtraction variables from featureExtractionModule to the instance that calls the function.
     
     @param const FeatureExtraction *featureExtractionModule: a pointer to a feature extraction module from which the values will be copied
     @return returns true if the copy was successfull, false otherwise
     */
    bool copyBaseVariables(const FeatureExtraction *featureExtractionModule);

    /**
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function should be overwritten by the derived class.
     
     @param const VectorDouble &inputVector: the inputVector that should be processed
     @return returns true if the data was processed, false otherwise (the base class always returns false)
     */
    virtual bool computeFeatures(const VectorDouble &inputVector){ return false; }
    
    /**
     This function is called by the GestureRecognitionPipeline's reset function.
     This function should be overwritten by the derived class.
     
     @return returns true if the module was reset, false otherwise (the base class always returns true)
     */
    virtual bool reset(){ return true; }
    
    /**
     This function clears any previous setup.
     
     @return returns true if the module was cleared, false otherwise
     */
    virtual bool clear();
    
    /**
     This saves the feature extraction settings to a file.
     This function should be overwritten by the derived class.
     
     @param const string filename: the filename to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(const string filename) const;
    
    /**
     This saves the feature extraction settings to a file.
     This function should be overwritten by the derived class.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool loadModelFromFile(const string filename);
    
    /**
     This saves the feature extraction settings to a file.
     This function should be overwritten by the derived class.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise (the base class always returns false)
     */
    virtual bool saveModelToFile(fstream &file) const{ return false; }
    
    /**
     This loads the feature extraction settings from a file.
     This function should be overwritten by the derived class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise (the base class always returns false)
     */
    virtual bool loadModelFromFile(fstream &file){ return false; }
	
    /**
     Returns the feature extraction type as a string.
     
     @return returns the feature extraction type as a string
     */
    string getFeatureExtractionType() const;
    
    /**
     Returns the size of the input vector expected by the feature extraction module.
     
     @return returns the size of the input vector expected by the feature extraction module
     */
    UINT getNumInputDimensions() const;
    
    /**
     Returns the size of the feature vector that will be computed by the feature extraction module.
     
     @return returns the size of the feature vector that will be computed by the feature extraction module
     */
    UINT getNumOutputDimensions() const;
    
    /**
     Returns true if the feature extraction module has been initialized correctly.
     
     @return returns true if the feature extraction module has been initialized succesfully, false otherwise
     */
    bool getInitialized() const;
    
    /**
     Returns true if the feature extraction module has just processed the last input vector and a new output feature vector is ready.
     
     @return returns true if the feature extraction module has just processed the last input vector and a new output feature vector is ready, false otherwise
     */
    bool getFeatureDataReady() const;
    
    /**
     Returns the current feature vector.
     
     @return returns the current feature vector, this vector will be empty if the module has not been initialized
     */
    VectorDouble getFeatureVector() const;
    
    /**
     Defines a map between a string (which will contain the name of the featureExtraction module, such as FFT) and a function returns a new instance of that featureExtraction
     */
    typedef std::map< string, FeatureExtraction*(*)() > StringFeatureExtractionMap;
    
    /*
     Creates a new feature extraction instance based on the input string (which should contain the name of a valid feature extraction such as FFT).
    
    @param string const &featureExtractionType: the name of the feature extraction module
    @return FeatureExtraction*: a pointer to the new instance of the feature extraction
    */
    static FeatureExtraction* createInstanceFromString(string const &featureExtractionType);
    
    /**
     Creates a new feature extraction instance based on the current featureExtractionType string value.
     
     @return FeatureExtraction*: a pointer to the new instance of the feature extraction
     */
    FeatureExtraction* createNewInstance() const;
    
protected:
    /**
     Initializes the base feature extraction module, this will resize the feature vector and get the instance ready for processing new data.
     
     @return returns true if the module was initialized, false otherwise
     */
    bool init();
    
    /**
     Saves the core base settings to a file.
     
     @return returns true if the base settings were saved, false otherwise
     */
    bool saveFeatureExtractionSettingsToFile(fstream &file) const;
    
    /**
     Loads the core base settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadFeatureExtractionSettingsFromFile(fstream &file);

    
    string featureExtractionType;
    bool initialized;
    bool featureDataReady;
    VectorDouble featureVector;
    static StringFeatureExtractionMap *getMap() {
        if( !stringFeatureExtractionMap ){ stringFeatureExtractionMap = new StringFeatureExtractionMap; } 
        return stringFeatureExtractionMap; 
    }
    
private:
    static StringFeatureExtractionMap *stringFeatureExtractionMap;
    static UINT numFeatureExtractionInstances;
    
};
    
//These two functions/classes are used to register any new FeatureExtraction Module with the FeatureExtraction base class
template< typename T >  FeatureExtraction *newFeatureExtractionModuleInstance() { return new T; }

template< typename T > 
class RegisterFeatureExtractionModule : FeatureExtraction { 
public:
    RegisterFeatureExtractionModule(string const &newFeatureExtractionModuleName) { 
        getMap()->insert( std::pair<string, FeatureExtraction*(*)()>(newFeatureExtractionModuleName, &newFeatureExtractionModuleInstance< T > ) );
    }
};

} //End of namespace GRT

#endif //GRT_FEATURE_EXTRACTION_HEADER
