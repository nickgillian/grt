#include "mainwindow.h"
#include "ui_mainwindow.h"

unsigned int MainWindow::numInstances = 0;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), model(NULL)
{
    numInstances++;
    //qDebug() << "NumInstances: " + QString::number( numInstances );

    //Register the log callbacks
    GRT::TrainingLog::registerObserver( *this );
    //GRT::WarningLog::registerObserver( *this );
    //GRT::ErrorLog::registerObserver( *this );

    //Register the custom data types
    qRegisterMetaType< QTextCursor >("QTextCursor");
    qRegisterMetaType< std::string >("std::string");
    qRegisterMetaType< GRT::VectorDouble >("GRT::VectorDouble");
    qRegisterMetaType< std::vector<unsigned int> >("std::vector<unsigned int>");
    qRegisterMetaType< GRT::ClassificationData >("GRT::ClassificationData");
    qRegisterMetaType< GRT::ClassificationSample >("GRT::ClassificationSample");
    qRegisterMetaType< GRT::RegressionData >("GRT::RegressionData");
    qRegisterMetaType< GRT::RegressionSample >("GRT::RegressionSample");
    qRegisterMetaType< GRT::TrainingResult >("GRT::TrainingResult");
    qRegisterMetaType< GRT::TestInstanceResult >("GRT::TestInstanceResult");
    qRegisterMetaType< GRT::GestureRecognitionPipeline >("GRT::GestureRecognitionPipeline");
    qRegisterMetaType< GRT::MLBase >("GRT::MLBase");

    //Setup the UI
    ui->setupUi(this);
    setWindowTitle( "Gesture Recognition Toolkit" );

    switch( GRT::Util::getOS() ){
        case GRT::Util::OS_OSX:
            setWindowIcon( QIcon("Resources/OSX/GRT.icns") );
        break;
        case GRT::Util::OS_LINUX:
        break;
        case GRT::Util::OS_WINDOWS:
        break;
        case GRT::Util::OS_UNKNOWN:
        break;
    }

    //Store the tabs in the tab view so we can use them later
    for(int i=0; i<ui->dataLabelingTool_trainingDataTab->count(); i++){
        dataLabelingToolTabHistory.push_back( ui->dataLabelingTool_trainingDataTab->widget(i) );
    }

    for(int i=0; i<ui->trainingTool_resultsTab->count(); i++){
        trainingToolTabHistory.push_back( ui->trainingTool_resultsTab->widget(i) );
    }

    //Setup the default graph colors
    defaultGraphColors.push_back( Qt::red );
    defaultGraphColors.push_back( Qt::green );
    defaultGraphColors.push_back( Qt::blue );
    defaultGraphColors.push_back( Qt::magenta );
    defaultGraphColors.push_back( Qt::yellow );
    defaultGraphColors.push_back( Qt::cyan );
    defaultGraphColors.push_back( Qt::gray );
    defaultGraphColors.push_back( Qt::darkRed );
    defaultGraphColors.push_back( Qt::darkGreen );
    defaultGraphColors.push_back( Qt::darkBlue );
    defaultGraphColors.push_back( Qt::darkMagenta );
    defaultGraphColors.push_back( Qt::darkYellow );
    defaultGraphColors.push_back( Qt::darkCyan );
    defaultGraphColors.push_back( Qt::darkGray );

    //Initialize the views
    initMainMenu();
    initSetupView();
    initDataIOView();
    initDataLabellingToolView();
    initPipelineToolView();
    initTrainingToolView();
    initPreditionView();
    initLogView();

    //Initialize the signals and slots
    initSignalsAndSlots();

    //Set the setup view as the default view
    showSetupView();

    //Set the pipeline into classification mode
    ui->setupView_classificationModeButton->setChecked( true );
    updatePipelineMode( Core::CLASSIFICATION_MODE );

    //Set the default number of inputs (we do not need to do this for the outputs)
    setNumInputs( 1 );

    //Set the current version
    core.setVersion( GRT_GUI_VERSION );

    //Start the core
    core.start();
}

MainWindow::~MainWindow()
{
    core.stop();
    delete ui;
    numInstances--;
}

unsigned int MainWindow::getCurrentView() const{ return ui->mainTab->currentIndex(); }

bool MainWindow::initMainMenu(){

    ui->mainTab->setCurrentIndex( SETUP_VIEW );
    ui->mainTab->setTabText(SETUP_VIEW,"Setup");
    ui->mainTab->setTabText(DATA_IO_VIEW,"Data I/O");
    ui->mainTab->setTabText(DATA_MANAGER_VIEW,"Data Manager");
    ui->mainTab->setTabText(PIPELINE_VIEW,"Pipeline Tool");
    ui->mainTab->setTabText(TRAINING_VIEW,"Training Tool");
    ui->mainTab->setTabText(PREDICTION_VIEW,"Prediction Tool");
    ui->mainTab->setTabText(LOG_VIEW,"Log");
    ui->mainWindow_infoTextField->setReadOnly( true );
    ui->mainWindow_pipelineTrainedInfoTextField->setReadOnly( true );
    ui->mainWindow_pipelineTrainedInfoTextField->setText( "NO" );

    return true;
}

bool MainWindow::initSetupView(){

    ui->setupView_classificationModeButton->setCheckable( true );
    ui->setupView_regressionModeButton->setCheckable( true );
    ui->setupView_timeseriesModeButton->setCheckable( true );
    ui->setupView_timeseriesModeButton->setEnabled( false );
    ui->setupView_numInputsSpinBox->setValue( 1 );
    ui->setupView_numOutputsSpinBox->setValue( 1 );
    ui->setupView_numInputsSpinBox->setRange( 1, 10000 );
    ui->setupView_numOutputsSpinBox->setRange( 1, 10000 );

    ui->setupView_classificationImage->setPixmap( QPixmap( QPixmap::fromImage( QImage( ":/ClassificationModeImage.png" ) ) ) );
    ui->setupView_classificationImage->setScaledContents( true );

    ui->setupView_regressionImage->setPixmap( QPixmap( QPixmap::fromImage( QImage( ":/RegressionModeImage.png" ) ) ) );
    ui->setupView_regressionImage->setScaledContents( true );

    ui->setupView_timeseriesImage->setPixmap( QPixmap( QPixmap::fromImage( QImage( ":/TimeseriesModeImage.png" ) ) ) );
    ui->setupView_timeseriesImage->setScaledContents( true );

    return true;
}

bool MainWindow::initDataIOView(){

    unsigned int oscServerIncomingPort = 5000;
    unsigned int oscServerOutgoingPort = 5001;

    oscServerIncomingPort += numInstances-1;

    ui->dataIO_oscIncomingPortSpinBox->setRange(1,50000);
    ui->dataIO_oscOutgoingPortSpinBox->setRange(1,50000);
    ui->dataIO_oscIncomingPortSpinBox->setValue( oscServerIncomingPort );
    ui->dataIO_oscOutgoingPortSpinBox->setValue( oscServerOutgoingPort );
    ui->dataIO_clientIPAddressTextField->setText( "127.0.0.1" );
    ui->dataIO_mainDataAddressTextField->setText( "/Data" );

    core.setMainDataAddress( "/Data" );
    core.resetOSCServer( oscServerIncomingPort );
    core.resetOSCClient("127.0.0.1",oscServerOutgoingPort);

    ui->dataIO_enableOSCCommandsButton->setCheckable( true );
    ui->dataIO_enableOSCCommandsButton->setChecked( true );
    ui->dataIO_enableOSCInputButton->setCheckable( true );
    ui->dataIO_enableOSCInputButton->setChecked( true );
    ui->dataIO_enableMouseInputButton->setCheckable( true );
    ui->dataIO_enableMouseInputButton->setChecked( false );
    ui->dataIO_oscIncomingPortSpinBox->setRange(1,50000);
    ui->dataIO_numInputDimensionsField->setText( QString::number( 1 )  );
    ui->dataIO_targetVectorSizeField->setText( QString::number( 1 )  );
    ui->dataIO_numInputDimensionsField->setReadOnly( true );
    ui->dataIO_targetVectorSizeField->setReadOnly( true );
    ui->dataIO_OSCMessageLog->setReadOnly( true );

    return true;
}

bool MainWindow::initDataLabellingToolView(){

    ui->dataLabellingTool_classificationModeRecordButton->setCheckable( true );
    ui->dataLabellingTool_regressionModeRecordButton->setCheckable( true );

    //Make sure the core is not recording
    core.setRecordingState( false );

    ui->dataLabellingTool_classificationModeRecordButton->setChecked( false );
    ui->dataLabellingTool_regressionModeRecordButton->setChecked( false );

    ui->dataLabellingTool_classificationDataInfoTextField->setText("");
    ui->dataLabellingTool_regressionDataInfoTextField->setText("");
    ui->dataLabellingTool_classLabel->setValue( core.getTrainingClassLabel() );
    ui->dataLabellingTool_classLabel->setRange(1,100000);
    ui->dataLabellingTool_numTrainingSamples->setText( QString::number( core.getNumTrainingSamples() ) );
    ui->dataLabellingTool_numTrainingSamples->setReadOnly( true );
    ui->dataLabellingTool_numInputDimensionsField->setText( QString::number( 1 ) );
    ui->dataLabellingTool_numInputDimensionsField->setReadOnly( true );
    ui->dataLabellingTool_numInputDimensionsField_2->setReadOnly( true );
    ui->dataLabellingTool_numInputDimensionsField_2->setText( QString::number( 1 ) );
    ui->dataLabellingTool_targetVectorTextField->setText("");
    ui->dataLabellingTool_targetVectorTextField->setReadOnly( true );
    ui->dataLabellingTool_numTargetDimensionsField->setText("");
    ui->dataLabellingTool_numTargetDimensionsField->setReadOnly( true );
    ui->dataLabellingTool_numClassesField->setText( QString::number( core.getNumClassesInTrainingData() ) );
    ui->dataLabellingTool_numClassesField->setReadOnly( true );

    //Set the graph titles (we need to do this here otherwise we get a new title each time the graph is drawn)
    QCustomPlot *plot;

    plot = ui->dataLabelingTool_classStatsGraph;
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(plot, "Number of samples per class"));

    return true;
}

bool MainWindow::initTrainingToolView(){

    ui->trainingTool_trainingMode->setCurrentIndex(0);
    ui->trainingTool_validationModeSettingsView->setCurrentIndex(0);
    ui->trainingTool_numTrainingSamples->setText("0");
    ui->trainingTool_numTestSamples->setText("0");
    ui->trainingTool_randomTestPercentageBox->setText("20%");
    ui->trainingTool_randomTestPercentageSlider->setValue(20);
    ui->trainingTool_randomTestPercentageSlider->setRange(0,100);
    ui->trainingTool_numCVFolds->setValue( 10 );
    ui->trainingTool_numCVFolds->setRange(1,1000000);
    ui->trainingTool_results->setText("");

    ui->trainingTool_numTrainingSamples->setReadOnly( true );
    ui->trainingTool_numTestSamples->setReadOnly( true );
    ui->trainingTool_randomTestPercentageBox->setReadOnly( true );

    ui->trainingTool_resultsTab->setCurrentIndex( 0 );
    ui->trainingTool_resultsTab->setTabText(0,"Results");
    ui->trainingTool_resultsTab->setTabText(1,"Precision");
    ui->trainingTool_resultsTab->setTabText(2,"Recall");
    ui->trainingTool_resultsTab->setTabText(3,"F-Measure");
    ui->trainingTool_resultsTab->setTabText(4,"RMS");

    //Training Tool Graphs
    QCustomPlot *plot;
    plot = ui->trainingTool_precisionGraph;
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(plot, "Precision results for each class"));

    plot = ui->trainingTool_recallGraph;
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(plot, "Recall results for each class"));

    plot = ui->trainingTool_fmeasureGraph;
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(plot, "F-Measure results for each class"));

    resetTrainingToolView( 0 );

    return true;
}

