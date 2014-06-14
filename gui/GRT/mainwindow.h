#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>
#include <QApplication>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QComboBox>
#include <QListView>
#include "Core.h"
#include "timeseriesgraph.h"
#include "bargraph.h"
#include "versioninfo.h"

#define NO_PRE_PROCESSING_SELECTED 0
#define MOVING_AVERAGE_FILTER_PRE_PROCESSING 1
#define DOUBLE_MOVING_AVERAGE_FILTER_PRE_PROCESSING 2
#define LOW_PASS_FILTER_PRE_PROCESSING 3
#define HIGH_PASS_FILTER_PRE_PROCESSING 4
#define DERIVATIVE_PRE_PROCESSING 5
#define DEAD_ZONE_PRE_PROCESSING 6

#define NO_FEATURE_EXTRACTION_SELECTED 0

#define CLASSIFIER_ANBC 0
#define CLASSIFIER_ADABOOST 1
#define CLASSIFIER_DECISION_TREE 2
#define CLASSIFIER_GMM 3
#define CLASSIFIER_KNN 4
#define CLASSIFIER_MINDIST 5
#define CLASSIFIER_RANDOM_FORESTS 6
#define CLASSIFIER_SOFTMAX 7
#define CLASSIFIER_SVM 8

#define NO_POST_POST_PROCESSING 0
#define CLASS_LABEL_FILTER_POST_PROCESSING 1
#define CLASS_LABEL_CHANGE_FILTER_POST_PROCESSING 2
#define CLASS_LABEL_TIMEOUT_FILTER_POST_PROCESSING 3

#define CLASSIFICATION_VIEW 0
#define REGRESSION_VIEW 1

#define DEFAULT_GRAPH_WIDTH 100

#define ALL_LOGS_VIEW 0
#define INFO_LOG_VIEW 1
#define WARNING_LOG_VIEW 2
#define ERROR_LOG_VIEW 3

namespace Ui {
    class MainWindow;
}

Q_DECLARE_METATYPE(std::string);
Q_DECLARE_METATYPE(GRT::VectorDouble);
Q_DECLARE_METATYPE(std::vector<GRT::UINT>);
Q_DECLARE_METATYPE(GRT::ClassificationData);
Q_DECLARE_METATYPE(GRT::ClassificationSample);
Q_DECLARE_METATYPE(GRT::RegressionData);
Q_DECLARE_METATYPE(GRT::RegressionSample);
Q_DECLARE_METATYPE(GRT::TrainingResult);
Q_DECLARE_METATYPE(GRT::TestInstanceResult);
Q_DECLARE_METATYPE(GRT::GestureRecognitionPipeline);
Q_DECLARE_METATYPE(GRT::MLBase);

class MainWindow : public QMainWindow,
                   public GRT::Observer< GRT::TrainingLogMessage >,
                   public GRT::Observer< GRT::TestingLogMessage >,
                   public GRT::Observer< GRT::WarningLogMessage >,
                   public GRT::Observer< GRT::ErrorLogMessage >,
                   public GRT::Observer< GRT::InfoLogMessage >
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    unsigned int getCurrentView() const{ return currentView; }

