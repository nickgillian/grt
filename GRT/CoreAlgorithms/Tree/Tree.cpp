/*
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

#define GRT_DLL_EXPORTS
#include "Tree.h"

GRT_BEGIN_NAMESPACE

Tree::Tree(const UINT numSplittingSteps,const UINT minNumSamplesPerNode,const UINT maxDepth,const bool removeFeaturesAtEachSpilt,const TrainingMode trainingMode) : MLBase("Tree")
{
    tree = NULL;
    this->numSplittingSteps = numSplittingSteps;
    this->minNumSamplesPerNode = minNumSamplesPerNode;
    this->maxDepth = maxDepth;
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    this->trainingMode = trainingMode;
}

Tree::~Tree(void)
{
    clear();
}

bool Tree::clear(){
    
    if( tree != NULL ){
        tree->clear();
        delete tree;
        tree = NULL;
    }
    
    return true;
}

bool Tree::print() const{
    std::ostringstream stream;
    if( tree != NULL ){
        if( tree->getModel( stream ) ){
            std::cout << stream.str();
            return true;
        }
    }
    return false;
}

bool Tree::getModel( std::ostream &stream ) const{
    
    if( tree != NULL ){
        if( tree->getModel( stream ) ){
            return true;
        }
    }
    
    return false;
}

Node* Tree::deepCopyTree() const{
    
    if( tree == NULL ){
        return NULL;
    }
    
    return tree->deepCopyNode();
}

const Node* Tree::getTree() const{
    return tree;
}

UINT Tree::getTrainingMode() const{
    return trainingMode;
}

UINT Tree::getNumSplittingSteps()const{
    return numSplittingSteps;
}

UINT Tree::getMinNumSamplesPerNode()const{
    return minNumSamplesPerNode;
}

UINT Tree::getMaxDepth()const{
    return maxDepth;
}

UINT Tree::getPredictedNodeID()const{
    
    if( tree == NULL ){
        return 0;
    }
    
    return tree->getPredictedNodeID();
}

bool Tree::getRemoveFeaturesAtEachSpilt() const{
    return removeFeaturesAtEachSpilt;
}

bool Tree::setTrainingMode(const TrainingMode trainingMode){ 
    if( trainingMode >= BEST_ITERATIVE_SPILT && trainingMode < NUM_TRAINING_MODES ){
        this->trainingMode = trainingMode;
        return true;
    }
    warningLog << "Unknown trainingMode: " << trainingMode << std::endl;
    return false;
}

bool Tree::setNumSplittingSteps(const UINT numSplittingSteps){
    if( numSplittingSteps > 0 ){
        this->numSplittingSteps = numSplittingSteps;
        return true;
    }
    warningLog << "setNumSplittingSteps(const UINT numSplittingSteps) - The number of splitting steps must be greater than zero!" << std::endl;
    return false;
}

bool Tree::setMinNumSamplesPerNode(const UINT minNumSamplesPerNode){
    if( minNumSamplesPerNode > 0 ){
        this->minNumSamplesPerNode = minNumSamplesPerNode;
        return true;
    }
    warningLog << "setMinNumSamplesPerNode(const UINT minNumSamplesPerNode) - The minimum number of samples per node must be greater than zero!" << std::endl;
    return false;
}

bool Tree::setMaxDepth(const UINT maxDepth){
    if( maxDepth > 0 ){
        this->maxDepth = maxDepth;
        return true;
    }
    warningLog << "setMaxDepth(const UINT maxDepth) - The maximum depth must be greater than zero!" << std::endl;
    return false;
}

bool Tree::setRemoveFeaturesAtEachSpilt(const bool removeFeaturesAtEachSpilt){
    this->removeFeaturesAtEachSpilt = removeFeaturesAtEachSpilt;
    return true;
}

GRT_END_NAMESPACE
