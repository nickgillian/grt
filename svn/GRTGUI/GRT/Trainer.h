#ifndef GRT_TRAINER_H
#define GRT_TRAINER_H

#include <QObject>
#include "../../GRT/GRT.h"
//Include the OSC server to get the boost headers and QDebug headers
#include "OSC/OSCServer.h"

namespace GRT{

class Trainer : public GRT::GRTBase{
public:
    Trainer(){ initialized = false; }
    ~Trainer(){}

    bool setupNoValidationTraining( const GRT::GestureRecognitionPipeline &pipeline, const GRT::ClassificationData &trainingData ){

        //Clear any previous setup
        clear();

        initialized = true;
        pipelineMode = CLASSIFICATION_MODE;
        trainingMode = NO_VALIDATION;
        this->pipeline = pipeline;
        this->classificationTrainingData = trainingData;
        return true;
    }

    bool setupNoValidationTraining( const GRT::GestureRecognitionPipeline &pipeline, const GRT::RegressionData &trainingData ){

        //Clear any previous setup
        clear();

        initialized = true;
        pipelineMode = REGRESSION_MODE;
        trainingMode = NO_VALIDATION;
        this->pipeline = pipeline;
        this->regressionTrainingData = trainingData;
        return true;
    }

    bool setupTrainingAndTesting( const GRT::GestureRecognitionPipeline &pipeline, const GRT::ClassificationData &trainingData, const GRT::ClassificationData &testData ){

        //Clear any previous setup
        clear();

        initialized = true;
        pipelineMode = CLASSIFICATION_MODE;
        trainingMode = TEST_DATASET;
        this->pipeline = pipeline;
        this->classificationTrainingData = trainingData;
        this->classificationTestData = testData;
        return true;
    }

    bool setupTrainingAndTesting( const GRT::GestureRecognitionPipeline &pipeline, const GRT::RegressionData &trainingData, const GRT::RegressionData &testData ){

        //Clear any previous setup
        clear();

        initialized = true;
        pipelineMode = REGRESSION_MODE;
        trainingMode = TEST_DATASET;
        this->pipeline = pipeline;
        this->regressionTrainingData = trainingData;
        this->regressionTestData = testData;
        return true;
    }

    bool setupCVTraining( const GRT::GestureRecognitionPipeline &pipeline, const GRT::ClassificationData &trainingData, const unsigned int numFolds ){

        //Clear any previous setup
        clear();

        initialized = true;
        pipelineMode = CLASSIFICATION_MODE;
        trainingMode = CROSS_VALIDATION;
        this->pipeline = pipeline;
        this->classificationTrainingData = trainingData;
        this->numFolds = numFolds;
        return true;
    }

    bool setupCVTraining( const GRT::GestureRecognitionPipeline &pipeline, const GRT::RegressionData &trainingData, const unsigned int numFolds ){

        //Clear any previous setup
        clear();

        initialized = true;
        pipelineMode = REGRESSION_MODE;
        trainingMode = CROSS_VALIDATION;
        this->pipeline = pipeline;
        this->regressionTrainingData = trainingData;
        this->numFolds = numFolds;
        return true;
    }

    bool train(){
        if( !initialized ){
            return false;
        }

        switch( trainingMode ){
            case NO_VALIDATION:
                return trainWithNoValidation();
            break;
            case TEST_DATASET:
                return trainAndTest();
            break;
            case CROSS_VALIDATION:
                return trainWithCV();
            break;
        }

        return false;
    }

    bool clear(){
        initialized = false;
        pipelineMode = CLASSIFICATION_MODE;
        trainingMode = NO_VALIDATION;
        numFolds = 0;
        pipeline.clearAll();
        classificationTrainingData.clear();
        classificationTestData.clear();
        regressionTrainingData.clear();
        regressionTestData.clear();
        timeseriesClassificationTrainingData.clear();
        timeseriesClassificationTestData.clear();
        return true;
    }

    std::string getLastErrorMessage() const{
        return pipeline.getLastErrorMessage();
    }

    std::string getLastWarningMessage() const{
        return pipeline.getLastWarningMessage();
    }

    GRT::GestureRecognitionPipeline getPipeline() const{
        return pipeline;
    }

protected:
    bool trainWithNoValidation(){
        bool result = false;
        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                result = pipeline.train( classificationTrainingData );
            break;
            case REGRESSION_MODE:
                result = pipeline.train( regressionTrainingData );
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
            break;
        }
        return result;
    }

    bool trainAndTest(){

        debugLog << "trainAndTest()" << endl;
        //Train
        bool result = false;
        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                result = pipeline.train( classificationTrainingData );
            break;
            case REGRESSION_MODE:
                result = pipeline.train( regressionTrainingData );
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
            break;
        }
        if( !result ){ debugLog << "Training Failed!" << endl; return false; }

        //Test
        debugLog << "Testing..." << endl;
        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                return pipeline.test( classificationTestData );
            break;
            case REGRESSION_MODE:
                return pipeline.test( regressionTestData );
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
            break;
        }
        return false;
    }

    bool trainWithCV(){
        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                return pipeline.train( classificationTrainingData, numFolds );
            break;
            case REGRESSION_MODE:
                return pipeline.train( regressionTrainingData, numFolds );
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
            break;
        }
        return false;
    }

    bool initialized;
    unsigned int pipelineMode;
    unsigned int trainingMode;
    unsigned int numFolds;
    GestureRecognitionPipeline pipeline;
    ClassificationData classificationTrainingData;
    ClassificationData classificationTestData;
    RegressionData regressionTrainingData;
    RegressionData regressionTestData;
    TimeSeriesClassificationData timeseriesClassificationTrainingData;
    TimeSeriesClassificationData timeseriesClassificationTestData;

    enum PipelineModes{CLASSIFICATION_MODE=0,REGRESSION_MODE,TIMESERIES_CLASSIFICATION_MODE};
    enum TrainingModes{NO_VALIDATION=0,TEST_DATASET,CROSS_VALIDATION};
};

}

#endif // GRT_TRAINER_H
