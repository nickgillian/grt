/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This file implements a DecisionTreeNode, which is a specific base node used for a DecisionTree.
 
 @example ClassificationModulesExamples/DecisionTreeExample/DecisionTreeExample.cpp
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

#ifndef GRT_DECISION_TREE_NODE_HEADER
#define GRT_DECISION_TREE_NODE_HEADER

#include "../../CoreAlgorithms/Tree/Node.h"
#include "../../CoreAlgorithms/Tree/Tree.h"
#include "../../DataStructures/ClassificationData.h"

GRT_BEGIN_NAMESPACE
    
class GRT_API DecisionTreeNode : public Node{
public:
    /**
     Default Constructor. Sets all the pointers to NULL.
     */
    DecisionTreeNode();
    
    /**
     Default Destructor. Cleans up any memory.
     */
    virtual ~DecisionTreeNode();
    
    /**
     This function recursively predicts if the probability of the input Vector.  
     If this node is a leaf node, then the class likelihoods are equal to the class probabilities at the leaf node.
     If this node is not a leaf node, then this function will recursively call the predict function on either the left or right children
     until a leaf node is reached.
     
     NOTE: The threshold, featureIndex and classProbabilities should be set first BEFORE this function is called. The threshold, featureIndex 
     and classProbabilities can be set by training the node through the DecisionTree class.
     
     @param x: the input Vector that will be used for the prediction
     @param classLikelihoods: a reference to a Vector that will store the class probabilities
     @return returns true if the input is greater than or equal to the nodes threshold, false otherwise
     */
    virtual bool predict(const VectorFloat &x,VectorFloat &classLikelihoods);
    
    /**
     This function calls the best spliting algorithm based on the current trainingMode.  
     
     This function will return true if the best spliting algorithm found a split, false otherwise.
     
     @param trainingMode: the training mode to use, this should be one of the
     @param numSplittingSteps: sets the number of iterations that will be used to search for the best threshold
     @param trainingData: the training data to use for the best split search
     @param features: a Vector containing the indexs of the features that can be used for the search
     @param classLabels: a Vector containing the class labels for the search
     @param featureIndex: this will store the best feature index found during the search
     @param minError: this will store the minimum error found during the search
     @return returns true if the best spliting algorithm found a split, false otherwise
     */
    virtual bool computeBestSpilt( const UINT &trainingMode, const UINT &numSplittingSteps,const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError );

    /**
     This functions cleans up any dynamic memory assigned by the node.
     It will recursively clear the memory for the left and right child nodes.
     
     @return returns true of the node was cleared correctly, false otherwise
     */
    virtual bool clear();
    
    /**
     This function adds the current model to the formatted stream.
     This function should be overwritten by the derived class.
     
     @param stream: a reference to the stream the model will be added to
     @return returns true if the model was added successfully, false otherwise
     */
    virtual bool getModel( std::ostream &stream ) const;
    
    /**
     This function returns a deep copy of the DecisionTreeNode and all it's children.
     The user is responsible for managing the dynamic data that is returned from this function as a pointer.
     
     @return returns a pointer to a deep copy of the DecisionTreeNode, or NULL if the deep copy was not successful
     */
    virtual Node* deepCopyNode() const;
    
    /**
     This function returns a deep copy of the DecisionTreeNode and all it's children.
     The user is responsible for managing the dynamic data that is returned from this function as a pointer.
     
     @return returns a pointer to a deep copy of the DecisionTreeNode, or NULL if the deep copy was not successful
     */
    DecisionTreeNode* deepCopy() const;
    
    /**
     This function returns the nodeSize, this is the number of training samples that reached the node during the training phase.
     
     @return returns the nodeSize
     */
    UINT getNodeSize() const;
    
    /**
     This function returns the number of classes in the class probabilities Vector.
     
     @return returns the number of classes in the class probabilities Vector
     */
    UINT getNumClasses() const;
    
    /**
     This function returns the class probabilities Vector.
     
     @return returns the classProbabilities Vector
     */
    VectorFloat getClassProbabilities() const;
    
