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

#ifndef GRT_KMEANS_FEATURES_HEADER
#define GRT_KMEANS_FEATURES_HEADER

//Include the main GRT header to get access to the FeatureExtraction base class
#include "../../CoreModules/FeatureExtraction.h"
#include "../../ClusteringModules/KMeans/KMeans.h"
#include "../../DataStructures/TimeSeriesClassificationData.h"
#include "../../DataStructures/TimeSeriesClassificationDataStream.h"
#include "../../DataStructures/UnlabelledData.h"

namespace GRT{
    
class KMeansFeatures : public FeatureExtraction{
public:
    /**
     Default constructor. Initalizes the KMeansFeatures, setting the number of input dimensions and the number of clusters to use in the quantization model.
	
	@param UINT numDimensions: the number of dimensions in the input data
    @param UINT numClusters: the number of quantization clusters
     */
    KMeansFeatures(const vector< UINT > numClustersPerLayer = vector< UINT >(1,100),const double alpha = 0.2,const bool useScaling = true);
	
    /**
     Copy constructor, copies the KMeansQuantizer from the rhs instance to this instance.
     
     @param const KMeansFeatures &rhs: another instance of this class from which the data will be copied to this instance
     */
    KMeansFeatures(const KMeansFeatures &rhs);
    
    /**
     Default Destructor
     */
    virtual ~KMeansFeatures();
    
    /**
     Sets the equals operator, copies the data from the rhs instance to this instance.
     
     @param const KMeansQuantizer &rhs: another instance of this class from which the data will be copied to this instance
     @return a reference to this instance
     */
    KMeansFeatures& operator=(const KMeansFeatures &rhs);

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
     
     @param const VectorDouble &inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
     @return returns true if the data was processed, false otherwise
     */
    virtual bool computeFeatures(const VectorDouble &inputVector);
    
    /**
     Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
     This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
     You should add any custom reset code to this function to define how your feature extraction module should be reset.
     
     @return true if the instance was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This saves the feature extraction settings to a file.
     
     @param const string filename: the filename to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(const string filename) const;
    
    /**
     This saves the feature extraction settings to a file.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool loadModelFromFile(const string filename);
    
    /**
     This saves the feature extraction settings to a file.
     This overrides the saveSettingsToFile function in the FeatureExtraction base class.
     You should add your own custom code to this function to define how your feature extraction module is saved to a file.
     
     @param fstream &file: a reference to the file to save the settings to
     @return returns true if the settings were saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads the feature extraction settings from a file.
     This overrides the loadSettingsFromFile function in the FeatureExtraction base class.
     
     @param fstream &file: a reference to the file to load the settings from
     @return returns true if the settings were loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);

    /**
     Trains the quantization model using the training dataset.
     
     @param ClassificationData &trainingData: the training dataset that will be used to train the quantizer
     @return returns true if the quantizer was trained successfully, false otherwise
     */
    virtual bool train_(ClassificationData &trainingData);

    /**
     Trains the quantization model using the training dataset.
     
     @param TimeSeriesClassificationData &trainingData: the training dataset that will be used to train the quantizer
     @return returns true if the quantizer was trained successfully, false otherwise
     */
    virtual bool train_(TimeSeriesClassificationData &trainingData);

    /**
     Trains the quantization model using the training dataset.
     
     @param TimeSeriesClassificationDataStream &trainingData: the training dataset that will be used to train the quantizer
     @return returns true if the quantizer was trained successfully, false otherwise
     */
    virtual bool train_(TimeSeriesClassificationDataStream &trainingData);

    /**
     Trains the quantization model using the training dataset.
     
     @param UnlabelledData &trainingData: the training dataset that will be used to train the quantizer
     @return returns true if the quantizer was trained successfully, false otherwise
     */
    virtual bool train_(UnlabelledData &trainingData);

    /**
     Trains the quantization model using the training dataset.
     
     @param MatrixDouble &trainingData: the training dataset that will be used to train the quantizer
     @return returns true if the quantizer was trained successfully, false otherwise
     */
    virtual bool train_(MatrixDouble &trainingData);
    
    bool computeFeatures(VectorDouble &inputVector,VectorDouble &outputVector);
    
    bool init( const vector< UINT > numClustersPerLayer );
    
    bool projectDataThroughLayer( const VectorDouble &input, VectorDouble &output, const UINT layer );
    
    UINT getNumLayers() const;
    UINT getLayerSize(const UINT layerIndex) const;
    vector< MatrixDouble > getClusters() const;
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;
    
protected:
    double alpha;
    vector< UINT > numClustersPerLayer;
    vector< MinMax > ranges;
    vector< MatrixDouble > clusters;
    
    static RegisterFeatureExtractionModule< KMeansFeatures > registerModule;
};

}//End of namespace GRT

#endif //GRT_KMEANS_FEATURES_HEADER
