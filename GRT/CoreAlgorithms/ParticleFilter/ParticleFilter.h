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

GRT_BEGIN_NAMESPACE

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
        estimationMode = WEIGHTED_MEAN;
        numParticles = 0;
        stateVectorSize = 0;
        numDeadParticles = 0;
        wNorm = 0;
        wDotProduct = 0;
        minimumWeightThreshold = 1.0e-20;
        resampleThreshold = 1.0e-5;
        robustMeanWeightDistance = 0.2;
        estimationLikelihood = 0;
        warningLog.setKey("[WARNING ParticleFilter]");
        errorLog.setKey("[ERROR ParticleFilter]");
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
     Provides direct constant access to the i'th particle. It is the user's responsibility to ensure i is within bounds.
     
     @param const unsigned int &i: the index of the particle you want
     @return returns a reference to the i'th particle (if i is valid)
     */
    const PARTICLE& operator[](const unsigned int &i) const {
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
     Provides direct constant access to the i'th particle before it was resampled. It is the user's responsibility to ensure i is within bounds.
     
     @param const unsigned int &i: the index of the particle you want
     @return returns a reference to the i'th particle (if i is valid)
     */
    const PARTICLE& operator()(const unsigned int &i) const {
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
            this->wNorm = rhs.wNorm;
            this->wDotProduct = rhs.wDotProduct;
            this->minimumWeightThreshold = rhs.minimumWeightThreshold;
            this->resampleThreshold = rhs.resampleThreshold;
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
     Initializes the particles.  The size of the init model sets the number of dimensions in the state Vector.  The size of the process
     noise and measurement noise Vectors can be different, depending on the exact problem you are using the ParticleFilter to solve.
     */
    virtual bool init(const unsigned int numParticles,const Vector< VectorFloat > &initModel,const VectorFloat &processNoise,const VectorFloat &measurementNoise){
        
        //Clear any previous setup
        clear();
        
        //Check to make sure each Vector in the initModel has 2 dimensions, these are min and max or mu and sigma depening on the init mode
        for(unsigned int i=0; i<initModel.size(); i++){
            if( initModel[i].size() != 2 ){
                errorLog << "ERROR: The " << i << " dimension of the initModel does not have 2 dimensions!" << std::endl;
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
            errorLog << "ERROR: Failed to init particles!" << std::endl;
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
    virtual bool initParticles( const UINT numParticles ){
        
        if( !initialized ) return false;
        
        if( stateVectorSize != x.size() ){
            return false;
        }
        
        this->numParticles = numParticles;
        particleDistributionA.clear();
        particleDistributionB.clear();
        cumsum.clear();
        particleDistributionA.resize( numParticles, PARTICLE(stateVectorSize) );
        particleDistributionB.resize( numParticles, PARTICLE(stateVectorSize) );
        particles = particleDistributionA;
        cumsum.resize( numParticles,0 );
        
        reset();
        
        return true;
    }

    
    /**
     The main filter function that should be called at each time step.
     
     @param SENSOR_DATA &data: the template data structure containing any data you need to pass to the update function.
     @return returns true if the filter was run successfully, false otherwise
     */
    virtual bool filter(SENSOR_DATA &data){
        
        if( !initialized ){
            errorLog << "ERROR: The particle filter has not been initialized!" << std::endl;
            return false;
        }

        if( !preFilterUpdate( data ) ){
            errorLog << "ERROR: Failed to complete preFilterUpdate!" << std::endl;
            return false;
        }
        
        unsigned int i = 0;
        typename Vector< PARTICLE >::iterator iter;
        
        //The main particle prediction loop
        for( iter = particles.begin(), i = 0; iter != particles.end(); ++iter, i++ ){
            if( !predict( *iter ) ){
                errorLog << "ERROR: Particle " << i << " failed prediction!" << std::endl;
                return false;
            }
        }
        
        //The main particle update loop
        for( iter = particles.begin(), i = 0; iter != particles.end(); ++iter, i++ ){
            if( !update( *iter, data ) ){
                errorLog << "ERROR: Particle " << i << " failed update!" << std::endl;
                return false;
            }
        }
        
        //Normalize the particle weights so they sum to 1
        if( normWeights ){
            if( !normalizeWeights() ){
                errorLog << "ERROR: Failed to normalize particle weights! " << std::endl;
                return false;
            }
        }
        
        //Compute the final state estimate
        if( !computeEstimate() ){
            errorLog << "ERROR: Failed to compute the final estimat!" << std::endl;
            return false;
        }
        
        //Check to see if we should resample the particles for the next iteration
        if( checkForResample() ){
        
            //Resample the particles
            if( !resample() ){
                errorLog << "ERROR: Failed to resample particles!" << std::endl;
                return false;
            }
        }

        if( !postFilterUpdate( data ) ){
            errorLog << "ERROR: Failed to complete postFilterUpdate!" << std::endl;
            return false;
        }
        
        return true;
    }
    
    /**
     Clears any previous setup.
     
     @return returns true if the filter was cleared successfully, false otherwise
     */
    virtual bool clear(){
        initialized = false;
        numParticles = 0;
        stateVectorSize = 0;
        estimationLikelihood = 0;
        wNorm = 0;
        wDotProduct = 0;
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
    
    virtual bool reset(){
        
        if( !initialized ){
            return false;
        }
        
        for(unsigned int i=0; i<numParticles; i++){
            for(unsigned int j=0; j<stateVectorSize; j++){
                switch( initMode ){
                    case INIT_MODE_UNIFORM:
                        particles[i].x[j] = rand.getRandomNumberUniform(initModel[j][0],initModel[j][1]);
                        break;
                    case INIT_MODE_GAUSSIAN:
                        particles[i].x[j] = initModel[j][0] + rand.getRandomNumberGauss(0,initModel[j][1]);
                        break;
                    default:
                        errorLog << "ERROR: Unknown initMode!" << std::endl;
                        return false;
                        break;
                }
                
            }
        }
        
        return true;
    }
    
    /**
     @return returns if the particle filter has been initialized, false otherwise
     */
    bool getInitialized() const {
        return initialized;
    }
    
    /**
     Gets the number of particles.
     
     @return returns the number of particles if the filter is initialized, zero otherwise
     */
    unsigned int getNumParticles() const{
        return initialized ? (unsigned int)particles.size() : 0;
    }
    
    /**
     Gets the number of dimensions in the state Vector.
     
     @return returns the number of dimensions in the state Vector the filter is initialized, zero otherwise
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
     This will be a Float in the range of [0 1] (or INF if the likelihood is INF).
     
     @return returns a Float representing the estimation likelihood
     */
    Float getEstimationLikelihood() const{
        return estimationLikelihood;
    }
    
    /**
     Gets this sum of all the weights. This is the value that is used to normalize the weights.
     
     @return returns a Float representing the sum of all the weights
     */
    Float getWeightSum() const {
        return wNorm;
    }
    
    /**
     Gets the current state estimation Vector.
     
     @return returns a VectorFloat containing the current state estimation
     */
    VectorFloat getStateEstimation() const{
        return x;
    }
    
    /**
     The function returns the current Vector of particles. These are the particles that have been resampled.
     
     @return returns a Vector with the current particles
     */
    Vector< PARTICLE > getParticles(){
        return (&particles == &particleDistributionA ? particleDistributionA : particleDistributionB);
    }
    
    /**
     The function returns the Vector of particles before they were resampled.
     
     @return returns a Vector with the old particles (i.e. before they were resampled
     */
    Vector< PARTICLE > getOldParticles(){
        return (&particles == &particleDistributionA ? particleDistributionB : particleDistributionA);
    }
    
    /**
     Gets the process noise Vector.
     
     @return returns a Float Vector containing the process noise
     */
    VectorFloat setProcessNoise() const{
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
     Sets the threshold used to determine if the particles should be resampled.
     The particles will be resampled if the wNorm value is less than the resampleThreshold.
     The resampleThreshold should be in the range [0 1], normally something like: 1.0e-20 works well.
     
     @param const Float resampleThreshold: the new resampleThreshold
     @return returns true if the parameter was successfully updated, false otherwise
     */
    bool setResampleThreshold(const Float resampleThreshold){
        this->resampleThreshold = resampleThreshold;
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
     
     The new init model Vector size must match the size of the current init model Vector.
     
     @param const Vector< VectorFloat > initModel: a Vector containing the new init model for the particle filter
     @return returns true if the initModel was successfully updated, false otherwise
     */
    bool setInitModel(const Vector< VectorFloat > initModel){
        if( this->initModel.size() == initModel.size() ){
            this->initModel = initModel;
            return true;
        }
        return false;
    }
    
    /**
     Sets the process noise. The new process noise Vector size must match the size of the current process noise Vector.
     
     @param const VectorFloat &processNoise: a Vector containing the new process noise for the particle filter
     @return returns true if the processNoise was successfully updated, false otherwise
     */
    bool setProcessNoise(const VectorFloat &processNoise){
        if( this->processNoise.size() == processNoise.size() ){
            this->processNoise = processNoise;
            return true;
        }
        return false;
    }
    
    /**
     Sets the measurement noise. The new measurement noise Vector size must match the size of the current measurement noise Vector.
     
     @param const VectorFloat &measurementNoise: a Vector containing the new measurement noise for the particle filter
     @return returns true if the measurementNoise was successfully updated, false otherwise
     */
    bool setMeasurementNoise(const VectorFloat &measurementNoise){
        if( this->measurementNoise.size() == measurementNoise.size() ){
            this->measurementNoise = measurementNoise;
            return true;
        }
        return false;
    }
    
protected:
    /**
     This is the main predict function. This function must be implemented in your derived class.
     
     @param PARTICLE &p: the current particle that should be passed through your prediction code
     @return returns true if the particle prediction was updated successfully, false otherwise
     */
    virtual bool predict( PARTICLE &p ){
        errorLog << "predict( PARTICLE &p ) Prediction function not implemented! This must be implemented by the derived class!" << std::endl;
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
        errorLog << "update( PARTICLE &p, SENSOR_DATA &data ) Update function not implemented! This must be implemented by the derived class!" << std::endl;
        return false;
    }
    
    /**
     This function normalizes the particle weights so they sum to 1.
     This is a virtual function, so you can override it in your derived class if needed.
     
     @return returns true if the particle weights were normalized successfully, false otherwise
     */
    virtual bool normalizeWeights(){
        
        //Compute the total particle weight and number of dead particles
        wNorm = 0;
        wDotProduct = 0;
        numDeadParticles = 0;
        typename Vector< PARTICLE >::iterator iter;
        for( iter = particles.begin(); iter != particles.end(); ++iter ){
            if( grt_isinf( iter->w ) ){
                numDeadParticles++;
                iter->w = 0;
            }else{
                wNorm += iter->w;
            }
        }
        
        if( wNorm == 0 ){
            if( verbose )
                warningLog << "normalizeWeights() - Weight norm is zero!" << std::endl;
            return true;
        }
        
        //Normalized the weights so they sum to 1
        Float weightUpdate = 1.0 / wNorm;
        for( iter = particles.begin(); iter != particles.end(); ++iter ){
            
            //Normalize the weights (so they sum to 1)
            iter->w *= weightUpdate;
            
            //Compute the total weights dot product (this is used later to test for degeneracy)
            wDotProduct += iter->w * iter->w;
        }
        wDotProduct = 1.0 / wDotProduct;
        
        //cout << "wNorm: " << wNorm << " wDotProduct: " << wDotProduct << std::endl;

        return true;
    }
    
    /**
     The function computes the state estimate, using the current particles. The exact method
     used to compute the estimate depends on the current estimationMode.
     This is a virtual function, so you can override it in your derived class if needed.
     
     @return returns true if the state estimation was correctly computed, false otherwise
     */
    virtual bool computeEstimate(){
        
        typename Vector< PARTICLE >::iterator iter;
        const unsigned int N = (unsigned int)x.size();
        unsigned int bestIndex = 0;
        unsigned int robustMeanParticleCounter = 0;
        Float bestWeight = 0;
        Float sum = 0;
        estimationLikelihood = 0;
        switch( estimationMode ){
            case MEAN:
                for(unsigned int j=0; j<N; j++){
                    x[j] = 0;
                }
                
                for( iter = particles.begin(); iter != particles.end(); ++iter ){
                    for(unsigned int j=0; j<N; j++){
                        x[j] += iter->x[j];
                    }
                    estimationLikelihood += grt_isnan(iter->w) ? 0 : iter->w;
                }
                
                for(unsigned int j=0; j<N; j++){
                    x[j] /= Float(numParticles);
                }
                estimationLikelihood /= Float(numParticles);
                break;
            case WEIGHTED_MEAN:
                for(unsigned int j=0; j<N; j++){
                    x[j] = 0;
                    sum = 0;
                    for( iter = particles.begin(); iter != particles.end(); ++iter ){
                        x[j] += iter->x[j] * iter->w;
                        sum += iter->w;
                    }
                    x[j] /= sum;
                }
                
                for( iter = particles.begin(); iter != particles.end(); ++iter ){
                    estimationLikelihood += grt_isnan(iter->w) ? 0 : iter->w;
                }
                estimationLikelihood /= Float(numParticles);
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
                for( iter = particles.begin(); iter != particles.end(); ++iter ){
                    if( fabs( iter->w - particles[bestIndex].w ) <= robustMeanWeightDistance ){
                        for(unsigned int j=0; j<N; j++){
                            x[j] += iter->x[j] * iter->w;
                        }
                        estimationLikelihood += grt_isnan(iter->w) ? 0 : iter->w;
                        sum += iter->w;
                        robustMeanParticleCounter++;
                    }
                }
                
                //Normalize x
                for(unsigned int j=0; j<N; j++){
                    x[j] /= sum;
                }
                estimationLikelihood /= Float(robustMeanParticleCounter);
                break;
            case BEST_PARTICLE:
                for(unsigned int i=0; i<numParticles; i++){
                    if( particles[i].w > bestWeight ){
                        bestWeight = particles[i].w;
                        bestIndex = i;
                    }
                }
                x = particles[bestIndex].x;
                estimationLikelihood = grt_isnan(particles[bestIndex].w) ? 0 : particles[bestIndex].w;
                break;
            default:
                errorLog << "ERROR: Unknown estimation mode!" << std::endl;
                return false;
                break;
        }
        return true;
    }
    
    /**
     This function checks to see if the particles should be resampled for the next iteration of filtering.
     This is a virtual function, so you can override it in your derived class if needed.
     
     @return returns true if the particles should be resampled, false otherwise
     */
    virtual bool checkForResample(){
        return wNorm < resampleThreshold;
    }
    
    /**
     The main resample function. Resamples the particles based on the particles weights.
     This is a virtual function, so you can override it in your derived class if needed.
     
     @return returns true if the particles were correctly resampled, false otherwise
     */
    virtual bool resample(){
        
        Vector< PARTICLE > *tempParticles = NULL;
        
        //If the particles are currently reference the first particle distribution, then set the tempParticles to the second distribution
        if( &particles == &particleDistributionA ) tempParticles = &particleDistributionB;
        else tempParticles = &particleDistributionA;
        
        //Select any weight that is above the minimum weight threshold
        Vector< IndexedDouble > weights;
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
        const unsigned int numRandomParticles = (unsigned int) round(numParticles/100.0*10.0);
        for(unsigned int n=0; n<numParticles; n++){
            
            if( numParticles-n > numParticles-numRandomParticles){
            
                //Pick a random number between 0 and the max cumsum
                Float randValue = rand.getRandomNumberUniform(0,cumsum[numWeights-1]);
                randIndex = 0;
                
                //Find which bin the rand value falls into, set the random index to this value
                for(unsigned int i=0; i<numWeights; i++){
                    if( randValue <= cumsum[i] ){
                        randIndex = i;
                        break;
                    }
                }
                
                (*tempParticles)[n] = particles[ weights[randIndex].index ];
            }else{
                //Randomly initalize the weakest particles
                PARTICLE &p = (*tempParticles)[n];
                for(unsigned int j=0; j<stateVectorSize; j++){
                    switch( initMode ){
                        case INIT_MODE_UNIFORM:
                            p.x[j] = rand.getRandomNumberUniform(initModel[j][0],initModel[j][1]);
                            break;
                        case INIT_MODE_GAUSSIAN:
                            p.x[j] = initModel[j][0] + rand.getRandomNumberGauss(0,initModel[j][1]);
                            break;
                        default:
                            errorLog << "ERROR: Unknown initMode!" << std::endl;
                            return false;
                            break;
                    }
                    
                }
            }
        }
        
        //Swap the particle references
        if( &particles == &particleDistributionA ) particles = particleDistributionB;
        else particles = particleDistributionA;
        
        return true;
    }

    /**
      	This function lets you define a custom pre filter update if needed.
	@param SENSOR_DATA &data: a reference to the sensor data input to the main filter function.
	@return returns true if the update was completed successfully, false otherwise
     */
    virtual bool preFilterUpdate( SENSOR_DATA &data ){
        return true;
    }

    /**
      	This function lets you define a custom post filter update if needed.
	@param SENSOR_DATA &data: a reference to the sensor data input to the main filter function.
	@return returns true if the update was completed successfully, false otherwise
     */
    virtual bool postFilterUpdate( SENSOR_DATA &data ){
        return true;
    }
    
    /**
     Computes the Gaussian likelihood for the input x, given mu and sigma.
     
     @param Float x: the x value for the Gaussian distrubution
     @param Float mu: the mu value for the Gaussian distrubution
     @param Float sigma: the sigma value for the Gaussian distrubution
     @return returns the Gaussian probabilty for the input x, given mu and sigma
     */
    Float gauss(Float x,Float mu,Float sigma){
        return 1.0/(SQRT_TWO_PI*sigma) * exp( -SQR(x-mu)/(2.0*SQR(sigma)) );
    }
    
    /**
     Computes the Radial Basic Function (RBF) for the input x, given mu, alpha, and the RBF weight.
     For speed, this function does not check to make sure the size of x and mu are the same. The user
     must therefore ensure that mu has the same size as x before they call this function.
     
     @param const Float x: the x value for the RBF function
     @param const Float mu: the center of the RBF function
     @param Float sigma: the sigma value for the RBF function
     @param Float weight: the weight for this RBF function. Default value=1.0
     @return returns the RBF function output for input x, given mu, alpha and the weight
     */
    Float rbf(const Float x,const Float mu,Float sigma,Float weight=1.0){
        return weight * exp( -SQR( fabs(x-mu) / sigma ) );
    }
    
    /**
     Computes the Radial Basic Function (RBF) for the input x, given mu, alpha, and the RBF weight.
     For speed, this function does not check to make sure the size of x and mu are the same. The user
     must therefore ensure that mu has the same size as x before they call this function.
     
     @param const VectorFloat &x: the x value for the RBF function
     @param const VectorFloat &mu: the center of the RBF function
     @param Float sigma: the sigma value for the RBF function
     @param Float weight: the weight for this RBF function. Default value=1.0
     @return returns the RBF function output for input x, given mu, alpha and the weight
     */
    Float rbf(const VectorFloat &x,const VectorFloat &mu,Float sigma,Float weight=1.0){
        Float sum = 0;
        const unsigned int N = (unsigned int)x.size();
        for(UINT i=0; i<N; i++){
            sum += fabs(x[i]-mu[i]);
        }
        return weight * exp( -SQR(sum/sigma) );
    }
    
    /**
     Computes the square of the input.
     
     @param const Float x: the value you want to compute the square of
     @return returns the square of the input
     */
    Float SQR(const Float x){ return x*x; }

    bool initialized;                               ///<A flag that indicates if the filter has been initialized
    bool verbose;                                   ///<A flag that indicates if warning and info messages should be printed
    bool normWeights;                               ///<A flag that indicates if the weights should be normalized at each filter iteration
    unsigned int numParticles;                      ///<The number of particles in the filter
    unsigned int stateVectorSize;                   ///<The size of the state Vector (x)
    unsigned int initMode;                          ///<The mode used to initialize the particles, this should be one of the InitModes enums.
    unsigned int estimationMode;                    ///<The estimation mode (used to compute the state estimation)
    unsigned int numDeadParticles;
    Float minimumWeightThreshold;                  ///<Any weight below this value will not be resampled
    Float robustMeanWeightDistance;                ///<The distance parameter used in the ROBUST_MEAN estimation mode
    Float estimationLikelihood;                    ///<The likelihood of the estimated state
    Float wNorm;                                   ///<Stores the total weight norm value
    Float wDotProduct;                             ///<Stores the dot product of all the weights, used to test for degeneracy
    Float resampleThreshold;                       ///<The threshold below which the particles will be resampled
    VectorFloat x;                                 ///<The state estimation
    Vector< VectorFloat > initModel;           ///<The noise model for the initial starting guess
    VectorFloat processNoise;                      ///<The noise covariance in the system
    VectorFloat measurementNoise;                  ///<The noise covariance in the measurement
    Vector< PARTICLE > &particles;              ///<A reference to the current active particle Vector
    Vector< PARTICLE > particleDistributionA;   ///<A Vector of particles, this will either hold the particles before or after a resample
    Vector< PARTICLE > particleDistributionB;   ///<A Vector of particles, this will either hold the particles before or after a resample
    VectorFloat cumsum;                            ///<The cumulative sum Vector used for resampling the particles
    Random rand;                                    ///<A random number generator
    WarningLog warningLog;
    ErrorLog errorLog;
    
public:
    enum InitModes{INIT_MODE_UNIFORM=0,INIT_MODE_GAUSSIAN};
    enum EstimationModes{MEAN=0,WEIGHTED_MEAN,ROBUST_MEAN,BEST_PARTICLE};
    
};

GRT_END_NAMESPACE

#endif //GRT_PARTICLE_FILTER_HEADER
