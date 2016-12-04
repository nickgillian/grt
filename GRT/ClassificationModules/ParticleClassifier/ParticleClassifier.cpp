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
#include "ParticleClassifier.h"

GRT_BEGIN_NAMESPACE
    
//Define the string that will be used to identify the object
const std::string ParticleClassifier::id = "ParticleClassifier";
std::string ParticleClassifier::getId() { return ParticleClassifier::id; }

//Register the ParticleClassifier module with the Classifier base class
RegisterClassifierModule< ParticleClassifier > ParticleClassifier::registerModule( ParticleClassifier::getId() );

ParticleClassifier::ParticleClassifier( const unsigned int numParticles,const Float sensorNoise,const Float transitionSigma,const Float phaseSigma,const Float velocitySigma ) : Classifier( ParticleClassifier::getId() )
{
    this->numParticles = numParticles;
    this->sensorNoise = sensorNoise;
    this->transitionSigma = transitionSigma;
    this->phaseSigma = phaseSigma;
    this->velocitySigma = velocitySigma;
    useNullRejection = true;
    supportsNullRejection = true;
    classifierMode = TIMESERIES_CLASSIFIER_MODE;
}

ParticleClassifier::ParticleClassifier(const ParticleClassifier &rhs) : Classifier( ParticleClassifier::getId() )
{
    *this = rhs;
}

ParticleClassifier::~ParticleClassifier(void)
{
}

ParticleClassifier& ParticleClassifier::operator=(const ParticleClassifier &rhs){
	
	if( this != &rhs ){
        
        this->numParticles = rhs.numParticles;
        this->sensorNoise = rhs.sensorNoise;
        this->transitionSigma = rhs.transitionSigma;
        this->phaseSigma = rhs.phaseSigma;
        this->velocitySigma = rhs.velocitySigma;
        this->particleFilter = rhs.particleFilter;
        
	    //Copy the classifier variables
		copyBaseVariables( (Classifier*)&rhs );
	}
	return *this;
}

bool ParticleClassifier::deepCopyFrom(const Classifier *classifier){
    if( classifier == NULL ) return false;
    
    if( this->getId() == classifier->getId() ){
        const ParticleClassifier *ptr = dynamic_cast<const ParticleClassifier*>(classifier);
        
        this->numParticles = ptr->numParticles;
        this->sensorNoise = ptr->sensorNoise;
        this->transitionSigma = ptr->transitionSigma;
        this->phaseSigma = ptr->phaseSigma;
        this->velocitySigma = ptr->velocitySigma;
        this->particleFilter = ptr->particleFilter;
        
	    //Copy the classifier variables
		return copyBaseVariables( classifier );
    }
    return false;
}

bool ParticleClassifier::train_(TimeSeriesClassificationData &trainingData){
    
    clear();
    
    numClasses = trainingData.getNumClasses();
    numInputDimensions = trainingData.getNumDimensions();
    ranges = trainingData.getRanges();
    
    //Scale the training data if needed
    if( useScaling ){
        trainingData.scale(0, 1);
    }
    
    //Train the particle filter
    particleFilter.train( numParticles, trainingData, sensorNoise, transitionSigma, phaseSigma, velocitySigma );
    
    classLabels.resize(numClasses);
    classLikelihoods.resize(numClasses,0);
    classDistances.resize(numClasses,0);
    
    for(unsigned int i=0; i<numClasses; i++){
        classLabels[i] = trainingData.getClassTracker()[i].classLabel;
    }
    
    trained = true;
  
    return trained;
}

