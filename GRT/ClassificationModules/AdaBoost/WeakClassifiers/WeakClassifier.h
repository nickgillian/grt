/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This is the main base class for all GRT WeakClassifiers.
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

#ifndef GRT_WEAK_CLASSIFIER_HEADER
#define GRT_WEAK_CLASSIFIER_HEADER

#include "../../../Util/GRTCommon.h"
#include "../../../DataStructures/ClassificationData.h"

GRT_BEGIN_NAMESPACE
    
#define WEAK_CLASSIFIER_POSITIVE_CLASS_LABEL 1
#define WEAK_CLASSIFIER_NEGATIVE_CLASS_LABEL 2
    
class GRT_API WeakClassifier{
public:
    /**
     Default Constructor.
     */
    WeakClassifier();
    
    /**
     Default Destructor.
     */
    virtual ~WeakClassifier();
    
    /**
     Copy Constructor. Defines how the data from the rhs instance are copied to this instance.
     
     @param rhs: the rhs instance from which the data will be copied to this instance
     */
    WeakClassifier(const WeakClassifier &rhs){
        *this = rhs;
    }
    
    /**
     Equals Operator. Defines how the data from the rhs instance are copied to this instance.
     
     @param rhs: the rhs instance from which the data will be copied to this instance
     @return returns a reference to this WeakClassifier instance
     */
    WeakClassifier& operator=(const WeakClassifier &rhs);
    
    /**
     This function copies the WeakClassifier base variables from the weakClassifer pointer to this instance.
     
     @param weakClassifer: the instance from which the data will be copied to this instance
     @return returns true if the base variables were copied, false otherwise
     */
    bool copyBaseVariables(const WeakClassifier *weakClassifer);
    
    /**
     This function performs a deep copy of the inherited classes variables from the weakClassifer pointer instance to this instance.
     This function should be overwritten in the inheriting class.
     
     @param const WeakClassifier *weakClassifer: the instance from which the data will be deep copied to this instance
     @return returns true if the deep copy was successful, false otherwise
     */
    virtual bool deepCopyFrom(const WeakClassifier *weakClassifer){
        return false;
    }
    
    /**
     This function is the main training interface for all the WeakClassifiers.
     This function should be overwritten in the inheriting class.
     
     @param trainingData: a reference to the training data that will be used to train the weak classifier model
     @param weights: the weight for each training sample, there should be as many weights as there are training samples
     @return returns true if the weak classifier model was trained successful, false otherwise
     */
    virtual bool train(ClassificationData &trainingData, VectorFloat &weights){
        return false;
    }
    
    /**
     This function is the main predict interface for all the WeakClassifiers.
     This function should be overwritten in the inheriting class.
     
     @param x: the input vector to be classified, should have the same dimensionality as the data used to train the model
     @return returns a Float value representing the prediction, which is normally -1 or +1
     */
    virtual Float predict(const VectorFloat &x){
        return 0;
    }
    
    /**
     This function can be used to save the WeakClassifier model and settings.
     This function should be overwritten in the inheriting class.
     
     @param file: a reference to the file that the model will be saved to
     @return returns true if the data was saved, false otherwise
     */
    virtual bool saveModelToFile( std::fstream &file ) const{ return false; }
    
    /**
     This function can be used to load the WeakClassifier model and settings.
     This function should be overwritten in the inheriting class.
     
     @param file: a reference to the file that the model will be saved to
     @return returns true if the data was loaded, false otherwise
     */
    virtual bool loadModelFromFile( std::fstream &file ){ return false; }
    
    /**
     This function can be used to print the WeakClassifier model and settings.
     This function should be overwritten in the inheriting class.
     */
    virtual void print() const{}
    
    /**
     @return returns the positive class label for this WeakClassifier
     */
    virtual Float getPositiveClassLabel() const{ return 1; }
    
    /**
     @return returns the negative class label for this WeakClassifier
     */
    virtual Float getNegativeClassLabel() const{ return -1; }
    
    /**
     @return returns the weak classifier type as a string, e.g. DecisionStump
     */
    std::string getWeakClassifierType() const{
        return weakClassifierType;
    }
    
    /**
     @return returns true if the WeakClassifier model has been trained, false otherwise
     */
    bool getTrained() const{
        return trained;
    }
    
    /**
     @return returns the number of input dimensions expected by the WeakClassifier
     */
    UINT getNumInputDimensions() const{
        return numInputDimensions;
    }

    bool getTrainingLoggingEnabled() const{
        return trainingLog.getLoggingEnabled();
    }

    bool setTrainingLoggingEnabled(const bool enabled){
        return trainingLog.setLoggingEnabled(enabled);
    }
    
    /**
     Defines a map between a string (which will contain the name of the WeakClassifier, such as DecisionStump) and a function returns a new instance of that WeakClassifier
     */
    typedef std::map< std::string, WeakClassifier*(*)() > StringWeakClassifierMap;
    
    /**
     Creates a new WeakClassifier instance based on the input string (which should contain the name of a valid WeakClassifier such as DecisionStump).
     
     @param weakClassifierType: the name of the WeakClassifier
     @return WeakClassifier*: a pointer to the new instance of the WeakClassifier
     */
    static WeakClassifier* createInstanceFromString( std::string const &weakClassifierType );
    
    /**
     Creates a new WeakClassifier instance based on the current weakClassifierType string value.
     
     @return WeakClassifier*: a pointer to the new instance of the WeakClassifier
     */
    WeakClassifier* createNewInstance() const;
    
protected:
    std::string weakClassifierType;  ///<A string that represents the weak classifier type, e.g. DecisionStump
    bool trained;               ///<A flag to show if the weak classifier model has been trained
    UINT numInputDimensions;    ///<The number of input dimensions to the weak classifier
    TrainingLog trainingLog;
    ErrorLog errorLog;
    WarningLog warningLog;
    
    static StringWeakClassifierMap *getMap() {
        if( !stringWeakClassifierMap ){ stringWeakClassifierMap = new StringWeakClassifierMap; }
        return stringWeakClassifierMap;
    }
    
private:
    static StringWeakClassifierMap *stringWeakClassifierMap;
    static UINT numWeakClassifierInstances;
};
    
//These two functions/classes are used to register any new WeakClassification Module with the WeakClassifier base class
template< typename T >  WeakClassifier *newWeakClassificationModuleInstance() { return new T; }

template< typename T >
class RegisterWeakClassifierModule : WeakClassifier {
public:
    RegisterWeakClassifierModule( std::string const &newWeakClassificationModuleName ) {
        getMap()->insert( std::pair< std::string, WeakClassifier*(*)() >(newWeakClassificationModuleName, &newWeakClassificationModuleInstance< T > ) );
    }
};

GRT_END_NAMESPACE

#endif //GRT_WEAK_CLASSIFIER_HEADER
