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

#ifndef GRT_PSO_PARTICLE_HEADER
#define GRT_PSO_PARTICLE_HEADER

#include "../../Util/GRTCommon.h"

namespace GRT{

template <class OBSERVATION_TYPE>
class PSOParticle{

public:
    /**
     Default Constructor.
     */
    PSOParticle(){
        initialized = false;
    }
 
    /**
     This constructor attempts to initialize the particle, see the init function for more info.
     
     @param const unsigned int K: the size of the particle state vector, must be greater than zero
     @param const vector< double > &xMin: the minimum range of the x state vector (i.e. the space that should be searched)
     @param const vector< double > &xMax: the maximum range of the x state vector (i.e. the space that should be searched)
     */
    PSOParticle(const unsigned int K,const vector<double> &xMin,const vector<double> &xMax){
        init(K,xMin,xMax);
    }
    
    /**
     Defines the copy constructor.
     
     @param const PSOParticle &rhs: another instance of the PSOParticle which will be copied to this instance
     */
    PSOParticle(const PSOParticle &rhs){
        *this = rhs;
    }
    
    /**
     Default Destructor.
     */
    virtual ~PSOParticle(){}
    
    /**
     Defines the equals operator, copies the data from the right hand instance to this instance.
     
     @param const PSOParticle &rhs: another instance of the PSOParticle which will be copied to this instance
     */
    PSOParticle& operator=(const PSOParticle &rhs){
        
        if( this != &rhs ){
            this->K = rhs.K;
            this->w = rhs.w;
            this->c1 = rhs.c1;
            this->c2 = rhs.c2;
            this->localBestCost = rhs.localBestCost;
            this->x = rhs.x;
            this->v = rhs.v;
            this->xMin = rhs.xMin;
            this->xMax = rhs.xMax;
            this->localBestX = rhs.localBestX;
            this->initialized = rhs.initialized;
            this->random = rhs.random;
        }
        return *this;
        
    }
    
    /**
     Initializes the PSOParticle. The size of xMin and xMax must match K.
     
     @param const unsigned int K: the size of the particle state vector, must be greater than zero
     @param const vector< double > &xMin: the minimum range of the x state vector (i.e. the space that should be searched)
     @param const vector< double > &xMax: the maximum range of the x state vector (i.e. the space that should be searched)
     @return returns true if the particle was initialized, false otherwise
     */
    virtual bool init(const unsigned int K,const vector<double> &xMin,const vector<double> &xMax){
        
        initialized = false;
        
        if( K != xMin.size() || K != xMax.size() ){
            return false;
        }
        
        this->K = K;
        this->xMin = xMin;
        this->xMax = xMax;
        w = 1.0;
        c1 = 2.0;
        c2 = 2.0;
        x.resize(K,0);
        v.resize(K,0);
        
        initialized = true;
        
        //Reset the state vector
        return reset();
    }
    
    /**
     Propogates the particle, using the model.  The default model used here is Gaussian Noise, in that case the propagation model
     will be a K dimensional vector, with each of the k values represen representing sigma for each dimension (it is assumed mu = 0).
     
     If you override this function then you can define your own model.
     
     @param const vector< double > &model: the propagation model, this should be a K*2 dimensional vector
     @return returns true if the particle was propagated, false otherwise
     */
    virtual bool propagate(const vector< double > &model){
        
        if( !initialized ) return false;
        
        if( model.size() != K ) return false;
        
        //Update the current position
        for(unsigned int k=0; k<K; k++){
            x[k] += random.getRandomNumberGauss(0,model[k]);
        }
        
        //Decrease the local best cost
        localBestCost *= 0.9;
        
        return true;
    }
    
