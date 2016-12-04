/**
@file
@author  Nicholas Gillian <ngillian@media.mit.edu>
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

#ifndef GRT_SVM_HEADER
#define GRT_SVM_HEADER

#include "../../CoreModules/Classifier.h"
#include "LIBSVM/libsvm.h"

GRT_BEGIN_NAMESPACE

#define SVM_MIN_SCALE_RANGE -1.0
#define SVM_MAX_SCALE_RANGE 1.0

/**
@brief This class acts as a front end for the LIBSVM library (http://www.csie.ntu.edu.tw/~cjlin/libsvm/).
It implements a Support Vector Machine (SVM) classifier, a powerful classifier that works well on a wide
range of classification problems, particularly on more complex problems that other classifiers (such as
the KNN, GMM or ANBC algorithms) might not be able to solve.

@example ClassificationModulesExamples/SVMExample/SVMExample.cpp

@remark This implementation is a wrapper for LIBSVM: Chang, Chih-Chung, and Chih-Jen Lin. "LIBSVM: a library for support vector machines." ACM Transactions on Intelligent Systems and Technology (TIST) 2, no. 3 (2011): 27.
*/
class GRT_API SVM : public Classifier{
public:
    enum SVMType{ C_SVC = 0, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR };
    enum KernelType{ LINEAR_KERNEL = 0, POLY_KERNEL, RBF_KERNEL, SIGMOID_KERNEL, PRECOMPUTED_KERNEL };

    /**
     Default constructor.
     
     Set the initial SVM settings, although these can be changed at any time using either init(...) function of the set... functions.
     
     @param kernelType: this sets the SVM kernelType. Options are LINEAR_KERNEL, POLY_KERNEL, RBF_KERNEL, SIGMOID_KERNEL, PRECOMPUTED_KERNEL. The default kernelType is kernelType=LINEAR_KERNEL
     @param svmType: this sets the SVM type. Options are C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR. The default svmType is svmType=C_SVC
     @param useScaling: sets if the training/prediction data will be scaled to the default range of [-1. 1.]. The SVM algorithm commonly achieves a better classification result if scaling is turned on. The default useScaling value is useScaling=true
     @param useNullRejection: sets if a predicted class will be rejected if the classes' probability is below the classificationThreshold. The default value is useNullRejection=false
     @param useAutoGamma: sets if the SVM gamma parameter will automatically be computed, if set to true then gamma will be set to (1.0/numFeatures), where numFeatures is the number of features in the training data. The default value is useAutoGamma=true
     @param gamma: sets the SVM gamma parameter. The default value is gamma=0.1
     @param degree: sets the SVM degree parameter. The default value is degree=3
     @param coef0: sets the SVM coef0 parameter. The default value is coef0=0
     @param nu: sets the SVM nu parameter. The default value is nu=0.5
     @param C: sets the SVM C parameter. The default value is C=1
     @param useCrossValidation: sets if the SVM model will be trained using cross validation. The default value is useCrossValidation=false
     @param kFoldValue: sets the number of folds that will be used for cross validation. The default value is kFoldValue=10
     */
	SVM(KernelType kernelType = LINEAR_KERNEL,SVMType svmType = C_SVC,bool useScaling = true,bool useNullRejection = false,bool useAutoGamma = true,Float gamma = 0.1,UINT degree = 3,Float coef0 = 0,Float nu = 0.5,Float C = 1,bool useCrossValidation = false,UINT kFoldValue = 10);
    
    /**
    Default copy constructor.  Copies the settings from the rhs SVM instance to this instance
    
    @param rhs: another instance of a SVM
    */
    SVM(const SVM &rhs);
    
    /**
    Default Destructor
    */
    virtual ~SVM();
    
    /**
    Defines how the data from the rhs SVM should be copied to this SVM
    
    @param rhs: another instance of a SVM
    @return returns a pointer to this instance of the SVM
    */
    SVM &operator=(const SVM &rhs);
    
    /**
    This is required for the Gesture Recognition Pipeline for when the pipeline.setClassifier(...) method is called.
    It clones the data from the Base Class Classifier pointer (which should be pointing to an SVM instance) into this instance
    
    @param classifier: a pointer to the Classifier Base Class, this should be pointing to another SVM instance
    @return returns true if the clone was successfull, false otherwise
    */
    virtual bool deepCopyFrom(const Classifier *classifier);
    
    /**
    This trains the SVM model, using the labelled classification data.
    This overrides the train function in the Classifier base class.
    
    @param trainingData: a reference to the training data
    @return returns true if the SVM model was trained, false otherwise
    */
    virtual bool train_(ClassificationData &trainingData);
    
