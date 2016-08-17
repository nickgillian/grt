
/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
@version 1.0

@brief This class implements a simple movement detection algorithm. This can be used to detect periods of 'low movement' and 'high movement' to act as additional context for other GRT algorithms.

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

#ifndef GRT_MOVEMENT_DETECTOR_HEADER
#define GRT_MOVEMENT_DETECTOR_HEADER

#include "../../CoreModules/MLBase.h"

GRT_BEGIN_NAMESPACE

class GRT_API MovementDetector : public MLBase{
    public:
    MovementDetector( const UINT numDimensions = 1, const Float upperThreshold = 1, const Float lowerThreshold = 0.9, const Float gamma = 0.95, const UINT searchTimeout = 0);
    
    virtual ~MovementDetector();
    
    virtual bool predict_( VectorFloat &input );
    
    /**
    This overrides the clear function in the ML base class.
    It will completely clear the ML module, removing any trained model and setting all the base variables to their default values.
    
    @return returns true if the module was cleared succesfully, false otherwise
    */
    virtual bool clear();
    
    /**
    This overrides the reset function in the ML base class.
    It will reset the MovementDetector, setting the state back to the search timeout and resetting the movementIndex to zero.
    
    @return returns true if the module was reset succesfully, false otherwise
    */
    virtual bool reset();
    
    /**
    This saves the trained MovementDetector model to a file.
    This overrides the save function in the ML base class.
    
    @param file: a reference to the file the MovementDetector model will be saved to
    @return returns true if the model was saved successfully, false otherwise
    */
    virtual bool save( std::fstream &file ) const;
    
    /**
    This loads a trained MovementDetector model from a file.
    This overrides the load function in the ML base class.
    
    @param file: a reference to the file the MovementDetector model will be loaded from
    @return returns true if the model was loaded successfully, false otherwise
    */
    virtual bool load( std::fstream &file );
    
    Float getUpperThreshold() const;
    
    Float getLowerThreshold() const;
    
    Float getMovementIndex() const;
    
    Float getGamma() const;
    
    bool getMovementDetected() const;
    
    bool getNoMovementDetect() const;
    
    UINT getState() const;
    
    UINT getSearchTimeout() const;
    
    bool setUpperThreshold(const Float upperThreshold);
    
    bool setLowerThreshold(const Float lowerThreshold);
    
    bool setGamma(const Float gamma);
    
    bool setSearchTimeout(const UINT searchTimeout);
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::saveModelToFile;
    using MLBase::loadModelFromFile;
    
    protected:
    
    UINT state;
    UINT searchTimeout;
    Float upperThreshold;
    Float lowerThreshold;
    Float movementIndex;
    Float gamma;
    bool firstSample;
    bool movementDetected;
    bool noMovementDetected;
    Timer searchTimer;
    VectorFloat lastSample;
    
    public:
    enum SearchStates{SEARCHING_FOR_MOVEMENT=0,SEARCHING_FOR_NO_MOVEMENT,SEARCH_TIMEOUT};
    
};

GRT_END_NAMESPACE

#endif //GRT_MOVEMENT_DETECTOR_HEADER
