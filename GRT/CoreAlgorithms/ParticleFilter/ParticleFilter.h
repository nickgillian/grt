/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a template based ParticleFilter.  The user is required to implement the predict and update functions for their specific task.
 
 @remark This implementation is based on Gillian N. et. al., Gestures Everywhere: A Multimodal Sensor Fusion and Analysis Framework for Pervasive Displays, Pervaisve Displays, 2014
 
 */

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

#ifndef GRT_PARTICLE_FILTER_HEADER
#define GRT_PARTICLE_FILTER_HEADER

#include "Particle.h"
#include "../../Util/GRTCommon.h"

namespace GRT{

template<class PARTICLE,class SENSOR_DATA>
class ParticleFilter{
    
public:
    /**
     Default Constructor
     */
    ParticleFilter():particles(particleDistributionA){
        initialized = false;
        verbose = true;
        normWeights = true;
        initMode = INIT_MODE_UNIFORM;
        estimationMode = MEAN;
        numParticles = 0;
        stateVectorSize = 0;
        numDeadParticles = 0;
        minimumWeightThreshold = 1.0e-5;
        robustMeanWeightDistance = 0.2;
        estimationLikelihood = 0;
        warningLog.setProceedingText("[WARNING ParticleFilter]");
        errorLog.setProceedingText("[ERROR ParticleFilter]");
    }
    
    /**
     Copy Constructor
     */
    ParticleFilter(const ParticleFilter &rhs):particles(particleDistributionA){
        *this = rhs;
    }
    
    /**
     Default Destructor
     */
    virtual ~ParticleFilter(){
    }
    
    /**
     Provides direct access to the i'th particle. It is the user's responsibility to ensure i is within bounds.
     
     @param const unsigned int &i: the index of the particle you want
     @return returns a reference to the i'th particle (if i is valid)
     */
    PARTICLE& operator[](const unsigned int &i){
        return particles[i];
    }
    
    /**
     Provides direct access to the i'th particle before it was resampled. It is the user's responsibility to ensure i is within bounds.
     
     @param const unsigned int &i: the index of the particle you want
     @return returns a reference to the i'th particle (if i is valid)
     */
    PARTICLE& operator()(const unsigned int &i){
        return (&particles == &particleDistributionA ? particleDistributionB[i] : particleDistributionA[i]);
    }
    
    /**
     Defines the equals operator.
     */
    ParticleFilter& operator=(const ParticleFilter &rhs){
        if( this != &rhs ){
            this->initialized = rhs.initialized;
            this->verbose = rhs.verbose;
            this->normWeights = rhs.normWeights;
            this->numParticles= rhs.numParticles;
            this->stateVectorSize = rhs.stateVectorSize;
            this->initMode = rhs.initMode;
            this->estimationMode= rhs.estimationMode;
            this->numDeadParticles = rhs.numDeadParticles;
            this->minimumWeightThreshold = rhs.minimumWeightThreshold;
            this->robustMeanWeightDistance= rhs.robustMeanWeightDistance;
            this->estimationLikelihood = rhs.estimationLikelihood;
            this->x = rhs.x;
            this->initModel = rhs.initModel;
            this->processNoise = rhs.processNoise;
            this->measurementNoise = rhs.measurementNoise;
            this->particleDistributionA = rhs.particleDistributionA;
            this->particleDistributionB = rhs.particleDistributionB;
            if( &rhs.particles == &rhs.particleDistributionA ){
                this->particles = particleDistributionA;
            }else{
                this->particles = particleDistributionB;
            }
            this->cumsum = rhs.cumsum;
            this->warningLog = rhs.warningLog;
            this->errorLog = rhs.errorLog;
        }
        return *this;
    }
    
