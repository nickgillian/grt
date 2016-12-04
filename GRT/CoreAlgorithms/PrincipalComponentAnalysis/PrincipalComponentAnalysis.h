/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0
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

#ifndef GRT_PRINCIPAL_COMPONENT_ANALYSIS_HEADER
#define GRT_PRINCIPAL_COMPONENT_ANALYSIS_HEADER

#include "../../Util/GRTCommon.h"
#include "../../CoreModules/MLBase.h"

GRT_BEGIN_NAMESPACE

/**
@brief This class runs the Principal Component Analysis (PCA) algorithm, a dimensionality reduction algorithm that
projects an [M N] matrix (where M==samples and N==dimensions) onto a new K dimensional subspace, where K is
normally much less than N.

This projection or transformation is defined in such a way that the first principal component has the largest
possible variance (that is, accounts for as much of the variability in the data as possible), and each succeeding
component has the highest variance possible under the constraint that it be orthogonal to (i.e., uncorrelated with)
the preceding components. Principal components are guaranteed to be independent only if the data set is jointly
normally distributed. PCA is sensitive to the relative scaling of the original variables.

The PCA algorithm will automatically mean subtract the input data, and also normalize the data if required.  To use
this algorithm, the user should first run the computeFeatureVector(...) function to build the PCA feature vector and
then run the project(...) function to project new data onto the new principal subspace.

@remark This implementation is based on Bishop, Christopher M. Pattern recognition and machine learning. Vol. 1. New York: springer, 2006.
*/
class GRT_API PrincipalComponentAnalysis : public MLBase{
public:
    /**
    Default constructor.
    */
    PrincipalComponentAnalysis();
    
    /**
    Default destructor.
    */
    virtual ~PrincipalComponentAnalysis();
    
    /**
    Runs the principal component analysis algorithm on the input data and builds the resulting feature vector
    so new data can be projected onto the principal subspace (using the project function).  The number of principal
    components is automatically computed by selecting the minimum number of components that reach the maxVariance
    value. This should be a value between [0 1], the default value of 0.95 represents 95% of the variance in the
    original dataset.
    
    @param data: a matrix containing the data from which the principal components will be computed. This should be an [M N] matrix, where M==samples and N==dimensions.
    @param maxVariance: sets the variance that should represented by the top K principal components. This should be a value between [0 1]. Default value=0.95
    @param normData: sets if the data will be z-normalized before running the PCA algorithm. Default value=false
    @return returns true if the principal components of the input matrix could be computed, false otherwise
    value
    */
    bool computeFeatureVector(const MatrixFloat &data,Float maxVariance=0.95,bool normData=false);
    
    /**
    Runs the principal component analysis algorithm on the input data and builds the resulting feature vector
    so new data can be projected onto the principal subspace (using the project function).  The number of principal
    components should be set be the user and must be less than or equal to the number of dimensions in the input data.
    
    @param data: a matrix containing the data from which the principal components will be computed. This should be an [M N] matrix, where M==samples and N==dimensions
    @param numPrincipalComponents: sets the number of principal components. This must be a value be less than or equal to the number of dimensions in the input data
    @param normData: sets if the data will be z-normalized before running the PCA algorithm. Default value=false
    @return returns true if the principal components of the input matrix could be computed, false otherwise
    value
    */
    bool computeFeatureVector(const MatrixFloat &data,UINT numPrincipalComponents,bool normData=false);
    
    /**
    Projects the input data matrix onto the principal subspace. The new projected data will be stored in the prjData
    matrix. The computeFeatureVector function should have been called at least once before this function is called.
    The number of the columns in the data matrix must match the numInputDimensions parameter.  The function will return true if the projection was successful, false otherwise.
    
    @param data: The data that should be projected onto the principal subspace. This should be an [M N] matrix, where N must equal the numInputDimensions value (there are no restrictions on M).
    @param prjData: A matrix into which the projected data will be stored. This matrix will be resized to [M K], where M is the number of rows in the data matrix and K is the numPrincipalComponents.
    @return returns true if the projection was successful, false otherwise
    */
    bool project(const MatrixFloat &data,MatrixFloat &prjData);
    