bool MainWindow::initPipelineToolView(){

    ////////////////////////////////////// Pre processing //////////////////////////////////////
    ui->pipelineTool_preProcessingType->setCurrentIndex( 0 );
    ui->pipelineTool_preProcessingOptionsView->setCurrentIndex( 0 );

    //Moving Average Filter
    ui->pipelineTool_movingAverageFilterSizeSpinBox->setRange(1,10000);
    ui->pipelineTool_movingAverageFilterSizeSpinBox->setValue( 5 );

    //Double Moving Average Filter
    ui->pipelineTool_doubleMovingAverageFilterSizeSpinBox->setRange(1,10000);
    ui->pipelineTool_doubleMovingAverageFilterSizeSpinBox->setValue( 5 );

    //Low Pass Filter
    ui->pipelineTool_lowPassFilterCutoffFrequencySpinBox->setValue( 10.0 );
    ui->pipelineTool_lowPassFilterCutoffFrequencySpinBox->setRange(0.0,10000.0);
    ui->pipelineTool_lowPassFilterSampleRateSpinBox->setValue( 100 );
    ui->pipelineTool_lowPassFilterSampleRateSpinBox->setRange(0.0,10000.0);
    ui->pipelineTool_lowPassFilterGainSpinBox->setValue( 1.0 );

    //High Pass Filter
    ui->pipelineTool_highPassFilterCutoffFrequencySpinBox->setValue( 10.0 );
    ui->pipelineTool_highPassFilterCutoffFrequencySpinBox->setRange(1.0,10000.0);
    ui->pipelineTool_highPassFilterSampleRateSpinBox->setValue( 100 );
    ui->pipelineTool_highPassFilterSampleRateSpinBox->setRange(1.0,10000.0);
    ui->pipelineTool_highPassFilterGainSpinBox->setValue( 1.0 );

    //Derivative
    ui->pipelineTool_derivativeOrderList->setCurrentIndex( 0 );

    //Dead Zone
    ui->pipelineTool_deadZoneLowerLimitSpinBox->setRange(-100000,100000);
    ui->pipelineTool_deadZoneUpperLimitSpinBox->setRange(-100000,100000);
    ui->pipelineTool_deadZoneLowerLimitSpinBox->setValue( -1 );
    ui->pipelineTool_deadZoneUpperLimitSpinBox->setValue( 1 );
    ui->pipelineTool_deadZoneLowerLimitSpinBox->setDecimals( 5 );
    ui->pipelineTool_deadZoneUpperLimitSpinBox->setDecimals( 5 );


    ////////////////////////////////////// Feature Extraction //////////////////////////////////////

    ////////////////////////////////////// Classificaition //////////////////////////////////////
    ui->pipelineTool_classifierRegressifierView->setCurrentIndex( 0 );
    ui->pipelineTool_classifierType->setCurrentIndex( 0 );
    ui->pipelineTool_regressifierType->setCurrentIndex( 0 );
    ui->pipelineTool_classifierOptionsView->setCurrentIndex( 0 );
    ui->pipelineTool_regressifierOptionsView->setCurrentIndex( 0 );
    ui->pipelineTool_enableScaling->setChecked( false );
    ui->pipelineTool_enableNullRejection->setChecked( false );
    ui->pipelineTool_nullRejectionCoeff->setValue( 2.0 );
    ui->pipelineTool_nullRejectionCoeff->setRange( 0.01, 100 );
    ui->pipelineTool_nullRejectionCoeff->setDecimals( 5 );

    //ANBC

    //AdaBoost
    ui->pipelineTool_adaboostNumBoostingIterations->setValue( 20 );
    ui->pipelineTool_adaboostNumBoostingIterations->setRange(1,100000);

    //DecisionTree
    ui->pipelineTool_decisionTree_numSpiltingSteps->setValue( 20 );
    ui->pipelineTool_decisionTree_numSpiltingSteps->setRange(1,10000);
    ui->pipelineTool_decisionTree_minSamplesPerNode->setValue( 5 );
    ui->pipelineTool_decisionTree_minSamplesPerNode->setRange(1,10000);
    ui->pipelineTool_decisionTree_maxDepth->setValue( 20 );
    ui->pipelineTool_decisionTree_maxDepth->setRange(1,1000);

    //GMM
    ui->pipelineTool_gmmNumMixtureModels->setValue( 2 );
    ui->pipelineTool_gmmNumMixtureModels->setRange(1,100);

    //KNN
    ui->pipelineTool_knnK->setValue( 5 );
    ui->pipelineTool_knnK->setRange(1,10000);

    //Min Dist Options
    ui->pipelineTool_minDistNumClusters->setValue( 2 );

    //Random Forests
    ui->pipelineTool_randomForests_numTrees->setValue( 10 );
    ui->pipelineTool_randomForests_numTrees->setRange(1,10000);
    ui->pipelineTool_randomForests_numSpiltingSteps->setValue( 20 );
    ui->pipelineTool_randomForests_numSpiltingSteps->setRange(1,10000);
    ui->pipelineTool_randomForests_minSamplesPerNode->setValue( 5 );
    ui->pipelineTool_randomForests_minSamplesPerNode->setRange(1,10000);
    ui->pipelineTool_randomForests_maxDepth->setValue( 20 );
    ui->pipelineTool_randomForests_maxDepth->setRange(1,1000);

    //SVM Options
    ui->pipelineTool_svmKernelType->setCurrentIndex( 0 );
    ui->pipelineTool_svmGamma->setValue( 0.1 );
    ui->pipelineTool_svmGamma->setDecimals( 5 );

    //Swipe Detector Options
    ui->pipelineTool_swipeDetector_plot->setCheckable( true );
    ui->pipelineTool_swipeDetector_swipeIndex->setMinimum( 0 );
    ui->pipelineTool_swipeDetector_swipeIndex->setMaximum( core.getNumInputDimensions() );
    ui->pipelineTool_swipeDetector_swipeDirection->setCurrentIndex( 0 );
    ui->pipelineTool_swipeDetector_swipeThreshold->setMinimum( - numeric_limits<double>::max() );
    ui->pipelineTool_swipeDetector_swipeThreshold->setMaximum( numeric_limits<double>::max() );
    ui->pipelineTool_swipeDetector_swipeThreshold->setValue( 1 );
    ui->pipelineTool_swipeDetector_hysteresisThreshold->setMinimum( - numeric_limits<double>::max() );
    ui->pipelineTool_swipeDetector_hysteresisThreshold->setMaximum( numeric_limits<double>::max() );
    ui->pipelineTool_swipeDetector_hysteresisThreshold->setValue( 0.5 );
    ui->pipelineTool_swipeDetector_movementThreshold->setMinimum( 0 );
    ui->pipelineTool_swipeDetector_movementThreshold->setMaximum( numeric_limits<double>::max() );
    ui->pipelineTool_swipeDetector_movementThreshold->setValue( 10000 );
    ui->pipelineTool_swipeDetector_swipeIntegrationCoeff->setMinimum( 0 );
    ui->pipelineTool_swipeDetector_swipeIntegrationCoeff->setMaximum( 1 );
    ui->pipelineTool_swipeDetector_swipeIntegrationCoeff->setValue( 0.92 );

    ////////////////////////////////////// Regression //////////////////////////////////////
    ui->pipelineTool_regressionView_enableScaling->setChecked( true );
    ui->pipelineTool_regressionView_minChangeSpinBox->setRange( 0.0, 1.0 );
    ui->pipelineTool_regressionView_minChangeSpinBox->setDecimals( 5 );
    ui->pipelineTool_regressionView_minChangeSpinBox->setValue( 0.01 );
    ui->pipelineTool_regressionView_maxNumEpochsSpinBox->setRange(1,10000);
    ui->pipelineTool_regressionView_maxNumEpochsSpinBox->setValue( 100 );

    //MLP
    ui->pipelineTool_mlpNumHiddenNeurons->setValue( 1 );
    ui->pipelineTool_mlpUseMDRegression->setChecked( true );
    ui->pipelineTool_mlpHiddenLayerType->setCurrentIndex( 0 );
    ui->pipelineTool_mlpOutputLayerType->setCurrentIndex( 0 );

    ////////////////////////////////////// Post Processing //////////////////////////////////////
    ui->pipelineTool_postProcessingStackedWidget->setCurrentIndex( 0 );
    ui->pipelineTool_postProcessingType->setCurrentIndex( 0 );
    ui->pipelineTool_postProcessingOptionsView->setCurrentIndex( 0 );

    //Class Label Filter
    ui->pipelineTool_classLabelFilterMinCountSpinBox->setRange(1,100000);
    ui->pipelineTool_classLabelFilterMinCountSpinBox->setValue( 5 );
    ui->pipelineTool_classLabelFilterBufferSizeSpinBox->setRange(1,100000);
    ui->pipelineTool_classLabelFilterBufferSizeSpinBox->setValue( 10 );

    //Class Change Filter

    //Class Label Timeout Filter
    ui->pipelineTool_classLabelTimeoutFilterTimeoutDurationSpinBox->setRange(1,1000000);
    ui->pipelineTool_classLabelTimeoutFilterTimeoutDurationSpinBox->setValue( 1000 );
    ui->pipelineTool_classLabelTimeoutFilterFilterModeList->setCurrentIndex( 0 );

    return true;
}

bool MainWindow::initPreditionView(){

    ui->predictionWindow_EnablePrediction->setCheckable( true );
    ui->predictionWindow_EnablePrediction->setChecked( true );
    ui->predictionWindow_data->setReadOnly( true );
    ui->predictionWindow_preProcessedData->setReadOnly( true );
    ui->predictionWindow_featureExtractionData->setReadOnly( true );
    ui->predictionWindow_predictedClassLabel->setReadOnly( true );
    ui->predictionWindow_maximumLikelihood->setReadOnly( true );
    ui->predictionWindow_classLikelihoods->setReadOnly( true );
    ui->predictionWindow_classDistances->setReadOnly( true );
    ui->predictionWindow_classLabels->setReadOnly( true );

    ui->predictionWindow_plotInputDataButton->setCheckable( true );
    ui->predictionWindow_plotPreProcessedDataButton->setCheckable( true );
    ui->predictionWindow_plotFeatureExtractionDataButton->setCheckable( true );
    ui->predictionWindow_plotPredictedClassLabelsDataButton->setCheckable( true );
    ui->predictionWindow_plotClassLikelihoodsDataButton->setCheckable( true );
    ui->predictionWindow_plotClassDistancesDataButton->setCheckable( true );
    ui->predictionWindow_plotRegressionDataButton->setCheckable( true );

    ui->predictionWindow_plotInputDataButton->setChecked( false );
    ui->predictionWindow_plotPreProcessedDataButton->setChecked( false );
    ui->predictionWindow_plotFeatureExtractionDataButton->setChecked( false );
    ui->predictionWindow_plotPredictedClassLabelsDataButton->setChecked( false );
    ui->predictionWindow_plotClassLikelihoodsDataButton->setChecked( false );
    ui->predictionWindow_plotClassDistancesDataButton->setChecked( false );
    ui->predictionWindow_plotRegressionDataButton->setChecked( false );

    ui->predictionWindow_plotInputDataButton->setEnabled( true );
    ui->predictionWindow_plotPreProcessedDataButton->setEnabled( false );
    ui->predictionWindow_plotFeatureExtractionDataButton->setEnabled( false );
    ui->predictionWindow_plotPredictedClassLabelsDataButton->setEnabled( false );
    ui->predictionWindow_plotClassLikelihoodsDataButton->setEnabled( false );
    ui->predictionWindow_plotClassDistancesDataButton->setEnabled( false );
    ui->predictionWindow_plotRegressionDataButton->setEnabled( true );

    //Setup the graphs
    inputDataGraph = new TimeseriesGraph();
    preProcessedDataGraph = new TimeseriesGraph();
    featureExtractionDataGraph = new TimeseriesGraph();
    classPredictionsGraph = new TimeseriesGraph();
    classLikelihoodsGraph = new TimeseriesGraph();
    classDistancesGraph = new TimeseriesGraph();
    regressionGraph = new TimeseriesGraph();
    swipeDetectorGraph = new TimeseriesGraph();

    inputDataGraph->setWindowTitle( "Input Data" );
    preProcessedDataGraph->setWindowTitle( "Pre Processed Data" );
    featureExtractionDataGraph->setWindowTitle( "Feature Extraction Data" );
    classPredictionsGraph->setWindowTitle( "Class Predictions" );
    classLikelihoodsGraph->setWindowTitle( "Class Likelihoods" );
    classDistancesGraph->setWindowTitle( "Class Distances" );
    regressionGraph->setWindowTitle( "Regression Data" );
    swipeDetectorGraph->setWindowTitle( "Swipe Detector" );

    return true;
}

bool MainWindow::initLogView(){

    ui->logView_allLogsButton->setCheckable( true );
    ui->logView_infoLogButton->setCheckable( true );
    ui->logView_warningLogButton->setCheckable( true );
    ui->logView_errorLogButton->setCheckable( true );

    ui->logView_allLogsButton->setChecked( true );
    updateLogView( ALL_LOGS_VIEW );

    return true;
}

