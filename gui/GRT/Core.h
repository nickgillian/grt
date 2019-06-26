#ifndef GRT_CORE_H
#define GRT_CORE_H

#define GRT_GUI_VERSION "0.1_18"

#include <GRT/GRT.h>

#include "OSC/OSCServer.h"
#include "TrainingThread.h"

//#include "OSC/OSCMessage.h"

#include <vector>
#include <QObject>

#define DEFAULT_CORE_THREAD_SLEEP_TIME 10

class Core : public QObject, public GRT::Observer< GRT::TrainingResult >, public GRT::Observer< GRT::TestInstanceResult >
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = 0);

    virtual ~Core();

    bool start();
    bool stop();
    void addMessaage( const OSCMessagePtr msg );
    bool getCoreRunning() const  { return coreRunning; }
    bool getTrained() const { return pipeline.getTrained(); }
    bool getTrainingInProcess() const { return trainingThread.getTrainingInProcess(); }
    bool getRecordStatus() const { return recordTrainingData; }
    unsigned int getNumInputDimensions() const { return numInputDimensions; }
    unsigned int getPipelineMode() const { return pipelineMode; }
    unsigned int getTrainingClassLabel() const { return trainingClassLabel; }
    unsigned int getNumTrainingSamples();
    unsigned int getNumTestSamples();
    unsigned int getNumClasses() const { return pipeline.getNumClasses(); }
    unsigned int getNumClassesInTrainingData();
    std::vector<unsigned int> getClassLabels() const { return pipeline.getClassLabels(); }
    GRT::VectorFloat getTargetVector() const { return targetVector; }
    GRT::ClassificationData getClassificationTrainingData() const { return classificationTrainingData; }
    GRT::ClassificationData getClassificationTestData() const { return classificationTestData; }
    GRT::RegressionData getRegressionTrainingData() const { return regressionTrainingData;}
    GRT::RegressionData getRegressionTestData() const { return regressionTestData; }
    GRT::TimeSeriesClassificationData getTimeSeriesClassificationTrainingData() const { return timeseriesClassificationTrainingData; }
    GRT::TimeSeriesClassificationData getTimeSeriesClassificationTestData() const { return timeseriesClassificationTestData; }
    GRT::UnlabelledData getClusterTrainingData() const { return clusterTrainingData; }
    GRT::UnlabelledData getClusterTestData() const { return clusterTestData; }
    double getTestAccuracy() const { return pipeline.getTestAccuracy(); }
    double getCrossValidationAccuracy() const { return pipeline.getCrossValidationAccuracy(); }
    double getTrainingRMSError() const { return pipeline.getTrainingRMSError(); }
    double getTrainingSSError() const { return pipeline.getTrainingSSError(); }
    GRT::GestureRecognitionPipeline getPipeline() const { return pipeline; }
    GRT::TestResult getTestResults() const { return pipeline.getTestResults(); }
    vector< GRT::TestResult > getCrossValidationResults() const { return pipeline.getCrossValidationResults(); }
    std::string getInfoMessage() const { return infoMessage; }
    std::string getVersion() const { return version; }
    std::string getIncomingDataAddress() const { return incomingDataAddress; }
    std::string getModelAsString() const { return pipeline.getModelAsString(); }

