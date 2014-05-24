/*
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

#include "GestureRecognitionPipeline.h"

namespace GRT{

GestureRecognitionPipeline::GestureRecognitionPipeline(void)
{
    initialized = false;
    trained = false;
    pipelineMode = PIPELINE_MODE_NOT_SET;
    inputVectorDimensions = 0;
    outputVectorDimensions = 0;
    predictedClassLabel = 0;
    predictionModuleIndex = 0;
    numTrainingSamples = 0;
    numTestSamples = 0;
    testAccuracy = 0;
    testRMSError = 0;
    testSquaredError = 0;
    testRejectionPrecision = 0;
    testRejectionRecall = 0;
    testTime = 0;
    trainingTime = 0;
    classifier = NULL;
    regressifier = NULL;
    contextModules.resize( NUM_CONTEXT_LEVELS );

    debugLog.setProceedingText("[DEBUG GRP]");
    errorLog.setProceedingText("[ERROR GRP]");
    warningLog.setProceedingText("[WARNING GRP]");
    testingLog.setProceedingText("[TEST GRP]");
}

GestureRecognitionPipeline::GestureRecognitionPipeline(const GestureRecognitionPipeline &rhs){
	
	initialized = false;
    trained = false;
    pipelineMode = PIPELINE_MODE_NOT_SET;
    inputVectorDimensions = 0;
    outputVectorDimensions = 0;
    predictedClassLabel = 0;
    predictionModuleIndex = 0;
    numTrainingSamples = 0;
    numTestSamples = 0;
    testAccuracy = 0;
    testRMSError = 0;
    testSquaredError = 0;
    testRejectionPrecision = 0;
    testRejectionRecall = 0;
    testTime = 0;
    trainingTime = 0;
    classifier = NULL;
    regressifier = NULL;
    contextModules.resize( NUM_CONTEXT_LEVELS );
    
    debugLog.setProceedingText("[DEBUG GRP]");
    errorLog.setProceedingText("[ERROR GRP]");
    warningLog.setProceedingText("[WARNING GRP]");
    testingLog.setProceedingText("[TEST GRP]");

	*this = rhs;
}


GestureRecognitionPipeline& GestureRecognitionPipeline::operator=(const GestureRecognitionPipeline &rhs){
	
	if( this != &rhs ){
        this->clearAll();
		
        //Copy the pipeline variables
		this->initialized = rhs.initialized;
        this->trained = rhs.trained;
	    this->inputVectorDimensions = rhs.inputVectorDimensions;
	    this->outputVectorDimensions = rhs.outputVectorDimensions;
	    this->predictedClassLabel = rhs.predictedClassLabel;
	    this->pipelineMode = rhs.pipelineMode;
	    this->predictionModuleIndex = rhs.predictionModuleIndex;
        this->numTrainingSamples = rhs.numTrainingSamples;
        this->numTestSamples = rhs.numTestSamples;
	    this->testAccuracy = rhs.testAccuracy;
	    this->testRMSError = rhs.testRMSError;
        this->testSquaredError = rhs.testSquaredError;
	    this->testTime = rhs.testTime;
	    this->trainingTime = rhs.trainingTime;
	    this->testFMeasure = rhs.testFMeasure;
	    this->testPrecision = rhs.testPrecision;
	    this->testRecall = rhs.testRecall;
	    this->regressionData = rhs.regressionData;
	    this->testRejectionPrecision = rhs.testRejectionPrecision;
	    this->testRejectionRecall = rhs.testRejectionRecall;
	    this->testConfusionMatrix = rhs.testConfusionMatrix;
        this->crossValidationResults = rhs.crossValidationResults;
        this->testResults = rhs.testResults;

        //Copy the GRT Base variables
        this->debugLog = rhs.debugLog;
        this->errorLog = rhs.errorLog;
        this->trainingLog = rhs.trainingLog;
        this->testingLog = rhs.testingLog;
        this->warningLog = rhs.warningLog;
	
		for(unsigned int i=0; i<rhs.preProcessingModules.size(); i++){
			this->addPreProcessingModule( *(rhs.preProcessingModules[i]) );
		}
		
		for(unsigned int i=0; i<rhs.featureExtractionModules.size(); i++){
			this->addFeatureExtractionModule( *(rhs.featureExtractionModules[i]) );
		}
		
		if( rhs.getIsPipelineInClassificationMode() ){
			setClassifier( *rhs.classifier );
		}
		
		if( rhs.getIsPipelineInRegressionMode() ){
			setRegressifier( *rhs.regressifier );
		}
		
		for(unsigned int i=0; i<rhs.postProcessingModules.size(); i++){
			this->addPostProcessingModule( *(rhs.postProcessingModules[i]) );
		}
		
		for(unsigned int k=0; k<NUM_CONTEXT_LEVELS; k++){
			for(unsigned int i=0; i<rhs.contextModules[k].size(); i++){
				this->addContextModule( *(rhs.contextModules[k][i]), k );
			}
		}

        //Adding the modules will automatically set trained to false, so make sure we get the correct state
        this->trained = rhs.trained;
	}
	
	return *this;
}

GestureRecognitionPipeline::~GestureRecognitionPipeline(void)
{
    //Clean up the memory
    deleteAllPreProcessingModules();
    deleteAllFeatureExtractionModules();
    deleteClassifier();
    deleteRegressifier();
    deleteAllPostProcessingModules();
    deleteAllContextModules();
} 
    
bool GestureRecognitionPipeline::train(const ClassificationData &trainingData){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << "train(ClassificationData trainingData) - Failed To Train Classifier, the classifier has not been set!" << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(ClassificationData trainingData) - Failed To Train Classifier, there is no training data!" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Set the input vector dimension size
    inputVectorDimensions = trainingData.getNumDimensions();
    
    //Pass the training data through any pre-processing or feature extraction units
    UINT numDimensions = trainingData.getNumDimensions();
    
    //If there are any preprocessing or feature extraction modules, then get the size of the last module
    if( getIsPreProcessingSet() || getIsFeatureExtractionSet() ){
        if( getIsFeatureExtractionSet() ){
            numDimensions = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }else{
            numDimensions = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
    }
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    ClassificationData processedTrainingData( numDimensions );
    
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        bool okToAddProcessedData = true;
        UINT classLabel = trainingData[i].getClassLabel();
        VectorDouble trainingSample = trainingData[i].getSample();
        
        //Perform any preprocessing
        if( getIsPreProcessingSet() ){
            for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                if( !preProcessingModules[moduleIndex]->process( trainingSample ) ){
                    errorLog << "train(ClassificationData trainingData) - Failed to PreProcess Training Data. PreProcessingModuleIndex: ";
                    errorLog << moduleIndex;
                    errorLog << endl;
                    return false;
                }
                trainingSample = preProcessingModules[moduleIndex]->getProcessedData();
            }
        }
        
        //Compute any features
        if( getIsFeatureExtractionSet() ){
            for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                if( !featureExtractionModules[moduleIndex]->computeFeatures( trainingSample ) ){
                    errorLog << "train(ClassificationData trainingData) - Failed to Compute Features from Training Data. FeatureExtractionModuleIndex ";
                    errorLog << moduleIndex;
                    errorLog << endl;
                    return false;
                }
                if( featureExtractionModules[moduleIndex]->getFeatureDataReady() ){
                    trainingSample = featureExtractionModules[moduleIndex]->getFeatureVector();
                }else{
                   okToAddProcessedData = false;
                   break;
                }
            }
        }

        if( okToAddProcessedData ){
            //Add the training sample to the processed training data
            processedTrainingData.addSample(classLabel, trainingSample);
        }
        
    }
    
    if( processedTrainingData.getNumSamples() != trainingData.getNumSamples() ){
        
        warningLog << "train(ClassificationData trainingData) - Lost " << trainingData.getNumSamples()-processedTrainingData.getNumSamples() << " of " << trainingData.getNumSamples() << " training samples due to the processing stage!" << endl;
    }

    //Store the number of training samples
    numTrainingSamples = processedTrainingData.getNumSamples();
    
    //Train the classifier
    cout << "Training classifier..." << endl;
    trained = classifier->train_( processedTrainingData );
    if( !trained ){
        errorLog << "train(ClassificationData trainingData) - Failed To Train Classifier: " << classifier->getLastErrorMessage() << endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::train(ClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << "train(ClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Train Classifier, the classifier has not been set!" << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(ClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Train Classifier, there is no training data!" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();

    //Spilt the data into K folds
    bool spiltResult = trainingData.spiltDataIntoKFolds(kFoldValue, useStratifiedSampling);
    
    if( !spiltResult ){
        return false;
    }
    
    //Run the k-fold training and testing
    double crossValidationAccuracy = 0;
    ClassificationData foldTrainingData;
    ClassificationData foldTestData;
    vector< TestResult > cvResults(kFoldValue);

    for(UINT k=0; k<kFoldValue; k++){
        ///Train the classification system
		foldTrainingData = trainingData.getTrainingFoldData(k);
        
        if( !train( foldTrainingData ) ){
            return false;
        }
        
        //Test the classification system
        foldTestData = trainingData.getTestFoldData(k);
        
        if( !test( foldTestData ) ){
            return false;
        }
        
        crossValidationAccuracy += getTestAccuracy();
        cvResults[k] = getTestResults();
    }

    //Flag that the model has been trained
    trained = true;
    
    //Set the accuracy of the classification system averaged over the kfolds
    testAccuracy = crossValidationAccuracy / double(kFoldValue);
    crossValidationResults = cvResults;
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::train(TimeSeriesClassificationData trainingData){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << "train(TimeSeriesClassificationData trainingData) - Failed To Train Classifier, the classifier has not been set!" << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(TimeSeriesClassificationData trainingData) - Failed To Train Classifier, there is no training data!" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Set the input vector dimension size of the pipeline
    inputVectorDimensions = trainingData.getNumDimensions();
    
    TimeSeriesClassificationData processedTrainingData( trainingData.getNumDimensions() );
    TimeSeriesClassificationData labelledTimeseriesClassificationData;
    ClassificationData labelledClassificationData;
    
    //Setup the data structure, if the classifier works with timeseries data then we use LabelledTimeSeriesClassificationData
    //otherwise we format the data as LabelledClassificationData
    if( classifier->getTimeseriesCompatible() ){
        UINT trainingDataInputDimensionSize = trainingData.getNumDimensions();
        if( getIsPreProcessingSet() ){
            trainingDataInputDimensionSize = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
        if( getIsFeatureExtractionSet() ){
            trainingDataInputDimensionSize = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
        labelledTimeseriesClassificationData.setNumDimensions( trainingDataInputDimensionSize );
    }else{
        UINT trainingDataInputDimensionSize = trainingData.getNumDimensions();
        if( getIsPreProcessingSet() ){
            trainingDataInputDimensionSize = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
        if( getIsFeatureExtractionSet() ){
            trainingDataInputDimensionSize = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
        labelledClassificationData.setNumDimensions( trainingDataInputDimensionSize );
    }
    
    //Pass the timeseries data through any pre-processing modules and add it to the processedTrainingData structure
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        UINT classLabel = trainingData[i].getClassLabel();
        MatrixDouble trainingSample = trainingData[i].getData();
        
        if( getIsPreProcessingSet() ){
            
            //Try to process the matrix data row-by-row
            bool resetPreprocessingModule = true;
            for(UINT r=0; r<trainingSample.getNumRows(); r++){
                VectorDouble sample = trainingSample.getRowVector( r );
                
                for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                    
                    if( resetPreprocessingModule ){
                        preProcessingModules[moduleIndex]->reset();
                    }
                    
                    //Validate the input and output dimensions match!
                    if( preProcessingModules[moduleIndex]->getNumInputDimensions() != preProcessingModules[moduleIndex]->getNumOutputDimensions() ){
                        errorLog << "train(TimeSeriesClassificationData trainingData) - Failed To PreProcess Training Data. The number of inputDimensions (";
                        errorLog << preProcessingModules[moduleIndex]->getNumInputDimensions();
                        errorLog << ") in  PreProcessingModule ";
                        errorLog << moduleIndex;
                        errorLog << " do not match the number of outputDimensions (";
                        errorLog << preProcessingModules[moduleIndex]->getNumOutputDimensions();
                        errorLog <<  endl;
                        return false;
                    }
                    
                    if( !preProcessingModules[moduleIndex]->process( sample ) ){
                        errorLog << "train(TimeSeriesClassificationData trainingData) - Failed To PreProcess Training Data. PreProcessingModuleIndex: ";
                        errorLog << moduleIndex;
                        errorLog << endl;
                        return false;
                    }
                    sample = preProcessingModules[moduleIndex]->getProcessedData();
                }
                
                //The preprocessing modules should only be reset when r==0
                resetPreprocessingModule = false;
                
                //Overwrite the original training sample with the preProcessed sample
                for(UINT c=0; c<sample.size(); c++){
                    trainingSample[r][c] = sample[c];
                }
            }
            
        }
        
        //Add the training sample to the processed training data
        processedTrainingData.addSample(classLabel,trainingSample);
    }
    
    //Loop over the processed training data, perfrom any feature extraction if needed
    //Add the data to either the timeseries or classification data structures
    for(UINT i=0; i<processedTrainingData.getNumSamples(); i++){
        UINT classLabel = processedTrainingData[i].getClassLabel();
        MatrixDouble trainingSample = processedTrainingData[i].getData();
        bool featureDataReady = false;
        bool resetFeatureExtractionModules = true;
        
        MatrixDouble featureData;
        //Try to process the matrix data row-by-row
        for(UINT r=0; r<trainingSample.getNumRows(); r++){
            VectorDouble inputVector = trainingSample.getRowVector( r );
            featureDataReady = true;
            
             //Pass the processed training data through the feature extraction
            if( getIsFeatureExtractionSet() ){
            
                for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                    
                    if( resetFeatureExtractionModules ){
                        featureExtractionModules[moduleIndex]->reset();
                    }
                    
                    if( !featureExtractionModules[moduleIndex]->computeFeatures( inputVector ) ){
                        errorLog << "train(TimeSeriesClassificationData trainingData) - Failed To Compute Features For Training Data. FeatureExtractionModuleIndex: ";
                        errorLog << moduleIndex;
                        errorLog << endl;
                        return false;
                    }
                    
                    //Overwrite the input vector with the features so this can either be input to the next feature module 
                    //or converted to the LabelledClassificationData format
                    inputVector = featureExtractionModules[moduleIndex]->getFeatureVector();
                    featureDataReady = featureExtractionModules[moduleIndex]->getFeatureDataReady();
                }
                
                //The feature extraction modules should only be reset on r == 0
                resetFeatureExtractionModules = false;
                
                if( featureDataReady ){
                    
                    if( classifier->getTimeseriesCompatible() ){
                        if( !featureData.push_back( inputVector ) ){
                            errorLog << "train(TimeSeriesClassificationData trainingData) - Failed To add feature vector to feature data matrix! FeatureExtractionModuleIndex: " << endl;
                            return false;
                        }
                    }else labelledClassificationData.addSample(classLabel, inputVector);
                }
                
            }else{
                if( classifier->getTimeseriesCompatible() ){
                    if( !featureData.push_back( inputVector ) ){
                        errorLog << "train(TimeSeriesClassificationData trainingData) - Failed To add feature vector to feature data matrix! FeatureExtractionModuleIndex: " << endl;
                        return false;
                    }
                }
                else labelledClassificationData.addSample(classLabel, inputVector);
            }
        }
        
        if( classifier->getTimeseriesCompatible() ) labelledTimeseriesClassificationData.addSample(classLabel, featureData);
        
    }
        
    //Train the classification system
    if( classifier->getTimeseriesCompatible() ){
        numTrainingSamples = labelledTimeseriesClassificationData.getNumSamples();
        trained = classifier->train( labelledTimeseriesClassificationData );
    }else{
        numTrainingSamples = labelledClassificationData.getNumSamples();
        trained = classifier->train( labelledClassificationData );
    }

    if( !trained ){
        errorLog << "train(TimeSeriesClassificationData trainingData) - Failed To Train Classifier" << classifier->getLastErrorMessage() << endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::train(TimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << "train(TimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Train Classifier, the classifier has not been set!" << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(TimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Train Classifier, there is no training data!" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Spilt the data into K folds
    if( !trainingData.spiltDataIntoKFolds(kFoldValue, useStratifiedSampling) ){
        errorLog << "train(TimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed To Spilt Dataset into KFolds!" << endl;
        return false;
    }
    
    //Run the k-fold training and testing
    double crossValidationAccuracy = 0;
    TimeSeriesClassificationData foldTrainingData;
    TimeSeriesClassificationData foldTestData;
    
    for(UINT k=0; k<kFoldValue; k++){
        ///Train the classification system
		foldTrainingData = trainingData.getTrainingFoldData(k);
        
        if( !train( foldTrainingData ) ){
            errorLog << "train(TimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed to train pipeline for fold " << k << "." << endl;
            return false;
        }
        
        //Test the classification system
        foldTestData = trainingData.getTestFoldData(k);
        
        if( !test( foldTestData ) ){
            errorLog << "train(TimeSeriesClassificationData trainingData,const UINT kFoldValue,const bool useStratifiedSampling) - Failed to test pipeline for fold " << k << "." << endl;
            return false;
        }
        
        crossValidationAccuracy += getTestAccuracy();
    }

    //Flag that the model has been trained
    trained = true;
    
    //Set the accuracy of the classification system averaged over the kfolds
    testAccuracy = crossValidationAccuracy / double(kFoldValue);
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::train(RegressionData trainingData){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Set the input vector dimension size
    inputVectorDimensions = trainingData.getNumInputDimensions();
    
    //Pass the training data through any pre-processing or feature extraction units
    RegressionData processedTrainingData;
    
    //Set the dimensionality of the data
    UINT numInputs = 0;
    UINT numTargets = trainingData.getNumTargetDimensions();
    if( !getIsPreProcessingSet() && !getIsFeatureExtractionSet() ){
        numInputs = trainingData.getNumInputDimensions();
    }else{
        
        if( getIsPreProcessingSet() && !getIsFeatureExtractionSet() ){
            numInputs = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
        
        if( !getIsPreProcessingSet() && getIsFeatureExtractionSet() ){
            numInputs = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
        
        if( getIsPreProcessingSet() && getIsFeatureExtractionSet() ){
            numInputs = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
    }
    
    processedTrainingData.setInputAndTargetDimensions(numInputs, numTargets);
    
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        VectorDouble inputVector = trainingData[i].getInputVector();
        VectorDouble targetVector = trainingData[i].getTargetVector();
        
        if( getIsPreProcessingSet() ){
            for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                if( !preProcessingModules[ moduleIndex ]->process( inputVector ) ){
                    errorLog << "train(const RegressionData trainingData) - Failed To Compute Features For Training Data. PreProcessingModuleIndex: " << moduleIndex << endl;
                    return false;
                }
                
                inputVector = preProcessingModules[ moduleIndex ]->getProcessedData();
            }
        }
        
        if( getIsFeatureExtractionSet() ){
            for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                if( !featureExtractionModules[ moduleIndex ]->computeFeatures( inputVector ) ){
                    errorLog << "train(const RegressionData trainingData) - Failed To Compute Features For Training Data. FeatureExtractionModuleIndex: " << moduleIndex << endl;
                    return false;
                }
                
                inputVector = featureExtractionModules[ moduleIndex ]->getFeatureVector();
            }
        }
        
        //Add the training sample to the processed training data
        if( !processedTrainingData.addSample(inputVector,targetVector) ){
            errorLog << "train(const RegressionData trainingData) - Failed to add processed training sample to training data" << endl;
            return false;
        }
    }
    
    //Store the number of training samples
    numTrainingSamples = processedTrainingData.getNumSamples();
    
    //Train the classification system
    if( getIsRegressifierSet() ){
        trained =  regressifier->train( processedTrainingData );
        if( !trained ){
            errorLog << "train(const RegressionData trainingData) - Failed To Train Regressifier: " << regressifier->getLastErrorMessage() << endl;
            return false;
        }
    }else{
        errorLog << "train(const RegressionData trainingData) - Classifier is not set" << endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::train(RegressionData trainingData,const UINT kFoldValue){

    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsRegressifierSet() ){
        errorLog << "train(RegressionData trainingData,const UINT kFoldValue) - Failed To Train Regressifier, the regressifier has not been set!" << endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << "train(RegressionData trainingData,const UINT kFoldValue) - Failed To Train Regressifier, there is no training data!" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Spilt the data into K folds
    bool spiltResult = trainingData.spiltDataIntoKFolds(kFoldValue);
    
    if( !spiltResult ){
        return false;
    }
    
    //Run the k-fold training and testing
    double crossValidationAccuracy = 0;
    RegressionData foldTrainingData;
    RegressionData foldTestData;
    for(UINT k=0; k<kFoldValue; k++){
        ///Train the classification system
        foldTrainingData = trainingData.getTrainingFoldData(k);
        
        if( !train( foldTrainingData ) ){
            return false;
        }
        
        //Test the classification system
        foldTestData = trainingData.getTestFoldData(k);
        
        if( !test( foldTestData ) ){
            return false;
        }

        crossValidationAccuracy += getTestRMSError();
        
    }

    //Flag that the model has been trained
    trained = true;

    testAccuracy = crossValidationAccuracy / double(kFoldValue);
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::test(const ClassificationData &testData){
    
    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorLog << "test(ClassificationData testData) - Classifier is not trained" << endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input vector's dimensions
    if( testData.getNumDimensions() != inputVectorDimensions ){
        errorLog << "test(ClassificationData testData) - The dimensionality of the test data (" + Util::toString(testData.getNumDimensions()) + ") does not match that of the input vector dimensions of the pipeline (" << inputVectorDimensions << ")" << endl;
        return false;
    }
    
    if( !getIsClassifierSet() ){
        errorLog << "test(ClassificationData testData) - The classifier has not been set" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();

	//Validate that the class labels in the test data match the class labels in the model
	bool classLabelValidationPassed = true;
	for(UINT i=0; i<testData.getNumClasses(); i++){
		bool labelFound = false;
		for(UINT k=0; k<classifier->getNumClasses(); k++){
			if( testData.getClassTracker()[i].classLabel == classifier->getClassLabels()[k] ){
				labelFound = true;
				break;
			}
		}

		if( !labelFound ){
			classLabelValidationPassed = false;
            errorLog << "test(ClassificationData testData) - The test dataset contains a class label (" << testData.getClassTracker()[i].classLabel << ") that is not in the model!" << endl;
		}
	}

	if( !classLabelValidationPassed ){
        errorLog << "test(ClassificationData testData) -  Model Class Labels: ";
        for(UINT k=0; k<classifier->getNumClasses(); k++){
            errorLog << classifier->getClassLabels()[k] << "\t";
		}
        errorLog << endl;
        return false;
    }

    double rejectionPrecisionCounter = 0;
    double rejectionRecallCounter = 0;
    unsigned int confusionMatrixSize = classifier->getNullRejectionEnabled() ? classifier->getNumClasses()+1 : classifier->getNumClasses();
    VectorDouble precisionCounter(classifier->getNumClasses(), 0);
    VectorDouble recallCounter(classifier->getNumClasses(), 0);
    VectorDouble confusionMatrixCounter(confusionMatrixSize,0);
    
    //Resize the test matrix
    testConfusionMatrix.resize(confusionMatrixSize, confusionMatrixSize);
    testConfusionMatrix.setAllValues(0);
    
    //Resize the precision and recall vectors
    testPrecision.clear();
    testRecall.clear();
    testFMeasure.clear();
    testPrecision.resize(getNumClassesInModel(), 0);
    testRecall.resize(getNumClassesInModel(), 0);
    testFMeasure.resize(getNumClassesInModel(), 0);
    testResults.resize(testData.getNumSamples());
    numTestSamples = testData.getNumSamples();
    
    //Start the test timer
    Timer timer;
    timer.start();

    //Run the test
    for(UINT i=0; i<numTestSamples; i++){
        UINT classLabel = testData[i].getClassLabel();
        VectorDouble testSample = testData[i].getSample();
        
        //Pass the test sample through the pipeline
        if( !predict( testSample ) ){
            errorLog << "test(ClassificationData testData) - Prediction failed for test sample at index: " << i << endl;
            return false;
        }
        
        //Update the test metrics
        UINT predictedClassLabel = getPredictedClassLabel();
        
        if( !updateTestMetrics(classLabel,predictedClassLabel,precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter) ){
            errorLog << "test(ClassificationData testData) - Failed to update test metrics at test sample index: " << i << endl;
            return false;
        }
        
        //Keep track of the classification results encase the user needs them later
        testResults[i].setClassificationResult(i, classLabel, predictedClassLabel, getUnProcessedPredictedClassLabel(),getClassLikelihoods(), getClassDistances());

        //Update any observers
        classifier->notifyTestResultsObservers( testResults[i] );
    }
    
    if( !computeTestMetrics(precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter, numTestSamples) ){
        errorLog <<"test(ClassificationData testData) - Failed to compute test metrics!" << endl;
        return false;
    }
    
    testTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::test(TimeSeriesClassificationData testData){

    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorLog << "test(TimeSeriesClassificationData testData) - The classifier has not been trained" << endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input vector's dimensions
    if( testData.getNumDimensions() != inputVectorDimensions ){
        errorLog << "test(TimeSeriesClassificationData testData) - The dimensionality of the test data (" << testData.getNumDimensions() << ") does not match that of the input vector dimensions of the pipeline (" << inputVectorDimensions << ")" << endl;
        return false;
    }
    
    if( !getIsClassifierSet() ){
        errorLog << "test(TimeSeriesClassificationData testData) - The classifier has not been set" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    double rejectionPrecisionCounter = 0;
    double rejectionRecallCounter = 0;
    const UINT K = classifier->getNumClasses();
    UINT confusionMatrixSize = classifier->getNullRejectionEnabled() ? K+1 : K;
    VectorDouble precisionCounter(K, 0);
    VectorDouble recallCounter(K, 0);
    VectorDouble confusionMatrixCounter(confusionMatrixSize,0);
    
    //Resize the test matrix
    testConfusionMatrix.resize(confusionMatrixSize,confusionMatrixSize);
    testConfusionMatrix.setAllValues(0);
    
    //Resize the precision and recall vectors
    testPrecision.resize(K, 0);
    testRecall.resize(K, 0);
    testFMeasure.resize(K, 0);
    numTestSamples = testData.getNumSamples();
    
    //Start the test timer
    Timer timer;
    timer.start();
    
    //Run the test
	const UINT M = testData.getNumSamples();
    for(UINT i=0; i<M; i++){
        UINT classLabel = testData[i].getClassLabel();
        MatrixDouble timeseries = testData[i].getData();
            
        //Pass the test timeseries through the pipeline
        if( !predict( timeseries ) ){
            errorLog << "test(TimeSeriesClassificationData testData) - Failed to run prediction for test sample index: " << i << endl;
            return false;
        }
        
        //Update the test metrics
        UINT predictedClassLabel = getPredictedClassLabel();
        
        if( !updateTestMetrics(classLabel,predictedClassLabel,precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter) ){
            errorLog << "test(TimeSeriesClassificationData testData) - Failed to update test metrics at test sample index: " << i << endl;
            return false;
        }
        
    }
        
    if( !computeTestMetrics(precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter, M) ){
        errorLog << "test(TimeSeriesClassificationData testData) - Failed to compute test metrics!" << endl;
        return false;
    }
    
    testTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::test(TimeSeriesClassificationDataStream testData){
    
    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorLog << "test(TimeSeriesClassificationDataStream testData) - The classifier has not been trained" << endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input vector's dimensions
    if( testData.getNumDimensions() != inputVectorDimensions ){
        errorLog << "test(TimeSeriesClassificationDataStream testData) - The dimensionality of the test data (" + Util::toString(testData.getNumDimensions()) + ") does not match that of the input vector dimensions of the pipeline (" << inputVectorDimensions << ")" << endl;
        return false;
    }
    
    if( !getIsClassifierSet() ){
        errorLog << "test(TimeSeriesClassificationDataStream testData) - The classifier has not been set" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    double rejectionPrecisionCounter = 0;
    double rejectionRecallCounter = 0;
    UINT confusionMatrixSize = classifier->getNullRejectionEnabled() ? classifier->getNumClasses()+1 : classifier->getNumClasses();
    VectorDouble precisionCounter(getNumClassesInModel(), 0);
    VectorDouble recallCounter(getNumClassesInModel(), 0);
    VectorDouble confusionMatrixCounter(confusionMatrixSize,0);
    
    //Resize the test matrix
    testConfusionMatrix.resize(confusionMatrixSize,confusionMatrixSize);
    testConfusionMatrix.setAllValues(0);
    
    //Resize the precision and recall vectors
    testPrecision.resize(getNumClassesInModel(), 0);
    testRecall.resize(getNumClassesInModel(), 0);
    testFMeasure.resize(getNumClassesInModel(), 0);
    
    //Resize the classification results vector
    testResults.resize(testData.getNumSamples());
    numTestSamples = testData.getNumSamples();
    
    //Start the test timer
    Timer timer;
    timer.start();
    
    //Run the test
    testData.resetPlaybackIndex(0); //Make sure that the test data start at 0
    for(UINT i=0; i<testData.getNumSamples(); i++){
        ClassificationSample sample = testData.getNextSample();
        UINT classLabel = sample.getClassLabel();
        VectorDouble testSample = sample.getSample();
            
        //Pass the test sample through the pipeline
        if( !predict( testSample ) ){
            errorLog << "test(TimeSeriesClassificationDataStream testData) - Prediction Failed! " << classifier->getLastErrorMessage() << endl;
            return false;
        }
        
        //Update the test metrics
        UINT predictedClassLabel = getPredictedClassLabel();
        
        /* //TODO - Need to update this!
        if( !updateTestMetrics(classLabel,predictedClassLabel,precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter) ){
            errorLog << "test(LabelledContinuousTimeSeriesClassificationData &testData) - Failed to update test metrics at test sample index: " << i << endl;
            return false;
        }
         */
        
        if( classLabel == predictedClassLabel ) testAccuracy++;
        
        //Store the test results so they can be used later
        testResults[i].setClassificationResult(i, classLabel, predictedClassLabel, getUnProcessedPredictedClassLabel(), getClassLikelihoods(), getClassDistances());
        
        //Notify all observers of the test result
        classifier->notifyTestResultsObservers( testResults[i] );
        
        testingLog << "test iteration: " << i;
        testingLog << "\tClassLabel: " << classLabel;
        testingLog << "\tPredictedClassLabel: " << predictedClassLabel;
        testingLog << "\tLikelihood: " << getMaximumLikelihood() << endl;
    }
    
    /* //TODO - Need to update this!
    if( !computeTestMetrics(precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter, testData.getNumSamples()) ){
        errorLog << "test(LabelledContinuousTimeSeriesClassificationData &testData) - Failed to compute test metrics !" << endl;
        return false;
    }
     */
    
    testTime = timer.getMilliSeconds();
    testAccuracy = testAccuracy / double( testData.getNumSamples() ) * 100.0;
    
    testingLog << "Test complete. Total testing time: " << testTime << endl;
    
    return true;
}
    