bool MainWindow::initSignalsAndSlots(){

    //Connect the main signals and slots
    connect(ui->mainMenu_about, SIGNAL(triggered()), this, SLOT(showVersionInfo()));
    connect(ui->mainTab, SIGNAL(currentChanged(int)), this, SLOT(updateMainView(int)));

    connect(ui->setupView_infoButton, SIGNAL(clicked()), this, SLOT(showSetupViewInfo()));
    connect(ui->setupView_classificationModeButton, SIGNAL(pressed()), this, SLOT(setPipelineModeAsClassificationMode()));
    connect(ui->setupView_regressionModeButton, SIGNAL(pressed()), this, SLOT(setPipelineModeAsRegressionMode()));
    connect(ui->setupView_timeseriesModeButton, SIGNAL(pressed()), this, SLOT(setPipelineModeAsTimeseriesMode()));
    connect(ui->setupView_numInputsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setNumInputs(int)));
    connect(ui->setupView_numOutputsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setNumOutputs(int)));
    connect(ui->setup_resetAllButton, SIGNAL(clicked()), this, SLOT(resetAll()));

    connect(ui->dataIO_infoButton, SIGNAL(clicked()), this, SLOT(showDataIOInfo()));
    connect(ui->dataIO_enableOSCInputButton, SIGNAL(clicked()), this, SLOT(updateOSCInput()));
    connect(ui->dataIO_enableMouseInputButton, SIGNAL(clicked()), this, SLOT(updateMouseInput()));
    connect(ui->dataIO_enableOSCCommandsButton, SIGNAL(clicked()), this, SLOT(updateOSCControlCommands()));
    connect(ui->dataIO_mainDataAddressTextField, SIGNAL(editingFinished()), this, SLOT(updateDataAddress()));
    connect(ui->dataIO_oscIncomingPortSpinBox, SIGNAL(valueChanged(int)), &core, SLOT(resetOSCServer(int)));
    connect(ui->dataIO_clientIPAddressTextField, SIGNAL(editingFinished()), this, SLOT(resetOSCClient()));
    connect(ui->dataIO_oscOutgoingPortSpinBox, SIGNAL(editingFinished()), this, SLOT(resetOSCClient()));

    connect(ui->dataLabellingTool_infoButton, SIGNAL(clicked()), this, SLOT(showDataLabellingToolInfo()));
    connect(ui->dataLabellingTool_classificationModeRecordButton, SIGNAL(clicked(bool)), this, SLOT(recordTrainingData(bool)));
    connect(ui->dataLabellingTool_regressionModeRecordButton, SIGNAL(clicked(bool)), this, SLOT(recordTrainingData(bool)));
    connect(ui->dataLabellingTool_saveButton, SIGNAL(clicked()),this, SLOT(saveTrainingDatasetToFile()));
    connect(ui->dataLabellingTool_loadButton, SIGNAL(clicked()),this, SLOT(loadTrainingDatasetFromFile()));
    connect(ui->dataLabellingTool_clearButton, SIGNAL(clicked()), &core, SLOT(clearTrainingData()));
    connect(ui->dataLabellingTool_classLabel, SIGNAL(valueChanged(int)), &core, SLOT(setTrainingClassLabel(int)));
    connect(ui->dataLabellingTool_targetVectorValueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateTargetVectorValue(double)));
    connect(ui->dataLabellingTool_classificationDatasetName, SIGNAL(editingFinished()), this, SLOT(updateDatasetName()));
    connect(ui->dataLabellingTool_regressionDatasetName, SIGNAL(editingFinished()), this, SLOT(updateDatasetName()));
    connect(ui->dataLabellingTool_classificationDataInfoTextField, SIGNAL(editingFinished()), this, SLOT(updateDatasetInfoText()));
    connect(ui->dataLabellingTool_regressionDataInfoTextField, SIGNAL(editingFinished()), this, SLOT(updateDatasetInfoText()));
    connect(ui->dataLabelingTool_trainingDataTab, SIGNAL(currentChanged(int)), this, SLOT(updateTrainingTabView(int)));
    connect(ui->dataLabellingTool_treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(handleDatasetClicked(QModelIndex)));

    connect(ui->pipelineTool_infoButton, SIGNAL(clicked()), this, SLOT(showPipelineToolInfo()));
    connect(ui->pipelineTool_savePipelineButton, SIGNAL(clicked()), this, SLOT(savePipelineToFile()));
    connect(ui->pipelineTool_loadPipelineButton, SIGNAL(clicked()), this, SLOT(loadPipelineFromFile()));
    connect(ui->pipelineTool_clearPipelineButton, SIGNAL(clicked()), this, SLOT(clearPipelineConfiguration()));
    connect(ui->pipelineTool_preProcessingType, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreProcessingView(int)));
    connect(ui->pipelineTool_classifierType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateClassifierView(int)));
    connect(ui->pipelineTool_regressifierType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateRegressifierView(int)));
    connect(ui->pipelineTool_movingAverageFilterSizeSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_doubleMovingAverageFilterSizeSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_lowPassFilterCutoffFrequencySpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_lowPassFilterGainSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_lowPassFilterSampleRateSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_highPassFilterCutoffFrequencySpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_highPassFilterGainSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_highPassFilterSampleRateSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_derivativeOrderList, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_deadZoneLowerLimitSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_deadZoneUpperLimitSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_classLabelFilterMinCountSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_classLabelFilterBufferSizeSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_classifierType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateClassifierView(int)));

    connect(ui->pipelineTool_swipeDetector_plot, SIGNAL(clicked(bool)), swipeDetectorGraph, SLOT(setVisible(bool)));
    connect(ui->pipelineTool_swipeDetector_swipeIndex, SIGNAL(editingFinished()), this, SLOT(updateClassifierSettings()));
    connect(ui->pipelineTool_swipeDetector_swipeDirection, SIGNAL(currentIndexChanged(int)), this, SLOT(updateClassifierSettings()));
    connect(ui->pipelineTool_swipeDetector_hysteresisThreshold, SIGNAL(editingFinished()), this, SLOT(updateClassifierSettings()));
    connect(ui->pipelineTool_swipeDetector_movementThreshold, SIGNAL(editingFinished()), this, SLOT(updateClassifierSettings()));
    connect(ui->pipelineTool_swipeDetector_swipeIntegrationCoeff, SIGNAL(editingFinished()), this, SLOT(updateClassifierSettings()));

    connect(ui->predictionTool_infoButton, SIGNAL(clicked()), this, SLOT(showPredictionToolInfo()));
    connect(ui->pipelineTool_postProcessingType, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePostProcessingView(int)));
    connect(ui->pipelineTool_classLabelTimeoutFilterTimeoutDurationSpinBox, SIGNAL(editingFinished()), this, SLOT(updatePreProcessingSettings()));
    connect(ui->pipelineTool_classLabelTimeoutFilterFilterModeList, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreProcessingSettings()));

    connect(ui->trainingTool_infoButton, SIGNAL(clicked()), this, SLOT(showTrainingToolInfo()));
    connect(ui->trainingTool_trainButton, SIGNAL(clicked()), this, SLOT(train()));
    connect(ui->trainingTool_trainingMode, SIGNAL(currentIndexChanged(int)), this, SLOT(resetTrainingToolView(int)));
    connect(ui->trainingTool_randomTestPercentageSlider, SIGNAL(sliderMoved(int)), this, SLOT(randomTestSliderMoved(int)));
    connect(ui->trainingTool_numCVFolds, SIGNAL(valueChanged(int)), this, SLOT(numCVFoldsValueChanged(int)));
    connect(ui->trainingTool_loadExternalTestDatasetButton, SIGNAL(clicked()), this, SLOT(loadTestDatasetFromFile()));

    connect(ui->predictionWindow_EnablePrediction, SIGNAL(clicked(bool)), &core, SLOT(enablePrediction(bool)));
    connect(ui->predictionWindow_plotInputDataButton, SIGNAL(clicked(bool)), inputDataGraph, SLOT(setVisible(bool)));
    connect(ui->predictionWindow_plotPreProcessedDataButton, SIGNAL(clicked(bool)), preProcessedDataGraph, SLOT(setVisible(bool)));
    connect(ui->predictionWindow_plotFeatureExtractionDataButton, SIGNAL(clicked(bool)), featureExtractionDataGraph, SLOT(setVisible(bool)));
    connect(ui->predictionWindow_plotPredictedClassLabelsDataButton, SIGNAL(clicked(bool)), classPredictionsGraph, SLOT(setVisible(bool)));
    connect(ui->predictionWindow_plotClassLikelihoodsDataButton, SIGNAL(clicked(bool)), classLikelihoodsGraph, SLOT(setVisible(bool)));
    connect(ui->predictionWindow_plotClassDistancesDataButton, SIGNAL(clicked(bool)), classDistancesGraph, SLOT(setVisible(bool)));
    connect(ui->predictionWindow_plotRegressionDataButton, SIGNAL(clicked(bool)), regressionGraph, SLOT(setVisible(bool)));

    connect(ui->logView_infoButton, SIGNAL(clicked()), this, SLOT(showLogViewInfo()));
    connect(ui->logView_allLogsButton, SIGNAL(clicked()), this, SLOT(showAllLogs()));
    connect(ui->logView_infoLogButton, SIGNAL(clicked()), this, SLOT(showInfoLog()));
    connect(ui->logView_warningLogButton, SIGNAL(clicked()), this, SLOT(showWarningLog()));
    connect(ui->logView_errorLogButton, SIGNAL(clicked()), this, SLOT(showErrorLog()));

    connect(&core, SIGNAL(tick()), this, SLOT(coreTick()));
    connect(&core, SIGNAL(newInfoMessage(std::string)), this, SLOT(updateInfoText(std::string)));
    connect(&core, SIGNAL(newWarningMessage(std::string)), this, SLOT(updateWarningText(std::string)));
    connect(&core, SIGNAL(newErrorMessage(std::string)), this, SLOT(updateErrorText(std::string)));
    connect(&core, SIGNAL(newHelpMessage(std::string)), this, SLOT(updateHelpText(std::string)));
    connect(&core, SIGNAL(newOSCMessage(std::string)), this, SLOT(updateOSCMessageLog(std::string)));
    connect(&core, SIGNAL(newTrainingResultReceived(const GRT::TrainingResult&)), this, SLOT(updateTrainingResults(const GRT::TrainingResult&)));
    connect(&core, SIGNAL(newTestInstanceResultReceived(const GRT::TestInstanceResult&)), this, SLOT(updateTestResults(const GRT::TestInstanceResult&)));
    connect(&core, SIGNAL(pipelineModeChanged(unsigned int)), this, SLOT(updatePipelineMode(unsigned int)));
    connect(&core, SIGNAL(numInputDimensionsChanged(int)), this, SLOT(updateNumInputDimensions(int)));
    connect(&core, SIGNAL(numTargetDimensionsChanged(int)), this, SLOT(updateNumTargetDimensions(int)));
    connect(&core, SIGNAL(trainingClassLabelChanged(unsigned int)), this, SLOT(updateTrainingClassLabel(unsigned int)));
    connect(&core, SIGNAL(dataChanged(GRT::VectorDouble)), this, SLOT(updateData(GRT::VectorDouble)));
    connect(&core, SIGNAL(targetDataChanged(GRT::VectorDouble)), this, SLOT(updateTargetVector(GRT::VectorDouble)));
    connect(&core, SIGNAL(recordStatusChanged(bool)), this, SLOT(updateRecordStatus(bool)));
    connect(&core, SIGNAL(pipelineConfigurationChanged()), this, SLOT(updatePipelineConfiguration()));
    connect(&core, SIGNAL(pipelineConfigurationReset()), this, SLOT(resetPipelineConfiguration()));
    connect(&core, SIGNAL(numTrainingSamplesChanged(unsigned int)), this, SLOT(updateNumTrainingSamples(unsigned int)));
    connect(&core, SIGNAL(newTrainingSampleAdded(unsigned int,GRT::ClassificationSample)), this, SLOT(addNewTrainingSample(unsigned int,GRT::ClassificationSample)));
    connect(&core, SIGNAL(trainMessageReceived()), ui->trainingTool_trainButton, SLOT(click()));
    connect(&core, SIGNAL(trainingDataReset(GRT::ClassificationData)), this, SLOT(resetTrainingData(GRT::ClassificationData)));
    connect(&core, SIGNAL(trainingDataReset(GRT::RegressionData)), this, SLOT(resetTrainingData(GRT::RegressionData)));
    connect(&core, SIGNAL(testDataReset(GRT::ClassificationData)), this, SLOT(resetTestData(GRT::ClassificationData)));
    connect(&core, SIGNAL(preProcessingDataChanged(GRT::VectorDouble)), this, SLOT(updatePreProcessingData(GRT::VectorDouble)));
    connect(&core, SIGNAL(featureExtractionDataChanged(GRT::VectorDouble)), this, SLOT(updateFeatureExtractionData(GRT::VectorDouble)));
    connect(&core, SIGNAL(predictionResultsChanged(unsigned int,double,GRT::VectorDouble,GRT::VectorDouble,std::vector<unsigned int>)), this, SLOT(updatePredictionResults(unsigned int,double,GRT::VectorDouble,GRT::VectorDouble,std::vector<unsigned int>)));
    connect(&core, SIGNAL(regressionResultsChanged(GRT::VectorDouble)), this, SLOT(updateRegressionResults(GRT::VectorDouble)));
    connect(&core, SIGNAL(pipelineTrainingStarted()), this, SLOT(pipelineTrainingStarted()));
    connect(&core, SIGNAL(pipelineTrainingFinished(bool)), this, SLOT(pipelineTrainingFinished(bool)));
    connect(&core, SIGNAL(pipelineTestingFinished(bool)), this, SLOT(pipelineTestingFinished(bool)));
    connect(&core, SIGNAL(setClassifierMessageReceived(unsigned int,bool,bool,double,double)), this, SLOT(updateClassifier(unsigned int,bool,bool,double,double)));

    //Setup the keyboard shortcuts
    QShortcut *ctrlRShortcut = new QShortcut( QKeySequence( QString::fromStdString("Ctrl+R") ), this);
    QObject::connect(ctrlRShortcut, SIGNAL(activated()), this, SLOT(ctrlRShortcut()));

    QShortcut *ctrlSShortcut = new QShortcut( QKeySequence( QString::fromStdString("Ctrl+S") ), this);
    QObject::connect(ctrlSShortcut, SIGNAL(activated()), this, SLOT(ctrlSShortcut()));

    QShortcut *ctrlLShortcut = new QShortcut( QKeySequence( QString::fromStdString("Ctrl+L") ), this);
    QObject::connect(ctrlLShortcut, SIGNAL(activated()), this, SLOT(ctrlLShortcut()));

    return true;
}

void MainWindow::updateMainView(int tabIndex){

    switch( tabIndex ){
        case SETUP_VIEW:
            showSetupView();
        break;
        case DATA_IO_VIEW:
        break;
        case DATA_MANAGER_VIEW:
        break;
        case PIPELINE_VIEW:
        break;
        case TRAINING_VIEW:
        break;
        case PREDICTION_VIEW:
        break;
        case LOG_VIEW:
        break;
    }
}

void MainWindow::showVersionInfo(){
    VersionInfo *window = new VersionInfo();
    GRT::GRTBase base;
    window->setFixedSize( 400, 300 );
    window->setWindowTitle( "Version Info" );
    window->setVersionText( GRT_GUI_VERSION );
    window->setRevisionText( QString::fromStdString( base.getGRTRevison() ) );
    window->setDevelopersText( "Nicholas Gillian" );
    window->show();
}

void MainWindow::showSetupView(){

    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            ui->setupView_classificationModeButton->setChecked( true );
            ui->setupView_regressionModeButton->setChecked( false );
            ui->setupView_timeseriesModeButton->setChecked( false );
        break;
        case Core::REGRESSION_MODE:
            ui->setupView_classificationModeButton->setChecked( false );
            ui->setupView_regressionModeButton->setChecked( true );
            ui->setupView_timeseriesModeButton->setChecked( false );
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
            ui->setupView_classificationModeButton->setChecked( false );
            ui->setupView_regressionModeButton->setChecked( false );
            ui->setupView_timeseriesModeButton->setChecked( true );
        break;
        default:
            return;
        break;
    }
}

void MainWindow::showDataIOView(){
    updateInfoText( "" );
}

void MainWindow::showDataLabellingToolView(){
    updateInfoText( "" );
}

void MainWindow::showPipelineToolView(){
    updateInfoText( "" );
}

void MainWindow::showTrainingToolView(){
    updateInfoText( "" );
    resetTrainingToolView( ui->trainingTool_trainingMode->currentIndex() );
}

void MainWindow::showPredictionView(){
    updateInfoText( "" );
}

void MainWindow::showLogView(){
    updateInfoText( "" );
}

void MainWindow::updateInfoText(std::string msg){
    ui->mainWindow_infoTextField->setText( QString::fromStdString( msg ) );
    QPalette p = ui->mainWindow_infoTextField->palette();
    p.setColor(QPalette::Text, Qt::black);
    ui->mainWindow_infoTextField->setPalette(p);
    core.setInfoMessage( msg );

    if( msg.length() > 0 ){
        GRT::TimeStamp ts;
        ts.setTimeStampAsNow();
        std::string t = "[";
        t += GRT::Util::toString(ts.hour) + ":" + GRT::Util::toString(ts.minute) + ":" + GRT::Util::toString(ts.second);
        t += "] ";
        ui->logView_infoLogScrollView->append( QString::fromStdString( t + msg ) );
        ui->logView_allLogsScrollView->append( QString::fromStdString( t + msg ) );
    }
}

void MainWindow::updateWarningText(std::string msg){
    ui->mainWindow_infoTextField->setText( QString::fromStdString( msg ) );
    QPalette p = ui->mainWindow_infoTextField->palette();
    p.setColor(QPalette::Text, QColor(200,100,20));
    ui->mainWindow_infoTextField->setPalette(p);
    core.setInfoMessage( msg );

    if( msg.length() > 0 ){
        GRT::TimeStamp ts;
        ts.setTimeStampAsNow();
        std::string t = "[";
        t += GRT::Util::toString(ts.hour) + ":" + GRT::Util::toString(ts.minute) + ":" + GRT::Util::toString(ts.second);
        t += "] ";
        ui->logView_warningLogScrollView->append( QString::fromStdString( t + msg ) );
        ui->logView_allLogsScrollView->append( QString::fromStdString( t + msg ) );
    }
}

void MainWindow::updateErrorText(std::string msg){

    if( msg.length() > 0 ){
        GRT::TimeStamp ts;
        ts.setTimeStampAsNow();
        std::string t = "[";
        t += GRT::Util::toString(ts.hour) + ":" + GRT::Util::toString(ts.minute) + ":" + GRT::Util::toString(ts.second);
        t += "] ";
        ui->logView_errorLogScrollView->append( QString::fromStdString( t + msg ) );
        ui->logView_allLogsScrollView->append( QString::fromStdString( t + msg ) );
    }

    QMessageBox::warning(0, "Error", QString::fromStdString( msg ));
    core.setInfoMessage( msg );
}

void MainWindow::updateHelpText(std::string msg){
    QMessageBox::information(0, "Information", QString::fromStdString( msg ));
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// SETUP VIEW FUNCTIONS //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::showSetupViewInfo(){
    QString infoText;
    infoText += "This is the Setup view. It lets you configure your project.\n\n";
    infoText += "There are two things you should do in this view. \n\n";
    infoText += "[1] Select if your machine learning task is a classification, regression, or timeseries task.\n";
    infoText += "[2] Set the number of inputs to your task, and the number of outputs. \n\n";
    infoText += "Checkout the main GUI wiki for me info: www.nickgillian.com/wiki/pmwiki.php/GRT/GUI \n\n";
    QMessageBox::information(0, "Information", infoText);
}

void MainWindow::setNumInputs(int numInputs){
    core.setNumInputDimensions( numInputs );
}

void MainWindow::setNumOutputs(int numOutputs){

    //Make sure the number of outputs is limited to 1 if we are not in regression mode
    if( core.getPipelineMode() == Core::REGRESSION_MODE ) core.setTargetVectorSize( numOutputs );
    else{
        ui->setupView_numOutputsSpinBox->setValue( 1 );
        QString infoText;
        infoText += "If your project is in classification mode or timeseries classification mode, then the number of outputs will be 1.\n\n";
        QMessageBox::information(0, "Information", infoText);
    }
}

void MainWindow::setPipelineModeAsClassificationMode(){
    core.setPipelineMode( Core::CLASSIFICATION_MODE );
    core.setNumInputDimensions( ui->setupView_numInputsSpinBox->value() );
    resetDefaultPipelineClassificationSetup();
}

void MainWindow::setPipelineModeAsRegressionMode(){
    core.setPipelineMode( Core::REGRESSION_MODE );
    core.setNumInputDimensions( ui->setupView_numInputsSpinBox->value() );
    core.setTargetVectorSize( ui->setupView_numOutputsSpinBox->value() );
    resetDefaultPipelineRegressionSetup();
}

void MainWindow::setPipelineModeAsTimeseriesMode(){
    //core.setPipelineMode( Core::TIMESERIES_CLASSIFICATION_MODE );

    QString infoText;
    infoText += "Sorry, the timeseries mode is not supported yet. This will be added soon!\n";
    QMessageBox::information(0, "Information", infoText);

    //core.setPipelineMode( Core::CLASSIFICATION_MODE );
}