    /**
     Initializes the particles.
     */
    virtual bool init(const unsigned int numParticles,const vector< VectorDouble > &initModel,const VectorDouble &processNoise,const VectorDouble &measurementNoise){
        
        //Clear any previous setup
        clear();
        
        //Check to make sure the init model and process noise vectors are the same size
        if( initModel.size() != processNoise.size() ){
            errorLog << "ERROR: The number of dimensions in the initModel and processNoise vectors do not match!" << endl;
            return false;
        }
        
        //Check to make sure each vector in the initModel has 2 dimensions, these are min and max or mu and sigma depening on the init mode
        for(unsigned int i=0; i<initModel.size(); i++){
            if( initModel[i].size() != 2 ){
                errorLog << "ERROR: The " << i << " dimension of the initModel does not have 2 dimensions!" << endl;
                return false;
            }
        }
        
        stateVectorSize = (unsigned int)initModel.size();
        this->initModel = initModel;
        this->processNoise = processNoise;
        this->measurementNoise = measurementNoise;
        x.resize( stateVectorSize );
        initialized = true;
        
        if( !initParticles( numParticles ) ){
            errorLog << "ERROR: Failed to init particles!" << endl;
            clear();
            return false;
        }
        
        return true;
    }
    
    /**
     The function initializes the particles at time t=0.
     This is a virtual function, so you can override it in your derived class if needed.
     
     @return returns true if the state estimation was correctly computed, false otherwise
     */
    virtual bool initParticles( UINT numParticles ){
        
        if( !initialized ) return false;
        
        if( this->numParticles != numParticles ){
            unsigned int numDimensions = x.size();
            this->numParticles = numParticles;
            particleDistributionA.resize( numParticles, PARTICLE(numDimensions) );
            particleDistributionB.resize( numParticles, PARTICLE(numDimensions) );
            particles = particleDistributionA;
            cumsum.resize( numParticles,0 );
        }
        
        const unsigned int N = x.size();
        for(unsigned int i=0; i<numParticles; i++){
            for(unsigned int j=0; j<N; j++){
                switch( initMode ){
                    case INIT_MODE_UNIFORM:
                        particles[i].x[j] = rand.getRandomNumberUniform(initModel[j][0],initModel[j][1]);
                        break;
                    case INIT_MODE_GAUSSIAN:
                        particles[i].x[j] = initModel[j][0] + rand.getRandomNumberGauss(0,initModel[j][1]);
                        break;
                    default:
                        errorLog << "ERROR: Unknown initMode!" << endl;
                        return false;
                        break;
                }
                
            }
        }
        
        return true;
    }

    
    /**
     The main filter function that should be called at each time step.
     
     @param SENSOR_DATA &data: the template data structure containing any data you need to pass to the update function.
     @return returns true if the filter was run successfully, false otherwise
     */
    bool filter(SENSOR_DATA &data){
        
        if( !initialized ){
            errorLog << "ERROR: The particle filter has not been initialized!" << endl;
            return false;
        }
        
        //The main particle prediction loop
        for(unsigned int i=0; i<numParticles; i++){
            if( !predict( particles[i] ) ){
                errorLog << "ERROR: Particle " << i << " failed prediction!" << endl;
                return false;
            }
        }
        
        //The main particle update loop
        for(unsigned int i=0; i<numParticles; i++){
            if( !update( particles[i], data ) ){
                errorLog << "ERROR: Particle " << i << " failed update!" << endl;
                return false;
            }
        }
        
        //Normalize the particle weights so they sum to 1
        if( normWeights ){
            if( !normalizeWeights() ){
                errorLog << "ERROR: Failed to normalize particle weights! " << endl;
                return false;
            }
        }
        
        //Compute the final state estimate
        if( !computeEstimate() ){
            errorLog << "ERROR: Failed to compute the final estimat!" << endl;
            return false;
        }
        
        //Resample the particles
        if( !resample() ){
            errorLog << "ERROR: Failed to resample particles!" << endl;
            return false;
        }
        
        return true;
    }
    
    /**
     Clears any previous setup.
     
     @return returns true if the filter was cleared successfully, false otherwise
     */
    bool clear(){
        initialized = false;
        numParticles = 0;
        stateVectorSize = 0;
        estimationLikelihood = 0;
        numDeadParticles = 0;
        x.clear();
        initModel.clear();
        processNoise.clear();
        measurementNoise.clear();
        particleDistributionA.clear();
        particleDistributionB.clear();
        cumsum.clear();
        return true;
    }
    
