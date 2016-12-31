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
#include "HMM.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string HMM::id = "HMM";
std::string HMM::getId() { return HMM::id; }

//Register the HMM with the classifier base type
RegisterClassifierModule< HMM > HMM::registerModule( HMM::getId() );

HMM::HMM(const UINT hmmType,const UINT modelType,const UINT delta,const bool useScaling,const bool useNullRejection) : Classifier( HMM::getId() )
{
    this->hmmType = hmmType;
    this->modelType = modelType;
    this->delta = delta;
    this->useScaling = useScaling;
    this->useNullRejection = useNullRejection;
    
    //Default discrete setup
    numStates = 10;
    numSymbols = 20;
    maxNumEpochs = 1000;
    minChange = 1.0e-5;
    
    //Default continuous setup
    downsampleFactor = 5;
    committeeSize = 5;
    sigma = 10.0;
    autoEstimateSigma = true;
    
    supportsNullRejection = false; //TODO - need to add better null rejection support
    classifierMode = TIMESERIES_CLASSIFIER_MODE;
}

HMM::HMM(const HMM &rhs) : Classifier( HMM::getId() )
{
    classifierMode = TIMESERIES_CLASSIFIER_MODE;
    *this = rhs;
}

HMM::~HMM(void)
{
}

HMM& HMM::operator=(const HMM &rhs){
    if( this != &rhs ){
        this->hmmType = rhs.hmmType;
        this->modelType = rhs.modelType;
        this->delta = rhs.delta;
        this->numStates = rhs.numStates;
        this->numSymbols = rhs.numSymbols;
        this->downsampleFactor = rhs.downsampleFactor;
        this->committeeSize = rhs.committeeSize;
        this->sigma = rhs.sigma;
        this->autoEstimateSigma = rhs.autoEstimateSigma;
        this->discreteModels = rhs.discreteModels;
        this->continuousModels = rhs.continuousModels;
        
        copyBaseVariables( (Classifier*)&rhs );
    }
    return *this;
    
}

bool HMM::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getId() == classifier->getId() ){
        
        //Cast the classifier pointer to an HMM pointer
        const HMM *ptr = dynamic_cast<const HMM*>(classifier);
        
        //Copy the HMM variables
        this->hmmType = ptr->hmmType;
        this->modelType = ptr->modelType;
        this->delta = ptr->delta;
        this->numStates = ptr->numStates;
        this->numSymbols = ptr->numSymbols;
        this->downsampleFactor = ptr->downsampleFactor;
        this->committeeSize = ptr->committeeSize;
        this->sigma = ptr->sigma;
        this->autoEstimateSigma = ptr->autoEstimateSigma;
        this->discreteModels = ptr->discreteModels;
        this->continuousModels = ptr->continuousModels;
        
        //Copy the base variables
        return copyBaseVariables( classifier );
    }
    return false;
}

bool HMM::train(ClassificationData trainingData){
    errorLog << __GRT_LOG__ << " The HMM classifier should be trained using the train(TimeSeriesClassificationData &trainingData) method" << std::endl;
    return false;
}


bool HMM::train_(TimeSeriesClassificationData &trainingData){
    
    switch( hmmType ){
        case HMM_DISCRETE:
        return train_discrete( trainingData );
        break;
        case HMM_CONTINUOUS:
        return train_continuous( trainingData );
        break;
    }
    
    errorLog << __GRT_LOG__ << " Failed to train model, unknown HMM type!" << std::endl;
    
    return false;
}