bool ParticleClassifier::predict_( VectorDouble &inputVector ){

    if( !trained ){
        errorLog << "predict_(VectorDouble &inputVector) - The model has not been trained!" << std::endl;
        return false;
    }
    
    if( numInputDimensions != inputVector.size() ){
        errorLog << "predict_(VectorDouble &inputVector) - The number of features in the model " << numInputDimensions << " does not match that of the input vector " << inputVector.size() << std::endl;
        return false;
    }
    
    //Scale the input data if needed
    if( useScaling ){
        for(unsigned int j=0; j<numInputDimensions; j++){
            inputVector[j] = scale(inputVector[j],ranges[j].minValue,ranges[j].maxValue,0,1);
        }
    }
    
    predictedClassLabel = 0;
    maxLikelihood = 0;
    std::fill(classLikelihoods.begin(),classLikelihoods.end(),0);
    std::fill(classDistances.begin(),classDistances.end(),0);

    //Update the particle filter
    particleFilter.filter( inputVector );
    
    //Count the number of particles per class
    unsigned int gestureTemplate = 0;
    unsigned int gestureLabel = 0;
    unsigned int gestureIndex = 0;
    for(unsigned int i=0; i<numParticles; i++){
        gestureTemplate = (unsigned int)particleFilter[i].x[0]; //The first element in the state vector is the gesture template index
        gestureLabel = particleFilter.gestureTemplates[ gestureTemplate ].classLabel;
        gestureIndex = getClassLabelIndexValue( gestureLabel );
        
        classDistances[ gestureIndex ] += particleFilter[i].w;
    }
    
    bool rejectPrediction = false;
    if( useNullRejection ){
        if( particleFilter.getWeightSum() < 1.0e-5 ){
            rejectPrediction = true;
        }
    }
    
    //Compute the class likelihoods
    for(unsigned int i=0; i<numClasses; i++){

        classLikelihoods[ i ] = rejectPrediction ? 0 : classDistances[i];

        if( classLikelihoods[i] > maxLikelihood ){
            predictedClassLabel = classLabels[i];
            maxLikelihood = classLikelihoods[i];
        }
    }
    
    //Estimate the phase
    phase = particleFilter.getStateEstimation()[1]; //The 2nd element in the state vector is the estimatied phase
    
    return true;

}

bool ParticleClassifier::clear(){
    
    Classifier::clear();
    
    particleFilter.clear();
    
    return true;
}
    
bool ParticleClassifier::reset(){
    
    Classifier::reset();
    
    particleFilter.reset();
    
    return true;
}
    
bool ParticleClassifier::save( std::fstream &file ) const{
    
    if(!file.is_open())
    {
        errorLog <<"save(fstream &file) - The file is not open!" << std::endl;
        return false;
    }

    return true;
}
    
bool ParticleClassifier::load( std::fstream &file ){
    
    clear();
    
    if(!file.is_open())
    {
        errorLog << "load(string filename) - Could not open file to load model" << std::endl;
        return false;
    }
    
    //Flag that the model is trained
    trained = true;
    
    //Resize the prediction results to make sure it is setup for realtime prediction
    maxLikelihood = DEFAULT_NULL_LIKELIHOOD_VALUE;
    bestDistance = DEFAULT_NULL_DISTANCE_VALUE;
    classLikelihoods.resize(numClasses,DEFAULT_NULL_LIKELIHOOD_VALUE);
    classDistances.resize(numClasses,DEFAULT_NULL_DISTANCE_VALUE);
    
    return true;
}
    
const Vector< ParticleClassifierGestureTemplate >& ParticleClassifier::getGestureTemplates() const{
    return particleFilter.gestureTemplates;
}
    
const ParticleClassifierParticleFilter& ParticleClassifier::getParticleFilter() const {
    return particleFilter;
}
    
VectorDouble ParticleClassifier::getStateEstimation() const{
    return particleFilter.getStateEstimation();
}
    
Float ParticleClassifier::getPhase() const{
    if( trained ){
        return particleFilter.getStateEstimation()[1];
    }
    return 0;
}

Float ParticleClassifier::getVelocity() const{
    if( trained ){
        return particleFilter.getStateEstimation()[2];
    }
    return 0;
}
    
bool ParticleClassifier::setNumParticles(const unsigned int numParticles){
    
    clear();
    
    this->numParticles = numParticles;
    
    return true;
}
  
bool ParticleClassifier::setSensorNoise(const unsigned int sensorNoise){
    
    clear();
    
    this->sensorNoise = sensorNoise;
    
    return true;
}

bool ParticleClassifier::setTransitionSigma(const unsigned int transitionSigma){
    
    clear();
    
    this->transitionSigma = transitionSigma;
    
    return true;
}

bool ParticleClassifier::setPhaseSigma(const unsigned int phaseSigma){
    
    clear();
    
    this->phaseSigma = phaseSigma;
    
    return true;
}

bool ParticleClassifier::setVelocitySigma(const unsigned int velocitySigma){
    
    clear();
    
    this->velocitySigma = velocitySigma;
    
    return true;
}
    
GRT_END_NAMESPACE
