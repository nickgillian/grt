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

GRT_BEGIN_NAMESPACE

class GRT_API Clusterer : public MLBase
{
public:
    /**
     Default Clusterer Constructor
     @param id: the id for the Clusterer (e.g., KMeans)
     */
	Clusterer( const std::string &id = "" );
    
    /**
     Default Clusterer Destructor
     */
	virtual ~Clusterer(void);
    
    /**
     This is the base deep copy function for the Clusterer modules. This function should be overwritten by the derived class.
     This deep copies the variables and models from the Clusterer pointer to this Clusterer instance.
     
     @param clusterer: a pointer to the Clusterer base class, this should be pointing to another instance of a matching derived class
     @return returns true if the clone was successfull, false otherwise (the Clusterer base class will always return flase)
     */
    virtual bool deepCopyFrom(const Clusterer *clusterer){ return false; }
    
    /**
     This copies the Clusterer base class variables from the Clusterer pointer to this instance.
     
     @param clusterer: a pointer to a Clusterer from which the values will be copied to this instance
     @return returns true if the copy was successfull, false otherwise
     */
    bool copyBaseVariables(const Clusterer *clusterer);
    
    /**
     This is the main interface for training the clusterer model.
     
     @param trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(MatrixFloat &trainingData);
    
    /**
     Override the main ClassificationData train function to pass MatrixFloat data to the Clusterer train function.
     
     @param trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     Override the main UnlabelledData train function to pass MatrixFloat data to the Clusterer train function.
     
     @param trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(UnlabelledData &trainingData);
    
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
     Returns the predicted cluster label.
     
     @return returns the predicted cluster label
     */
    UINT getPredictedClusterLabel() const;
    
    /**
     Returns the current maximumLikelihood value.
     The maximumLikelihood value is computed during the prediction phase and is the likelihood of the most likely model.
     This value will return 0 if a prediction has not been made.
     
     @return returns the current maximumLikelihood value
     */
    Float getMaximumLikelihood() const;
    
    /**
     Returns the current bestDistance value.
     The bestDistance value is computed during the prediction phase and is either the minimum or maximum distance, depending on the algorithm.
     This value will return 0 if a prediction has not been made.
     
     @return returns the current bestDistance value
     */
    Float getBestDistance() const;
    
    /**
     Gets a Vector of the cluster likelihoods from the last prediction, this will be an N-dimensional Vector, where N is the number of clusters in the model.
     The exact form of these likelihoods depends on the cluster algorithm.
     
     @return returns a Vector of the cluster likelihoods from the last prediction, an empty Vector will be returned if the model has not been trained
     */
    VectorFloat getClusterLikelihoods() const;
    
    /**
     Gets a Vector of the cluster distances from the last prediction, this will be an N-dimensional Vector, where N is the number of clusters in the model.
     The exact form of these distances depends on the cluster algorithm.
     
     @return returns a Vector of the cluster distances from the last prediction, an empty Vector will be returned if the model has not been trained
     */
    VectorFloat getClusterDistances() const;
    
    /**
     Gets a Vector of unsigned ints containing the label of each cluster, this will be an K-dimensional Vector, where K is the number of clusters in the model.

     @return returns a Vector of unsigned ints containing the label of each cluster, an empty Vector will be returned if the model has not been trained
     */
    Vector< UINT > getClusterLabels() const;

    GRT_DEPRECATED_MSG( "getClustererType() is deprecated, use getId() or getBaseId() instead", std::string getClustererType() const );
    
    /**
     Sets the number of clusters that will be used the next time a model is trained.
     This will clear any previous model.
     The number of clusters must be greater than zero.
     
     @param numClusters: the number of clusters, must be greater than zero
     @return returns true if the value was updated successfully, false otherwise
     */
    bool setNumClusters(const UINT numClusters);
    
    /**
     Defines a map between a string (which will contain the name of the Clusterer, such as KMeans) and a function returns a new instance of that Clusterer
     */
    typedef std::map< std::string, Clusterer*(*)() > StringClustererMap;
    
    /**
     Creates a new Clusterer instance based on the input string (which should contain the name of a valid Clusterer such as KMeans).
     
     @param id: the id of the Clusterer
     @return a pointer to the new instance of the Clusterer
     */
    static Clusterer* create( std::string const &id );
    
    /**
     Creates a new Clusterer instance based on the current clustererType string value.
     
     @return Clusterer*: a pointer to the new instance of the Clusterer
    */
    Clusterer* create() const;

    GRT_DEPRECATED_MSG( "createNewInstance is deprecated, use create instead.", Clusterer* createNewInstance() const );
    GRT_DEPRECATED_MSG( "createInstanceFromString is deprecated, use create instead.", static Clusterer* createInstanceFromString( const std::string &id ) );
    
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
     Returns a Vector of the names of all Clusterers that have been registered with the base Clusterer.
     
     @return Vector< std::string >: a Vector containing the names of the Clusterers that have been registered with the base Clusterer
    */
	static Vector< std::string > getRegisteredClusterers();
	
	//Tell the compiler we are explicitly using the following classes from the base class (this stops hidden overloaded virtual function warnings)
    using MLBase::train;
    
protected:
    /**
     Saves the core clusterer settings to a file.
     
     @return returns true if the base settings were saved, false otherwise
     */
    bool saveClustererSettingsToFile( std::fstream &file ) const;
    
    /**
     Loads the core clusterer settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadClustererSettingsFromFile( std::fstream &file );

    UINT numClusters;                   ///< Number of clusters in the model
    UINT predictedClusterLabel;         ///< Stores the predicted cluster label from the most recent predict( )
    Float maxLikelihood;
    Float bestDistance;
    VectorFloat clusterLikelihoods;
    VectorFloat clusterDistances;
    Vector< UINT > clusterLabels;
    bool converged;
    Vector<MinMax> ranges;
    
    static StringClustererMap *getMap() {
        if( !stringClustererMap ){ stringClustererMap = new StringClustererMap; } 
        return stringClustererMap; 
    }
    
private:
    static StringClustererMap *stringClustererMap;
    static UINT numClustererInstances;
    
};
    
template< typename T >  Clusterer* createNewClustererModuleInstance() { return new T; } ///< Returns a pointer to a new instance of the template class, the caller is responsible for deleting the pointer

/**
 @brief This class provides an interface for classes to register themselves with the clusterer base class, this enables Cluterer algorithms to
 be automatically be created from just a string, e.g.: Clusterer *kmeans = create( "KMeans" );
*/
template< typename T >
class RegisterClustererModule : Clusterer { 
public:
    RegisterClustererModule( const std::string &newModuleId ) { 
        getMap()->insert( std::pair< std::string, Clusterer*(*)() >(newModuleId, &createNewClustererModuleInstance< T > ) );
    }
};

GRT_END_NAMESPACE

#endif //GRT_CLUSTERER_HEADER

