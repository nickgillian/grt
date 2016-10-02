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
#include "FiniteStateMachine.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string FiniteStateMachine::id = "FiniteStateMachine";
std::string FiniteStateMachine::getId() { return FiniteStateMachine::id; }

//Register the FiniteStateMachine module with the Classifier base class
RegisterClassifierModule< FiniteStateMachine > FiniteStateMachine::registerModule( getId() );

FiniteStateMachine::FiniteStateMachine(const UINT numParticles,const UINT numClustersPerState,const Float stateTransitionSmoothingCoeff,const Float measurementNoise) : Classifier( FiniteStateMachine::getId() )
{
    this->numParticles = numParticles;
    this->numClustersPerState = numClustersPerState;
    this->stateTransitionSmoothingCoeff = stateTransitionSmoothingCoeff;
    this->measurementNoise = measurementNoise;
    classifierMode = TIMESERIES_CLASSIFIER_MODE;
    
    //Set the learning settings that will be used to build the KMeans model
    minChange = 1.0e-5;
    minNumEpochs = 0;
    maxNumEpochs = 1000;
}

FiniteStateMachine::FiniteStateMachine(const FiniteStateMachine &rhs) : Classifier( FiniteStateMachine::getId() )
{
    classifierMode = STANDARD_CLASSIFIER_MODE;
    *this = rhs;
}

FiniteStateMachine::~FiniteStateMachine(void)
{
}

FiniteStateMachine& FiniteStateMachine::operator=(const FiniteStateMachine &rhs){
    if( this != &rhs ){
        
        this->clear();
        
        //Classifier variables
        copyBaseVariables( (Classifier*)&rhs );
        
        //Copy the FiniteStateMachine variable
        this->numParticles = rhs.numParticles;
        this->numClustersPerState = rhs.numClustersPerState;
        this->stateTransitionSmoothingCoeff = rhs.stateTransitionSmoothingCoeff;
        this->measurementNoise = rhs.measurementNoise;
        this->particles = rhs.particles;
        this->stateTransitions = rhs.stateTransitions;
        this->stateEmissions = rhs.stateEmissions;
        
        if( rhs.trained ){
            this->initParticles();
        }
    }
    return *this;
}

bool FiniteStateMachine::deepCopyFrom(const Classifier *classifier){
    
    if( classifier == NULL ) return false;
    
    if( this->getClassifierType() == classifier->getClassifierType() ){
        
        FiniteStateMachine *ptr = (FiniteStateMachine*)classifier;
        
        this->clear();
        
        //Clone the classifier variables
        if( !copyBaseVariables( classifier ) ){
            errorLog << "deepCopyFrom(const Classifier *classifier) - Failed to deep copy classifier base class!" << std::endl;
            return false;
        }
        
        this->numParticles = ptr->numParticles;
        this->numClustersPerState = ptr->numClustersPerState;
        this->stateTransitionSmoothingCoeff = ptr->stateTransitionSmoothingCoeff;
        this->measurementNoise = ptr->measurementNoise;
        this->particles = ptr->particles;
        this->stateTransitions = ptr->stateTransitions;
        this->stateEmissions = ptr->stateEmissions;
        
        if( ptr->trained ){
            this->initParticles();
        }
        
        return true;
    }
    return false;
}

bool FiniteStateMachine::train_( ClassificationData &trainingData ){
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumDimensions();
    
    if( M == 0 ){
        errorLog << "train_(ClassificationData &trainingData) - Training data has zero samples!" << std::endl;
        clear();
        return false;
    }
    
    //Convert the classification data into a continuous time stream
    TimeSeriesClassificationDataStream timeseries;
    timeseries.setNumDimensions( N );
    
    for(unsigned int i=0; i<M; i++){
        timeseries.addSample(trainingData[i].getClassLabel(), trainingData[i].getSample());
    }
    
    //Train the particle filter
    if( !train_( timeseries ) ){
        clear();
        errorLog << "train_(ClassificationData &trainingData) - Failed to train particle filter!" << std::endl;
        return false;
    }
    
    return true;
}