bool GestureRecognitionPipeline::test(RegressionData testData){
    
    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorLog << "test(RegressionData testData) - Regressifier is not trained" << endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input vector's dimensions
    if( testData.getNumInputDimensions() != inputVectorDimensions ){
        errorLog << "test(RegressionData testData) - The dimensionality of the test data (" << testData.getNumInputDimensions() << ") does not match that of the input vector dimensions of the pipeline (" << inputVectorDimensions << ")" << endl;
        return false;
    }
    
    if( !getIsRegressifierSet() ){
        errorLog << "test(RegressionData testData) - The regressifier has not been set" << endl;
        return false;
    }
    
    if( regressifier->getNumOutputDimensions() != testData.getNumTargetDimensions() ){
        errorLog << "test(RegressionData testData) - The size of the output of the regressifier (" << regressifier->getNumOutputDimensions() << ") does not match that of the size of the number of target dimensions (" << testData.getNumTargetDimensions() << ")" << endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    numTestSamples = testData.getNumSamples();
    testResults.resize( numTestSamples );
    
    //Start the test timer
    Timer timer;
    timer.start();
    
    //Run the test
    testSquaredError = 0;
    testRMSError = 0;
    for(UINT i=0; i<numTestSamples; i++){
        VectorDouble inputVector = testData[i].getInputVector();
        VectorDouble targetVector = testData[i].getTargetVector();
        
        //Pass the test sample through the pipeline
        if( !map( inputVector ) ){
            errorLog <<  "test(RegressionData testData) - Failed to map input vector!" << endl;
            return false;
        }
        
        //Update the RMS error
        double sum = 0;
        VectorDouble regressionData = regressifier->getRegressionData();
        for(UINT j=0; j<targetVector.size(); j++){
            sum += SQR( regressionData[j]-targetVector[j] );
        }

        testSquaredError += sum;

        //Keep track of the regression results encase the user needs them later
        testResults[i].setRegressionResult(i,regressionData,targetVector);

        //Update any observers
        regressifier->notifyTestResultsObservers( testResults[i] );
    }
    
    //Compute the test metrics
    testRMSError = sqrt( testSquaredError / double( testData.getNumSamples() ) );
    
    testTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::predict(const VectorDouble &inputVector){
	
	//Make sure the classification model has been trained
    if( !trained ){
        errorLog << "predict(const VectorDouble &inputVector) - The classifier has not been trained" << endl;
        return false;
    }
    
    //Make sure the dimensionality of the input vector matches the inputVectorDimensions
    if( inputVector.size() != inputVectorDimensions ){
        errorLog << "predict(const VectorDouble &inputVector) - The dimensionality of the input vector (" << int(inputVector.size()) << ") does not match that of the input vector dimensions of the pipeline (" << inputVectorDimensions << ")" << endl;
        return false;
    }

	if( getIsClassifierSet() ){
        return predict_classifier( inputVector );
    }

	if( getIsRegressifierSet() ){
        return predict_regressifier( inputVector );
    }

    errorLog << "predict(const VectorDouble &inputVector) - Neither a classifier or regressifer is not set" << endl;
	return false;
}

bool GestureRecognitionPipeline::predict(MatrixDouble inputMatrix){
	
	//Make sure the classification model has been trained
    if( !trained ){
        errorLog << "predict(MatrixDouble inputMatrix) - The classifier has not been trained" << endl;
        return false;
    }
    
    //Make sure the dimensionality of the input matrix matches the inputVectorDimensions
    if( inputMatrix.getNumCols() != inputVectorDimensions ){
        errorLog << "predict(MatrixDouble inputMatrix) - The dimensionality of the input matrix (" << inputMatrix.getNumCols() << ") does not match that of the input vector dimensions of the pipeline (" << inputVectorDimensions << ")" << endl;
        return false;
    }

	if( !getIsClassifierSet() ){
        errorLog << "predict(MatrixDouble inputMatrix) - A classifier has not been set" << endl;
		return false;
    }

	predictedClassLabel = 0;
    
    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    
    //Perform any pre-processing
    if( getIsPreProcessingSet() ){
		
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
			MatrixDouble tmpMatrix( inputMatrix.getNumRows(), preProcessingModules[moduleIndex]->getNumOutputDimensions() );
			
			for(UINT i=0; i<inputMatrix.getNumRows(); i++){
            	if( !preProcessingModules[moduleIndex]->process( inputMatrix.getRowVector(i) ) ){
                    errorLog << "predict(MatrixDouble inputMatrix) - Failed to PreProcess Input Matrix. PreProcessingModuleIndex: " << moduleIndex << endl;
                	return false;
            	}
            	tmpMatrix.setRowVector( preProcessingModules[moduleIndex]->getProcessedData(), i );
			}
			
			//Update the input matrix with the preprocessed data
			inputMatrix = tmpMatrix;
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_PREPROCESSING;
    //Todo
    
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() ){
	
	    for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
			MatrixDouble tmpMatrix( inputMatrix.getNumRows(), featureExtractionModules[moduleIndex]->getNumOutputDimensions() );
			
			for(UINT i=0; i<inputMatrix.getNumRows(); i++){
            	if( !featureExtractionModules[moduleIndex]->computeFeatures( inputMatrix.getRowVector(i) ) ){
                    errorLog << "predict(MatrixDouble inputMatrix) - Failed to PreProcess Input Matrix. FeatureExtractionModuleIndex: " << moduleIndex << endl;
                	return false;
            	}
            	tmpMatrix.setRowVector( featureExtractionModules[moduleIndex]->getFeatureVector(), i );
			}
			
			//Update the input matrix with the preprocessed data
			inputMatrix = tmpMatrix;
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
    //Todo
    
    //Perform the classification
    if( !classifier->predict( inputMatrix ) ){
        errorLog <<"predict(MatrixDouble inputMatrix) - Prediction Failed! " << classifier->getLastErrorMessage() << endl;
        return false;
    }
    predictedClassLabel = classifier->getPredictedClassLabel();
    
    //Update the context module
    //Todo
    
    //Perform any post processing
    predictionModuleIndex = AFTER_CLASSIFIER;
    if( getIsPostProcessingSet() ){
        
        if( pipelineMode != CLASSIFICATION_MODE){
            errorLog << "predict_classifier(VectorDouble inputVector) - Pipeline Mode Is Not in CLASSIFICATION_MODE!" << endl;
            return false;
        }
        
        VectorDouble data;
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            
            //Select which input we should give the postprocessing module
            if( postProcessingModules[moduleIndex]->getIsPostProcessingInputModePredictedClassLabel() ){
                //Set the input
                data.resize(1);
                data[0] = predictedClassLabel;
                
                //Verify that the input size is OK
                if( data.size() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                    errorLog << "predict_classifier(VectorDouble inputVector) - The size of the data vector (" << int(data.size()) << ") does not match that of the postProcessingModule (" << postProcessingModules[moduleIndex]->getNumInputDimensions() << ") at the moduleIndex: " << moduleIndex << endl;
                    return false;
                }
                
                //Postprocess the data
                if( !postProcessingModules[moduleIndex]->process( data ) ){
                    errorLog << "predict_classifier(VectorDouble inputVector) - Failed to post process data. PostProcessing moduleIndex: " << moduleIndex << endl;
                    return false;
                }
                
                //Select which output we should update
                data = postProcessingModules[moduleIndex]->getProcessedData();  
            }
            
            //Select which output we should update
            if( postProcessingModules[moduleIndex]->getIsPostProcessingOutputModePredictedClassLabel() ){
                //Get the processed predicted class label
                data = postProcessingModules[moduleIndex]->getProcessedData(); 
                
                //Verify that the output size is OK
                if( data.size() != 1 ){
                    errorLog << "predict_classifier(VectorDouble inputVector) - The size of the processed data vector (" << int(data.size()) << ") from postProcessingModule at the moduleIndex: " << moduleIndex << " is not equal to 1 even though it is in OutputModePredictedClassLabel!" << endl;
                    return false;
                }
                
                //Update the predicted class label
                predictedClassLabel = (UINT)data[0];
            }
                  
        }
    } 
    
    //Update the context module
    //TODO
    predictionModuleIndex = END_OF_PIPELINE;

	return true;
}

bool GestureRecognitionPipeline::map(VectorDouble inputVector){
	return predict_regressifier( inputVector );
}

bool GestureRecognitionPipeline::predict_classifier(VectorDouble inputVector){
    
    predictedClassLabel = 0;
    
    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    if( contextModules[ START_OF_PIPELINE ].size() > 0 ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ START_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->process( inputVector ) ){
                errorLog << "predict_classifier(VectorDouble inputVector) - Context Module Failed at START_OF_PIPELINE. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->getOK() ){
                return true;
            }
            inputVector = contextModules[ START_OF_PIPELINE ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any pre-processing
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
            if( !preProcessingModules[moduleIndex]->process( inputVector ) ){
                errorLog << "predict_classifier(VectorDouble inputVector) - Failed to PreProcess Input Vector. PreProcessingModuleIndex: " << moduleIndex << endl;
                return false;
            }
            inputVector = preProcessingModules[moduleIndex]->getProcessedData();
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_PREPROCESSING;
    if( contextModules[ AFTER_PREPROCESSING ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_PREPROCESSING ].size(); moduleIndex++){
            if( !contextModules[ AFTER_PREPROCESSING ][moduleIndex]->process( inputVector ) ){
                errorLog << "predict_classifier(VectorDouble inputVector) - Context Module Failed at AFTER_PREPROCESSING. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ AFTER_PREPROCESSING ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_PREPROCESSING;
                return false;
            }
            inputVector = contextModules[ AFTER_PREPROCESSING ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
            if( !featureExtractionModules[moduleIndex]->computeFeatures( inputVector ) ){
                errorLog << "predict_classifier(VectorDouble inputVector) - Failed to compute features from data. FeatureExtractionModuleIndex: " << moduleIndex << endl;
                return false;
            }
            inputVector = featureExtractionModules[moduleIndex]->getFeatureVector();
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
    if( contextModules[ AFTER_FEATURE_EXTRACTION ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_FEATURE_EXTRACTION ].size(); moduleIndex++){
            if( !contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->process( inputVector ) ){
                errorLog << "predict_classifier(VectorDouble inputVector) - Context Module Failed at AFTER_FEATURE_EXTRACTION. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
                return false;
            }
            inputVector = contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform the classification
    if( !classifier->predict(inputVector) ){
        errorLog << "predict_classifier(VectorDouble inputVector) - Prediction Failed! " << classifier->getLastErrorMessage() << endl;
        return false;
    }
    predictedClassLabel = classifier->getPredictedClassLabel();
    
    //Update the context module
    if( contextModules[ AFTER_CLASSIFIER ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_CLASSIFIER ].size(); moduleIndex++){
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->process( vector<double>(1,predictedClassLabel) ) ){
                errorLog << "predict_classifier(VectorDouble inputVector) - Context Module Failed at AFTER_CLASSIFIER. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_CLASSIFIER;
                return false;
            }
            predictedClassLabel = (UINT)contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getProcessedData()[0];
        }
    }
    
    //Perform any post processing
    predictionModuleIndex = AFTER_CLASSIFIER;
    if( getIsPostProcessingSet() ){
        
        if( pipelineMode != CLASSIFICATION_MODE){
            errorLog << "predict_classifier(VectorDouble inputVector) - Pipeline Mode Is Not in CLASSIFICATION_MODE!" << endl;
            return false;
        }
        
        VectorDouble data;
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            
            //Select which input we should give the postprocessing module
            if( postProcessingModules[moduleIndex]->getIsPostProcessingInputModePredictedClassLabel() ){
                //Set the input
                data.resize(1);
                data[0] = predictedClassLabel;
                
                //Verify that the input size is OK
                if( data.size() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                    errorLog << "predict_classifier(VectorDouble inputVector) - The size of the data vector (" << int(data.size()) << ") does not match that of the postProcessingModule (" << postProcessingModules[moduleIndex]->getNumInputDimensions() << ") at the moduleIndex: " << moduleIndex << endl;
                    return false;
                }
                
                //Postprocess the data
                if( !postProcessingModules[moduleIndex]->process( data ) ){
                    errorLog << "predict_classifier(VectorDouble inputVector) - Failed to post process data. PostProcessing moduleIndex: " << moduleIndex << endl;
                    return false;
                }
                
                //Select which output we should update
                data = postProcessingModules[moduleIndex]->getProcessedData();  
            }
            
            //Select which output we should update
            if( postProcessingModules[moduleIndex]->getIsPostProcessingOutputModePredictedClassLabel() ){
                //Get the processed predicted class label
                data = postProcessingModules[moduleIndex]->getProcessedData(); 
                
                //Verify that the output size is OK
                if( data.size() != 1 ){
                    errorLog << "predict_classifier(VectorDouble inputVector) - The size of the processed data vector (" << int(data.size()) << ") from postProcessingModule at the moduleIndex: " << moduleIndex << " is not equal to 1 even though it is in OutputModePredictedClassLabel!" << endl;
                    return false;
                }
                
                //Update the predicted class label
                predictedClassLabel = (UINT)data[0];
            }
                  
        }
    } 
    
    //Update the context module
    predictionModuleIndex = END_OF_PIPELINE;
    if( contextModules[ END_OF_PIPELINE ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ END_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->process( vector<double>(1,predictedClassLabel) ) ){
                errorLog << "predict_classifier(VectorDouble inputVector) - Context Module Failed at END_OF_PIPELINE. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->getOK() ){
                predictionModuleIndex = END_OF_PIPELINE;
                return false;
            }
            predictedClassLabel = (UINT)contextModules[ END_OF_PIPELINE ][moduleIndex]->getProcessedData()[0];
        }
    }
    
    return true;
}
    
bool GestureRecognitionPipeline::predict_regressifier(VectorDouble inputVector){
    
    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    if( contextModules[ START_OF_PIPELINE ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ START_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->process( inputVector ) ){
                errorLog << "predict_regressifier(VectorDouble inputVector) - Context Module Failed at START_OF_PIPELINE. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->getOK() ){
                return true;
            }
            inputVector = contextModules[ START_OF_PIPELINE ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any pre-processing
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
            if( !preProcessingModules[moduleIndex]->process( inputVector ) ){
                errorLog << "predict_regressifier(VectorDouble inputVector) - Failed to PreProcess Input Vector. PreProcessingModuleIndex: " << moduleIndex << endl;
                return false;
            }
            inputVector = preProcessingModules[moduleIndex]->getProcessedData();
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_PREPROCESSING;
    if( contextModules[ AFTER_PREPROCESSING ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_PREPROCESSING ].size(); moduleIndex++){
            if( !contextModules[ AFTER_PREPROCESSING ][moduleIndex]->process( inputVector ) ){
                errorLog << "predict_regressifier(VectorDouble inputVector) - Context Module Failed at AFTER_PREPROCESSING. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ AFTER_PREPROCESSING ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_PREPROCESSING;
                return false;
            }
            inputVector = contextModules[ AFTER_PREPROCESSING ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
            if( !featureExtractionModules[moduleIndex]->computeFeatures( inputVector ) ){
                errorLog << "predict_regressifier(VectorDouble inputVector) - Failed to compute features from data. FeatureExtractionModuleIndex: " << moduleIndex << endl;
                return false;
            }
            inputVector = featureExtractionModules[moduleIndex]->getFeatureVector();
        }
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
    if( contextModules[ AFTER_FEATURE_EXTRACTION ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_FEATURE_EXTRACTION ].size(); moduleIndex++){
            if( !contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->process( inputVector ) ){
                errorLog << "predict_regressifier(VectorDouble inputVector) - Context Module Failed at AFTER_FEATURE_EXTRACTION. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_FEATURE_EXTRACTION;
                return false;
            }
            inputVector = contextModules[ AFTER_FEATURE_EXTRACTION ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform the regression
    if( !regressifier->predict(inputVector) ){
        errorLog << "predict_regressifier(VectorDouble inputVector) - Prediction Failed! " << regressifier->getLastErrorMessage() << endl;
        return false;
    }
    regressionData = regressifier->getRegressionData();
    
    //Update the context module
    if( contextModules[ AFTER_CLASSIFIER ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_CLASSIFIER ].size(); moduleIndex++){
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->process( regressionData ) ){
                errorLog << "predict_regressifier(VectorDouble inputVector) - Context Module Failed at AFTER_CLASSIFIER. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_CLASSIFIER;
                return false;
            }
            regressionData = contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getProcessedData();
        }
    }
    
    //Perform any post processing
    predictionModuleIndex = AFTER_CLASSIFIER;
    if( getIsPostProcessingSet() ){
        
        if( pipelineMode != REGRESSION_MODE ){
            errorLog << "predict_regressifier(VectorDouble inputVector) - Pipeline Mode Is Not In RegressionMode!" << endl;
            return false;
        }
          
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            if( regressionData.size() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                errorLog << "predict_regressifier(VectorDouble inputVector) - The size of the regression vector (" << int(regressionData.size()) << ") does not match that of the postProcessingModule (" << postProcessingModules[moduleIndex]->getNumInputDimensions() << ") at the moduleIndex: " << moduleIndex << endl;
                return false;
            }
            
            if( !postProcessingModules[moduleIndex]->process( regressionData ) ){
                errorLog << "predict_regressifier(VectorDouble inputVector) - Failed to post process data. PostProcessing moduleIndex: " << moduleIndex << endl;
                return false;
            }
            regressionData = postProcessingModules[moduleIndex]->getProcessedData();        
        }
        
    } 
    
    //Update the context module
    predictionModuleIndex = END_OF_PIPELINE;
    if( contextModules[ END_OF_PIPELINE ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ END_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->process( inputVector ) ){
                errorLog << "predict_regressifier(VectorDouble inputVector) - Context Module Failed at END_OF_PIPELINE. ModuleIndex: " << moduleIndex << endl;
                return false;
            }
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->getOK() ){
                predictionModuleIndex = END_OF_PIPELINE;
                return false;
            }
            regressionData = contextModules[ END_OF_PIPELINE ][moduleIndex]->getProcessedData();
        }
    }
    
    return true;
}
    
bool GestureRecognitionPipeline::reset(){
    
    //Reset any pre processing
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
            if( !preProcessingModules[ moduleIndex ]->reset() ){
                errorLog << "Failed To Reset PreProcessingModule " << moduleIndex << endl;
                return false;
            }
        }
    }
    
    //Reset any feature extraction
    if( getIsFeatureExtractionSet() ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
            if( !featureExtractionModules[ moduleIndex ]->reset() ){
                errorLog << "Failed To Reset FeatureExtractionModule " << moduleIndex << endl;
                return false;
            }
        }
    }
    
    //Reset the classifier
    if( getIsClassifierSet() ){
        if( !classifier->reset() ){
            errorLog << "Failed To Reset Classifier! " << classifier->getLastErrorMessage() << endl;
            return false;
        }
    }
    
    //Reset the regressiier
    if( getIsRegressifierSet() ){
        if( !regressifier->reset() ){
            errorLog << "Failed To Reset Regressifier! " << regressifier->getLastErrorMessage() << endl;
            return false;
        }
    }
    
    //Reset any post processing
    if( getIsPostProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            if( !postProcessingModules[ moduleIndex ]->reset() ){
                errorLog << "Failed To Reset PostProcessingModule " << moduleIndex << endl;
                return false;
            }
        }
    } 
    
    return true;
}
    
