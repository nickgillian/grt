/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 @version 1.0
 
 @brief The TrainingResult class provides a data structure for storing the results of a classification or regression training iteration.
 */

#ifndef GRT_TRAINING_RESULT_HEADER
#define GRT_TRAINING_RESULT_HEADER

GRT_BEGIN_NAMESPACE
    
//Forward declartion of MLBase class
class MLBase;

class TrainingResult{
public:
    /**
     Default Constructor.
     
     Initializes the TrainingResult instance.
     */
    TrainingResult(){
        trainingMode = CLASSIFICATION_MODE;
        trainingIteration = 0;
        accuracy = 0;
        totalSquaredTrainingError = 0;
        rootMeanSquaredTrainingError = 0;
        trainer = NULL;
    }
    
    /**
     Copy Constructor.
     
     Initializes this instance by copying the data from the rhs instance
     
     @param const TrainingResult &rhs: another instance of the TrainingResult class
     */
    TrainingResult(const TrainingResult &rhs){
        *this = rhs;
    }
    
    /**
     Default Destructor.
     */
    ~TrainingResult(){
        
    }

    /**
     Defines the Equals Operator.

     This copies the data from the rhs instance to this instance, returning a reference to the current instance.

     @param const TrainingResult &rhs: another instance of the TrainingResult class
     */
    TrainingResult& operator=(const TrainingResult &rhs){
        if( this != &rhs ){
            this->trainingMode = rhs.trainingMode;
            this->trainingIteration = rhs.trainingIteration;
            this->accuracy = rhs.accuracy;
            this->totalSquaredTrainingError = rhs.totalSquaredTrainingError;
            this->rootMeanSquaredTrainingError = rhs.rootMeanSquaredTrainingError;
            this->trainer = rhs.trainer;
        }
        return *this;
    }
    
    /**
     Gets the current training mode, this will be one of the TrainingMode enums.

     @return returns the current training mode, this will be one of the TrainingMode enums
     */
    unsigned int getTrainingMode() const{
        return trainingMode;
    }
    
    /**
     Gets the training iteration, this represents which iteration (or epoch) the training results correspond to.

     @return returns the training iteration
     */
    unsigned int getTrainingIteration() const{
        return trainingIteration;
    }
    
    /**
     Gets the accuracy for the training result at the current training iteration. This is only used if the trainingMode is
     in CLASSIFICATION_MODE.

     @return returns the accuracy
     */
    Float getAccuracy() const{
        return accuracy;
    }
    
    /**
     Gets the total squared error for the training data at the current training iteration. This is only used if the trainingMode is
     in REGRESSION_MODE.

     @return returns the totalSquaredTrainingError
     */
    Float getTotalSquaredTrainingError() const{
        return totalSquaredTrainingError;
    }
    
    /**
     Gets the root mean squared error for the training data at the current training iteration. This is only used if the trainingMode is
     in REGRESSION_MODE.

     @return returns the rootMeanSquaredTrainingError
     */
    Float getRootMeanSquaredTrainingError() const{
        return rootMeanSquaredTrainingError;
    }
    
    /**
     Gets a pointer to the class used for training.
     
     @return returns a pointer to the trainer
     */
    MLBase* getTrainer() const{
        return trainer;
    }
    
    /**
     Sets the training result for classification data. This will place the training mode into CLASSIFICATION_MODE.

     @param trainingIteration: the current training iteration (or epoch)
     @param accuracy: the accuracy for the current training iteration
     @param trainer: a pointer to the class used to generate the result
     @return returns true if the training result was set successfully
     */
    bool setClassificationResult(unsigned int trainingIteration,Float accuracy,MLBase *trainer){
        this->trainingMode = CLASSIFICATION_MODE;
        this->trainingIteration = trainingIteration;
        this->accuracy = accuracy;
        this->trainer = trainer;
        return true;
    }
    
    /**
     Sets the training result for regression data. This will place the training mode into REGRESSION_MODE.

     @param trainingIteration: the current training iteration (or epoch)
     @param totalSquaredTrainingError: the total squared training error for the current iteration
     @param rootMeanSquaredTrainingError: the root mean squared training error for the current iteration
     @param trainer: a pointer to the class used to generate the result
     @return returns true if the training result was set successfully
     */
    bool setRegressionResult(unsigned int trainingIteration,Float totalSquaredTrainingError,Float rootMeanSquaredTrainingError,MLBase *trainer){
        this->trainingMode = REGRESSION_MODE;
        this->trainingIteration = trainingIteration;
        this->totalSquaredTrainingError = totalSquaredTrainingError;
        this->rootMeanSquaredTrainingError = rootMeanSquaredTrainingError;
        this->trainer = trainer;
        return true;
    }

protected:
    
    unsigned int trainingMode;
    unsigned int trainingIteration;
    Float accuracy;
    Float totalSquaredTrainingError;
    Float rootMeanSquaredTrainingError;
    MLBase *trainer;
    
public:
    
    enum TrainingMode{CLASSIFICATION_MODE=0,REGRESSION_MODE};

};

GRT_END_NAMESPACE

#endif //GRT_TRAINING_RESULT_HEADER
