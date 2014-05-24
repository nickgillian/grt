/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements the Linear Discriminant Analysis Classification algorithm.
 
 @warning: This algorithm is still under development and has not been fully implemented yet!
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

#ifndef GRT_LDA_HEADER
#define GRT_LDA_HEADER

#include "../../CoreModules/Classifier.h"
#include "../../Util/LUDecomposition.h"

namespace GRT{
    
class LDAClassModel{
public:
    LDAClassModel(){
        classLabel = 0;
        priorProb = 0;
    }
    
    ~LDAClassModel(){
        
    }
    
    UINT getNumDimensions() const { return (UINT)weights.size(); }
    
    UINT classLabel;
    double priorProb;
    VectorDouble weights;
    
};
    
class LDA : public Classifier
{
public:
    /**
     Default Constructor
     */
	LDA(bool useScaling=false,bool useNullRejection=true,double nullRejectionCoeff=10.0);
    
    /**
     Default Destructor
     */
	virtual ~LDA(void);
    
    /**
     Defines how the data from the rhs LDA should be copied to this LDA
     
     @param const LDA &rhs: another instance of a LDA
     @return returns a pointer to this instance of the LDA
     */
	LDA &operator=(const LDA &rhs){
		if( this != &rhs ){
            //LDA variables
            this->models = rhs.models;
            
            //Classifier variables
            copyBaseVariables( (Classifier*)&rhs );
		}
		return *this;
	}
    
    //Override the base class methods
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier method is called.  
     It clones the data from the Base Class Classifier pointer (which should be pointing to an LDA instance) into this instance
     
     @param Classifier *classifier: a pointer to the Classifier Base Class, this should be pointing to another LDA instance
     @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier){
        if( classifier == NULL ) return false;
        
        if( this->getClassifierType() == classifier->getClassifierType() ){
   
            LDA *ptr = (LDA*)classifier;
            //Clone the LDA values 
            this->models = ptr->models;
            
            //Clone the classifier variables
            return copyBaseVariables( classifier );
        }
        return false;
    }
    
    /**
     This trains the LDA model, using the labelled classification data.
     This overrides the train function in the Classifier base class.
     
     @param LabelledClassificationData trainingData: a reference to the training data
     @return returns true if the LDA model was trained, false otherwise
    */
    virtual bool train(ClassificationData trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the Classifier base class.
     
     @param VectorDouble inputVector: the input vector to classify
     @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict(VectorDouble inputVector);
    
    /**
     This saves the trained LDA model to a file.
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param string filename: the name of the file to save the LDA model to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the trained LDA model to a file.
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the LDA model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained LDA model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param string filename: the name of the file to load the LDA model from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads a trained LDA model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the LDA model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    //Getters
    vector< LDAClassModel > getModels(){ if( trained ){ return models; } return vector< LDAClassModel >(); }
    
    //Setters


private:
	MatrixDouble computeBetweenClassScatterMatrix( ClassificationData &data );
	MatrixDouble computeWithinClassScatterMatrix( ClassificationData &data );
	
    vector< LDAClassModel > models;
};

} //End of namespace GRT

#endif //GRT_LDA_HEADER