bool GestureRecognitionPipeline::savePipelineToFile(const string &filename){
    
    if( !initialized ){
        errorLog << "Failed to write pipeline to file as the pipeline has not been initialized yet!" << endl;
        return false;
    }
    
    fstream file;
    
    file.open(filename.c_str(), iostream::out );
    
    if( !file.is_open() ){
        errorLog << "Failed to open file with filename: " << filename << endl;
        return false;
    }
    
    //Write the pipeline header info
    file << "GRT_PIPELINE_FILE_V1.0\n";
    file << "PipelineMode: " << getPipelineModeAsString() << endl;
    file << "NumPreprocessingModules: " << getNumPreProcessingModules() << endl;
    file << "NumFeatureExtractionModules: " << getNumFeatureExtractionModules() << endl;
    file << "NumPostprocessingModules: " << getNumPostProcessingModules() << endl;
    file << "Trained: " << getTrained() << endl;
    
    //Write the module datatype names
    file << "PreProcessingModuleDatatypes:";
    for(UINT i=0; i<getNumPreProcessingModules(); i++){
        file << "\t" << preProcessingModules[i]->getPreProcessingType();
    }
    file << endl;
    
    file << "FeatureExtractionModuleDatatypes:";
    for(UINT i=0; i<getNumFeatureExtractionModules(); i++){
        file << "\t" << featureExtractionModules[i]->getFeatureExtractionType();
    }
    file << endl;
    
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
            if( getIsClassifierSet() ) file << "ClassificationModuleDatatype:\t" << classifier->getClassifierType() << endl;
            else file << "ClassificationModuleDatatype:\tCLASSIFIER_NOT_SET" << endl;
            break;
        case REGRESSION_MODE:
            if( getIsRegressifierSet() ) file << "RegressionnModuleDatatype:\t" << regressifier->getRegressifierType() << endl;
            else file << "RegressionnModuleDatatype:\tREGRESSIFIER_NOT_SET" << endl;
            break;
        default:
            break;
    }
    
    file << "PostProcessingModuleDatatypes:";
    for(UINT i=0; i<getNumPostProcessingModules(); i++){
        file << "\t" << postProcessingModules[i]->getPostProcessingType();
    }
    file << endl;
    
    //Write the preprocessing module data to the file
    for(UINT i=0; i<getNumPreProcessingModules(); i++){
        file << "PreProcessingModule_" << Util::intToString(i+1) << endl;
        if( !preProcessingModules[i]->saveModelToFile( file ) ){
            errorLog << "Failed to write preprocessing module " << i << " settings to file!" << endl;
            file.close();
            return false;
        }
    }
    
    //Write the feature extraction module data to the file
    for(UINT i=0; i<getNumFeatureExtractionModules(); i++){
        file << "FeatureExtractionModule_" << Util::intToString(i+1) << endl;
        if( !featureExtractionModules[i]->saveModelToFile( file ) ){
            errorLog << "Failed to write feature extraction module " << i << " settings to file!" << endl;
            file.close();
            return false;
        }
    }
    
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
            if( getIsClassifierSet() ){
                if( !classifier->saveModelToFile( file ) ){
                    errorLog << "Failed to write classifier model to file!" << endl;
                    file.close();
                    return false;
                }
            }
            break;
        case REGRESSION_MODE:
            if( getIsRegressifierSet() ){
                if( !regressifier->saveModelToFile( file ) ){
                    errorLog << "Failed to write regressifier model to file!" << endl;
                    file.close();
                    return false;
                }
            }
            break;
        default:
            break;
    }
    
    //Write the post processing module data to the file
    for(UINT i=0; i<getNumPostProcessingModules(); i++){
        file << "PostProcessingModule_" << Util::intToString(i+1) << endl;
        if( !postProcessingModules[i]->saveModelToFile( file ) ){
            errorLog <<"Failed to write post processing module " << i << " settings to file!" << endl;
            file.close();
            return false;
        }
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool GestureRecognitionPipeline::loadPipelineFromFile(const string &filename){
    
    fstream file;

	//Clear any previous setup
	clearAll();
    
    file.open(filename.c_str(), iostream::in );
    
    if( !file.is_open() ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to open file with filename: " << filename << endl;
        return false;
    }

	string word;
	
	//Load the file header
	file >> word;
	if( word != "GRT_PIPELINE_FILE_V1.0" ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to read file header" << endl;
		file.close();
        return false;
	}
	
	//Load the pipeline mode
	file >> word;
	if( word != "PipelineMode:" ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to read PipelineMode" << endl;
		file.close();
        return false;
	}
	file >> word;
	pipelineMode = getPipelineModeFromString(word);
	
	//Load the NumPreprocessingModules
	file >> word;
	if( word != "NumPreprocessingModules:" ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to read NumPreprocessingModules" << endl;
		file.close();
        return false;
	}
	unsigned int numPreprocessingModules;
	file >> numPreprocessingModules;
	
	//Load the NumFeatureExtractionModules
	file >> word;
	if( word != "NumFeatureExtractionModules:" ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to read NumFeatureExtractionModules" << endl;
		file.close();
        return false;
	}
	unsigned int numFeatureExtractionModules;
	file >> numFeatureExtractionModules;
	
	//Load the NumPostprocessingModules
	file >> word;
	if( word != "NumPostprocessingModules:" ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to read NumPostprocessingModules" << endl;
		file.close();
        return false;
	}
	unsigned int numPostprocessingModules;
	file >> numPostprocessingModules;
	
	//Load if the pipeline has been trained
	file >> word;
	if( word != "Trained:" ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to read Trained" << endl;
		file.close();
        return false;
	}
	file >> trained;
	
	//Resize the modules
	if( numPreprocessingModules > 0 ) preProcessingModules.resize(numPreprocessingModules,NULL);
	if( numFeatureExtractionModules > 0 ) featureExtractionModules.resize(numFeatureExtractionModules,NULL);
	if( numPostprocessingModules > 0 ) postProcessingModules.resize(numPostprocessingModules,NULL);
	
	//Load the preprocessing module datatypes and initialize the modules
	file >> word;
	if( word != "PreProcessingModuleDatatypes:" ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to read PreProcessingModuleDatatypes" << endl;
		file.close();
        return false;
	}
    for(UINT i=0; i<numPreprocessingModules; i++){
		file >> word;
		preProcessingModules[i] = PreProcessing::createInstanceFromString( word );
		if( preProcessingModules[i] == NULL ){
            errorLog << "loadPipelineFromFile(string filename) - Failed to create preprocessing instance from string: " << word << endl;
			file.close();
	        return false;
		}
    }

	//Load the feature extraction module datatypes and initialize the modules
	file >> word;
	if( word != "FeatureExtractionModuleDatatypes:" ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to read FeatureExtractionModuleDatatypes" << endl;
		file.close();
        return false;
	}
    for(UINT i=0; i<numFeatureExtractionModules; i++){
		file >> word;
		featureExtractionModules[i] = FeatureExtraction::createInstanceFromString( word );
		if( featureExtractionModules[i] == NULL ){
            errorLog << "loadPipelineFromFile(string filename) - Failed to create feature extraction instance from string: " << word << endl;
			file.close();
	        return false;
		}
    }

	switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
			file >> word;
			if( word != "ClassificationModuleDatatype:" ){
                errorLog << "loadPipelineFromFile(string filename) - Failed to read ClassificationModuleDatatype" << endl;
				file.close();
		        return false;
			}
			//Load the classifier type
			file >> word;
			
			//Initialize the classifier
			classifier = Classifier::createInstanceFromString( word );
			if( classifier == NULL ){
                errorLog << "loadPipelineFromFile(string filename) - Failed to create classifier instance from string: " << word << endl;
				file.close();
		        return false;
			}
            break;
        case REGRESSION_MODE:
			file >> word;
			if( word != "RegressionnModuleDatatype:" ){
                errorLog << "loadPipelineFromFile(string filename) - Failed to read RegressionnModuleDatatype" << endl;
				file.close();
		        return false;
			}
			//Load the regressifier type
			file >> word;
			
			//Initialize the regressifier
			regressifier = Regressifier::createInstanceFromString( word );
			if( regressifier == NULL ){
                errorLog << "loadPipelineFromFile(string filename) - Failed to create regressifier instance from string: " << word << endl;
				file.close();
		        return false;
			}
            break;
        default:
            break;
    }

	//Load the post processing module datatypes and initialize the modules
	file >> word;
	if( word != "PostProcessingModuleDatatypes:" ){
        errorLog << "loadPipelineFromFile(string filename) - Failed to read PostProcessingModuleDatatypes" << endl;
		file.close();
	    return false;
	}
	for(UINT i=0; i<numPostprocessingModules; i++){
		file >> word;
		postProcessingModules[i] = PostProcessing::createInstanceFromString( word );
	}
	
	//Load the preprocessing module data from the file
    for(UINT i=0; i<numPreprocessingModules; i++){
		//Load the preprocessing module header
		file >> word;
        if( !preProcessingModules[i]->loadModelFromFile( file ) ){
            errorLog << "Failed to load preprocessing module " << i << " settings from file!" << endl;
            file.close();
            return false;
        }
    }

	//Load the feature extraction module data from the file
	for(UINT i=0; i<numFeatureExtractionModules; i++){
		//Load the feature extraction module header
		file >> word;
	    if( !featureExtractionModules[i]->loadModelFromFile( file ) ){
            errorLog << "Failed to load feature extraction module " << i << " settings from file!" << endl;
	        file.close();
	        return false;
	    }
	}
	
	//Load the classifier or regressifer data
	switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
               if( !classifier->loadModelFromFile( file ) ){
                   errorLog << "Failed to load classifier model from file!" << endl;
                   file.close();
                   return false;
               }
            break;
        case REGRESSION_MODE:
               if( !regressifier->loadModelFromFile( file ) ){
                   errorLog << "Failed to load regressifier model from file!" << endl;
                   file.close();
                   return false;
               }
            break;
        default:
            break;
    }

	//Load the post processing module data from the file
    for(UINT i=0; i<numPostprocessingModules; i++){
		//Load the post processing module header
		file >> word;
        if( !postProcessingModules[i]->loadModelFromFile( file ) ){
            errorLog << "Failed to load post processing module " << i << " settings from file!" << endl;
            file.close();
            return false;
        }
    }
    
    //Close the file
    file.close();
    
    //Set the expected input vector size
    inputVectorDimensions = 0;
    
    if( numPreprocessingModules > 0 ){
        inputVectorDimensions = preProcessingModules[0]->getNumInputDimensions();
    }else{
        if( numFeatureExtractionModules > 0 ){
            inputVectorDimensions = featureExtractionModules[0]->getNumInputDimensions();
        }else{
            switch( pipelineMode ){
                case PIPELINE_MODE_NOT_SET:
                    break;
                case CLASSIFICATION_MODE:
                    inputVectorDimensions = classifier->getNumInputFeatures();
                    break;
                case REGRESSION_MODE:
                    inputVectorDimensions = regressifier->getNumInputFeatures();
                    break;
                default:
                    break;
            }
        }
    }
    
    return true;
}
    
