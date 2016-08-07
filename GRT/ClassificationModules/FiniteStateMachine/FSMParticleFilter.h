/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0
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

#ifndef GRT_FSM_PARTICLE_FILTER_HEADER
#define GRT_FSM_PARTICLE_FILTER_HEADER

#include "../../CoreAlgorithms/ParticleFilter/ParticleFilter.h"
#include "FSMParticle.h"

GRT_BEGIN_NAMESPACE

class FSMParticleFilter : public ParticleFilter< FSMParticle, VectorFloat >{
    public:
    FSMParticleFilter():errorLog("[ERROR FSMParticleFilter]"){
        pt = NULL;
        pe = NULL;
    }
    
    FSMParticleFilter( const FSMParticleFilter &rhs ):errorLog("[ERROR FSMParticleFilter]"){
        pt = NULL;
        pe = NULL;
        //TODO: Need to handle copy better
    }
    
    virtual ~FSMParticleFilter(){
        
    }
    
    virtual bool predict( FSMParticle &p ){
        
        if( !initialized ){
            errorLog << "predict( FSMParticle &p ) - Particle Filter has not been initialized!" << std::endl;
            return false;
        }
        
        if( pt == NULL || pe == NULL ){
            errorLog << "predict( FSMParticle &p ) - pt or pe are NULL!" << std::endl;
            return false;
        }
        
        //Update the particles current state
        p.currentState = random.getRandomNumberWeighted( pt->at( p.currentState ) );
        
        //Get the model for the current state
        const Vector< VectorDouble > &model = pe->at( p.currentState );
        
        //Pick a random sample from the model and set this as the particles state vector
        const unsigned int K = model.getSize();
        if( K > 0 )
        p.x =  model[ random.getRandomNumberInt(0, K) ];
        
        return true;
    }
    
    virtual bool update( FSMParticle &p, VectorDouble &data ){
        
        if( !initialized ){
            errorLog << "update( FSMParticle &p, VectorDouble &data ) - Particle Filter has not been initialized!" << std::endl;
            return false;
        }
        
        if( p.x.size() != data.size() ){
            errorLog << "update( FSMParticle &p, VectorDouble &data ) - x does not match data.size()!" << std::endl;
            return false;
        }
        
        //Estimate the particles weight, given its current state and the sensor data
        p.w = 1;
        const size_t N = p.x.size();
        for(size_t i=0; i<N; i++){
            p.w *= gauss(p.x[i], data[i], measurementNoise[i]);
        }
        
        /*
        std::cout << "w: " << p.w << " p.x: ";
        for(size_t i=0; i<N; i++){
        std::cout << p.x[i] << " ";
        }
        std::cout << " data: ";
        for(size_t i=0; i<N; i++){
        std::cout << data[i] << " ";
        }
        std::cout << std::endl;
        */
        
        return true;
    }
    
    bool setLookupTables( Vector< Vector< IndexedDouble > > &pt, Vector< Vector< VectorDouble > > &pe ){
        
        this->pt = &pt;
        this->pe = &pe;
        
        return true;
    }
    
    Random random;
    ErrorLog errorLog;
    Vector< Vector< IndexedDouble > > *pt;
    Vector< Vector< VectorDouble > > *pe;
};

GRT_END_NAMESPACE

#endif