bool HMM::train_discrete(TimeSeriesClassificationData &trainingData){
    
    clear();
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << __GRT_LOG__ << " There are no training samples to train the HMM classifer!" << std::endl;
        return false;
    }
    
    if( trainingData.getNumDimensions() != 1 ){
        errorLog << __GRT_LOG__ << " The number of dimensions in the training data must be 1. If your training data is not 1 dimensional then you must quantize the training data using one of the GRT quantization algorithms" << std::endl;
        return false;
    }
    
    //Reset the HMM
    numInputDimensions = trainingData.getNumDimensions();
    numClasses = trainingData.getNumClasses();
    discreteModels.resize( numClasses );
    classLabels.resize( numClasses );
    
    //Init the models
    for(UINT k=0; k<numClasses; k++){
        discreteModels[k].resetModel(numStates,numSymbols,modelType,delta);
        discreteModels[k].setMaxNumEpochs( maxNumEpochs );
        discreteModels[k].setMinChange( minChange );
    }
    
    //Train each of the models
    for(UINT k=0; k<numClasses; k++){
        //Get the class ID of this gesture
        UINT classID = trainingData.getClassTracker()[k].classLabel;
        classLabels[k] = classID;
        
        //Convert this classes training data into a list of observation sequences
        TimeSeriesClassificationData classData = trainingData.getClassData( classID );
        Vector< Vector< UINT > > observationSequences;
        if( !convertDataToObservationSequence( classData, observationSequences ) ){
            return false;
        }
        
        //Train the model
        if( !discreteModels[k].train( observationSequences ) ){
            errorLog << __GRT_LOG__ << " Failed to train HMM for class " << classID << std::endl;
            return false;
        }
    }
    
    //Compute the rejection thresholds
    nullRejectionThresholds.resize(numClasses);
    
    for(UINT k=0; k<numClasses; k++){
        //Get the class ID of this gesture
        UINT classID = trainingData.getClassTracker()[k].classLabel;
        classLabels[k] = classID;
        
        //Convert this classes training data into a list of observation sequences
        TimeSeriesClassificationData classData = trainingData.getClassData( classID );
        Vector< Vector< UINT > > observationSequences;
        if( !convertDataToObservationSequence( classData, observationSequences ) ){
            return false;
        }
        
        //Test the model
        Float loglikelihood = 0;
        Float avgLoglikelihood = 0;
        for(UINT i=0; i<observationSequences.size(); i++){
            loglikelihood = discreteModels[k].predict( observationSequences[i] );
            avgLoglikelihood += fabs( loglikelihood );
        }
        nullRejectionThresholds[k] = -( avgLoglikelihood / Float( observationSequences.size() ) );
    }
    
    //Flag that the model has been trained
    trained = true;
    converged = true;
    
    return true;
    
}

bool HMM::train_continuous(TimeSeriesClassificationData &trainingData){
    
    clear();
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << __GRT_LOG__ << " There are no training samples to train the CHMM classifer!" << std::endl;
        return false;
    }
    
    //Reset the CHMM
    numInputDimensions = trainingData.getNumDimensions();
    numClasses = trainingData.getNumClasses();
    classLabels.resize( numClasses );
    for(UINT k=0; k<numClasses; k++){
        classLabels[k] = trainingData.getClassTracker()[k].classLabel;
    }
    
    //Scale the training data if needed
    ranges = trainingData.getRanges();
    if( useScaling ){
        trainingData.scale(0, 1);
    }
    
    //Setup the models, there will be 1 model for each training sample
    const UINT numTrainingSamples = trainingData.getNumSamples();
    continuousModels.resize( numTrainingSamples );
    
    //Train each of the models
    for(UINT k=0; k<numTrainingSamples; k++){
        
        //Init the model
        continuousModels[k].setDownsampleFactor( downsampleFactor );
        continuousModels[k].setModelType( modelType );
        continuousModels[k].setDelta( delta );
        continuousModels[k].setSigma( sigma );
        continuousModels[k].setAutoEstimateSigma( autoEstimateSigma );
        continuousModels[k].enableScaling( false ); //Scaling should always off for the models as we do any scaling in the CHMM
        
        //Train the model
        if( !continuousModels[k].train_( trainingData[k] ) ){
            errorLog << __GRT_LOG__ << " Failed to train CHMM for sample " << k << std::endl;
                return false;
        }
    }
    
    if( committeeSize > trainingData.getNumSamples() ){
        committeeSize = trainingData.getNumSamples();
        warningLog << __GRT_LOG__ << " The committeeSize is larger than the number of training sample. Setting committeeSize to number of training samples: " << trainingData.getNumSamples() << std::endl;
    }
    
    //Flag that the model has been trained
    trained = true;
    
    //Compute any null rejection thresholds if needed
    if( useNullRejection ){
        //Compute the rejection thresholds
        nullRejectionThresholds.resize(numClasses);
    }
    
    return true;
}

