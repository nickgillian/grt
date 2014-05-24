/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This is the main base class that all GRT Clustering algorithms should inherit from. 
 
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

#ifndef GRT_CLUSTERER_HEADER
#define GRT_CLUSTERER_HEADER

#include "MLBase.h"

namespace GRT{

class Clusterer : public MLBase
{
public:
    /**
     Default Clusterer Constructor
     */
	Clusterer(void);
    
    /**
     Default Clusterer Destructor
     */
	virtual ~Clusterer(void);
    
    /**
     This is the base deep copy function for the Clusterer modules. This function should be overwritten by the derived class.
     This deep copies the variables and models from the Clusterer pointer to this Clusterer instance.
     
     @param const Clusterer *Clusterer: a pointer to the Clusterer base class, this should be pointing to another instance of a matching derived class
     @return returns true if the clone was successfull, false otherwise (the Clusterer base class will always return flase)
     */
    virtual bool deepCopyFrom(const Clusterer *Clusterer){ return false; }
    
    /**
     This copies the Clusterer base class variables from the Clusterer pointer to this instance.
     
     @param const Clusterer *Clusterer: a pointer to a Clusterer from which the values will be copied to this instance
     @return returns true if the copy was successfull, false otherwise
     */
    bool copyBaseVariables(const Clusterer *Clusterer);
    
    /**
     This resets the Clusterer.
     This overrides the reset function in the MLBase base class.
     
     @return returns true if the Clusterer was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This function clears the Clusterer module, removing any trained model and setting all the base variables to their default values.
     
     @return returns true if the derived class was cleared succesfully, false otherwise
     */
    virtual bool clear();
    
    /**
     Returns true if the training algorithm converged during the most recent training process.
     This function will return false if the model has not been trained.
     
     @return returns true if the training algorithm converged succesfully, false otherwise
     */
    bool getConverged() const;
    
    /**
     Returns the number of clusters in the model.
     
     @return returns the number of clusters
     */
    UINT getNumClusters() const;

    /**
     Returns the classifeir type as a string.
     
     @return returns the Clusterer type as a string
     */
    string getClustererType() const;
    
    /**
     Sets the number of clusters that will be used the next time a model is trained.
     This will clear any previous model.
     The number of clusters must be greater than zero.
     
     @param UINT numClusters: the number of clusters, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setNumClusters(const UINT numClusters);
    
    /**
     Defines a map between a string (which will contain the name of the Clusterer, such as KMeans) and a function returns a new instance of that Clusterer
     */
    typedef std::map< string, Clusterer*(*)() > StringClustererMap;
    
    /**
     Creates a new Clusterer instance based on the input string (which should contain the name of a valid Clusterer such as KMeans).
     
     @param string const &ClustererType: the name of the Clusterer
     @return Clusterer*: a pointer to the new instance of the Clusterer
     */
    static Clusterer* createInstanceFromString(string const &ClustererType);
    
    /**
     Creates a new Clusterer instance based on the current clustererType string value.
     
     @return Clusterer*: a pointer to the new instance of the Clusterer
    */
    Clusterer* createNewInstance() const;
    
    /**
     This creates a new Clusterer instance and deep copies the variables and models from this instance into the deep copy.
     The function will then return a pointer to the new instance. It is up to the user who calls this function to delete the dynamic instance
     when they are finished using it.
     
     @return returns a pointer to a new Clusterer instance which is a deep copy of this instance
     */
    Clusterer* deepCopy() const;
    
    /**
     Returns a pointer to this Clusterer. This is useful for a derived class so it can get easy access to this base Clusterer.
     
     @return Clusterer&: a reference to this Clusterer
     */
    const Clusterer& getBaseClusterer() const;

    /**
     Returns a vector of the names of all Clusterers that have been registered with the base Clusterer.
     
     @return vector< string >: a vector containing the names of the Clusterers that have been registered with the base Clusterer
    */
	static vector< string > getRegisteredClusterers();
	
	//Tell the compiler we are explicitly using the following classes from the base class (this stops hidden overloaded virtual function warnings)
    using MLBase::train;
    
protected:
    /**
     Saves the core clusterer settings to a file.
     
     @return returns true if the base settings were saved, false otherwise
     */
    bool saveClustererSettingsToFile(fstream &file) const;
    
    /**
     Loads the core clusterer settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadClustererSettingsFromFile(fstream &file);

    string clustererType;
    UINT numClusters;                   //Number of clusters in the model
    bool converged;
    vector<MinMax> ranges;
    
    static StringClustererMap *getMap() {
        if( !stringClustererMap ){ stringClustererMap = new StringClustererMap; } 
        return stringClustererMap; 
    }
    
private:
    static StringClustererMap *stringClustererMap;
    static UINT numClustererInstances;
    
};
    
//These two functions/classes are used to register any new Classification Module with the Clusterer base class
template< typename T >  Clusterer* getNewClassificationModuleInstance() { return new T; }

template< typename T >
class RegisterClustererModule : Clusterer { 
public:
    RegisterClustererModule(string const &newClassificationModuleName) { 
        getMap()->insert( std::pair<string, Clusterer*(*)()>(newClassificationModuleName, &getNewClassificationModuleInstance< T > ) );
    }
};

} //End of namespace GRT

#endif //GRT_CLUSTERER_HEADER

