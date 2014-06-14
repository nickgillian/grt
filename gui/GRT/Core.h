#ifndef GRT_CORE_H
#define GRT_CORE_H

#define GRT_GUI_VERSION "0.1_12"

#include <QObject>
#include "OSC/OSCServer.h"
#include "../../GRT/GRT.h"
#include "TrainingThread.h"

#define DEFAULT_CORE_THREAD_SLEEP_TIME 10

class Core : public QObject, public GRT::Observer< GRT::TrainingResult >, public GRT::Observer< GRT::TestInstanceResult >
{
    Q_OBJECT
public:
    explicit Core(QObject *parent = 0);

    ~Core();

    bool start();
    bool stop();
    bool getCoreRunning();

    bool getRecordStatus();
    bool getTrained();
    unsigned int getPipelineMode();
    unsigned int getTrainingClassLabel();
    unsigned int getNumTrainingSamples();
    unsigned int getNumTestSamples();
    unsigned int getNumClasses();
    unsigned int getNumClassesInTrainingData();
    vector<unsigned int> getClassLabels();
    GRT::ClassificationData getClassificationTrainingData();
    GRT::ClassificationData getClassificationTestData();
    GRT::RegressionData getRegressionTrainingData();
    GRT::RegressionData getRegressionTestData();
    double getTestAccuracy();
    double getCrossValidationAccuracy();
    double getTrainingRMSError();
    double getTrainingSSError();
    GRT::GestureRecognitionPipeline getPipeline();
    GRT::TestResult getTestResults();
    vector< GRT::TestResult > getCrossValidationResults();
    std::string getInfoMessage();
    std::string getVersion();

signals:
    void coreStarted();
    void coreStopped();
    void newInfoMessage(std::string msg);
    void newWarningMessage(std::string msg);
    void newErrorMessage(std::string msg);
    void newHelpMessage(std::string msg);
    void newOSCMessage(std::string msg);
    void pipelineModeChanged(unsigned int pipelineMode);
    void numInputDimensionsChanged(int newSize);
    void numTargetDimensionsChanged(int newSize);
    void dataChanged(GRT::VectorDouble data);
    void targetDataChanged(GRT::VectorDouble data);
    void trainingClassLabelChanged(unsigned int trainingClassLabel);
    void recordStatusChanged(bool record);
    void pipelineConfigurationReset();
    void pipelineConfigurationChanged();
    void newTrainingResultReceived( const GRT::TrainingResult &data );
    void newTestInstanceResultReceived( const GRT::TestInstanceResult &data );
    void newTrainingSampleAdded(unsigned int numTrainingSamples,GRT::ClassificationSample trainingSample);
    void newTrainingSampleAdded(unsigned int numTrainingSamples,GRT::RegressionSample trainingSample);
    void numTrainingSamplesChanged(unsigned int numTrainingSamples);
    void trainMessageReceived();
    void trainingDataReset(GRT::ClassificationData trainingData);
    void trainingDataReset(GRT::RegressionData regressionData);
    void testDataReset(GRT::ClassificationData testData);
    void testDataReset(GRT::RegressionData regressionData);
    void saveTrainingDataToFileResult(bool result);
    void loadTrainingDataFromFileResult(bool result);
    void loadTestDataFromFileResult(bool result);
    void preProcessingDataChanged(const GRT::VectorDouble &preProcessedData);
    void featureExtractionDataChanged(const GRT::VectorDouble &featureExtractionData);
    void predictionResultsChanged(unsigned int predictedClassLabel,double maximumLikelihood,GRT::VectorDouble classLikelihoods,GRT::VectorDouble classDistances,std::vector<GRT::UINT> classLabels);
    void regressionResultsChanged(GRT::VectorDouble regressionData);
    void pipelineTrainingStarted();
    void pipelineTrainingFinished(bool result);
    void pipelineTestingFinished(bool result);
    void setClassifierMessageReceived(unsigned int classifierType,bool useScaling,bool useNullRejection,double nullRejectionCoeff,double parameter1);
    
public slots:
    bool resetOSCClient(std::string clientAddress,int clientPort);
    bool resetOSCServer(int incomingOSCDataPort);
    bool setVersion(std::string version);
    bool setEnableOSCInput(bool state);
    bool setEnableOSCControlCommands(bool state);
    bool setPipelineMode(unsigned int pipelineMode);
    bool setRecordingState(bool state);
    bool saveTrainingDatasetToFile(std::string filename);
    bool loadTrainingDatasetFromFile(std::string filename);
    bool loadTestDatasetFromFile(std::string filename);
    void clearTrainingData();
    bool train();
    bool trainAndTestOnRandomSubset(unsigned int randomTestSubsetPercentage);
    bool trainAndTestOnTestDataset();
    bool trainWithCrossValidation(unsigned int numFolds);
    bool enablePrediction(bool);