bool FiniteStateMachine::train_( TimeSeriesClassificationData &trainingData ){
    
    const unsigned int M = trainingData.getNumSamples();
    const unsigned int N = trainingData.getNumDimensions();
    
    if( M == 0 ){
        errorLog << "train_(TimeSeriesClassificationData &trainingData) - Training data has zero samples!" << std::endl;
        clear();
        return false;
    }
    
    //Convert the timeseries classification data into a continuous time stream
    TimeSeriesClassificationDataStream timeseries;
    timeseries.setNumDimensions( N );
    
    for(unsigned int i=0; i<M; i++){
        for(unsigned int j=0; j<trainingData[i].getLength(); j++){
            timeseries.addSample(trainingData[i].getClassLabel(), trainingData[i].getData().getRow(j));
        }
    }
    
    //Train the particle filter
    if( !train_( timeseries ) ){
        clear();
        errorLog << "train_(TimeSeriesClassificationData &trainingData) - Failed to train particle filter!" << std::endl;
        return false;
    }
    
    return true;
}

bool FiniteStateMachine::train_( TimeSeriesClassificationDataStream &data ){
    
    //Clear any previous model
    clear();
    
    const UINT M = data.getNumSamples();
    const UINT N = data.getNumDimensions();
    const UINT K = data.getNumClasses();
    
    if( M == 0 ){
        return false;
    }
    
    numInputDimensions = N;
    numClasses = K;
    ranges = data.getRanges();
    
    //Scale the training data if needed
    if( useScaling ){
        //Scale the training data between 0 and 1
        data.scale(0, 1);
    }
    
    stateTransitions.resize(K, K);
    stateTransitions.setAllValues(0);
    
    //Get a copy of the class labels, each class label is a state
    classLabels = data.getClassLabels();
    
    //Count how many times we transitiion from one state to another
    UINT lastStateIndex = getClassLabelIndexValue( data[0].getClassLabel() );
    UINT currentStateIndex = 0;
    for(UINT i=1; i<data.getNumSamples(); i++){
        currentStateIndex = getClassLabelIndexValue( data[i].getClassLabel() );
        stateTransitions[ lastStateIndex ][ currentStateIndex ]++;
        lastStateIndex = currentStateIndex;
    }
    
    //Normalize the state transitions
    Float sum = 0;
    for(UINT i=0; i<K; i++){
        sum = 0;
        for(UINT j=0; j<K; j++){
            sum += stateTransitions[i][j] + stateTransitionSmoothingCoeff;
        }
        for(UINT j=0; j<K; j++){
            stateTransitions[i][j] /= sum;
        }
    }
    
    //Build the state emissions model for each state
    for(UINT k=0; k<K; k++){
        
        //Get the data that belongs to the current state
        MatrixFloat classData;
        for(UINT i=0; i<M; i++){
            if( data[i].getClassLabel() == classLabels[k] ){
                classData.push_back( data[i].getSample() );
            }
        }
        
        //Make sure there are enough training samples to support the numClustersPerState
        if( classData.getNumRows() < numClustersPerState ){
            errorLog << "train_(TimeSeriesClassificationDataStream &trainingData) - There are not enough samples in state " << classLabels[k] << "! You should reduce the numClustersPerState to: " << classData.getNumRows() << std::endl;
            clear();
            return false;
        }
        
        //Use KMeans to find a clusters within the state data
        KMeans kmeans;
        kmeans.setNumClusters( numClustersPerState );
        kmeans.setMinChange( minChange );
        kmeans.setMinNumEpochs( minNumEpochs );
        kmeans.setMaxNumEpochs( maxNumEpochs );
        
        if( !kmeans.train_( classData ) ){
            errorLog << "train_(TimeSeriesClassificationDataStream &trainingData) - Failed to train kmeans cluster for class k: " << classLabels[k] << std::endl;
                clear();
            return false;
        }
        
        //Add the clusters for this state to the stateEmissions vector
        stateEmissions.push_back( kmeans.getClusters() );
    }
    
    //Flag the model is trained
    trained = true;
    
    //Init the particles
    initParticles();
    
    //Reset the particles to random starting states
    reset();
    
    print();
    
    return true;
}

