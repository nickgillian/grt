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
#include <QMouseEvent>
#include <QShortcut>
#include "Core.h"
#include "timeseriesgraph.h"
#include "bargraph.h"
#include "featureplot.h"
#include "versioninfo.h"

enum Views{
    SETUP_VIEW=0,
    DATA_IO_VIEW,
    DATA_MANAGER_VIEW,
    PIPELINE_VIEW,
    TRAINING_VIEW,
    PREDICTION_VIEW,
    LOG_VIEW,
    SETTINGS_VIEW,
    HELP_VIEW
};

enum PreProcessingOptions{
    NO_PRE_PROCESSING_SELECTED=0,
    MOVING_AVERAGE_FILTER_PRE_PROCESSING,
    DOUBLE_MOVING_AVERAGE_FILTER_PRE_PROCESSING,
    LEAKY_INTEGRATOR_PRE_PROCESSING,
    LOW_PASS_FILTER_PRE_PROCESSING,
    HIGH_PASS_FILTER_PRE_PROCESSING,
    DERIVATIVE_PRE_PROCESSING,
    DEAD_ZONE_PRE_PROCESSING
};

enum FeatureExtractionOptions{
    NO_FEATURE_EXTRACTION_SELECTED=0
};

enum ClassifierOptions{
    CLASSIFIER_ANBC=0,
    CLASSIFIER_ADABOOST,
    CLASSIFIER_DECISION_TREE,
    CLASSIFIER_GMM,
    CLASSIFIER_KNN,
    CLASSIFIER_MINDIST,
    CLASSIFIER_RANDOM_FORESTS,
    CLASSIFIER_SOFTMAX,
    CLASSIFIER_SVM,
    CLASSIFIER_SWIPE_DETECTOR
};

enum RegressionOptions{
    REGRESSIFIER_LINEAR = 0,
    REGRESSIFIER_LOGISTIC,
    REGRESSIFIER_MLP
};

enum TimeseriesClassifierOptions{
    TIMESERIES_CLASSIFIER_DTW=0,
    TIMESERIES_CLASSIFIER_HMM_DISCRETE,
    TIMESERIES_CLASSIFIER_HMM_CONTINUOUS,
    TIMESERIES_PARTICLE_CLASSIFIER,
    TIMESERIES_CLASSIFIER_FSM
};

enum ClusterOptions{
    CLUSTER_KMEANS=0,
    CLUSTER_GAUSSIAN_MIXTURE_MODEL,
    CLUSTER_HIERARCHICAL_CLUSTERING,
    CLUSTER_CLUSTER_TREE
};

enum PostProcessingOptions{
    NO_POST_POST_PROCESSING = 0,
    CLASS_LABEL_FILTER_POST_PROCESSING,
    CLASS_LABEL_CHANGE_FILTER_POST_PROCESSING,
    CLASS_LABEL_TIMEOUT_FILTER_POST_PROCESSING
};

enum DecisionTreeNodeOptions{
    DECISION_TREE_CLUSTER_NODE=0,
    DECISION_TREE_THRESHOLD_NODE
};

enum PipelineViewModes{
    CLASSIFICATION_VIEW=0,
    REGRESSION_VIEW,
    TIMESERIES_CLASSIFICATION_VIEW,
    CLUSTER_VIEW
};

enum LogViewModes{
    ALL_LOGS_VIEW=0,
    INFO_LOG_VIEW,
    WARNING_LOG_VIEW,
    ERROR_LOG_VIEW
};

enum TrainingModes{
    TRAINING_MODE_NO_VALIDATION=0,
    TRAINING_MODE_RANDOM_SUBSET,
    TRAINING_MODE_EXTERNAL_TEST_DATASET,
    TRAINING_MODE_CROSS_VALIDATION
};

#define DEFAULT_GRAPH_WIDTH 100
#define FEATURE_PLOT_DEFAULT_WIDTH 600
#define FEATURE_PLOT_DEFAULT_HEIGHT 600

namespace Ui {
    class MainWindow;
}

Q_DECLARE_METATYPE(std::string);
Q_DECLARE_METATYPE(GRT::VectorFloat);
Q_DECLARE_METATYPE(std::vector<unsigned int>);
Q_DECLARE_METATYPE(GRT::ClassificationData);
Q_DECLARE_METATYPE(GRT::ClassificationSample);
Q_DECLARE_METATYPE(GRT::RegressionData);
Q_DECLARE_METATYPE(GRT::RegressionSample);
Q_DECLARE_METATYPE(GRT::TrainingResult);
Q_DECLARE_METATYPE(GRT::TestInstanceResult);
Q_DECLARE_METATYPE(GRT::GestureRecognitionPipeline);
Q_DECLARE_METATYPE(GRT::MLBase);
Q_DECLARE_METATYPE(GRT::TimeSeriesClassificationSample);

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
    virtual ~MainWindow();

    unsigned int getCurrentView() const;