private slots:
    ////////////////////////////////// MAIN VIEW FUNCTIONS ////////////////////////////////
    void showVersionInfo();
    void showSetupView();
    void showDataIOView();
    void showDataLabellingToolView();
    void showPipelineToolView();
    void showTrainingToolView();
    void showPredictionView();
    void showLogView();
    void updateInfoText(std::string msg);
    void updateWarningText(std::string msg);
    void updateErrorText(std::string msg);
    void updateHelpText(std::string msg);

    /////////////////////////////////// SETUP VIEW FUNCTIONS /////////////////////////////////
    void showSetupViewInfo();
    void setNumInputs(int numInputs);
    void setNumOutputs(int numOutputs);
    void setPipelineModeAsClassificationMode();
    void setPipelineModeAsRegressionMode();
    void setPipelineModeAsTimeseriesMode();
    void updatePipelineMode(unsigned int);
    void resetAll();

    ////////////////////////////////// DATA IO TOOL FUNCTIONS ////////////////////////////////
    void showDataIOInfo();
    void updateOSCInput();
    void updateOSCControlCommands();
    void resetOSCServer( int port );
    void resetOSCClient();
    void updateDataAddress();
    void updateOSCMessageLog(std::string msg);
    void updateNumInputDimensions(int numInputDimensions);
    void updateNumTargetDimensions(int numTargetDimensions);

    //////////////////////////// DATA LABELING TOOL FUNCTIONS ///////////////////////////////
    void showDataLabellingToolInfo();
    void recordTrainingData(bool state);
    void saveTrainingDatasetToFile();
    void loadTrainingDatasetFromFile();
    void updateTrainingClassLabel(unsigned int trainingClassLabel);
    void updateRecordStatus(bool recordStatus);
    void updateNumTrainingSamples(unsigned int numTrainingSamples);
    void addNewTrainingSample(unsigned int numTrainingSamples,GRT::ClassificationSample trainingSample);
    void resetTrainingData(GRT::ClassificationData trainingData);
    void resetTrainingData(GRT::RegressionData trainingData);
    void resetTestData(GRT::ClassificationData testData);
    void handleDatasetClicked(const QModelIndex &index);
    void updateDatasetName();
    void updateDatasetInfoText();
    void updateTrainingTabView(int tabIndex);
    void updateDatasetStatsView();
    void updateClassStatsGraph();
    void updatePCAProjectionGraph();

    //////////////////////////////// PIPELINE TOOl FUNCTIONS ////////////////////////////////
    void showPipelineToolInfo();
    void updatePipelineFilename();
    void savePipelineToFile();
    void loadPipelineFromFile();
    void updatePreProcessingView(int viewIndex);
    void updateFeatureExtractionView(int viewIndex);
    void updateClassifierView(int viewIndex);
    void updateRegressifierView(int viewIndex);
    void updatePostProcessingView(int viewIndex);
    void refreshPipelineSetup();
    void updatePreProcessingSettings();
    void clearPipelineConfiguration();
    void updatePipelineConfiguration();
    void resetPipelineConfiguration();
    void resetDefaultPipelineClassificationSetup();
    void resetDefaultPipelineRegressionSetup();
    void setupDefaultClassifier();
    void setupDefaultRegressifier();
    void updateClassifier(unsigned int classifierType,bool useScaling,bool useNullRejection,double nullRejectionCoeff,double parameter1);

    //////////////////////////////// TRAINING TOOl FUNCTIONS ////////////////////////////////
    void showTrainingToolInfo();
    void loadTestDatasetFromFile();
    void resetTrainingToolView(int trainingMode);
    void train();
    void randomTestSliderMoved(int value);
    void numCVFoldsValueChanged(int value);
    void pipelineTrainingStarted();
    void pipelineTrainingFinished(bool result);
    void pipelineTestingFinished(bool result);
    void updateTrainingResults(const GRT::TrainingResult &trainingResult);
    void updateTestResults(const GRT::TestInstanceResult&);

    /////////////////////////////// PREDICTION VIEW FUNCTIONS ///////////////////////////////
    void showPredictionToolInfo();
    void updatePreProcessingData(const GRT::VectorDouble &preProcessedData);
    void updateFeatureExtractionData(const GRT::VectorDouble &featureExtractionData);
    void updatePredictionResults(unsigned int predictedClassLabel,double maximumLikelihood,GRT::VectorDouble classLikelihoods,GRT::VectorDouble classDistances,std::vector<GRT::UINT> classLabels);
    void updateRegressionResults(GRT::VectorDouble regressionData);
    void resetPredictionViewGraphs();

    ///////////////////////////////    LOG VIEW FUNCTIONS     ///////////////////////////////
    void showLogViewInfo();
    void showAllLogs();
    void showInfoLog();
    void showWarningLog();
    void showErrorLog();
    void updateLogView(unsigned int viewID);

    /////////////////////////////////// CORE DATA FUNCTIONS /////////////////////////////////
    void updateData(GRT::VectorDouble data);
    void updateTargetVector(GRT::VectorDouble targetVector);

private:
    bool initMainMenu();
    bool initSetupView();
    bool initDataIOView();
    bool initDataLabellingToolView();
    bool initPipelineToolView();
    bool initTrainingToolView();
    bool initPreditionView();
    bool initLogView();
    bool initSignalsAndSlots();
    virtual void notify(const GRT::TrainingLogMessage &log);
    virtual void notify(const GRT::TestingLogMessage &log);
    virtual void notify(const GRT::WarningLogMessage &log);
    virtual void notify(const GRT::ErrorLogMessage &log);
    virtual void notify(const GRT::InfoLogMessage &log);

    Ui::MainWindow *ui;
    QStandardItemModel *model;
    Core core;
    static unsigned int numInstances;
    unsigned int currentView;
    vector< QWidget* > tabHistory;
    TimeseriesGraph *inputDataGraph;
    TimeseriesGraph *preProcessedDataGraph;
    TimeseriesGraph *classPredictionsGraph;
    TimeseriesGraph *featureExtractionDataGraph;
    TimeseriesGraph *classLikelihoodsGraph;
    TimeseriesGraph *classDistancesGraph;
    TimeseriesGraph *regressionGraph;
    GRT::ErrorLog errorLog;
    GRT::WarningLog warningLog;

    enum Views{SETUP_VIEW=0,DATA_IO_VIEW,DATA_LABELING_VIEW,PIPELINE_VIEW,TRAINING_TOOL_VIEW,PREDICTION_VIEW,LOG_VIEW};

};

#endif // MAINWINDOW_H