signals:
    void coreStarted();
    void coreStopped();
    void tick();
    void newInfoMessage( const std::string msg );
    void newWarningMessage( const std::string msg );
    void newErrorMessage( const std::string msg );
    void newHelpMessage( const std::string msg );
    void newOSCMessage( const std::string msg );
    void pipelineModeChanged( const unsigned int pipelineMode );
    void numInputDimensionsChanged( const int newSize );
    void numTargetDimensionsChanged( const int newSize );
    void dataChanged( const GRT::VectorFloat &data );
    void targetDataChanged( const GRT::VectorFloat &data );
    void trainingClassLabelChanged( const unsigned int trainingClassLabel );
    void recordStatusChanged( const bool record );
    void pipelineConfigurationReset();
    void pipelineConfigurationChanged();
    void newTrainingResultReceived( const GRT::TrainingResult &data );
    void newTestInstanceResultReceived( const GRT::TestInstanceResult &data );
    void newTrainingSampleAdded( const unsigned int numTrainingSamples,const GRT::ClassificationSample &trainingSample );
    void newTrainingSampleAdded( const unsigned int numTrainingSamples,const GRT::RegressionSample &trainingSample );
    void newTrainingSampleAdded( const unsigned int numTrainingSamples,const GRT::TimeSeriesClassificationSample &trainingSample );
    void newTrainingSampleAdded( const GRT::MatrixFloat &trainingSample );
    void newTrainingSampleAdded( const GRT::VectorFloat &trainingSample );
    void numTrainingSamplesChanged( const unsigned int numTrainingSamples );
    void trainMessageReceived();
    void trainingDataReset( const GRT::ClassificationData &trainingData );
    void trainingDataReset( const GRT::RegressionData &trainingData );
    void trainingDataReset( const GRT::TimeSeriesClassificationData &trainingData );
    void trainingDataReset( const GRT::UnlabelledData &trainingData );
    void testDataReset( const GRT::ClassificationData &testData );
    void testDataReset( const GRT::RegressionData &testData );
    void testDataReset( const GRT::TimeSeriesClassificationData &testData );
    void testDataReset( const GRT::UnlabelledData &testData );
    void saveTrainingDataToFileResult( const bool result );
    void loadTrainingDataFromFileResult( const bool result );
    void loadTestDataFromFileResult( const bool result );
    void preProcessingDataChanged( const GRT::VectorFloat &preProcessedData );
    void featureExtractionDataChanged( const GRT::VectorFloat &featureExtractionData );
    void classificationResultsChanged( const unsigned int predictedClassLabel,const double maximumLikelihood,const GRT::VectorFloat &classLikelihoods,const GRT::VectorFloat &classDistances,const std::vector<unsigned int> &classLabels );
    void regressionResultsChanged( const GRT::VectorFloat &regressionData );
    void timeseriesClassificationResultsChanged( const unsigned int predictedClassLabel,const double maximumLikelihood,const double phase,const GRT::VectorFloat &classLikelihoods,const GRT::VectorFloat &classDistances,const std::vector<unsigned int> &classLabels );
    void clusterResultsChanged( const unsigned int predictedClusterLabel,const double maximumLikelihood,const GRT::VectorFloat &clusterLikelihoods,const GRT::VectorFloat &clusterDistances,const std::vector<unsigned int> &clusterLabels );
    void pipelineTrainingStarted();
    void pipelineTrainingFinished( const bool result );
    void pipelineTestingFinished( const bool result );
    void setClassifierMessageReceived( const unsigned int classifierType,const bool useScaling,const bool useNullRejection,const double nullRejectionCoeff,const double parameter1 );
    
public slots:
    bool resetOSCClient( const std::string clientAddress,const int clientPort );
    bool resetOSCServer( const int in_OSCDataPort );
    void setVersion( const std::string version );
    void setEnableOSCInput( const bool state );
    void setEnableOSCControlCommands( const bool state );
    void setPipelineMode( const unsigned int pipelineMode );
    bool setRecordingState( const bool state );
    bool saveTrainingDatasetToFile( const std::string filename );
    bool loadTrainingDatasetFromFile( const std::string filename );
    bool loadTestDatasetFromFile( const std::string filename );
    void clearTrainingData();
    bool train();
    bool trainAndTestOnRandomSubset( const unsigned int randomTestSubsetPercentage,const bool useStratifiedSampling );
    bool trainAndTestOnTestDataset();
    bool trainWithCrossValidation( const unsigned int numFolds );
    bool enablePrediction( const bool enable );

    void setCoreSleepTime( const unsigned int cST ) { coreSleepTime = cST; }
    bool setNumInputDimensions( const int numInputDimensions );
    bool setTargetVectorSize( const int targetVectorSize );
    bool setTrainingClassLabel( const int trainingClassLabel );
    bool setMainDataAddress( const std::string address );
    bool setDatasetName( const std::string datasetName );
    bool setDatasetInfoText( const std::string datasetName );
    bool setPreProcessing( const GRT::PreProcessing &preProcessing );
    bool setFeatureExtraction( const GRT::FeatureExtraction &featureExtraction );
    bool setClassifier( const GRT::Classifier &classifier );
    bool setRegressifier( const GRT::Regressifier &regressifier );
    bool setClusterer( const GRT::Clusterer &clusterer );
    bool setPostProcessing( const GRT::PostProcessing &postProcessing );
    bool setPipeline( const GRT::GestureRecognitionPipeline &pipeline );
    bool setTargetVector( const GRT::VectorFloat &targetVector );
    bool removeAllPreProcessingModules();
    bool removeAllPostProcessingModules();
    bool savePipelineToFile( const std::string filename );
    bool loadPipelineFromFile( const std::string filename );
    void setInfoMessage( const std::string iM ) { infoMessage = iM; }