    /**
     This function sets the Decision Tree Node as a leaf node.
     
     @param nodeSize: sets the node size, this is the number of training samples at that node
     @param classProbabilities: the Vector of class probabilities at this node
     @return returns true if the node was updated, false otherwise
     */
    bool setLeafNode( const UINT nodeSize, const VectorFloat &classProbabilities );
    
    /**
     This function sets the Decision Tree Node nodeSize.
     
     @param nodeSize: sets the node size, this is the number of training samples at that node
     @return returns true if the node size was set, false otherwise
     */
    bool setNodeSize(const UINT nodeSize);
    
    /**
     This function sets the Decision Tree Node class probabilities.
     
     @param classProbabilities: the Vector of class probabilities at this node
     @return returns true if the node was set classProbabilities, false otherwise
     */
    bool setClassProbabilities(const VectorFloat &classProbabilities);
    
    static UINT getClassLabelIndexValue(UINT classLabel,const Vector< UINT > &classLabels);
    
    using Node::predict;
    
protected:
    virtual bool computeBestSpiltBestIterativeSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError ){
        
        errorLog << "computeBestSpiltBestIterativeSpilt(...) - Base class not overwritten!" << std::endl;
        
        return false;
    }
    
    virtual bool computeBestSpiltBestRandomSpilt( const UINT &numSplittingSteps, const ClassificationData &trainingData, const Vector< UINT > &features, const Vector< UINT > &classLabels, UINT &featureIndex, Float &minError ){
        
        errorLog << "computeBestSpiltBestRandomSpilt(...) - Base class not overwritten!" << std::endl;
        
        return false;
    }
    
    /**
     This saves the DecisionTreeNode custom parameters to a file. It will be called automatically by the Node base class
     if the save function is called.
     
     @param file: a reference to the file the parameters will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveParametersToFile( std::fstream &file ) const{
        
        if( !file.is_open() )
        {
            errorLog << "saveParametersToFile(fstream &file) - File is not open!" << std::endl;
            return false;
        }
        
        //Save the custom DecisionTreeNode parameters
        file << "NodeSize: " << nodeSize << std::endl;
        file << "NumClasses: " << classProbabilities.size() << std::endl;
        file << "ClassProbabilities: ";
        if( classProbabilities.size() > 0 ){
            for(UINT i=0; i<classProbabilities.size(); i++){
                file << classProbabilities[i];
                if( i < classProbabilities.size()-1 ) file << "\t";
                else file << std::endl;
            }
        }
        
        return true;
    }
    
    /**
     This loads the Decision Tree Node parameters from a file.
     
     @param file: a reference to the file the parameters will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadParametersFromFile( std::fstream &file ){
        
        if( !file.is_open() )
        {
            errorLog << "loadParametersFromFile(fstream &file) - File is not open!" << std::endl;
            return false;
        }
        
        classProbabilities.clear();
        
        std::string word;
        UINT numClasses;
        
        //Load the custom DecisionTreeNode Parameters
        file >> word;
        if( word != "NodeSize:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find NodeSize header!" << std::endl;
            return false;
        }
        file >> nodeSize;
        
        file >> word;
        if( word != "NumClasses:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find NumClasses header!" << std::endl;
            return false;
        }
        file >> numClasses;
        if( numClasses > 0 )
            classProbabilities.resize( numClasses );
        
        file >> word;
        if( word != "ClassProbabilities:" ){
            errorLog << "loadParametersFromFile(fstream &file) - Failed to find ClassProbabilities header!" << std::endl;
            return false;
        }
        if( numClasses > 0 ){
            for(UINT i=0; i<numClasses; i++){
                file >> classProbabilities[i];
            }
        }
        
        return true;
    }
    
    UINT nodeSize;
    VectorFloat classProbabilities;
    
    static RegisterNode< DecisionTreeNode > registerModule;
};

GRT_END_NAMESPACE

#endif //GRT_DECISION_TREE_NODE_HEADER

