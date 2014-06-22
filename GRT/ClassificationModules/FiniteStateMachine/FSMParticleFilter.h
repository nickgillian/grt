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

#ifndef GRT_FPSM_PARTICLE_FILTER_HEADER
#define GRT_FPSM_PARTICLE_FILTER_HEADER

#include "../../CoreAlgorithms/ParticleFilter/ParticleFilter.h"
#include "FSMParticle.h"

namespace GRT{
    
class FSMParticleFilter : public ParticleFilter< FSMParticle, VectorDouble >{
public:
    FSMParticleFilter():errorLog("[ERROR FSMParticleFilter]"){
        pt = NULL;
        pe = NULL;
    }
    
    virtual ~FSMParticleFilter(){
        
    }
    
    virtual bool predict( FSMParticle &p ){
        
        if( !initialized ){
            errorLog << "predict( FSMParticle &p ) - Particle Filter has not been initialized!" << endl;
            return false;
        }
        
        if( pt == NULL || pe == NULL ){
            errorLog << "predict( FSMParticle &p ) - pt or pe are NULL!" << endl;
            return false;
        }
        
        //Update the particles current state
        p.currentState = random.getRandomNumberWeighted( pt->at( p.currentState ) );
        
        //Get the model for the current state
        const vector< VectorDouble > &model = pe->at( p.currentState );
        
        //Pick a random sample from the model and set this as the particles state vector
        const int K = model.size();
        if( K > 0 )
            p.x =  model[ random.getRandomNumberInt(0, K) ];
        
        return true;
    }
    
    virtual bool update( FSMParticle &p, VectorDouble &data ){
        
        if( !initialized ){
            errorLog << "update( FSMParticle &p, VectorDouble &data ) - Particle Filter has not been initialized!" << endl;
            return false;
        }
        
        if( p.x.size() != data.size() ){
            errorLog << "update( FSMParticle &p, VectorDouble &data ) - x does not match data.size()!" << endl;
            return false;
        }
        
        //Estimate the particles weight, given its current state and the sensor data
        p.w = 1;
        const size_t N = p.x.size();
        for(size_t i=0; i<N; i++){
            p.w *= gauss(p.x[i], data[i], measurementNoise[i]);
        }
        
        return true;
    }
    
    bool setLookupTables( vector< vector< IndexedDouble > > &pt, vector< vector< VectorDouble > > &pe ){
        
        this->pt = &pt;
        this->pe = &pe;
        
        return true;
    }

    Random random;
    ErrorLog errorLog;
    vector< vector< IndexedDouble > > *pt;
    vector< vector< VectorDouble > > *pe;
};

} //End of namespace GRT

#endif
