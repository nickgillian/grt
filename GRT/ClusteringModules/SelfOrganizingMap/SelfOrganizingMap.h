/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This class implements the Self Oganizing Map clustering algorithm.
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

#ifndef GRT_SELF_ORGANIZING_MAP_HEADER
#define GRT_SELF_ORGANIZING_MAP_HEADER

#include "../../Util/GRTCommon.h"
#include "../../CoreModules/Clusterer.h"
#include "../../Util/Random.h"

GRT_BEGIN_NAMESPACE
    
class GaussNeuron{
public:
    GaussNeuron(){
        numInputs = 0;
        sigma = 0;
        initialized = false;
    }
    
    ~GaussNeuron(){
        
    }
    
    Float& operator[](const UINT index){
        return weights[ index ];
    }

    const Float& operator[](const UINT index) const{
        return weights[ index ];
    }
    
    bool init(const UINT numInputs,const Float sigma = 2.0,const Float minWeightRange = -1.0,const Float maxWeightRange = 1.0){
        
        this->numInputs = numInputs;
        this->sigma = sigma;
        weights.resize( numInputs );
        
        //Set the random seed
        Random random;
        random.setSeed( (unsigned long long)time(NULL) );
        
        //Randomise the weights between [minWeightRange maxWeightRange]
        for(unsigned int i=0; i<numInputs; i++){
            weights[i] = random.getRandomNumberUniform(minWeightRange,maxWeightRange);
        }
        
        initialized = true;

        return true;
    }
    
    bool clear(){
        
        numInputs = 0;
        weights.clear();
        initialized = false;
        return true;
        
    }
    
    bool getInitialized() const {
        return initialized;
    }
    
    Float getWeightDistance( const VectorFloat &x ) const {
        
        Float dist = 0;
        
        for(UINT i=0; i<numInputs; i++){
            dist += x[i]- weights[i];
        }
        
        return dist;
    }
    
    Float getSquaredWeightDistance( const VectorFloat &x ) const {
        
        Float dist = 0;
        
        for(UINT i=0; i<numInputs; i++){
            dist += grt_sqr( x[i]- weights[i] );
        }
        
        return dist;
    }
    
    Float fire( const VectorFloat &x ) const {
        Float y = 0;
        
        for(UINT i=0; i<numInputs; i++){
            y += grt_sqr( x[i]- weights[i] );
        }
        
        return exp( - (y/(2*grt_sqr(sigma))) );
    }
    
    bool save( std::fstream &file ) const {
        
        if( !file.is_open() ){
            return false;
        }
        
        if( !initialized ){
            return false;
        }
        
        file << "GAUSS_NEURON\n";
        file << "NumInputs: " << numInputs << std::endl;
        file << "Weights: ";
        for(UINT i=0; i<numInputs; i++){
            file << weights[i];
            if( i < numInputs-1 ) file << "\t";
        }
        file << std::endl;
        file << "Sigma: " << sigma << std::endl;
        
        return true;
    }
    
    bool load( std::fstream &file ){
        
        if( !file.is_open() ){
            return false;
        }
    
        clear();
        
        std::string word;
        
        //Read the header
        file >> word;
        if( word != "GAUSS_NEURON" ){
            return false;
        }
        
        //Read the num inputs
        file >> word;
        if( word != "NumInputs:" ){
            return false;
        }
        file >> numInputs;
        
        //Resize the weight Vector
        weights.resize( numInputs );
        
        //Read the weights header
        file >> word;
        if( word != "Weights:" ){
            return false;
        }

        //Load the weights
        for(UINT i=0; i<numInputs; i++){
            file >> weights[i];
        }
        
        //Load the sigma value
        file >> word;
        if( word != "Sigma:" ){
            return false;
        }
        file >> sigma;
        
        initialized = true;
        
        return true;
    }

    UINT numInputs;
    VectorFloat weights;
    Float sigma;
    bool initialized;
};

class GRT_API SelfOrganizingMap : public Clusterer{
        
public:
    enum NetworkTypology{RANDOM_NETWORK=0};
    
	/**
     Default Constructor.
     */
	SelfOrganizingMap(const UINT networkSize = 5, const UINT networkTypology = RANDOM_NETWORK, const UINT maxNumEpochs = 1000,const Float sigmaWeight = 0.2, const Float alphaStart = 0.3, const Float alphaEnd = 0.1);
    
