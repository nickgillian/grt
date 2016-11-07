/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief This class implements the TimeDomainFeatures feature extraction module.

This class extracts basic time domain features: mean, standard deviation, Euclidean norm, and RMS.

It computes these features over one or more frames of a buffer / window.
For instance, if the buffer length is 100 and the number of frames is 5, each feature will be computed five times, once per each frame of 20 samples.
As a result, the buffer length must be evenly divisible by the number of frames.

Features are computed independently on each dimension of input data.
Thus, the total output dimension equals the number of frames times the number of input dimensions times the number of features selected.

Optionally, this class can offset the input data, i.e. subtract the value of the first data point in the buffer from the value of all subsequent data points (prior to computing any features).

The output is ordered by input dimension, then by frame, then by feature, e.g. for three dimensions, two frames, and two features (mean and standard deviation), the output would be (dimension 0, frame 0, mean), (dimension 0, frame 0, std. dev.), (dimension 0, frame 1, mean), (dimension 0, frame 1, std. dev.), (dimension 1, frame 0, mean), (dimension 1, frame 0, std. dev.), (dimension 1, frame 1, mean), (dimension 1, frame 1, std. dev.), (dimension 2, frame 0, mean), (dimension 2, frame 0, std. dev.), (dimension 2, frame 1, mean), (dimension 2, frame 1, std. dev.)
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

#ifndef GRT_TIME_DOMAIN_FEATURES_HEADER
#define GRT_TIME_DOMAIN_FEATURES_HEADER

#include "../../CoreModules/FeatureExtraction.h"
#include "../../Util/Util.h"

GRT_BEGIN_NAMESPACE

class GRT_API TimeDomainFeatures : public FeatureExtraction{
public:
    /**
    */
    TimeDomainFeatures(const UINT bufferLength=100,const UINT numFrames=10,const UINT numDimensions = 1,const bool offsetInput = false,const bool useMean = true,const bool useStdDev = true,const bool useEuclideanNorm = true,const bool useRMS = true);
    
    /**
    Copy constructor, copies the TimeDomainFeatures from the rhs instance to this instance.
    
    @param rhs: another instance of the TimeDomainFeatures class from which the data will be copied to this instance
    */
    TimeDomainFeatures(const TimeDomainFeatures &rhs);
    
    /**
    Default Destructor
    */
    virtual ~TimeDomainFeatures();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance.
    
    @param rhs: another instance of the TimeDomainFeatures class from which the data will be copied to this instance
    @return a reference to this instance of TimeDomainFeatures
    */
    TimeDomainFeatures& operator=(const TimeDomainFeatures &rhs);
    
    /**
    Sets the FeatureExtraction deepCopyFrom function, overwriting the base FeatureExtraction function.
    This function is used to deep copy the values from the input pointer to this instance of the FeatureExtraction module.
    This function is called by the GestureRecognitionPipeline when the user adds a new FeatureExtraction module to the pipeline.
    
    @param featureExtraction: a pointer to another instance of a TimeDomainFeatures, the values of that instance will be cloned to this instance
    @return returns true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const FeatureExtraction *featureExtraction);
    
    /**
    Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This function calls the TimeDomainFeatures's update function.
    
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
    Initializes the MovementTrajectoryFeatures
    */
    bool init(const UINT bufferLength,const UINT numFrames,const UINT numDimensions,const bool offsetInput,const bool useMean,const bool useStdDev,const bool useEuclideanNorm,const bool useRMS);
    
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
    Gets a reference to the circular buffer.
    
    @return a reference to the circular buffer
    */
    const CircularBuffer< VectorFloat > &getBufferData() const;
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;

    /**
    Gets a string that represents the TimeDomainFeatures class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
protected:
    UINT bufferLength;
    UINT numFrames;
    bool offsetInput;
    bool useMean;
    bool useStdDev;
    bool useEuclideanNorm;
    bool useRMS;
    CircularBuffer< VectorFloat > dataBuffer;
    
private:
    static RegisterFeatureExtractionModule< TimeDomainFeatures > registerModule;
    static std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_TIME_DOMAIN_FEATURES_HEADER
    