/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief This class implements a MixtureModel, which is a container for holding a class model for the
GRT::GMM class.

@example ClassificationModulesExamples/GMMExample/GMMExample.cpp
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

#ifndef GRT_MIXTURE_MODEL_HEADER
#define GRT_MIXTURE_MODEL_HEADER

#include "../../CoreModules/Classifier.h"
#include "../../ClusteringModules/GaussianMixtureModels/GaussianMixtureModels.h"

GRT_BEGIN_NAMESPACE

class GuassModel{
public:
    GuassModel(){
        det = 0;
    }
    
    ~GuassModel(){
        
    }
    
    bool printModelValues() const{
        if( mu.size() == 0 ) return false;
        
        std::cout << "Determinate: " << det << std::endl;
        std::cout << "Mu: ";
        for(UINT i=0; i<mu.getSize(); i++)
        std::cout << mu[i] << "\t";
        std::cout << std::endl;
        
        std::cout << "Sigma: \n";
        for(UINT i=0; i<sigma.getNumRows(); i++){
            for(UINT j=0; j<sigma.getNumCols(); j++){
                std::cout << sigma[i][j] << "\t";
            }std::cout << std::endl;
        }std::cout << std::endl;
        
        std::cout << "InvSigma: \n";
        for(UINT i=0; i<invSigma.getNumRows(); i++){
            for(UINT j=0; j<invSigma.getNumCols(); j++){
                std::cout << invSigma[i][j] << "\t";
            }std::cout << std::endl;
        }std::cout << std::endl;
        
        return true;
    }
    
    Float det;
    VectorFloat mu;
    MatrixFloat sigma;
    MatrixFloat invSigma;
};

class MixtureModel{
public:
    MixtureModel(){
        classLabel = 0;
        K = 0;
        normFactor = 1;
        nullRejectionThreshold = 0;
        trainingMu = 0;
        trainingSigma = 0;
        gamma = 1;
    }
    ~MixtureModel(){
        gaussModels.clear();
    }
    
    inline GuassModel& operator[](const UINT i){
        return gaussModels[i];
    }
    
    inline const GuassModel& operator[](const UINT i) const{
        return gaussModels[i];
    }
    
    Float computeMixtureLikelihood( const VectorFloat &x ){
        Float sum = 0;
        for(UINT k=0; k<K; k++){
            sum += gauss(x,gaussModels[k].det,gaussModels[k].mu,gaussModels[k].invSigma);
        }
        //Normalize the mixture likelihood
        return sum/normFactor;
    }
    
    bool resize(UINT K){
        if( K > 0 ){
            this->K = K;
            gaussModels.clear();
            gaussModels.resize(K);
            return true;
        }
        return false;
    }
    
    bool recomputeNullRejectionThreshold(Float gamma){
        Float newRejectionThreshold = 0;
        //TODO - Need a way of improving the null rejection threshold for the GMMs!!!!
        newRejectionThreshold = trainingMu - (trainingSigma*gamma);
        newRejectionThreshold = 0.02;
        
        //Make sure that the new rejection threshold is greater than zero
        if( newRejectionThreshold > 0 ){
            this->gamma = gamma;
            this->nullRejectionThreshold = newRejectionThreshold;
            return true;
        }
        return false;
    }
    
    bool recomputeNormalizationFactor(){
        normFactor = 0;
        for(UINT k=0; k<K; k++){
            normFactor += gauss(gaussModels[k].mu,gaussModels[k].det,gaussModels[k].mu,gaussModels[k].invSigma);
        }
        return true;
    }
    
    bool printModelValues() const{
        if( gaussModels.getSize() > 0 ){
            for(UINT k=0; k<gaussModels.getSize(); k++){
                gaussModels[k].printModelValues();
            }
            return true;
        }
        return false;
    }
    
    UINT getK() const { return K; }
    
    UINT getClassLabel() const { return classLabel; }
    
    Float getTrainingMu() const {
        return trainingMu;
    }
    
    Float getTrainingSigma() const {
        return trainingSigma;
    }
    
    Float getNullRejectionThreshold() const {
        return nullRejectionThreshold;
    }
    
    Float getNormalizationFactor() const {
        return normFactor;
    }
    
    bool setClassLabel(const UINT classLabel){
        this->classLabel = classLabel;
        return true;
    }
    
    bool setNormalizationFactor(const Float normFactor){
        this->normFactor = normFactor;
        return true;
    }
    
    bool setTrainingMuAndSigma(const Float trainingMu,const Float trainingSigma){
        this->trainingMu = trainingMu;
        this->trainingSigma = trainingSigma;
        return true;
    }
    
    bool setNullRejectionThreshold(const Float nullRejectionThreshold){
        this->nullRejectionThreshold = nullRejectionThreshold;
        return true;
    }
    
private:
    Float gauss(const VectorFloat &x,Float det,const VectorFloat &mu,const MatrixFloat &invSigma){
        
        Float y = 0;
        Float sum = 0;
        const UINT N = x.getSize();
        VectorFloat temp(N,0);
        
        //Compute the first part of the equation
        y = (1.0/pow(TWO_PI,N/2.0)) * (1.0/pow(det,0.5));
        
        //Compute the later half
        for(UINT i=0; i<N; i++){
            for(UINT j=0; j<N; j++){
                temp[i] += (x[j]-mu[j]) * invSigma[j][i];
            }
            sum += (x[i]-mu[i]) * temp[i];
        }
        
        return y*grt_exp( -0.5*sum );
    }
    
    UINT classLabel;
    UINT K;
    Float nullRejectionThreshold;
    Float gamma;                           //The number of standard deviations to use for the threshold
    Float trainingMu;                      //The average confidence value in the training data
    Float trainingSigma;                   //The simga confidence value in the training data
    Float normFactor;
    Vector< GuassModel > gaussModels;
    
};

GRT_END_NAMESPACE

#endif //GRT_MIXTURE_MODEL_HEADER
