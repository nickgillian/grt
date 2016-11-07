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

GRT_BEGIN_NAMESPACE

class GRT_API FeatureExtraction : public MLBase
{
public:
    /**
     Default FeatureExtraction Constructor
     @param id: the id of the class inheriting from the FeatureExtraction base class
     */
	FeatureExtraction( const std::string id = "" );
    
    /**
     Default FeatureExtraction Destructor
     */
	virtual ~FeatureExtraction();
    
    /**
     This is the base deepCopyFrom function for the FeatureExtraction modules. This function should be overwritten by the derived class.
     
     @param featureExtraction: a pointer to the FeatureExtraction base class, this should be pointing to another instance of a matching derived class
     @return returns true if the deep copy was successfull, false otherwise (the FeatureExtraction base class will always return flase)
     */
    virtual bool deepCopyFrom(const FeatureExtraction *rhs){ return false; };
    
    /**
     This copies the FeatureExtraction variables from featureExtractionModule to the instance that calls the function.
     
     @param featureExtractionModule: a pointer to a feature extraction module from which the values will be copied
     @return returns true if the copy was successfull, false otherwise
     */
    bool copyBaseVariables(const FeatureExtraction *featureExtractionModule);

    /**
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function should be overwritten by the derived class.
     
     @param inputVector: the inputVector that should be processed
     @return returns true if the data was processed, false otherwise (the base class always returns false)
     */
    virtual bool computeFeatures(const VectorFloat &inputVector){ return false; }

    /**
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This function should be overwritten by the derived class.
     
     @param inputMatrix: the inputVector that should be processed
     @return returns true if the data was processed, false otherwise (the base class always returns false)
     */
    virtual bool computeFeatures(const MatrixFloat &inputMatrix){ return false; }
    
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
    const VectorFloat& getFeatureVector() const;

    /**
     Returns the current feature matrix.
     
     @return returns the current feature matrix, this matrix will be empty if the module has not been initialized
     */
    const MatrixFloat& getFeatureMatrix() const;
    
    /**
     Defines a map between a string (which will contain the name of the featureExtraction module, such as FFT) and a function returns a new instance of that featureExtraction
     */
    typedef std::map< std::string, FeatureExtraction*(*)() > StringFeatureExtractionMap;
    
    /*
     Creates a new feature extraction instance based on the input string (which should contain the name of a valid feature extraction such as FFT).
    
    @param id: the name of the feature extraction module
    @return a pointer to the new instance of the feature extraction
    */
    static FeatureExtraction* create( const std::string &id );
    
    /**
     Creates a new feature extraction instance based on the current featureExtractionType string value.
     
     @return a pointer to the new instance of the feature extraction
     */
    FeatureExtraction* create() const;
    
    using MLBase::save;
    using MLBase::load;

    GRT_DEPRECATED_MSG( "createNewInstance is deprecated, use create() instead.", FeatureExtraction* createNewInstance() const );
    GRT_DEPRECATED_MSG( "createInstanceFromString(id) is deprecated, use create(id) instead.", static FeatureExtraction* createInstanceFromString( const std::string &id ) );
    GRT_DEPRECATED_MSG( "getFeatureExtractionType is deprecated, use getId() instead", std::string getFeatureExtractionType() const );
    
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
    bool saveFeatureExtractionSettingsToFile( std::fstream &file ) const;
    
    /**
     Loads the core base settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadFeatureExtractionSettingsFromFile( std::fstream &file );

    std::string featureExtractionType;
    bool initialized;
    bool featureDataReady;
    VectorFloat featureVector;
    MatrixFloat featureMatrix;
    
    static StringFeatureExtractionMap *getMap() {
        if( !stringFeatureExtractionMap ){ stringFeatureExtractionMap = new StringFeatureExtractionMap; } 
        return stringFeatureExtractionMap; 
    }
    
private:
    static StringFeatureExtractionMap *stringFeatureExtractionMap;
    static UINT numFeatureExtractionInstances;
    
};
    
template< typename T >  FeatureExtraction *createNewFeatureExtractionModule() { return new T; } ///< Returns a pointer to a new instance of the template class, the caller is responsible for deleting the pointer

template< typename T > 
class RegisterFeatureExtractionModule : FeatureExtraction { 
public:
    RegisterFeatureExtractionModule( const std::string &newModuleId ) { 
        getMap()->insert( std::pair< std::string, FeatureExtraction*(*)()>(newModuleId, &createNewFeatureExtractionModule< T > ) );
    }
};

GRT_END_NAMESPACE

#endif //GRT_FEATURE_EXTRACTION_HEADER