bool FiniteStateMachine::predict_(VectorFloat &inputVector){
    
    if( !trained ){
        errorLog << "predict_(VectorFloat &inputVector) - Model Not Trained!" << std::endl;
        return false;
    }
    
    predictedClassLabel = 0;
    maxLikelihood = -10000;
    
    if( inputVector.size() != numInputDimensions ){
        errorLog << "predict_(VectorFloat &inputVector) - The size of the input vector (" << inputVector.size() << ") does not match the num features in the model (" << numInputDimensions << std::endl;
        return false;
    }
    
    if( useScaling ){
        for(UINT n=0; n<numInputDimensions; n++){
            inputVector[n] = scale(inputVector[n], ranges[n].minValue, ranges[n].maxValue, 0, 1);
        }
    }
    
    if( classLikelihoods.size() != numClasses ) classLikelihoods.resize(numClasses,0);
    if( classDistances.size() != numClasses ) classDistances.resize(numClasses,0);
    
    std::fill(classLikelihoods.begin(),classLikelihoods.end(),0);
    std::fill(classDistances.begin(),classDistances.end(),0);
    
    //Update the particle filter
    particles.filter( inputVector );
    
    //Compute the state estimation
    Float sum = 0;
    for(UINT i=0; i<numParticles; i++){
        sum += particles[i].w;
        classLikelihoods[ particles[i].currentState ] += particles[i].w;
        classDistances[ particles[i].currentState ] += particles[i].w;
    }
    
    //Normalize the class likelihoods
    maxLikelihood = 0;
    predictedClassLabel = 0;
    for(UINT i=0; i<numClasses; i++){
        classLikelihoods[ i ] /= sum;
        
        if( classLikelihoods[i] > maxLikelihood ){
            maxLikelihood = classLikelihoods[i];
            predictedClassLabel = classLabels[i];
        }
    }
    
    return true;
}

bool FiniteStateMachine::reset(){
    
    if( trained ){
        //Randomize the particles starting states
        for(UINT i=0; i<numParticles; i++){
            particles[i].currentState = random.getRandomNumberInt(0, numClasses);
            particles[i].w = 0;
        }
    }
    
    return true;
}

bool FiniteStateMachine::clear(){
    
    //Clear the Classifier variables
    Classifier::clear();
    
    //Clear the model
    stateTransitions.clear();
    stateEmissions.clear();
    pt.clear();
    pe.clear();
    particles.clear();
    
    return true;
}

bool FiniteStateMachine::print() const {
    
    infoLog << "FiniteStateMachineModel" << std::endl;
    infoLog << "NumParticles: " << numParticles << std::endl;
    infoLog << "NumClustersPerState: " << numClustersPerState << std::endl;
    infoLog << "StateTransitionSmoothingCoeff: " << stateTransitionSmoothingCoeff << std::endl;
    
    if( trained ){
        infoLog << "StateTransitions: " << std::endl;
        for(unsigned int i=0; i<stateTransitions.getNumRows(); i++){
            for(unsigned int j=0; j<stateTransitions.getNumCols(); j++){
                infoLog << stateTransitions[i][j] << " ";
            }
            infoLog << std::endl;
        }
        
        infoLog << "StateEmissions: " << std::endl;
        for(unsigned int k=0; k<stateEmissions.size(); k++){
            for(unsigned int i=0; i<stateEmissions[k].getNumRows(); i++){
                for(unsigned int j=0; j<stateEmissions[k].getNumCols(); j++){
                    infoLog << stateEmissions[k][i][j] << " ";
                }
                infoLog << std::endl;
            }
        }
    }
    
    return true;
}

