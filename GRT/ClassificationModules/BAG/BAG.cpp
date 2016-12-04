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
#include "BAG.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string BAG::id = "BAG";
std::string BAG::getId() { return BAG::id; }

//Register the BAG module with the Classifier base class
RegisterClassifierModule< BAG >  BAG::registerModule( BAG::getId() );

BAG::BAG(bool useScaling) : Classifier( BAG::getId() )
{
    this->useScaling = useScaling;
    useNullRejection = false;
    classifierMode = STANDARD_CLASSIFIER_MODE;
}

BAG::BAG(const BAG &rhs):Classifier( BAG::getId() ){
    classifierMode = STANDARD_CLASSIFIER_MODE;
    *this = rhs;
}

BAG::~BAG(void)
{
    clearEnsemble();
}

BAG& BAG::operator=(const BAG &rhs){
    if( this != &rhs ){
        //Clear any previous ensemble
        clearEnsemble();
        
        //Copy the weights
        this->weights = rhs.weights;
        
        //Deep copy each classifier in the ensemble
        for(UINT i=0; i<rhs.getEnsembleSize(); i++){
            addClassifierToEnsemble( *(rhs.ensemble[i]) );
        }
        //Copy the base classifier variables
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
}

bool BAG::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        BAG *ptr = (BAG*)classifier;
        
        //Clear any previous ensemble
        clearEnsemble();
        
        //Copy the weights
        this->weights = ptr->weights;
        
        //Deep copy each classifier in the ensemble
        for(UINT i=0; i<ptr->getEnsembleSize(); i++){
            addClassifierToEnsemble( *(ptr->ensemble[i]) );
        }
        //Copy the base classifier variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool BAG::train_(ClassificationData &trainingData){
    
    //Clear any previous models
    clear();
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int K = trainingData.getNumClasses();
    
    if( M == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << std::endl;
        return false;
    }
    
    numInputDimensions = N;
    numOutputDimensions = K;
    numClasses = K;
    classLabels.resize(K);
    ranges = trainingData.getRanges();
    ClassificationData validationData;
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        trainingData.scale(0, 1);
    }

    if( useValidationSet ){
        validationData = trainingData.split( 100-validationSetSize );
    }
    
    UINT ensembleSize = ensemble.getSize();
    
    if( ensembleSize == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - The ensemble size is zero! You need to add some classifiers to the ensemble first." << std::endl;
        return false;
    }
    
    for(UINT i=0; i<ensembleSize; i++){
        if( ensemble[i] == NULL ){
            errorLog << "train_(ClassificationData &trainingData) - The classifier at ensemble index " << i << " has not been set!" << std::endl;
            return false;
        }
    }
    
    //Train the ensemble
    for(UINT i=0; i<ensembleSize; i++){
        ClassificationData boostedDataset = trainingData.getBootstrappedDataset();

        //Propagate the training logging to the ensemble
        ensemble[i]->setTrainingLoggingEnabled( this->getTrainingLoggingEnabled() );
        
        trainingLog << "Training ensemble " << i+1 << ". Ensemble type: " << ensemble[i]->getId() << ". Num Training Samples: " << boostedDataset.getNumSamples() << std::endl;
        
        //Train the classifier with the bootstrapped dataset
        if( !ensemble[i]->train_( boostedDataset ) ){
            errorLog << "train_(ClassificationData &trainingData) - The classifier at ensemble index " << i << " failed training!" << std::endl;
            return false;
        }
    }
    
    //Set the class labels
    classLabels = trainingData.getClassLabels();
    
    //Flag that the model has been trained
    trained = true;

    //Compute the final training stats
    trainingSetAccuracy = 0;
    validationSetAccuracy = 0;

    //If scaling was on, then the data will already be scaled, so turn it off temporially so we can test the model accuracy
    bool scalingState = useScaling;
    useScaling = false;
    if( !computeAccuracy( trainingData, trainingSetAccuracy ) ){
        trained = false;
        errorLog << "Failed to compute training set accuracy! Failed to fully train model!" << std::endl;
        return false;
    }
    
    if( useValidationSet ){
        if( !computeAccuracy( validationData, validationSetAccuracy ) ){
            trained = false;
            errorLog << "Failed to compute validation set accuracy! Failed to fully train model!" << std::endl;
            return false;
        }
        
    }

    trainingLog << "Training set accuracy: " << trainingSetAccuracy << std::endl;

    if( useValidationSet ){
        trainingLog << "Validation set accuracy: " << validationSetAccuracy << std::endl;
    }

    //Reset the scaling state for future prediction
    useScaling = scalingState;
    
    return trained;
}