    /**
     Gets the number of particles.
     
     @return returns the number of particles if the filter is initialized, zero otherwise
     */
    unsigned int getNumParticles() const{
        return initialized ? (unsigned int)particles.size() : 0;
    }
    
    /**
     Gets the number of dimensions in the state vector.
     
     @return returns the number of dimensions in the state vector the filter is initialized, zero otherwise
     */
    unsigned int getStateVectorSize() const{
        return initialized ? stateVectorSize : 0;
    }
    
    /**
     Gets the number of dead particles. Dead particles are particles with weights of INF.
     
     @return returns the number of dead particles
     */
    unsigned int getNumDeadParticles() const{
        return numDeadParticles;
    }
    
    /**
     Gets the current initMode.
     
     @return returns an unsigned int representing the current initMode
     */
    unsigned int getInitMode() const{
        return initMode;
    }
    
    /**
     Gets the current estimationMode.
     
     @return returns an unsigned int representing the current estimationMode
     */
    unsigned int getEstimationMode() const{
        return estimationMode;
    }
    
    /**
     Gets how sure the particle filter is about the estimated state.
     This will be a double in the range of [0 1] (or INF if the likelihood is INF).
     
     @return returns a double representing the estimation likelihood
     */
    double getEstimationLikelihood() const{
        return estimationLikelihood;
    }
    
    /**
     Gets the current state estimation vector.
     
     @return returns a double vector containing the current state estimation
     */
    VectorDouble getStateEstimation() const{
        return x;
    }
    
    /**
     Gets the process noise vector.
     
     @return returns a double vector containing the process noise
     */
    VectorDouble setProcessNoise() const{
        return processNoise;
    }
    
    /**
     Sets the verbose mode.
     
     @param const bool verbose: the new verbose mode
     @return returns true if the verboseMode was successfully updated, false otherwise
     */
    bool setVerbose(const bool verbose){
        this->verbose = verbose;
        return true;
    }
    
    /**
     Sets if the particle weights should be updated at each filter iteration.
     
     @param const bool normWeights: the new normWeights mode
     @return returns true if the normWeights was successfully updated, false otherwise
     */
    bool setNormalizeWeights(const bool normWeights){
        this->normWeights = normWeights;
        return true;
    }
    
    /**
     Sets the estimation mode. This should be one of the EstimationModes.
     
     @param const unsigned int estimationMode: the new estimation mode (must be one of the EstimationModes enums)
     @return returns true if the estimationMode was successfully updated, false otherwise
     */
    bool setEstimationMode(const unsigned int estimationMode){
        if( estimationMode == MEAN || estimationMode == WEIGHTED_MEAN ||
            estimationMode == ROBUST_MEAN || estimationMode == BEST_PARTICLE )
        {
            this->estimationMode = estimationMode;
            return true;
        }
        return false;
    }
    
    /**
     Sets the init mode. This should be one of the InitModes.
     
     @param const unsigned int initMode: the new init mode (must be one of the InitMode enums)
     @return returns true if the initMode was successfully updated, false otherwise
     */
    bool setInitMode(const unsigned int initMode){
        if( initMode == INIT_MODE_UNIFORM || initMode == INIT_MODE_GAUSSIAN )
        {
            this->initMode = initMode;
            return true;
        }
        return false;
    }
    
    /**
     Sets the init model. This is the noise model that is used for the initial starting guess when the user calls the #initParticles(UINT numParticles)
     function.
     
     The new init model vector size must match the size of the current init model vector.
     
     @param const vector< VectorDouble > initModel: a vector containing the new init model for the particle filter
     @return returns true if the initModel was successfully updated, false otherwise
     */
    bool setInitModel(const vector< VectorDouble > initModel){
        if( this->initModel.size() == initModel.size() ){
            this->initModel = initModel;
            return true;
        }
        return false;
    }
    
    /**
     Sets the process noise. The new process noise vector size must match the size of the current process noise vector.
     
     @param const VectorDouble &processNoise: a vector containing the new process noise for the particle filter
     @return returns true if the processNoise was successfully updated, false otherwise
     */
    bool setProcessNoise(const VectorDouble &processNoise){
        if( this->processNoise.size() == processNoise.size() ){
            this->processNoise = processNoise;
            return true;
        }
        return false;
    }
    
