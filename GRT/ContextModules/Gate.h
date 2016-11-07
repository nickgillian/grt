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

#ifndef GRT_GATE_HEADER
#define GRT_GATE_HEADER

#include "../Util/GRTCommon.h"
#include "../CoreModules/Context.h"

GRT_BEGIN_NAMESPACE

class GRT_API Gate : public Context{
public:
    /**
    Default Constructor. Sets the gate status

    @param gateOpen: sets the gate status (open or closed). Default value gateOpen=true
    */
    Gate(const bool gateOpen=true);

    /**
    Copy Constructor.
    
    Copies the values from the rhs Gate to this instance of the Gate.
    
    @param rhs: the rhs from which the values will be copied to this this instance of the Gate
    */
    Gate(const Gate &rhs);

    /**
    Default Destructor
    */
    virtual ~Gate();

    /**
    Assigns the equals operator setting how the values from the rhs instance will be copied to this instance.
    
    @param rhs: the rhs instance from which the values will be copied to this this instance of the Gate
    @return returns a reference to this instance of the Gate
    */
    Gate& operator=(const Gate &rhs);

    /**
    Sets the Context deepCopyFrom function, overwriting the base Context function.
    This function is used to deep copy the values from the input pointer to this instance of the Context module.
    This function is called by the GestureRecognitionPipeline when the user adds a new Context module to the pipeline.
    
    @param postProcessing: a pointer to another instance of a Gate, the values of that instance will be cloned to this instance
    @return true if the deep copy was successful, false otherwise
    */
    virtual bool deepCopyFrom(const Context *context);
    
    virtual bool process(VectorDouble inputVector);
    virtual bool reset();
    
    bool updateContext(bool value){ 
        gateOpen = value;
        okToContinue = value;
        return true;
    }
    
    //Setters
    void setGateStatus(bool gateOpen){ this->gateOpen = gateOpen; }
    
    //Getters
    bool getGateStatus(){ return gateOpen; }

    /**
    Gets a string that represents the ID of this class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
protected:
    bool gateOpen;
    
private:
    static const std::string id;  
    static RegisterContextModule< Gate > registerModule;
};
    
GRT_END_NAMESPACE

#endif //GRT_GATE_HEADER
