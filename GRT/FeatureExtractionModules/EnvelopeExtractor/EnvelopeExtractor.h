/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief
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

#ifndef GRT_ENVELOPE_EXTRACTOR_HEADER
#define GRT_ENVELOPE_EXTRACTOR_HEADER

//Include the main GRT header to get access to the FeatureExtraction base class
#include "../../CoreModules/FeatureExtraction.h"

GRT_BEGIN_NAMESPACE

class GRT_API EnvelopeExtractor : public FeatureExtraction{
public:
    /**
    Default constructor. Initalizes the EnvelopeExtractor, setting the number of input dimensions and the number of clusters to use in the quantization model.
    
    @param numDimensions: the number of dimensions in the input data
    @param numClusters: the number of quantization clusters
    */
    EnvelopeExtractor(const UINT bufferSize = 100,const UINT numDimensions = 1);
    
    /**
    Copy constructor, copies the KMeansQuantizer from the rhs instance to this instance.
    
    @param rhs: another instance of this class from which the data will be copied to this instance
    */
    EnvelopeExtractor(const EnvelopeExtractor &rhs);
    
    /**
    Default Destructor
    */
    virtual ~EnvelopeExtractor();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance.
    
    @param rhs: another instance of this class from which the data will be copied to this instance
    @return a reference to this instance
    */
    EnvelopeExtractor& operator=(const EnvelopeExtractor &rhs);
    
    /**
    Sets the FeatureExtraction deepCopyFrom function, overwriting the base FeatureExtraction function.
    This function is used to deep copy the values from the input pointer to this instance of the FeatureExtraction module.
    This function is called by the GestureRecognitionPipeline when the user adds a new FeatureExtraction module to the pipeleine.
    
    @param featureExtraction: a pointer to another instance of this class, the values of that instance will be cloned to this instance
    @return returns true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const FeatureExtraction *featureExtraction);
    
    /**
    Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This is where you should add your main feature extraction code.
    
    @param inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
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
    This overrides the save function in the FeatureExtraction base class.
    You should add your own custom code to this function to define how your feature extraction module is saved to a file.
    
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
    Initialize the envelope extractor.  This will setup the internal buffers required for using the module.
    This function is automatically called by the envelope extractor constructor.
    
    @param bufferSize: the size of the internal buffer used to compute the envelope, a larger buffer size will result in a smoother envelope
    @param numDimensions: the expected number of dimensions in the input signal
    @return returns true if the module was initialized successfully, false otherwise
    */
    bool init( const UINT bufferSize = 100,const UINT numDimensions = 1 );
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;

    /**
    Gets a string that represents the EnvelopeExtractor class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
protected:
    UINT bufferSize;
    CircularBuffer< VectorFloat > buffer;
    
private:
    static RegisterFeatureExtractionModule< EnvelopeExtractor > registerModule;
    static std::string id;
};

GRT_END_NAMESPACE

#endif //HEADER GUARD
    