    /**
     Sets the measurement noise. The new measurement noise vector size must match the size of the current measurement noise vector.
     
     @param const VectorDouble &measurementNoise: a vector containing the new measurement noise for the particle filter
     @return returns true if the measurementNoise was successfully updated, false otherwise
     */
    bool setMeasurementNoise(const VectorDouble &measurementNoise){
        if( this->measurementNoise.size() == measurementNoise.size() ){
            this->measurementNoise = measurementNoise;
            return true;
        }
        return false;
    }
    
    /**
     The function returns the current vector of particles. These are the particles that have been resampled.
     
     @return returns a vector with the current particles
     */
    vector< PARTICLE > getParticles(){
        return (&particles == &particleDistributionA ? particleDistributionA : particleDistributionB);
    }
    
    /**
     The function returns the vector of particles before they were resampled.
     
     @return returns a vector with the old particles (i.e. before they were resampled
     */
    vector< PARTICLE > getOldParticles(){
        return (&particles == &particleDistributionA ? particleDistributionB : particleDistributionA);
    }
    
protected:
    /**
     This is the main predict function. This function must be implemented in your derived class.
     
     @param PARTICLE &p: the current particle that should be passed through your prediction code
     @return returns true if the particle prediction was updated successfully, false otherwise
     */
    virtual bool predict( PARTICLE &p ){
        errorLog << "predict( PARTICLE &p ) Prediction function not implemented!" << endl;
        return false;
    }
    
    /**
     This is the main update function in which the weight of the particle should be computed, based
     on the particles current estimated state and the latest sensor data.
     
     This function must be implemented in your derived class.
     
     @param PARTICLE &p: the current particle
     @param SENSOR_DATA &data: the current sensor data
     @return returns true if the particle update was updated successfully, false otherwise
     */
    virtual bool update( PARTICLE &p, SENSOR_DATA &data ){
        errorLog << "update( PARTICLE &p, SENSOR_DATA &data ) Update function not implemented!" << endl;
        return false;
    }
    
    /**
     This function normalizes the particle weights so they sum to 1.
     This is a virtual function, so you can override it in your derived class if needed.
     
     @return returns true if the particle weights were normalized successfully, false otherwise
     */
    virtual bool normalizeWeights(){
        
        //Compute the total particle weight
        double wNorm = 0;
        numDeadParticles = 0;
        for(unsigned int i=0; i<numParticles; i++){
            wNorm += particles[i].w;
            
            if( std::isinf( particles[i].w ) ){
                cout << "particle is inf!\n";
                numDeadParticles++;
            }
        }
        
        if( std::isnan(wNorm) ){
            if( verbose )
                warningLog << "WARNING: Weight norm is NAN!" << endl;
            return true;
        }
        
        if( std::isinf(wNorm) ){
            if( verbose )
                warningLog << "WARNING: Weight norm is INF!" << endl;
            return true;
        }
        
        //Normalized the weights so they sum to 1
        for(unsigned int i=0; i<numParticles; i++){
            particles[i].w /= wNorm;
        }

        return true;
    }
    
