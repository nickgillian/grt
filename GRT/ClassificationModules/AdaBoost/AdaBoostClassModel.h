/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief This file implements a container for an AdaBoost class model.
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

#ifndef GRT_ADABOOST_CLASS_MODEL_HEADER
#define GRT_ADABOOST_CLASS_MODEL_HEADER

#include "WeakClassifiers/WeakClassifier.h"

GRT_BEGIN_NAMESPACE

class GRT_API AdaBoostClassModel{
    public:
    AdaBoostClassModel(){
        classLabel = 0;
        errorLog.setKey("[ERROR AdaBoostClassModel]");
    }
    ~AdaBoostClassModel(){
        clear();
    }
    
    AdaBoostClassModel(const AdaBoostClassModel &rhs){
        *this = rhs;
    }
    
    AdaBoostClassModel& operator=(const AdaBoostClassModel &rhs){
        if( this != &rhs ){
            clear();
            this->classLabel = rhs.classLabel;
            
            //We need to deep copy the weak classifiers
            for(UINT i=0; i<rhs.getNumWeakClassifiers(); i++){
                if( !addClassifierToCommitee(rhs.weakClassifiers[i], rhs.weights[i]) ){
                    clear();
                    errorLog << "operator= Failed to deep copy weak classifiers!" << std::endl;
                    break;
                }
            }
        }
        return *this;
    }
    
    bool clear(){
        classLabel = 0;
        weights.clear();
        for(UINT i=0; i<weakClassifiers.size(); i++){
            if( weakClassifiers[i] != NULL ){
                delete weakClassifiers[i];
                weakClassifiers[i] = NULL;
            }
        }
        weakClassifiers.clear();
        return true;
    }
    
    bool setClassLabel(const UINT classLabel){
        this->classLabel = classLabel;
        return true;
    }
    
    bool addClassifierToCommitee(const WeakClassifier *weakClassifier,Float weight){
        if( weakClassifier == NULL ) return false;
        
        //Deep copy the weak classifier
        WeakClassifier *weakClassifierPtr = weakClassifier->createNewInstance();
        if( !weakClassifierPtr->deepCopyFrom( weakClassifier ) ){
            delete weakClassifierPtr;
            weakClassifierPtr = NULL;
            errorLog << "addClassifierToCommitee(...) Failed to add weak classifier to commitee!" << std::endl;
            return false;
        }
        weights.push_back( weight );
        weakClassifiers.push_back( weakClassifierPtr );
        
        return true;
    }
    
    Float predict(const VectorFloat &inputVector){
        Float v = 0;
        UINT N = (UINT)weakClassifiers.getSize();
        for(UINT i=0; i<N; i++){
            v += weakClassifiers[i]->predict( inputVector ) * weights[i];
        }
        //Return the value, rather than the sign of the value
        return v;
    }
    
    void print() const{
        std::cout << "ClassLabel: " << classLabel << std::endl;
        for(UINT i=0; i<weakClassifiers.getSize(); i++){
            std::cout << "Weight: " << weights[i] << std::endl;
            weakClassifiers[i]->print();
        }
    }
    
    UINT getClassLabel() const{
        return classLabel;
    }
    
    UINT getNumWeakClassifiers() const{
        return weakClassifiers.getSize();
    }
    
    Vector< WeakClassifier* > getWeakClassifiers() const{
        
        Vector< WeakClassifier* > classifiers;
        
        for(UINT i=0; i<getNumWeakClassifiers(); i++){
            
            //Deep copy the weak classifier
            WeakClassifier *weakClassifierPtr = weakClassifiers[i]->createNewInstance();
            if( !weakClassifierPtr->deepCopyFrom( weakClassifiers[i] ) ){
                return classifiers;
            }
            classifiers.push_back( weakClassifierPtr );
        }
        
        return classifiers;
    }
    
    template< class T > T* getWeakClassifier(const UINT &index){
        
        if( index >= weakClassifiers.getSize() ) return NULL;
        
        T temp;
        
        if( temp.getWeakClassifierType() == weakClassifiers[index]->getWeakClassifierType() ){
            return (T*)weakClassifiers[index];
        }
        return NULL;
    }
    
