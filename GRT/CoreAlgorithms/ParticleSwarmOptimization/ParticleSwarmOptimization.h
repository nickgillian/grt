/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements a template based ParticleSwarmOptimization algorithm.
 
 @remark This implementation is based on Reyes-Sierra, Margarita, and CA Coello Coello. "Multi-objective particle swarm optimizers: A survey of the state-of-the-art." International journal of computational intelligence research 2, no. 3 (2006): 287-308.
 
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

#ifndef GRT_PARTICLE_SWARM_OPTIMIZATION_HEADER
#define GRT_PARTICLE_SWARM_OPTIMIZATION_HEADER

#include "../../Util/GRTCommon.h"
#include "PSOParticle.h"

GRT_BEGIN_NAMESPACE

template<class PARTICLE_TYPE,class OBSERVATION_TYPE>
class ParticleSwarmOptimization{
    
public:
    /**
     Default Constructor.
     */
    ParticleSwarmOptimization() : infoLog("[ParticleSwarmOptimization]"), errorLog("[ERROR ParticleSwarmOptimization]") {
        initialized = false;
        minImprovement = 1.0e-10;
        maxIter = 500;
        maxNumIterNoChange = 10;
    }
    
    /**
     Default Destructor.
     */
    virtual ~ParticleSwarmOptimization(){
        globalBestX.clear();
        particles.clear();
    }
    
    /**
     Provides direct access to the i'th particle. It is up to the user to ensure that i is within the range [0 numParticles-1].
     
     @param index: the index of the particle you want to access
     @return returns a reference to the i'th particle
     */
    PARTICLE_TYPE& operator[](const unsigned int &index){
        return particles[ index ];
    }
    
    /**
     This function initializes the PSO algorithm.  This allows the user to set the number of particles, the size of the particle state Vector (set by K),
     the minimum and maximum range of the space the PSO algorithm should search, and the propagation model.
     
     The default propagation model for each particle is just Gaussian noise, in that case the propagation model will be a K dimensional Vector, with
     each element representing sigma for the Gaussian noise for each dimension.
     
     After you call this function, you can call the search() function to run the actual search. You can reset the particles at any time using the reset() function.
     
     @param numParticles: the number of particles that will be used for the search
     @param K: the size of the particles state Vector
     @param xMin: the minimum range of the x state Vector (i.e. the space that should be searched)
     @param xMax: the maximum range of the x state Vector (i.e. the space that should be searched)
     @param propagationModel: the propagation model used for each particle, this is normally just gaussian noise (see above)
     @return returns true if the PSO algorithm was initialized successfully, false otherwise
     */
    virtual bool init(const unsigned int numParticles,const unsigned int K,const VectorFloat &xMin,const VectorFloat &xMax,const VectorFloat &propagationModel){
        
        initialized = false;
        
        if( K != xMin.size() || K != xMax.size() ) return false;
        
        this->K = K;
        this->xMin = xMin;
        this->xMax = xMax;
        this->propagationModel = propagationModel;
        
        //Clear any previous searches
        particles.clear();
        iterHistory.clear();
        globalBestXHistory.clear();
        
        //Reset the global variables
        globalBestCost = 0;
        globalBestX.clear();
        globalBestX.resize(K,0);
        
        //Initialize the new particles with random positions and velocities
        Random random;
        particles.resize(numParticles);
        typename Vector< PARTICLE_TYPE >::iterator pIter = particles.begin();
        for(pIter = particles.begin(); pIter != particles.end(); pIter++){
            pIter->setRandomSeed( random.getRandomNumberInt(0, 100000) );
            pIter->init(K,xMin,xMax);
        }
        
        //Store the initial distrubution
        iterHistory.push_back( particles );
        globalBestXHistory.push_back( globalBestX );
        
        //Flag that everything has been initialized
        initialized = true;
        
        return true;
    }
    
    /**
     Resets each particles state Vector to a random position and velocity.  You need to initialize the PSO algorithm first before you can use this function. 
     
     @return returns true if the PSO algorithm was reset successfully, false otherwise
     */
    virtual bool reset(){
        
        if( !initialized ) return false;
        
        typename Vector< PARTICLE_TYPE >::iterator pIter = particles.begin();
        for(pIter = particles.begin(); pIter != particles.end(); pIter++){
            pIter->reset();
        }
        
        return true;
    }
    