bool FiniteStateMachine::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }
    
    //Write the header info
    file << "GRT_FSM_MODEL_FILE_V1.0\n";
    
    //Write the classifier settings to the file
    if( !Classifier::saveBaseSettingsToFile(file) ){
        errorLog << "save(fstream &file) - Failed to save classifier base settings to file!" << std::endl;
        return false;
    }
    
    file << "NumParticles: " << numParticles << std::endl;
    file << "NumClustersPerState: " << numClustersPerState << std::endl;
    file << "StateTransitionSmoothingCoeff: " << stateTransitionSmoothingCoeff << std::endl;
    
    if( trained ){
        file << "StateTransitions:" << std::endl;
        for(unsigned int i=0; i<stateTransitions.getNumRows(); i++){
            for(unsigned int j=0; j<stateTransitions.getNumCols(); j++){
                file << stateTransitions[i][j] << " ";
            }
            file << std::endl;
        }
        
        file << "StateEmissions:" << std::endl;
        for(unsigned int k=0; k<numClasses; k++){
            for(unsigned int i=0; i<stateEmissions[k].getNumRows(); i++){
                for(unsigned int j=0; j<stateEmissions[k].getNumCols(); j++){
                    file << stateEmissions[k][i][j] << " ";
                }
                file << std::endl;
            }
        }
        
        if( !useScaling ){
            file << "Ranges: " << std::endl;
            for(UINT i=0; i<ranges.size(); i++){
                file << ranges[i].minValue << "\t" << ranges[i].maxValue << std::endl;
            }
        }
    }
    
    return true;
}

bool FiniteStateMachine::load( std::fstream &file ){
    
    //Clear any previous model
    clear();
    
    if(!file.is_open())
    {
        errorLog << "load(string filename) - Could not open file to load model" << std::endl;
        return false;
    }
    
    std::string word;
    
    //Find the file type header
    file >> word;
    if( word != "GRT_FSM_MODEL_FILE_V1.0" ){
        errorLog << "load(string filename) - Could not find Model File Header" << std::endl;
        return false;
    }
    
    //Load the base settings from the file
    if( !Classifier::loadBaseSettingsFromFile(file) ){
        errorLog << "load(string filename) - Failed to load base settings from file!" << std::endl;
        return false;
    }
    
    //Find the NumParticles header
    file >> word;
    if( word != "NumParticles:" ){
        errorLog << "load(string filename) - Could not find NumParticles Header" << std::endl;
        return false;
    }
    file >> numParticles;
    
    //Find the NumClustersPerState header
    file >> word;
    if( word != "NumClustersPerState:" ){
        errorLog << "load(string filename) - Could not find NumClustersPerState Header" << std::endl;
        return false;
    }
    file >> numClustersPerState;
    
    //Find the StateTransitionSmoothingCoeff header
    file >> word;
    if( word != "StateTransitionSmoothingCoeff:" ){
        errorLog << "load(string filename) - Could not find stateTransitionSmoothingCoeff Header" << std::endl;
        return false;
    }
    file >> stateTransitionSmoothingCoeff;
    
    if( trained ){
        
        //Find the StateTransitions header
        file >> word;
        if( word != "StateTransitions:" ){
            errorLog << "load(string filename) - Could not find StateTransitions Header" << std::endl;
            return false;
        }
        stateTransitions.resize(numClasses, numClasses);
        
        for(unsigned int i=0; i<stateTransitions.getNumRows(); i++){
            for(unsigned int j=0; j<stateTransitions.getNumCols(); j++){
                file >> stateTransitions[i][j];
            }
        }
        
        //Find the StateEmissions header
        file >> word;
        if( word != "StateEmissions:" ){
            errorLog << "load(string filename) - Could not find StateEmissions Header" << std::endl;
            return false;
        }
        stateEmissions.resize( numClasses );
        
        for(unsigned int k=0; k<numClasses; k++){
            stateEmissions[k].resize( numClustersPerState, numInputDimensions );
            for(unsigned int i=0; i<stateEmissions[k].getNumRows(); i++){
                for(unsigned int j=0; j<stateEmissions[k].getNumCols(); j++){
                    file >> stateEmissions[k][i][j];
                }
            }
        }
        
        if( !useScaling ){
            //Load if the Ranges
            file >> word;
            if( word != "Ranges:" ){
                errorLog << "load(string filename) - Failed to read Ranges header!" << std::endl;
                clear();
                return false;
            }
            ranges.resize(numInputDimensions);
            
            for(UINT i=0; i<ranges.size(); i++){
                file >> ranges[i].minValue;
                file >> ranges[i].maxValue;
            }
        }
        
        initParticles();
    }
    
    return true;
}