bool GestureRecognitionPipeline::preProcessData(VectorDouble inputVector,bool computeFeatures){
    
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
            
            if( inputVector.size() != preProcessingModules[ moduleIndex ]->getNumInputDimensions() ){
                errorLog << "preProcessData(VectorDouble inputVector) - The size of the input vector (" << preProcessingModules[ moduleIndex ]->getNumInputDimensions() << ") does not match that of the PreProcessing Module at moduleIndex: " << moduleIndex << endl;
                return false;
            }
            
            if( !preProcessingModules[ moduleIndex ]->process( inputVector ) ){
                errorLog << "preProcessData(VectorDouble inputVector) - Failed To PreProcess Input Vector. PreProcessing moduleIndex: " << moduleIndex << endl;
                return false;
            }
            inputVector = preProcessingModules[ moduleIndex ]->getProcessedData();
        }
    }
    
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() && computeFeatures ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
            if( inputVector.size() != featureExtractionModules[ moduleIndex ]->getNumInputDimensions() ){
                errorLog << "preProcessData(VectorDouble inputVector) - The size of the input vector (" << featureExtractionModules[ moduleIndex ]->getNumInputDimensions() << ") does not match that of the FeatureExtraction Module at moduleIndex: " << moduleIndex << endl;
                return false;
            }
            
            if( !featureExtractionModules[ moduleIndex ]->computeFeatures( inputVector ) ){
                errorLog << "preProcessData(VectorDouble inputVector) - Failed To Compute Features from Input Vector. FeatureExtraction moduleIndex: " << moduleIndex << endl;
                return false;
            }
            inputVector = featureExtractionModules[ moduleIndex ]->getFeatureVector();
        }
    }
    
    return true;
}
 
