/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This file contains the GRTBase class. This is the core base class for all the GRT modules.
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

#ifndef GRT_GRT_BASE_HEADER
#define GRT_GRT_BASE_HEADER

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <float.h>
#include <cmath>
#include <time.h>
#include <map>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <cstddef>

#include "../Util/GRTCommon.h"

GRT_BEGIN_NAMESPACE

class GRT_API GRTBase
{
public:
    /**
     Default GRTBase Constructor
     */
	GRTBase(void);
    
    /**
     Default GRTBase Destructor
     */
	virtual ~GRTBase(void);
    
    /**
     This copies the GRTBase variables from the GRTBase pointer to the instance that calls the function.
     
     @param const GRTBase *GRTBase: a pointer to a GRTBase from which the values will be copied to the instance that calls the function
     @return returns true if the copy was successfull, false otherwise
     */
    bool copyGRTBaseVariables(const GRTBase *GRTBase);
    
    /**
     This function returns the name of the current class as a std::string.  For example, if you asked AdaBoost for the class type then this function would
     return "AdaBoost".
     
     return returns a std::string representing the class type
     */
    std::string getClassType() const;
    
    /**
	 This function returns the last warning message as a std::string. If no warnings have occured, the std::string will be empty.
     
     @return returns the last warning message as a std::string.
     */
    std::string getLastWarningMessage() const;
    
    /**
	 This function returns the last error message as a std::string. If no errors have occured, the std::string will be empty.
     
     @return returns the last error message as a std::string.
     */
    std::string getLastErrorMessage() const;
    
    /**
	 This function returns the last info message as a std::string. If no info have occured, the std::string will be empty.
     
     @return returns the last info message as a std::string.
     */
    std::string getLastInfoMessage() const;
    
    /**
     Sets if info logging is enabled/disabled for this specific instance.
     If you want to enable/disable info logging globally, then you should use the InfoLog::enableLogging( bool ) function.
     
     @return returns true if the parameter was updated, false otherwise
     */
    bool setInfoLoggingEnabled(const bool loggingEnabled);
    
    /**
     Sets if warning logging is enabled/disabled for this specific instance.
     If you want to enable/disable warning logging globally, then you should use the WarningLog::enableLogging( bool ) function.
     
     @return returns true if the parameter was updated, false otherwise
     */
    bool setWarningLoggingEnabled(const bool loggingEnabled);
    
    /**
     Sets if error logging is enabled/disabled for this specific instance.
     If you want to enable/disable error logging globally, then you should use the ErrorLog::enableLogging( bool ) function.
     
     @return returns true if the parameter was updated, false otherwise
     */
    bool setErrorLoggingEnabled(const bool loggingEnabled);
    
    /**
     This functions the GRT version number and revision as a std::string. If you do not want the revision number then set the returnRevision
     parameter to false.
     
     @param returnRevision: sets if the revision number should be added to the std::string that is returned. Default value is true.
     @return returns the GRT version number and revision as a std::string.
     */
    static std::string getGRTVersion(bool returnRevision = true);
    
    /**
	 This functions returns the current GRT revision as a std::string.
     
     @return returns the current GRT revision as a std::string.
     */
    static std::string getGRTRevison();
    
    /**
	 This functions returns a pointer to the current instance.
     
     @return returns a GRTBase pointer to the current instance.
     */
    GRTBase* getGRTBasePointer();
    
    /**
	 This functions returns a const pointer to the current instance.
     
     @return returns a const GRTBase pointer to the current instance.
     */
    const GRTBase* getGRTBasePointer() const;

protected:
    inline Float SQR(const Float &x) const{ return x*x; }

    std::string classType;
    DebugLog debugLog;
    ErrorLog errorLog;
    InfoLog infoLog;
    TrainingLog trainingLog;
    TestingLog testingLog;
    WarningLog warningLog;
    
};

GRT_END_NAMESPACE

#endif //GRT_GRT_BASE_HEADER

