/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This is the main base class that all GRT Feature Extraction algorithms should inherit from.
 
 A large number of the functions in this class are virtual and simply return false as these functions must be overwridden by the inheriting class.
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

#ifndef GRT_CONTEXT_HEADER
#define GRT_CONTEXT_HEADER

#include "MLBase.h"

namespace GRT{

class Context : public MLBase
{
public:
	Context(void){ 
        contextType = "NOT_SET"; 
        initialized = false; 
        okToContinue = true;
        numInputDimensions = 0;
        numOutputDimensions = 0;
        numContextInstances++;
    }
    
	virtual ~Context(void){
		if( --numContextInstances == 0 ){
	        delete stringContextMap;
	        stringContextMap = NULL;
	    }
	}
    
    //Clone method
    virtual bool deepCopyFrom(const Context *rhs){ return false; }
    
    bool copyBaseVariables(const Context *context){
        
        if( context == NULL ){
            errorLog << "copyBaseVariables(const Context *context) - context pointer is NULL!" << endl;
            return false;
        }
        
        if( !this->copyGRTBaseVariables( context ) ){
            return false;
        }
        
        this->contextType = context->contextType;
        this->initialized = context->initialized;
        this->okToContinue = context->okToContinue;
        this->numInputDimensions = context->numInputDimensions;
        this->numOutputDimensions = context->numOutputDimensions;
        this->data = context->data;
        this->debugLog = context->debugLog;
        this->errorLog = context->errorLog;
        this->warningLog = context->warningLog;

        return true;
    }

    virtual bool process(VectorDouble inputVector){ return false; }

    virtual bool reset(){ return false; }
    
    virtual bool updateContext(bool value){ return false; }
    
    //Getters
	string getContextType() const { return contextType; }
	UINT getNumInputDimensions() const { return numInputDimensions; }
	UINT getNumOutputDimensions() const { return numOutputDimensions; }
	bool getInitialized() const { return initialized; }
	bool getOK() const { return okToContinue; }
	VectorDouble getProcessedData() const { return data; }
    
    /**
     Defines a map between a string (which will contain the name of the context module, such as Gate) and a function returns a new instance of that context
     */
    typedef std::map< string, Context*(*)() > StringContextMap;
    
    /**
     Creates a new context instance based on the input string (which should contain the name of a valid context module such as Gate).
     
     @param string const &contextType: the name of the context module
     @return Context*: a pointer to the new instance of the context module
     */
    static Context* createInstanceFromString(string const &contextType);
    
    /**
     Creates a new context instance based on the current contextType string value.
     
     @return Context*: a pointer to the new instance of the context module
    */
    Context* createNewInstance() const;
    
protected:
    /**
     Initializes the base context module, this will resize the data vector and get the instance ready for processing new data.

     @return returns true if the module was initialized, false otherwise
     */
    bool init();
    
    /**
     Saves the core context settings to a file.
     
     @return returns true if the base settings were saved, false otherwise
     */
    bool saveContextSettingsToFile(fstream &file) const;
    
    /**
     Loads the core context settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadContextSettingsFromFile(fstream &file);

    string contextType;
    bool initialized;
    bool okToContinue;
    VectorDouble data;

	static StringContextMap *getMap(){
        if( !stringContextMap ){ stringContextMap = new StringContextMap; }
        return stringContextMap;
    }

private:
    static StringContextMap *stringContextMap;
    static UINT numContextInstances;
};

//These two functions/classes are used to register any new Context Module with the Context base class
template< typename T >  Context *newContextModuleInstance() { return new T; }

template< typename T > 
class RegisterContextModule : Context { 
public:
    RegisterContextModule(string const &newContextModuleName) { 
        getMap()->insert( std::pair<string, Context*(*)()>(newContextModuleName, &newContextModuleInstance< T > ) );
    }
};

} //End of namespace GRT

#endif //GRT_CONTEXT_HEADER
