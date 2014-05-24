/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief This is the main base class that all GRT Classification algorithms should inherit from.
 
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

#ifndef GRT_CLASSIFIER_HEADER
#define GRT_CLASSIFIER_HEADER

#include "MLBase.h"

namespace GRT{
    
#define DEFAULT_NULL_LIKELIHOOD_VALUE 0
#define DEFAULT_NULL_DISTANCE_VALUE 0

class Classifier : public MLBase
{
public:
    /**
     Default Classifier Constructor
     */
	Classifier(void);
    
    /**
     Default Classifier Destructor
     */
	virtual ~Classifier(void);
    
    /**
     This is the base deep copy function for the Classifier modules. This function should be overwritten by the derived class.
     This deep copies the variables and models from the classifier pointer to this classifier instance.
     
     @param const Classifier *classifier: a pointer to the Classifier base class, this should be pointing to another instance of a matching derived class
     @return returns true if the clone was successfull, false otherwise (the Classifier base class will always return flase)
     */
    virtual bool deepCopyFrom(const Classifier *classifier){ return false; }
    
    /**
     This copies the Classifier base class variables from the classifier pointer to this instance.
     
     @param const Classifier *classifier: a pointer to a classifier from which the values will be copied to this instance
     @return returns true if the copy was successfull, false otherwise
     */
    bool copyBaseVariables(const Classifier *classifier);
    
    /**
     This resets the classifier.
     This overrides the reset function in the MLBase base class.
     
     @return returns true if the classifier was reset, false otherwise
     */
    virtual bool reset();
    
    /**
     This function clears the classifier module, removing any trained model and setting all the base variables to their default values.
     
     @return returns true if the derived class was cleared succesfully, false otherwise
     */
    virtual bool clear();

    /**
     Returns the classifeir type as a string.
     
     @return returns the classifier type as a string
     */
    string getClassifierType() const;
    
    /**
     Returns true if nullRejection is enabled.
     
     @return returns true if nullRejection is enabled, false otherwise
     */
    bool getNullRejectionEnabled() const;
    
    /**
     Returns the current nullRejectionCoeff value.
     The nullRejectionCoeff parameter is a multipler controlling the null rejection threshold for each class.
     
     @return returns the current nullRejectionCoeff value
     */
    double getNullRejectionCoeff() const;
    
    /**
     Returns the current maximumLikelihood value.
     The maximumLikelihood value is computed during the prediction phase and is the likelihood of the most likely model.
     This value will return 0 if a prediction has not been made.
     
     @return returns the current maximumLikelihood value
     */
    double getMaximumLikelihood() const;
    
    /**
     Returns the current bestDistance value.
     The bestDistance value is computed during the prediction phase and is either the minimum or maximum distance, depending on the algorithm.
     This value will return 0 if a prediction has not been made.
     
     @return returns the current bestDistance value
     */
    double getBestDistance() const;
    
    /**
     Gets the number of classes in trained model.
     
     @return returns the number of classes in the trained model, a value of 0 will be returned if the model has not been trained
     */
    virtual UINT getNumClasses() const;
    
    /**
     Gets the index of the query classLabel in the classLabels vector. If the query classLabel does not exist in the classLabels vector
     then the function will return zero.
     
     @param UINT classLabel: the query classLabel
     @return returns index of the query classLabel in the classLabels vector
     */
    UINT getClassLabelIndexValue(UINT classLabel) const;
    
    /**
     Gets the predicted class label from the last prediction.
     
     @return returns the label of the last predicted class, a value of 0 will be returned if the model has not been trained
     */
    UINT getPredictedClassLabel() const;
    
    /**
     Gets a vector of the class likelihoods from the last prediction, this will be an N-dimensional vector, where N is the number of classes in the model.  
     The exact form of these likelihoods depends on the classification algorithm.
     
     @return returns a vector of the class likelihoods from the last prediction, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getClassLikelihoods() const;
    
    /**
     Gets a vector of the class distances from the last prediction, this will be an N-dimensional vector, where N is the number of classes in the model.  
     The exact form of these distances depends on the classification algorithm.
     
     @return returns a vector of the class distances from the last prediction, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getClassDistances() const;
    
    /**
     Gets a vector containing the null rejection thresholds for each class, this will be an N-dimensional vector, where N is the number of classes in the model.  
     
     @return returns a vector containing the null rejection thresholds for each class, an empty vector will be returned if the model has not been trained
     */
    VectorDouble getNullRejectionThresholds() const;
    
    /**
     Gets a vector containing the label each class represents, this will be an N-dimensional vector, where N is the number of classes in the model. 
     This is useful if the model was trained with non-monotonically class labels (i.e. class labels such as [1, 3, 6, 9, 12] instead of [1, 2, 3, 4, 5]).
     
     @return returns a vector containing the class labels for each class, an empty vector will be returned if the model has not been trained
     */
    vector< UINT > getClassLabels() const;
    
    /**
     Gets a vector of the ranges used to scale the data for training and prediction, these ranges are only used if the classifier has been trained
     with the #useScaling flag set to true. This should be an N-dimensional vector, where N is the number of features in your data.
     
     @return returns a vector containing the ranges used to scale the data for classification, an empty vector will be returned if the model has not been trained
     */
    vector<MinMax> getRanges() const;