/////////////////////////////// GETTERS ///////////////////////////////
bool GestureRecognitionPipeline::getIsInitialized() const{ 
    return initialized; 
}
    
bool GestureRecognitionPipeline::getTrained() const{ 
    return trained;
}
    
bool GestureRecognitionPipeline::getIsPreProcessingSet() const{ 
    return preProcessingModules.size() > 0; 
} 
    
bool GestureRecognitionPipeline::getIsFeatureExtractionSet() const{ 
    return featureExtractionModules.size() > 0; 
}
    
bool GestureRecognitionPipeline::getIsClassifierSet() const{ 
    return (classifier!=NULL); 
}
    
bool GestureRecognitionPipeline::getIsRegressifierSet() const{ 
    return (regressifier!=NULL); 
}
    
bool GestureRecognitionPipeline::getIsPostProcessingSet() const{ 
    return postProcessingModules.size() > 0; 
}
    
bool GestureRecognitionPipeline::getIsContextSet() const{ 
    for(UINT i=0; i<NUM_CONTEXT_LEVELS; i++){
        if( contextModules[i].size() > 0 ) return true;
    }
    return false;
}
    
bool GestureRecognitionPipeline::getIsPipelineModeSet() const{ 
    return (pipelineMode!=PIPELINE_MODE_NOT_SET); 
}
    
