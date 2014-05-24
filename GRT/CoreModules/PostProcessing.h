/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This is the main base class that all GRT PostProcessing algorithms should inherit from.
 
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

#ifndef GRT_POST_PROCESSING_HEADER
#define GRT_POST_PROCESSING_HEADER

#include "MLBase.h"

namespace GRT{

class PostProcessing : public MLBase
{
public:
    /**
     Default Constructor
     */
	PostProcessing(void);
	
    /**
     Default Destructor
     */
	virtual ~PostProcessing(void);
    
    /**
     This is the base deepCopyFrom function for the PostProcessing modules. This function should be overwritten by the derived class.
     
     @param const PostProcessing *postProcessing: a pointer to the PostProcessing base class, this should be pointing to another instance of a matching derived class
     @return returns true if the deep copy was successfull, false otherwise (the PostProcessing base class will always return flase)
     */
	virtual bool deepCopyFrom(const PostProcessing *postProcessing){ return false; }
    
    /**
     This copies the PostProcessing variables from postProcessingModule to the instance that calls the function.
     
     @param const PostProcessing *postProcessingModule: a pointer to a post processing module from which the values will be copied
     @return returns true if the copy was successfull, false otherwise
     */
	bool copyBaseVariables(const PostProcessing *postProcessingModule);

    /**
     This is the main processing interface for all the post processing modules and should be overwritten by the inheriting class.
     
     @param const VectorDouble &inputVector: a vector containing the data that should be processed
     @return returns true if the post processing was successfull, false otherwise
     */
    virtual bool process(const VectorDouble &inputVector){ return false; }
    
    /**
     This function is called by the GestureRecognitionPipeline's reset function and will reset the PostProcessing module.
     This function should be overwritten by the derived class.
     
     @return returns true if the module was reset, false otherwise (the base class always returns true)
     */
    virtual bool reset(){ return true; }
    
    /**
     This saves the post processing settings to a file.
     This function should be overwritten by the derived class.
     
     @param const string filename: the filename to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(const string filename) const;
    
    /**
     This saves the post processing settings to a file.
     This function should be overwritten by the derived class.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool loadModelFromFile(const string filename);
    
    /**
     This saves the post processing settings to a file.
     This function should be overwritten by the derived class.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise (the base class always returns false)
     */
    virtual bool saveModelToFile(fstream &file) const{ return false; }
    
    /**
     This loads the post processing settings from a file.
     This function should be overwritten by the derived class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise (the base class always returns false)
     */
    virtual bool loadModelFromFile(fstream &file){ return false; }
	
    /**
     @return returns the post processing type as a string, e.g. ClassLabelTimeoutFilter
     */
	string getPostProcessingType() const;
    
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
     @return returns a VectorDouble containing the most recent processed data
     */
	VectorDouble getProcessedData() const;
    
    /**
     This typedef defines a map between a string and a PostProcessing pointer.
     */
	typedef std::map< string, PostProcessing*(*)() > StringPostProcessingMap;
    
    /**
     This static function will dynamically create a new PostProcessing instance from a string.
     
     @param string const &postProcessingType: the name of the PostProcessing class you want to dynamically create
     @return a pointer to the new PostProcessing instance that was created
     */
    static PostProcessing* createInstanceFromString(string const &postProcessingType);
    
    /**
     This static function will dynamically create a new PostProcessing instance based on the type of this instance
     */
    PostProcessing* createNewInstance() const;
    
    enum PostprocessingInputModes{INPUT_MODE_NOT_SET=0,INPUT_MODE_PREDICTED_CLASS_LABEL,INPUT_MODE_CLASS_LIKELIHOODS};
    enum PostprocessingOutputModes{OUTPUT_MODE_NOT_SET=0,OUTPUT_MODE_PREDICTED_CLASS_LABEL,OUTPUT_MODE_CLASS_LIKELIHOODS};
    
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
    bool savePostProcessingSettingsToFile(fstream &file) const;
    
    /**
     Loads the core postprocessing settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadPostProcessingSettingsFromFile(fstream &file);

    string postProcessingType;
    bool initialized;
    UINT numInputDimensions;
    UINT numOutputDimensions;
    UINT postProcessingInputMode;
    UINT postProcessingOutputMode;
    VectorDouble processedData;
    
    static StringPostProcessingMap *getMap() {
        if( !stringPostProcessingMap ){ stringPostProcessingMap = new StringPostProcessingMap; } 
        return stringPostProcessingMap; 
    }

private:
    static StringPostProcessingMap *stringPostProcessingMap;
    static UINT numPostProcessingInstances;

};

//These two functions/classes are used to register any new PostProcessing Module with the PostProcessing base class
template< typename T >  PostProcessing *newPostProcessingModuleInstance() { return new T; }

template< typename T > 
class RegisterPostProcessingModule : PostProcessing { 
public:
    RegisterPostProcessingModule(string const &newPostProcessingModuleName) { 
        getMap()->insert( std::pair<string, PostProcessing*(*)()>(newPostProcessingModuleName, &newPostProcessingModuleInstance< T > ) );
    }
};

} //End of namespace GRT

#endif //GRT_POST_PROCESSING_HEADER