    /**
    This predicts the class of the inputVector.
    This overrides the predict function in the Classifier base class.
    
    @param inputVector: the input vector to classify
    @return returns true if the prediction was performed, false otherwise
    */
    virtual bool predict_(VectorFloat &inputVector);
    
    /**
     Clears any previous model or problem.
     */
	virtual bool clear();
    
    /**
     This saves the trained SVM model to a file.
     This overrides the save function in the Classifier base class.
     
     @param file: a reference to the file the SVM model will be saved to
     @return returns true if the model was saved successfully, false otherwise
     */
    virtual bool save( std::fstream &file ) const;
    
    /**
     This loads a trained SVM model from a file.
     This overrides the load function in the Classifier base class.
     
     @param file: a reference to the file the SVM model will be loaded from
     @return returns true if the model was loaded successfully, false otherwise
     */
    virtual bool load( std::fstream &file );
    
    /**
     This initializes the SVM settings and parameters.  Any previous model, settings, or problems will be cleared.
     
     @param kernelType: this sets the SVM kernelType. Options are LINEAR_KERNEL, POLY_KERNEL, RBF_KERNEL, SIGMOID_KERNEL, PRECOMPUTED_KERNEL     @param UINT svmType: this sets the SVM type. Options are C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR
     @param useScaling: sets if the training/prediction data will be scaled to the default range of [-1. 1.]. The SVM algorithm commonly achieves a better classification result if scaling is turned on
     @param useNullRejection: sets if a predicted class will be rejected if the classes' probability is below the classificationThreshold
     @param useAutoGamma: sets if the SVM gamma parameter will automatically be computed, if set to true then gamma will be set to (1.0/numFeatures), where numFeatures is the number of features in the training data
     @param gamma: sets the SVM gamma parameter
     @param degree: sets the SVM degree parameter
     @param coef0: sets the SVM coef0 parameter
     @param nu: sets the SVM nu parameter
     @param C: sets the SVM C parameter
     @param useCrossValidation: sets if the SVM model will be trained using cross validation
     @param kFoldValue: sets the number of folds that will be used for cross validation
     @return returns true if the SVM was initialized, false otherwise
     */
    bool init(KernelType kernelType,SVMType svmType,bool useScaling,bool useNullRejection,bool useAutoGamma,Float gamma,UINT degree,Float coef0,Float nu,Float C,bool useCrossValidation,UINT kFoldValue);
    
    /**
    This initializes the default SVM settings and parameters. Any previous model, settings, or problems will be cleared.
    */
    void initDefaultSVMSettings();
    
    /**
    Gets if cross validation is enabled.
    
    @return returns true if cross validation is enabled, false otherwise
    */
    bool getIsCrossValidationTrainingEnabled() const;
    
    /**
    Gets if the gamma parameter will be automatically computed from the training data.
    If false, then the user needs to set the gamma parameter.
    
    @return returns true if the gamma parameter will be automatically computed, false otherwise
    */
    bool getIsAutoGammaEnabled() const;
    
    /**
    Gets the current SVM type as a string.
    
    If the type has not been set correctly then the string value UNKNOWN will be returned.
    
    @return returns the current SVM type as a string.
    */
    std::string getSVMType() const;
    
    /**
    Gets the current kernel type as a string.
    
    If the type has not been set correctly then the string value UNKNOWN will be returned.
    
    @return returns the current kernel type as a string.
    */
    std::string getKernelType() const;
    
    /**
    Gets the current degree value.
    
    @return returns the current degree value.
    */
    UINT getDegree() const;
    
    /**
    Returns the number of classes in the trained model.
    
    If the model has not been trained then 0 will be returned.
    
    @return returns the number of classes in the trained model or 0 if the model has not been trained
    */
    virtual UINT getNumClasses() const;
    
    /**
    Gets the current gamma value.
    
    @return returns the current gamma value.
    */
    Float getGamma() const;
    
    /**
    Gets the current nu value.
    
    @return returns the current nu value.
    */
    Float getNu() const;
    
    /**
    Gets the current coef0 value.
    
    @return returns the current coef0 value.
    */
    Float getCoef0() const;
    
    /**
    Gets the current C value.
    
    @return returns the current C value.
    */
    Float getC() const;
    
    /**
    Gets the last cross validation result, if the model has been trained and cross validation was enabled.
    
    @return returns the last cross validation result.
    */
    Float getCrossValidationResult() const;
    
    /**
     Returns a pointer to the svm_model, this will be NULL if the model has not been trained.
     
     @return returns a pointer to the svm_model.
     */
    const struct LIBSVM::svm_model *getLIBSVMModel() const;
    