bool HMM::predict_(VectorFloat &inputVector){
    
    switch( hmmType ){
        case HMM_DISCRETE:
        return predict_discrete( inputVector );
        break;
        case HMM_CONTINUOUS:
        return predict_continuous( inputVector );
        break;
    }
    
    errorLog << __GRT_LOG__ << " Failed to predict, unknown HMM type!" << std::endl;
    
    return false;
}

bool HMM::predict_discrete( VectorFloat &inputVector ){
    
    predictedClassLabel = 0;
    maxLikelihood = -10000;
    
    if( !trained ){
        errorLog << __GRT_LOG__ << " The HMM classifier has not been trained!" << std::endl;
        return false;
    }
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << __GRT_LOG__ << " The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    Float sum = 0;
    bestDistance = -99e+99;
    UINT bestIndex = 0;
    UINT newObservation = (UINT)inputVector[0];
    
    if( newObservation >= numSymbols ){
        errorLog << __GRT_LOG__ << " The new observation is not a valid symbol! It should be in the range [0 numSymbols-1]" << std::endl;
        return false;
    }
    
    for(UINT k=0; k<numClasses; k++){
        classDistances[k] = discreteModels[k].predict( newObservation );
        
        //Set the class likelihood as the antilog of the class distances
        classLikelihoods[k] = grt_antilog( classDistances[k] );
        
        //The loglikelihood values are negative so we want the values closest to 0
        if( classDistances[k] > bestDistance ){
            bestDistance = classDistances[k];
            bestIndex = k;
        }
        
        sum += classLikelihoods[k];
    }
    
    //Turn the class distances into proper likelihoods
    for(UINT k=0; k<numClasses; k++){
        classLikelihoods[k] /= sum;
    }
    
    maxLikelihood = classLikelihoods[ bestIndex ];
    predictedClassLabel = classLabels[ bestIndex ];
    
    if( useNullRejection ){
        if( maxLikelihood > nullRejectionThresholds[ bestIndex ] ){
            predictedClassLabel = classLabels[ bestIndex ];
        }else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    }
    
    return true;
}