    /**
     The function computes the state estimate, using the current particles. The exact method
     used to compute the estimate depends on the current estimationMode.
     This is a virtual function, so you can override it in your derived class if needed.
     
     @return returns true if the state estimation was correctly computed, false otherwise
     */
    virtual bool computeEstimate(){
        
        const unsigned int N = x.size();
        unsigned int bestIndex = 0;
        unsigned int robustMeanParticleCounter = 0;
        double bestWeight = 0;
        double sum = 0;
        estimationLikelihood = 0;
        switch( estimationMode ){
            case MEAN:
                for(unsigned int j=0; j<N; j++){
                    x[j] = 0;
                }
                
                for(unsigned int i=0; i<numParticles; i++){
                    for(unsigned int j=0; j<N; j++){
                        x[j] += particles[i][j];
                    }
                    estimationLikelihood += std::isnan(particles[i].w) ? 0 : particles[i].w;
                }
                
                for(unsigned int j=0; j<N; j++){
                    x[j] /= double(numParticles);
                }
                estimationLikelihood /= double(numParticles);
                break;
            case WEIGHTED_MEAN:
                for(unsigned int j=0; j<N; j++){
                    x[j] = 0;
                    sum = 0;
                    for(unsigned int i=0; i<numParticles; i++){
                        x[j] += particles[i][j] * particles[i].w;
                        sum += particles[i].w;
                    }
                    x[j] /= sum;
                }
                
                for(unsigned int i=0; i<numParticles; i++){
                    estimationLikelihood += std::isnan(particles[i].w) ? 0 : particles[i].w;
                }
                estimationLikelihood /= double(numParticles);
                break;
            case ROBUST_MEAN:
                //Reset x
                for(unsigned int j=0; j<N; j++){
                    x[j] = 0;
                }
                sum = 0;
                
                //Find the particle with the best weight
                for(unsigned int i=0; i<numParticles; i++){
                    if( particles[i].w > bestWeight ){
                        bestWeight = particles[i].w;
                        bestIndex = i;
                    }
                }
                
                //Use all the particles within a given distance of that weight
                for(unsigned int i=0; i<numParticles; i++){
                    if( fabs( particles[i].w - particles[bestIndex].w ) <= robustMeanWeightDistance ){
                        for(unsigned int j=0; j<N; j++){
                            x[j] += particles[i][j] * particles[i].w;
                        }
                        estimationLikelihood += std::isnan(particles[i].w) ? 0 : particles[i].w;
                        sum += particles[i].w;
                        robustMeanParticleCounter++;
                    }
                }
                
                //Normalize x
                for(unsigned int j=0; j<N; j++){
                    x[j] /= sum;
                }
                estimationLikelihood /= double(robustMeanParticleCounter);
                break;
            case BEST_PARTICLE:
                for(unsigned int i=0; i<numParticles; i++){
                    if( particles[i].w > bestWeight ){
                        bestWeight = particles[i].w;
                        bestIndex = i;
                    }
                }
                x = particles[bestIndex].x;
                estimationLikelihood = std::isnan(particles[bestIndex].w) ? 0 : particles[bestIndex].w;
                break;
            default:
                errorLog << "ERROR: Unknown estimation mode!" << endl;
                return false;
                break;
        }
        
        return true;
    }
    
    /**
     The main resample function. Resamples the particles based on the particles weights.
     This is a virtual function, so you can override it in your derived class if needed.
     
     @return returns true if the particles were correctly resampled, false otherwise
     */
    virtual bool resample(){
        
        vector< PARTICLE > *tempParticles = NULL;
        
        //If the particles are currently reference the first particle distribution, then set the tempParticles to the second distribution
        if( &particles == &particleDistributionA ) tempParticles = &particleDistributionB;
        else tempParticles = &particleDistributionA;
        
        //Select any weight that is above the minimum weight threshold
        vector< IndexedDouble > weights;
        weights.reserve(numParticles);
        for(unsigned int i=0; i<numParticles; i++){
            if( particles[i].w >= minimumWeightThreshold ){
                weights.push_back( IndexedDouble(i,particles[i].w) );
            }
        }
        
        //Sort the weights
        sort(weights.begin(),weights.end(),IndexedDouble::sortIndexedDoubleByValueAscending);
        const unsigned int numWeights = (unsigned int)weights.size();
        unsigned int randIndex = 0;
        
        //If there are no valid weights then we just pick N random particles
        if( numWeights == 0 ){
            for(unsigned int n=0; n<numParticles; n++){
                (*tempParticles)[n] = particles[ rand.getRandomNumberInt(0, numParticles) ];
            }
            if( &particles == &particleDistributionA ) particles = particleDistributionB;
            else particles = particleDistributionA;
            return true;
        }
        
        //Compute the cumulative sum
        cumsum[0] = weights[0].value;
        for(unsigned int i=1; i<numWeights; i++){
            cumsum[i] = cumsum[i-1] + weights[i].value;
        }
        
        //Resample the weights
        for(unsigned int n=0; n<numParticles; n++){
            
            //Pick a random number between 0 and the max cumsum
            double randValue = rand.getRandomNumberUniform(0,cumsum[numWeights-1]);
            randIndex = 0;
            
            //Find which bin the rand value falls into, set the random index to this value
            for(unsigned int i=0; i<numWeights; i++){
                if( randValue <= cumsum[i] ){
                    randIndex = i;
                    break;
                }
            }
            
            (*tempParticles)[n] = particles[ weights[randIndex].index ];
        }
        
        //Flip the particle references
        if( &particles == &particleDistributionA ) particles = particleDistributionB;
        else particles = particleDistributionA;
        
        return true;
    }
    
