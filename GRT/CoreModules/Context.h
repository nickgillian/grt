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

GRT_BEGIN_NAMESPACE

class GRT_API Context : public MLBase
{
public:
	Context( const std::string &id = "" ) : MLBase( id, MLBase::CONTEXT )
    {
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
            errorLog << "copyBaseVariables(const Context *context) - context pointer is NULL!" << std::endl;
            return false;
        }
        
        if( !this->copyGRTBaseVariables( context ) ){
            return false;
        }
        
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

    virtual bool process(VectorFloat inputVector){ return false; }

    virtual bool reset(){ return false; }
    
    virtual bool updateContext(bool value){ return false; }
    
    //Getters
	std::string getContextType() const { return contextType; }
	UINT getNumInputDimensions() const { return numInputDimensions; }
	UINT getNumOutputDimensions() const { return numOutputDimensions; }
	bool getInitialized() const { return initialized; }
	bool getOK() const { return okToContinue; }
	VectorFloat getProcessedData() const { return data; }
    
    /**
     Defines a map between a string (which will contain the name of the context module, such as Gate) and a function returns a new instance of that context
     */
    typedef std::map< std::string, Context*(*)() > StringContextMap;

    /**
     Creates a new Context instance based on the input string (which should contain the name of a valid Context such as Gate).
     
     @param id: the id of the Context
     @return a pointer to the new instance of the Context
     */
    static Context* create( std::string const &id );
    
    /**
     Creates a new Context instance based on the current type string value.
     
     @return Context*: a pointer to the new instance of the Context
    */
    Context* create() const;

    GRT_DEPRECATED_MSG( "createNewInstance is deprecated, use create instead.", Context* createNewInstance() const );
    GRT_DEPRECATED_MSG( "createInstanceFromString is deprecated, use create instead.", static Context* createInstanceFromString( const std::string &id ) );
    
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
    bool saveContextSettingsToFile( std::fstream &file ) const;
    
    /**
     Loads the core context settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadContextSettingsFromFile( std::fstream &file );

    std::string contextType;
    bool initialized;
    bool okToContinue;
    VectorFloat data;

	static StringContextMap *getMap(){
        if( !stringContextMap ){ stringContextMap = new StringContextMap; }
        return stringContextMap;
    }

private:
    static StringContextMap *stringContextMap;
    static UINT numContextInstances;
};

template< typename T >  Context *createNewContextModuleInstance() { return new T; } ///< Returns a pointer to a new instance of the template class, the caller is responsible for deleting the pointer

/**
 @brief This class provides an interface for classes to register themselves with the Context base class, this enables Context algorithms to
 be automatically be created from just a string, e.g.: Context *gate = create( "Gate" );
*/
template< typename T > 
class RegisterContextModule : Context { 
public:
    RegisterContextModule( const std::string &newContextModuleName ) { 
        getMap()->insert( std::pair< std::string, Context*(*)() >(newContextModuleName, &createNewContextModuleInstance< T > ) );
    }
};

GRT_END_NAMESPACE

#endif //GRT_CONTEXT_HEADER
