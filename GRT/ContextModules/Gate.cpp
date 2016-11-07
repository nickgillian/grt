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

#define GRT_DLL_EXPORTS
#include "Gate.h"

GRT_BEGIN_NAMESPACE

//Define the string that will be used to identify the object
const std::string Gate::id = "Gate";
std::string Gate::getId() { return Gate::id; }
	
//Register the Gate module with the Context base class
RegisterContextModule< Gate > Gate::registerModule( Gate::getId() );
    
Gate::Gate(const bool gateOpen) : Context( Gate::getId() )
{
    this->gateOpen = gateOpen;
    initialized = true;
}

Gate::Gate(const Gate &rhs) : Context( Gate::getId() )
{
    *this = rhs;
}

Gate::~Gate(){
    
}

Gate& Gate::operator=(const Gate &rhs)
{
    if( this != &rhs )
    {
        this->gateOpen = rhs.gateOpen;

        //Clone the context base variables
        copyBaseVariables( dynamic_cast<const Context*>( &rhs ) );
    }
    return *this;
}

bool Gate::deepCopyFrom(const Context *context){
    
    if( context == NULL ) return false;
    
    if( this->getId() == context->getId() ){
        
        const Gate *ptr = dynamic_cast<const Gate*>(context);
        
        //Clone the Gate values
        this->gateOpen = ptr->gateOpen;
        
        //Clone the base variables
        return copyBaseVariables( context );
    }
    return false;
}

bool Gate::process(VectorDouble inputVector){
    data = inputVector;
    okToContinue = gateOpen;
    return true;
}

bool Gate::reset(){
    return true;
}
    
GRT_END_NAMESPACE