bool BAG::predict_(VectorFloat &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &inputVector) - Model Not Trained!" << std::endl;
        return false;
    }
    
    predictedClassLabel = 0;
    maxLikelihood = -10000;
    
    if( !trained ) return false;
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &inputVector) - The size of the input Vector (" << inputVector.getSize() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
    
    if( classLikelihoods.getSize() != numClasses ) classLikelihoods.resize(numClasses);
    if( classDistances.getSize() != numClasses ) classDistances.resize(numClasses);
    
    //Reset the likelihoods and distances
    for(UINT k=0; k<numClasses; k++){
        classLikelihoods[k] = 0;
        classDistances[k] = 0;
    }
    
    //Run the prediction for each classifier
    Float sum = 0;
    UINT ensembleSize = ensemble.getSize();
    for(UINT i=0; i<ensembleSize; i++){
        
        if( !ensemble[i]->predict(inputVector) ){
            errorLog << "predict_(VectorFloat &inputVector) - The " << i << " classifier in the ensemble failed prediction!" << std::endl;
            return false;
        }
        
        classLikelihoods[ getClassLabelIndexValue( ensemble[i]->getPredictedClassLabel() ) ] += weights[i];
        classDistances[ getClassLabelIndexValue( ensemble[i]->getPredictedClassLabel() ) ] += ensemble[i]->getMaximumLikelihood() * weights[i];
        
        sum += weights[i];
    }
    
    //Set the predicted class label as the most common class
    Float maxCount = 0;
    UINT maxIndex = 0;
    for(UINT i=0; i<numClasses; i++){
        if( classLikelihoods[i] > maxCount ){
            maxIndex = i;
            maxCount = classLikelihoods[i];
        }
        classLikelihoods[i] /= sum;
        classDistances[i] /= Float(ensembleSize);
    }
    
    predictedClassLabel = classLabels[ maxIndex ];
    maxLikelihood = classLikelihoods[ maxIndex ];
    
    return true;
}

bool BAG::reset(){
    
    //Reset all the classifiers
    for(UINT i=0; i<ensemble.getSize(); i++){
        if( ensemble[i] != NULL ){
            ensemble[i]->reset();
        }
    }
    
    return true;
}

bool BAG::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear all the classifiers, but do not remove the ensemble
    for(UINT i=0; i<ensemble.getSize(); i++){
        if( ensemble[i] != NULL ){
            ensemble[i]->clear();
        }
    }
    
    return true;
}

bool BAG::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    const UINT ensembleSize = getEnsembleSize();
    
    //Write the header info
    file << "GRT_BAG_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog <<"save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    if( trained ){
        
        file << "EnsembleSize: " << ensembleSize << std::endl;
        
        if( getEnsembleSize() > 0 ){
            
            //Save the weights
            file << "Weights: ";
            for(UINT i=0; i<getEnsembleSize(); i++){
                file << weights[i];
                if( i < ensembleSize-1 ) file << "\t";
                else file << "\n";
                }
            
            //Save the classifier types
            file << "ClassifierTypes: ";
            for(UINT i=0; i<getEnsembleSize(); i++){
                file << ensemble[i]->getId() << std::endl;
            }
            
            //Save the ensemble
            file << "Ensemble:" << std::endl;
            for(UINT i=0; i<getEnsembleSize(); i++){
                if( !ensemble[i]->save( file ) ){
                    errorLog <<"save(fstream &file) - Failed to save classifier " << i << " to file!" << std::endl;
                    return false;
                }
            }
        }
        
    }
    
    //NOTE: We do not need to close the file
    
    return true;
}

