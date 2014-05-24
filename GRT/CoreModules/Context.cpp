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

#include "Context.h"
namespace GRT{
    
Context::StringContextMap* Context::stringContextMap = NULL;
UINT Context::numContextInstances = 0;
    
Context* Context::createInstanceFromString(string const &contextType){
    
    StringContextMap::iterator iter = getMap()->find( contextType );
    if( iter == getMap()->end() ){
        return NULL;
    }
    return iter->second();
}
    
Context* Context::createNewInstance() const{
    return createInstanceFromString( contextType );
}
    
bool Context::init(){
    
    //Clear any previous data
    data.clear();
    
    if( numOutputDimensions == 0 ){
        errorLog << "init() - Failed to init module, the number of output dimensions is zero!" << endl;
        initialized = false;
        return false;
    }
    
    //Flag that the context data is OK to continue
    okToContinue = true;
    
    //Resize the feature vector
    data.resize(numOutputDimensions,0);
    
    //Flag the module has been initialized
    initialized = true;
    
    return true;
}

bool Context::saveContextSettingsToFile(fstream &file) const{
    
    if( !file.is_open() ){
        errorLog << "saveContextSettingsToFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    if( !MLBase::saveBaseSettingsToFile( file ) ) return false;
    
    file << "Initialized: " << initialized << endl;
    
    return true;
}

bool Context::loadContextSettingsFromFile(fstream &file){
    
    if( !file.is_open() ){
        errorLog << "loadContextSettingsFromFile(fstream &file) - The file is not open!" << endl;
        return false;
    }
    
    //Try and load the base settings from the file
    if( !MLBase::loadBaseSettingsFromFile( file ) ){
        return false;
    }
    
    string word;
    
    //Load if the filter has been initialized
    file >> word;
    if( word != "Initialized:" ){
        errorLog << "loadContextSettingsFromFile(fstream &file) - Failed to read Initialized header!" << endl;
        clear();
        return false;
    }
    file >> initialized;
    
    //If the module has been initalized then call the init function to setup the feature data vector
    if( initialized ){
        return init();
    }
    
    return true;
}

} //End of namespace GRT

