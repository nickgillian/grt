/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
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

#ifndef GRT_POST_PROCESSING_HEADER
#define GRT_POST_PROCESSING_HEADER

#include "MLBase.h"

GRT_BEGIN_NAMESPACE

/**
 @brief This is the main base class that all GRT PostProcessing algorithms should inherit from.
 A large number of the functions in this class are virtual and simply return false as these functions must be overwridden by the inheriting class.
*/
class GRT_API PostProcessing : public MLBase
{
public:
    /**
     Default Constructor
     */
	PostProcessing( const std::string &id = "" );
	
    /**
     Default Destructor
     */
	virtual ~PostProcessing(void);
    
    /**
     This is the base deepCopyFrom function for the PostProcessing modules. This function should be overwritten by the derived class.
     
     @param postProcessing: a pointer to the PostProcessing base class, this should be pointing to another instance of a matching derived class
     @return returns true if the deep copy was successfull, false otherwise (the PostProcessing base class will always return flase)
     */
	virtual bool deepCopyFrom(const PostProcessing *postProcessing){ return false; }
    
    /**
     This copies the PostProcessing variables from postProcessingModule to the instance that calls the function.
     
     @param postProcessingModule: a pointer to a post processing module from which the values will be copied
     @return returns true if the copy was successfull, false otherwise
     */
	bool copyBaseVariables(const PostProcessing *postProcessingModule);

    /**
     This is the main processing interface for all the post processing modules and should be overwritten by the inheriting class.
     
     @param inputVector: a vector containing the data that should be processed
     @return returns true if the post processing was successfull, false otherwise
     */
    virtual bool process(const VectorFloat &inputVector){ return false; }
    
    /**
     This function is called by the GestureRecognitionPipeline's reset function and will reset the PostProcessing module.
     This function should be overwritten by the derived class.
     
     @return returns true if the module was reset, false otherwise (the base class always returns true)
     */
    virtual bool reset(){ return true; }
    
    /**
     This saves the post processing settings to a file.
     This function should be overwritten by the derived class.
     
     @param filename: the filename to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(std::string filename) const;
    
    /**
     This saves the post processing settings to a file.
     This function should be overwritten by the derived class.
     
     @param file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool loadModelFromFile(std::string filename);
    
    /**
     This saves the post processing settings to a file.
     This function should be overwritten by the derived class.
     
     @param file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise (the base class always returns false)
     */
    virtual bool saveModelToFile(std::fstream &file) const{ return false; }
    
    /**
     This loads the post processing settings from a file.
     This function should be overwritten by the derived class.
     
     @param file: a reference to the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise (the base class always returns false)
     */
    virtual bool loadModelFromFile(std::fstream &file){ return false; }
    
    /**
     @return returns the post processing input mode, this will be one of the PostprocessingInputModes enums
     */
	UINT getPostProcessingInputMode() const;
    
    /**
     @return returns the post processing output mode, this will be one of the PostprocessingOutputModes enums
     */
	UINT getPostProcessingOutputMode() const;
    
    /**
     Returns the size of the input vector expected by the post processing module.
     
     @return returns the size of the input vector expected by the post processing module
     */
    UINT getNumInputDimensions() const;
    
    /**
     Returns the size of the vector that will be computed by the post processing module.
     
     @return returns the size of the vector that will be computed by the post processing module
     */
    UINT getNumOutputDimensions() const;
    
    /**
     Returns true if the post processing module has been initialized correctly.
     
     @return returns true if the post processing module has been initialized succesfully, false otherwise
     */
    bool getInitialized() const;
    
    /**
     @return returns true if input mode is INPUT_MODE_PREDICTED_CLASS_LABEL, false otherwise
     */
	bool getIsPostProcessingInputModePredictedClassLabel() const;
    
    /**
     @return returns true if input mode is INPUT_MODE_CLASS_LIKELIHOODS, false otherwise
     */
	bool getIsPostProcessingInputModeClassLikelihoods() const;
    
    /**
     @return returns true if input mode is OUTPUT_MODE_PREDICTED_CLASS_LABEL, false otherwise
     */
	bool getIsPostProcessingOutputModePredictedClassLabel() const;
    
    /**
     @return returns true if input mode is OUTPUT_MODE_CLASS_LIKELIHOODS, false otherwise
     */
	bool getIsPostProcessingOutputModeClassLikelihoods() const;
    
    /**
     @return returns a VectorFloat containing the most recent processed data
     */
	VectorFloat getProcessedData() const;
    
    /**
     This typedef defines a map between a string and a PostProcessing pointer.
     */
	typedef std::map< std::string, PostProcessing*(*)() > StringPostProcessingMap;
    
    /**
     This static function will dynamically create a new PostProcessing instance from a string.
     
     @param postProcessingType: the name of the PostProcessing class you want to dynamically create
     @return a pointer to the new PostProcessing instance that was created
     */
    static PostProcessing* create(const std::string &postProcessingType);
    
    /**
     This static function will dynamically create a new PostProcessing instance based on the type of this instance
     */
    PostProcessing* create() const;
    
    enum PostprocessingInputModes{INPUT_MODE_NOT_SET=0,INPUT_MODE_PREDICTED_CLASS_LABEL,INPUT_MODE_CLASS_LIKELIHOODS};
    enum PostprocessingOutputModes{OUTPUT_MODE_NOT_SET=0,OUTPUT_MODE_PREDICTED_CLASS_LABEL,OUTPUT_MODE_CLASS_LIKELIHOODS};

    GRT_DEPRECATED_MSG( "createNewInstance is deprecated, use create instead.", PostProcessing* createNewInstance() const );
    GRT_DEPRECATED_MSG( "createInstanceFromString is deprecated, use create instead.", static PostProcessing* createInstanceFromString( const std::string &id ) );
    GRT_DEPRECATED_MSG( "getPostProcessingType is deprecated, use getId() instead", std::string getPostProcessingType() const );
    
protected:
    /**
     Initializes the base postprocessing module, this will resize the processedData vector and get the instance ready for processing new data.
     The inheriting class must first initialize the module before calling this function.
     
     @return returns true if the module was initialized, false otherwise
     */
    bool init();
    
    /**
     Saves the core postprocessing settings to a file.
     
     @return returns true if the base settings were saved, false otherwise
     */
    bool savePostProcessingSettingsToFile(std::fstream &file) const;
    
    /**
     Loads the core postprocessing settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadPostProcessingSettingsFromFile(std::fstream &file);

    std::string postProcessingType;
    bool initialized;
    UINT postProcessingInputMode;
    UINT postProcessingOutputMode;
    VectorFloat processedData;
    
    static StringPostProcessingMap *getMap() {
        if( !stringPostProcessingMap ){ stringPostProcessingMap = new StringPostProcessingMap; } 
        return stringPostProcessingMap; 
    }

private:
    static StringPostProcessingMap *stringPostProcessingMap;
    static UINT numPostProcessingInstances;
};

template< typename T >  PostProcessing *createNewPostProcessingModule() { return new T; } ///<Returns a pointer to a new instance of the template class, the caller is responsible for deleting the pointer

template< typename T > 
class RegisterPostProcessingModule : PostProcessing { 
public:
    RegisterPostProcessingModule( std::string const &newModuleId ) { 
        getMap()->insert( std::pair< std::string, PostProcessing*(*)() >(newModuleId, &createNewPostProcessingModule< T > ) );
    }
};

GRT_END_NAMESPACE

#endif //GRT_POST_PROCESSING_HEADER