bool GestureRecognitionPipeline::getIsPipelineInClassificationMode() const{ 
    return (pipelineMode==CLASSIFICATION_MODE); 
}
    
bool GestureRecognitionPipeline::getIsPipelineInRegressionMode() const{ 
    return pipelineMode==REGRESSION_MODE; 
}

UINT GestureRecognitionPipeline::getInputVectorDimensionsSize() const{ 
    
    if( getIsPreProcessingSet() ){
        return preProcessingModules[0]->getNumInputDimensions();
    }
    
    if( getIsFeatureExtractionSet() ){
        return featureExtractionModules[0]->getNumInputDimensions();
    }
    
    if( getIsPipelineInClassificationMode() && getIsClassifierSet() ){
        return classifier->getNumInputFeatures();
    }
    if( getIsPipelineInRegressionMode() && getIsRegressifierSet() ){
        return regressifier->getNumInputFeatures(); 
    }
    return 0; 
}
    
UINT GestureRecognitionPipeline::getOutputVectorDimensionsSize() const{ 
    if( getIsClassifierSet() ) return 1;    //The output of the pipeline for classification will always be 1
    if( getIsRegressifierSet() ){
        return regressifier->getNumOutputDimensions();  
    }
    return 0;
}
    
UINT GestureRecognitionPipeline::getNumClassesInModel() const{ 
    return getNumClasses();
}
    
UINT GestureRecognitionPipeline::getNumClasses() const{
    return (getIsClassifierSet() ? classifier->getNumClasses() : 0); 
}
    
UINT GestureRecognitionPipeline::getNumPreProcessingModules() const{ 
    return (UINT)preProcessingModules.size(); 
}
    
UINT GestureRecognitionPipeline::getNumFeatureExtractionModules() const{ 
    return (UINT)featureExtractionModules.size(); 
}
    
UINT GestureRecognitionPipeline::getNumPostProcessingModules() const{ 
    return (UINT)postProcessingModules.size(); 
}
    
UINT GestureRecognitionPipeline::getPredictionModuleIndexPosition() const{ 
    return predictionModuleIndex; 
}
    
UINT GestureRecognitionPipeline::getPredictedClassLabel() const{ 
    return (getIsClassifierSet() ? predictedClassLabel : 0); 
}
    
UINT GestureRecognitionPipeline::getUnProcessedPredictedClassLabel() const{ 
    return (getIsClassifierSet() ? classifier->getPredictedClassLabel() : 0); 
}

double GestureRecognitionPipeline::getMaximumLikelihood() const{ 
    return (getIsClassifierSet() ? classifier->getMaximumLikelihood() : 0); 
}
    
double GestureRecognitionPipeline::getCrossValidationAccuracy() const{ 
    return (getIsClassifierSet()||getIsRegressifierSet() ? testAccuracy : 0);
}
    
double GestureRecognitionPipeline::getTestAccuracy() const{ 
    return testAccuracy; 
}
    
double GestureRecognitionPipeline::getTestRMSError() const{ 
    return testRMSError; 
}

double GestureRecognitionPipeline::getTestSSError() const{
    return testSquaredError;
}
    
double GestureRecognitionPipeline::getTestFMeasure(const UINT classLabel) const{
    
    if( !getIsClassifierSet() ) return -1;
    if( getClassLabels().size() != testFMeasure.size() ) return -1;
    
    for(UINT i=0; i<testFMeasure.size(); i++){
        if( getClassLabels()[i] == classLabel ){
            return testFMeasure[i];
        }
    }
    return -1;
}

double GestureRecognitionPipeline::getTestPrecision(const UINT classLabel) const{
    
    if( !getIsClassifierSet() ) return -1;
    if( getClassLabels().size() != testPrecision.size() ) return -1;
    
    for(UINT i=0; i<testPrecision.size(); i++){
        if( getClassLabels()[i] == classLabel ){
            return testPrecision[i];
        }
    }
    return -1;
}

double GestureRecognitionPipeline::getTestRecall(const UINT classLabel) const{
    
    if( !getIsClassifierSet() ) return -1;
    if( getClassLabels().size() != testRecall.size() ) return -1;
    
    for(UINT i=0; i<testRecall.size(); i++){
        if( getClassLabels()[i] == classLabel ){
            return testRecall[i];
        }
    }
    return -1;
}

double GestureRecognitionPipeline::getTestRejectionPrecision() const{ 
    return testRejectionPrecision; 
}
    
double GestureRecognitionPipeline::getTestRejectionRecall() const{ 
    return testRejectionRecall; 
}
    
double GestureRecognitionPipeline::getTestTime() const{
    return testTime;
}

double GestureRecognitionPipeline::getTrainingTime() const{
    return trainingTime;
}

double GestureRecognitionPipeline::getTrainingRMSError() const{
    return getIsRegressifierSet() ? regressifier->getRootMeanSquaredTrainingError() : 0;
}

double GestureRecognitionPipeline::getTrainingSSError() const{
    return getIsRegressifierSet() ? regressifier->getTotalSquaredTrainingError() : 0;
}
    
MatrixDouble GestureRecognitionPipeline::getTestConfusionMatrix() const{ 
    return testConfusionMatrix; 
}
    
TestResult GestureRecognitionPipeline::getTestResults() const {
    TestResult testResults;
    testResults.numTrainingSamples = numTrainingSamples;
    testResults.numTestSamples = numTestSamples;
    testResults.accuracy = testAccuracy;
    testResults.rmsError = testRMSError;
    testResults.totalSquaredError = testSquaredError;
    testResults.trainingTime = trainingTime;
    testResults.testTime = testTime;
    testResults.rejectionPrecision = testRejectionPrecision;
    testResults.rejectionRecall = testRejectionRecall;
    testResults.precision = testPrecision;
    testResults.recall = testRecall;
    testResults.fMeasure = testFMeasure;
    testResults.confusionMatrix = testConfusionMatrix;
    return testResults;
}
    
