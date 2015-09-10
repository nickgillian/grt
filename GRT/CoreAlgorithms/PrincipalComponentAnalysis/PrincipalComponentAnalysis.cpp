/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
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

#include "PrincipalComponentAnalysis.h"

namespace GRT{

PrincipalComponentAnalysis::PrincipalComponentAnalysis(){
    trained = false;
    normData = false;
	numInputDimensions = 0;
	numPrincipalComponents = 0;
    maxVariance = 0;
    
    classType = "PrincipalComponentAnalysis";
    errorLog.setProceedingText("[ERROR PrincipalComponentAnalysis]");
    warningLog.setProceedingText("[WARNING PrincipalComponentAnalysis]");
}

PrincipalComponentAnalysis::~PrincipalComponentAnalysis(){
	
}

bool PrincipalComponentAnalysis::computeFeatureVector(const MatrixDouble &data,double maxVariance,bool normData){
    trained = false;
    this->maxVariance = maxVariance;
    this->normData = normData;
    return computeFeatureVector_(data,MAX_VARIANCE);
}

bool PrincipalComponentAnalysis::computeFeatureVector(const MatrixDouble &data,UINT numPrincipalComponents,bool normData){
    trained = false;
    if( numPrincipalComponents > data.getNumCols() ){
        errorLog << "computeFeatureVector(const MatrixDouble &data,UINT numPrincipalComponents,bool normData) - The number of principal components (";
        errorLog << numPrincipalComponents << ") is greater than the number of columns in your data (" << data.getNumCols() << ")" << endl;
        return false;
    }
    this->numPrincipalComponents = numPrincipalComponents;
    this->normData = normData;
    return computeFeatureVector_(data,MAX_NUM_PCS);
}

bool PrincipalComponentAnalysis::computeFeatureVector_(const MatrixDouble &data,const UINT analysisMode){

    trained = false;
    const UINT M = data.getNumRows();
    const UINT N = data.getNumCols();
    this->numInputDimensions = N;

    MatrixDouble msData( M, N );

    //Compute the mean and standard deviation of the input data
    mean = data.getMean();
    stdDev = data.getStdDev();

    if( normData ){
        //Normalize the data
        for(UINT i=0; i<M; i++)
            for(UINT j=0; j<N; j++)
                msData[i][j] = (data[i][j]-mean[j]) / stdDev[j];

    }else{
        //Mean Subtract Data
        for(UINT i=0; i<M; i++)
            for(UINT j=0; j<N; j++)
                msData[i][j] = data[i][j] - mean[j];
    }

    //Get the covariance matrix
    MatrixDouble cov = msData.getCovarianceMatrix();

    //Use Eigen Value Decomposition to find eigenvectors of the covariance matrix
    EigenvalueDecomposition eig;

    if( !eig.decompose( cov ) ){
        mean.clear();
        stdDev.clear();
        componentWeights.clear();
        sortedEigenvalues.clear();
        eigenvectors.clear();
        errorLog << "computeFeatureVector(const MatrixDouble &data,UINT analysisMode) - Failed to decompose input matrix!" << endl;
        return false;
    }

    //Get the eigenvectors and eigenvalues
    eigenvectors = eig.getEigenvectors();
    eigenvalues = eig.getRealEigenvalues();

    //Any eigenvalues less than 0 are not worth anything so set to 0
    for(UINT i=0; i<eigenvalues.size(); i++){
        if( eigenvalues[i] < 0 )
            eigenvalues[i] = 0;
    }

    //Sort the eigenvalues and compute the component weights
    double sum = 0;
    UINT componentIndex = 0;
    sortedEigenvalues.clear();
    componentWeights.resize(N,0);

    while( true ){
        double maxValue = 0;
        UINT index = 0;
        for(UINT i=0; i<eigenvalues.size(); i++){
            if( eigenvalues[i] > maxValue ){
                maxValue = eigenvalues[i];
                index = i;
            }
        }
        if( maxValue == 0 || componentIndex >= eigenvalues.size() ){
            break;
        }
        sortedEigenvalues.push_back( IndexedDouble(index,maxValue) );
        componentWeights[ componentIndex++ ] = eigenvalues[ index ];
        sum += eigenvalues[ index ];
        eigenvalues[ index ] = 0; //Set the maxValue to zero so it won't be used again
    }

    double cumulativeVariance = 0;
    switch( analysisMode ){
        case MAX_VARIANCE:
            //Normalize the component weights and workout how many components we need to use to reach the maxVariance
            numPrincipalComponents = 0;
            for(UINT k=0; k<N; k++){
                componentWeights[k] /= sum;
                cumulativeVariance += componentWeights[k];
                if( cumulativeVariance >= maxVariance && numPrincipalComponents==0 ){
                    numPrincipalComponents = k+1;
                }
            }
        break;
        case MAX_NUM_PCS:
            //Normalize the component weights and compute the maxVariance
            maxVariance = 0;
            for(UINT k=0; k<N; k++){
                componentWeights[k] /= sum;
                if( k < numPrincipalComponents ){
                    maxVariance += componentWeights[k];
                }
            }
        break;
        default:
        errorLog << "computeFeatureVector(const MatrixDouble &data,UINT analysisMode) - Unknown analysis mode!" << endl;
        break;
    }
    
    //Get the raw eigenvalues (encase the user asks for these later)
    eigenvalues = eig.getRealEigenvalues();

    //Flag that the features have been computed
    trained = true;

    return true;
}

bool PrincipalComponentAnalysis::project(const MatrixDouble &data,MatrixDouble &prjData){
	
    if( !trained ){
        warningLog << "project(const MatrixDouble &data,MatrixDouble &prjData) - The PrincipalComponentAnalysis module has not been trained!" << endl;
        return false;
    }

    if( data.getNumCols() != numInputDimensions ){
        warningLog << "project(const MatrixDouble &data,MatrixDouble &prjData) - The number of columns in the input vector (" << data.getNumCols() << ") does not match the number of input dimensions (" << numInputDimensions << ")!" << endl;
        return false;
    }
	
    MatrixDouble msData( data );
    prjData.resize(data.getNumRows(),numPrincipalComponents);
	
    if( normData ){
        //Mean subtract the data
        for(UINT i=0; i<data.getNumRows(); i++)
            for(UINT j=0; j<numInputDimensions; j++)
                msData[i][j] = (msData[i][j]-mean[j])/stdDev[j];
    }else{
        //Mean subtract the data
        for(UINT i=0; i<data.getNumRows(); i++)
            for(UINT j=0; j<numInputDimensions; j++)
                msData[i][j] -= mean[j];
    }
	
    //Projected Data
    for(UINT row=0; row<msData.getNumRows(); row++){//For each row in the final data
        for(UINT i=0; i<numPrincipalComponents; i++){//For each PC
            prjData[row][i]=0;
	    for(UINT j=0; j<data.getNumCols(); j++)//For each feature
                prjData[row][i] += msData[row][j] * eigenvectors[j][sortedEigenvalues[i].index];
        }
    }
	
    return true;
}
    
bool PrincipalComponentAnalysis::project(const VectorDouble &data,VectorDouble &prjData){
    
    const unsigned int N = (unsigned int)data.size();
    
    if( !trained ){
        warningLog << "project(const VectorDouble &data,VectorDouble &prjData) - The PrincipalComponentAnalysis module has not been trained!" << endl;
        return false;
    }

    if( N != numInputDimensions ){
        warningLog << "project(const VectorDouble &data,VectorDouble &prjData) - The size of the input vector (" << N << ") does not match the number of input dimensions (" << numInputDimensions << ")!" << endl;
        return false;
    }
    
    VectorDouble msData = data;
    
    if( normData ){
        //Mean subtract the data
        for(UINT j=0; j<numInputDimensions; j++)
            msData[j] = (msData[j]-mean[j])/stdDev[j];
    }else{
        //Mean subtract the data
        for(UINT j=0; j<numInputDimensions; j++)
            msData[j] -= mean[j];
    }
    
    //Projected Data
    prjData.resize( numPrincipalComponents );
    for(UINT i=0; i<numPrincipalComponents; i++){//For each PC
        prjData[i]=0;
        for(UINT j=0; j<N; j++)//For each feature
            prjData[i] += msData[j] * eigenvectors[j][sortedEigenvalues[i].index];
    }
    
    return true;
}

bool PrincipalComponentAnalysis::saveModelToFile(fstream &file) const {

    //Write the header info
    file << "GRT_PCA_MODEL_FILE_V1.0\n";

    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;

    file << "NumPrincipalComponents: " << numPrincipalComponents << endl;
    file << "NormData: " << normData << endl;
    file << "MaxVariance: " << maxVariance << endl;

    if( trained ){
        file << "Mean: ";
        for(unsigned int i=0; i<numInputDimensions; i++){
            file << mean[i] << " ";
        } 
        file << endl;

        file << "StdDev: ";
        for(unsigned int i=0; i<numInputDimensions; i++){
            file << stdDev[i] << " ";
        } 
        file << endl;

        file << "ComponentWeights: ";
        for(unsigned int i=0; i<numInputDimensions; i++){
            file << componentWeights[i] << " ";
        } 
        file << endl;

        file << "Eigenvalues: ";
        for(unsigned int i=0; i<numInputDimensions; i++){
            file << eigenvalues[i] << " ";
        } 
        file << endl;

        file << "SortedEigenvalues: ";
        for(unsigned int i=0; i<numInputDimensions; i++){
            file << sortedEigenvalues[i].index << " ";
            file << sortedEigenvalues[i].value << " ";
        } 
        file << endl;

        file << "Eigenvectors: ";
        file << eigenvectors.getNumRows() << " " << eigenvectors.getNumCols() << endl;
        for(unsigned int i=0; i<eigenvectors.getNumRows(); i++){
            for(unsigned int j=0; j<eigenvectors.getNumCols(); j++){
                file << eigenvectors[i][j];
                if( j+1 < eigenvectors.getNumCols() ) file << " ";
                else file << endl;
            }
        } 
        file << endl;
    }

    return true;
}

bool PrincipalComponentAnalysis::loadModelFromFile(fstream &file) {

    std::string word;

    //Read the header info
    file >> word;
    if(  word != "GRT_PCA_MODEL_FILE_V1.0" ){
        return false;
    }

    if( !MLBase::loadBaseSettingsFromFile( file ) ) return false;

    //Read the num components
    file >> word;
    if(  word != "NumPrincipalComponents:" ){
        return false;
    }
    file >> numPrincipalComponents;

    //Read the normData
    file >> word;
    if(  word != "NormData:" ){
        return false;
    }
    file >> normData;

    //Read the MaxVariance
    file >> word;
    if(  word != "MaxVariance:" ){
        return false;
    }
    file >> maxVariance;

    if( trained ){
        //Read the mean vector
        file >> word;
        if(  word != "Mean:" ){
            trained = false;
            return false;
        }
        mean.resize( numInputDimensions );

        for(unsigned int i=0; i<numInputDimensions; i++){
            file >> mean[i];
        } 

        //Read the stddev vector
        file >> word;
        if(  word != "StdDev:" ){
            trained = false;
            return false;
        }
        stdDev.resize( numInputDimensions );

        for(unsigned int i=0; i<numInputDimensions; i++){
            file >> stdDev[i];
        } 

        //Read the ComponentWeights vector
        file >> word;
        if(  word != "ComponentWeights:" ){
            trained = false;
            return false;
        }
        componentWeights.resize( numInputDimensions );

        for(unsigned int i=0; i<numInputDimensions; i++){
            file >> componentWeights[i];
        } 

        //Read the Eigenvalues vector
        file >> word;
        if(  word != "Eigenvalues:" ){
            trained = false;
            return false;
        }
        eigenvalues.resize( numInputDimensions );

        for(unsigned int i=0; i<numInputDimensions; i++){
            file >> eigenvalues[i];
        } 

        //Read the SortedEigenvalues vector
        file >> word;
        if(  word != "SortedEigenvalues:" ){
            trained = false;
            return false;
        }
        sortedEigenvalues.resize( numInputDimensions );

        for(unsigned int i=0; i<numInputDimensions; i++){
            file >> sortedEigenvalues[i].index;
            file >> sortedEigenvalues[i].value;
        } 

        //Read the Eigenvectors vector
        file >> word;
        if(  word != "Eigenvectors:" ){
            trained = false;
            return false;
        }
        UINT numRows;
        UINT numCols;
        file >> numRows;
        file >> numCols;
        eigenvectors.resize( numRows, numCols );

        for(unsigned int i=0; i<eigenvectors.getNumRows(); i++){
            for(unsigned int j=0; j<eigenvectors.getNumCols(); j++){
                file >> eigenvectors[i][j];
            }
        } 
    }

    return true;
}
    
bool PrincipalComponentAnalysis::print(string title) const{
    
    if( title != "" ){
        cout << title << endl;
    }
    if( !trained ){
        cout << "Not Trained!\n";
        return false;
    }
    cout << "NumInputDimensions: " << numInputDimensions << " NumPrincipalComponents: " << numPrincipalComponents << endl;
    cout << "ComponentWeights: ";
    for(UINT k=0; k<componentWeights.size(); k++){
        cout << "\t" << componentWeights[k];
    }
    cout << endl;
    cout << "SortedEigenValues: ";
    for(UINT k=0; k<sortedEigenvalues.size(); k++){
        cout << "\t" << sortedEigenvalues[k].value;
    }
    cout << endl;
    eigenvectors.print("Eigenvectors:");
    
    return true;
}
    
MatrixDouble PrincipalComponentAnalysis::getEigenVectors() const{
    return eigenvectors;
}

bool PrincipalComponentAnalysis::setModel( const VectorDouble &mean, const MatrixDouble &eigenvectors ){

    if( (UINT)mean.size() != eigenvectors.getNumCols() ){
        return false;
    }

    trained = true;
    numInputDimensions = eigenvectors.getNumCols();
    numPrincipalComponents = eigenvectors.getNumRows();
    this->mean = mean;
    stdDev.clear();
    componentWeights.clear();
    eigenvalues.clear();
    sortedEigenvalues.clear();
    this->eigenvectors = eigenvectors;
    
    //The eigenvectors are already sorted, so the sorted eigenvalues just holds the default index
    for(UINT i=0; i<numPrincipalComponents; i++){
        sortedEigenvalues.push_back( IndexedDouble(i,0.0) );
    }
    return true;
}


}//End of namespace GRT
