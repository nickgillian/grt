/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class provides an empty template to help you create your own custom FeatureExtraction module.
 You should rename this class (from FeatureExtractionTemplate to something more suitable for your custom module) and add
 your own custom code to the following methods:
 - Default Constructor, Copy Constructor, Destructor, and equals operator
 - clone
 - computeFeatures
 - reset
 - saveSettingsToFile
 - loadSettingsFromFile
 */

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

#pragma once

//Include the main GRT header to get access to the FeatureExtraction base class
#include <GRT/GRT.h>

GRT_BEGIN_NAMESPACE //This macro places all the following code inside the GRT namespace
    
class FeatureExtractionTemplate : public FeatureExtraction{
public:
    /**
     */
    FeatureExtractionTemplate();
	
    /**
     Copy constructor, copies the FeatureExtractionTemplate from the rhs instance to this instance.
     
     @param const FeatureExtractionTemplate &rhs: another instance of this class from which the data will be copied to this instance
     */
    FeatureExtractionTemplate(const FeatureExtractionTemplate &rhs);
    
    /**
     Default Destructor
     */
    virtual ~FeatureExtractionTemplate();
    
    /**
     Sets the equals operator, copies the data from the rhs instance to this instance.
     
     @param const FeatureExtractionTemplate &rhs: another instance of this class from which the data will be copied to this instance
     @return a reference to this instance
     */
    FeatureExtractionTemplate& operator=(const FeatureExtractionTemplate &rhs);

    /**
     Sets the FeatureExtraction deepCopyFrom function, overwriting the base FeatureExtraction function.
     This function is used to deep copy the values from the input pointer to this instance of the FeatureExtraction module.
     This function is called by the GestureRecognitionPipeline when the user adds a new FeatureExtraction module to the pipeleine.
     
     @param const FeatureExtraction *featureExtraction: a pointer to another instance of this class, the values of that instance will be cloned to this instance
     @return returns true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const FeatureExtraction *featureExtraction);
    
    /**
     Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
     This is where you should add your main feature extraction code.
     
     @param const VectorFloat &inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
     @return returns true if the data was processed, false otherwise
     */
    virtual bool computeFeatures(const VectorFloat &inputVector);
    
    /**
     Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     You should add any custom reset code to this function to define how your feature extraction module should be reset.
     
     @return true if the instance was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This saves the feature extraction settings to a file.
     This overrides the saveSettingsToFile function in the FeatureExtraction base class.
     You should add your own custom code to this function to define how your feature extraction module is saved to a file.
     
     @param file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool save( std::fstream &file ) const;
    
    /**
     This loads the feature extraction settings from a file.
     This overrides the loadSettingsFromFile function in the FeatureExtraction base class.
     
     @param file: a reference to the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool load( std::fstream &file );

    //Tell the compiler we are using the following functions from the base class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;

    /**
    Gets a string that represents the custom class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();

    //You should add any custom public methods here...

protected:
    //Add any custom class variables here that are required for your custom feature extraction module
    
    static const std::string id;
    static RegisterFeatureExtractionModule< FeatureExtractionTemplate > registerModule; //This is used to register your custom feature extraction module with the FeatureExtraction base class
};

GRT_END_NAMESPACE //This macro places all the code above inside the GRT namespace