    /**
     Sets the SVM type.
     This should be one of the SVMTypes enumeration types.
     
     @param svmType: the new SVM type, options are C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR 
     @return returns true if the SVM type was set correctly, false otherwise
     */
    bool setSVMType(const SVMType svmType);
    
    /**
     Sets the kernel type.
     This should be one of the SVMKernelTypes enumeration types.
     
     @param kernelType: the new kernel, options are LINEAR_KERNEL, POLY_KERNEL, RBF_KERNEL, SIGMOID_KERNEL, PRECOMPUTED_KERNEL 
     @return returns true if the kernel type was set correctly, false otherwise
     */
    bool setKernelType(const KernelType kernelType);
    
    /**
    Sets the SVM gamma parameter.  The user should only try and set this value manually if the useAutoGamma parameter has been set to false.
    
    @param gamma: the new gamma value
    @return returns true if the gamma parameter was updated, false otherwise
    */
    bool setGamma(const Float gamma);
    
    /**
    Sets the SVM degree parameter.
    This is only used if the SVM kernel parameter is set to POLY_KERNEL.
    
    @param degree: the new degree value
    @return returns true if the degree parameter was updated, false otherwise
    */
    bool setDegree(const UINT degree);
    
    /**
    Sets the SVM nu parameter.
    This is only used if the SVM type parameter is set to NU_SVC, ONE_CLASS, or NU_SVR.
    
    @param nu: the new nu value
    @return returns true if the nu parameter was updated, false otherwise
    */
    bool setNu(const Float nu);
    
    /**
    Sets the SVM coef0 parameter.
    This is only used if the SVM kernel type parameter is set to POLY_KERNEL or SIGMOID_KERNEL.
    
    @param coef0: the new coef0 value
    @return returns true if the gamcoef0ma parameter was updated, false otherwise
    */
    bool setCoef0(const Float coef0);
    
    /**
    Sets the SVM C parameter.
    This is only used if the SVM type parameter is set to C_SVC, EPSILON_SVR or NU_SVR.
    
    @param C: the new C value
    @return returns true if the C parameter was updated, false otherwise
    */
    bool setC(const Float C);
    
    /**
    Sets the kFold cross validation value.
    
    @param kFoldValue: the new kFoldValue, must be greater than 0
    return returns true if the kFoldValue was set, false otherwise
    */
    bool setKFoldCrossValidationValue(const UINT kFoldValue);
    
    /**
    Sets if the gamma parameter will be automatically computed from the training data.
    
    @param useAutoGamma: the new useAutoGamma setting
    return returns true if the useAutoGamma was set, false otherwise
    */
    bool enableAutoGamma(const bool useAutoGamma);
    
    /**
    Sets if cross validation should be used during the training phase.
    
    @param useCrossValidation: the new useCrossValidation setting
    return returns true if the useCrossValidation was set, false otherwise
    */
    bool enableCrossValidationTraining(const bool useCrossValidation);

    /**
    Gets a string that represents the class.
    
    @return returns a string containing the ID of this class
    */
    static std::string getId();
    
    //Tell the compiler we are using the following functions from the MLBase class to stop hidden virtual function warnings
    using MLBase::save;
    using MLBase::load;
    using MLBase::train_;
    using MLBase::predict_;
    
protected:
    void deleteProblemSet();
    bool validateProblemAndParameters();
    bool validateSVMType(SVMType svmType);
    bool validateKernelType(KernelType kernelType);
    bool convertClassificationDataToLIBSVMFormat(ClassificationData &trainingData);
    bool trainSVM();
    
    bool predictSVM(VectorFloat &inputVector);
    bool predictSVM(VectorFloat &inputVector,Float &maxProbability, VectorFloat &probabilites);
    bool loadLegacyModelFromFile( std::fstream &file );
    
    struct LIBSVM::svm_model *deepCopyModel() const;
    bool deepCopyProblem( const struct LIBSVM::svm_problem &source_problem, struct LIBSVM::svm_problem &target_problem, const unsigned int numInputDimensions ) const;
    bool deepCopyParam( const LIBSVM::svm_parameter &source_param, LIBSVM::svm_parameter &target_param ) const;
    
    bool problemSet;
    struct LIBSVM::svm_model *model;
    struct LIBSVM::svm_parameter param;
    struct LIBSVM::svm_problem prob;
    UINT kFoldValue;
    Float classificationThreshold;
    Float crossValidationResult;
    bool useAutoGamma;
    bool useCrossValidation;
    
private:
    static RegisterClassifierModule< SVM > registerModule;
    static const std::string id;
};

GRT_END_NAMESPACE

#endif //GRT_SVM_HEADER
    