    /**
     Defines how the data from the rhs SelfOrganizingMap should be copied to this SelfOrganizingMap
     
     @param rhs: another instance of a SelfOrganizingMap
     @return returns a reference to this instance of the SelfOrganizingMap
     */
    SelfOrganizingMap(const SelfOrganizingMap &rhs);
    
    /**
     Default Destructor.
     */
    virtual ~SelfOrganizingMap();
    
    /**
     Defines how the data from the rhs SelfOrganizingMap should be copied to this SelfOrganizingMap
     
     @param rhs: another instance of a SelfOrganizingMap
     @return returns a reference to this instance of the SelfOrganizingMap
     */
    SelfOrganizingMap &operator=(const SelfOrganizingMap &rhs);
    
	/**
     This deep copies the variables and models from the Clusterer pointer to this SelfOrganizingMap instance.
     This overrides the base deep copy function for the Clusterer modules.
     
     @param clusterer: a pointer to the Clusterer base class, this should be pointing to another SelfOrganizingMap instance
     @return returns true if the clone was successfull, false otherwise (the Clusterer base class will always return flase)
     */
    virtual bool deepCopyFrom(const Clusterer *clusterer);
    
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
     This is the main training interface for referenced MatrixFloat data. It overrides the train_ function in the ML base class.
     This function runs the main training algorithm and is called by all the other train and train_ functions.
     
     @param trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(MatrixFloat &trainingData);
    
    /**
     This is the main training interface for reference ClassificationData data. It overrides the train_ function in the ML base class.
     
     @param trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This is the main training interface for reference UnlabelledData data. It overrides the trainInplace function in the ML base class.
     
     @param trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
	virtual bool train_(UnlabelledData &trainingData);
    
    /**
     This function maps the input Vector x by reference through the self organizing map. 
     The function will return true if the mapping was successful.
     The mapped data can then be accessed via the getMappedData function. 
     You need to train the SOM model before you can use this function.
     Because the data is mapped by reference, the x input data might be modified by the map (if it has to scale the input data for example).
     
     @param x: the input Vector for mapping
     @return returns true if the mapping was completed succesfully, false otherwise
     */
    virtual bool map_( VectorFloat &x );
    
    /**
     This saves the trained SOM model to a file.
     This overrides the saveModelToFile function in the base class.
     
     @param file: a reference to the file the SOM model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool save( std::fstream &file ) const;
    
    /**
     This loads a trained SOM model from a file.
     This overrides the loadModelFromFile function in the base class.
     
     @param file: a reference to the file the SOM model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool load( std::fstream &file );
    
    /**
     This function validates the network typology to ensure it is one of the NetworkTypology enums.
     
     @param networkTypology: the network typology you want to test
     @return returns true if the network typology is valid, false otherwise
     */
    bool validateNetworkTypology( const UINT networkTypology );
    
    /**
     This function returns the size of the SOM network. This is the same as the number of clusters in the network.
     
     @param const UINT networkTypology: the network typology you want to test
     @return returns true if the network typology is valid, false otherwise
     */
    UINT getNetworkSize() const;
    
    Float getAlphaStart() const;
    
    Float getAlphaEnd() const;
    
    VectorFloat getMappedData() const;
    
    Matrix< GaussNeuron > getNeurons() const;
    
    const Matrix< GaussNeuron > &getNeuronsRef() const;

    Matrix< VectorFloat > getWeightsMatrix() const;
    
    bool setNetworkSize( const UINT networkSize );
    
    bool setNetworkTypology( const UINT networkTypology );
    
    bool setAlphaStart( const Float alphaStart );
    
    bool setAlphaEnd( const Float alphaEnd );

    bool setSigmaWeight( const Float sigmaWeight );
    
    //Tell the compiler we are using the base class train method to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;

    /**
    Gets a string that represents the SelfOrganizingMap class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
protected:
    UINT networkTypology;
    Float sigmaWeight;
    Float alphaStart;
    Float alphaEnd;
    VectorFloat mappedData;
    Matrix< GaussNeuron > neurons;
    
private:
    static RegisterClustererModule< SelfOrganizingMap > registerModule;
    static const std::string id;	
};
    
GRT_END_NAMESPACE

#endif //GRT_SELF_ORGANIZING_MAP_HEADER