    bool setNumInputDimensions(int numInputDimensions);
    bool setTargetVectorSize(int targetVectorSize);
    bool setTrainingClassLabel(int trainingClassLabel);
    bool setMainDataAddress(std::string address);
    bool setDatasetName(std::string datasetName);
    bool setDatasetInfoText(std::string datasetName);
    bool setPreProcessing( const GRT::PreProcessing &preProcessing );
    bool setFeatureExtraction( const GRT::FeatureExtraction &featureExtraction );
    bool setClassifier( const GRT::Classifier &classifier );
    bool setRegressifier( const GRT::Regressifier &regressifier );
    bool setPostProcessing( const GRT::PostProcessing &postProcessing );
    bool setPipeline( const GRT::GestureRecognitionPipeline &pipeline );
    bool removeAllPreProcessingModules();
    bool removeAllPostProcessingModules();
    bool savePipelineToFile(std::string filename);
    bool loadPipelineFromFile(std::string filename);
    bool setInfoMessage(std::string infoMessage);

protected:
    virtual void notify( const GRT::TrainingResult &data );
    virtual void notify( const GRT::TestInstanceResult &data );
    void mainThreadFunction();
    bool processOSCMessage( const OSCMessage &m );
    bool processNewData();
    void sendPreProcessedData( const GRT::VectorDouble &preProcessedData );
    void sendFeatureExtractionData( const GRT::VectorDouble &featureData );
    void sendPredictionResults( unsigned int predictedClassLabel, double maximumLikelihood );
    void sendClassLikelihoods( const GRT::VectorDouble &classLikelihoods );
    void sendClassDistances( const GRT::VectorDouble &classDistances );
    void sendClassLabels( const vector< unsigned int > &classLabels );
    void sendRegressionData( const GRT::VectorDouble &regressionData );
    void sendStatusMessage(int pipelineMode,int trained,int recording,int numTrainingSamples,int numClassesInTrainingData,std::string infoMessage,std::string version);

    //Core Stuff
    boost::mutex mutex;
    boost::shared_ptr< boost::thread > mainThread;
    bool coreRunning;
    bool stopMainThread;
    bool verbose;
    bool enableOSCInput;
    bool enableOSCControlCommands;

    //OSC Stuff
    unsigned int incomingOSCDataPort;
    unsigned int outgoingOSCDataPort;
    string outgoingOSCAddress;
    string incomingDataAddress;
    OSCServer oscServer;
    boost::shared_ptr< UdpTransmitSocket > socket;

    //Data Stuff
    unsigned int numInputDimensions;
    unsigned int targetVectorSize;
    unsigned int trainingClassLabel;
    unsigned int pipelineMode;
    GRT::VectorDouble inputData;
    GRT::VectorDouble targetVector;
    bool newDataReceived;
    bool recordTrainingData;
    bool predictionModeEnabled;
    GRT::ClassificationData classificationTrainingData;
    GRT::ClassificationData classificationTestData;
    GRT::RegressionData regressionTrainingData;
    GRT::RegressionData regressionTestData;
    GRT::TimeSeriesClassificationData timeseriesClassificationTrainingData;
    GRT::TimeSeriesClassificationData timeseriesClassificationTestData;
    GRT::GestureRecognitionPipeline pipeline;
    GRT::WarningLog warningLog;
    GRT::ErrorLog errorLog;
    std::string infoMessage;
    std::string version;

    TrainingThread trainingThread;

public:
    enum PipelineModes{CLASSIFICATION_MODE=0,REGRESSION_MODE,TIMESERIES_CLASSIFICATION_MODE};
    
};

#endif // GRT_CORE_H
