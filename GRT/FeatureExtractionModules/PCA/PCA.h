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

#ifndef GRT_PCA_HEADER
#define GRT_PCA_HEADER

//Include the main GRT header to get access to the FeatureExtraction base class
#include "../../CoreModules/FeatureExtraction.h"
#include "../../CoreAlgorithms/PrincipalComponentAnalysis/PrincipalComponentAnalysis.h"

GRT_BEGIN_NAMESPACE

class GRT_API PCA : public FeatureExtraction{
public:
    /**
    Default constructor. Initalizes the PCA, setting the number of input dimensions and the number of clusters to use in the quantization model.
    
    @param numDimensions: the number of dimensions in the input data, zero means the module needs to be initialized later via the init function
    @param numPrincipalComponents: the number of dimensions the input data will be projected to, zero means the module needs to be initialized later via the init function
    */
    PCA(const UINT numDimensions = 0, const UINT numPrincipalComponents = 0);
    
    /**
    Copy constructor, copies the PCA module from the rhs instance to this instance.
    
    @param rhs: another instance of this class from which the data will be copied to this instance
    */
    PCA(const PCA &rhs);
    
    /**
    Default Destructor
    */
    virtual ~PCA();
    
    /**
    Sets the equals operator, copies the data from the rhs instance to this instance.
    
    @param rhs: another instance of this class from which the data will be copied to this instance
    @return a reference to this instance
    */
    PCA& operator=(const PCA &rhs);
    
    /**
    Sets the FeatureExtraction deepCopyFrom function, overwriting the base FeatureExtraction function.
    This function is used to deep copy the values from the input pointer to this instance of the FeatureExtraction module.
    This function is called by the GestureRecognitionPipeline when the user adds a new FeatureExtraction module to the pipeleine.
    
    @param featureExtraction: a pointer to another instance of this class, the values of that instance will be cloned to this instance
    @return returns true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const FeatureExtraction *featureExtraction) override;
    
    /**
    Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This is where you should add your main feature extraction code.
    
    @param inputVector: the inputVector that should be processed.  Must have the same dimensionality as the FeatureExtraction module
    @return returns true if the data was processed, false otherwise
    */
    virtual bool computeFeatures(const VectorFloat &inputVector) override;

    /**
    Sets the FeatureExtraction computeFeatures function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when any new input data needs to be processed (during the prediction phase for example).
    This is where you should add your main feature extraction code.
    
    @param inputMatrix: the inputMatrix that should be processed.  Must have the same number of columns as the expected input dimensions
    @return returns true if the data was processed, false otherwise
    */
    virtual bool computeFeatures(const MatrixFloat &inputMatrix) override;
    
    /**
    Sets the FeatureExtraction reset function, overwriting the base FeatureExtraction function.
    This function is called by the GestureRecognitionPipeline when the pipelines main reset() function is called.
    You should add any custom reset code to this function to define how your feature extraction module should be reset.
    
    @return true if the instance was reset, false otherwise
    */
    virtual bool reset() override;
    
    /**
    This saves the feature extraction settings to a file.
    This overrides the save function in the FeatureExtraction base class.
    You should add your own custom code to this function to define how your feature extraction module is saved to a file.
    
    @param file: a reference to the file to save the settings to
    @return returns true if the settings were saved successfully, false otherwise
    */
    virtual bool save(std::fstream &file) const override;
    
    /**
    This loads the feature extraction settings from a file.
    This overrides the load function in the FeatureExtraction base class.
    
    @param file: a reference to the file to load the settings from
    @return returns true if the settings were loaded successfully, false otherwise
    */
    virtual bool load(std::fstream &file) override;

    /**
     This is the main training interface for referenced MatrixFloat data. It overrides the train_ function in the ML base class.
     
     @param data: a reference to the training data that will be used to train the PCA model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(MatrixFloat &data) override;

     /**
     This is a helper interface for training a PCA model. It overrides the train_ function in the ML base class.
     This function converts the data to MatrixFloat and calls train_(MatrixFloat &data).
     
     @param data: a reference to the training data that will be used to train the PCA model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(ClassificationData &data) override;

    bool init(const UINT numDimensions, const UINT numPrincipalComponents);

    PrincipalComponentAnalysis* getPCA();

    /**
    Gets a string that represents the PCA class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();

    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    
protected:
    PrincipalComponentAnalysis pca;
    
private:
    static RegisterFeatureExtractionModule< PCA > registerModule;
    static std::string id;
};

GRT_END_NAMESPACE

#endif //HEADER GUARD
    