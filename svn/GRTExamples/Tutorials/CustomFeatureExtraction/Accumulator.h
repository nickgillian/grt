/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class provides a simple working example of some custom feature extraction.
 
 The class implements an Accumulator feature extraction, this is a very simple feature extraction 
 (which is probably not very useful but is good for demonstrating how to create your own feature extraction)
 that takes the input, sums up the values, and then adds the last value (which is first multipled by a weight).
 */

/*
 @section LICENSE
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
#include "GRT.h"

namespace GRT{
    
class Accumulator : public FeatureExtraction{
public:
    /**
     Default Constructor.
     
     @param UINT numDimensions: the number of dimensions in the input data, must be greater than 0. Default value = 1
     @param double lastValueWeight: the weight assigned to the last input value. Default value = 0.9
     */
    Accumulator(UINT numDimensions=1,double lastValueWeight=0.9);
	
    /**
     Copy constructor, copies the Accumulator from the rhs instance to this instance.
     
     @param const Accumulator &rhs: another instance of this class from which the data will be copied to this instance
     */
    Accumulator(const Accumulator &rhs);
    
    /**
     Default Destructor
     */
    virtual ~Accumulator();
    
    /**
     Sets the equals operator, copies the data from the rhs instance to this instance.
     
     @param const Accumulator &rhs: another instance of this class from which the data will be copied to this instance
     @return a reference to this instance
     */
    Accumulator& operator=(const Accumulator &rhs);

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
     
     @param const VectorDouble &inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
     @return returns true if the data was processed, false otherwise
     */
    virtual bool computeFeatures(const VectorDouble &inputVector);
    
    /**
     Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     
     @return true if the instance was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This saves the feature extraction settings to a file.
     
     @param const string filename: the filename to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveSettingsToFile(const string filename) const;
    
    /**
     This saves the feature extraction settings to a file.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool loadSettingsFromFile(const string filename);
    
    /**
     This saves the feature extraction settings to a file.
     This overrides the saveSettingsToFile function in the FeatureExtraction base class.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveSettingsToFile(fstream &file) const;
    
    /**
     This loads the feature extraction settings from a file.
     This overrides the loadSettingsFromFile function in the FeatureExtraction base class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadSettingsFromFile(fstream &file);

    ///////////////////// Here are the custom methods for the accumulator /////////////////////
    
    /**
     This initilizes the accumulator instance.
     
     @param UINT numDimensions: the number of dimensions in the input data, must be greater than 0
     @param double lastValueWeight: the weight assigned to the last input value
     @return returns true if the instance was initilized successfully, false otherwise
     */
    bool init(UINT numDimensions,double lastValueWeight);
    
    /**
     Gets the last weight value. The lastWeightValue is the weight assigned to the last input.
     
     @return returns the lastValueWeight if the instance is initilized, 0 otherwise
     */
    double getLastValueWeight();
    
    /**
     Gets the last value. The lastValue is the (weighted) last value of the accumulator instance.
     
     @return returns the lastValue if the instance is initilized, 0 otherwise
     */
    double getLastValue();
    
    /**
     Sets the lastValueWeight. The lastWeightValue is the weight assigned to the last input.
     
     @return returns true if the lastValueWeight was updated successfully, false otherwise
     */
    bool setLastValueWeight(double lastValueWeight);
    
    /**
     Sets the lastValue. The lastValue is the value added to the current input.
     
     @return returns true if the lastValue was updated successfully, false otherwise
     */
    bool setLastValue(double lastValue);

protected:
    double lastValueWeight;
    double lastValue;
    
    static RegisterFeatureExtractionModule< Accumulator > registerModule; //This is used to register the Accumulator module with the FeatureExtraction base class
};

}//End of namespace GRT