    /**
     Sets if the classifier should use nullRejection.
     
     If set to true then the classifier will reject a predicted class label if the likelihood of the prediction is below (or above depending on the 
     algorithm) the models rejectionThreshold. If a prediction is rejected then the default null class label of 0 will be returned.
     If set to false then the classifier will simply return the most likely predicted class.
     
     @return returns true if nullRejection was updated successfully, false otherwise
     */
    bool enableNullRejection(bool useNullRejection);
    
    /**
     Sets the nullRejectionCoeff, this is a multipler controlling the null rejection threshold for each class.
     
     @return returns true if nullRejectionCoeff was updated successfully, false otherwise
     */
    virtual bool setNullRejectionCoeff(double nullRejectionCoeff);

	/**
     Manually sets the nullRejectionThresholds, these are the thresholds used for null rejection for each class.
	 This needs to be called after the model has been trained. Calling the #setNullRejectionCoeff or #recomputeNullRejectionThresholds
	 functions will override these values. The size of the newRejectionThresholds vector must match the number of classes in the model.
     
	 @param VectorDouble newRejectionThresholds: the new rejection thresholds
     @return returns true if nullRejectionThresholds were updated successfully, false otherwise
     */
	virtual bool setNullRejectionThresholds(VectorDouble newRejectionThresholds);
    
    /**
     Recomputes the null rejection thresholds for each model.
     
     @return returns true if the nullRejectionThresholds were updated successfully, false otherwise
     */
    virtual bool recomputeNullRejectionThresholds(){ return false; }
    
    /**
     Indicates if the classifier can be used to classify timeseries data.
     If true then the classifier can accept training data in the LabelledTimeSeriesClassificationData format.
     
     return returns true if the classifier can be used to classify timeseries data, false otherwise
     */
    bool getTimeseriesCompatible() const{ return classifierMode==TIMESERIES_CLASSIFIER_MODE; }
    
    /**
     Defines a map between a string (which will contain the name of the classifier, such as ANBC) and a function returns a new instance of that classifier
     */
    typedef std::map< string, Classifier*(*)() > StringClassifierMap;
    
    /**
     Creates a new classifier instance based on the input string (which should contain the name of a valid classifier such as ANBC).
     
     @param string const &classifierType: the name of the classifier
     @return Classifier*: a pointer to the new instance of the classifier
     */
    static Classifier* createInstanceFromString(string const &classifierType);
    
    /**
     Creates a new classifier instance based on the current classifierType string value.
     
     @return Classifier*: a pointer to the new instance of the classifier
    */
    Classifier* createNewInstance() const;
    
    /**
     This creates a new Classifier instance and deep copies the variables and models from this instance into the deep copy.
     The function will then return a pointer to the new instance. It is up to the user who calls this function to delete the dynamic instance
     when they are finished using it.
     
     @return returns a pointer to a new Classifier instance which is a deep copy of this instance
     */
    Classifier* deepCopy() const;
    
    /**
     Returns a pointer to the classifier.
     
     @return returns a pointer the current classifier
     */
    const Classifier* getClassifierPointer() const;
    
    /**
     Returns a pointer to this classifier. This is useful for a derived class so it can get easy access to this base classifier.
     
     @return Classifier&: a reference to this classifier
     */
    const Classifier& getBaseClassifier() const;

    /**
     Returns a vector of the names of all classifiers that have been registered with the base classifier.
     
     @return vector< string >: a vector containing the names of the classifiers that have been registered with the base classifier
    */
	static vector< string > getRegisteredClassifiers();
    
protected:
    /**
     Saves the core base settings to a file.
     
     @return returns true if the base settings were saved, false otherwise
     */
    bool saveBaseSettingsToFile(fstream &file) const;
    
    /**
     Loads the core base settings from a file.
     
     @return returns true if the base settings were loaded, false otherwise
     */
    bool loadBaseSettingsFromFile(fstream &file);

    string classifierType;
    bool useNullRejection;
    UINT numClasses;
    UINT predictedClassLabel;
    UINT classifierMode;
    double nullRejectionCoeff;
    double maxLikelihood;
    double bestDistance;
    VectorDouble classLikelihoods;
    VectorDouble classDistances;
    VectorDouble nullRejectionThresholds;
    vector< UINT > classLabels;
    vector<MinMax> ranges;
    
    static StringClassifierMap *getMap() {
        if( !stringClassifierMap ){ stringClassifierMap = new StringClassifierMap; } 
        return stringClassifierMap; 
    }
        
    enum ClassifierModes{STANDARD_CLASSIFIER_MODE=0,TIMESERIES_CLASSIFIER_MODE};
    
private:
    static StringClassifierMap *stringClassifierMap;
    static UINT numClassifierInstances;
    
};
    
//These two functions/classes are used to register any new Classification Module with the Classifier base class
template< typename T >  Classifier* getNewClassificationModuleInstance() { return new T; }

template< typename T >
class RegisterClassifierModule : public Classifier {
public:
    RegisterClassifierModule(string const &newClassificationModuleName) { 
        getMap()->insert( std::pair<string, Classifier*(*)()>(newClassificationModuleName, &getNewClassificationModuleInstance< T > ) );
    }
};

} //End of namespace GRT

#endif //GRT_CLASSIFIER_HEADER