void MainWindow::updatePipelineMode(unsigned int pipelineMode){

    //Remove all the tabs from the dataLabellingTool
    while( ui->dataLabelingTool_trainingDataTab->count() > 0 ){
        ui->dataLabelingTool_trainingDataTab->removeTab(0);
    }

    //Remove all the tabs from the trainingTool
    while( ui->trainingTool_resultsTab->count() > 0 ){
        ui->trainingTool_resultsTab->removeTab(0);
    }

    switch( pipelineMode ){
        case Core::CLASSIFICATION_MODE:
            setupDefaultClassifier();
            if( !ui->setupView_classificationModeButton->isChecked() && !ui->setupView_classificationModeButton->isDown() ){
                ui->setupView_classificationModeButton->setChecked( true );
            }
            ui->setupView_regressionModeButton->setChecked( false );
            ui->setupView_timeseriesModeButton->setChecked( false );
            ui->dataLabellingTool_recordingControlsWidget->setCurrentIndex( CLASSIFICATION_VIEW );
            ui->predictionWindow_classificationRegressionResultsView->setCurrentIndex( CLASSIFICATION_VIEW );
            ui->dataIO_targetVectorSizeField->setText( QString::number( 0 ) );
            ui->dataIO_targetVectorSizeField->setEnabled( false );
            ui->dataIO_targetVectorSizeField->setVisible( false );
            ui->dataIO_targetVectorSizeLabel->setVisible( false );
            ui->dataLabellingTool_targetVectorTextField->setText("");
            ui->dataLabellingTool_numTargetDimensionsField->setText("");
            ui->pipelineTool_postProcessingStackedWidget->setCurrentIndex( 0 );
            ui->pipelineTool_postProcessingType->setCurrentIndex( 0 );

            //Add the tabs for classification
            ui->dataLabelingTool_trainingDataTab->insertTab( 0, dataLabelingToolTabHistory[1], "Dataset Stats" );
            ui->dataLabelingTool_trainingDataTab->insertTab( 1, dataLabelingToolTabHistory[0], "Table View" );
            ui->dataLabelingTool_trainingDataTab->insertTab( 2, dataLabelingToolTabHistory[2], "Class Counter" );
            ui->dataLabelingTool_trainingDataTab->insertTab( 3, dataLabelingToolTabHistory[3], "PCA Projection" );
            ui->dataLabelingTool_trainingDataTab->insertTab( 4, dataLabelingToolTabHistory[4], "Timeseries Graph" );
            ui->dataLabelingTool_trainingDataTab->setCurrentIndex( 0 );

            //Add the tabs for classification
            ui->trainingTool_resultsTab->insertTab( 0, trainingToolTabHistory[0], "Results" );
            ui->trainingTool_resultsTab->insertTab( 1, trainingToolTabHistory[1], "Precision" );
            ui->trainingTool_resultsTab->insertTab( 2, trainingToolTabHistory[2], "Recall" );
            ui->trainingTool_resultsTab->insertTab( 3, trainingToolTabHistory[3], "F-Measure" );
            ui->trainingTool_resultsTab->setCurrentIndex( 0 );
        break;
        case Core::REGRESSION_MODE:
            setupDefaultRegressifier();
            if( !ui->setupView_regressionModeButton->isChecked() && !ui->setupView_regressionModeButton->isDown() ){
                ui->setupView_regressionModeButton->setChecked( true );
            }
            ui->setupView_classificationModeButton->setChecked( false );
            ui->setupView_timeseriesModeButton->setChecked( false );
            ui->dataLabellingTool_recordingControlsWidget->setCurrentIndex( REGRESSION_VIEW );
            ui->predictionWindow_classificationRegressionResultsView->setCurrentIndex( REGRESSION_VIEW );
            ui->dataIO_targetVectorSizeField->setText( QString::number( ui->setupView_numOutputsSpinBox->value() ) );
            ui->dataLabellingTool_numTargetDimensionsField->setText( QString::number( ui->setupView_numOutputsSpinBox->value() ) );
            ui->dataIO_targetVectorSizeField->setEnabled( true );
            ui->dataIO_targetVectorSizeField->setVisible( true );
            ui->dataIO_targetVectorSizeLabel->setVisible( true );
            ui->pipelineTool_postProcessingStackedWidget->setCurrentIndex( 1 );
            ui->pipelineTool_postProcessingType_2->setCurrentIndex( 0 );

            //Add the tabs for regression
            ui->dataLabelingTool_trainingDataTab->insertTab( 0, dataLabelingToolTabHistory[1], "Dataset Stats" );
            ui->dataLabelingTool_trainingDataTab->insertTab( 1, dataLabelingToolTabHistory[0], "Table View" );
            ui->dataLabelingTool_trainingDataTab->insertTab( 2, dataLabelingToolTabHistory[3], "PCA Projection" );
            ui->dataLabelingTool_trainingDataTab->setCurrentIndex( 0 );

            //Add the tabs for regression
            ui->trainingTool_resultsTab->insertTab( 0, trainingToolTabHistory[0], "Results" );
            //ui->trainingTool_resultsTab->insertTab( 1, trainingToolTabHistory[4], "RMS" );
            ui->trainingTool_resultsTab->setCurrentIndex( 0 );
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
            setPipelineModeAsTimeseriesMode();
            /*
            if( !ui->setupView_timeseriesModeButton->isChecked() && !ui->setupView_timeseriesModeButton->isDown() ){
                ui->setupView_timeseriesModeButton->setChecked( true );
            }
            ui->setupView_classificationModeButton->setChecked( false );
            ui->setupView_regressionModeButton->setChecked( false );
            ui->dataLabellingTool_recordingControlsWidget->setCurrentIndex( CLASSIFICATION_VIEW );
            ui->predictionWindow_classificationRegressionResultsView->setCurrentIndex( CLASSIFICATION_VIEW );
            ui->dataIO_targetVectorSizeField->setText( QString::number( 0 ) );
            ui->dataIO_targetVectorSizeField->setEnabled( false );
            ui->dataIO_targetVectorSizeField->setVisible( false );
            ui->dataIO_targetVectorSizeLabel->setVisible( false );
            ui->dataLabellingTool_targetVectorTextField->setText("");
            */
        break;
        default:
            return;
        break;
    }
}

void MainWindow::resetAll(){

    //Remove all training data
    core.clearTrainingData();

    //Reset the pipeline setup to the default
    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            resetDefaultPipelineClassificationSetup();
        break;
        case Core::REGRESSION_MODE:
            resetDefaultPipelineRegressionSetup();
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
        break;
    }

    updateInfoText( "Pipeline Reset" );
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// DATA IO TOOL FUNCTIONS /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::showDataIOInfo(){
    QString infoText;
    infoText += "This is the Data I/O view. It lets you configure the OpenSoundControl inputs and outputs for your application.\n\n";
    infoText += "You should send your realtime data to the OSC input port (default 5000), using the OSC namespace '/Data'. \n\n";
    infoText += "Before you send any data, you should set the number of dimensions in your data. You can do this in the Setup View. \n";
    infoText += "The OSC Output Setup lets you control the ip address and port of where the realtime output of the application will be sent. \n\n";
    infoText += "Checkout the main GUI wiki for more info: www.nickgillian.com/wiki/pmwiki.php/GRT/GUI \n\n";
    QMessageBox::information(0, "Information", infoText);
}

void MainWindow::updateOSCInput(){
    bool state = ui->dataIO_enableOSCInputButton->isChecked();
    core.setEnableOSCInput( state );
}

void MainWindow::updateMouseInput(){

    bool enableMouseInput = ui->dataIO_enableMouseInputButton->isChecked();

    if( enableMouseInput ){
        //Set the number of inputs to 2 (for the [x y] values of the mouse)
        setNumInputs( 2 );
    }else{

    }

}

void MainWindow::updateOSCControlCommands(){
    bool state = ui->dataIO_enableOSCCommandsButton->isChecked();
    core.setEnableOSCControlCommands( state );
}

void MainWindow::resetOSCServer(int port){
    core.resetOSCServer( port );
}

void MainWindow::resetOSCClient(){
    int port = ui->dataIO_oscOutgoingPortSpinBox->value();
    string address = ui->dataIO_clientIPAddressTextField->text().toUtf8().constData();
    core.resetOSCClient(address,port);
}

void MainWindow::updateDataAddress(){
    string address = ui->dataIO_mainDataAddressTextField->text().toUtf8().constData();
    core.setMainDataAddress( address );
}

void MainWindow::updateNumInputDimensions(int numInputDimensions){
    ui->setupView_numInputsSpinBox->setValue( numInputDimensions );
    ui->dataIO_numInputDimensionsField->setText( QString::number( numInputDimensions ) );
    ui->dataLabellingTool_numInputDimensionsField->setText( QString::number( numInputDimensions ) );
    ui->dataLabellingTool_numInputDimensionsField_2->setText( QString::number( numInputDimensions ) );

    int numHiddenNeurons = (int)max((numInputDimensions + ui->setupView_numOutputsSpinBox->value()) / 2,2) ;
    ui->pipelineTool_mlpNumHiddenNeurons->setValue( numHiddenNeurons );

    ui->pipelineTool_swipeDetector_swipeIndex->setMaximum( numInputDimensions-1 );

    inputDataGraph->init(numInputDimensions,DEFAULT_GRAPH_WIDTH);

    updateInfoText( "" );
}

void MainWindow::updateNumTargetDimensions(int numTargetDimensions){

    //Reset the setupview
    ui->setupView_numOutputsSpinBox->setValue ( numTargetDimensions );

    //Reset the dataIO view
    ui->dataIO_targetVectorSizeField->setText( QString::number( numTargetDimensions ) );

    //Reset the labelling tool
    ui->dataLabellingTool_targetVectorDimensionSpinBox->setMinimum(0);
    ui->dataLabellingTool_targetVectorDimensionSpinBox->setMaximum(numTargetDimensions-1);
    ui->dataLabellingTool_targetVectorDimensionSpinBox->setValue(0);
    ui->dataLabellingTool_targetVectorValueSpinBox->setValue( 0 );
    ui->dataLabellingTool_targetVectorValueSpinBox->setMinimum( - numeric_limits<double>::max() );
    ui->dataLabellingTool_targetVectorValueSpinBox->setMaximum( numeric_limits<double>::max() );

    //Reset the pipeline tool
    int numHiddenNeurons = (int)max((ui->setupView_numInputsSpinBox->value() + numTargetDimensions) / 2,2);
    ui->pipelineTool_mlpNumHiddenNeurons->setValue( numHiddenNeurons );

    updateInfoText( "" );
}

void MainWindow::updateOSCMessageLog(std::string msg){
    ui->dataIO_OSCMessageLog->appendPlainText( QString::fromUtf8( msg.c_str() ) );
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// DATA LABELING TOOL FUNCTIONS /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::recordTrainingData(bool buttonState){
    core.setRecordingState( buttonState );
}

void MainWindow::saveTrainingDatasetToFile(){
    QDir directory;
    QString filenameAndPath = QFileDialog::getSaveFileName(this, tr("Enter filename"), directory.path());
    std::string filename = filenameAndPath.toUtf8().constData();
    core.saveTrainingDatasetToFile( filename );
}

void MainWindow::loadTrainingDatasetFromFile(){
    QDir directory;
    QString filenameAndPath = QFileDialog::getOpenFileName(this, tr("Enter filename"), directory.path());
    std::string filename = filenameAndPath.toUtf8().constData();
    core.loadTrainingDatasetFromFile( filename );
}

void MainWindow::updateTrainingClassLabel(unsigned int trainingClassLabel){
    ui->dataLabellingTool_classLabel->setValue( trainingClassLabel );
}

void MainWindow::updateTargetVectorValue(double value){

    //Get the current target vector from the core
    GRT::VectorDouble targetVector = core.getTargetVector();

    //Update the target vector with the new value
    int index = ui->dataLabellingTool_targetVectorDimensionSpinBox->value();
    targetVector[ index ] = value;

    //Update the core
    core.setTargetVector( targetVector );
}

void MainWindow::updateRecordStatus(bool recordStatus){

    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            ui->dataLabellingTool_classificationModeRecordButton->setChecked( recordStatus );
        break;
        case Core::REGRESSION_MODE:
            ui->dataLabellingTool_regressionModeRecordButton->setChecked( recordStatus );
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
        //todo
        break;
    }
}

void MainWindow::updateNumTrainingSamples(unsigned int numTrainingSamples){

    ui->dataLabellingTool_numTrainingSamples->setText( QString::number( numTrainingSamples ) );
    ui->dataLabellingTool_numTrainingSamples_2->setText( QString::number( numTrainingSamples ) );

    ui->dataLabellingTool_numClassesField->setText( QString::number( core.getNumClassesInTrainingData() ) );

    if( numTrainingSamples > 0 ){
        resetTrainingToolView( ui->trainingTool_trainingMode->currentIndex() );
    }

    //If the user is looking at the data labelling view then update the training table view
    if( getCurrentView() == DATA_MANAGER_VIEW ){
        updateTrainingTabView( ui->dataLabelingTool_trainingDataTab->currentIndex() );
    }
}

void MainWindow::addNewTrainingSample(unsigned int numTrainingSamples,GRT::ClassificationSample trainingSample){

}

void MainWindow::resetTrainingData(GRT::ClassificationData trainingData){

    ui->dataLabellingTool_classificationDatasetName->setText( QString::fromStdString( trainingData.getDatasetName() ) );
    ui->dataLabellingTool_classificationDataInfoTextField->setText( QString::fromStdString( trainingData.getInfoText() ) );
    ui->dataLabellingTool_numClassesField->setText( QString::number( trainingData.getNumClasses() ) );

    ui->dataLabellingTool_treeView->setRootIsDecorated(false);
    ui->dataLabellingTool_treeView->setAlternatingRowColors(true);
    ui->dataLabellingTool_treeView->clearSelection();

    const unsigned int N = trainingData.getNumDimensions();
    const unsigned int M = trainingData.getNumSamples();

    model = new QStandardItemModel(0, 2+N, this);

    //Add the headers
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Sample Index"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Class Label"));

    for(unsigned int n=0; n<N; n++){
        std::string colName = "Feature " + GRT::Util::intToString(n+1);
        model->setHeaderData(2+n, Qt::Horizontal, QString::fromStdString(colName) );
    }

    //Add the data
    for(unsigned int i=0; i<M; i++){
        model->insertRow(i);
        model->setData(model->index(i, 0), i);
        model->setData(model->index(i, 1), trainingData[i].getClassLabel());
        for(unsigned int n=0; n<N; n++){
            model->setData(model->index(i, 2+n), trainingData[i][n] );
        }
    }

    ui->dataLabellingTool_treeView->setModel( model );

    resetTrainingToolView( ui->trainingTool_trainingMode->currentIndex() );
}

void MainWindow::resetTrainingData(GRT::RegressionData trainingData){

    ui->dataLabellingTool_regressionDatasetName->setText( QString::fromStdString( trainingData.getDatasetName() ) );
    ui->dataLabellingTool_regressionDataInfoTextField->setText( QString::fromStdString( trainingData.getInfoText() ) );

    ui->dataLabellingTool_treeView->setRootIsDecorated(false);
    ui->dataLabellingTool_treeView->setAlternatingRowColors(true);
    ui->dataLabellingTool_treeView->clearSelection();

    const unsigned int N = trainingData.getNumInputDimensions();
    const unsigned int T = trainingData.getNumTargetDimensions();
    const unsigned int M = trainingData.getNumSamples();

    model = new QStandardItemModel(0, 1+N+T, this);

    //Add the headers
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Sample Index"));

    for(unsigned int n=0; n<N; n++){
        std::string colName = "Feature " + GRT::Util::intToString(n+1);
        model->setHeaderData(1+n, Qt::Horizontal, QString::fromStdString(colName) );
    }

    for(unsigned int t=0; t<T; t++){
        std::string colName = "Target " + GRT::Util::intToString(t+1);
        model->setHeaderData(1+N+t, Qt::Horizontal, QString::fromStdString(colName) );
    }

    //Add the data
    for(unsigned int i=0; i<M; i++){
        model->insertRow(i);
        model->setData(model->index(i, 0), i);
        for(unsigned int n=0; n<N; n++){
            model->setData(model->index(i, 1+n), trainingData[i].getInputVector()[n] );
        }
        for(unsigned int t=0; t<T; t++){
            model->setData(model->index(i, 1+N+t), trainingData[i].getTargetVector()[t] );
        }
    }

    ui->dataLabellingTool_treeView->setModel( model );

    resetTrainingToolView( ui->trainingTool_trainingMode->currentIndex() );
}

