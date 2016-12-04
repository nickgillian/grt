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
#include "GRTBase.h"

GRT_BEGIN_NAMESPACE
    
GRTBase::GRTBase(const std::string &id):classId(id){
    if( classId == "" ){
        infoLog.setKey("[" + classId + "]");
        debugLog.setKey("[DEBUG " + classId + "]");
        errorLog.setKey("[ERROR " + classId + "]");
        warningLog.setKey("[WARNING " + classId + "]");
    }else{
        infoLog.setKey("[" + classId + "]");
        debugLog.setKey("[DEBUG]");
        errorLog.setKey("[ERROR]");
        warningLog.setKey("[WARNING]");
    }
}
    
GRTBase::~GRTBase(void){
}
    
bool GRTBase::copyGRTBaseVariables(const GRTBase *base){
    
    if( base == NULL ){
        errorLog << "copyBaseVariables(const GRTBase *base) - base pointer is NULL!" << std::endl;
        return false;
    }

    this->classId = base->classId;
    this->debugLog = base->debugLog;
    this->errorLog = base->errorLog;
    this->infoLog = base->infoLog;
    this->warningLog = base->warningLog;
    
    return true;
}
    
std::string GRTBase::getLastWarningMessage() const {
    return warningLog.getLastMessage();
}

std::string GRTBase::getLastErrorMessage() const {
    return errorLog.getLastMessage();
}
    
std::string GRTBase::getLastInfoMessage() const {
    return infoLog.getLastMessage();
}
    
std::string GRTBase::getGRTVersion(bool returnRevision){
    std::string version = GRT_VERSION;
    if( returnRevision ) version += " revision: " + getGRTRevison();
    return version;
}

std::string GRTBase::getGRTRevison(){
    return GRT_REVISION;
}
    
std::string GRTBase::getClassType() const{
    return getId();
}

std::string GRTBase::getId() const{
    return classId;
}
    
GRTBase* GRTBase::getGRTBasePointer(){
    return this;
}

const GRTBase* GRTBase::getGRTBasePointer() const{
    return this;
}

bool GRTBase::setInfoLoggingEnabled(const bool loggingEnabled){
    return infoLog.setInstanceLoggingEnabled( loggingEnabled );
}
    
bool GRTBase::setWarningLoggingEnabled(const bool loggingEnabled){
    return warningLog.setInstanceLoggingEnabled( loggingEnabled );
}
    
bool GRTBase::setErrorLoggingEnabled(const bool loggingEnabled){
    return errorLog.setInstanceLoggingEnabled( loggingEnabled );
}

bool GRTBase::setDebugLoggingEnabled(const bool loggingEnabled){
    return debugLog.setInstanceLoggingEnabled( loggingEnabled );
}

GRT_END_NAMESPACE
