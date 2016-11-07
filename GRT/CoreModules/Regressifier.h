/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This is the main base class that all GRT Regression algorithms should inherit from.
 
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

#ifndef GRT_REGRESSIFIER_HEADER
#define GRT_REGRESSIFIER_HEADER

#include "MLBase.h"
#include "../DataStructures/ClassificationData.h"
#include "../DataStructures/TimeSeriesClassificationData.h"

GRT_BEGIN_NAMESPACE
    
#define DEFAULT_NULL_LIKELIHOOD_VALUE 0
#define DEFAULT_NULL_DISTANCE_VALUE 0

class GRT_API Regressifier : public MLBase
{
public:
    /**
     Default Regressifier Destructor
     @param id: a unique string that identifies the Regressifier parent class (e.g., LinearRegression)
     */
	Regressifier( const std::string &id = "" );
    
    /**
     Default Regressifier Destructor
     */
	virtual ~Regressifier(void);
    
    /**
     This is the base deep copy function for the Regressifier modules. This function should be overwritten by the derived class.
     This deep copies the variables and models from the regressifier pointer to this regressifier instance.
     
     @param regressifier: a pointer to the Regressifier base class, this should be pointing to another instance of a matching derived class
     @return returns true if the deep copy was successfull, false otherwise (the Regressifier base class will always return false)
     */
    virtual bool deepCopyFrom(const Regressifier *regressifier){ return false; }
    
	/**
     This copies the Regressifier variables from the regressifier pointer to this instance.
     
     @param regressifier: a pointer to a regressifier from which the values will be copied to this instance
     @return returns true if the copy was successfull, false otherwise
     */
    bool copyBaseVariables(const Regressifier *regressifier);
    
    /**
     This resets the regressifier.
     This overrides the reset function in the MLBase base class.
     
     @return returns true if the regressifier was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This function clears the regressifier module, removing any trained model and setting all the base variables to their default values.
     
     @return returns true if the module was cleared succesfully, false otherwise
     */
    virtual bool clear();
    
    /**
     Gets a Vector containing the regression data output by the regression algorithm, this will be an M-dimensional Vector, where M is the number of output dimensions in the model.  
     
     @return returns a Vector containing the regression data output by the regression algorithm, an empty Vector will be returned if the model has not been trained
     */
    VectorFloat getRegressionData() const;
    
    /**
     Returns the ranges of the input (i.e. feature) data.
     
     @return returns a Vector of MinMax values representing the ranges of the input data
     */
	Vector< MinMax > getInputRanges() const;
    
    /**
     Returns the ranges of the output (i.e. target) data.
     
     @return returns a Vector of MinMax values representing the ranges of the target data
     */
	Vector< MinMax > getOutputRanges() const;
    
    /**
     Defines a map between a string (which will contain the name of the regressifier, such as LinearRegression) and a function returns a new instance of that regressifier
     */
    typedef std::map< std::string, Regressifier*(*)() > StringRegressifierMap;
    
    /**
     This creates a new Regressifier instance and deep copies the variables and models from this instance into the deep copy.
     The function will then return a pointer to the new instance. It is up to the user who calls this function to delete the dynamic instance
     when they are finished using it.
     
     @return returns a pointer to a new Regressifier instance which is a deep copy of this instance
     */
    Regressifier* deepCopy() const;
    
    /**
     Returns a pointer to this regressifier. This is useful for a derived class so it can get easy access to this base regressifier.
     
     @return a reference to this regressifier
     */
    const Regressifier& getBaseRegressifier() const;
    
    /**
     Returns a Vector of the names of all regressifiers that have been registered with the base regressifier.
     
     @return a Vector containing the names of the regressifiers that have been registered with the base regressifier
     */
	static Vector< std::string > getRegisteredRegressifiers();

    /**
     Creates a new regressifier instance based on the input string (which should contain the name of a valid regressifier such as LinearRegression).
     
     @param id: the id of the regressifier
     @return a pointer to the new instance of the regressifier
     */
    static Regressifier* create( const std::string &id );

    /**
     Creates a new regressifier instance based on the current regressifierType string value.
     
     @return Regressifier*: a pointer to the new instance of the regressifier
    */
    Regressifier* create() const;

    GRT_DEPRECATED_MSG( "createNewInstance is deprecated, use create() instead.", Regressifier* createNewInstance() const );
    GRT_DEPRECATED_MSG( "createInstanceFromString(id) is deprecated, use create(id) instead.", static Regressifier* createInstanceFromString( const std::string &id ) );
    GRT_DEPRECATED_MSG( "getRegressifierType is deprecated, use getId() instead", std::string getRegressifierType() const );
    
protected:
    /**
     Saves the core base settings to a file.
     
     @return returns true if the base settings were saved, false otherwise
     */
    bool saveBaseSettingsToFile( std::fstream &file ) const;
    
    /**
     Loads the core base settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadBaseSettingsFromFile( std::fstream &file );

    std::string regressifierType;
    VectorFloat regressionData;
    Vector< MinMax > inputVectorRanges;
	Vector< MinMax > targetVectorRanges;
    
    static StringRegressifierMap *getMap() {
        if( !stringRegressifierMap ){ stringRegressifierMap = new StringRegressifierMap; } 
        return stringRegressifierMap; 
    }
    
private:
    static StringRegressifierMap *stringRegressifierMap;
    static UINT numRegressifierInstances;

};
    
template< typename T >  
Regressifier *createNewRegressionInstance() { return new T; } ///< Returns a pointer to a new instance of the template class, the caller is responsible for deleting the pointer

/**
 @brief This class provides an interface for classes to register themselves with the regressifier base class, this enables Regression algorithms to
 be automatically be created from just a string, e.g.: Regressifier *mlp = create( "MLP" );
*/
template< typename T > 
class RegisterRegressifierModule : public Regressifier { 
public:
    RegisterRegressifierModule( const std::string &newRegresionModuleName ) { 
        getMap()->insert( std::pair< std::string, Regressifier*(*)() >(newRegresionModuleName, &createNewRegressionInstance< T > ) );
    }
};

GRT_END_NAMESPACE

#endif //GRT_REGRESSIFIER_HEADER