VectorDouble GestureRecognitionPipeline::getTestPrecision() const{
    return testPrecision;
}

VectorDouble GestureRecognitionPipeline::getTestRecall() const{
    return testRecall;
}

VectorDouble GestureRecognitionPipeline::getTestFMeasure() const{
    return testFMeasure;
}

VectorDouble GestureRecognitionPipeline::getClassLikelihoods() const{ 
    if( getIsClassifierSet() ){ return classifier->getClassLikelihoods(); }
    else{ return VectorDouble(); } 
}

VectorDouble GestureRecognitionPipeline::getClassDistances() const{ 
    if( getIsClassifierSet() ){ return classifier->getClassDistances(); }
    else{ return VectorDouble(); } 
}

VectorDouble GestureRecognitionPipeline::getNullRejectionThresholds() const{
    if( getIsClassifierSet() ){ return classifier->getNullRejectionThresholds(); }
    else{ return VectorDouble(); } 
}

VectorDouble GestureRecognitionPipeline::getRegressionData() const{ 
	return regressionData;
}

VectorDouble GestureRecognitionPipeline::getUnProcessedRegressionData() const{ 
    if( getIsRegressifierSet() ) {
        return regressifier->getRegressionData();
    }
    return VectorDouble();
}
    
VectorDouble GestureRecognitionPipeline::getPreProcessedData() const{
    if( getIsPreProcessingSet() ){ 
        return preProcessingModules[ preProcessingModules.size()-1 ]->getProcessedData(); 
    }
    return VectorDouble();
}

VectorDouble GestureRecognitionPipeline::getPreProcessedData(const UINT moduleIndex) const{
    if( getIsPreProcessingSet() ){ 
        if( moduleIndex < preProcessingModules.size() ){
            return preProcessingModules[ moduleIndex ]->getProcessedData(); 
        }
    }
    return VectorDouble();
}

VectorDouble GestureRecognitionPipeline::getFeatureExtractionData() const{
    if( getIsFeatureExtractionSet() ){ 
        return featureExtractionModules[ featureExtractionModules.size()-1 ]->getFeatureVector(); 
    }
    return VectorDouble();
}
    
VectorDouble GestureRecognitionPipeline::getFeatureExtractionData(const UINT moduleIndex) const{
    if( getIsFeatureExtractionSet() ){ 
        if( moduleIndex < featureExtractionModules.size() ){
            return featureExtractionModules[ moduleIndex ]->getFeatureVector(); 
        }
    }
    warningLog << "getFeatureExtractionData(UINT moduleIndex) - Failed to get class labels!" << endl;
    return VectorDouble();
}
    
vector< UINT > GestureRecognitionPipeline::getClassLabels() const{ 
    if( getIsClassifierSet() )
        return classifier->getClassLabels();
    warningLog << "getClassLabels() - Failed to get class labels!" << endl;
    return vector< UINT>(); 
}
    
vector< TestInstanceResult > GestureRecognitionPipeline::getTestInstanceResults() const{
    return testResults;
}

vector< TestResult > GestureRecognitionPipeline::getCrossValidationResults() const{
    return crossValidationResults;
}
    
PreProcessing* GestureRecognitionPipeline::getPreProcessingModule(const UINT moduleIndex) const{
    if( moduleIndex < preProcessingModules.size() ){
        return preProcessingModules[ moduleIndex ];
    }
    warningLog << "getPreProcessingModule(const UINT moduleIndex) - Failed to get pre processing module!" << endl;
    return NULL;
}
    
FeatureExtraction* GestureRecognitionPipeline::getFeatureExtractionModule(const UINT moduleIndex) const{
    if( moduleIndex < featureExtractionModules.size() ){
        return featureExtractionModules[ moduleIndex ];
    }
    warningLog << "getFeatureExtractionModule(const UINT moduleIndex) - Failed to get feature extraction module!" << endl;
    return NULL;
}
    
Classifier* GestureRecognitionPipeline::getClassifier() const{
    return classifier; 
}
    
Regressifier* GestureRecognitionPipeline::getRegressifier() const{
    return regressifier; 
}
    
PostProcessing* GestureRecognitionPipeline::getPostProcessingModule(UINT moduleIndex) const{
    if( moduleIndex < postProcessingModules.size() ){
        return postProcessingModules[ moduleIndex ];
    }
    warningLog << "getPostProcessingModule(UINT moduleIndex) - Failed to get post processing module!" << endl;
    return NULL;
}
    
Context* GestureRecognitionPipeline::getContextModule(UINT contextLevel,UINT moduleIndex) const{
    if( contextLevel < contextModules.size() ){
        if( moduleIndex < contextModules[ contextLevel ].size() ){
            return contextModules[ contextLevel ][ moduleIndex ];
        }
    }
    warningLog << "getContextModule(UINT contextLevel,UINT moduleIndex) - Failed to get context module!" << endl;
    return NULL;
}
    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////                SETTERS                    ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
