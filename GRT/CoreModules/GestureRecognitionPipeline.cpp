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

#define GRT_DLL_EXPORTS
#include "GestureRecognitionPipeline.h"

GRT_BEGIN_NAMESPACE

GestureRecognitionPipeline::GestureRecognitionPipeline(void) : MLBase( "Pipeline" )
{
    init();
}

GestureRecognitionPipeline::GestureRecognitionPipeline(const GestureRecognitionPipeline &rhs) : MLBase( "Pipeline" )
{
	init();

    //Invoke the equals operator to copy the rhs data to this instance
	*this = rhs;
}

GestureRecognitionPipeline& GestureRecognitionPipeline::operator=(const GestureRecognitionPipeline &rhs){
	
	if( this != &rhs ){
        this->clear();

        //Copy the base class
        this->copyMLBaseVariables( dynamic_cast<const MLBase*>(&rhs) );
		
        //Copy the pipeline variables
		this->initialized = rhs.initialized;
        this->trained = rhs.trained;
        this->info = rhs.info;
	    this->inputVectorDimensions = rhs.inputVectorDimensions;
	    this->outputVectorDimensions = rhs.outputVectorDimensions;
	    this->predictedClassLabel = rhs.predictedClassLabel;
        this->predictedClusterLabel = rhs.predictedClusterLabel;
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
	
		for(unsigned int i=0; i<rhs.preProcessingModules.getSize(); i++){
			this->addPreProcessingModule( *(rhs.preProcessingModules[i]) );
		}
		
		for(unsigned int i=0; i<rhs.featureExtractionModules.getSize(); i++){
			this->addFeatureExtractionModule( *(rhs.featureExtractionModules[i]) );
		}
		
		if( rhs.getIsPipelineInClassificationMode() ){
			setClassifier( *rhs.classifier );
		}
		
		if( rhs.getIsPipelineInRegressionMode() ){
			setRegressifier( *rhs.regressifier );
		}
        
        if( rhs.getIsClustererSet() ){
			setClusterer( *rhs.clusterer );
		}
		
		for(unsigned int i=0; i<rhs.postProcessingModules.getSize(); i++){
			this->addPostProcessingModule( *(rhs.postProcessingModules[i]) );
		}
		
		for(unsigned int k=0; k<NUM_CONTEXT_LEVELS; k++){
			for(unsigned int i=0; i<rhs.contextModules[k].getSize(); i++){
				this->addContextModule( *(rhs.contextModules[k][i]), k );
			}
		}

        //Adding the modules will automatically set trained to false, so make sure we get the correct state
        this->trained = rhs.trained;
	}
	
	return *this;
}

GestureRecognitionPipeline& GestureRecognitionPipeline::operator << (const PreProcessing &module) {
    this->addPreProcessingModule( module );
    return *this;
}

GestureRecognitionPipeline& GestureRecognitionPipeline::operator << (const FeatureExtraction &module) {
    this->addFeatureExtractionModule( module );
    return *this;
}

GestureRecognitionPipeline& GestureRecognitionPipeline::operator << (const Classifier &module) {
    this->setClassifier( module );
    return *this;
}

GestureRecognitionPipeline& GestureRecognitionPipeline::operator << (const Regressifier &module) {
    this->setRegressifier( module );
    return *this;
}

GestureRecognitionPipeline& GestureRecognitionPipeline::operator << (const Clusterer &module) {
    this->setClusterer( module );
    return *this;
}

GestureRecognitionPipeline& GestureRecognitionPipeline::operator << (const PostProcessing &module) {
    this->addPostProcessingModule( module );
    return *this;
}

GestureRecognitionPipeline::~GestureRecognitionPipeline(void)
{
    //Clean up the memory
    clear();
} 
    