bool HMM::predict_continuous( VectorFloat &inputVector ){
    
    if( !trained ){
        errorLog << __GRT_LOG__ << " The HMM classifier has not been trained!" << std::endl;
        return false;
    }
    
    if( inputVector.getSize() != numInputDimensions ){
        errorLog << __GRT_LOG__ << " The size of the input vector (" << inputVector.getSize() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    //Scale the input vector if needed
    if( useScaling ){
        for(UINT i=0; i<numInputDimensions; i++){
            inputVector[i] = scale(inputVector[i], ranges[i].minValue, ranges[i].maxValue, 0, 1);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    std::fill(classLikelihoods.begin(),classLikelihoods.end(),0);
    std::fill(classDistances.begin(),classDistances.end(),0);
    
    bestDistance = -1000;
    UINT bestIndex = 0;
    Float minValue = -1000;
    
    const UINT numModels = (UINT)continuousModels.size();
    Vector< IndexedDouble > results(numModels);
    for(UINT i=0; i<numModels; i++){
        
        //Run the prediction for this model
        if( continuousModels[i].predict_( inputVector ) ){
            results[i].value = continuousModels[i].getLoglikelihood();
            results[i].index = continuousModels[i].getClassLabel();
        }else{
            errorLog << __GRT_LOG__ << " Prediction failed for model: " << i << std::endl;
            return false;
        }
        
        if( results[i].value < minValue ){
            if( !grt_isnan(results[i].value) ){
                minValue = results[i].value;
            }
        }
        
        if( results[i].value > bestDistance ){
            if( !grt_isnan(results[i].value) ){
                bestDistance = results[i].value;
                bestIndex = i;
            }
        }
        
        //cout << "value: " << results[i].value << " label: " << results[i].index << std::endl;
    }
    
    //Store the phase from the best model
    phase = continuousModels[ bestIndex ].getPhase();
    
    //Sort the results
    std::sort(results.begin(),results.end(),IndexedDouble::sortIndexedDoubleByValueDescending);
    
    //Run the majority vote
    const Float committeeWeight = 1.0 / committeeSize;
    for(UINT i=0; i<committeeSize; i++){
        classDistances[ getClassLabelIndexValue( results[i].index ) ] += Util::scale(results[i].value, -1000, 0, 0, committeeWeight, true);
    }
    
    //Turn the class distances into likelihoods
    Float sum = Util::sum(classDistances);
    if( sum > 0 ){
        for(UINT k=0; k<numClasses; k++){
            classLikelihoods[k] = classDistances[k] / sum;
        }
        
        //Find the maximum label
        for(UINT k=0; k<numClasses; k++){
            if( classDistances[k] > bestDistance ){
                bestDistance = classDistances[k];
                bestIndex = k;
            }
        }
        
        maxLikelihood = classLikelihoods[ bestIndex ];
        predictedClassLabel = classLabels[ bestIndex ];
    }else{
        //If the sum is not greater than 1, then no class is close to any model
        maxLikelihood = 0;
        predictedClassLabel = 0;
    }
    
    return true;
}

bool HMM::predict_(MatrixFloat &timeseries){
    
    switch( hmmType ){
        case HMM_DISCRETE:
        return predict_discrete( timeseries );
        break;
        case HMM_CONTINUOUS:
        return predict_continuous( timeseries );
        break;
    }
    
    errorLog << __GRT_LOG__ << " Failed to predict, unknown HMM type!" << std::endl;
    
    return false;
}

bool HMM::predict_discrete(MatrixFloat &timeseries){
    
    if( !trained ){
        errorLog << __GRT_LOG__ << " The HMM classifier has not been trained!" << std::endl;
        return false;
    }
    
    if( timeseries.getNumCols() != 1 ){
        errorLog << __GRT_LOG__ << " The number of columns in the input matrix must be 1. It is: " << timeseries.getNumCols() << std::endl;
        return false;
    }
    
    //Covert the matrix Float to observations
    const UINT M = timeseries.getNumRows();
    Vector<UINT> observationSequence( M );
    
    for(UINT i=0; i<M; i++){
        observationSequence[i] = (UINT)timeseries[i][0];
        
        if( observationSequence[i] >= numSymbols ){
            errorLog << __GRT_LOG__ << " The new observation is not a valid symbol! It should be in the range [0 numSymbols-1]" << std::endl;
            return false;
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    bestDistance = -99e+99;
    UINT bestIndex = 0;
    Float sum = 0;
    for(UINT k=0; k<numClasses; k++){
        classDistances[k] = discreteModels[k].predict( observationSequence );
        
        //Set the class likelihood as the antilog of the class distances
        classLikelihoods[k] = grt_antilog( classDistances[k] );
        
        //The loglikelihood values are negative so we want the values closest to 0
        if( classDistances[k] > bestDistance ){
            bestDistance = classDistances[k];
            bestIndex = k;
        }
        
        sum += classLikelihoods[k];
    }
    
    //Turn the class distances into proper likelihoods
    for(UINT k=0; k<numClasses; k++){
        classLikelihoods[k] /= sum;
    }
    
    maxLikelihood = classLikelihoods[ bestIndex ];
    predictedClassLabel = classLabels[ bestIndex ];
    
    if( useNullRejection ){
        if( maxLikelihood > nullRejectionThresholds[ bestIndex ] ){
            predictedClassLabel = classLabels[ bestIndex ];
        }else predictedClassLabel = GRT_DEFAULT_NULL_CLASS_LABEL;
    }
    
    return true;
}


bool HMM::predict_continuous(MatrixFloat &timeseries){
    
    if( !trained ){
        errorLog << __GRT_LOG__ << " The HMM classifier has not been trained!" << std::endl;
        return false;
    }
    
    if( timeseries.getNumCols() != numInputDimensions ){
        errorLog << __GRT_LOG__ << " The number of columns in the input matrix (" << timeseries.getNumCols() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    //Scale the input vector if needed
    if( useScaling ){
        const UINT timeseriesLength = timeseries.getNumRows();
        for(UINT j=0; j<numInputDimensions; j++){
            for(UINT i=0; i<timeseriesLength; i++){
                timeseries[i][j] = scale(timeseries[i][j], ranges[j].minValue, ranges[j].maxValue, 0, 1);
            }
        }
    }
    
    if( classLikelihoods.getSize() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.getSize() != numClasses ) classDistances.resize(numClasses,0);
    
    std::fill(classLikelihoods.begin(),classLikelihoods.end(),0);
    std::fill(classDistances.begin(),classDistances.end(),0);
    
    bestDistance = -1000;
    UINT bestIndex = 0;
    Float minValue = -1000;
    
    const UINT numModels = (UINT)continuousModels.size();
    Vector< IndexedDouble > results(numModels);
    for(UINT i=0; i<numModels; i++){
        
        //Run the prediction for this model
        if( continuousModels[i].predict_( timeseries ) ){
            results[i].value = continuousModels[i].getLoglikelihood();
            results[i].index = continuousModels[i].getClassLabel();
        }else{
            errorLog << __GRT_LOG__ << " Prediction failed for model: " << i << std::endl;
            return false;
        }
        
        if( results[i].value < minValue ){
            minValue = results[i].value;
        }
        
        if( results[i].value > bestDistance ){
            bestDistance = results[i].value;
            bestIndex = i;
        }
    }
    
    //Store the phase from the best model
    phase = continuousModels[ bestIndex ].getPhase();
    
    //Sort the results
    std::sort(results.begin(),results.end(),IndexedDouble::sortIndexedDoubleByValueDescending);
    
    //Run the majority vote
    const Float committeeWeight = 1.0 / committeeSize;
    for(UINT i=0; i<committeeSize; i++){
        classDistances[ getClassLabelIndexValue( results[i].index ) ] += Util::scale(results[i].value, -1000, 0, 0, committeeWeight, true);
    }
    
    //Turn the class distances into likelihoods
    Float sum = Util::sum(classDistances);
    if( sum > 0 ){
        for(UINT k=0; k<numClasses; k++){
            classLikelihoods[k] = classDistances[k] / sum;
        }
        
        //Find the maximum label
        for(UINT k=0; k<numClasses; k++){
            if( classDistances[k] > bestDistance ){
                bestDistance = classDistances[k];
                bestIndex = k;
            }
        }
        
        maxLikelihood = classLikelihoods[ bestIndex ];
        predictedClassLabel = classLabels[ bestIndex ];
    }else{
        //If the sum is not greater than 1, then no class is close to any model
        maxLikelihood = 0;
        predictedClassLabel = 0;
    }
    return true;
}

bool HMM::reset(){
    
    //Reset the base class
    Classifier::reset();
    
    switch( hmmType ){
        case HMM_DISCRETE:
        for(size_t i=0; i<discreteModels.size(); i++){
            discreteModels[i].reset();
        }
        break;
        case HMM_CONTINUOUS:
        for(size_t i=0; i<continuousModels.size(); i++){
            continuousModels[i].reset();
        }
        break;
    }
    
    return true;
}

bool HMM::clear(){
    
    //Clear the base class
    Classifier::clear();
    
    discreteModels.clear();
    continuousModels.clear();
    
    return true;
}

bool HMM::print() const{
    
    std::cout << "HMM Model\n";
    
    //Write the generic hmm data
    std::cout << "HmmType: " << hmmType << std::endl;
    std::cout << "ModelType: " << modelType << std::endl;
    std::cout << "Delta: " << delta << std::endl;
    
    //Write the model specific data
    switch( hmmType ){
        case HMM_DISCRETE:
        std::cout << "NumStates: " << numStates << std::endl;
        std::cout << "NumSymbols: " << numSymbols << std::endl;
        std::cout << "NumRandomTrainingIterations: " << numRandomTrainingIterations << std::endl;
        std::cout << "NumDiscreteModels: " << discreteModels.getSize() << std::endl;
        std::cout << "DiscreteModels: " << std::endl;
        for(size_t i=0; i<discreteModels.getSize(); i++){
            if( !discreteModels[i].print() ){
                errorLog << __GRT_LOG__ << " Failed to print discrete model " << i << " to file!" << std::endl;
                return false;
            }
        }
        break;
        case HMM_CONTINUOUS:
        std::cout << "DownsampleFactor: " << downsampleFactor << std::endl;
        std::cout << "CommitteeSize: " << committeeSize << std::endl;
        std::cout << "Sigma: " << sigma << std::endl;
        std::cout << "AutoEstimateSigma: " << autoEstimateSigma << std::endl;
        std::cout << "NumContinuousModels: " << continuousModels.getSize() << std::endl;
        std::cout << "ContinuousModels: " << std::endl;
        for(size_t i=0; i<continuousModels.getSize(); i++){
            if( !continuousModels[i].print() ){
                errorLog << __GRT_LOG__ << " Failed to print continuous model " << i << " to file!" << std::endl;
                return false;
            }
        }
        break;
    }
    
    return true;
}

bool HMM::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog << __GRT_LOG__ << " File is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "HMM_MODEL_FILE_V2.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog << __GRT_LOG__ << " Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    //Write the generic hmm data
    file << "HmmType: " << hmmType << std::endl;
    file << "ModelType: " << modelType << std::endl;
    file << "Delta: " << delta << std::endl;
    
    //Write the model specific data
    switch( hmmType ){
        case HMM_DISCRETE:
        file << "NumStates: " << numStates << std::endl;
        file << "NumSymbols: " << numSymbols << std::endl;
        file << "NumRandomTrainingIterations: " << numRandomTrainingIterations << std::endl;
        file << "NumDiscreteModels: " << discreteModels.getSize() << std::endl;
        file << "DiscreteModels: " << std::endl;
        for(size_t i=0; i<discreteModels.getSize(); i++){
            if( !discreteModels[i].save( file ) ){
                errorLog << __GRT_LOG__ << " Failed to save discrete model " << i << " to file!" << std::endl;
                return false;
            }
        }
        break;
        case HMM_CONTINUOUS:
        file << "DownsampleFactor: " << downsampleFactor << std::endl;
        file << "CommitteeSize: " << committeeSize << std::endl;
        file << "Sigma: " << sigma << std::endl;
        file << "NumContinuousModels: " << continuousModels.getSize() << std::endl;
        file << "ContinuousModels: " << std::endl;
        for(UINT i=0; i<continuousModels.getSize(); i++){
            if( !continuousModels[i].save( file ) ){
                errorLog << __GRT_LOG__ << " Failed to save continuous model " << i << " to file!" << std::endl;
                return false;
            }
        }
        break;
    }
    
    return true;
}

bool HMM::load( std::fstream &file ){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << __GRT_LOG__ << " File is not open!" << std::endl;
        return false;
    }
    
    std::string word;
    UINT numModels = 0;
    
    file >> word;
    
    //Find the file type header
    if(word != "HMM_MODEL_FILE_V2.0"){
        errorLog << __GRT_LOG__ << " Could not find Model File Header!" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << __GRT_LOG__ << " Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    //Load the generic hmm data
    file >> word;
    if(word != "HmmType:"){
        errorLog << __GRT_LOG__ << " Could not find HmmType." << std::endl;
        return false;
    }
    file >> hmmType;
    
    file >> word;
    if(word != "ModelType:"){
        errorLog << __GRT_LOG__ << " Could not find ModelType." << std::endl;
        return false;
    }
    file >> modelType;
    
    file >> word;
    if(word != "Delta:"){
        errorLog << __GRT_LOG__ << " Could not find Delta." << std::endl;
        return false;
    }
    file >> delta;
    
    //Load the model specific data
    switch( hmmType ){
        case HMM_DISCRETE:
        
        file >> word;
        if(word != "NumStates:"){
            errorLog << __GRT_LOG__ << " Could not find NumStates." << std::endl;
            return false;
        }
        file >> numStates;
        
        file >> word;
        if(word != "NumSymbols:"){
            errorLog << __GRT_LOG__ << " Could not find NumSymbols." << std::endl;
            return false;
        }
        file >> numSymbols;
        
        file >> word;
        if(word != "NumRandomTrainingIterations:"){
            errorLog << __GRT_LOG__ << " Could not find NumRandomTrainingIterations." << std::endl;
            return false;
        }
        file >> numRandomTrainingIterations;
        
        file >> word;
        if(word != "NumDiscreteModels:"){
            errorLog <<  __GRT_LOG__ << " Could not find NumDiscreteModels." << std::endl;
            return false;
        }
        file >> numModels;
        
        file >> word;
        if(word != "DiscreteModels:"){
            errorLog << __GRT_LOG__ << " Could not find DiscreteModels." << std::endl;
            return false;
        }
        
        if( numModels > 0 ){
            discreteModels.resize(numModels);
            for(size_t i=0; i<discreteModels.getSize(); i++){
                if( !discreteModels[i].load( file ) ){
                    errorLog << __GRT_LOG__ << " Failed to load discrete model " << i << " from file!" << std::endl;
                    return false;
                }
            }
        }
        break;
        case HMM_CONTINUOUS:
        
        file >> word;
        if(word != "DownsampleFactor:"){
            errorLog << __GRT_LOG__ << " Could not find DownsampleFactor." << std::endl;
            return false;
        }
        file >> downsampleFactor;
        
        file >> word;
        if(word != "CommitteeSize:"){
            errorLog << __GRT_LOG__ << "  Could not find CommitteeSize." << std::endl;
            return false;
        }
        file >> committeeSize;
        
        file >> word;
        if(word != "Sigma:"){
            errorLog << __GRT_LOG__ << " Could not find Sigma." << std::endl;
            return false;
        }
        file >> sigma;
        
        file >> word;
        if(word != "NumContinuousModels:"){
            errorLog << __GRT_LOG__ << " Could not find NumContinuousModels." << std::endl;
            return false;
        }
        file >> numModels;
        
        file >> word;
        if(word != "ContinuousModels:"){
            errorLog << __GRT_LOG__ << " Could not find ContinuousModels." << std::endl;
            return false;
        }
        
        if( numModels > 0 ){
            continuousModels.resize(numModels);
            for(size_t i=0; i<continuousModels.getSize(); i++){
                if( !continuousModels[i].load( file ) ){
                    errorLog << __GRT_LOG__ << " Failed to load continuous model " << i << " from file!" << std::endl;
                    return false;
                }
            }
        }
        break;
    }
    
    return true;
    
}

bool HMM::convertDataToObservationSequence( TimeSeriesClassificationData &classData, Vector< Vector< UINT > > &observationSequences ){
    
    observationSequences.resize( classData.getNumSamples() );
    
    for(UINT i=0; i<classData.getNumSamples(); i++){
        MatrixFloat &timeseries = classData[i].getData();
        observationSequences[i].resize( timeseries.getNumRows() );
        for(UINT j=0; j<timeseries.getNumRows(); j++){
            if( timeseries[j][0] >= numSymbols ){
                errorLog << __GRT_LOG__ << " Found an observation sequence with a value outside of the symbol range! Value: " << timeseries[j][0] << std::endl;
                return false;
            }
            observationSequences[i][j] = (UINT)timeseries[j][0];
        }
    }
    
    return true;
}

UINT HMM::getHMMType() const{
    return hmmType;
}

UINT HMM::getModelType() const{
    return modelType;
}

UINT HMM::getDelta() const{
    return delta;
}

UINT HMM::getNumStates() const{
    return numStates;
}

UINT HMM::getNumSymbols() const{
    return numSymbols;
}

UINT HMM::getNumRandomTrainingIterations() const{
    return numRandomTrainingIterations;
}

Vector< DiscreteHiddenMarkovModel > HMM::getDiscreteModels() const{
    return discreteModels;
}

Vector< ContinuousHiddenMarkovModel > HMM::getContinuousModels() const{
    return continuousModels;
}

bool HMM::setHMMType(const UINT hmmType){
    
    clear();
    
    if( hmmType == HMM_DISCRETE || hmmType == HMM_CONTINUOUS ){
        this->hmmType = hmmType;
        return true;
    }
    
    warningLog << __GRT_LOG__ << " Unknown HMM type!" << std::endl;
    return false;
}

bool HMM::setModelType(const UINT modelType){
    
    clear();
    
    if( modelType == HMM_ERGODIC || modelType == HMM_LEFTRIGHT ){
        this->modelType = modelType;
        return true;
    }
    
    warningLog << __GRT_LOG__ << " Unknown model type!" << std::endl;
    return false;
}

bool HMM::setDelta(const UINT delta){
    
    clear();
    
    if( delta > 0 ){
        this->delta = delta;
        return true;
    }
    
    warningLog << __GRT_LOG__ << " Delta must be greater than zero!" << std::endl;
    return false;
}

bool HMM::setDownsampleFactor(const UINT downsampleFactor){
    
    clear();
    if( downsampleFactor > 0 ){
        this->downsampleFactor = downsampleFactor;
        return true;
    }
    return false;
}

bool HMM::setCommitteeSize(const UINT committeeSize){
    
    if( committeeSize > 0 ){
        this->committeeSize = committeeSize;
        return true;
    }
    
    return false;
}

bool HMM::setNumStates(const UINT numStates){
    
    clear();
    
    if( numStates > 0 ){
        this->numStates = numStates;
        return true;
    }
    
    warningLog << __GRT_LOG__ << " Num states must be greater than zero!" << std::endl;
    return false;
}

bool HMM::setNumSymbols(const UINT numSymbols){
    
    clear();
    
    if( numSymbols > 0 ){
        this->numSymbols = numSymbols;
        return true;
    }
    
    warningLog << __GRT_LOG__ << " Num symbols must be greater than zero!" << std::endl;
    return false;
}

bool HMM::setNumRandomTrainingIterations(const UINT numRandomTrainingIterations){
    
    clear();
    
    if( numRandomTrainingIterations > 0 ){
        this->numRandomTrainingIterations = numRandomTrainingIterations;
        return true;
    }
    
    warningLog << __GRT_LOG__ << " The number of random training iterations must be greater than zero!" << std::endl;
    return false;
}

bool HMM::setSigma(const Float sigma){
    if( sigma > 0 ){
        this->sigma = sigma;
        for(UINT i=0; i<continuousModels.getSize(); i++){
            continuousModels[i].setSigma( sigma );
        }
        return true;
    }
    return false;
}

bool HMM::setAutoEstimateSigma(const bool autoEstimateSigma){
    
    clear();
    
    this->autoEstimateSigma = autoEstimateSigma;
    
    return true;
}

GRT_END_NAMESPACE