    /**
     Performs the main search.  You need to initialize the PSO algorithm first before you can use this function.
     
     @param OBSERVATION_TYPE &observation: a reference to the observation data used for the search
     @return returns true if the search ran successfully, false otherwise
     */
    virtual bool search(OBSERVATION_TYPE &observation){
        
        if( !initialized ) return false;

        //Propagate all the particles
        typename Vector< PARTICLE_TYPE >::iterator pIter;
        for(pIter = particles.begin(); pIter != particles.end(); pIter++){
            if( !pIter->propagate( propagationModel ) ){
                errorLog << "search(...) - Particle propagation failed!" << std::endl;
                return false;
            }
        }
        
        unsigned int iterCounter = 0;
        unsigned int numIterNoChangeCounter = 0;
        Float currentMaxima = 0;
        Float lastMaxima = 0;
        Float delta = 0;
        bool keepSearching = true;
        
        //Decrement the global best cost
        globalBestCost = 0;
        
        while( keepSearching ){
            currentMaxima = searchIteration( observation );
            
            iterHistory.push_back( particles );
            globalBestXHistory.push_back( globalBestX );
            
            delta = fabs( currentMaxima - lastMaxima );
            lastMaxima = currentMaxima;
            
            if( delta < minImprovement ){
                if( ++numIterNoChangeCounter >= maxNumIterNoChange ){
                    keepSearching = false;
                    infoLog << "search(...) - Reached no change limit, stopping search at iteration: " << iterCounter << " with a change of: " << fabs( currentMaxima - lastMaxima ) << std::endl;
                }
            }else numIterNoChangeCounter = 0;
            
            if( ++iterCounter == maxIter ){
                keepSearching = false;
                infoLog << "search(...) - Max iteration reached, stopping search at iteration: " << iterCounter << std::endl;
            }
        }
        
        //Compute the final state estimation
        finalX = globalBestX;
        
        return true;
    }
    
    /**
     Performs one iteration of the search.  Normally, you do not call this function directly but instead call the main search() function.
     You need to initialize the PSO algorithm first before you can use this function.
     
     @param observation: a reference to the observation data used for the search
     @return returns true if the search ran successfully, false otherwise
     */
    virtual Float searchIteration(OBSERVATION_TYPE &observation){
        
        if( !initialized ) return 0;
        
        unsigned int index = 0;
        unsigned int bestIndex = 0;
        Float currentBestMaxima = 0;
        Float epsilon = 0;
        typename Vector< PARTICLE_TYPE >::iterator pIter;
        
        //Compute the cost for each particle, tracking the best cost of all the particles in the swarm
        for(pIter = particles.begin(); pIter != particles.end(); pIter++){
            
            //Evaluate the current particle
            epsilon = pIter->evaluate(observation);

            //Check to see if this is the best
            if( epsilon >  currentBestMaxima ){
                currentBestMaxima = epsilon;
                bestIndex = index;
            }
            
            index++;
        }
        
        //Check to see if we need to update the global cost and position
        if( currentBestMaxima > globalBestCost ){
            globalBestCost = currentBestMaxima;
            globalBestX = particles[ bestIndex ].x;
        }

        //Update the position and velocity of all of the particles
        for(pIter = particles.begin(); pIter != particles.end(); pIter++){
            pIter->update( globalBestX );
        }
        
        return currentBestMaxima;
    }
    
    /**
     Updates the propagation model. You need to initialize the PSO algorithm first before you can use this function.
     
     const propagationModel: the new propagation model
     @return returns true if the propagationModel was updated successfully, false otherwise
     */
    bool setPropagationModel( const VectorFloat &propagationModel ){
        
        if( !initialized ) return false;
        
        this->propagationModel = propagationModel;
        
        return true;
    }

    bool initialized;   ///< A flag to indicate if the PSO algorithm has been initialized
    unsigned int K;     ///< The size of the particles state Vector
    Float minImprovement;
    unsigned int maxIter;
    unsigned int maxNumIterNoChange;
    Float globalBestCost;  ///< The current global best cost over all the particles
    VectorFloat finalX;  ///< The final estimate
    VectorFloat globalBestX;   ///< The state Vector of the particle with the best cost
    VectorFloat xMin;  ///< The minimum range of the state space
    VectorFloat xMax;  ///< The maximum range of the state space
    VectorFloat propagationModel;  ///<The propagation model used to update each particle
    Vector< PARTICLE_TYPE > particles;  ///< A Vector containing all the particles used for the search
    Vector< VectorFloat > globalBestXHistory;  ///< A buffer to keep track of the search history
    Vector< Vector< PARTICLE_TYPE > > iterHistory;  ///< A buffer to keep track of the search history
    
    InfoLog infoLog;
    ErrorLog errorLog;
};
    
GRT_END_NAMESPACE

#endif //GRT_PARTICLE_SWARM_OPTIMIZATION_HEADER