    VectorFloat getWeights() const{
        return weights;
    }
    
    bool save( std::fstream &file ) const{
        
        if(!file.is_open())
        {
            return false;
        }
        
        UINT N = getNumWeakClassifiers();
        
        //Write the Model info
        file << "ClassLabel: " << classLabel << std::endl;
        file << "NumWeakClassifiers: "<< getNumWeakClassifiers() << std::endl;
        file << "WeakClassifierTypes: " << std::endl;
        for(UINT i=0; i<N; i++){
            if( weakClassifiers[i] == NULL ) return false;
            file << weakClassifiers[i]->getWeakClassifierType() << std::endl;
        }
        
        file << "Weights: ";
        for(UINT i=0; i<N; i++){
            file << weights[i] << " ";
        }
        file << std::endl;
        
        file << "WeakClassifiers: " << std::endl;
        for(UINT i=0; i<N; i++){
            if( weakClassifiers[i] == NULL ) return false;
            if( !weakClassifiers[i]->saveModelToFile( file ) ) return false;
        }
        
        return true;
    }
    
    bool load( std::fstream &file ){
        
        //Clear any previous models
        clear();
        
        if(!file.is_open())
        {
            errorLog <<"load(fstream &file) - The file is not open!" << std::endl;
            return false;
        }
        
        std::string word;
        UINT numWeakClassifiers = 0;
        
        file >> word;
        if( word != "ClassLabel:" ){
            errorLog <<"load(fstream &file) - Failed to read ClassLabel header!" << std::endl;
            return false;
        }
        file >> classLabel;
        
        file >> word;
        if( word != "NumWeakClassifiers:" ){
            errorLog <<"load(fstream &file) - Failed to read NumWeakClassifiers header!" << std::endl;
            return false;
        }
        file >> numWeakClassifiers;
        
        file >> word;
        if( word != "WeakClassifierTypes:" ){
            errorLog <<"load(fstream &file) - Failed to read WeakClassifierTypes header!" << std::endl;
            return false;
        }
        
        //Load the weak classifier types and setup the weak classifiers
        if( numWeakClassifiers > 0 ){
            weights.resize( numWeakClassifiers, 0 );
            weakClassifiers.resize( numWeakClassifiers, NULL );
            
            for(UINT i=0; i<numWeakClassifiers; i++){
                file >> word;
                weakClassifiers[i] = WeakClassifier::createInstanceFromString(word);
                if( weakClassifiers[i] == NULL ){
                    errorLog << "load(fstream &file) - WeakClassifier " << i << " is NULL!" << std::endl;
                    return false;
                }
            }
        }
        
        //Load the Weights
        file >> word;
        if( word != "Weights:" ){
            errorLog <<"loadModelFromFile(fstream &file) - Failed to read Weights header!" << std::endl;
            return false;
        }
        for(UINT i=0; i<numWeakClassifiers; i++){
            file >> weights[i];
        }
        
        //Load the WeakClassifiers
        file >> word;
        if( word != "WeakClassifiers:" ){
            errorLog <<"loadModelFromFile(fstream &file) - Failed to read WeakClassifiers header!" << std::endl;
            errorLog << word << std::endl;
            return false;
        }
        for(UINT i=0; i<numWeakClassifiers; i++){
            if( !weakClassifiers[i]->loadModelFromFile( file ) ){
                errorLog <<"loadModelFromFile(fstream &file) - Failed to load weakClassifer: " << i << std::endl;
                return false;
            }
        }
        
        //We don't need to close the file as the function that called this function should handle that
        return true;
    }
    
    bool normalizeWeights(){
        if( weights.size() == 0 ) return false;
        Float sum = 0;
        UINT N = (UINT)weights.size();
        for(UINT i=0; i<N; i++){
            sum += weights[i];
        }
        for(UINT i=0; i<N; i++){
            weights[i] /= sum;
        }
        return true;
    }
    
protected:
    UINT classLabel;
    VectorFloat weights;
    Vector< WeakClassifier* > weakClassifiers;
    ErrorLog errorLog;
    
};

GRT_END_NAMESPACE

#endif// GRT_ADABOOST_CLASS_MODEL_HEADER