void MainWindow::handleDatasetClicked(const QModelIndex &index){

    //QStandardItem *item = model->itemFromIndex(index);

    //QVariant v = item->data();
    //qDebug() << "v: " << v;
}

void MainWindow::resetTestData(GRT::ClassificationData testData){
    ui->trainingTool_numTestSamples->setText( QString::number( testData.getNumSamples() ) );
}

void MainWindow::showDataLabellingToolInfo(){
    QString infoText;
    infoText += "This is the Data Labeling view. It lets you record, label, and manage your training and testing data.\n\n";

    infoText += "Checkout the main GUI wiki for more info: www.nickgillian.com/wiki/pmwiki.php/GRT/GUI \n\n";
    QMessageBox::information(0, "Information", infoText);
}

void MainWindow::updateDatasetName(){

    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            core.setDatasetName( ui->dataLabellingTool_classificationDatasetName->text().toStdString() );
        break;
        case Core::REGRESSION_MODE:
            core.setDatasetName( ui->dataLabellingTool_regressionDatasetName->text().toStdString() );
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
            //TODO
        break;
    }

}

void MainWindow::updateDatasetInfoText(){

    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            core.setDatasetInfoText( ui->dataLabellingTool_classificationDataInfoTextField->text().toStdString() );
        break;
        case Core::REGRESSION_MODE:
            core.setDatasetInfoText( ui->dataLabellingTool_regressionDataInfoTextField->text().toStdString() );
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
            //TODO
        break;
    }
}

void MainWindow::updateTrainingTabView(int tabIndex){

    QString infoText;

    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            if( tabIndex == 0 ){
                updateDatasetStatsView();
            }
            if( tabIndex == 1 ){
                resetTrainingData( core.getClassificationTrainingData() );
            }
            if( tabIndex == 2 ){
                updateClassStatsGraph();
            }
            if( tabIndex == 3 ){
                infoText += "Sorry, the PCA plot is currently disabled due to a plotting bug, this will fixed in a future release!\n";
                QMessageBox::information(0, "Information", infoText);
                //updatePCAProjectionGraph();
            }
            if( tabIndex == 4 ){
                updateTimeseriesGraph();
            }
        break;
        case Core::REGRESSION_MODE:
            if( tabIndex == 0 ){
                updateDatasetStatsView();
            }
            if( tabIndex == 1 ){
                resetTrainingData( core.getRegressionTrainingData() );
            }
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
        break;
    }

}

void MainWindow::updateDatasetStatsView(){

    GRT::ClassificationData classificationData;
    GRT::RegressionData regressionData;

    string statsText;

    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            classificationData = core.getClassificationTrainingData();
            statsText = classificationData.getStatsAsString();
        break;
        case Core::REGRESSION_MODE:
            regressionData = core.getRegressionTrainingData();
            statsText = regressionData.getStatsAsString();
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
        break;
    }

    ui->dataLabelingTool_datasetStatsView->setText( QString::fromStdString(statsText) );

}

void MainWindow::updateClassStatsGraph(){

    QCustomPlot *plot = ui->dataLabelingTool_classStatsGraph;
    GRT::ClassificationData trainingData = core.getClassificationTrainingData();
    unsigned int K = trainingData.getNumClasses();
    vector< unsigned int > classLabels = trainingData.getClassLabels();
    vector< GRT::ClassTracker > classTracker = trainingData.getClassTracker();
    QVector<double> keyData;
    QVector<double> valueData;
    QVector<double> tickVector;
    QVector<QString> tickLabels;

    //Clear any previous graphs
    plot->clearPlottables();
    plot->clearGraphs();

    //Add a new bar graph
    QCPBars *bar = new QCPBars(plot->xAxis, plot->yAxis);
    plot->addPlottable( bar );

    //Add the data to the graph
    for(unsigned int k=0; k<K; k++){
        keyData << classLabels[k];
        valueData << classTracker[k].counter;
    }
    bar->setData(keyData, valueData);

    //Add the tick labels
    for(unsigned int k=0; k<K; k++){
        tickVector << double(k+1);
        tickLabels << QString::fromStdString( GRT::Util::intToString( classLabels[k]) );
    }
    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setAutoTickLabels(false);
    plot->xAxis->setTickVector( tickVector );
    plot->xAxis->setTickVectorLabels( tickLabels );
    plot->xAxis->setLabel("Class Label");
    plot->rescaleAxes();
    plot->replot();
}


void MainWindow::updatePCAProjectionGraph(){

    QCustomPlot *plot = ui->dataLabelingTool_pcaProjectionPlot;

    //Clear any previous graphs
    plot->clearGraphs();

    return;

    //Get the training data
    GRT::ClassificationData trainingData = core.getClassificationTrainingData();
    GRT::MatrixDouble data = trainingData.getDataAsMatrixDouble();

    if( trainingData.getNumSamples() == 0 ){
        return;
    }

    if( trainingData.getNumDimensions() == 1 ){
        return;
    }

    //Setup the PCA
    GRT::PrincipalComponentAnalysis pca;
    const unsigned int maxNumPCs = 2;
    const bool normData = true;
    if( !pca.computeFeatureVector( data, maxNumPCs, normData ) ){
        errorLog << "Failed to compute PCA model!" << std::endl;
        return;
    }

    //Project the data
    GRT::MatrixDouble prjData;
    if( !pca.project(data,prjData) ){
        errorLog << "Failed to project data!" << std::endl;
        return;
    }

    if( prjData.getNumRows() != data.getNumRows() ){
        errorLog << "Rows != M" << std::endl;
        return;
    }
    if( prjData.getNumCols() != 2 ){
        errorLog << "Cols != 2" << std::endl;
        return;
    }

    const unsigned int M = data.getNumRows();
    const unsigned int K = trainingData.getNumClasses();
    vector< GRT::MinMax > ranges = prjData.getRanges();
    vector< unsigned int > classLabels = trainingData.getClassLabels();
    vector< GRT::ClassTracker > classTracker = trainingData.getClassTracker();
    GRT::Random random;

    vector< QColor > classColors(K);
    if( K >=1 ) classColors[0] = QColor(255, 0, 0, 200);
    if( K >=2 ) classColors[1] = QColor(0, 255, 0, 200);
    if( K >=3 ) classColors[2] = QColor(0, 0, 255, 200);
    for(unsigned int k=3; k<K; k++){
        classColors[k] = QColor(random.getRandomNumberInt(0,255), random.getRandomNumberInt(0,255), random.getRandomNumberInt(0,255), 200);
    }

    //Add a new bar graph for each class
    for(unsigned int k=0; k<K; k++){
        const unsigned int numSamplesInClass = classTracker[k].counter;

        cout << "NumSamples: " << numSamplesInClass << endl;

        if( numSamplesInClass == 0 ){
            plot->clearGraphs();
            errorLog << "Failed to plot data! Class " << classLabels[k] << " has no samples!" << std::endl;
            return;
        }

        QVector< double > x( numSamplesInClass );
        QVector< double > y( numSamplesInClass );
        plot->addGraph();
        plot->graph(k)->setPen( QPen( classColors[k] ) );
        plot->graph(k)->setBrush( QBrush( classColors[k] ) );
        plot->graph(k)->setLineStyle( QCPGraph::lsNone );
        plot->graph(k)->setScatterStyle( QCPScatterStyle::ssCross );

        unsigned int index = 0;
        for(unsigned int i=0; i<M; i++)
        {
            if( trainingData[i].getClassLabel() == classTracker[k].classLabel ){
                x[ index ] = prjData[i][0];
                y[ index ] = prjData[i][1];
                index++;
            }
        }

        // pass data points to graphs:
        plot->graph( k )->setData(x, y);
    }

    plot->xAxis->setVisible( true );
    plot->xAxis->setTickLabels( false );
    plot->yAxis->setVisible( true );
    plot->yAxis->setTickLabels( false );

    //plot->setTitle("PCA Projection");
    plot->rescaleAxes();
    plot->replot();
}

void MainWindow::updateTimeseriesGraph(){

    GRT::ClassificationData classificationData = core.getClassificationTrainingData();
    const unsigned int numDimensions = classificationData.getNumDimensions();
    const unsigned int graphWidth = classificationData.getNumSamples();
    bool lockRanges = false;
    GRT::MatrixDouble data(graphWidth,numDimensions);
    vector< unsigned int > labels(graphWidth);
    double minRange = numeric_limits< double >::max();
    double maxRange = numeric_limits< double >::min();
    double minLabel = numeric_limits< double >::max();
    double maxLabel = numeric_limits< double >::min();
    vector< Qt::GlobalColor > &colors = defaultGraphColors;

    //Get the data and the class labels
    for(unsigned int i=0; i<graphWidth; i++){
        labels[i] = classificationData[i].getClassLabel();
        for(unsigned int j=0; j<numDimensions; j++){
            data[i][j] = classificationData[i][j];
        }
    }

    //Get the data to plot
    QVector< double > x( graphWidth );
    vector< QVector<double> > y(numDimensions, QVector<double>(graphWidth) );
    QVector<double> z(graphWidth);

    for (unsigned int i=0; i<graphWidth; i++){
      x[i] = i;
      z[i] = labels[i];
      if( z[i] < minLabel ) minLabel = z[i];
      else if( z[i] > maxLabel ) maxLabel = z[i];
      for(unsigned int j=0; j<numDimensions; j++){
          y[j][i] = data[i][j];
          if( !lockRanges ){
            if( data[i][j] < minRange ) minRange = data[i][j];
            else if( data[i][j] > maxRange ) maxRange = data[i][j];
          }
      }
    }

    //Plot the data timeseries
    QCustomPlot *plot = ui->dataLabelingTool_timeseriesDataGraph;

    //Clear any previous graphs
    plot->clearGraphs();

    //Create the graphs
    for(unsigned int j=0; j<numDimensions; j++){
        plot->addGraph();
        plot->graph(j)->setPen( QPen( colors[j%colors.size()] ));
        plot->graph(j)->setData(x, y[j]);
    }

    // give the axes some labels:
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("Data");
    plot->yAxis->setAutoTickStep(false);
    plot->yAxis->setTickStep( (maxRange-minRange) / 5 );

    // set axes ranges, so we see all data:
    plot->xAxis->setRange(0, graphWidth);
    plot->yAxis->setRange(minRange, maxRange);
    plot->replot();


    ////////////////// Plot the labels timeseries //////////////////
    plot = ui->dataLabelingTool_timeseriesLabelGraph;

    //Clear any previous graphs
    plot->clearGraphs();

    //Create the graphs
    plot->addGraph();
    plot->graph(0)->setPen( QPen( colors[0] ));
    plot->graph(0)->setData(x, z);

    // give the axes some labels:
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("Label");
    plot->yAxis->setAutoTickStep(false);
    plot->yAxis->setTickStep(1);

    // set axes ranges, so we see all data:
    plot->xAxis->setRange(0, graphWidth);
    plot->yAxis->setRange(minLabel, maxLabel);
    plot->replot();
}

void MainWindow::ctrlRShortcut(){

    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            ui->dataLabellingTool_classificationModeRecordButton->click();
        break;
        case Core::REGRESSION_MODE:
            ui->dataLabellingTool_regressionModeRecordButton->click();
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
        break;
        default:
            return;
        break;
    }

    return;
}

void MainWindow::ctrlSShortcut(){

    ui->dataLabellingTool_saveButton->click();

    return;
}

void MainWindow::ctrlLShortcut(){

    ui->dataLabellingTool_loadButton->click();

    return;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// TRAINING TOOL FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::resetTrainingToolView( int trainingMode ){

    unsigned int numTrainingSamples = core.getNumTrainingSamples();
    unsigned int numTestSamples = core.getNumTestSamples();
    unsigned int temp = 0;

    ui->trainingTool_trainButton->setEnabled( false );
    ui->trainingTool_numTrainingSamples->setEnabled( false );
    ui->trainingTool_numTestSamples->setEnabled( false );
    ui->trainingTool_randomTestPercentageSlider->setEnabled( false );
    ui->trainingTool_randomTestPercentageBox->setEnabled( false );
    ui->trainingTool_loadExternalTestDatasetButton->setEnabled( false );
    ui->trainingTool_numCVFolds->setEnabled( false );
    ui->trainingTool_trainingMode->setEnabled( false );
    ui->trainingTool_results->setEnabled( false );
    ui->trainingTool_validationModeSettingsView->setCurrentIndex( trainingMode );

    if( numTrainingSamples == 0 ){
        ui->trainingTool_numTrainingSamples->setText( "0" );
        ui->trainingTool_numTestSamples->setText( "0" );
    }else{
        ui->trainingTool_trainButton->setEnabled( true );
        ui->trainingTool_trainingMode->setEnabled( true );
        ui->trainingTool_numTrainingSamples->setEnabled( true );

        switch( trainingMode ){
            case 0: //No Validation
                ui->trainingTool_numTrainingSamples->setText( QString::number( numTrainingSamples ));
                ui->trainingTool_numTestSamples->setText( "0" );
            break;
            case 1: //Random Subset
                ui->trainingTool_numTestSamples->setEnabled( true );
                ui->trainingTool_randomTestPercentageSlider->setEnabled( true );
                ui->trainingTool_randomTestPercentageBox->setEnabled( true );
                temp = (unsigned int)floor( numTrainingSamples * ui->trainingTool_randomTestPercentageSlider->value() / 100.0 );
                ui->trainingTool_numTrainingSamples->setText( QString::number( numTrainingSamples-temp ) );
                ui->trainingTool_numTestSamples->setText( QString::number( temp ) );
            break;
            case 2: //External Test Dataset
                ui->trainingTool_numTestSamples->setEnabled( true );
                ui->trainingTool_loadExternalTestDatasetButton->setEnabled( true );
                ui->trainingTool_numTrainingSamples->setText( QString::number( numTrainingSamples ) );
                ui->trainingTool_numTestSamples->setText( QString::number( numTestSamples ) );
            break;
            case 3://Cross Validation
                ui->trainingTool_numCVFolds->setEnabled( true );
                temp = (unsigned int)floor( numTrainingSamples / double( ui->trainingTool_numCVFolds->value() ) );
                ui->trainingTool_numTrainingSamples->setText( QString::number( numTrainingSamples - temp ));
                ui->trainingTool_numTestSamples->setText( QString::number( temp ) );
            break;
            default:
            break;
        }
    }

    //Clear the graphs
    clearPrecisionGraph();
    clearRecallGraph();
    clearFmeasureGraph();

}

void MainWindow::train(){

    //Refresh the pipeline setup to make sure all the settings are synced with the core
    refreshPipelineSetup();

    unsigned int trainingMode = ui->trainingTool_trainingMode->currentIndex();

    switch( trainingMode ){
        case 0://No Validation
            core.train();
        break;
        case 1://Random Subset
            core.trainAndTestOnRandomSubset( ui->trainingTool_randomTestPercentageSlider->value() );
        break;
        case 2://External Test Dataset
            core.trainAndTestOnTestDataset();
        break;
        case 3://Cross Validation
            QString value = ui->trainingTool_numCVFolds->text();
            core.trainWithCrossValidation( value.toInt() );
        break;
    }
}

void MainWindow::randomTestSliderMoved(int value){
    ui->trainingTool_randomTestPercentageBox->setText( QString::number( value ) + "%" );
    unsigned int M = core.getNumTrainingSamples();
    unsigned int N = (unsigned int)floor( M * value / 100.0 );
    ui->trainingTool_numTrainingSamples->setText( QString::number( M-N ) );
    ui->trainingTool_numTestSamples->setText( QString::number( N ) );
}