bool GestureRecognitionPipeline::train_(ClassificationData &trainingData){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << __FILENAME__ << " Failed To Train Classifier, the classifier has not been set!" << std::endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << __FILENAME__ << " Failed To Train Classifier, there is no training data!" << std::endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Set the input Vector dimension size
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
    processedTrainingData.reserve( trainingData.getNumSamples() );
    UINT classLabel = 0;
    VectorFloat trainingSample;
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        bool okToAddProcessedData = true;
        classLabel = trainingData[i].getClassLabel();
        trainingSample = trainingData[i].getSample();
        
        //Perform any preprocessing
        if( getIsPreProcessingSet() ){
            for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                if( !preProcessingModules[moduleIndex]->process( trainingSample ) ){
                    errorLog << __FILENAME__ << " Failed to PreProcess Training Data. PreProcessingModuleIndex: ";
                    errorLog << moduleIndex;
                    errorLog << std::endl;
                    return false;
                }
                trainingSample = preProcessingModules[moduleIndex]->getProcessedData();
            }
        }
        
        //Compute any features
        if( getIsFeatureExtractionSet() ){
            for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                if( !featureExtractionModules[moduleIndex]->computeFeatures( trainingSample ) ){
                    errorLog << __FILENAME__ << " Failed to Compute Features from Training Data. FeatureExtractionModuleIndex ";
                    errorLog << moduleIndex;
                    errorLog << std::endl;
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
        warningLog << __FILENAME__ << " Lost " << trainingData.getNumSamples()-processedTrainingData.getNumSamples() << " of " << trainingData.getNumSamples() << " training samples due to the processing stage!" << std::endl;
    }

    //Store the number of training samples
    numTrainingSamples = processedTrainingData.getNumSamples();
    
    //Train the classifier
    trained = classifier->train_( processedTrainingData );
    if( !trained ){
        errorLog << __FILENAME__ << " Failed To Train Classifier: " << classifier->getLastErrorMessage() << std::endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::train(const ClassificationData &trainingData,const UINT kFoldValue,const bool useStratifiedSampling){
    //Get a copy of the data so we can spilt it
    ClassificationData data = trainingData;
    return train_( data, kFoldValue, useStratifiedSampling );
}

bool GestureRecognitionPipeline::train_(ClassificationData &data,const UINT kFoldValue,const bool useStratifiedSampling){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << __FILENAME__ << " Failed To Train Classifier, the classifier has not been set!" << std::endl;
        return false;
    }
    
    if( data.getNumSamples() == 0 ){
        errorLog << __FILENAME__ << " Failed To Train Classifier, there is no training data!" << std::endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();

    //Spilt the data into K folds
    if( !data.spiltDataIntoKFolds(kFoldValue, useStratifiedSampling) ){
        errorLog << __FILENAME__ << " Failed To Train Classifier, failed to split data into K folds!" << std::endl;
        return false;
    }
    
    //Run the k-fold training and testing
    Float crossValidationAccuracy = 0;
    ClassificationData foldTrainingData;
    ClassificationData foldTestData;
    Vector< TestResult > cvResults(kFoldValue);

    for(UINT k=0; k<kFoldValue; k++){
        ///Train the classification system
		foldTrainingData = data.getTrainingFoldData(k);
        
        if( !train_( foldTrainingData ) ){
            return false;
        }
        
        //Test the classification system
        foldTestData = data.getTestFoldData(k);
        
        if( !test( foldTestData ) ){
            return false;
        }
        
        crossValidationAccuracy += getTestAccuracy();
        cvResults[k] = getTestResults();
    }

    //Flag that the model has been trained
    trained = true;
    
    //Set the accuracy of the classification system averaged over the kfolds
    testAccuracy = crossValidationAccuracy / Float(kFoldValue);
    crossValidationResults = cvResults;
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::train_( TimeSeriesClassificationData &trainingData ){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << __FILENAME__ << " Failed To Train Classifier, the classifier has not been set!" << std::endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << __FILENAME__ << " Failed To Train Classifier, there is no training data!" << std::endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Set the input Vector dimension size of the pipeline
    inputVectorDimensions = trainingData.getNumDimensions();
   
    TimeSeriesClassificationData processedTrainingData( trainingData.getNumDimensions() );
    TimeSeriesClassificationData timeseriesClassificationData;
    ClassificationData classificationData;

    bool allowNullGestureClass = true;
	processedTrainingData.setAllowNullGestureClass( allowNullGestureClass );
	timeseriesClassificationData.setAllowNullGestureClass( allowNullGestureClass );
	classificationData.setAllowNullGestureClass( allowNullGestureClass );
    
    //Setup the data structure, if the classifier works with timeseries data then we use TimeSeriesClassificationData
    //otherwise we format the data as ClassificationData
    if( classifier->getTimeseriesCompatible() ){
        UINT trainingDataInputDimensionSize = trainingData.getNumDimensions();
        if( getIsPreProcessingSet() ){
            trainingDataInputDimensionSize = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
        if( getIsFeatureExtractionSet() ){
            trainingDataInputDimensionSize = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
        timeseriesClassificationData.setNumDimensions( trainingDataInputDimensionSize );
    }else{
        UINT trainingDataInputDimensionSize = trainingData.getNumDimensions();
        if( getIsPreProcessingSet() ){
            trainingDataInputDimensionSize = preProcessingModules[ preProcessingModules.size()-1 ]->getNumOutputDimensions();
        }
        if( getIsFeatureExtractionSet() ){
            trainingDataInputDimensionSize = featureExtractionModules[ featureExtractionModules.size()-1 ]->getNumOutputDimensions();
        }
        classificationData.setNumDimensions( trainingDataInputDimensionSize );
    }
    
    //Pass the timeseries data through any pre-processing modules and add it to the processedTrainingData structure
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        UINT classLabel = trainingData[i].getClassLabel();
        MatrixFloat trainingSample = trainingData[i].getData();
        
        if( getIsPreProcessingSet() ){
            
            //Try to process the matrix data row-by-row
            bool resetPreprocessingModule = true;
            VectorFloat sample;
            for(UINT r=0; r<trainingSample.getNumRows(); r++){
                sample = trainingSample.getRow( r );
                
                for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                    
                    if( resetPreprocessingModule ){
                        preProcessingModules[moduleIndex]->reset();
                    }
                    
                    //Validate the input and output dimensions match!
                    if( preProcessingModules[moduleIndex]->getNumInputDimensions() != preProcessingModules[moduleIndex]->getNumOutputDimensions() ){
                        errorLog << __FILENAME__ << " Failed To PreProcess Training Data. The number of inputDimensions (";
                        errorLog << preProcessingModules[moduleIndex]->getNumInputDimensions();
                        errorLog << ") in  PreProcessingModule ";
                        errorLog << moduleIndex;
                        errorLog << " do not match the number of outputDimensions (";
                        errorLog << preProcessingModules[moduleIndex]->getNumOutputDimensions();
                        errorLog <<  std::endl;
                        return false;
                    }
                    
                    if( !preProcessingModules[moduleIndex]->process( sample ) ){
                        errorLog << __FILENAME__ << " Failed To PreProcess Training Data. PreProcessingModuleIndex: ";
                        errorLog << moduleIndex;
                        errorLog << std::endl;
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
        MatrixFloat trainingSample = processedTrainingData[i].getData();
        bool featureDataReady = false;
        bool resetFeatureExtractionModules = true;
        
		VectorFloat inputVector;
        MatrixFloat featureData;
        //Try to process the matrix data row-by-row
        for(UINT r=0; r<trainingSample.getNumRows(); r++){
            inputVector = trainingSample.getRow( r );
            featureDataReady = true;
            
             //Pass the processed training data through the feature extraction
            if( getIsFeatureExtractionSet() ){
            
                for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                    
                    if( resetFeatureExtractionModules ){
                        featureExtractionModules[moduleIndex]->reset();
                    }
                    
                    if( !featureExtractionModules[moduleIndex]->computeFeatures( inputVector ) ){
                        errorLog << __FILENAME__ << " Failed To Compute Features For Training Data. FeatureExtractionModuleIndex: ";
                        errorLog << moduleIndex;
                        errorLog << std::endl;
                        return false;
                    }
                    
                    //Overwrite the input Vector with the features so this can either be input to the next feature module 
                    //or converted to the LabelledClassificationData format
                    inputVector = featureExtractionModules[moduleIndex]->getFeatureVector();
                    featureDataReady = featureExtractionModules[moduleIndex]->getFeatureDataReady();
                    
                    //If feature data is not ready at any stage, stop the rest of the pipeline.
                    if( !featureDataReady ) { break; }
                }
                
                //The feature extraction modules should only be reset on r == 0
                resetFeatureExtractionModules = false;
                
                if( featureDataReady ){
                    
                    if( classifier->getTimeseriesCompatible() ){
                        if( !featureData.push_back( inputVector ) ){
                            errorLog << __FILENAME__ << " Failed To add feature Vector to feature data matrix! FeatureExtractionModuleIndex: " << std::endl;
                            return false;
                        }
                    }else classificationData.addSample(classLabel, inputVector);
                }
                
            }else{
                if( classifier->getTimeseriesCompatible() ){
                    if( !featureData.push_back( inputVector ) ){
                        errorLog << __FILENAME__ << " Failed To add feature Vector to feature data matrix! FeatureExtractionModuleIndex: " << std::endl;
                        return false;
                    }
                }
                else classificationData.addSample(classLabel, inputVector);
            }
        }
        
        if( classifier->getTimeseriesCompatible() ) timeseriesClassificationData.addSample(classLabel, featureData);
        
    }
        
    //Train the classification system
    if( classifier->getTimeseriesCompatible() ){
        numTrainingSamples = timeseriesClassificationData.getNumSamples();
        trained = classifier->train_( timeseriesClassificationData );
    }else{
        numTrainingSamples = classificationData.getNumSamples();
        trained = classifier->train_( classificationData );
    }

    if( !trained ){
        errorLog << __FILENAME__ << " Failed To Train Classifier" << classifier->getLastErrorMessage() << std::endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::train(const TimeSeriesClassificationData &trainingData,const UINT kFoldValue,const bool useStratifiedSampling){
    //Get a copy of the data so we can split it
    TimeSeriesClassificationData data = trainingData;
    return train_( data, kFoldValue, useStratifiedSampling );
}

bool GestureRecognitionPipeline::train_(TimeSeriesClassificationData &data,const UINT kFoldValue,const bool useStratifiedSampling){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << __FILENAME__ << " - Failed To Train Classifier, the classifier has not been set!" << std::endl;
        return false;
    }
    
    if( data.getNumSamples() == 0 ){
        errorLog << __FILENAME__ << " Failed To Train Classifier, there is no training data!" << std::endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Spilt the data into K folds
    if( !data.spiltDataIntoKFolds(kFoldValue, useStratifiedSampling) ){
        errorLog << __FILENAME__ << " Failed To Spilt Dataset into KFolds!" << std::endl;
        return false;
    }
    
    //Run the k-fold training and testing
    Float crossValidationAccuracy = 0;
    TimeSeriesClassificationData foldTrainingData;
    TimeSeriesClassificationData foldTestData;
    
    for(UINT k=0; k<kFoldValue; k++){
        ///Train the classification system
		foldTrainingData = data.getTrainingFoldData(k);
        
        if( !train_( foldTrainingData ) ){
            errorLog << __FILENAME__ << " Failed to train pipeline for fold " << k << "." << std::endl;
            return false;
        }
        
        //Test the classification system
        foldTestData = data.getTestFoldData(k);
        
        if( !test( foldTestData ) ){
            errorLog << __FILENAME__ << " Failed to test pipeline for fold " << k << "." << std::endl;
            return false;
        }
        
        crossValidationAccuracy += getTestAccuracy();
    }

    //Flag that the model has been trained
    trained = true;
    
    //Set the accuracy of the classification system averaged over the kfolds
    testAccuracy = crossValidationAccuracy / Float(kFoldValue);
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::train_(ClassificationDataStream &trainingData){

    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClassifierSet() ){
        errorLog << __FILENAME__ << " Failed To train Classifier, the classifier has not been set!" << std::endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << __FILENAME__ << " Failed To train Classifier, there is no training data!" << std::endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Set the input Vector dimension size
    inputVectorDimensions = trainingData.getNumDimensions();
    
    //Pass the training data through any pre-processing or feature extraction units
    UINT numDimensions = inputVectorDimensions;
    
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
    processedTrainingData.reserve( trainingData.getNumSamples() );
    UINT classLabel = 0;
    VectorFloat trainingSample;
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        bool okToAddProcessedData = true;
        classLabel = trainingData[i].getClassLabel();
        trainingSample = trainingData[i].getSample();
        
        //Perform any preprocessing
        if( getIsPreProcessingSet() ){
            for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                if( !preProcessingModules[moduleIndex]->process( trainingSample ) ){
                    errorLog << __FILENAME__ << " Failed to PreProcess training Data. PreProcessingModuleIndex: ";
                    errorLog << moduleIndex;
                    errorLog << std::endl;
                    return false;
                }
                trainingSample = preProcessingModules[moduleIndex]->getProcessedData();
            }
        }
        
        //Compute any features
        if( getIsFeatureExtractionSet() ){
            for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                if( !featureExtractionModules[moduleIndex]->computeFeatures( trainingSample ) ){
                    errorLog << __FILENAME__ << " Failed to Compute Features from training Data. FeatureExtractionModuleIndex ";
                    errorLog << moduleIndex;
                    errorLog << std::endl;
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
        warningLog << __FILENAME__ << " Lost " << trainingData.getNumSamples()-processedTrainingData.getNumSamples() << " of " << trainingData.getNumSamples() << " training samples due to the processing stage!" << std::endl;
    }

    //Store the number of training samples
    numTrainingSamples = processedTrainingData.getNumSamples();
    
    //Train the classifier
    trained = classifier->train_( processedTrainingData );
    if( !trained ){
        errorLog << __FILENAME__ << " Failed To Train Classifier: " << classifier->getLastErrorMessage() << std::endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();

    return true;
}
    
bool GestureRecognitionPipeline::train_(RegressionData &trainingData){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();
    
    //Set the input Vector dimension size
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
        VectorFloat inputVector = trainingData[i].getInputVector();
        VectorFloat targetVector = trainingData[i].getTargetVector();
        
        if( getIsPreProcessingSet() ){
            for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                if( !preProcessingModules[ moduleIndex ]->process( inputVector ) ){
                    errorLog << __FILENAME__ << " Failed To Compute Features For Training Data. PreProcessingModuleIndex: " << moduleIndex << std::endl;
                    return false;
                }
                
                inputVector = preProcessingModules[ moduleIndex ]->getProcessedData();
            }
        }
        
        if( getIsFeatureExtractionSet() ){
            for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                if( !featureExtractionModules[ moduleIndex ]->computeFeatures( inputVector ) ){
                    errorLog << __FILENAME__ << " Failed To Compute Features For Training Data. FeatureExtractionModuleIndex: " << moduleIndex << std::endl;
                    return false;
                }
                
                inputVector = featureExtractionModules[ moduleIndex ]->getFeatureVector();
            }
        }
        
        //Add the training sample to the processed training data
        if( !processedTrainingData.addSample(inputVector,targetVector) ){
            errorLog << __FILENAME__ << " Failed to add processed training sample to training data" << std::endl;
            return false;
        }
    }
    
    //Store the number of training samples
    numTrainingSamples = processedTrainingData.getNumSamples();
    
    //Train the classification system
    if( getIsRegressifierSet() ){
        trained =  regressifier->train_( processedTrainingData );
        if( !trained ){
            errorLog << __FILENAME__ << " Failed To Train Regressifier: " << regressifier->getLastErrorMessage() << std::endl;
            return false;
        }
    }else{
        errorLog << __FILENAME__ << " Classifier is not set" << std::endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::train_(RegressionData &trainingData,RegressionData &validationData){ return false; } //TODO Need to add this function

bool GestureRecognitionPipeline::train(const RegressionData &trainingData,const UINT kFoldValue){
    //Get a copy of the training data so we can split it
    RegressionData data = trainingData;
    return train_( data, kFoldValue );
}

bool GestureRecognitionPipeline::train_(RegressionData &data,const UINT kFoldValue){

    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsRegressifierSet() ){
        errorLog << __FILENAME__ << " Failed To Train Regressifier, the regressifier has not been set!" << std::endl;
        return false;
    }
    
    if( data.getNumSamples() == 0 ){
        errorLog << __FILENAME__ << " Failed To Train Regressifier, there is no training data!" << std::endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Start the training timer
    Timer timer;
    timer.start();

    //Spilt the data into K folds
    bool spiltResult = data.spiltDataIntoKFolds(kFoldValue);
    
    if( !spiltResult ){
        return false;
    }
    
    //Run the k-fold training and testing
    Float crossValidationAccuracy = 0;
    RegressionData foldTrainingData;
    RegressionData foldTestData;
    for(UINT k=0; k<kFoldValue; k++){
        ///Train the classification system
        foldTrainingData = data.getTrainingFoldData(k);
        
        if( !train_( foldTrainingData ) ){
            return false;
        }
        
        //Test the classification system
        foldTestData = data.getTestFoldData(k);
        
        if( !test( foldTestData ) ){
            return false;
        }

        crossValidationAccuracy += getTestRMSError();
        
    }

    //Flag that the model has been trained
    trained = true;

    testAccuracy = crossValidationAccuracy / Float(kFoldValue);
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::train_(UnlabelledData &trainingData){
    
    trained = false;
    trainingTime = 0;
    clearTestResults();
    
    if( !getIsClustererSet() ){
        errorLog << __FILENAME__ << " Failed To Train Clusterer, the clusterer has not been set!" << std::endl;
        return false;
    }
    
    if( trainingData.getNumSamples() == 0 ){
        errorLog << __FILENAME__ << " Failed To Train Clusterer, there is no training data!" << std::endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Set the input Vector dimension size
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
    
    UnlabelledData processedTrainingData( numDimensions );
    
    for(UINT i=0; i<trainingData.getNumSamples(); i++){
        bool okToAddProcessedData = true;
        VectorFloat trainingSample = trainingData[i];
        
        //Perform any preprocessing
        if( getIsPreProcessingSet() ){
            for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
                if( !preProcessingModules[moduleIndex]->process( trainingSample ) ){
                    errorLog << __FILENAME__ << " Failed to PreProcess Training Data. PreProcessingModuleIndex: ";
                    errorLog << moduleIndex;
                    errorLog << std::endl;
                    return false;
                }
                trainingSample = preProcessingModules[moduleIndex]->getProcessedData();
            }
        }
        
        //Compute any features
        if( getIsFeatureExtractionSet() ){
            for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.size(); moduleIndex++){
                if( !featureExtractionModules[moduleIndex]->computeFeatures( trainingSample ) ){
                    errorLog << __FILENAME__ << " Failed to Compute Features from Training Data. FeatureExtractionModuleIndex ";
                    errorLog << moduleIndex;
                    errorLog << std::endl;
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
            processedTrainingData.addSample(trainingSample);
        }
        
    }
    
    if( processedTrainingData.getNumSamples() != trainingData.getNumSamples() ){
        
        warningLog << __FILENAME__ << " Lost " << trainingData.getNumSamples()-processedTrainingData.getNumSamples() << " of " << trainingData.getNumSamples() << " training samples due to the processing stage!" << std::endl;
    }
    
    //Store the number of training samples
    numTrainingSamples = processedTrainingData.getNumSamples();
    
    //Train the cluster model
    trained = clusterer->train_( processedTrainingData );
    if( !trained ){
        errorLog << __FILENAME__ << " Failed To Train Clusterer: " << clusterer->getLastErrorMessage() << std::endl;
        return false;
    }
    
    //Store the training time
    trainingTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::test(const ClassificationData &testData){
    
    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorLog << __FILENAME__ << " Classifier is not trained" << std::endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input Vector's dimensions
    if( testData.getNumDimensions() != inputVectorDimensions ){
        errorLog << __FILENAME__ << " The dimensionality of the test data (" + Util::toString(testData.getNumDimensions()) + ") does not match that of the input Vector dimensions of the pipeline (" << inputVectorDimensions << ")" << std::endl;
        return false;
    }
    
    if( !getIsClassifierSet() ){
        errorLog << __FILENAME__ << " The classifier has not been set" << std::endl;
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
            errorLog << __FILENAME__ << " The test dataset contains a class label (" << testData.getClassTracker()[i].classLabel << ") that is not in the model!" << std::endl;
		}
	}

	if( !classLabelValidationPassed ){
        errorLog << __FILENAME__ << "  Model Class Labels: ";
        for(UINT k=0; k<classifier->getNumClasses(); k++){
            errorLog << classifier->getClassLabels()[k] << "\t";
		}
        errorLog << std::endl;
        return false;
    }

    Float rejectionPrecisionCounter = 0;
    Float rejectionRecallCounter = 0;
    unsigned int confusionMatrixSize = classifier->getNullRejectionEnabled() ? classifier->getNumClasses()+1 : classifier->getNumClasses();
    VectorFloat precisionCounter(classifier->getNumClasses(), 0);
    VectorFloat recallCounter(classifier->getNumClasses(), 0);
    VectorFloat confusionMatrixCounter(confusionMatrixSize,0);
    
    //Resize the test matrix
    testConfusionMatrix.resize(confusionMatrixSize, confusionMatrixSize);
    testConfusionMatrix.setAllValues(0);
    
    //Resize the precision and recall Vectors
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
        VectorFloat testSample = testData[i].getSample();
        
        //Pass the test sample through the pipeline
        if( !predict_( testSample ) ){
            errorLog << __FILENAME__ << " Prediction failed for test sample at index: " << i << std::endl;
            return false;
        }
        
        //Update the test metrics
        UINT predictedClassLabel = getPredictedClassLabel();
        
        if( !updateTestMetrics(classLabel,predictedClassLabel,precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter) ){
            errorLog << __FILENAME__ << " Failed to update test metrics at test sample index: " << i << std::endl;
            return false;
        }
        //cout << "i: " << i << " class label: " << classLabel << " predictedClassLabel: " << predictedClassLabel << std::endl;
        
        //Keep track of the classification results encase the user needs them later
        testResults[i].setClassificationResult(i, classLabel, predictedClassLabel, getUnProcessedPredictedClassLabel(),getClassLikelihoods(), getClassDistances());

        //Update any observers
        classifier->notifyTestResultsObservers( testResults[i] );
    }
    
    if( !computeTestMetrics(precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter, numTestSamples) ){
        errorLog << __FILENAME__ << " Failed to compute test metrics!" << std::endl;
        return false;
    }
    
    testTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::test(const TimeSeriesClassificationData &testData){

    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorLog << __FILENAME__ << " The classifier has not been trained" << std::endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input Vector's dimensions
    if( testData.getNumDimensions() != inputVectorDimensions ){
        errorLog << __FILENAME__ << " The dimensionality of the test data (" << testData.getNumDimensions() << ") does not match that of the input Vector dimensions of the pipeline (" << inputVectorDimensions << ")" << std::endl;
        return false;
    }
    
    if( !getIsClassifierSet() ){
        errorLog << __FILENAME__ << " The classifier has not been set" << std::endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    Float rejectionPrecisionCounter = 0;
    Float rejectionRecallCounter = 0;
    const UINT K = classifier->getNumClasses();
    UINT confusionMatrixSize = classifier->getNullRejectionEnabled() ? K+1 : K;
    VectorFloat precisionCounter(K, 0);
    VectorFloat recallCounter(K, 0);
    VectorFloat confusionMatrixCounter(confusionMatrixSize,0);
    
    //Resize the test matrix
    testConfusionMatrix.resize(confusionMatrixSize,confusionMatrixSize);
    testConfusionMatrix.setAllValues(0);
    
    //Resize the precision and recall Vectors
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
        MatrixFloat timeseries = testData[i].getData();
            
        //Pass the test timeseries through the pipeline
        if( !predict_( timeseries ) ){
            errorLog << __FILENAME__ << " Failed to run prediction for test sample index: " << i << std::endl;
            return false;
        }
        
        //Update the test metrics
        UINT predictedClassLabel = getPredictedClassLabel();
        
        if( !updateTestMetrics(classLabel,predictedClassLabel,precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter) ){
            errorLog << __FILENAME__ << " Failed to update test metrics at test sample index: " << i << std::endl;
            return false;
        }
        
    }
        
    if( !computeTestMetrics(precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter, M) ){
        errorLog << __FILENAME__ << " Failed to compute test metrics!" << std::endl;
        return false;
    }
    
    testTime = timer.getMilliSeconds();
    
    return true;
}
    
bool GestureRecognitionPipeline::test(const ClassificationDataStream &testData){
    
    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorLog << __FILENAME__ << " The classifier has not been trained" << std::endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input Vector's dimensions
    if( testData.getNumDimensions() != inputVectorDimensions ){
        errorLog << __FILENAME__ << " The dimensionality of the test data (" + Util::toString(testData.getNumDimensions()) + ") does not match that of the input Vector dimensions of the pipeline (" << inputVectorDimensions << ")" << std::endl;
        return false;
    }
    
    if( !getIsClassifierSet() ){
        errorLog << __FILENAME__ << " The classifier has not been set" << std::endl;
        return false;
    }
    
    //Reset all the modules
    reset();
    
    //Float rejectionPrecisionCounter = 0;
    //Float rejectionRecallCounter = 0;
    UINT confusionMatrixSize = classifier->getNullRejectionEnabled() ? classifier->getNumClasses()+1 : classifier->getNumClasses();
    VectorFloat precisionCounter(getNumClassesInModel(), 0);
    VectorFloat recallCounter(getNumClassesInModel(), 0);
    VectorFloat confusionMatrixCounter(confusionMatrixSize,0);
    
    //Resize the test matrix
    testConfusionMatrix.resize(confusionMatrixSize,confusionMatrixSize);
    testConfusionMatrix.setAllValues(0);
    
    //Resize the precision and recall Vectors
    testPrecision.resize(getNumClassesInModel(), 0);
    testRecall.resize(getNumClassesInModel(), 0);
    testFMeasure.resize(getNumClassesInModel(), 0);
    
    //Resize the classification results Vector
    testResults.resize(testData.getNumSamples());
    numTestSamples = testData.getNumSamples();
    
    //Start the test timer
    Timer timer;
    timer.start();

    //Get a copy of the data so we can modify it
    ClassificationDataStream data = testData;
    
    //Run the test
    data.resetPlaybackIndex(0); //Make sure that the test data start at 0
    for(UINT i=0; i<data.getNumSamples(); i++){
        ClassificationSample sample = data.getNextSample();
        UINT classLabel = sample.getClassLabel();
        VectorFloat testSample = sample.getSample();
            
        //Pass the test sample through the pipeline
        if( !predict_( testSample ) ){
            errorLog << __FILENAME__ << " Prediction Failed! " << classifier->getLastErrorMessage() << std::endl;
            return false;
        }
        
        //Update the test metrics
        UINT predictedClassLabel = getPredictedClassLabel();
        
        /* //TODO - Need to update this!
        if( !updateTestMetrics(classLabel,predictedClassLabel,precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter) ){
            errorLog << "test(LabelledContinuousTimeSeriesClassificationData &testData) - Failed to update test metrics at test sample index: " << i << std::endl;
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
        testingLog << "\tLikelihood: " << getMaximumLikelihood() << std::endl;
    }
    
    /* //TODO - Need to update this!
    if( !computeTestMetrics(precisionCounter,recallCounter,rejectionPrecisionCounter,rejectionRecallCounter, confusionMatrixCounter, testData.getNumSamples()) ){
        errorLog << "test(LabelledContinuousTimeSeriesClassificationData &testData) - Failed to compute test metrics !" << std::endl;
        return false;
    }
     */
    
    testTime = timer.getMilliSeconds();
    testAccuracy = testAccuracy / Float( testData.getNumSamples() ) * 100.0;
    
    testingLog << "Test complete. Total testing time: " << testTime << std::endl;
    
    return true;
}
    
bool GestureRecognitionPipeline::test(const RegressionData &testData){
    
    //Clear any previous test results
    clearTestResults();
    
    //Make sure the classification model has been trained
    if( !trained ){
        errorLog << __FILENAME__ << " Regressifier is not trained" << std::endl;
        return false;
    }
    
    //Make sure the dimensionality of the test data matches the input Vector's dimensions
    if( testData.getNumInputDimensions() != inputVectorDimensions ){
        errorLog << __FILENAME__ << " The dimensionality of the test data (" << testData.getNumInputDimensions() << ") does not match that of the input Vector dimensions of the pipeline (" << inputVectorDimensions << ")" << std::endl;
        return false;
    }
    
    if( !getIsRegressifierSet() ){
        errorLog << __FILENAME__ << " The regressifier has not been set" << std::endl;
        return false;
    }
    
    if( regressifier->getNumOutputDimensions() != testData.getNumTargetDimensions() ){
        errorLog << __FILENAME__ << " The size of the output of the regressifier (" << regressifier->getNumOutputDimensions() << ") does not match that of the size of the number of target dimensions (" << testData.getNumTargetDimensions() << ")" << std::endl;
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
        VectorFloat inputVector = testData[i].getInputVector();
        VectorFloat targetVector = testData[i].getTargetVector();
        
        //Pass the test sample through the pipeline
        if( !map( inputVector ) ){
            errorLog << __FILENAME__ << " Failed to map input Vector!" << std::endl;
            return false;
        }
        
        //Update the RMS error
        Float sum = 0;
        VectorFloat regressionData = regressifier->getRegressionData();
        for(UINT j=0; j<targetVector.size(); j++){
            sum += grt_sqr( regressionData[j]-targetVector[j] );
        }

        testSquaredError += sum;

        //Keep track of the regression results encase the user needs them later
        testResults[i].setRegressionResult(i,regressionData,targetVector);

        //Update any observers
        regressifier->notifyTestResultsObservers( testResults[i] );
    }
    
    //Compute the test metrics
    testRMSError = sqrt( testSquaredError / Float( testData.getNumSamples() ) );
    
    testTime = timer.getMilliSeconds();
    
    return true;
}

bool GestureRecognitionPipeline::predict_(VectorFloat &inputVector){
	
	//Make sure the classification model has been trained
    if( !trained ){
        errorLog << __FILENAME__ << " The classifier has not been trained" << std::endl;
        return false;
    }
    
    //Make sure the dimensionality of the input Vector matches the inputVectorDimensions
    if( inputVector.size() != inputVectorDimensions ){
        errorLog << __FILENAME__ << " The dimensionality of the input Vector (" << int(inputVector.size()) << ") does not match that of the input Vector dimensions of the pipeline (" << inputVectorDimensions << ")" << std::endl;
        return false;
    }

	if( getIsClassifierSet() ){
        return predict_classifier( inputVector );
    }

	if( getIsRegressifierSet() ){
        return predict_regressifier( inputVector );
    }
    
    if( getIsClustererSet() ){
        return predict_clusterer( inputVector );
    }

    errorLog << __FILENAME__ << " Neither a classifier, regressifer or clusterer is set" << std::endl;
	return false;
}

bool GestureRecognitionPipeline::predict_(MatrixFloat &input){
	
	//Make sure the classification model has been trained
    if( !trained ){
        errorLog << __FILENAME__ << " The classifier has not been trained!" << std::endl;
        return false;
    }

    //Make sure the dimensionality of the input matrix matches the inputVectorDimensions
    if( input.getNumCols() != inputVectorDimensions ){
        errorLog << __FILENAME__ << " The dimensionality of the input matrix (" << input.getNumCols() << ") does not match that of the input Vector dimensions of the pipeline (" << inputVectorDimensions << ")" << std::endl;
        return false;
    }

    if( !getIsClassifierSet() ){
        errorLog << __FILENAME__ << " A classifier has not been set" << std::endl;
        return false;
    }

    //Get a copy of the input matrix so it can be processed
    MatrixFloat inputMatrix = input;

    //Get a pointer to the input matrix so we can pass it down the pipeline
    const void *data = static_cast< const void* >( &inputMatrix );
    DataType dataType = DATA_TYPE_MATRIX;

    //Setup a temporary matrix and vector
    //VectorFloat tmpVector;
    //MatrixFloat tmpMatrix;
    
    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    
    //Perform any pre-processing
    /*
    if( getIsPreProcessingSet() ){
        
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.size(); moduleIndex++){
            MatrixFloat tmpMatrix( inputMatrix.getNumRows(), preProcessingModules[moduleIndex]->getNumOutputDimensions() );
            
            for(UINT i=0; i<inputMatrix.getNumRows(); i++){
                if( !preProcessingModules[moduleIndex]->process( inputMatrix.getRow(i) ) ){
                    errorLog << "predict_timeseries(const MatrixFloat &inputMatrix) - Failed to PreProcess Input Matrix. PreProcessingModuleIndex: " << moduleIndex << std::endl;
                    return false;
                }
                tmpMatrix.setRowVector( preProcessingModules[moduleIndex]->getProcessedData(), i );
            }
            
            //Update the input matrix with the preprocessed data
            inputMatrix = tmpMatrix;
        }
    }
    */
    
    //Update the context module
    predictionModuleIndex = AFTER_PREPROCESSING;
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() ){

        const void *feInput = data;
        const void *feOutput = NULL;
        const UINT numFeatureExtractionModules = featureExtractionModules.getSize();
        DataType inputType = DATA_TYPE_UNKNOWN;
        DataType outputType = DATA_TYPE_UNKNOWN;
        for(UINT moduleIndex=0; moduleIndex<numFeatureExtractionModules; moduleIndex++){

            inputType = featureExtractionModules[ moduleIndex ]->getInputType();
            outputType = featureExtractionModules[ moduleIndex ]->getOutputType();

            //Run the feature extraction algorithm
            switch( inputType ){
                case DATA_TYPE_VECTOR:
                    if( !featureExtractionModules[ moduleIndex ]->computeFeatures( *static_cast< const VectorFloat* >( feInput ) ) ){
                        errorLog << __FILENAME__ << " Failed to PreProcess Input Matrix. FeatureExtractionModuleIndex: " << moduleIndex << std::endl;
                        return false;
                    }
                break;
                case DATA_TYPE_MATRIX:
                    if( !featureExtractionModules[ moduleIndex ]->computeFeatures( *static_cast< const MatrixFloat* >( feInput ) ) ){
                        errorLog << __FILENAME__ << " Failed to PreProcess Input Matrix. FeatureExtractionModuleIndex: " << moduleIndex << std::endl;
                        return false;
                    }
                break;
                default:
                    errorLog << __FILENAME__ << " Failed to process data. Unknown output data type for FeatureExtractionModuleIndex: " << moduleIndex << std::endl;
                    return false;
                break;
            }
            
            //Get the results and store them in the feOutput pointer
            switch( outputType ){
                case DATA_TYPE_VECTOR:
                    feOutput = static_cast< const void* >( &featureExtractionModules[ moduleIndex ]->getFeatureVector() );
                break;
                case DATA_TYPE_MATRIX:
                    feOutput = static_cast< const void* >( &featureExtractionModules[ moduleIndex ]->getFeatureMatrix() );
                break;
                default:
                    errorLog << __FILENAME__ << " Failed to process data. Unknown output data type for FeatureExtractionModuleIndex: " << moduleIndex << std::endl;
                    return false;
                break;
            }

            //If there are more feature extraction modules to process, then set the feInput 
            if( moduleIndex+1 < numFeatureExtractionModules ){
                feInput = feOutput;
            }
        }

        //Update the data pointer to the data from the last output from the end feature extraction module
        data = feOutput;
        dataType = outputType;
    }
    
    //Update the context module
    predictionModuleIndex = AFTER_FEATURE_EXTRACTION;

    //Perform the classification
    switch( dataType ){
        case DATA_TYPE_VECTOR:
            if( !classifier->predict_( *(VectorFloat*)data ) ){
                errorLog << __FILENAME__ << " Prediction Failed! " << classifier->getLastErrorMessage() << std::endl;
                return false;
            }
        break;
        case DATA_TYPE_MATRIX:
            if( !classifier->predict_( *(MatrixFloat*)data ) ){
                errorLog << __FILENAME__ << " Prediction Failed! " << classifier->getLastErrorMessage() << std::endl;
                return false;
            }
        break;
        default:
            errorLog << __FILENAME__ << " Failed to run prediction. Unknown data type!" << std::endl;
            return false;
        break;
    }
    
    predictedClassLabel = classifier->getPredictedClassLabel();
    
    //Update the context module
    //Todo
    
    //Perform any post processing
    predictionModuleIndex = AFTER_CLASSIFIER;
/*
    if( getIsPostProcessingSet() ){
        
        if( pipelineMode != CLASSIFICATION_MODE){
            errorLog << "predict_timeseries(const MatrixFloat &inputMatrix) - Pipeline Mode Is Not in CLASSIFICATION_MODE!" << std::endl;
            return false;
        }
        
        VectorFloat data;
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            
            //Select which input we should give the postprocessing module
            if( postProcessingModules[moduleIndex]->getIsPostProcessingInputModePredictedClassLabel() ){
                //Set the input
                data.resize(1);
                data[0] = predictedClassLabel;
                
                //Verify that the input size is OK
                if( data.size() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                    errorLog << "predict_timeseries(const MatrixFloat &inputMatrix) - The size of the data Vector (" << int(data.size()) << ") does not match that of the postProcessingModule (" << postProcessingModules[moduleIndex]->getNumInputDimensions() << ") at the moduleIndex: " << moduleIndex << std::endl;
                    return false;
                }
                
                //Postprocess the data
                if( !postProcessingModules[moduleIndex]->process( data ) ){
                    errorLog << "predict_timeseries(const MatrixFloat &inputMatrix) - Failed to post process data. PostProcessing moduleIndex: " << moduleIndex << std::endl;
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
                    errorLog << "predict_timeseries(const MatrixFloat &inputMatrix) - The size of the processed data Vector (" << int(data.size()) << ") from postProcessingModule at the moduleIndex: " << moduleIndex << " is not equal to 1 even though it is in OutputModePredictedClassLabel!" << std::endl;
                    return false;
                }
                
                //Update the predicted class label
                predictedClassLabel = (UINT)data[0];
            }
                  
        }
    } 
*/
    //Update the context module
    //TODO
    predictionModuleIndex = END_OF_PIPELINE;
	return true;
}

bool GestureRecognitionPipeline::map_(VectorFloat &inputVector){
	return predict_regressifier( inputVector );
}

bool GestureRecognitionPipeline::predict_classifier(const VectorFloat &input){
    
    predictedClassLabel = 0;
	VectorFloat inputVector = input;
    
    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    if( contextModules[ START_OF_PIPELINE ].size() > 0 ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ START_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->process( inputVector ) ){
                errorLog << __FILENAME__ << " Context Module Failed at START_OF_PIPELINE. ModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Failed to PreProcess Input Vector. PreProcessingModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Context Module Failed at AFTER_PREPROCESSING. ModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Failed to compute features from data. FeatureExtractionModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Context Module Failed at AFTER_FEATURE_EXTRACTION. ModuleIndex: " << moduleIndex << std::endl;
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
    if( !classifier->predict_(inputVector) ){
        errorLog << __FILENAME__ << " Prediction Failed! " << classifier->getLastErrorMessage() << std::endl;
        return false;
    }
    predictedClassLabel = classifier->getPredictedClassLabel();
    
    //Update the context module
    if( contextModules[ AFTER_CLASSIFIER ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_CLASSIFIER ].size(); moduleIndex++){
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->process( VectorFloat(1,predictedClassLabel) ) ){
                errorLog << __FILENAME__ << " Context Module Failed at AFTER_CLASSIFIER. ModuleIndex: " << moduleIndex << std::endl;
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
            errorLog << __FILENAME__ << " Pipeline Mode Is Not in CLASSIFICATION_MODE!" << std::endl;
            return false;
        }
        
        VectorFloat data;
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            
            //Select which input we should give the postprocessing module
            if( postProcessingModules[moduleIndex]->getIsPostProcessingInputModePredictedClassLabel() ){
                //Set the input
                data.resize(1);
                data[0] = predictedClassLabel;
                
                //Verify that the input size is OK
                if( data.size() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                    errorLog << __FILENAME__ << " The size of the data Vector (" << int(data.size()) << ") does not match that of the postProcessingModule (" << postProcessingModules[moduleIndex]->getNumInputDimensions() << ") at the moduleIndex: " << moduleIndex << std::endl;
                    return false;
                }
                
                //Postprocess the data
                if( !postProcessingModules[moduleIndex]->process( data ) ){
                    errorLog << __FILENAME__ << " Failed to post process data. PostProcessing moduleIndex: " << moduleIndex << std::endl;
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
                    errorLog << __FILENAME__ << " The size of the processed data Vector (" << int(data.size()) << ") from postProcessingModule at the moduleIndex: " << moduleIndex << " is not equal to 1 even though it is in OutputModePredictedClassLabel!" << std::endl;
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
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->process( VectorFloat(1,predictedClassLabel) ) ){
                errorLog << __FILENAME__ << " Context Module Failed at END_OF_PIPELINE. ModuleIndex: " << moduleIndex << std::endl;
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
    
bool GestureRecognitionPipeline::predict_regressifier(const VectorFloat &input){
    
	VectorFloat inputVector = input;

    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    if( contextModules[ START_OF_PIPELINE ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ START_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->process( inputVector ) ){
                errorLog << __FILENAME__ << " Context Module Failed at START_OF_PIPELINE. ModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Failed to PreProcess Input Vector. PreProcessingModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Context Module Failed at AFTER_PREPROCESSING. ModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Failed to compute features from data. FeatureExtractionModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Context Module Failed at AFTER_FEATURE_EXTRACTION. ModuleIndex: " << moduleIndex << std::endl;
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
    if( !regressifier->predict_(inputVector) ){
        errorLog << __FILENAME__ << " Prediction Failed! " << regressifier->getLastErrorMessage() << std::endl;
        return false;
    }
    regressionData = regressifier->getRegressionData();
    
    //Update the context module
    if( contextModules[ AFTER_CLASSIFIER ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_CLASSIFIER ].size(); moduleIndex++){
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->process( regressionData ) ){
                errorLog << __FILENAME__ << " Context Module Failed at AFTER_CLASSIFIER. ModuleIndex: " << moduleIndex << std::endl;
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
            errorLog << __FILENAME__ << " Pipeline Mode Is Not In RegressionMode!" << std::endl;
            return false;
        }
          
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            if( regressionData.size() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                errorLog << __FILENAME__ << " The size of the regression Vector (" << int(regressionData.size()) << ") does not match that of the postProcessingModule (" << postProcessingModules[moduleIndex]->getNumInputDimensions() << ") at the moduleIndex: " << moduleIndex << std::endl;
                return false;
            }
            
            if( !postProcessingModules[moduleIndex]->process( regressionData ) ){
                errorLog << __FILENAME__ << " Failed to post process data. PostProcessing moduleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Context Module Failed at END_OF_PIPELINE. ModuleIndex: " << moduleIndex << std::endl;
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
    
bool GestureRecognitionPipeline::predict_clusterer(const VectorFloat &input){
    
	VectorFloat inputVector = input;
    predictedClusterLabel = 0;
    
    //Update the context module
    predictionModuleIndex = START_OF_PIPELINE;
    if( contextModules[ START_OF_PIPELINE ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ START_OF_PIPELINE ].size(); moduleIndex++){
            if( !contextModules[ START_OF_PIPELINE ][moduleIndex]->process( inputVector ) ){
                errorLog << __FILENAME__ << " Context Module Failed at START_OF_PIPELINE. ModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Failed to PreProcess Input Vector. PreProcessingModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Context Module Failed at AFTER_PREPROCESSING. ModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Failed to compute features from data. FeatureExtractionModuleIndex: " << moduleIndex << std::endl;
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
                errorLog << __FILENAME__ << " Context Module Failed at AFTER_FEATURE_EXTRACTION. ModuleIndex: " << moduleIndex << std::endl;
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
    if( !clusterer->predict_(inputVector) ){
        errorLog << __FILENAME__ << " Prediction Failed! " << clusterer->getLastErrorMessage() << std::endl;
        return false;
    }
    predictedClusterLabel = clusterer->getPredictedClusterLabel();
    
    //Update the context module
    if( contextModules[ AFTER_CLASSIFIER ].size() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ AFTER_CLASSIFIER ].size(); moduleIndex++){
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->process( VectorFloat(1,predictedClusterLabel) ) ){
                errorLog << __FILENAME__ << " Context Module Failed at AFTER_CLASSIFIER. ModuleIndex: " << moduleIndex << std::endl;
                return false;
            }
            if( !contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getOK() ){
                predictionModuleIndex = AFTER_CLASSIFIER;
                return false;
            }
            predictedClusterLabel = (UINT)contextModules[ AFTER_CLASSIFIER ][moduleIndex]->getProcessedData()[0];
        }
    }
    
    //Perform any post processing
    predictionModuleIndex = AFTER_CLASSIFIER;
    if( getIsPostProcessingSet() ){
        
        if( pipelineMode != CLASSIFICATION_MODE){
            errorLog << __FILENAME__ << " Pipeline Mode Is Not in CLASSIFICATION_MODE!" << std::endl;
            return false;
        }
        
        VectorFloat data;
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.size(); moduleIndex++){
            
            //Select which input we should give the postprocessing module
            if( postProcessingModules[moduleIndex]->getIsPostProcessingInputModePredictedClassLabel() ){
                //Set the input
                data.resize(1);
                data[0] = predictedClusterLabel;
                
                //Verify that the input size is OK
                if( data.getSize() != postProcessingModules[moduleIndex]->getNumInputDimensions() ){
                    errorLog << __FILENAME__ << " The size of the data Vector (" << int(data.size()) << ") does not match that of the postProcessingModule (" << postProcessingModules[moduleIndex]->getNumInputDimensions() << ") at the moduleIndex: " << moduleIndex << std::endl;
                    return false;
                }
                
                //Postprocess the data
                if( !postProcessingModules[moduleIndex]->process( data ) ){
                    errorLog << __FILENAME__ << " Failed to post process data. PostProcessing moduleIndex: " << moduleIndex << std::endl;
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
                if( data.getSize() != 1 ){
                    errorLog << __FILENAME__ << " The size of the processed data Vector (" << int(data.getSize()) << ") from postProcessingModule at the moduleIndex: " << moduleIndex << " is not equal to 1 even though it is in OutputModePredictedClassLabel!" << std::endl;
                    return false;
                }
                
                //Update the predicted cluster label
                predictedClusterLabel = (UINT)data[0];
            }
            
        }
    }
    
    //Update the context module
    predictionModuleIndex = END_OF_PIPELINE;
    if( contextModules[ END_OF_PIPELINE ].getSize() ){
        for(UINT moduleIndex=0; moduleIndex<contextModules[ END_OF_PIPELINE ].getSize(); moduleIndex++){
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->process( VectorFloat(1,predictedClassLabel) ) ){
                errorLog << __FILENAME__ << " Context Module Failed at END_OF_PIPELINE. ModuleIndex: " << moduleIndex << std::endl;
                return false;
            }
            if( !contextModules[ END_OF_PIPELINE ][moduleIndex]->getOK() ){
                predictionModuleIndex = END_OF_PIPELINE;
                return false;
            }
            predictedClusterLabel = (UINT)contextModules[ END_OF_PIPELINE ][moduleIndex]->getProcessedData()[0];
        }
    }
    
    return true;
}
    
bool GestureRecognitionPipeline::reset(){
    
    //Reset any pre processing
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.getSize(); moduleIndex++){
            if( !preProcessingModules[ moduleIndex ]->reset() ){
                errorLog << __FILENAME__ << " Failed To Reset PreProcessingModule " << moduleIndex << std::endl;
                return false;
            }
        }
    }
    
    //Reset any feature extraction
    if( getIsFeatureExtractionSet() ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.getSize(); moduleIndex++){
            if( !featureExtractionModules[ moduleIndex ]->reset() ){
                errorLog << __FILENAME__ << " Failed To Reset FeatureExtractionModule " << moduleIndex << std::endl;
                return false;
            }
        }
    }
    
    //Reset the classifier
    if( getIsClassifierSet() ){
        if( !classifier->reset() ){
            errorLog << __FILENAME__ << " Failed To Reset Classifier! " << classifier->getLastErrorMessage() << std::endl;
            return false;
        }
    }
    
    //Reset the regressiier
    if( getIsRegressifierSet() ){
        if( !regressifier->reset() ){
            errorLog << __FILENAME__ << " Failed To Reset Regressifier! " << regressifier->getLastErrorMessage() << std::endl;
            return false;
        }
    }
    
    //Reset the clusterer
    if( getIsClustererSet() ){
        if( !clusterer->reset() ){
            errorLog << __FILENAME__ << " Failed To Reset clusterer! " << clusterer->getLastErrorMessage() << std::endl;
            return false;
        }
    }
    
    //Reset any post processing
    if( getIsPostProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<postProcessingModules.getSize(); moduleIndex++){
            if( !postProcessingModules[ moduleIndex ]->reset() ){
                errorLog << __FILENAME__ << " Failed To Reset PostProcessingModule " << moduleIndex << std::endl;
                return false;
            }
        }
    } 
    
    return true;
}

bool GestureRecognitionPipeline::clearAll(){ return clear(); }

bool GestureRecognitionPipeline::clear(){

    //Clear the entire pipeline
    clearTestResults();
    deleteAllPreProcessingModules();
    deleteAllFeatureExtractionModules();
    deleteClassifier();
    deleteRegressifier();
    deleteClusterer();
    deleteAllPostProcessingModules();
    deleteAllContextModules();

    //Reset the pipeline
    return init();
}

bool GestureRecognitionPipeline::clearModel(){
    
    //Clear any preprocessing module
    for(UINT i=0; i<getNumPreProcessingModules(); i++){
        preProcessingModules[i]->clear();
    }
    
    //Clear any feature extraction module
    for(UINT i=0; i<getNumFeatureExtractionModules(); i++){
        featureExtractionModules[i]->clear();
    }
    
    //Clear any ML model
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
            if( getIsClassifierSet() ){
                classifier->clear();
            }
            break;
        case REGRESSION_MODE:
            if( getIsRegressifierSet() ){
                regressifier->clear();
            }
            break;
        case CLUSTER_MODE:
            if( getIsClustererSet() ){
                clusterer->clear();
            }
            break;
        default:
            break;
    }
    
    //Clear any post processing modules
    for(UINT i=0; i<getNumPostProcessingModules(); i++){
        postProcessingModules[i]->clear();
    }

    return true;
}
    
bool GestureRecognitionPipeline::savePipelineToFile(const std::string &filename) const {
    return save( filename );
}
    
bool GestureRecognitionPipeline::save(const std::string &filename) const {
    
    if( !initialized ){
        errorLog << __FILENAME__ << " Failed to write pipeline to file as the pipeline has not been initialized yet!" << std::endl;
        return false;
    }
    
    std::fstream file;
    
    file.open(filename.c_str(), std::iostream::out );
    
    if( !file.is_open() ){
        errorLog << __FILENAME__ << " Failed to open file with filename: " << filename << std::endl;
        return false;
    }
    
    //Write the pipeline header info
    file << "GRT_PIPELINE_FILE_V3.0\n";
    file << "PipelineMode: " << getPipelineModeAsString() << std::endl;
    file << "NumPreprocessingModules: " << getNumPreProcessingModules() << std::endl;
    file << "NumFeatureExtractionModules: " << getNumFeatureExtractionModules() << std::endl;
    file << "NumPostprocessingModules: " << getNumPostProcessingModules() << std::endl;
    file << "Trained: " << getTrained() << std::endl;
    file << "Info: " << info << std::endl;
    
    //Write the module datatype names
    file << "PreProcessingModuleDatatypes:";
    for(UINT i=0; i<getNumPreProcessingModules(); i++){
        file << "\t" << preProcessingModules[i]->getId();
    }
    file << std::endl;
    
    file << "FeatureExtractionModuleDatatypes:";
    for(UINT i=0; i<getNumFeatureExtractionModules(); i++){
        file << "\t" << featureExtractionModules[i]->getId();
    }
    file << std::endl;
    
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
            if( getIsClassifierSet() ) file << "ClassificationModuleDatatype:\t" << classifier->getId() << std::endl;
            else file << "ClassificationModuleDatatype:\tCLASSIFIER_NOT_SET" << std::endl;
            break;
        case REGRESSION_MODE:
            if( getIsRegressifierSet() ) file << "RegressionModuleDatatype:\t" << regressifier->getId() << std::endl;
            else file << "RegressionModuleDatatype:\tREGRESSIFIER_NOT_SET" << std::endl;
            break;
        case CLUSTER_MODE:
            if( getIsClustererSet() ) file << "ClusterModuleDatatype:\t" << clusterer->getId() << std::endl;
            else file << "ClusterModuleDatatype:\tCLUSTERER_NOT_SET" << std::endl;
            break;
        default:
            break;
    }
    
    file << "PostProcessingModuleDatatypes:";
    for(UINT i=0; i<getNumPostProcessingModules(); i++){
        file << "\t" << postProcessingModules[i]->getId();
    }
    file << std::endl;
    
    //Write the preprocessing module data to the file
    for(UINT i=0; i<getNumPreProcessingModules(); i++){
        file << "PreProcessingModule_" << Util::intToString(i+1) << std::endl;
        if( !preProcessingModules[i]->save( file ) ){
            errorLog << "Failed to write preprocessing module " << i << " settings to file!" << std::endl;
            file.close();
            return false;
        }
    }
    
    //Write the feature extraction module data to the file
    for(UINT i=0; i<getNumFeatureExtractionModules(); i++){
        file << "FeatureExtractionModule_" << Util::intToString(i+1) << std::endl;
        if( !featureExtractionModules[i]->save( file ) ){
            errorLog << "Failed to write feature extraction module " << i << " settings to file!" << std::endl;
            file.close();
            return false;
        }
    }
    
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
            if( getIsClassifierSet() ){
                if( !classifier->save( file ) ){
                    errorLog << "Failed to write classifier model to file!" << std::endl;
                    file.close();
                    return false;
                }
            }
            break;
        case REGRESSION_MODE:
            if( getIsRegressifierSet() ){
                if( !regressifier->save( file ) ){
                    errorLog << "Failed to write regressifier model to file!" << std::endl;
                    file.close();
                    return false;
                }
            }
            break;
        case CLUSTER_MODE:
            if( getIsClustererSet() ){
                if( !clusterer->save( file ) ){
                    errorLog << "Failed to write clusterer model to file!" << std::endl;
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
        file << "PostProcessingModule_" << Util::intToString(i+1) << std::endl;
        if( !postProcessingModules[i]->save( file ) ){
            errorLog <<"Failed to write post processing module " << i << " settings to file!" << std::endl;
            file.close();
            return false;
        }
    }
    
    //Close the file
    file.close();
    
    return true;
}

bool GestureRecognitionPipeline::loadPipelineFromFile(const std::string &filename){
    return load( filename );
}
    
bool GestureRecognitionPipeline::load(const std::string &filename){
    
    std::fstream file;

	//Clear any previous setup
	clear();
    
    file.open(filename.c_str(), std::iostream::in );
    
    if( !file.is_open() ){
        errorLog << __FILENAME__ << " Failed to open file with filename: " << filename << std::endl;
        return false;
    }

	std::string word;
	
	//Load the file header
	file >> word;
	if( word != "GRT_PIPELINE_FILE_V3.0" ){
        errorLog << __FILENAME__ << " Failed to read file header" << std::endl;
		file.close();
        return false;
	}
	
	//Load the pipeline mode
	file >> word;
	if( word != "PipelineMode:" ){
        errorLog << __FILENAME__ << " Failed to read PipelineMode" << std::endl;
		file.close();
        return false;
	}
	file >> word;
	pipelineMode = getPipelineModeFromString(word);
	
	//Load the NumPreprocessingModules
	file >> word;
	if( word != "NumPreprocessingModules:" ){
        errorLog << __FILENAME__ << " Failed to read NumPreprocessingModules header" << std::endl;
		file.close();
        return false;
	}
	unsigned int numPreprocessingModules;
	file >> numPreprocessingModules;
	
	//Load the NumFeatureExtractionModules
	file >> word;
	if( word != "NumFeatureExtractionModules:" ){
        errorLog << __FILENAME__ << " Failed to read NumFeatureExtractionModules header" << std::endl;
		file.close();
        return false;
	}
	unsigned int numFeatureExtractionModules;
	file >> numFeatureExtractionModules;
	
	//Load the NumPostprocessingModules
	file >> word;
	if( word != "NumPostprocessingModules:" ){
        errorLog << __FILENAME__ << " Failed to read NumPostprocessingModules header" << std::endl;
		file.close();
        return false;
	}
	unsigned int numPostprocessingModules;
	file >> numPostprocessingModules;
	
	//Load if the pipeline has been trained
	file >> word;
	if( word != "Trained:" ){
        errorLog << __FILENAME__ << " Failed to read Trained header" << std::endl;
		file.close();
        return false;
	}
	file >> trained;
    
	//Load the info
	file >> word;
	if( word != "Info:" ){
    	errorLog << __FILENAME__ << " Failed to read Info header" << std::endl;
    	file.close();
    	return false;
	}
	info = "";
	//Read the info text
	file >> word;
	while( word != "PreProcessingModuleDatatypes:" ){
        info += word;
        file >> word;
	}
    
	//Resize the modules
	if( numPreprocessingModules > 0 ) preProcessingModules.resize(numPreprocessingModules,NULL);
	if( numFeatureExtractionModules > 0 ) featureExtractionModules.resize(numFeatureExtractionModules,NULL);
	if( numPostprocessingModules > 0 ) postProcessingModules.resize(numPostprocessingModules,NULL);
	
	//Load the preprocessing module datatypes and initialize the modules
	if( word != "PreProcessingModuleDatatypes:" ){
        errorLog << __FILENAME__ << " Failed to read PreProcessingModuleDatatypes" << std::endl;
		file.close();
        return false;
	}
    for(UINT i=0; i<numPreprocessingModules; i++){
		file >> word;
		preProcessingModules[i] = PreProcessing::create( word );
		if( preProcessingModules[i] == NULL ){
            errorLog << __FILENAME__ << " Failed to create preprocessing instance from string: " << word << std::endl;
			file.close();
	        return false;
		}
    }

	//Load the feature extraction module datatypes and initialize the modules
	file >> word;
	if( word != "FeatureExtractionModuleDatatypes:" ){
        errorLog << __FILENAME__ << " Failed to read FeatureExtractionModuleDatatypes" << std::endl;
		file.close();
        return false;
	}
    for(UINT i=0; i<numFeatureExtractionModules; i++){
		file >> word;
		featureExtractionModules[i] = FeatureExtraction::create( word );
		if( featureExtractionModules[i] == NULL ){
            errorLog << __FILENAME__ << " Failed to create feature extraction instance from string: " << word << std::endl;
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
                errorLog << __FILENAME__ << " Failed to read ClassificationModuleDatatype" << std::endl;
				file.close();
		        return false;
			}
			//Load the classifier type
			file >> word;
			
			//Initialize the classifier
			classifier = Classifier::create( word );
			if( classifier == NULL ){
                errorLog << __FILENAME__ << " Failed to create classifier instance from string: " << word << std::endl;
				file.close();
		        return false;
			}
            break;
        case REGRESSION_MODE:
			file >> word;
			if( word != "RegressionModuleDatatype:" ){
                errorLog << __FILENAME__ << " Failed to read RegressionModuleDatatype" << std::endl;
				file.close();
		        return false;
			}
			//Load the regressifier type
			file >> word;
			
			//Initialize the regressifier
			regressifier = Regressifier::create( word );
			if( regressifier == NULL ){
                errorLog << __FILENAME__ << " Failed to create regressifier instance from string: " << word << std::endl;
				file.close();
		        return false;
			}
            break;
        case CLUSTER_MODE:
            file >> word;
			if( word != "ClusterModuleDatatype:" ){
                errorLog << __FILENAME__ << " Failed to read ClusterModuleDatatype" << std::endl;
				file.close();
		        return false;
			}
			//Load the clusterer type
			file >> word;
			
			//Initialize the clusterer
			clusterer = Clusterer::create( word );
			if( clusterer == NULL ){
                errorLog << __FILENAME__ << " Failed to create clusterer instance from string: " << word << std::endl;
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
        errorLog << __FILENAME__ << " Failed to read PostProcessingModuleDatatypes" << std::endl;
		file.close();
	    return false;
	}
	for(UINT i=0; i<numPostprocessingModules; i++){
		file >> word;
		postProcessingModules[i] = PostProcessing::create( word );
	}
	
	//Load the preprocessing module data from the file
    for(UINT i=0; i<numPreprocessingModules; i++){
		//Load the preprocessing module header
		file >> word;
        if( !preProcessingModules[i]->load( file ) ){
            errorLog <<  __FILENAME__ << " Failed to load preprocessing module " << i << " settings from file!" << std::endl;
            file.close();
            return false;
        }
    }

	//Load the feature extraction module data from the file
	for(UINT i=0; i<numFeatureExtractionModules; i++){
		//Load the feature extraction module header
		file >> word;
	    if( !featureExtractionModules[i]->load( file ) ){
            errorLog <<  __FILENAME__ << " Failed to load feature extraction module " << i << " settings from file!" << std::endl;
	        file.close();
	        return false;
	    }
	}
	
	//Load the classifier or regressifer data
	switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
               if( !classifier->load( file ) ){
                   errorLog <<  __FILENAME__ << " Failed to load classifier model from file!" << std::endl;
                   file.close();
                   return false;
               }
            break;
        case REGRESSION_MODE:
               if( !regressifier->load( file ) ){
                   errorLog <<  __FILENAME__ << " Failed to load regressifier model from file!" << std::endl;
                   file.close();
                   return false;
               }
            break;
        case CLUSTER_MODE:
            if( !clusterer->load( file ) ){
                errorLog <<  __FILENAME__ << " Failed to load cluster model from file!" << std::endl;
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
        if( !postProcessingModules[i]->load( file ) ){
            errorLog <<  __FILENAME__ << " Failed to load post processing module " << i << " settings from file!" << std::endl;
            file.close();
            return false;
        }
    }
    
    //Close the file
    file.close();
    
    //Set the expected input Vector size
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
                    inputVectorDimensions = classifier->getNumInputDimensions();
                    break;
                case REGRESSION_MODE:
                    inputVectorDimensions = regressifier->getNumInputDimensions();
                    break;
                case CLUSTER_MODE:
                    inputVectorDimensions = clusterer->getNumInputDimensions();
                    break;
                default:
                    break;
            }
        }
    }

    //Flag that the pipeline is now initialized
    initialized = true;
    
    return true;
}
    
bool GestureRecognitionPipeline::preProcessData(VectorFloat inputVector,bool computeFeatures){
    
    if( getIsPreProcessingSet() ){
        for(UINT moduleIndex=0; moduleIndex<preProcessingModules.getSize(); moduleIndex++){
            
            if( inputVector.getSize() != preProcessingModules[ moduleIndex ]->getNumInputDimensions() ){
                errorLog << __FILENAME__ << " The size of the input Vector (" << preProcessingModules[ moduleIndex ]->getNumInputDimensions() << ") does not match that of the PreProcessing Module at moduleIndex: " << moduleIndex << std::endl;
                return false;
            }
            
            if( !preProcessingModules[ moduleIndex ]->process( inputVector ) ){
                errorLog << __FILENAME__ << " Failed To PreProcess Input Vector. PreProcessing moduleIndex: " << moduleIndex << std::endl;
                return false;
            }
            inputVector = preProcessingModules[ moduleIndex ]->getProcessedData();
        }
    }
    
    //Perform any feature extraction
    if( getIsFeatureExtractionSet() && computeFeatures ){
        for(UINT moduleIndex=0; moduleIndex<featureExtractionModules.getSize(); moduleIndex++){
            if( inputVector.getSize() != featureExtractionModules[ moduleIndex ]->getNumInputDimensions() ){
                errorLog << __FILENAME__ << " The size of the input Vector (" << featureExtractionModules[ moduleIndex ]->getNumInputDimensions() << ") does not match that of the FeatureExtraction Module at moduleIndex: " << moduleIndex << std::endl;
                return false;
            }
            
            if( !featureExtractionModules[ moduleIndex ]->computeFeatures( inputVector ) ){
                errorLog << __FILENAME__ << " Failed To Compute Features from Input Vector. FeatureExtraction moduleIndex: " << moduleIndex << std::endl;
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
    
bool GestureRecognitionPipeline::getIsPreProcessingSet() const{ 
    return preProcessingModules.getSize() > 0; 
} 
    
bool GestureRecognitionPipeline::getIsFeatureExtractionSet() const{ 
    return featureExtractionModules.getSize() > 0; 
}
    
bool GestureRecognitionPipeline::getIsClassifierSet() const{ 
    return (classifier!=NULL); 
}
    
bool GestureRecognitionPipeline::getIsRegressifierSet() const{ 
    return (regressifier!=NULL); 
}
  
bool GestureRecognitionPipeline::getIsClustererSet() const{
    return (clusterer!=NULL);
}
    
bool GestureRecognitionPipeline::getIsPostProcessingSet() const{ 
    return postProcessingModules.size() > 0; 
}
    
bool GestureRecognitionPipeline::getIsContextSet() const{ 
    for(UINT i=0; i<NUM_CONTEXT_LEVELS; i++){
        if( contextModules[i].getSize() > 0 ) return true;
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
    UINT numClasses = 0;
    if( getIsClassifierSet() ){
        numClasses = classifier->getNumClasses();
    }
    if( getIsClustererSet() ){
        numClasses = clusterer->getNumClusters();
    }
    return numClasses;
}
    
UINT GestureRecognitionPipeline::getNumPreProcessingModules() const{ 
    return preProcessingModules.getSize(); 
}
    
UINT GestureRecognitionPipeline::getNumFeatureExtractionModules() const{ 
    return featureExtractionModules.getSize(); 
}
    
UINT GestureRecognitionPipeline::getNumPostProcessingModules() const{ 
    return postProcessingModules.getSize(); 
}
    
UINT GestureRecognitionPipeline::getPredictionModuleIndexPosition() const{ 
    return predictionModuleIndex; 
}
    
UINT GestureRecognitionPipeline::getPredictedClassLabel() const{
    
    if( getIsClassifierSet() ){
        return predictedClassLabel;
    }
    if( getIsClustererSet() ){
        return predictedClusterLabel;
    }
    return 0;
}
    
UINT GestureRecognitionPipeline::getUnProcessedPredictedClassLabel() const{
    if( getIsClassifierSet() ){
        return classifier->getPredictedClassLabel();
    }
    if( getIsClustererSet() ){
        return clusterer->getPredictedClusterLabel();
    }
    return 0;
}
    
UINT GestureRecognitionPipeline::getNumTrainingSamples() const{
    return numTrainingSamples;
}

UINT GestureRecognitionPipeline::getNumTestSamples() const{
    return numTestSamples;
}

Float GestureRecognitionPipeline::getMaximumLikelihood() const{
    if( getIsClassifierSet() ){
        return classifier->getMaximumLikelihood();
    }
    if( getIsClustererSet() ){
        return clusterer->getMaximumLikelihood();
    }
    return 0;
}
 
Float GestureRecognitionPipeline::getPhase() const{
    if( getIsClassifierSet() ){
        return classifier->getPhase();
    }
    return 0;
}

Float GestureRecognitionPipeline::getTrainingSetAccuracy() const{
    if( getIsClassifierSet() ){
        return classifier->getTrainingSetAccuracy();
    }
    return 0;
}
    
Float GestureRecognitionPipeline::getCrossValidationAccuracy() const{ 
    return (getIsClassifierSet()||getIsRegressifierSet() ? testAccuracy : 0);
}
    
Float GestureRecognitionPipeline::getTestAccuracy() const{ 
    return testAccuracy; 
}
    
Float GestureRecognitionPipeline::getTestRMSError() const{ 
    return testRMSError; 
}

Float GestureRecognitionPipeline::getTestSSError() const{
    return testSquaredError;
}
    
Float GestureRecognitionPipeline::getTestFMeasure(const UINT classLabel) const{
    
    if( !getIsClassifierSet() ) return -1;
    if( getClassLabels().size() != testFMeasure.getSize() ) return -1;
    
    for(UINT i=0; i<testFMeasure.getSize(); i++){
        if( getClassLabels()[i] == classLabel ){
            return testFMeasure[i];
        }
    }
    return -1;
}

Float GestureRecognitionPipeline::getTestPrecision(const UINT classLabel) const{
    
    if( !getIsClassifierSet() ) return -1;
    if( getClassLabels().size() != testPrecision.getSize() ) return -1;
    
    for(UINT i=0; i<testPrecision.getSize(); i++){
        if( getClassLabels()[i] == classLabel ){
            return testPrecision[i];
        }
    }
    return -1;
}

Float GestureRecognitionPipeline::getTestRecall(const UINT classLabel) const{
    
    if( !getIsClassifierSet() ) return -1;
    if( getClassLabels().getSize() != testRecall.getSize() ) return -1;
    
    for(UINT i=0; i<testRecall.getSize(); i++){
        if( getClassLabels()[i] == classLabel ){
            return testRecall[i];
        }
    }
    return -1;
}

Float GestureRecognitionPipeline::getTestRejectionPrecision() const{ 
    return testRejectionPrecision; 
}
    
Float GestureRecognitionPipeline::getTestRejectionRecall() const{ 
    return testRejectionRecall; 
}
    
Float GestureRecognitionPipeline::getTestTime() const{
    return testTime;
}

Float GestureRecognitionPipeline::getTrainingTime() const{
    return trainingTime;
}

Float GestureRecognitionPipeline::getTrainingRMSError() const{
    return getIsRegressifierSet() ? regressifier->getRMSTrainingError() : 0;
}

Float GestureRecognitionPipeline::getTrainingSSError() const{
    return getIsRegressifierSet() ? regressifier->getTotalSquaredTrainingError() : 0;
}
    
MatrixFloat GestureRecognitionPipeline::getTestConfusionMatrix() const{ 
    return testConfusionMatrix; 
}
    
Vector< TrainingResult > GestureRecognitionPipeline::getTrainingResults() const{
    if( getIsClassifierSet() ){
        return classifier->getTrainingResults();
    }
    if( getIsRegressifierSet() ){
        return regressifier->getTrainingResults();
    }
    return Vector< TrainingResult >();
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
    
VectorFloat GestureRecognitionPipeline::getTestPrecision() const{
    return testPrecision;
}

VectorFloat GestureRecognitionPipeline::getTestRecall() const{
    return testRecall;
}

VectorFloat GestureRecognitionPipeline::getTestFMeasure() const{
    return testFMeasure;
}

VectorFloat GestureRecognitionPipeline::getClassLikelihoods() const{ 
    if( getIsClassifierSet() ){ return classifier->getClassLikelihoods(); }
    if( getIsClustererSet() ){ return clusterer->getClusterLikelihoods(); }
    else{ return VectorFloat(); } 
}

VectorFloat GestureRecognitionPipeline::getClassDistances() const{ 
    if( getIsClassifierSet() ){ return classifier->getClassDistances(); }
    if( getIsClustererSet() ){ return clusterer->getClusterDistances(); }
    else{ return VectorFloat(); } 
}

VectorFloat GestureRecognitionPipeline::getNullRejectionThresholds() const{
    if( getIsClassifierSet() ){ return classifier->getNullRejectionThresholds(); }
    else{ return VectorFloat(); } 
}

VectorFloat GestureRecognitionPipeline::getRegressionData() const{ 
	return regressionData;
}

VectorFloat GestureRecognitionPipeline::getUnProcessedRegressionData() const{ 
    if( getIsRegressifierSet() ) {
        return regressifier->getRegressionData();
    }
    return VectorFloat();
}
    
VectorFloat GestureRecognitionPipeline::getPreProcessedData() const{
    if( getIsPreProcessingSet() ){ 
        return preProcessingModules[ preProcessingModules.getSize()-1 ]->getProcessedData(); 
    }
    return VectorFloat();
}

VectorFloat GestureRecognitionPipeline::getPreProcessedData(const UINT moduleIndex) const{
    if( getIsPreProcessingSet() ){ 
        if( moduleIndex < preProcessingModules.getSize() ){
            return preProcessingModules[ moduleIndex ]->getProcessedData(); 
        }
    }
    return VectorFloat();
}

VectorFloat GestureRecognitionPipeline::getFeatureExtractionData() const{
    if( getIsFeatureExtractionSet() ){ 
        return featureExtractionModules[ featureExtractionModules.getSize()-1 ]->getFeatureVector(); 
    }
    return VectorFloat();
}
    
VectorFloat GestureRecognitionPipeline::getFeatureExtractionData(const UINT moduleIndex) const{
    if( getIsFeatureExtractionSet() ){ 
        if( moduleIndex < featureExtractionModules.getSize() ){
            return featureExtractionModules[ moduleIndex ]->getFeatureVector(); 
        }
    }
    warningLog << __FILENAME__ << " Failed to get class labels!" << std::endl;
    return VectorFloat();
}
    
Vector< UINT > GestureRecognitionPipeline::getClassLabels() const{ 
    if( getIsClassifierSet() ){
        return classifier->getClassLabels();
    }
    if( getIsClustererSet() ){
        return clusterer->getClusterLabels();
    }
    warningLog << __FILENAME__ << " Failed to get class labels!" << std::endl;
    return Vector< UINT>(); 
}
    
Vector< TestInstanceResult > GestureRecognitionPipeline::getTestInstanceResults() const{
    return testResults;
}

Vector< TestResult > GestureRecognitionPipeline::getCrossValidationResults() const{
    return crossValidationResults;
}
    
PreProcessing* GestureRecognitionPipeline::getPreProcessingModule(const UINT moduleIndex) const{
    if( moduleIndex < preProcessingModules.getSize() ){
        return preProcessingModules[ moduleIndex ];
    }
    warningLog << __FILENAME__ << " Failed to get pre processing module!" << std::endl;
    return NULL;
}
    
FeatureExtraction* GestureRecognitionPipeline::getFeatureExtractionModule(const UINT moduleIndex) const{
    if( moduleIndex < featureExtractionModules.getSize() ){
        return featureExtractionModules[ moduleIndex ];
    }
    warningLog << __FILENAME__ << " Failed to get feature extraction module!" << std::endl;
    return NULL;
}
    
Classifier* GestureRecognitionPipeline::getClassifier() const{
    return classifier; 
}
    
Regressifier* GestureRecognitionPipeline::getRegressifier() const{
    return regressifier; 
}
    
Clusterer* GestureRecognitionPipeline::getClusterer() const{
    return clusterer;
}
    
PostProcessing* GestureRecognitionPipeline::getPostProcessingModule(UINT moduleIndex) const{
    if( moduleIndex < postProcessingModules.getSize() ){
        return postProcessingModules[ moduleIndex ];
    }
    warningLog << __FILENAME__ << "Failed to get post processing module!" << std::endl;
    return NULL;
}
    
Context* GestureRecognitionPipeline::getContextModule(UINT contextLevel,UINT moduleIndex) const{
    if( contextLevel < contextModules.getSize() ){
        if( moduleIndex < contextModules[ contextLevel ].getSize() ){
            return contextModules[ contextLevel ][ moduleIndex ];
        }
    }
    warningLog << __FILENAME__ << " Failed to get context module!" << std::endl;
    return NULL;
}
    
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////                SETTERS                    ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
bool GestureRecognitionPipeline::addPreProcessingModule(const PreProcessing &preProcessingModule,UINT insertIndex){
    
    //Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= preProcessingModules.getSize() ){
        errorLog << __FILENAME__ << "Invalid insertIndex value!" << std::endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    PreProcessing *newInstance = preProcessingModule.create();
    
    //Verify that the clone was successful
    if( !newInstance->deepCopyFrom( &preProcessingModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorLog << __FILENAME__ << " PreProcessing Module Not Set!" << std::endl;
        return false;
    }
    
    //Add the new instance to the preProcessingModules
    Vector< PreProcessing* >::iterator iter = preProcessingModules.begin();
    
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
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= featureExtractionModules.getSize() ){
        errorLog << __FILENAME__ << " Invalid insertIndex value!" << std::endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    FeatureExtraction *newInstance = featureExtractionModule.create();
    
    //Verify that the clone was successful
    if( !newInstance->deepCopyFrom( &featureExtractionModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorLog << __FILENAME__ << " FeatureExtraction Module Not Set!" << std::endl;
        return false;
    }
    
    //Add the new instance to the preProcessingModules
    Vector< FeatureExtraction* >::iterator iter = featureExtractionModules.begin();
    
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
    
    //Delete any previous classifier, regressifier or clusterer
    deleteClassifier();
    deleteRegressifier();
    deleteClusterer();

    //Create a new instance of the classifier and then clone the values across from the reference classifier
    this->classifier = classifier.create( classifier.getId() );
    
    if( this->classifier == NULL ){
        errorLog << __FILENAME__ << " Classifier Module Not Set!" << std::endl;
        return false;
    }
    
    //Deep copy the data from the rhs classifier into this classifier
    if( !this->classifier->deepCopyFrom( &classifier ) ){
        deleteClassifier();
        pipelineMode = PIPELINE_MODE_NOT_SET;
        errorLog << __FILENAME__ << " Classifier Module Not Set!" << std::endl;
        return false;
    }
    
    //Set the mode of the pipeline to classification mode
    pipelineMode = CLASSIFICATION_MODE;
    
    //Flag that the key part of the pipeline has now been initialized
    initialized = true;
    
    //If there is no preprocessing / feature extraction and the classifier is trained, then flag the pipeline is trained
    //Otherwise the user needs to train the pipeline
    if( !getIsPreProcessingSet() && !getIsFeatureExtractionSet() && classifier.getTrained() ){
        inputVectorDimensions = classifier.getNumInputDimensions();
        trained = true;
    }else trained = false;

    return true;
}

bool GestureRecognitionPipeline::setRegressifier(const Regressifier &regressifier){
    
    //Delete any previous classifier, regressifier or clusterer
    deleteClassifier();
    deleteRegressifier();
    deleteClusterer();
    
    //Set the mode of the pipeline to regression mode
    pipelineMode = REGRESSION_MODE;
    
    //Create a new instance of the regressifier and then clone the values across from the reference regressifier
    this->regressifier = regressifier.create( regressifier.getId() );
    
    //Validate that the regressifier was cloned correctly
    if( !this->regressifier->deepCopyFrom( &regressifier ) ){
        deleteRegressifier();
        pipelineMode = PIPELINE_MODE_NOT_SET;
        errorLog << __FILENAME__ << " Regressifier Module Not Set!" << std::endl;
        return false;
    }
    
    //Flag that the key part of the pipeline has now been initialized
    initialized = true;

    //If there is no preprocessing / feature extraction and the regressifier is trained, then flag the pipeline is trained
    //Otherwise the user needs to train the pipeline
    if( !getIsPreProcessingSet() && !getIsFeatureExtractionSet() ){
        trained = regressifier.getTrained();
    }else trained = false;
    
    return true;
}
    
bool GestureRecognitionPipeline::setClusterer(const Clusterer &clusterer){
    
    //Delete any previous classifier, regressifier or clusterer
    deleteClassifier();
    deleteRegressifier();
    deleteClusterer();
    
    //Set the mode of the pipeline to cluster mode
    pipelineMode = CLUSTER_MODE;
    
    //Create a new instance of the clusterer and then clone the values across from the reference clusterer
    this->clusterer = clusterer.create( clusterer.getId() );
    
    //Validate that the regressifier was cloned correctly
    if( !this->clusterer->deepCopyFrom( &clusterer ) ){
        deleteClusterer();
        pipelineMode = PIPELINE_MODE_NOT_SET;
        errorLog << __FILENAME__ << " Clusterer Module Not Set!" << std::endl;
        return false;
    }
    
    //Flag that the key part of the pipeline has now been initialized
    initialized = true;
    
    //If there is no preprocessing / feature extraction and the regressifier is trained, then flag the pipeline is trained
    //Otherwise the user needs to train the pipeline
    if( !getIsPreProcessingSet() && !getIsFeatureExtractionSet() ){
        trained = clusterer.getTrained();
    }else trained = false;
    
    return true;
}

bool GestureRecognitionPipeline::addPostProcessingModule(const PostProcessing &postProcessingModule,UINT insertIndex){
    
    //Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= postProcessingModules.getSize() ){
        errorLog << __FILENAME__ << " Invalid insertIndex value!" << std::endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    PostProcessing *newInstance = postProcessingModule.create( postProcessingModule.getId() );
    
    //Verify that the clone was successful
    if( !newInstance->deepCopyFrom( &postProcessingModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorLog << __FILENAME__ << " PostProcessing Module Not Set!" << std::endl;
        return false;
    }
    
    //Add the new instance to the postProcessingModules
    Vector< PostProcessing* >::iterator iter = postProcessingModules.begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = postProcessingModules.end(); 
    else iter = postProcessingModules.begin() + insertIndex;
    
    postProcessingModules.insert(iter, newInstance);

    //Note, we don't change the trained state of the pipeline for post processing modules, as they are added after the core ML module

    return true;
}

bool GestureRecognitionPipeline::setPostProcessingModule(const PostProcessing &postProcessingModule){
	removeAllPostProcessingModules();
	return addPostProcessingModule( postProcessingModule );
}

bool GestureRecognitionPipeline::addContextModule(const Context &contextModule,UINT contextLevel,UINT insertIndex){
	
	//Validate the contextLevel is valid
    if( contextLevel >= contextModules.getSize() ){
        errorLog << __FILENAME__ << " Invalid contextLevel value!" << std::endl;
        return false;
    }

	//Validate the insertIndex is valid
    if( insertIndex != INSERT_AT_END_INDEX && insertIndex >= contextModules[contextLevel].getSize() ){
        errorLog << __FILENAME__ << " Invalid insertIndex value!" << std::endl;
        return false;
    }
    
    //Create a new instance of the preProcessing and then clone the values across from the reference preProcessing
    Context *newInstance = contextModule.create( contextModule.getId() );
    
    //Verify that the clone was successful
    if( !newInstance->deepCopyFrom( &contextModule ) ){
        delete newInstance;
        newInstance = NULL;
        errorLog << __FILENAME__ << " Context Module Not Set!" << std::endl;
        return false;
    }
    
    //Add the new instance to the contextModules
    Vector< Context* >::iterator iter = contextModules[ contextLevel ].begin();
    
    if( insertIndex == INSERT_AT_END_INDEX ) iter = contextModules[ contextLevel ].end(); 
    else iter = contextModules[ contextLevel ].begin() + insertIndex;
    
    contextModules[ contextLevel ].insert(iter, newInstance);
    
    return true;
}

bool GestureRecognitionPipeline::updateContextModule(bool value,UINT contextLevel,UINT moduleIndex){
    
    //Validate the contextLevel is valid
    if( contextLevel >= contextModules.getSize() ){
        errorLog << __FILENAME__ << " Context Level is out of bounds!" << std::endl;
        return false;
    }
    
    //Validate the moduleIndex is valid
    if( moduleIndex >= contextModules[contextLevel].getSize() ){
        errorLog << __FILENAME__ << " Invalid contextLevel value!"  << std::endl;
        return false;
    }
    
    return contextModules[contextLevel][moduleIndex]->updateContext( value );
}
    
bool GestureRecognitionPipeline::removeAllPreProcessingModules(){
    deleteAllPreProcessingModules();
    return true;
}
    
bool GestureRecognitionPipeline::removePreProcessingModule(UINT moduleIndex){
    if( moduleIndex >= preProcessingModules.getSize() ){
        errorLog << __FILENAME__ << " Invalid moduleIndex " << moduleIndex << ". The size of the preProcessingModules Vector is " << int(preProcessingModules.size()) << std::endl;
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
    if( moduleIndex >= featureExtractionModules.getSize() ){
        errorLog << __FILENAME__ << " Invalid moduleIndex " << moduleIndex << ". The size of the featureExtractionModules Vector is " << int(featureExtractionModules.size()) << std::endl;
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
        errorLog << __FILENAME__ << " Invalid moduleIndex " << moduleIndex << ". The size of the postProcessingModules Vector is " << int(postProcessingModules.size()) << std::endl;
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
        errorLog << __FILENAME__ << " Invalid moduleIndex " << moduleIndex << " is out of bounds!" << std::endl;
        return false;
    }
    
    if( moduleIndex >= contextModules[contextLevel].size() ){
        errorLog << __FILENAME__ << " Invalid moduleIndex " << moduleIndex << ". The size of the contextModules Vector at context level " << " is " << int(contextModules[contextLevel].size()) << std::endl;
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
    
bool GestureRecognitionPipeline::setInfo(const std::string &info){
    this->info = info;
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////          PROTECTED FUNCTIONS              ///////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    
void GestureRecognitionPipeline::deleteAllPreProcessingModules(){
    if( preProcessingModules.getSize() != 0 ){
        for(UINT i=0; i<preProcessingModules.getSize(); i++){
            delete preProcessingModules[i];
            preProcessingModules[i] = NULL;
        }
        preProcessingModules.clear();
        trained = false;
    }
}
    
void GestureRecognitionPipeline::deleteAllFeatureExtractionModules(){
    if( featureExtractionModules.getSize() != 0 ){
        for(UINT i=0; i<featureExtractionModules.getSize(); i++){
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
    
void GestureRecognitionPipeline::deleteClusterer(){
    if( clusterer != NULL ){
        delete clusterer;
        clusterer = NULL;
    }
    trained = false;
    initialized = false;
}
    
void GestureRecognitionPipeline::deleteAllPostProcessingModules(){
    if( postProcessingModules.size() != 0 ){
        for(UINT i=0; i<postProcessingModules.getSize(); i++){
            delete postProcessingModules[i];
            postProcessingModules[i] = NULL;
        }
        postProcessingModules.clear();
        trained = false;
    }
}
    
void GestureRecognitionPipeline::deleteAllContextModules(){
    for(UINT i=0; i<contextModules.getSize(); i++){
        for(UINT j=0; j<contextModules[i].getSize(); j++){
            delete contextModules[i][j];
            contextModules[i][j] = NULL;
        }
        contextModules[i].clear();
    }
}

bool GestureRecognitionPipeline::init(){
    initialized = false;
    trained = false;
    info = "";
    pipelineMode = PIPELINE_MODE_NOT_SET;
    inputVectorDimensions = 0;
    outputVectorDimensions = 0;
    predictedClassLabel = 0;
    predictedClusterLabel = 0;
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
    clusterer = NULL;
    contextModules.resize( NUM_CONTEXT_LEVELS );
    return true;
}
    
bool GestureRecognitionPipeline::updateTestMetrics(const UINT classLabel,const UINT predictedClassLabel,VectorFloat &precisionCounter,VectorFloat &recallCounter,Float &rejectionPrecisionCounter,Float &rejectionRecallCounter,VectorFloat &confusionMatrixCounter){

	const bool nullRejectionEnabled = classifier->getNullRejectionEnabled();

    //Find the index of the classLabel
    UINT predictedClassLabelIndex =0;
    bool predictedClassLabelIndexFound = false;
    const UINT K = getNumClassesInModel();
    for(UINT k=0; k<K; k++){
        if( predictedClassLabel == classifier->getClassLabels()[k] ){
            predictedClassLabelIndex = k;
            predictedClassLabelIndexFound = true;
            break;
        }
    }
        
    if( !predictedClassLabelIndexFound && (nullRejectionEnabled == false || predictedClassLabel != GRT_DEFAULT_NULL_CLASS_LABEL) ){
        errorLog << __FILENAME__ << " Failed to find class label index for label: " << predictedClassLabel << std::endl;
        return false;
    }

    //Find the index of the class label
    UINT actualClassLabelIndex = 0;
    for(UINT k=0; k<K; k++){
        if( classLabel == classifier->getClassLabels()[k] ){
             actualClassLabelIndex = k;
             break;
        }
    }

    //Update the classification accuracy
    if( classLabel == predictedClassLabel ){
        testAccuracy++;
    }

    if( nullRejectionEnabled == false ){

        //Update the precision
        if( classLabel == predictedClassLabel ){
            //Update the precision value
            testPrecision[ predictedClassLabelIndex ]++;
        }
        //Update the precision counter
        precisionCounter[ predictedClassLabelIndex ]++;

        //Update the recall
        if( classLabel == predictedClassLabel ){
            //Update the recall value
            testRecall[ predictedClassLabelIndex ]++;
        }
        //Update the recall counter
        recallCounter[ actualClassLabelIndex ]++;

        //Update the confusion matrix
        testConfusionMatrix[ actualClassLabelIndex  ][ predictedClassLabelIndex ]++;
        confusionMatrixCounter[ actualClassLabelIndex ]++;

    }else{ //Null rejection is enabled
        //Update the precision
        if( predictedClassLabel != GRT_DEFAULT_NULL_CLASS_LABEL ){
            if( classLabel == predictedClassLabel ){
                //Update the precision value
                testPrecision[ predictedClassLabelIndex ]++;
            }
            //Update the precision counter
            precisionCounter[ predictedClassLabelIndex ]++;
        }

        //Update the recall
        if( classLabel != GRT_DEFAULT_NULL_CLASS_LABEL ){
            if( classLabel == predictedClassLabel ){
                //Update the recall value
                testRecall[ predictedClassLabelIndex ]++;
            }
            //Update the recall counter
            recallCounter[ actualClassLabelIndex ]++;
        }

        //Update the rejection precision
        if( predictedClassLabel == GRT_DEFAULT_NULL_CLASS_LABEL ){
            if( classLabel == GRT_DEFAULT_NULL_CLASS_LABEL ) testRejectionPrecision++;
            rejectionPrecisionCounter++;
        }

        //Update the rejection recall
        if( classLabel == GRT_DEFAULT_NULL_CLASS_LABEL ){
            if( predictedClassLabel == GRT_DEFAULT_NULL_CLASS_LABEL ) testRejectionRecall++;
            rejectionRecallCounter++;
        }

        //Update the confusion matrix
        if( classLabel == GRT_DEFAULT_NULL_CLASS_LABEL ) actualClassLabelIndex = 0;
        else actualClassLabelIndex++;
        if( predictedClassLabel == GRT_DEFAULT_NULL_CLASS_LABEL ) predictedClassLabelIndex = 0;
        else predictedClassLabelIndex++;
        testConfusionMatrix[ actualClassLabelIndex  ][ predictedClassLabelIndex ]++;
        confusionMatrixCounter[ actualClassLabelIndex ]++;
    }
    
    return true;
}

bool GestureRecognitionPipeline::computeTestMetrics(VectorFloat &precisionCounter,VectorFloat &recallCounter,Float &rejectionPrecisionCounter,Float &rejectionRecallCounter,VectorFloat &confusionMatrixCounter,const UINT numTestSamples){
        
    //Compute the test metrics
    testAccuracy = testAccuracy/Float(numTestSamples) * 100.0;
    
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
    
    for(UINT r=0; r<confusionMatrixCounter.getSize(); r++){
        if( confusionMatrixCounter[r] > 0 ){
            for(UINT c=0; c<testConfusionMatrix.getNumCols(); c++){
                testConfusionMatrix[r][c] /= confusionMatrixCounter[r];
            }
        }
    }
    
    return true;
}
    
std::string GestureRecognitionPipeline::getModelAsString() const{
    std::string model = "";
    
    switch( pipelineMode ){
        case PIPELINE_MODE_NOT_SET:
            break;
        case CLASSIFICATION_MODE:
            if( getIsClassifierSet() ){
                model += "Classifier: " + classifier->getId() + "\n";
                model += classifier->getModelAsString();
            }
            break;
        case REGRESSION_MODE:
            if( getIsRegressifierSet() ){
                model += "Regressifier: " + regressifier->getId() + "\n";
                model += regressifier->getModelAsString();
            }
            break;
        default:
            break;
    }
    
    return model;
}
    
std::string GestureRecognitionPipeline::getPipelineModeAsString() const{
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
    
std::string GestureRecognitionPipeline::getInfo() const{
    return info;
}

UINT GestureRecognitionPipeline::getPipelineModeFromString(std::string pipelineModeAsString) const{
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

GRT_END_NAMESPACE