    /**
     Update the particle, using the globalBest state vector across all particles.
     
     @param const vector< double > &globalBestX: the globe best state vector across all particles
     @return returns true if the particle was updated, false otherwise
     */
    virtual bool update(const vector< double > &globalBestX){
        
        if( !initialized ) return false;
        
        double r1 = 0;
        double r2 = 0;
        double vMax = 0;
        for(unsigned int k=0; k<K; k++){
            r1 = random.getRandomNumberUniform(0.0,1.0);
            r2 = random.getRandomNumberUniform(0.0,1.0);
            
            //Update the velocity
            v[k] = ( w * v[k] ) + ( c1 * r1 * (localBestX[k]-x[k]) ) + ( c2 * r2 * (globalBestX[k]-x[k]) );
            
            //Velocity Clamping
            vMax = 0.1 * ((xMax[k]-xMin[k])/2.0);
            if( v[k] > vMax ){ v[k] = vMax; }
            else if( v[k] < -vMax ){ v[k] = -vMax; }
            
            //Position Update
            x[k] = x[k] + v[k];
            
            //Update the xMax xMin values
            if( x[k] > xMax[k] ){ xMax[k] = x[k]; }
            else if( x[k] < xMin[k] ){ xMin[k] = x[k]; }
        }
        
        return true;
    }
    
    /**
     This function is used to evaluate the fitness of each particle. This function uses the squared distance between the estimated
     state vector and the observation, the observation must therefore have a length of K.
     
     For most cases, you will need to override this function and replace the evaluation code with your own.
     
     @param OBSERVATION_TYPE &observation: a reference to the latest observation
     @return returns the particles evaluation, -1 will be returned if the evaluation fails
     */
    virtual double evaluate(OBSERVATION_TYPE &observation){
        if( !initialized ) return -1;
        
        if( observation.size() != K ) return -1;
        
        //A cost of 1.0 is best, 0 is the worse
        double cost = 0;
        for(UINT i=0; i<K; i++){
            cost += SQR(x[i]-observation[i]);
        }
        cost += 0.0001;
        cost = 1.0/(cost*cost);
        
        //Check to see if we have found a local maxima, if so store the values of x that achieved it
        if( cost > localBestCost ){
            localBestCost = cost;
            localBestX = x;
        }
        return cost;
    }
    
    /**
     Resets the particles state vector with random uniform noise.
     
     @return returns true if the particle was reset, false otherwise
     */
    virtual bool reset(){
        
        if( !initialized ) return false;

        for(unsigned int k=0; k<K; k++){
            x[k] = random.getRandomNumberUniform(xMin[k],xMax[k]);
            v[k] = random.getRandomNumberUniform(-1.0,1);
        }
        
        //Set the local best state vector to the current x state vector
        localBestX = x;
        localBestCost = 0;
        
        return true;
    }
    
    /**
     Sets the particles random seed.
     
     @param unsigned long long seed: the new seed value
     @return returns true if the seed was set, false otherwise
     */
    bool setRandomSeed(unsigned long long seed){
        random.setSeed( seed );
        return true;
    }
    
    /**
     Gets the Normal Gaussian Distrubution for x, given mu and sigma
     
     @param double x: the x parameter for the Gaussian distrubution
     @param double mu: the mu parameter for the Gaussian distrubution
     @param double sigma: the sigma parameter for the Gaussian distrubution
     @return returns the likelihood of x, given mu and sigma
     */
    inline double normal(double x,double mu,double sigma){
        return ( 1.0/(sigma*SQRT_TWO_PI) ) * exp( - ( SQR(x-mu)/(2.0*SQR(sigma)) ) );
    }
    
    /**
     Gets the standard Gaussian Distrubution for x, given mu and sigma
     
     @param double x: the x parameter for the Gaussian distrubution
     @param double mu: the mu parameter for the Gaussian distrubution
     @param double sigma: the sigma parameter for the Gaussian distrubution
     @return returns the likelihood of x, given mu and sigma
     */
    inline double gauss(double x,double mu,double sigma){
        return exp( - ( SQR(x-mu)/(2.0*SQR(sigma)) ) );
    }
    
    /**
     Gets the square of x.
     
     @param double x: the value you want to square
     @return returns the square of x
     */
    inline double SQR(double x){ return x*x; }
    
    bool initialized;
    unsigned int K;
    double w;
    double c1;
    double c2;
    double localBestCost;
    vector< double > x;
    vector< double > v;
    vector< double > localBestX;
    vector< double > xMin;
    vector< double > xMax;
    Random random;

};

}

#endif //GRT_PSO_PARTICLE_HEADER