private slots:
    ////////////////////////////////// MAIN VIEW FUNCTIONS ////////////////////////////////
    void updateMainView(const int tabIndex);
    void showVersionInfo();
    void showSetupView();
    void showDataIOView();
    void showDataLabellingToolView();
    void showPipelineToolView();
    void showTrainingToolView();
    void showPredictionView();
    void showLogView();
    void updateInfoText(const std::string msg);
    void updateWarningText(const std::string msg);
    void updateErrorText(const std::string msg);
    void updateHelpText(const std::string msg);

    /////////////////////////////////// SETUP VIEW FUNCTIONS /////////////////////////////////
    void showSetupViewInfo();
    void setNumInputs(const int numInputs);
    void setNumOutputs(const int numOutputs);
    void setPipelineModeAsClassificationMode();
    void setPipelineModeAsRegressionMode();
    void setPipelineModeAsTimeseriesMode();
    void setPipelineModeAsClusterMode();
    void updatePipelineMode(const unsigned int pipelineMode);
    void resetAll();

    ////////////////////////////////// DATA IO TOOL FUNCTIONS ////////////////////////////////
    void showDataIOInfo();
    void updateOSCInput();
    void updateMouseInput();
    void updateOSCControlCommands();
    void resetOSCServer(const int port);
    void resetOSCClient();
    void updateDataAddress();
    void updateOSCMessageLog(const std::string msg);
    void updateNumInputDimensions(const int numInputDimensions);
    void updateNumTargetDimensions(const int numTargetDimensions);

    //////////////////////////// DATA LABELING TOOL FUNCTIONS ///////////////////////////////
    void showDataLabellingToolInfo();
    void recordTrainingData(const bool state);
    void saveTrainingDatasetToFile();
    void loadTrainingDatasetFromFile();
    void updateTrainingClassLabel(const unsigned int trainingClassLabel);
    void updateTargetVectorValue(const double value);
    void updateRecordStatus(const bool recordStatus);
    void updateNumTrainingSamples(const unsigned int numTrainingSamples);
    void addNewTrainingSample(const unsigned int numTrainingSamples,const GRT::ClassificationSample &trainingSample);
    void addNewTrainingSample(const GRT::MatrixFloat &trainingSample);
    void addNewTrainingSample(const unsigned int numTrainingSamples,const GRT::TimeSeriesClassificationSample &trainingSample);
    void resetTrainingData(const GRT::ClassificationData &trainingData);
    void resetTrainingData(const GRT::RegressionData &trainingData);
    void resetTrainingData(const GRT::TimeSeriesClassificationData &trainingData);
    void resetTrainingData(const GRT::UnlabelledData &trainingData);
    void resetTestData(const GRT::ClassificationData &testData);
    void handleDatasetClicked(const QModelIndex &index);
    void updateDatasetName();
    void updateDatasetInfoText();
    void updateTrainingTabView(const int tabIndex);
    void updateDatasetStatsView();
    void updateClassStatsGraph();
    void updatePCAProjectionGraph();
    void updateTimeseriesGraph();
    void updateTimeseriesTrainingDataPlot();
    void generateFeaturePlot();
    void ctrlRShortcut();
    void ctrlSShortcut();
    void ctrlLShortcut();

    //////////////////////////////// PIPELINE TOOl FUNCTIONS ////////////////////////////////
    void showPipelineToolInfo();
    void updatePipelineFilename();
    void savePipelineToFile();
    void loadPipelineFromFile();
    void updatePreProcessingView(const int viewIndex);
    void updateFeatureExtractionView(const int viewIndex);
    void updateClassifierView(const int viewIndex);
    void updateRegressifierView(const int viewIndex);
    void updateClusterView(const int viewIndex);
    void updateTimeseriesClassifierView(const int viewIndex);
    void updatePostProcessingView(const int viewIndex);
    void refreshPipelineSetup();
    void updatePreProcessingSettings();
    void updateClassifierSettings();
    void updateTimeseriesClassifierSettings();
    void updateClusterSettings();
    void clearPipelineConfiguration();
    void updatePipelineConfiguration();
    void resetPipelineConfiguration();
    void resetDefaultPipelineClassificationSetup();
    void resetDefaultPipelineRegressionSetup();
    void resetDefaultPipelineTimeseriesClassificationSetup();
    void resetDefaultPipelineClusterSetup();
    void setupDefaultClassifier();
    void setupDefaultRegressifier();
    void setupDefaultTimeseriesClassifier();
    void setupDefaultCluster();
    void updateClassifier(const unsigned int classifierType,const bool useScaling,const bool useNullRejection,const double nullRejectionCoeff,const double parameter1);

    //////////////////////////////// TRAINING TOOl FUNCTIONS ////////////////////////////////
    void showTrainingToolInfo();
    void loadTestDatasetFromFile();
    void resetTrainingToolView(const int trainingMode);
    void train();
    void randomTestSliderMoved(const int value);
    void numCVFoldsValueChanged(const int value);
    void pipelineTrainingStarted();
    void pipelineTrainingFinished(const bool result);
    void pipelineTestingFinished(const bool result);
    void updateTrainingToolLog(const std::string message);
    void updateTrainingResults(const GRT::TrainingResult &trainingResult);
    void updateTestResults(const GRT::TestInstanceResult &testResult);
    void updatePrecisionGraph(const GRT::VectorFloat &precision,const vector< unsigned int > &classLabels);
    void updateRecallGraph(const GRT::VectorFloat &recall,const vector< unsigned int > &classLabels);
    void updateFmeasureGraph(const GRT::VectorFloat &fmeasure,const vector< unsigned int > &classLabels);
    void updateConfusionMatrixGraph(const GRT::MatrixFloat &confusionMatrix,const vector< unsigned int > &classLabels);
    void clearPrecisionGraph();
    void clearRecallGraph();
    void clearFmeasureGraph();
    void clearConfusionMatrixGraph();

    /////////////////////////////// PREDICTION VIEW FUNCTIONS ///////////////////////////////
    void showPredictionToolInfo();
    void updatePreProcessingData(const GRT::VectorFloat &preProcessedData);
    void updateFeatureExtractionData(const GRT::VectorFloat &featureExtractionData);
    void updateClassificationResults(const unsigned int predictedClassLabel,const double maximumLikelihood,const GRT::VectorFloat &classLikelihoods,const GRT::VectorFloat &classDistances,const std::vector<unsigned int> &classLabels);
    void updateRegressionResults(const GRT::VectorFloat &regressionData);
    void updateTimeseriesClassificationResults(const unsigned int predictedClassLabel,const double maximumLikelihood,const double phase,const GRT::VectorFloat &classLikelihoods,const GRT::VectorFloat &classDistances,const std::vector<unsigned int> &classLabels);
    void updateClusterResults(const unsigned int predictedClusterLabel,const double maximumLikelihood,const GRT::VectorFloat &clusterLikelihoods,const GRT::VectorFloat &clusterDistances,const std::vector<unsigned int> &clusterLabels);
    void resetPredictionViewGraphs();

    ///////////////////////////////    LOG VIEW FUNCTIONS     ///////////////////////////////
    void showLogViewInfo();
    void showAllLogs();
    void showInfoLog();
    void showWarningLog();
    void showErrorLog();
    void updateLogView(const unsigned int viewID);

    //////////////////////////////   SETTINGS VIEW FUNCTIONS     /////////////////////////////
    void openGRTWiki();
    void openGRTForum();
    void openGRTSource();
    void openGRTDownload();
    void updateCoreRefreshRate(const double rate);
    void updateMaximumGraphRefreshRate(const double framerate);

    /////////////////////////////////// CORE DATA FUNCTIONS /////////////////////////////////
    void coreTick();
    void updateData( const GRT::VectorFloat &data );
    void updateTargetVector( const GRT::VectorFloat &targetVector );

