/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief This class implements the MovementIndex feature module.  The MovementIndex module computes the amount of
movement or variation within an N-dimensional signal over a given time window.  The MovementIndex class
is good for extracting features that describe how much change is occuring in an N-dimensional signal over
time.  An example application might be to use the MovementIndex in combination with one of the GRT classification
algorithms to determine if an object is being moved or held still.
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

#ifndef GRT_MOVEMENT_INDEX_HEADER
#define GRT_MOVEMENT_INDEX_HEADER

#include "../../CoreModules/FeatureExtraction.h"
#include "../../Util/Util.h"

GRT_BEGIN_NAMESPACE

class GRT_API MovementIndex : public FeatureExtraction{
public:
    /**
    Default Constructor. Sets the buffer length and the number of input dimensions.
    
    @param bufferLength: the size of the buffer that will hold the last N samples used to compute the movement index
    @param numDimensions: sets the number of dimensions
    */
    MovementIndex(const UINT bufferLength=100,const UINT numDimensions = 1);
    
    /**
    Copy constructor, copies the MovementIndex from the rhs instance to this instance.
    
    @param rhs: another instance of the MovementIndex class from which the data will be copied to this instance
    */
    MovementIndex(const MovementIndex &rhs);
    
    /**
    Default Destructor
    */
    virtual ~MovementIndex();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance.
    
    @param rhs: another instance of the MovementIndex class from which the data will be copied to this instance
    @return a reference to this instance of MovementIndex
    */
    MovementIndex& operator=(const MovementIndex &rhs);
    
    /**
    Sets the FeatureExtraction deepCopyFrom function, overwriting the base FeatureExtraction function.
    This function is used to deep copy the values from the input pointer to this instance of the FeatureExtraction module.
    This function is called by the GestureRecognitionPipeline when the user adds a new FeatureExtraction module to the pipeleine.
    
    @param featureExtraction: a pointer to another instance of a MovementIndex, the values of that instance will be cloned to this instance
    @return returns true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const FeatureExtraction *featureExtraction);
    
    /**
    Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This function calls the MovementIndex's update function.
    
    @param inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
    @return returns true if the data was processed, false otherwise
    */
    virtual bool computeFeatures(const VectorFloat &inputVector);
    
    /**
    Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
    This function resets the feature extraction by re-initiliazing the instance.
    
    @return true if the filter was reset, false otherwise
    */
    virtual bool reset();
    
    /**
    This saves the feature extraction settings to a file.
    This overrides the save function in the FeatureExtraction base class.
    
    @param file: a reference to the file to save the settings to
    @return returns true if the settings were saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads the feature extraction settings from a file.
    This overrides the load function in the FeatureExtraction base class.
    
    @param file: a reference to the file to load the settings from
    @return returns true if the settings were loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    Initializes the MovementIndex.  This sets the bufferLength and the number of input dimensions.
    
    @param bufferLength: the size of the buffer that will hold the last N samples used to compute the movement index
    @param numDimensions: sets the number of dimensions
    @return returns true if the module was initialized
    */
    bool init(const UINT bufferLength,const UINT numDimensions);
    
    /**
    Computes the features from the input, this should only be called if the dimensionality of this instance was set to 1.
    
    @param x: the value to compute features from, this should only be called if the dimensionality of the filter was set to 1
    @return a vector containing the features, an empty vector will be returned if the features were not computed
    */
    VectorFloat update(const Float x);
    
    /**
    Computes the features from the input, the dimensionality of x should match that of this instance.
    
    @param x: a vector containing the values to be processed, must be the same size as the numInputDimensions
    @return a vector containing the features, an empty vector will be returned if the features were not computed
    */
    VectorFloat update(const VectorFloat &x);
    
    /**
    Gets the current values in the data buffer.
    An empty circular buffer will be returned if the feature extraction module has not been initialized.
    
    @return returns a curcular buffer containing the data buffer values, an empty circular buffer will be returned if the feature extraction module has not been initialized
    */
    CircularBuffer< VectorFloat > getData() const;
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;

    /**
    Gets a string that represents the MovementIndex class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
protected:
    UINT bufferLength;
    CircularBuffer< VectorFloat > dataBuffer;
    
private:
    static RegisterFeatureExtractionModule< MovementIndex > registerModule;
    static std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_MOVEMENT_INDEX_HEADER
    