void MainWindow::numCVFoldsValueChanged(int value){
    unsigned int M = core.getNumTrainingSamples();
    unsigned int N = (unsigned int)floor( M / double( value ) );
    ui->trainingTool_numTrainingSamples->setText( QString::number( M-N ));
    ui->trainingTool_numTestSamples->setText( QString::number( N ) );
}

void MainWindow::loadTestDatasetFromFile(){
    QDir directory;
    QString filenameAndPath = QFileDialog::getOpenFileName(this, tr("Enter filename"), directory.path());
    std::string filename = filenameAndPath.toUtf8().constData();
    core.loadTestDatasetFromFile( filename );
}

void MainWindow::showTrainingToolInfo(){
    QString infoText;
    infoText += "This is the Data Labelling view. It lets you record, label, and manage your training data.\n\n";

    infoText += "Checkout the main GUI wiki for more info: www.nickgillian.com/wiki/pmwiki.php/GRT/GUI \n\n";
    QMessageBox::information(0, "Information", infoText);
}

void MainWindow::pipelineTrainingStarted(){

    clearPrecisionGraph();
    clearRecallGraph();
    clearFmeasureGraph();

    QString infoText;
    infoText += "---------------------------------------------------------------\n";
    infoText += "Training Started\n";
    infoText += "---------------------------------------------------------------";
    ui->trainingTool_results->setText( infoText );
    ui->trainingTool_results->setEnabled( true );



}

void MainWindow::pipelineTrainingFinished(bool result){

    qDebug() << "pipelineTrainingFinished(bool result)";

    boost::mutex::scoped_lock lock( mutex );

    if( !result ){
        QString infoText;
        infoText += "Training Failed!";
        ui->trainingTool_results->append( infoText );
        return;
    }

    QString infoText;
    vector< unsigned int > classLabels;
    unsigned int K;
    GRT::TestResult testResult;
    vector< GRT::TestResult > crossValidationResults;

    unsigned int pipelineMode = core.getPipelineMode();
    unsigned int trainingMode = ui->trainingTool_trainingMode->currentIndex();

    switch( trainingMode ){
        case 0://No Validation
            testResult = core.getTestResults();

            if( pipelineMode == Core::CLASSIFICATION_MODE || pipelineMode == Core::TIMESERIES_CLASSIFICATION_MODE ){
                infoText += "- No Validation.\n";
            }

            infoText += "---------------------------------------------------------------\n";
            infoText += "Training Complete\n";
            infoText += "---------------------------------------------------------------\n";

            infoText += "- Number of training samples:   ";
            infoText += QString::number( testResult.numTrainingSamples );
            infoText += "\n";

            infoText += "- Total training time:    ";
            infoText += QString::number( (testResult.trainingTime>0?testResult.trainingTime:1) );
            infoText += "ms\n";

            if( pipelineMode == Core::REGRESSION_MODE ){
                infoText += "- RMS Error: \t";
                infoText += QString::number( core.getTrainingRMSError() );
                infoText += "\n";

                infoText += "- SSE Error: \t";
                infoText += QString::number( core.getTrainingSSError() );
                infoText += "\n";
            }
        break;
        case 1://Random Subset
        case 2://External Test Dataset
            testResult = core.getTestResults();

            //If the number of test samples is zero then we have got the training complete status messge
            if( testResult.numTestSamples == 0 ){
                infoText += "---------------------------------------------------------------\n";
                infoText += "Training Complete\n";
                infoText += "---------------------------------------------------------------\n";

                infoText += "- Number of training samples:  ";
                infoText += QString::number( testResult.numTrainingSamples );
                infoText += "\n";

                infoText += "- Total training time:          ";
                infoText += QString::number( (testResult.trainingTime>0?testResult.trainingTime:1) );
                infoText += "ms\n";
                return;
            }

            infoText += "---------------------------------------------------------------\n";
            infoText += "Testing Complete\n";
            infoText += "---------------------------------------------------------------\n";

            infoText += "- Number of training samples:     ";
            infoText += QString::number( testResult.numTrainingSamples );
            infoText += "\n";

            infoText += "- Number of test samples:           ";
            infoText += QString::number( testResult.numTestSamples );
            infoText += "\n";

            infoText += "- Total training time:                   ";
            infoText += QString::number( (testResult.trainingTime>0?testResult.trainingTime:1) );
            infoText += "ms\n";

            infoText += "- Total test time:                         ";
            infoText += QString::number( (testResult.testTime>0?testResult.testTime:1) );
            infoText += "ms\n";

            if( pipelineMode == Core::CLASSIFICATION_MODE || pipelineMode == Core::TIMESERIES_CLASSIFICATION_MODE ){
                classLabels = core.getClassLabels();
                K = (unsigned int)classLabels.size();

                infoText += "- Accuracy:     ";
                infoText += QString::number( testResult.accuracy );
                infoText += "%\n";

                if( testResult.precision.size() == K ){
                    infoText += "- Precision:     ";
                    for(unsigned int i=0; i<K; i++){
                        infoText += "[" +  QString::number( classLabels[i] )  + "]: " + QString::number( testResult.precision[i] ) + "\t";
                    }
                    infoText += "\n";
                }
                updatePrecisionGraph( testResult.precision, classLabels );

                if( testResult.recall.size() == K ){
                    infoText += "- Recall:          ";
                    for(unsigned int i=0; i<K; i++){
                        infoText += "[" +  QString::number( classLabels[i] )  + "]: " + QString::number( testResult.recall[i] ) + "\t";
                    }
                    infoText += "\n";
                }
                updateRecallGraph( testResult.recall, classLabels );

                if( testResult.fMeasure.size() == K ){
                    infoText += "- F-Measure:  ";
                    for(unsigned int i=0; i<K; i++){
                        infoText += "[" +  QString::number( classLabels[i] )  + "]: " + QString::number( testResult.fMeasure[i] ) + "\t";
                    }
                    infoText += "\n";
                }
                updateFmeasureGraph( testResult.fMeasure, classLabels );

                infoText += "- Confusion Matrix: \n";
                for(unsigned int i=0; i<testResult.confusionMatrix.getNumRows(); i++){
                    for(unsigned int j=0; j<testResult.confusionMatrix.getNumCols(); j++){
                        infoText += QString::number( testResult.confusionMatrix[i][j] ) + "\t";
                    }
                    infoText += "\n";
                }
                infoText += "\n";

            }

            if( pipelineMode == Core::REGRESSION_MODE ){
                infoText += "- RMS Error: \t";
                infoText += QString::number( testResult.rmsError );
                infoText += "\n";

                infoText += "- SSE Error: \t";
                infoText += QString::number( testResult.totalSquaredError );
                infoText += "\n";


            }
        break;
        case 3://Cross Validation
            crossValidationResults = core.getCrossValidationResults();
            infoText += "---------------------------------------------------------------\n";
            infoText += "Cross Validation Testing Complete\n";
            infoText += "---------------------------------------------------------------\n";

            if( pipelineMode == Core::CLASSIFICATION_MODE || pipelineMode == Core::TIMESERIES_CLASSIFICATION_MODE ){
                classLabels = core.getClassLabels();
                K = (unsigned int)classLabels.size();

                cout << "K: " << crossValidationResults.size() << endl;
                for(unsigned int k=0; k<crossValidationResults.size(); k++){
                    infoText += "- Fold: " + QString::number( k+1 );
                    infoText += " Accuracy: \t";
                    infoText += QString::number( testResult.accuracy );
                    infoText += "%\n";
                }

                infoText += "- Cross Validation Accuracy: ";
                infoText += QString::number( core.getCrossValidationAccuracy() );
                infoText += "\n";
            }

            if( pipelineMode == Core::REGRESSION_MODE ){
                for(unsigned int k=0; k<crossValidationResults.size(); k++){
                    infoText += "- Fold: " + QString::number( k+1 );
                    infoText += "    RMS Error: \t";
                    infoText += QString::number( crossValidationResults[k].rmsError );

                    infoText += "    SSE Error: \t";
                    infoText += QString::number( crossValidationResults[k].totalSquaredError );
                    infoText += "\n";
                }

                infoText += "- Average RMS Error: ";
                infoText += QString::number( core.getCrossValidationAccuracy() );
                infoText += "\n";
            }

        break;
    }

    ui->trainingTool_results->append( infoText );

}

void MainWindow::pipelineTestingFinished(bool result){

    //qDebug() << "pipelineTestingFinished(bool result): " << result;

    if( !result ){
        QString infoText;
        infoText += "Testing Failed!";
        ui->trainingTool_results->append( infoText );
        return;
    }

    QString infoText;
    infoText += "Testing Complete!";
    ui->trainingTool_results->append( infoText );

}

void MainWindow::updateTrainingResults(const GRT::TrainingResult &trainingResult){

    //qDebug() << "updateTrainingResults(const GRT::TrainingResult &trainingResult,const GRT::MLBase *model)";

    QString infoText;

    infoText += "- Iteration: " + QString::number( trainingResult.getTrainingIteration() );

    if( trainingResult.getTrainingMode() == GRT::TrainingResult::CLASSIFICATION_MODE ){
        infoText += "\tAccuracy: " + QString::number( trainingResult.getAccuracy() );
    }else{
        infoText += "\tRMS: " + QString::number( trainingResult.getRootMeanSquaredTrainingError() );
        infoText += "\tSSE: " + QString::number( trainingResult.getTotalSquaredTrainingError() );
    }

    ui->trainingTool_results->append( infoText );
}

void MainWindow::updateTestResults(const GRT::TestInstanceResult &testResult){

    QString infoText;

    if( testResult.getTestIteration() == 0 ){
        infoText += "---------------------------------------------------------------\n";
        infoText += "Training Complete\n";
        infoText += "---------------------------------------------------------------\n";
        infoText += "---------------------------------------------------------------\n";
        infoText += "Starting Testing\n";
        infoText += "---------------------------------------------------------------\n";
    }

    infoText += "- Iteration: " + QString::number( testResult.getTestIteration() ) + "   ";

    if( testResult.getTestMode() == GRT::TestInstanceResult::CLASSIFICATION_MODE ){
        infoText += "ClassLabel: " + QString::number( testResult.getClassLabel() ) + "   ";
        infoText += "Predicted Class Label: " + QString::number( testResult.getPredictedClassLabel() ) + "   ";
        infoText += "Likelihood: " + QString::number( testResult.getMaximumLikelihood() ) + "   ";
    }else{
        infoText += "\tSquared Error: " + QString::number( testResult.getSquaredError() );
    }

    ui->trainingTool_results->append( infoText );

}

void MainWindow::updatePrecisionGraph(const GRT::VectorDouble &precision,const vector< unsigned int > &classLabels){

    QCustomPlot *plot = ui->trainingTool_precisionGraph;
    unsigned int K = (unsigned int)classLabels.size();

    QVector<double> keyData;
    QVector<double> valueData;
    QVector<double> tickVector;
    QVector<QString> tickLabels;

    //Clear any previous graphs
    plot->clearPlottables();

    //Add a new bar graph
    QCPBars *bar = new QCPBars(plot->xAxis, plot->yAxis);
    plot->addPlottable( bar );

    //Add the data to the graph
    for(unsigned int k=0; k<K; k++){
        keyData << classLabels[k];
        valueData << precision[k];
    }
    bar->setData(keyData, valueData);

    //Add the tick labels
    for(unsigned int k=0; k<K; k++){
        tickVector << double(k+1);
        tickLabels << QString::fromStdString( GRT::Util::intToString( classLabels[k]) );
    }
    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setAutoTickLabels(false);
    plot->xAxis->setTickVector( tickVector );
    plot->xAxis->setTickVectorLabels( tickLabels );
    plot->xAxis->setLabel("Class Labels");
    plot->yAxis->setRange(0, 1);

    //plot->setTitle("Precision results for each class");
    plot->rescaleAxes();
    plot->replot();
}

void MainWindow::updateRecallGraph(const GRT::VectorDouble &recall,const vector< unsigned int > &classLabels){

    QCustomPlot *plot = ui->trainingTool_recallGraph;
    unsigned int K = (unsigned int)classLabels.size();

    QVector<double> keyData;
    QVector<double> valueData;
    QVector<double> tickVector;
    QVector<QString> tickLabels;

    //Clear any previous graphs
    plot->clearPlottables();

    //Add a new bar graph
    QCPBars *bar = new QCPBars(plot->xAxis, plot->yAxis);
    plot->addPlottable( bar );

    //Add the data to the graph
    for(unsigned int k=0; k<K; k++){
        keyData << classLabels[k];
        valueData << recall[k];
    }
    bar->setData(keyData, valueData);

    //Add the tick labels
    for(unsigned int k=0; k<K; k++){
        tickVector << double(k+1);
        tickLabels << QString::fromStdString( GRT::Util::intToString( classLabels[k]) );
    }
    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setAutoTickLabels(false);
    plot->xAxis->setTickVector( tickVector );
    plot->xAxis->setTickVectorLabels( tickLabels );
    plot->xAxis->setLabel("Class Labels");
    plot->yAxis->setRange(0, 1);

    //plot->setTitle("Recall results for each class");
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(plot, "Recall results for each class"));
    plot->rescaleAxes();
    plot->replot();
}

void MainWindow::updateFmeasureGraph(const GRT::VectorDouble &fmeasure,const vector< unsigned int > &classLabels){

    QCustomPlot *plot = ui->trainingTool_fmeasureGraph;
    unsigned int K = (unsigned int)classLabels.size();

    QVector<double> keyData;
    QVector<double> valueData;
    QVector<double> xTickVector;
    QVector<double> yTickVector;
    QVector<QString> tickLabels;

    //Clear any previous graphs
    plot->clearPlottables();

    //Add a new bar graph
    QCPBars *bar = new QCPBars(plot->xAxis, plot->yAxis);
    plot->addPlottable( bar );

    //Add the data to the graph
    for(unsigned int k=0; k<K; k++){
        keyData << classLabels[k];
        valueData << fmeasure[k];
    }
    bar->setData(keyData, valueData);

    //Add the x tick labels
    for(unsigned int k=0; k<K; k++){
        xTickVector << double(k+1);
        tickLabels << QString::fromStdString( GRT::Util::intToString( classLabels[k]) );
    }

    //Add the y tick labels
    yTickVector << 0 << 0.2 << 0.4 << 0.6 << 0.8 << 1.0;

    plot->xAxis->setAutoTicks(false);
    plot->xAxis->setAutoTickLabels(false);
    //plot->yAxis->setAutoTicks(false);
    //plot->yAxis->setAutoTickLabels(false);
    plot->xAxis->setTickVectorLabels( tickLabels );
    plot->xAxis->setTickVector( xTickVector );
    //plot->yAxis->setTickVector( yTickVector );
    plot->xAxis->setLabel("Class Labels");

    //plot->setTitle("F-Measure results for each class");
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(plot, "F-Measure results for each class"));
    plot->rescaleAxes();
    plot->replot();
}

void MainWindow::clearPrecisionGraph(){
    QCustomPlot *plot = ui->trainingTool_precisionGraph;
    plot->clearPlottables();
}

void MainWindow::clearRecallGraph(){
    QCustomPlot *plot = ui->trainingTool_recallGraph;
    plot->clearPlottables();
}

void MainWindow::clearFmeasureGraph(){
    QCustomPlot *plot = ui->trainingTool_fmeasureGraph;
    plot->clearPlottables();
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// PIPELINE TOOl FUNCTIONS ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::showPipelineToolInfo(){
    QString infoText;
    infoText += "This is the Pipeline Tool view. It lets you setup, load, and save a custom gesture recognition pipeline.\n\n";

    infoText += "The first thing you should do is select the classification or regression algorithm that you would like to use.\n\n";

    infoText += "You can then add addition preprocessing, feature extraction, or postprocessing modules to support this if you need to.\n\n";

    infoText += "Checkout the main GUI wiki for more info: www.nickgillian.com/wiki/pmwiki.php/GRT/GUI \n\n";
    QMessageBox::information(0, "Information", infoText);
}

