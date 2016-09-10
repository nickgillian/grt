#ifndef GRT_CORE_H
#define GRT_CORE_H

#define GRT_GUI_VERSION "0.1_18"

#include <QObject>
#include "OSC/OSCServer.h"
#include <GRT/GRT.h>
#include "TrainingThread.h"

#define DEFAULT_CORE_THREAD_SLEEP_TIME 10

class Core : public QObject, public GRT::Observer< GRT::TrainingResult >, public GRT::Observer< GRT::TestInstanceResult >
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = 0);

    virtual ~Core();

    bool start();
    bool stop();
    bool addMessaage( const OSCMessagePtr msg );
    bool getCoreRunning();
    bool getTrained();
    bool getTrainingInProcess();
    bool getRecordStatus();
    unsigned int getNumInputDimensions();
    unsigned int getPipelineMode();
    unsigned int getTrainingClassLabel();
    unsigned int getNumTrainingSamples();
    unsigned int getNumTestSamples();
    unsigned int getNumClasses();
    unsigned int getNumClassesInTrainingData();
    vector<unsigned int> getClassLabels();
    GRT::VectorFloat getTargetVector();
    GRT::ClassificationData getClassificationTrainingData();
    GRT::ClassificationData getClassificationTestData();
    GRT::RegressionData getRegressionTrainingData();
    GRT::RegressionData getRegressionTestData();
    GRT::TimeSeriesClassificationData getTimeSeriesClassificationTrainingData();
    GRT::TimeSeriesClassificationData getTimeSeriesClassificationTestData();
    GRT::UnlabelledData getClusterTrainingData();
    GRT::UnlabelledData getClusterTestData();
    double getTestAccuracy();
    double getCrossValidationAccuracy();
    double getTrainingRMSError();
    double getTrainingSSError();
    GRT::GestureRecognitionPipeline getPipeline();
    GRT::TestResult getTestResults();
    vector< GRT::TestResult > getCrossValidationResults();
    std::string getInfoMessage();
    std::string getVersion();
    std::string getIncomingDataAddress();
    std::string getModelAsString();

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
    bool resetOSCServer( const int incomingOSCDataPort );
    bool setVersion( const std::string version );
    bool setEnableOSCInput( const bool state );
    bool setEnableOSCControlCommands( const bool state );
    bool setPipelineMode( const unsigned int pipelineMode );
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

    bool setCoreSleepTime( const unsigned int coreSleepTime );
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
    bool setInfoMessage( const std::string infoMessage );

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
    bool coreRunning;
    bool stopMainThread;
    bool verbose;
    bool debug;
    bool enableOSCInput;
    bool enableOSCControlCommands;

    //OSC Stuff
    unsigned int incomingOSCDataPort;
    unsigned int outgoingOSCDataPort;
    string outgoingOSCAddress;
    string incomingDataAddress;
    OSCServer oscServer;
    std::shared_ptr< UdpTransmitSocket > socket;

    //Data Stuff
    unsigned int coreSleepTime;
    unsigned int numInputDimensions;
    unsigned int targetVectorSize;
    unsigned int trainingClassLabel;
    unsigned int pipelineMode;
    GRT::VectorFloat inputData;
    GRT::VectorFloat targetVector;
    bool newDataReceived;
    bool recordTrainingData;
    bool predictionModeEnabled;
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
    std::string version;

    TrainingThread trainingThread;

public:
    enum PipelineModes{CLASSIFICATION_MODE=0,REGRESSION_MODE,TIMESERIES_CLASSIFICATION_MODE,CLUSTER_MODE};
    
};

#endif // GRT_CORE_H
