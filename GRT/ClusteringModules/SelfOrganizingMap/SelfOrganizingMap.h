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

namespace GRT{
    
class GaussNeuron{
public:
    GaussNeuron(){
        numInputs = 0;
        sigma = 0;
        initialized = false;
    }
    
    ~GaussNeuron(){
        
    }
    
    double& operator[](const UINT index){
        return weights[ index ];
    }
    
    bool init(const UINT numInputs,const double sigma = 2.0){
        
        this->numInputs = numInputs;
        this->sigma = sigma;
        weights.resize( numInputs );
        
        //Set the random seed
        Random random;
        random.setSeed( (unsigned long long)time(NULL) );
        
        //Randomise the weights between [0.4 0.6]
        for(unsigned int i=0; i<numInputs; i++){
            weights[i] = random.getRandomNumberUniform(0.4,0.6);
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
    
    double getWeightDistance( const VectorDouble &x ) const {
        
        double dist = 0;
        
        for(UINT i=0; i<numInputs; i++){
            dist += x[i]- weights[i];
        }
        
        return dist;
    }
    
    double getSquaredWeightDistance( const VectorDouble &x ) const {
        
        double dist = 0;
        
        for(UINT i=0; i<numInputs; i++){
            dist += SQR( x[i]- weights[i] );
        }
        
        return dist;
    }
    
    double fire( const VectorDouble &x ) const {
        double y = 0;
        
        for(UINT i=0; i<numInputs; i++){
            y += SQR( x[i]- weights[i] );
        }
        
        return exp( - (y/(2*SQR(sigma))) );
    }
    
    bool saveNeuronToFile(fstream &file) const {
        
        if( !file.is_open() ){
            return false;
        }
        
        if( !initialized ){
            return false;
        }
        
        file << "GAUSS_NEURON\n";
        file << "NumInputs: " << numInputs << endl;
        file << "Weights: ";
        for(UINT i=0; i<numInputs; i++){
            file << weights[i];
            if( i < numInputs-1 ) file << "\t";
        }
        file << endl;
        file << "Sigma: " << sigma << endl;
        
        return true;
    }
    
    bool loadNeuronFromFile(fstream &file){
        
        if( !file.is_open() ){
            return false;
        }
    
        clear();
        
        string word;
        
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
        
        //Resize the weight vector
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
    VectorDouble weights;
    double sigma;
    bool initialized;
};

class SelfOrganizingMap : public Clusterer{
        
public:
	/**
     Default Constructor.
     */
	SelfOrganizingMap(const UINT networkSize = 20, const UINT networkTypology = RANDOM_NETWORK, const UINT maxNumEpochs = 1000,const double alphaStart = 0.8, const double alphaEnd = 0.1);
    
    /**
     Defines how the data from the rhs SelfOrganizingMap should be copied to this SelfOrganizingMap
     
     @param const SelfOrganizingMap &rhs: another instance of a SelfOrganizingMap
     @return returns a reference to this instance of the SelfOrganizingMap
     */
    SelfOrganizingMap(const SelfOrganizingMap &rhs);
    
    /**
     Default Destructor.
     */
    virtual ~SelfOrganizingMap();
    
    /**
     Defines how the data from the rhs SelfOrganizingMap should be copied to this SelfOrganizingMap
     
     @param const SelfOrganizingMap &rhs: another instance of a SelfOrganizingMap
     @return returns a reference to this instance of the SelfOrganizingMap
     */
    SelfOrganizingMap &operator=(const SelfOrganizingMap &rhs);
    
	/**
     This deep copies the variables and models from the Clusterer pointer to this SelfOrganizingMap instance.
     This overrides the base deep copy function for the Clusterer modules.
     
     @param const Clusterer *clusterer: a pointer to the Clusterer base class, this should be pointing to another SelfOrganizingMap instance
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
     This is the main training interface for referenced MatrixDouble data. It overrides the train_ function in the ML base class.
     This function runs the main training algorithm and is called by all the other train and train_ functions.
     
     @param MatrixDouble &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(MatrixDouble &data);
    
    /**
     This is the main training interface for reference ClassificationData data. It overrides the train_ function in the ML base class.
     
     @param ClassificationData &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
     This is the main training interface for reference UnlabelledData data. It overrides the trainInplace function in the ML base class.
     
     @param UnlabelledData &trainingData: a reference to the training data that will be used to train the ML model
     @return returns true if the model was successfully trained, false otherwise
     */
	virtual bool train_(UnlabelledData &trainingData);
    
    /**
     This function maps the input vector x by reference through the self organizing map. 
     The function will return true if the mapping was successful.
     The mapped data can then be accessed via the getMappedData function. 
     You need to train the SOM model before you can use this function.
     Because the data is mapped by reference, the x input data might be modified by the map (if it has to scale the input data for example).
     
     @param VectorDouble &inputVector: the input vector for mapping
     @return returns true if the mapping was completed succesfully, false otherwise
     */
    virtual bool map_( VectorDouble &x );
    
    /**
     This saves the trained SOM model to a file.
     This overrides the saveModelToFile function in the base class.
     
     @param string filename: the name of the file to save the SOM model to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(string filename) const;
    
    /**
     This saves the trained SOM model to a file.
     This overrides the saveModelToFile function in the base class.
     
     @param fstream &file: a reference to the file the SOM model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool saveModelToFile(fstream &file) const;
    
    /**
     This loads a trained SOM model from a file.
     This overrides the loadModelFromFile function in the base class.
     
     @param string filename: the name of the file to load the SOM model from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(string filename);
    
    /**
     This loads a trained SOM model from a file.
     This overrides the loadModelFromFile function in the base class.
     
     @param fstream &file: a reference to the file the SOM model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool loadModelFromFile(fstream &file);
    
    /**
     This function validates the network typology to ensure it is one of the NetworkTypology enums.
     
     @param const UINT networkTypology: the network typology you want to test
     @return returns true if the network typology is valid, false otherwise
     */
    bool validateNetworkTypology( const UINT networkTypology );
    
    /**
     This function returns the size of the SOM network. This is the same as the number of clusters in the network.
     
     @param const UINT networkTypology: the network typology you want to test
     @return returns true if the network typology is valid, false otherwise
     */
    UINT getNetworkSize() const;
    
    double getAlphaStart() const;
    
    double getAlphaEnd() const;
    
    VectorDouble getMappedData() const;
    
    vector< GaussNeuron > getNeurons() const;
    
    const vector< GaussNeuron > &getNeuronsRef() const;
    
    MatrixDouble getNetworkWeights() const;
    
    bool setNetworkSize( const UINT networkSize );
    
    bool setNetworkTypology( const UINT networkTypology );
    
    bool setAlphaStart( const double alphaStart );
    
    bool setAlphaEnd( const double alphaEnd );
    
protected:
    UINT networkTypology;
    double alphaStart;
    double alphaEnd;
    VectorDouble mappedData;
    vector< GaussNeuron > neurons;
    MatrixDouble networkWeights;
    
private:
    static RegisterClustererModule< SelfOrganizingMap > registerModule;
    
public:
    
    enum NetworkTypology{RANDOM_NETWORK=0};
		
};
    
}//End of namespace GRT

#endif //GRT_SELF_ORGANIZING_MAP_HEADER