void MainWindow::updatePipelineFilename(){
    //QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
}

void MainWindow::savePipelineToFile(){
    QDir directory;
    QString filenameAndPath = QFileDialog::getSaveFileName(this, tr("Enter filename"), directory.path());
    std::string filename = filenameAndPath.toUtf8().constData();
    if( !core.savePipelineToFile( filename ) )
        emit updateInfoText( "WARNING: Failed to save pipeline." );
}

void MainWindow::loadPipelineFromFile(){
    QDir directory;
    QString filenameAndPath = QFileDialog::getOpenFileName(this, tr("Enter filename"), directory.path());
    std::string filename = filenameAndPath.toUtf8().constData();
    if( !core.loadPipelineFromFile( filename ) )
        emit updateInfoText( "WARNING: Failed to load pipeline." );
}

void MainWindow::updatePreProcessingView(int viewIndex){

    ui->pipelineTool_preProcessingOptionsView->setCurrentIndex( viewIndex );
    ui->pipelineTool_preProcessingType->setCurrentIndex( viewIndex );

    unsigned int numDimensions = ui->setupView_numInputsSpinBox->value();
    GRT::LowPassFilter lpf;
    GRT::HighPassFilter hpf;

    if( viewIndex == NO_PRE_PROCESSING_SELECTED ){
        ui->predictionWindow_plotPreProcessedDataButton->setEnabled( false );
        ui->predictionWindow_plotPreProcessedDataButton->setChecked( false );
    }else{
        ui->predictionWindow_plotPreProcessedDataButton->setEnabled( true );
        preProcessedDataGraph->init(numDimensions,DEFAULT_GRAPH_WIDTH);
    }

    switch( viewIndex ){
        case NO_PRE_PROCESSING_SELECTED:
            core.removeAllPreProcessingModules();
        break;
        case MOVING_AVERAGE_FILTER_PRE_PROCESSING:
            core.setPreProcessing( GRT::MovingAverageFilter( ui->pipelineTool_movingAverageFilterSizeSpinBox->value(), numDimensions ) );
        break;
        case DOUBLE_MOVING_AVERAGE_FILTER_PRE_PROCESSING:
            core.setPreProcessing( GRT::DoubleMovingAverageFilter( ui->pipelineTool_doubleMovingAverageFilterSizeSpinBox->value(), numDimensions ) );
        break;
        case LOW_PASS_FILTER_PRE_PROCESSING:
            lpf.init( 1, ui->pipelineTool_lowPassFilterGainSpinBox->value(), numDimensions );
            lpf.setCutoffFrequency( ui->pipelineTool_lowPassFilterCutoffFrequencySpinBox->value(), 1.0 / ui->pipelineTool_lowPassFilterSampleRateSpinBox->value() );
            core.setPreProcessing( lpf );
        break;
        case HIGH_PASS_FILTER_PRE_PROCESSING:
            hpf.init( 1, ui->pipelineTool_highPassFilterGainSpinBox->value(), numDimensions );
            hpf.setCutoffFrequency( ui->pipelineTool_highPassFilterCutoffFrequencySpinBox->value(), 1.0 / ui->pipelineTool_highPassFilterSampleRateSpinBox->value() );
            core.setPreProcessing( hpf );
        break;
        case DERIVATIVE_PRE_PROCESSING:
            if( ui->pipelineTool_derivativeOrderList->currentIndex() == 0 ) core.setPreProcessing( GRT::Derivative( GRT::Derivative::FIRST_DERIVATIVE, 1.0, numDimensions) );
            else core.setPreProcessing( GRT::Derivative( GRT::Derivative::SECOND_DERIVATIVE, 1.0, numDimensions) );
        break;
        case DEAD_ZONE_PRE_PROCESSING:
            core.setPreProcessing( GRT::DeadZone( ui->pipelineTool_deadZoneLowerLimitSpinBox->value(), ui->pipelineTool_deadZoneUpperLimitSpinBox->value(), numDimensions ) );
        break;
        default:
            updateInfoText( "WARNING: Unknown preprocessing mode!" );
        break;
    }
}

void MainWindow::updateFeatureExtractionView(int viewIndex){

    ui->pipelineTool_featureExtractionType->setCurrentIndex( viewIndex );

    if( viewIndex == NO_FEATURE_EXTRACTION_SELECTED ){
        ui->predictionWindow_plotFeatureExtractionDataButton->setEnabled( false );
        ui->predictionWindow_plotFeatureExtractionDataButton->setChecked( false );
    }else{
        ui->predictionWindow_plotFeatureExtractionDataButton->setEnabled( true );
        //featureExtractionDataGraph->init(numDimensions);
    }

}

void MainWindow::updateClassifierView(int viewIndex){

    ui->pipelineTool_classifierOptionsView->setCurrentIndex( viewIndex );
    ui->pipelineTool_classifierType->setCurrentIndex( viewIndex );

    GRT::ANBC anbc;
    GRT::AdaBoost adaBoost;
    GRT::DecisionTree decisionTree;
    GRT::GMM gmm;
    GRT::KNN knn;
    GRT::MinDist minDist;
    GRT::RandomForests randomForests;
    GRT::Softmax softMax;
    GRT::SVM svm;

    switch( viewIndex ){
        case CLASSIFIER_ANBC:
            anbc.enableScaling( ui->pipelineTool_enableScaling->isChecked() );
            anbc.enableNullRejection( ui->pipelineTool_enableNullRejection->isChecked() );
            anbc.setNullRejectionCoeff( ui->pipelineTool_nullRejectionCoeff->value() );
            core.setClassifier( anbc );
            break;
        case CLASSIFIER_ADABOOST:
            adaBoost.setWeakClassifier( GRT::DecisionStump() );
            adaBoost.setNumBoostingIterations( ui->pipelineTool_adaboostNumBoostingIterations->value() );
            core.setClassifier( adaBoost );
            break;
        case CLASSIFIER_DECISION_TREE:
            decisionTree.enableScaling( ui->pipelineTool_enableScaling->isChecked() );
            decisionTree.enableNullRejection( ui->pipelineTool_enableNullRejection->isChecked() );
            decisionTree.setNullRejectionCoeff( ui->pipelineTool_nullRejectionCoeff->value() );
            decisionTree.setMaxDepth( ui->pipelineTool_decisionTree_maxDepth->value() );
            decisionTree.setMinNumSamplesPerNode( ui->pipelineTool_decisionTree_minSamplesPerNode->value() );
            decisionTree.setNumSplittingSteps( ui->pipelineTool_decisionTree_numSpiltingSteps->value() );
            core.setClassifier( decisionTree );
            break;
        case CLASSIFIER_GMM:
            gmm.enableScaling( ui->pipelineTool_enableScaling->isChecked() );
            gmm.enableNullRejection( ui->pipelineTool_enableNullRejection->isChecked() );
            gmm.setNullRejectionCoeff( ui->pipelineTool_nullRejectionCoeff->value() );
            gmm.setNumMixtureModels( ui->pipelineTool_gmmNumMixtureModels->value() );
            core.setClassifier( gmm );
            break;
        case CLASSIFIER_KNN:
            knn.enableScaling( ui->pipelineTool_enableScaling->isChecked() );
            knn.enableNullRejection( ui->pipelineTool_enableNullRejection->isChecked() );
            knn.setNullRejectionCoeff( ui->pipelineTool_nullRejectionCoeff->value() );
            knn.setK( ui->pipelineTool_knnK->value() );
            core.setClassifier( knn );
            break;
        case CLASSIFIER_MINDIST:
            minDist.enableScaling( ui->pipelineTool_enableScaling->isChecked() );
            minDist.enableNullRejection( ui->pipelineTool_enableNullRejection->isChecked() );
            minDist.setNullRejectionCoeff( ui->pipelineTool_nullRejectionCoeff->value() );
            minDist.setNumClusters( ui->pipelineTool_minDistNumClusters->value() );
            core.setClassifier( minDist );
            break;
        case CLASSIFIER_RANDOM_FORESTS:
            randomForests.enableScaling( ui->pipelineTool_enableScaling->isChecked() );
            randomForests.enableNullRejection( ui->pipelineTool_enableNullRejection->isChecked() );
            randomForests.setNullRejectionCoeff( ui->pipelineTool_nullRejectionCoeff->value() );
            randomForests.setForestSize( ui->pipelineTool_randomForests_numTrees->value() );
            randomForests.setNumRandomSpilts( ui->pipelineTool_randomForests_numSpiltingSteps->value() );
            randomForests.setMaxDepth( ui->pipelineTool_randomForests_maxDepth->value() );
            randomForests.setMinNumSamplesPerNode( ui->pipelineTool_randomForests_minSamplesPerNode->value() );
            core.setClassifier( randomForests );
            break;
        case CLASSIFIER_SOFTMAX:
            softMax.enableScaling( ui->pipelineTool_enableScaling->isChecked() );
            softMax.enableNullRejection( ui->pipelineTool_enableNullRejection->isChecked() );
            softMax.setNullRejectionCoeff( ui->pipelineTool_nullRejectionCoeff->value() );
            core.setClassifier( softMax );
            break;
        case CLASSIFIER_SVM:
            svm.enableScaling( ui->pipelineTool_enableScaling->isChecked() );
            svm.enableNullRejection( ui->pipelineTool_enableNullRejection->isChecked() );
            svm.setNullRejectionCoeff( ui->pipelineTool_nullRejectionCoeff->value() );
            core.setClassifier( svm );
            break;
        case CLASSIFIER_SWIPE_DETECTOR:

            //Reset the graph
            swipeDetectorGraph->init(3, DEFAULT_GRAPH_WIDTH);

            swipeDetector.init( core.getNumInputDimensions() );
            swipeDetector.setSwipeIndex( ui->pipelineTool_swipeDetector_swipeIndex->value() );
            swipeDetector.setSwipeDirection( ui->pipelineTool_swipeDetector_swipeDirection->currentIndex() );
            swipeDetector.setSwipeThreshold( ui->pipelineTool_swipeDetector_swipeThreshold->value() );
            swipeDetector.setHysteresisThreshold( ui->pipelineTool_swipeDetector_hysteresisThreshold->value() );
            swipeDetector.setMovementThreshold( ui->pipelineTool_swipeDetector_movementThreshold->value() );
            swipeDetector.setSwipeIntegrationCoeff( ui->pipelineTool_swipeDetector_swipeIntegrationCoeff->value() );
            swipeDetector.reset();

            core.setClassifier( swipeDetector );
            break;
    }
}

void MainWindow::updateRegressifierView(int viewIndex){

    ui->pipelineTool_regressifierOptionsView->setCurrentIndex( viewIndex );
    ui->pipelineTool_regressifierType->setCurrentIndex( viewIndex );

    GRT::LinearRegression linearRegression;
    GRT::LogisticRegression logisticRegression;
    GRT::MLP mlp;
    bool enableScaling = ui->pipelineTool_regressionView_enableScaling->isChecked();
    bool useMDRegression = false;
    double minChange = ui->pipelineTool_regressionView_minChangeSpinBox->value();
    unsigned int maxNumEpochs = ui->pipelineTool_regressionView_maxNumEpochsSpinBox->value();
    unsigned int numInputs = ui->setupView_numInputsSpinBox->value();
    unsigned int numOutputs = ui->setupView_numOutputsSpinBox->value();
    unsigned int numOutputNeurons = 0;
    unsigned int inputLayerActivationFunction = GRT::Neuron::LINEAR;
    unsigned int hiddenLayerActiviationFunction = GRT::Neuron::LINEAR;
    unsigned int outputLayerActivationFunction = GRT::Neuron::LINEAR;

    //Check to see if we should automatically use multidimensional regression
    if( numOutputs > 1 ){
        useMDRegression = true;
    }else useMDRegression = false;

    //If we are using a MLP then the user can switch the MD regression on/off
    if( viewIndex == REGRESSIFIER_MLP ){
        useMDRegression = ui->pipelineTool_mlpUseMDRegression->isChecked();
        if( useMDRegression ) numOutputNeurons = 1;
        else numOutputNeurons = numOutputs;
    }

    switch( viewIndex ){
        case 0: //LINEAR REGRESSION
            linearRegression.enableScaling( enableScaling );
            linearRegression.setMaxNumIterations( maxNumEpochs );
            linearRegression.setMinChange( minChange );

            if( useMDRegression ) core.setRegressifier( GRT::MultidimensionalRegression( linearRegression, enableScaling ) );
            else core.setRegressifier( linearRegression );
        break;
        case 1: //LOGISTIC REGRESSION
            logisticRegression.enableScaling( enableScaling );
            logisticRegression.setMaxNumIterations( maxNumEpochs );
            logisticRegression.setMinChange( minChange );

            if( useMDRegression ) core.setRegressifier( GRT::MultidimensionalRegression( logisticRegression, enableScaling ) );
            else core.setRegressifier( logisticRegression );
        break;
        case 2: //MLP
            //Set the activation functions
            hiddenLayerActiviationFunction = ui->pipelineTool_mlpHiddenLayerType->currentIndex();
            outputLayerActivationFunction = ui->pipelineTool_mlpOutputLayerType->currentIndex();

            //Init the mlp
            mlp.init( numInputs,
                      ui->pipelineTool_mlpNumHiddenNeurons->value(),
                      numOutputNeurons,
                      inputLayerActivationFunction,hiddenLayerActiviationFunction,outputLayerActivationFunction);

            mlp.enableScaling( enableScaling );
            mlp.setMaxNumEpochs( maxNumEpochs );
            mlp.setMinChange( minChange );

            if( useMDRegression ) core.setRegressifier( GRT::MultidimensionalRegression( mlp, enableScaling ) );
            else core.setRegressifier( mlp );
        break;
    }

}

void MainWindow::updatePostProcessingView(int viewIndex){

    ui->pipelineTool_postProcessingOptionsView->setCurrentIndex( viewIndex );
    ui->pipelineTool_postProcessingType->setCurrentIndex( viewIndex );

    switch( viewIndex ){
        case NO_POST_POST_PROCESSING:
            core.removeAllPostProcessingModules();
        break;
        case CLASS_LABEL_FILTER_POST_PROCESSING:
            core.setPostProcessing( GRT::ClassLabelFilter( ui->pipelineTool_classLabelFilterMinCountSpinBox->value(), ui->pipelineTool_classLabelFilterBufferSizeSpinBox->value() ) );
        break;
        case CLASS_LABEL_CHANGE_FILTER_POST_PROCESSING:
            core.setPostProcessing(  GRT::ClassLabelChangeFilter( ) );
        break;
        case CLASS_LABEL_TIMEOUT_FILTER_POST_PROCESSING:
            core.setPostProcessing(  GRT::ClassLabelTimeoutFilter( ui->pipelineTool_classLabelTimeoutFilterTimeoutDurationSpinBox->value(), ui->pipelineTool_classLabelTimeoutFilterFilterModeList->currentIndex() ) );
        break;
    }

}

void MainWindow::refreshPipelineSetup(){

    updatePreProcessingView( ui->pipelineTool_preProcessingType->currentIndex() );
    updateFeatureExtractionView( ui->pipelineTool_preProcessingType->currentIndex() );

    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            updateClassifierView( ui->pipelineTool_classifierType->currentIndex() );
        break;
        case Core::REGRESSION_MODE:
            updateRegressifierView(  ui->pipelineTool_regressifierType->currentIndex() );
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
        break;
    }
    updatePostProcessingView( ui->pipelineTool_postProcessingType->currentIndex() );
}

void MainWindow::updatePreProcessingSettings(){
    updatePreProcessingView( ui->pipelineTool_preProcessingOptionsView->currentIndex() );
}

void MainWindow::updateClassifierSettings(){
    updateClassifierView( ui->pipelineTool_classifierType->currentIndex() );
}