protected:
    virtual void notify( const GRT::TrainingResult &data );
    virtual void notify( const GRT::TestInstanceResult &data );
    void mainThreadFunction();
    bool processOSCMessage( const OSCMessagePtr m );
    bool processNewData();
    void sendPreProcessedData( const GRT::VectorFloat &preProcessedData );
    void sendFeatureExtractionData( const GRT::VectorFloat &featureData );
    void sendPredictionResults( unsigned int predictedClassLabel, double maximumLikelihood );
    void sendClassLikelihoods( const GRT::VectorFloat &classLikelihoods );
    void sendClassDistances( const GRT::VectorFloat &classDistances );
    void sendClassLabels( const vector< unsigned int > &classLabels );
    void sendRegressionData( const GRT::VectorFloat &regressionData );
    void sendStatusMessage( const int pipelineMode,const int trained,const int recording,const int numTrainingSamples,const int numClassesInTrainingData,const std::string infoMessage,const std::string version );

    //Core Stuff
    std::mutex mutex;
    std::shared_ptr< std::thread > mainThread;
    bool coreRunning = false;
    bool stopMainThread = false;
    bool verbose = true;
    bool debug = true;
    bool enableOSCInput = true;
    bool enableOSCControlCommands = true;

    //OSC Stuff
    unsigned int incomingOSCDataPort = 5000;
    unsigned int outgoingOSCDataPort = 5001;
    string outgoingOSCAddress = "127.0.0.1";
    string incomingDataAddress = "/Data";
    OSCServer oscServer;
    std::shared_ptr< UdpTransmitSocket > socket;

    //Data Stuff
    unsigned int coreSleepTime = DEFAULT_CORE_THREAD_SLEEP_TIME;
    unsigned int numInputDimensions = 1;
    unsigned int targetVectorSize = 1;
    unsigned int trainingClassLabel = 1;
    unsigned int pipelineMode = CLASSIFICATION_MODE;
    GRT::VectorFloat inputData;
    GRT::VectorFloat targetVector;
    bool newDataReceived = false;
    bool recordTrainingData = false;
    bool predictionModeEnabled = true;
    GRT::ClassificationData classificationTrainingData;
    GRT::ClassificationData classificationTestData;
    GRT::RegressionData regressionTrainingData;
    GRT::RegressionData regressionTestData;
    GRT::TimeSeriesClassificationData timeseriesClassificationTrainingData;
    GRT::TimeSeriesClassificationData timeseriesClassificationTestData;
    GRT::MatrixFloat timeseriesSample;
    GRT::UnlabelledData clusterTrainingData;
    GRT::UnlabelledData clusterTestData;
    GRT::GestureRecognitionPipeline pipeline;
    GRT::WarningLog warningLog;
    GRT::ErrorLog errorLog;
    std::string infoMessage;
    std::string version = GRT_GUI_VERSION;

    TrainingThread trainingThread;

public:
    enum PipelineModes{CLASSIFICATION_MODE=0,REGRESSION_MODE,TIMESERIES_CLASSIFICATION_MODE,CLUSTER_MODE};
    
};

#endif // GRT_CORE_H