bool GestureRecognitionPipeline::addPreProcessingModule(const PreProcessing &preProcessingModule,UINT insertIndex){
    
    //Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= preProcessingModules.size() ){
        errorLog << "addPreProcessingModule(const PreProcessing &preProcessingModule) - Invalid insertIndex value!" << endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    PreProcessing *newInstance = preProcessingModule.createNewInstance();
    
    //Verify that the clone was successful
    if( !newInstance->deepCopyFrom( &preProcessingModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorLog << "addPreProcessingModule(const PreProcessing &preProcessingModule) - PreProcessing Module Not Set!" << endl;
        return false;
    }
    
    //Add the new instance to the preProcessingModules
    vector< PreProcessing* >::iterator iter = preProcessingModules.begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = preProcessingModules.end(); 
    else iter = preProcessingModules.begin() + insertIndex;
    
    preProcessingModules.insert(iter, newInstance);

    //The pipeline has been changed, so flag that the pipeline is no longer trained
    trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::setPreProcessingModule(const PreProcessing &preProcessingModule){
	removeAllPreProcessingModules();
	return addPreProcessingModule( preProcessingModule );
}

bool GestureRecognitionPipeline::addFeatureExtractionModule(const FeatureExtraction &featureExtractionModule,UINT insertIndex){
    
    //Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= featureExtractionModules.size() ){
        errorLog << "addFeatureExtractionModule(const FeatureExtraction &featureExtractionModule) - Invalid insertIndex value!" << endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    FeatureExtraction *newInstance = featureExtractionModule.createNewInstance();
    
    //Verify that the clone was successful
    if( !newInstance->deepCopyFrom( &featureExtractionModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorLog << "addFeatureExtractionModule(const FeatureExtraction &featureExtractionModule - FeatureExtraction Module Not Set!" << endl;
        return false;
    }
    
    //Add the new instance to the preProcessingModules
    vector< FeatureExtraction* >::iterator iter = featureExtractionModules.begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = featureExtractionModules.end(); 
    else iter = featureExtractionModules.begin() + insertIndex;
    
    featureExtractionModules.insert(iter, newInstance);

    //The pipeline has been changed, so flag that the pipeline is no longer trained
    trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::setFeatureExtractionModule(const FeatureExtraction &featureExtractionModule){
	removeAllFeatureExtractionModules();
	return addFeatureExtractionModule( featureExtractionModule );
}

bool GestureRecognitionPipeline::setClassifier(const Classifier &classifier){
    //Delete any previous classifier and regressifier
    deleteClassifier();
    deleteRegressifier();
    
    //Create a new instance of the classifier and then clone the values across from the reference classifier
    this->classifier = classifier.createNewInstance();
    
    if( this->classifier == NULL ){
        errorLog << "setClassifier(const Classifier classifier) - Classifier Module Not Set!" << endl;
        return false;
    }
    
    //Validate that the classifier was cloned correctly
    if( !this->classifier->deepCopyFrom( &classifier ) ){
        deleteClassifier();
        pipelineMode = PIPELINE_MODE_NOT_SET;
        errorLog << "setClassifier(const Classifier classifier) - Classifier Module Not Set!" << endl;
        return false;
    }
    
    //Set the mode of the pipeline to classification mode
    pipelineMode = CLASSIFICATION_MODE;
    
    //Flag that the key part of the pipeline has now been initialized
    initialized = true;

    //The pipeline has been changed, so flag that the pipeline is no longer trained
    trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::setRegressifier(const Regressifier &regressifier){
    
    //Delete any previous classifier and regressifier
    deleteClassifier();
    deleteRegressifier();
    
    //Set the mode of the pipeline to regression mode
    pipelineMode = REGRESSION_MODE;
    
    //Create a new instance of the regressifier and then clone the values across from the reference regressifier
    this->regressifier = regressifier.createNewInstance();
    
    //Validate that the regressifier was cloned correctly
    if( !this->regressifier->deepCopyFrom( &regressifier ) ){
        deleteRegressifier();
        pipelineMode = PIPELINE_MODE_NOT_SET;
        errorLog << "setRegressifier(const Regressifier &regressifier) - Regressifier Module Not Set!" << endl;
        return false;
    }
    
    //Flag that the key part of the pipeline has now been initialized
    initialized = true;

    //The pipeline has been changed, so flag that the pipeline is no longer trained
    trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::addPostProcessingModule(const PostProcessing &postProcessingModule,UINT insertIndex){
    
    //Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= postProcessingModules.size() ){
        errorLog << "addPostProcessingModule((const PostProcessing &postProcessingModule) - Invalid insertIndex value!" << endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    PostProcessing *newInstance = postProcessingModule.createNewInstance();
    
    //Verify that the clone was successful
    if( !newInstance->deepCopyFrom( &postProcessingModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorLog << "addPostProcessingModule(const PostProcessing &postProcessingModule) - PostProcessing Module Not Set!" << endl;
        return false;
    }
    
    //Add the new instance to the postProcessingModules
    vector< PostProcessing* >::iterator iter = postProcessingModules.begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = postProcessingModules.end(); 
    else iter = postProcessingModules.begin() + insertIndex;
    
    postProcessingModules.insert(iter, newInstance);

    //The pipeline has been changed, so flag that the pipeline is no longer trained
    trained = false;

    return true;
}

bool GestureRecognitionPipeline::setPostProcessingModule(const PostProcessing &postProcessingModule){
	removeAllPostProcessingModules();
	return addPostProcessingModule( postProcessingModule );
}

bool GestureRecognitionPipeline::addContextModule(const Context &contextModule,UINT contextLevel,UINT insertIndex){
	
	//Validate the contextLevel is valid
    if( contextLevel >= contextModules.size() ){
        errorLog << "addContextModule(...) - Invalid contextLevel value!" << endl;
        return false;
    }

	//Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= contextModules[contextLevel].size() ){
        errorLog << "addContextModule(...) - Invalid insertIndex value!" << endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    Context *newInstance = contextModule.createNewInstance();
    
    //Verify that the clone was successful
    if( !newInstance->deepCopyFrom( &contextModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorLog << "addContextModule(...) - Context Module Not Set!" << endl;
        return false;
    }
    
    //Add the new instance to the contextModules
    vector< Context* >::iterator iter = contextModules[ contextLevel ].begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = contextModules[ contextLevel ].end(); 
    else iter = contextModules[ contextLevel ].begin() + insertIndex;
    
    contextModules[ contextLevel ].insert(iter, newInstance);
    
    return true;
}

bool GestureRecognitionPipeline::updateContextModule(bool value,UINT contextLevel,UINT moduleIndex){
    
    //Validate the contextLevel is valid
    if( contextLevel >= contextModules.size() ){
        errorLog << "updateContextModule(...) - Context Level is out of bounds!" << endl;
        return false;
    }
    
    //Validate the moduleIndex is valid
    if( moduleIndex >= contextModules[contextLevel].size() ){
        errorLog << "updateContextModule(...) - Invalid contextLevel value!"  << endl;
        return false;
    }
    
    return contextModules[contextLevel][moduleIndex]->updateContext( value );
}
    
bool GestureRecognitionPipeline::removeAllPreProcessingModules(){
    deleteAllPreProcessingModules();
    return true;
}
    
bool GestureRecognitionPipeline::removePreProcessingModule(UINT moduleIndex){
    if( moduleIndex >= preProcessingModules.size() ){
        errorLog << "removePreProcessingModule(UINT moduleIndex) - Invalid moduleIndex " << moduleIndex << ". The size of the preProcessingModules vector is " << int(preProcessingModules.size()) << endl;
        return false;
    }
    
    //Delete the module
    delete preProcessingModules[ moduleIndex ];
    preProcessingModules[ moduleIndex ] = NULL;
    preProcessingModules.erase( preProcessingModules.begin() + moduleIndex );

    //The pipeline has been changed, so flag that the pipeline is no longer trained
    trained = false;

    return true;
}
   
bool GestureRecognitionPipeline::removeAllFeatureExtractionModules(){
    deleteAllFeatureExtractionModules();
    return true;
}
        
bool GestureRecognitionPipeline::removeFeatureExtractionModule(UINT moduleIndex){
    if( moduleIndex >= featureExtractionModules.size() ){
        errorLog << "removeFeatureExtractionModule(UINT moduleIndex) - Invalid moduleIndex " << moduleIndex << ". The size of the featureExtractionModules vector is " << int(featureExtractionModules.size()) << endl;
        return false;
    }
    
    //Delete the module
    delete featureExtractionModules[ moduleIndex ];
    featureExtractionModules[ moduleIndex ] = NULL;
    featureExtractionModules.erase( featureExtractionModules.begin() + moduleIndex );

    //The pipeline has been changed, so flag that the pipeline is no longer trained
    trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::removeAllPostProcessingModules(){
    deleteAllPostProcessingModules();
    return true;
}

bool GestureRecognitionPipeline::removePostProcessingModule(UINT moduleIndex){
    if( moduleIndex >= postProcessingModules.size() ){
        errorLog << "removePostProcessingModule(UINT moduleIndex) - Invalid moduleIndex " << moduleIndex << ". The size of the postProcessingModules vector is " << int(postProcessingModules.size()) << endl;
        return false;
    }
    
    //Delete the module
    delete postProcessingModules[ moduleIndex ];
    postProcessingModules[ moduleIndex ] = NULL;
    postProcessingModules.erase( postProcessingModules.begin() + moduleIndex );

    //The pipeline has been changed, so flag that the pipeline is no longer trained
    trained = false;
    
    return true;
}
    
bool GestureRecognitionPipeline::removeContextModule(UINT contextLevel,UINT moduleIndex){
    if( contextLevel >= NUM_CONTEXT_LEVELS ){
        errorLog << "removeContextModule(UINT contextLevel,UINT moduleIndex) - Invalid moduleIndex " << moduleIndex << " is out of bounds!" << endl;
        return false;
    }
    
    if( moduleIndex >= contextModules[contextLevel].size() ){
        errorLog << "removePostProcessingModule(UINT moduleIndex) - Invalid moduleIndex " << moduleIndex << ". The size of the contextModules vector at context level " << " is " << int(contextModules[contextLevel].size()) << endl;
        return false;
    }
    
    //Delete the module
    delete contextModules[contextLevel][moduleIndex];
    contextModules[contextLevel][moduleIndex] = NULL;
    contextModules[contextLevel].erase( contextModules[contextLevel].begin() + moduleIndex );
    return true;
}
    
bool GestureRecognitionPipeline::removeAllContextModules(){
    deleteAllContextModules();
    return true;
}

bool GestureRecognitionPipeline::clearAll(){
	
    clearTestResults();
	removeAllPreProcessingModules();
	removeAllFeatureExtractionModules();
	removeClassifier();
	removeRegressifier();
	removeAllPostProcessingModules();
	removeAllContextModules();
	
	return true;
}
    
bool GestureRecognitionPipeline::clearTestResults(){
    
    numTestSamples = 0;
    testAccuracy = 0;
    testRMSError = 0;
    testSquaredError = 0;
    testTime = 0;
    testFMeasure.clear();
    testPrecision.clear();
    testRecall.clear();
    testRejectionPrecision = 0;
    testRejectionRecall = 0;
    testConfusionMatrix.clear();
    testResults.clear();
    crossValidationResults.clear();
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////          PROTECTED FUNCTIONS              ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
void GestureRecognitionPipeline::deleteAllPreProcessingModules(){
    if( preProcessingModules.size() != 0 ){
        for(UINT i=0; i<preProcessingModules.size(); i++){
            delete preProcessingModules[i];
            preProcessingModules[i] = NULL;
        }
        preProcessingModules.clear();
        trained = false;
    }
}
    
void GestureRecognitionPipeline::deleteAllFeatureExtractionModules(){
    if( featureExtractionModules.size() != 0 ){
        for(UINT i=0; i<featureExtractionModules.size(); i++){
            delete featureExtractionModules[i];
            featureExtractionModules[i] = NULL;
        }
        featureExtractionModules.clear();
        trained = false;
    }
}
    
void GestureRecognitionPipeline::deleteClassifier(){
    if( classifier != NULL ){
        delete classifier;
        classifier = NULL;
    }
    trained = false;
    initialized = false;
}
    
void GestureRecognitionPipeline::deleteRegressifier(){
    if( regressifier != NULL ){
        delete regressifier;
        regressifier = NULL;
    }
    trained = false;
    initialized = false;
}
    
void GestureRecognitionPipeline::deleteAllPostProcessingModules(){
    if( postProcessingModules.size() != 0 ){
        for(UINT i=0; i<postProcessingModules.size(); i++){
            delete postProcessingModules[i];
            postProcessingModules[i] = NULL;
        }
        postProcessingModules.clear();
        trained = false;
    }
}
    
void GestureRecognitionPipeline::deleteAllContextModules(){
    for(UINT i=0; i<contextModules.size(); i++){
        for(UINT j=0; j<contextModules[i].size(); j++){
            delete contextModules[i][j];
            contextModules[i][j] = NULL;
        }
        contextModules[i].clear();
    }
}
    
bool GestureRecognitionPipeline::updateTestMetrics(const UINT classLabel,const UINT predictedClassLabel,VectorDouble &precisionCounter,VectorDouble &recallCounter,double &rejectionPrecisionCounter,double &rejectionRecallCounter,VectorDouble &confusionMatrixCounter){

    //Find the index of the classLabel
    UINT predictedClassLabelIndex =0;
    bool predictedClassLabelIndexFound = false;
    if( predictedClassLabel != 0 ){
        for(UINT k=0; k<getNumClassesInModel(); k++){
            if( predictedClassLabel == classifier->getClassLabels()[k] ){
                predictedClassLabelIndex = k;
                predictedClassLabelIndexFound = true;
                break;
            }
        }
        
        if( !predictedClassLabelIndexFound ){
            errorLog << "Failed to find class label index for label: " << predictedClassLabel << endl;
            return false;
        }
    }

    //Find the index of the class label
    UINT actualClassLabelIndex = 0;
    if( classLabel != 0 ){
        for(UINT k=0; k<getNumClassesInModel(); k++){
            if( classLabel == classifier->getClassLabels()[k] ){
                actualClassLabelIndex = k;
                break;
            }
        }
    }

    //Update the classification accuracy
    if( classLabel == predictedClassLabel ){
        testAccuracy++;
    }

    //Update the precision
    if( predictedClassLabel != 0 ){
        if( classLabel == predictedClassLabel ){
            //Update the precision value
            testPrecision[ predictedClassLabelIndex ]++;
        }
        //Update the precision counter
        precisionCounter[ predictedClassLabelIndex ]++;
    }

    //Update the recall
    if( classLabel != 0 ){
        if( classLabel == predictedClassLabel ){
            //Update the recall value
            testRecall[ predictedClassLabelIndex ]++;
        }
        //Update the recall counter
        recallCounter[ actualClassLabelIndex ]++;
    }

    //Update the rejection precision
    if( predictedClassLabel == 0 ){
        if( classLabel == 0 ) testRejectionPrecision++;
        rejectionPrecisionCounter++;
    }

    //Update the rejection recall
    if( classLabel == 0 ){
        if( predictedClassLabel == 0 ) testRejectionRecall++;
        rejectionRecallCounter++;
    }

    //Update the confusion matrix
    if( classifier->getNullRejectionEnabled() ){
        if( classLabel == 0 ) actualClassLabelIndex = 0;
        else actualClassLabelIndex++;
        if( predictedClassLabel == 0 ) predictedClassLabelIndex = 0;
        else predictedClassLabelIndex++;
    }
    testConfusionMatrix[ actualClassLabelIndex  ][ predictedClassLabelIndex ]++;
    confusionMatrixCounter[ actualClassLabelIndex ]++;
    
    return true;
}

bool GestureRecognitionPipeline::computeTestMetrics(VectorDouble &precisionCounter,VectorDouble &recallCounter,double &rejectionPrecisionCounter,double &rejectionRecallCounter,VectorDouble &confusionMatrixCounter,const UINT numTestSamples){
        
    //Compute the test metrics
    testAccuracy = testAccuracy/double(numTestSamples) * 100.0;
    
    for(UINT k=0; k<getNumClassesInModel(); k++){
        if( precisionCounter[k] > 0 ) testPrecision[k] /= precisionCounter[k];
        else testPrecision[k] = 0;
        if( recallCounter[k] > 0 ) testRecall[k] /= recallCounter[k];
        else testRecall[k] = 0;
        
        if( precisionCounter[k] + recallCounter[k] > 0 )
            testFMeasure[k] = 2 * ((testPrecision[k]*testRecall[k])/(testPrecision[k]+testRecall[k]));
        else testFMeasure[k] = 0;
    }
    if( rejectionPrecisionCounter > 0 ) testRejectionPrecision /= rejectionPrecisionCounter;
    if( rejectionRecallCounter > 0 ) testRejectionRecall /= rejectionRecallCounter;
    
    
    for(UINT r=0; r<confusionMatrixCounter.size(); r++){
        if( confusionMatrixCounter[r] > 0 ){
            for(UINT c=0; c<testConfusionMatrix.getNumCols(); c++){
                testConfusionMatrix[r][c] /= confusionMatrixCounter[r];
            }
        }
    }
    
    return true;
}
    
string GestureRecognitionPipeline::getPipelineModeAsString() const{
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            return "PIPELINE_MODE_NOT_SET";
            break;
        case CLASSIFICATION_MODE:
            return "CLASSIFICATION_MODE";
            break;
        case REGRESSION_MODE:
            return "REGRESSION_MODE";
            break;
        default:
            return "ERROR_UNKNWON_PIPELINE_MODE";
            break;
    }
    
    return "ERROR_UNKNWON_PIPELINE_MODE";
}

UINT GestureRecognitionPipeline::getPipelineModeFromString(string pipelineModeAsString) const{
	if( pipelineModeAsString == "PIPELINE_MODE_NOT_SET" ){
		return PIPELINE_MODE_NOT_SET;
	}
	if( pipelineModeAsString == "CLASSIFICATION_MODE" ){
		return CLASSIFICATION_MODE;
	}
	if( pipelineModeAsString == "REGRESSION_MODE" ){
		return REGRESSION_MODE;
	}
	return PIPELINE_MODE_NOT_SET;
}

} //End of namespace GRT