void MainWindow::clearPipelineConfiguration(){
    switch( core.getPipelineMode() ){
        case Core::CLASSIFICATION_MODE:
            resetDefaultPipelineClassificationSetup();
        break;
        case Core::REGRESSION_MODE:
            resetDefaultPipelineRegressionSetup();
        break;
        case Core::TIMESERIES_CLASSIFICATION_MODE:
        break;
    }
}

void MainWindow::updatePipelineConfiguration(){
    if( core.getTrained() ){
        ui->mainWindow_pipelineTrainedInfoTextField->setText( "YES" );
        ui->predictionWindow_plotPredictedClassLabelsDataButton->setEnabled( true );
        ui->predictionWindow_plotClassLikelihoodsDataButton->setEnabled( true );
        ui->predictionWindow_plotClassDistancesDataButton->setEnabled( true );
        ui->predictionWindow_plotRegressionDataButton->setEnabled( true );

        unsigned int K = 0;
        unsigned int maxClassLabel = 0;

        switch( core.getPipelineMode() ){
            case Core::CLASSIFICATION_MODE:
                K = core.getNumClasses();
                maxClassLabel = GRT::Util::getMax( core.getClassLabels() );
                classPredictionsGraph->init(1,DEFAULT_GRAPH_WIDTH);
                classPredictionsGraph->setYAxisRanges(0,maxClassLabel,true);
                classLikelihoodsGraph->init( K, DEFAULT_GRAPH_WIDTH );
                classLikelihoodsGraph->setYAxisRanges(0,1,true);
                classDistancesGraph->init( K, DEFAULT_GRAPH_WIDTH );
            break;
            case Core::REGRESSION_MODE:
                regressionGraph->init( ui->setupView_numOutputsSpinBox->value(), DEFAULT_GRAPH_WIDTH );
            break;
            case Core::TIMESERIES_CLASSIFICATION_MODE:
             break;
        }

    }else{
        ui->mainWindow_pipelineTrainedInfoTextField->setText( "NO" );
        ui->predictionWindow_plotPredictedClassLabelsDataButton->setEnabled( false );
        ui->predictionWindow_plotClassLikelihoodsDataButton->setEnabled( false );
        ui->predictionWindow_plotClassDistancesDataButton->setEnabled( false );
        ui->predictionWindow_plotRegressionDataButton->setEnabled( false );
    }

}

void MainWindow::resetPipelineConfiguration(){

    //qDebug() << "resetPipelineConfiguration()" ;

    /*

    GRT::GestureRecognitionPipeline pipeline = core.getPipeline();

    if( pipeline.getIsPreProcessingSet() ){
        if( pipeline.getNumPreProcessingModules() > 1 ){
            emit updateInfoText( "WARNING: Found more than 1 preprocessing module. Displaying settings for 1st module" );
        }
        GRT::PreProcessing *preProcessing = pipeline.getPreProcessingModule(0);
        std::string type = preProcessing->getPreProcessingType();

        if( type == "MovingAverageFilter" ){
            updatePreProcessingView( MOVING_AVERAGE_FILTER_PRE_PROCESSING );
            ui->pipelineTool_postProcessingType->setCurrentIndex( MOVING_AVERAGE_FILTER_PRE_PROCESSING );

            GRT::MovingAverageFilter *filter = pipeline.getPreProcessingModule< GRT::MovingAverageFilter >(0);
            ui->pipelineTool_movingAverageFilterSizeSpinBox->setValue( filter->getFilterSize() );
        }
    }else updatePreProcessingView( NO_PRE_PROCESSING_SELECTED );

    if( pipeline.getTrained() ){
        ui->mainWindow_pipelineTrainedInfoTextField->setText( "YES" );
    }else ui->mainWindow_pipelineTrainedInfoTextField->setText( "NO" );

    */

}

void MainWindow::resetDefaultPipelineClassificationSetup(){
    updatePreProcessingView( 0 );
    updateFeatureExtractionView( 0 );
    updateClassifierView( 0 );
    updatePostProcessingView( 0 );
    ui->trainingTool_results->setText("");
}

void MainWindow::resetDefaultPipelineRegressionSetup(){
    updatePreProcessingView( 0 );
    updateFeatureExtractionView( 0 );
    updateRegressifierView( 0 );
    updatePostProcessingView( 0 );
    ui->trainingTool_results->setText("");
}

void MainWindow::setupDefaultClassifier(){
    ui->pipelineTool_classifierRegressifierView->setCurrentIndex( 0 );
    ui->pipelineTool_classifierType->setCurrentIndex( 0 );
    ui->pipelineTool_classifierOptionsView->setCurrentIndex( 0 );
    ui->trainingTool_results->setText("");
}

void MainWindow::setupDefaultRegressifier(){
    ui->pipelineTool_classifierRegressifierView->setCurrentIndex( 1 );
    ui->pipelineTool_regressifierType->setCurrentIndex( 0 );
    ui->pipelineTool_regressifierOptionsView->setCurrentIndex( 0 );
    ui->trainingTool_results->setText("");
}

void MainWindow::updateClassifier(unsigned int classifierType,bool useScaling,bool useNullRejection,double nullRejectionCoeff,double parameter1){

    //Setup the classifier view
    ui->pipelineTool_classifierType->setCurrentIndex( classifierType );
    ui->pipelineTool_enableScaling->setChecked( useScaling );
    ui->pipelineTool_enableNullRejection->setChecked( useNullRejection );
    ui->pipelineTool_nullRejectionCoeff->setValue( nullRejectionCoeff );
    ui->trainingTool_results->setText("");
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// PREDICTION VIEW FUNCTIONS ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::updatePreProcessingData(const GRT::VectorDouble &preProcessedData){

    QString infoText;
    for(size_t k=0; k<preProcessedData.size(); k++){
        infoText += "[" + QString::number( k ) + "]: ";
        infoText += QString::number( preProcessedData[k] );
        infoText += " ";
    }
    ui->predictionWindow_preProcessedData->setText( infoText );
    preProcessedDataGraph->update( preProcessedData );
}

void MainWindow::updateFeatureExtractionData(const GRT::VectorDouble &featureExtractionData){

    QString infoText;
    for(size_t k=0; k<featureExtractionData.size(); k++){
        infoText += "[" + QString::number( k ) + "]: ";
        infoText += QString::number( featureExtractionData[k] );
        infoText += " ";
    }
    ui->predictionWindow_featureExtractionData->setText( infoText );
    featureExtractionDataGraph->update( featureExtractionData );
}

void MainWindow::updatePredictionResults(unsigned int predictedClassLabel,double maximumLikelihood,GRT::VectorDouble classLikelihoods,GRT::VectorDouble classDistances,std::vector<unsigned int> classLabels){

    classPredictionsGraph->update( GRT::VectorDouble(1,predictedClassLabel) );
    classLikelihoodsGraph->update( classLikelihoods );
    classDistancesGraph->update( classDistances );
    ui->predictionWindow_predictedClassLabel->setText( QString::number( predictedClassLabel ) );
    ui->predictionWindow_maximumLikelihood->setText( QString::number( maximumLikelihood ) );

    const size_t K = classLabels.size();

    if( K != classLikelihoods.size() ){
        errorLog << "updatePredictionResults(...) The size of the class labels vector does not match the size of the class likelihoods vector!" << endl;
        return;
    }
    if( K != classDistances.size() ){
        errorLog << "updatePredictionResults(...) The size of the class labels vector does not match the size of the class distances vector!" << endl;
        return;
    }

    QString infoText;
    for(size_t k=0; k<K; k++){
        infoText += "[" + QString::number( k ) + "]: ";
        if( classLikelihoods[k] > 1.0e-4 ) infoText += QString::number( classLikelihoods[k] );
        else infoText += QString::number( 0 );
        infoText += " ";
    }
    ui->predictionWindow_classLikelihoods->setText( infoText );

    infoText = "";
    for(size_t k=0; k<K; k++){
        infoText += "[" + QString::number( k ) + "]: ";
        infoText += QString::number( classDistances[k] );
        infoText += " ";
    }
    ui->predictionWindow_classDistances->setText( infoText );

    infoText = "";
    for(size_t k=0; k<K; k++){
        infoText += "[" + QString::number( k ) + "]: ";
        infoText += QString::number( classLabels[k] );
        infoText += " ";
    }
    ui->predictionWindow_classLabels->setText( infoText );

}

void MainWindow::updateRegressionResults(GRT::VectorDouble regressionData){

    const size_t N = regressionData.size();

    QString infoText;
    for(size_t n=0; n<N; n++){
        infoText += "[" + QString::number( n ) + "]: ";
        infoText += QString::number( regressionData[n] );
        infoText += " ";
    }
    ui->predictionWindow_regressionData->setText( infoText );

    regressionGraph->update( regressionData );

}

void MainWindow::resetPredictionViewGraphs(){

    //qDebug() << "resetPredictionViewGraphs" ;

    return;
/*
    QCustomPlot *classLikelihoodsPlot = ui->predictionView_classLikelihoodsPlot;
    QCustomPlot *classDistancesPlot = ui->predictionView_classDistancesPlot;

    //Clear any previous graphs
    classLikelihoodsPlot->clearGraphs();
    classDistancesPlot->clearGraphs();

    //Check to make sure a model has been trained
    if( !core.getModelTrained() ) return;

    //Get the data to plot
    const unsigned int K = core.getNumClasses();
    vector<unsigned int> classLabels = core.getClassLabels();

    QCPBars *bar1 = new QCPBars(classLikelihoodsPlot->xAxis, classLikelihoodsPlot->yAxis);
    QCPBars *bar2 = new QCPBars(classDistancesPlot->xAxis, classDistancesPlot->yAxis);
    classLikelihoodsPlot->addPlottable(bar1);
    classDistancesPlot->addPlottable(bar2);

    QVector<QString> tickLabels;
    for(unsigned int k=0; k<K; k++){
        tickLabels << QString::fromStdString( GRT::Util::intToString( classLabels[k]) );
    }

    classLikelihoodsPlot->setTitle("Class Likelihoods");
    classDistancesPlot->setTitle("Class Distances");
    classLikelihoodsPlot->replot();
    classDistancesPlot->replot();
    */
}

void MainWindow::showPredictionToolInfo(){
    QString infoText;
    infoText += "This is the Prediction Tool view. It lets you see the predicted values from your custom gesture recognition pipeline.\n\n";

    infoText += "You need to train your pipeline before you can view the prediction results.\n\n";

    infoText += "Checkout the main GUI wiki for more info: www.nickgillian.com/wiki/pmwiki.php/GRT/GUI \n\n";
    QMessageBox::information(0, "Information", infoText);
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////     LOG VIEW FUNCTIONS    ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::showLogViewInfo(){
    QString infoText;
    infoText += "This is the Log view. It lets you see all the log messages generated by the GUI.\n\n";

    infoText += "Checkout the main GUI wiki for more info: www.nickgillian.com/wiki/pmwiki.php/GRT/GUI \n\n";
    QMessageBox::information(0, "Information", infoText);
}

void MainWindow::showAllLogs(){
    updateLogView( ALL_LOGS_VIEW );
}

void MainWindow::showInfoLog(){
    updateLogView( INFO_LOG_VIEW );
}

void MainWindow::showWarningLog(){
    updateLogView( WARNING_LOG_VIEW );
}

void MainWindow::showErrorLog(){
    updateLogView( ERROR_LOG_VIEW );
}

void MainWindow::updateLogView(unsigned int viewID){

    switch( viewID ){
        case ALL_LOGS_VIEW:
            ui->logView_logFrame->setCurrentIndex( 0 );
            ui->logView_infoLogButton->setChecked( false );
            ui->logView_warningLogButton->setChecked( false );
            ui->logView_errorLogButton->setChecked( false );
        break;
        case INFO_LOG_VIEW:
            ui->logView_logFrame->setCurrentIndex( 1 );
            ui->logView_allLogsButton->setChecked( false );
            ui->logView_warningLogButton->setChecked( false );
            ui->logView_errorLogButton->setChecked( false );
        break;
        case WARNING_LOG_VIEW:
            ui->logView_logFrame->setCurrentIndex( 2 );
            ui->logView_allLogsButton->setChecked( false );
            ui->logView_infoLogButton->setChecked( false );
            ui->logView_errorLogButton->setChecked( false );
        break;
        case ERROR_LOG_VIEW:
            ui->logView_logFrame->setCurrentIndex( 3 );
            ui->logView_allLogsButton->setChecked( false );
            ui->logView_infoLogButton->setChecked( false );
            ui->logView_warningLogButton->setChecked( false );
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////     CORE DATA FUNCTIONS   ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::coreTick(){

    if( ui->dataIO_enableMouseInputButton->isChecked() ){

        //Generate a dummy OSC data message for the mouse
        GRT::VectorDouble mouseData(2);
        mouseData[0] = this->mapFromGlobal(QCursor::pos()).x();
        mouseData[1] = this->mapFromGlobal(QCursor::pos()).y();

        OSCMessagePtr msg( new OSCMessage );

        msg->setSenderAddress( "localhost" );
        msg->setAddressPattern( core.getIncomingDataAddress() );
        msg->addArg( mouseData[0] );
        msg->addArg( mouseData[1] );

        //Add the message to the OSC server, this will make it appear like an external message
        core.addMessaage( msg );
    }
}

void MainWindow::updateData(GRT::VectorDouble data){
    QString text = "";
    for(size_t i=0; i<data.size(); i++){
        text += "[" + QString::number( i ) + "]: ";
        text += QString::number( data[i] );
        if( i != data.size()-1 ) text += " ";
    }
    ui->predictionWindow_data->setText( text );
    inputDataGraph->update( data );


    //TODO - THis is a quick hack

    //Check to see if we are using the swipe detector
    if( ui->pipelineTool_classifierType->currentIndex() == CLASSIFIER_SWIPE_DETECTOR ){

        if( swipeDetector.getTrained() ){
            swipeDetector.predict( data );
            GRT::VectorDouble swipeDetectionData( 3 );
            swipeDetectionData[0] = swipeDetector.getSwipeValue();
            swipeDetectionData[1] = swipeDetector.getSwipeThreshold();
            swipeDetectionData[2] = swipeDetector.getHysteresisThreshold();

            swipeDetectorGraph->update( swipeDetectionData );
        }

    }
}

void MainWindow::updateTargetVector(GRT::VectorDouble targetVector){

    QString text = "";
    for(size_t i=0; i<targetVector.size(); i++){
        text += "[" + QString::number( i ) + "]: ";
        text += QString::number( targetVector[i] );
        if( i != targetVector.size()-1 ) text += " ";
    }
    ui->dataLabellingTool_targetVectorTextField->setText( text );
}

void MainWindow::notify(const GRT::TrainingLogMessage &log){
    boost::mutex::scoped_lock lock( mutex );
    std::string message = log.getProceedingText() + " " + log.getMessage();
    //ui->trainingTool_results->append( QString::fromStdString( message ) ); //This seems to be crashing things sometimes, WHY?
}

void MainWindow::notify(const GRT::TestingLogMessage &log){
    boost::mutex::scoped_lock lock( mutex );
    std::string message = log.getProceedingText() + " " + log.getMessage();
    ui->trainingTool_results->append( QString::fromStdString( message ) );
}

void MainWindow::notify(const GRT::WarningLogMessage &log){
    std::string message = log.getProceedingText() + " " + log.getMessage();
    updateWarningText( message );
}

void MainWindow::notify(const GRT::ErrorLogMessage &log){
    std::string message = log.getProceedingText() + " " + log.getMessage();
    updateErrorText( message );
}

void MainWindow::notify(const GRT::InfoLogMessage &log){
    std::string message = log.getProceedingText() + " " + log.getMessage();
    updateInfoText( message );
}
