/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This is the main base class that all GRT PreProcessing algorithms should inherit from.
 
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

#ifndef GRT_PRE_PROCESSING_HEADER
#define GRT_PRE_PROCESSING_HEADER

#include "MLBase.h"

GRT_BEGIN_NAMESPACE

class GRT_API PreProcessing : public MLBase
{
public:
    /**
     Default Constructor
     @param id: the id of the parent PreProcessing module (e.g., LowPassFilter)
     */
	PreProcessing( const std::string &id = "" );
	
    /**
     Default Destructor
     */
	virtual ~PreProcessing(void);
    
    /**
     This is the base deepCopyFrom function for the PreProcessing modules. This function should be overwritten by the derived class.
     
     @param preProcessing: a pointer to the PreProcessing base class, this should be pointing to another instance of a matching derived class
     @return returns true if the deep copy was successfull, false otherwise (the PreProcessing base class will always return flase)
     */
    virtual bool deepCopyFrom(const PreProcessing *rhs){ return false; }
    
    /**
     This copies the PreProcessing variables from preProcessing to the instance that calls the function.
     
     @param preProcessing: a pointer to a pre processing module from which the values will be copied
     @return returns true if the copy was successfull, false otherwise
     */
    bool copyBaseVariables(const PreProcessing *preProcessingModule);

    /**
     This is the main processing interface for all the pre processing modules and should be overwritten by the inheriting class.
     
     @param inputVector: a vector containing the data that should be processed
     @return returns true if the pre processing was successfull, false otherwise
     */
    virtual bool process(const VectorFloat &inputVector){ return false; }
    
    /**
     This is the main reset interface for all the GRT preprocessing modules. This should be overwritten by the derived class.
     
     @return returns true if the derived class was reset succesfully, false otherwise (the base class always returns true)
     */
    virtual bool reset();
    
    /**
     This is the main clear interface for all the GRT preprocessing modules. This should be overwritten by the derived class.
     It will completely clear the module, removing any IO values setting and all the base variables to their default values.
     
     @return returns true if the derived class was cleared succesfully, false otherwise
     */
    virtual bool clear();
	
    /**
     Returns the size of the input vector expected by the pre processing module.
     
     @return returns the size of the input vector expected by the pre processing module
     */
    UINT getNumInputDimensions() const;
    
    /**
     Returns the size of the vector that will be computed by the pre processing module.
     
     @return returns the size of the vector that will be computed by the pre processing module
     */
    UINT getNumOutputDimensions() const;
    
    /**
     Returns true if the pre processing module has been initialized correctly.
     
     @return returns true if the pre processing module has been initialized succesfully, false otherwise
     */
    bool getInitialized() const;

    /**
     @return returns a VectorFloat containing the most recent processed data
     */
	VectorFloat getProcessedData() const;
    
    /**
     This typedef defines a map between a string and a PreProcessing pointer.
     */
	typedef std::map< std::string, PreProcessing*(*)() > StringPreProcessingMap;
    
    /**
     This static function will dynamically create a new PreProcessing instance from a string.
     
     @param id: the name of the PreProcessing class you want to dynamically create
     @return a pointer to the new PreProcessing instance that was created
     */
    static PreProcessing* create(const std::string &id);
    
    /**
     This static function will dynamically create a new PreProcessing instance based on the type of this instance
    */
    PreProcessing* create() const;

    GRT_DEPRECATED_MSG( "createNewInstance is deprecated, use create() instead.", PreProcessing* createNewInstance() const );
    GRT_DEPRECATED_MSG( "createInstanceFromString(id) is deprecated, use create(id) instead.", static PreProcessing* createInstanceFromString( const std::string &id ) );
    GRT_DEPRECATED_MSG( "getPreProcessingType is deprecated, use getId() instead", std::string getPreProcessingType() const );
    
protected:
    /**
     Initializes the base preprocessing module, this will resize the processedData vector and get the instance ready for preprocessing new data.
     The inheriting class must first initialize the module before calling this function.
     
     @return returns true if the module was initialized, false otherwise
     */
    bool init();
    
    /**
     Saves the core preprocessing settings to a file.
     
     @return returns true if the base settings were saved, false otherwise
     */
    bool savePreProcessingSettingsToFile(std::fstream &file) const;
    
    /**
     Loads the core preprocessing settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadPreProcessingSettingsFromFile(std::fstream &file);

    std::string preProcessingType;
    bool initialized;
    VectorFloat processedData;

	static StringPreProcessingMap *getMap() {
        if( !stringPreProcessingMap ){ stringPreProcessingMap = new StringPreProcessingMap; } 
        return stringPreProcessingMap; 
    }

private:
    static StringPreProcessingMap *stringPreProcessingMap;
    static UINT numPreProcessingInstances;
};

template< typename T >  
PreProcessing *createNewPreProcessingModule() { return new T; } ///<Returns a pointer to a new instance of the template class, the caller is responsible for deleting the pointer

/**
 @brief This class provides an interface for classes to register themselves with the preprocessing base class, this enables PreProcessing algorithms to
 be automatically be created from just a string, e.g.: PreProcessing *lpf = create( "LowPassFilter" );
*/
template< typename T > 
class RegisterPreProcessingModule : PreProcessing { 
public:
    RegisterPreProcessingModule( const std::string &newModuleId ) { 
        getMap()->insert( std::pair< std::string, PreProcessing*(*)() >(newModuleId, &createNewPreProcessingModule< T > ) );
    }
};

GRT_END_NAMESPACE

#endif // GRT_POST_PROCESSING_HEADER