    /**
    Projects the input data vector onto the principal subspace. The new projected data will be stored in the prjData vector.
    The computeFeatureVector function should have been called at least once before this function is called.
    The size of the data vector must match the numInputDimensions parameter.  The function will return true if the projection was successful, false otherwise.
    
    @param data: The data that should be projected onto the principal subspace. This should be an N-dimensional vector, where N must equal the numInputDimensions value.
    @param prjData: A vector into which the projected data will be stored. This vector will be resized to K, where K is the numPrincipalComponents.
    @return returns true if the projection was successful, false otherwise
    */
    bool project(const VectorFloat &data,VectorFloat &prjData);
    
    /**
    This saves the trained PCA model to a file.
    
    @param file: a reference to the file the model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained PCA model from a file.
    
    @param file: a reference to the file the model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    /**
    Returns true if z-normalization is being applied to new data.
    @return returns true if the normData is true, false otherwise
    */
    bool getNormData() const { return normData; }
    
    /**
    Returns the number of input dimensions in the original input data.
    @return returns the numInputDimensions parameter.
    */
    UINT getNumInputDimensions() const { return numInputDimensions; }
    
    /**
    Returns the number of principal components that was required to reach the maxVariance parameter.
    @return returns the number of principal components that was required to reach the maxVariance parameter
    */
    UINT getNumPrincipalComponents() const { return numPrincipalComponents; }
    
    /**
    Returns the maxVariance parameter, set by the user when the computeFeatureVector was called.
    returns the maxVariance parameter, set by the user when the computeFeatureVector was called
    */
    Float getMaxVariance() const { return maxVariance; }
    
    /**
    Returns the mean shift vector, computed during the computeFeatureVector function. New data will be subtracted
    by this value before it is projected onto the principal subspace.
    @return returns the mean shift vector, computed during the computeFeatureVector function
    */
    VectorFloat getMeanVector() const { return mean; }
    
    /**
    Returns the standard deviation vector that is used to normalize new data, this is computed during the
    computeFeatureVector function. This is only used id the normData parameter is true. If true, new data will
    be z-normalized by this value before it is projected onto the principal subspace.
    @return returns the stdDev vector, computed during the computeFeatureVector function
    */
    VectorFloat getStdDevVector() const { return stdDev; }
    
    /**
    Returns the weights for each principal component, these weights sum to 1.
    @return returns a vector of the weights for each principal component, these weights sum to 1
    */
    VectorFloat getComponentWeights() const { return componentWeights; }
    
    /**
    Returns the raw eigen values (these are not sorted).
    @return returns a vector of the raw eigen values
    */
    VectorFloat getEigenValues() const { return eigenvalues; }
    
    /**
    A helper function that prints the PCA info. If the user sets the title string, then this will be written in
    addition with the PCA data.
    */
    virtual bool print( std::string title="" ) const;
    
    /**
    Returns a matrix containing the eigen vectors.
    @return returns a matrix containing the raw eigen vectors
    */
    MatrixFloat getEigenVectors() const;
    
    bool setModel( const VectorFloat &mean, const MatrixFloat &eigenvectors );

    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::print;
    
protected:
    bool computeFeatureVector_(const MatrixFloat &data,UINT analysisMode);
    
    bool normData;
    UINT numPrincipalComponents;
    Float maxVariance;
    VectorFloat mean;
    VectorFloat stdDev;
    VectorFloat componentWeights;
    VectorFloat eigenvalues;
    Vector< IndexedDouble > sortedEigenvalues;
    MatrixFloat eigenvectors;
    
    enum AnalysisMode{MAX_VARIANCE=0,MAX_NUM_PCS};
};

GRT_END_NAMESPACE

#endif //GRT_PRINCIPAL_COMPONENT_ANALYSIS_HEADER