    /**
     Computes the Gaussian likelihood for the input x, given mu and sigma.
     
     @param double x: the x value for the Gaussian distrubution
     @param double mu: the mu value for the Gaussian distrubution
     @param double sigma: the sigma value for the Gaussian distrubution
     @return returns the Gaussian probabilty for the input x, given mu and sigma
     */
    double gauss(double x,double mu,double sigma){
        return 1.0/(SQRT_TWO_PI*sigma) * exp( -SQR(x-mu)/(2.0*SQR(sigma)) );
    }
    
    /**
     Computes the Radial Basic Function (RBF) for the input x, given mu, alpha, and the RBF weight.
     For speed, this function does not check to make sure the size of x and mu are the same. The user
     must therefore ensure that mu has the same size as x before they call this function.
     
     @param const VectorDouble &x: the x value for the RBF function
     @param const VectorDouble &mu: the center of the RBF function
     @param double sigma: the sigma value for the RBF function
     @param double weight: the weight for this RBF function. Default value=1.0
     @return returns the RBF function output for input x, given mu, alpha and the weight
     */
    double rbf(const VectorDouble &x,const VectorDouble &mu,double sigma,double weight=1.0){
        double sum = 0;
        const unsigned int N = x.size();
        for(UINT i=0; i<N; i++){
            sum += fabs(x[i]-mu[i]);
        }
        return weight * exp( -SQR(sum/sigma) );
    }
    
    /**
     Computes the square of the input.
     
     @param const double x: the value you want to compute the square of
     @return returns the square of the input
     */
    double SQR(const double x){ return x*x; }
    
    bool initialized;                           ///<A flag that indicates if the filter has been initialized
    bool verbose;                               ///<A flag that indicates if warning and info messages should be printed
    bool normWeights;                           ///<A flag that indicates if the weights should be normalized at each filter iteration
    unsigned int numParticles;                  ///<The number of particles in the filter
    unsigned int stateVectorSize;               ///<The size of the state vector (x)
    unsigned int initMode;                      ///<The mode used to initialize the particles, this should be one of the InitModes enums.
    unsigned int estimationMode;                ///<The estimation mode (used to compute the state estimation)
    unsigned int numDeadParticles;
    double minimumWeightThreshold;              ///<Any weight below this value will not be resampled
    double robustMeanWeightDistance;            ///<The distance parameter used in the ROBUST_MEAN estimation mode
    double estimationLikelihood;                ///<The likelihood of the estimated state
    VectorDouble x;                             ///<The state estimation
    vector< VectorDouble > initModel;           ///<The noise model for the initial starting guess
    VectorDouble processNoise;                  ///<The noise covariance in the system
    VectorDouble measurementNoise;              ///<The noise covariance in the measurement
    vector< PARTICLE > &particles;              ///<A reference to the current active particle vector
    vector< PARTICLE > particleDistributionA;   ///<A vector of particles, this will either hold the particles before or after a resample
    vector< PARTICLE > particleDistributionB;   ///<A vector of particles, this will either hold the particles before or after a resample
    VectorDouble cumsum;                        ///<The cumulative sum vector used for resampling the particles
    Random rand;                                ///<A random number generator
    WarningLog warningLog;
    ErrorLog errorLog;
    
public:
    enum InitModes{INIT_MODE_UNIFORM=0,INIT_MODE_GAUSSIAN};
    enum EstimationModes{MEAN=0,WEIGHTED_MEAN,ROBUST_MEAN,BEST_PARTICLE};
    
};

}

#endif //GRT_PARTICLE_FILTER_HEADER