bool FiniteStateMachine::recomputePT(){
    
    if( !trained ){
        warningLog << "recomputePT() - Failed to init particles, the model has not been trained!" << std::endl;
        return false;
    }
    
    pt.clear();
    
    //Build pt, this is simply the state transitions formated as indexed Floats to make the prediction stage more efficient
    const UINT K = stateTransitions.getNumRows();
    for(UINT i=0; i<K; i++){
        Vector< IndexedDouble > model(K);
        for(UINT j=0; j<K; j++){
            model[j].index = j;
            model[j].value = stateTransitions[i][j];
        }
        pt.push_back( model );
    }
    
    return true;
}

bool FiniteStateMachine::recomputePE(){
    
    if( !trained ){
        warningLog << "recomputePE() - Failed to init particles, the model has not been trained!" << std::endl;
        return false;
    }
    
    pe.clear();
    
    const UINT K = stateEmissions.getSize();
    
    //Run over each state (k)
    for(UINT k=0; k<K; k++){
        
        //For each state, convert the Matrix of emissions data to a vector of vectors (this format is more efficient for the particle filter)
        Vector< VectorFloat > model;
        model.reserve( numClustersPerState );
        for(UINT i=0; i<stateEmissions[k].getNumRows(); i++){
            model.push_back( stateEmissions[k].getRow(i) );
        }
        
        pe.push_back( model );
    }
    
    return true;
}

bool FiniteStateMachine::initParticles(){
    
    if( !trained ){
        warningLog << "initParticles() - Failed to init particles, the model has not been trained!" << std::endl;
        return false;
    }
    
    //Init the particles
    Vector< VectorFloat > initModel( numInputDimensions, VectorFloat(2,0) );
    VectorFloat initProcessNoise( numInputDimensions, 0 ); //Process noise is ignored for the FSM particle filter
    VectorFloat initMeasurementNoise( numInputDimensions, 0 );
    
    //Setup the init model
    for(unsigned int i=0; i<numInputDimensions; i++){
        initModel[i][0] = useScaling ? 0 : ranges[i].minValue;
        initModel[i][1] = useScaling ? 1 : ranges[i].maxValue;
    }
    
    //Set the measurement noise
    for(unsigned int i=0; i<numInputDimensions; i++){
        initMeasurementNoise[i] = measurementNoise;
    }
    
    particles.init(numParticles, initModel, initProcessNoise, initMeasurementNoise);
    
    recomputePT();
    recomputePE();
    
    //Set the lookup table references
    particles.setLookupTables( pt, pe );
    
    //Reset the particles
    reset();
    
    return true;
}

bool FiniteStateMachine::setNumParticles(const UINT numParticles){
    
    clear();
    
    this->numParticles = numParticles;
    
    return true;
}

bool FiniteStateMachine::setNumClustersPerState(const UINT numClustersPerState){
    
    clear();
    
    this->numClustersPerState = numClustersPerState;
    
    return true;
}

bool FiniteStateMachine::setStateTransitionSmoothingCoeff(const Float stateTransitionSmoothingCoeff){
    
    clear();
    
    this->stateTransitionSmoothingCoeff = stateTransitionSmoothingCoeff;
    
    return true;
}

bool FiniteStateMachine::setMeasurementNoise(const Float measurementNoise){
    
    clear();
    
    this->measurementNoise = measurementNoise;
    
    return true;
}

GRT_END_NAMESPACE
