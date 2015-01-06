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

#ifndef GRT_PARTICLE_CLASSIFIER_HEADER
#define GRT_PARTICLE_CLASSIFIER_HEADER

#include "../../CoreModules/Classifier.h"
#include "ParticleClassifierParticleFilter.h"

namespace GRT{

class ParticleClassifier : public Classifier
{
public:
	
    /**
     Default Constructor
     */
	ParticleClassifier(const unsigned int numParticles = 2000,const double sensorNoise = 20.0,const double transitionSigma = 0.005,const double phaseSigma = 0.1,const double velocitySigma = 0.01);
	
    /**
     Default copy constructor
     
     Defines how the data from the rhs ParticleClassifier should be copied to this ParticleClassifier
     
     @param const ParticleClassifier &rhs: another instance of a ParticleClassifier
     */
	ParticleClassifier(const ParticleClassifier &rhs);
	
     /**
     Default Destructor
     */
	virtual ~ParticleClassifier(void);
    
    /**
     Defines how the data from the rhs ParticleClassifier should be copied to this ParticleClassifier
     
     @param const ParticleClassifier &rhs: another instance of a ParticleClassifier
     @return returns a pointer to this instance of the ParticleClassifier
     */
	ParticleClassifier& operator=(const ParticleClassifier &rhs);
    
    /**
     This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
     It clones the data from the Base Class Classifier pointer (which should be pointing to an ParticleClassifier instance) into this instance
     
     @param Classifier *classifier: a pointer to the Classifier Base Class, this should be pointing to another ParticleClassifier instance
     @return returns true if the clone was successfull, false otherwise
     */
	virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
     This trains the ParticleClassifier model, using the labelled timeseries classification data.
     This overrides the train function in the Classifier base class.
     
     @param TimeSeriesClassificationData trainingData: a reference to the training data
     @return returns true if the DTW model was trained, false otherwise
     */
    virtual bool train_(TimeSeriesClassificationData &trainingData);
    
    /**
     This predicts the class of the inputVector.
     This overrides the predict function in the Classifier base class.
     
     @param VectorDouble inputVector: the input vector to classify
     @return returns true if the prediction was performed, false otherwise
     */
    virtual bool predict_(VectorDouble &inputVector);
    
    /**
     This saves the trained ParticleClassifier model to a file.
     This overrides the saveModelToFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the ParticleClassifier model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained ParticleClassifier model from a file.
     This overrides the loadModelFromFile function in the Classifier base class.
     
     @param fstream &file: a reference to the file the ParticleClassifier model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    /**
     This clears the ParticleClassifier classifier.
     
     @return returns true if the ParticleClassifier model was successfully cleared, false otherwise.
     */
    virtual bool clear();
    
    /**
     This resets the ParticleClassifier classifier.
     
     @return returns true if the ParticleClassifier model was successfully reset, false otherwise.
     */
    virtual bool reset();
    
    const vector< ParticleClassifierGestureTemplate >& getGestureTemplates() const;
    
    const ParticleClassifierParticleFilter& getParticleFilter() const;
    
    VectorDouble getStateEstimation() const;
    
    double getPhase() const;
    
    double getVelocity() const;
    
    bool setNumParticles(const unsigned int numParticles);

    bool setSensorNoise(const unsigned int sensorNoise);
    
    bool setTransitionSigma(const unsigned int transitionSigma);
    
    bool setPhaseSigma(const unsigned int phaseSigma);
    
    bool setVelocitySigma(const unsigned int velocitySigma);
    
    
    using MLBase::predict;
    using MLBase::train;
    
protected:
    unsigned int numParticles;
    double sensorNoise;
    double transitionSigma;
    double phaseSigma;
    double velocitySigma;
	ParticleClassifierParticleFilter particleFilter;
    
private:
    static RegisterClassifierModule< ParticleClassifier > registerModule;
	

};
    
}//End of namespace GRT

#endif