private:
    bool initMainMenu();
    bool initSetupView();
    bool initDataIOView();
    bool initDataLabellingToolView();
    bool initPipelineToolView();
    bool initTrainingToolView();
    bool initPreditionView();
    bool initLogView();
    bool initSettingsView();
    bool initHelpView();
    bool initSignalsAndSlots();
    virtual void notify(const GRT::TrainingLogMessage &log);
    virtual void notify(const GRT::TestingLogMessage &log);
    virtual void notify(const GRT::WarningLogMessage &log);
    virtual void notify(const GRT::ErrorLogMessage &log);
    virtual void notify(const GRT::InfoLogMessage &log);

    std::mutex mutex;
    Ui::MainWindow *ui;
    QStandardItemModel *model;
    Core core;
    static unsigned int numInstances;
    vector< QWidget* > dataLabelingToolTabHistory;
    vector< QWidget* > trainingToolTabHistory;
    TimeseriesGraph *inputDataGraph;
    TimeseriesGraph *preProcessedDataGraph;
    TimeseriesGraph *classPredictionsGraph;
    TimeseriesGraph *featureExtractionDataGraph;
    TimeseriesGraph *classLikelihoodsGraph;
    TimeseriesGraph *classDistancesGraph;
    TimeseriesGraph *regressionGraph;
    TimeseriesGraph *swipeDetectorGraph;
    TimeseriesGraph *clusterPredictionsGraph;
    TimeseriesGraph *clusterLikelihoodsGraph;
    TimeseriesGraph *clusterDistancesGraph;
    FeaturePlot *pcaGraph;
    vector< FeaturePlot* > featurePlots;
    GRT::ErrorLog errorLog;
    GRT::WarningLog warningLog;
    vector< Qt::GlobalColor > defaultGraphColors;
    GRT::Timer lastGuiUpdateTimer;

    GRT::SwipeDetector swipeDetector;

};

#endif // MAINWINDOW_H
