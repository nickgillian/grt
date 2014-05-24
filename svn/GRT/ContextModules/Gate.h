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

namespace GRT{

class Gate : public Context{
public:
    Gate(bool gateOpen=true);
    virtual ~Gate(void);
    
    //Override the base class methods
    virtual bool deepCopyFrom(const Context *context){
        
        if( context == NULL ) return false;
        
        if( this->getContextType() == context->getContextType() ){
            
            Gate *ptr = (Gate*)context;
            //Clone the Gate values 
            this->gateOpen = ptr->gateOpen;
            
            //Clone the base variables
            return copyBaseVariables( context );
        }
        return false;
    }
    
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
    
protected:
    bool gateOpen;
    
    static RegisterContextModule< Gate > registerModule;
};
    
}//End of namespace GRT

#endif //GRT_GATE_HEADER
