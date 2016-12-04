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

#ifndef GRT_PARTICLE_CLASSIFIER_PARTICLE_FILTER_HEADER
#define GRT_PARTICLE_CLASSIFIER_PARTICLE_FILTER_HEADER

#include "../../CoreAlgorithms/ParticleFilter/ParticleFilter.h"

GRT_BEGIN_NAMESPACE

class ParticleClassifierGestureTemplate{
    public:
    ParticleClassifierGestureTemplate(){
        classLabel = 0;
    }
    
    virtual ~ParticleClassifierGestureTemplate(){
        
    }
    
    unsigned int getLength() const {
        return timeseries.getNumRows();
    }
    
    unsigned int classLabel;
    MatrixFloat timeseries;
};

class ParticleClassifierParticleFilter : public ParticleFilter< Particle,VectorFloat > {
    
public:
    ParticleClassifierParticleFilter(){
        setEstimationMode( WEIGHTED_MEAN );
        clear();
    }
    
    virtual ~ParticleClassifierParticleFilter(){
        
    }
    
    virtual bool preFilterUpdate( VectorFloat &data ){
        
        //Randomly reset a small number of particles to ensure the classifier does not get stuck on one gesture
        unsigned int numRandomFlipParticles = 0;//(unsigned int)floor( processNoise[0] * Float(numParticles) );
        
        for(unsigned int i=0; i<numRandomFlipParticles; i++){
            //unsigned int randomParticleIndex = rand.getRandomNumberInt(0, numParticles);
            unsigned int randomParticleIndex = i;
            
            particles[ randomParticleIndex ].x[0] = rand.getRandomNumberInt(0, numTemplates); //Randomly pick a template
            particles[ randomParticleIndex ].x[1] = rand.getRandomNumberUniform(0,1); //Randomly pick a phase
            particles[ randomParticleIndex ].x[2] = 0; //rand.getRandomNumberUniform(-processNoise[2],processNoise[2]); //Randomly pick a speed
        }
        
        return true;
    }
    
    virtual bool postFilterUpdate( VectorFloat &data ){
        
        return true;
    }
    
    virtual bool predict( Particle &p ){
        
        //Given the prior set of particles, randomly generate new state estimations using the process model
        const Float phase = p.x[1];
        
        //Update the phase
        p.x[1] = Util::limit( phase + rand.getRandomNumberGauss(0.0,processNoise[1]) , 0, 1);
        
        //Update the velocity
        p.x[2] += phase-p.x[1];
        
        //Limit the velocity
        p.x[2] = Util::limit( p.x[2], -processNoise[2], processNoise[2] );
        
        return true;
    }
    
    virtual bool update( Particle &p, VectorFloat &data ){
        
        //Generate the weights for the current particle
        p.w = 1;
        
        //Get the gesture template
        const unsigned int templateIndex = (unsigned int)p.x[0];
        
        if( templateIndex >= numTemplates ){
            errorLog << "update( Particle &p, VectorFloat &data ) - Template index out of bounds! templateIndex: " << templateIndex << std::endl;
            return false;
        }
        
        //Get the current position in the template
        const unsigned int templateLength = gestureTemplates[templateIndex].timeseries.getNumRows();
        const unsigned int templatePos = (unsigned int)(p.x[1] * Float(templateLength-1));
        
        if( templatePos >= templateLength ){
            errorLog << "update( Particle &p, VectorFloat &data ) - Template position out of bounds! templatePos: " << templatePos << " templateLength: " << templateLength << std::endl;
            return false;
        }
        
        for(unsigned int j=0; j<numInputDimensions; j++){
            p.w *= gauss( data[j], gestureTemplates[templateIndex].timeseries[templatePos][j], measurementNoise[j] );
        }
        
        return true;
    }
    
    virtual bool clear(){
        
        //Clear the base class
        ParticleFilter::clear();
        
        numInputDimensions = 0;
        numTemplates = 0;
        numClasses = 0;
        resampleCounter = 0;
        gestureTemplates.clear();
        
        return true;
    }
    
    bool train( const unsigned int numParticles, const TimeSeriesClassificationData &trainingData, Float sensorNoise, Float transitionSigma, Float phaseSigma, Float velocitySigma){
        
        //Clear any previous model
        clear();
        
        this->numParticles = numParticles;
        numInputDimensions = trainingData.getNumDimensions();
        numTemplates = trainingData.getNumSamples();
        numClasses = trainingData.getNumClasses();
        
        gestureTemplates.resize( numTemplates );
        for(unsigned int i=0; i<numTemplates; i++){
            gestureTemplates[i].classLabel = trainingData[i].getClassLabel();
            gestureTemplates[i].timeseries = trainingData[i].getData();
        }
        
        //Init the particle filter
        //State vector is:
        //[0] gesture template index
        //[1] phase (position within the template, normalized [0 1])
        //[2] velocity (value between [-0.2 0.2])
        stateVectorSize = 3;
        initModel.resize( stateVectorSize, VectorFloat(2,0) );
        processNoise.resize( stateVectorSize );
        measurementNoise.resize( numInputDimensions );
        
        //Min state range
        initModel[0][0] = 0;
        initModel[1][0] = 0;
        initModel[2][0] = -0.2;
        
        //Max state range
        initModel[0][1] = numTemplates;
        initModel[1][1] = 1;
        initModel[2][1] = 0.2;
        
        //Set the measurement noise - this sets the sigma difference between the estimated position in the template and sensor input
        for(unsigned int i=0; i<numInputDimensions; i++){
            measurementNoise[i] = sensorNoise;
        }
        
        //Set the process noise used for the
        processNoise[0] = transitionSigma; //Controls the random template selection
        processNoise[1] = phaseSigma; //Controls the phase update
        processNoise[2] = velocitySigma; //Controls the maximum velocity update
        
        x.resize( stateVectorSize );
        initialized = true;
        
        if( !initParticles( numParticles ) ){
            errorLog << "ERROR: Failed to init particles!" << std::endl;
            clear();
            return false;
        }
        
        return true;
    }
    
    /*
    virtual bool checkForResample(){
    if( ++resampleCounter >= 100 ){
    resampleCounter = 0;
    return true;
    }
    return false;
    }
    */
    
    unsigned int numInputDimensions;
    unsigned int numTemplates;
    unsigned int numClasses;
    unsigned int resampleCounter;
    Vector< ParticleClassifierGestureTemplate > gestureTemplates;
    
};

GRT_END_NAMESPACE

#endif //GRT_PARTICLE_CLASSIFIER_PARTICLE_FILTER_HEADER
    