bool BAG::load( std::fstream &file ){
    
    clear();
    UINT ensembleSize = 0;
    
    if(!file.is_open())
    {
        errorLog << "load(string filename) - Could not open file to load model" << std::endl;
        return false;
    }
    
    std::string word;
    file >> word;
    
    //Check to see if we should load a legacy file
    if( word == "GRT_BAG_MODEL_FILE_V1.0" ){
        return loadLegacyModelFromFile( file );
    }
    
    //Find the file type header
    if(word != "GRT_BAG_MODEL_FILE_V2.0"){
        errorLog << "load(string filename) - Could not find Model File Header" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        clear();
        return false;
    }
    
    if( trained ){
        
        //Load the ensemble size
        file >> word;
        if(word != "EnsembleSize:"){
            errorLog << "load(string filename) - Could not find the EnsembleSize!" << std::endl;
            clear();
            return false;
        }
        file >> ensembleSize;
        
        //Load the weights
        weights.resize( ensembleSize );
        
        file >> word;
        if(word != "Weights:"){
            errorLog << "load(string filename) - Could not find the Weights!" << std::endl;
            clear();
            return false;
        }
        for(UINT i=0; i<ensembleSize; i++){
            file >> weights[i];
        }
        
        //Load the classifier types
        Vector< std::string > classifierTypes( ensembleSize );
        
        file >> word;
        if(word != "ClassifierTypes:"){
            errorLog << "load(string filename) - Could not find the ClassifierTypes!" << std::endl;
            clear();
            return false;
        }
        for(UINT i=0; i<ensembleSize; i++){
            file >> classifierTypes[i];
        }
        
        //Load the ensemble
        file >> word;
        if(word != "Ensemble:"){
            errorLog << "load(string filename) - Could not find the Ensemble! Found: " << word << std::endl;
            clear();
            return false;
        }
        ensemble.resize(ensembleSize,NULL);
        for(UINT i=0; i<ensembleSize; i++){
            ensemble[i] = create( classifierTypes[i] );
            
            if( ensemble[i] == NULL ){
                errorLog << "load(string filename) - Could not create a new classifier instance from the classifierType: " << classifierTypes[i] << std::endl;
                clear();
                clearEnsemble();
                return false;
            }
            
            if( !ensemble[i]->load( file ) ){
                errorLog << "load(string filename) - Failed to load ensemble classifier: " << i << std::endl;
                clear();
                clearEnsemble();
                return false;
            }
        }
        
        //Recompute the null rejection thresholds
        recomputeNullRejectionThresholds();
        
        //Resize the prediction results to make sure it is setup for realtime prediction
        maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
        bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
        classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
        classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    }
    
    return true;
}

UINT BAG::getEnsembleSize() const{
    return ensemble.getSize();
}

VectorFloat BAG::getEnsembleWeights() const{
    return weights;
}

const Vector< Classifier* > BAG::getEnsemble() const{
    return ensemble;
}

bool BAG::addClassifierToEnsemble(const Classifier &classifier,Float weight){
    
    trained = false;
    
    Classifier *newClassifier = classifier.create( classifier.getId() );
    
    if( newClassifier == NULL ){
        return false;
    }
    
    if( !newClassifier->deepCopyFrom( &classifier ) ){
        return false;
    }
    
    weights.push_back( weight );
    ensemble.push_back( newClassifier );
    
    return true;
}

bool BAG::clearEnsemble(){
    
    trained = false;
    for(UINT i=0; i<ensemble.getSize(); i++){
        if( ensemble[i] != NULL ){
            delete ensemble[i];
            ensemble[i] = NULL;
        }
    }
    weights.clear();
    ensemble.clear();
    
    return true;
}

bool BAG::setWeights(const VectorFloat &weights){
    
    if( this->weights.size() != weights.size() ){
        return false;
    }
    this->weights = weights;
    return true;
}

bool BAG::loadLegacyModelFromFile( std::fstream &file ){
    
    std::string word;
    
    return true;
}

GRT_END_NAMESPACE
