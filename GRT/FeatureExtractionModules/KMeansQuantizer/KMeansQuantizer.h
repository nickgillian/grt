/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief The KMeansQuantizer module quantizes the N-dimensional input vector to a 1-dimensional discrete value.
This value will be between [0 K-1], where K is the number of clusters used to create the quantization model.
Before you use the KMeansQuantizer, you need to train a quantization model. To do this, you select the number
of clusters you want your quantizer to have and then give it any training data in the following formats:
- ClassificationData
- TimeSeriesClassificationData
- TimeSeriesClassificationDataStream
- UnlabelledClassificationData
- MatrixFloat
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

#ifndef GRT_KMEANS_QUANTIZER_HEADER
#define GRT_KMEANS_QUANTIZER_HEADER

//Include the main GRT header to get access to the FeatureExtraction base class
#include "../../CoreModules/FeatureExtraction.h"
#include "../../ClusteringModules/KMeans/KMeans.h"
#include "../../DataStructures/ClassificationDataStream.h"
#include "../../DataStructures/TimeSeriesClassificationData.h"
#include "../../DataStructures/UnlabelledData.h"

GRT_BEGIN_NAMESPACE

class GRT_API KMeansQuantizer : public FeatureExtraction{
public:
    /**
    Default constructor. Initalizes the KMeansQuantizer, setting the number of input dimensions and the number of clusters to use in the quantization model.
    
    @param numClusters: the number of quantization clusters
    */
    KMeansQuantizer(const UINT numClusters=10);
    
    /**
    Copy constructor, copies the KMeansQuantizer from the rhs instance to this instance.
    
    @param rhs: another instance of this class from which the data will be copied to this instance
    */
    KMeansQuantizer(const KMeansQuantizer &rhs);
    
    /**
    Default Destructor
    */
    virtual ~KMeansQuantizer();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance.
    
    @param rhs: another instance of this class from which the data will be copied to this instance
    @return a reference to this instance
    */
    KMeansQuantizer& operator=(const KMeansQuantizer &rhs);
    
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
    
    @param inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
    @return returns true if the data was processed, false otherwise
    */
    virtual bool computeFeatures(const VectorFloat &inputVector);
    
    /**
    Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
    
    @return true if the instance was reset, false otherwise
    */
    virtual bool reset();
    
    /**
    Sets the FeatureExtraction clear function, overwriting the base FeatureExtraction function.
    
    @return true if the instance was reset, false otherwise
    */
    virtual bool clear();
    
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
    Trains the quantization model using the training dataset.
    
    @param trainingData: the training dataset that will be used to train the quantizer
    @return returns true if the quantizer was trained successfully, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
    Trains the quantization model using the training dataset.
    
    @param trainingData: the training dataset that will be used to train the quantizer
    @return returns true if the quantizer was trained successfully, false otherwise
    */
    virtual bool train_(TimeSeriesClassificationData &trainingData);
    
    /**
    Trains the quantization model using the training dataset.
    
    @param trainingData: the training dataset that will be used to train the quantizer
    @return returns true if the quantizer was trained successfully, false otherwise
    */
    virtual bool train_(ClassificationDataStream &trainingData);
    
    /**
    Trains the quantization model using the training dataset.
    
    @param trainingData: the training dataset that will be used to train the quantizer
    @return returns true if the quantizer was trained successfully, false otherwise
    */
    virtual bool train_(UnlabelledData &trainingData);
    
    /**
    Trains the quantization model using the training dataset.
    
    @param trainingData: the training dataset that will be used to train the quantizer
    @return returns true if the quantizer was trained successfully, false otherwise
    */
    virtual bool train_(MatrixFloat &trainingData);
    
    /**
    Quantizes the input value using the quantization model. The quantization model must be trained first before you call this function.
    
    @param inputValue: the value you want to quantize
    @return returns the quantized value
    */
    UINT quantize(const Float inputValue);
    
    /**
    Quantizes the input value using the quantization model. The quantization model must be trained first before you call this function.
    
    @param inputVector: the vector you want to quantize
    @return returns the quantized value
    */
    UINT quantize(const VectorFloat &inputVector);
    
    /**
    Gets if the quantization model has been trained.
    
    @return returns true if the quantization model has been trained, false otherwise
    */
    bool getQuantizerTrained() const { return trained; }
    
    /**
    Gets the number of clusters in the quantizer.
    
    @return returns the numbers of clusters in the quantizer.
    */
    UINT getNumClusters() const;
    
    /**
    Gets the most recent quantized value. This can also be accessed by using the first element in the featureVector.
    
    @return returns the most recent quantized value
    */
    UINT getQuantizedValue() const { return (trained ? (UINT)featureVector[0] : 0); }
    
    /**
    Gets the quantization distances from the most recent quantization.
    
    @return returns a VectorFloat containing the quantization distances from the most recent quantization
    */
    VectorFloat getQuantizationDistances() const{
        return quantizationDistances;
    }
    
    /**
    Gets the quantization model. This will be a [K N] matrix containing the quantization clusters, where K is the
    number of clusters and N is the number of dimensions in the input data.
    
    @return returns a MatrixFloat containing the quantization model
    */
    MatrixFloat getQuantizationModel() const{
        return clusters;
    }
    
    /**
    Sets the number of clusters in the quantizer.  This will clear any previously trained model.
    
    @return returns true if the number of clusters was updated, false otherwise
    */
    bool setNumClusters(const UINT numClusters);
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_;
    using MLBase::predict_;

    /**
    Gets a string that represents the KMeansQuantizer class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
protected:
    UINT numClusters;
    MatrixFloat clusters;
    VectorFloat quantizationDistances;
    
private:
    static RegisterFeatureExtractionModule< KMeansQuantizer > registerModule;
    static std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_KMEANS_QUANTIZER_HEADER
    