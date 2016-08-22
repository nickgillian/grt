#include "Core.h"

Core::Core(QObject *parent) : QObject(parent)
{
    coreSleepTime = DEFAULT_CORE_THREAD_SLEEP_TIME;
    coreRunning = false;
    stopMainThread = false;
    verbose = true;
    debug = false;
    enableOSCInput = true;
    enableOSCControlCommands = true;
    infoMessage = "";
    version = GRT_GUI_VERSION;

    incomingOSCDataPort = 5000;
    outgoingOSCDataPort = 5001;
    outgoingOSCAddress = "127.0.0.1";
    oscServer.setVerbose( false );
    incomingDataAddress = "/Data";

    numInputDimensions = 1;
    targetVectorSize = 1;
    inputData.resize( numInputDimensions );
    targetVector.resize( targetVectorSize, 0 );
    trainingClassLabel = 1;
    recordTrainingData = false;
    newDataReceived = false;
    predictionModeEnabled = true;
    pipelineMode = CLASSIFICATION_MODE;

    //Connect the training thread signals
    connect(&trainingThread, SIGNAL(newInfoMessage(std::string)), this, SIGNAL(newInfoMessage(std::string)));
    connect(&trainingThread, SIGNAL(newWarningMessage(std::string)), this, SIGNAL(newWarningMessage(std::string)));
    connect(&trainingThread, SIGNAL(newErrorMessage(std::string)), this, SIGNAL(newErrorMessage(std::string)));
    connect(&trainingThread, SIGNAL(newHelpMessage(std::string)), this, SIGNAL(newHelpMessage(std::string)));
    connect(&trainingThread, SIGNAL(pipelineTrainingStarted()), this, SIGNAL(pipelineTrainingStarted()));
    connect(&trainingThread, SIGNAL(pipelineTrainingFinished(bool)), this, SIGNAL(pipelineTrainingFinished(bool)));
    connect(&trainingThread, SIGNAL(pipelineTestingFinished(bool)), this, SIGNAL(pipelineTestingFinished(bool)));
    connect(&trainingThread, SIGNAL(pipelineUpdated(const GRT::GestureRecognitionPipeline&)), this, SLOT(setPipeline(const GRT::GestureRecognitionPipeline&)));

}

Core::~Core(){
    if( getCoreRunning() ){
        stop();
    }
}

bool Core::start(){

    if( getCoreRunning() ){
        if( debug )
            qDebug() << "WARNING: Core::start() - The core is already running!" << endl;

        if( verbose ){
            emit newInfoMessage( "WARNING: Failed to start core, it is already running!" );
        }
        return false;
    }

    if( debug )
        qDebug() << STRING_TO_QSTRING("Core::start() - Starting main thread...");

    try{
        mainThread.reset( new std::thread( std::bind( &Core::mainThreadFunction, this) ) );
    }catch( std::exception const &error ){
        QString qstr = "ERROR: Core::start() - Failed to start server thread! Exception: ";
        qstr += error.what();
        qstr += "\n";
        qDebug() << qstr;
        return false;
    }

    return true;
}

bool Core::stop(){

    if( !getCoreRunning() ){
        if( debug )
            qDebug() << "WARNING: Core::stop() - The core is not running!" << endl;


        if( verbose ){
            emit newInfoMessage( "WARNING: Failed to stop core, it is not running!" );
        }
        return false;
    }

    if( debug )
        qDebug() << STRING_TO_QSTRING("Core::stop() - Stopping main thread...");

    //Flag that the core should stop
    {
        std::unique_lock< std::mutex > lock( mutex );
        stopMainThread = true;
    }

    //Wait for it to stop
    mainThread->join();
    mainThread.reset();

    return true;
}

////////////////////////////////////////////////////////////////////////////
//////////////////////////      PUBLIC SLOTS      //////////////////////////
///////////////////////////////////////////////////////////////////////////

bool Core::resetOSCClient( const std::string clientAddress,const int clientPort ){

    outgoingOSCAddress = clientAddress;
    outgoingOSCDataPort = (unsigned int)clientPort;

    try{
        socket.reset( new UdpTransmitSocket( IpEndpointName( outgoingOSCAddress.c_str(), (int)outgoingOSCDataPort ) ) );
    }catch( std::exception const &error  ){
        QString qstr = "ERROR: Core::resetOSCClient() - Exception: ";
        qstr += error.what();
        qDebug() << qstr;
        return false;
    }

    return true;
}

bool Core::resetOSCServer( const int incomingOSCDataPort ){

    if( getCoreRunning() ){
        //Stop the OSC server
        oscServer.stop();

        //Update the OSC port
        this->incomingOSCDataPort = (unsigned int)incomingOSCDataPort;
        oscServer.setIncomingDataPort( incomingOSCDataPort );
        if( oscServer.start() ){
            emit newInfoMessage( "OSC Server now running on port: " + GRT::Util::toString( incomingOSCDataPort ) );
            return true;
        }
        emit newErrorMessage( "Failed to start OSC Server on port: " + GRT::Util::toString( incomingOSCDataPort ) );
        return false;
    }

    this->incomingOSCDataPort = (unsigned int)incomingOSCDataPort;

    return true;
}

bool Core::addMessaage( const OSCMessagePtr msg ){
    std::unique_lock< std::mutex > lock( mutex );
    oscServer.addMessaage( msg );
    return true;
}

bool Core::setVersion( const std::string version ){
    std::unique_lock< std::mutex > lock( mutex );
    this->version = version;
    return true;
}

bool Core::setEnableOSCInput( const bool state ){
    std::unique_lock< std::mutex > lock( mutex );
    enableOSCInput = state;
    return true;
}

bool Core::setEnableOSCControlCommands( const bool state ){
    std::unique_lock< std::mutex > lock( mutex );
    enableOSCControlCommands = state;
    return true;
}

bool Core::setPipelineMode( const unsigned int pipelineMode ){
    bool modeChanged = false;
    if( pipelineMode != this->pipelineMode ){
        std::unique_lock< std::mutex > lock( mutex );
        this->pipelineMode = pipelineMode;
        modeChanged = true;
    }
    if( modeChanged )
        emit pipelineModeChanged( pipelineMode );
    return true;
}

bool Core::setRecordingState( const bool state ){

    bool newSampleAdded = false;
    unsigned int numTrainingSamples = 0;
    GRT::TimeSeriesClassificationSample timeseriesSample_;

    {
        std::unique_lock< std::mutex > lock( mutex );
        recordTrainingData = state;

        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
            break;
            case REGRESSION_MODE:
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                //If the record training data has stopped then we should add the current timeseries sample to the timeseries classification data
                if( !recordTrainingData ){
                    if(timeseriesSample.getNumRows() >= 1 ){
                        if( timeseriesClassificationTrainingData.addSample(trainingClassLabel, timeseriesSample) ){
                            newSampleAdded = true;
                            numTrainingSamples = timeseriesClassificationTrainingData.getNumSamples();
                            timeseriesSample_ = timeseriesClassificationTrainingData[ numTrainingSamples-1 ];
                        }
                    }
                    timeseriesSample.clear();
                }
            break;
            case CLUSTER_MODE:
            break;
            default:
                qDebug() << "ERROR: Core::setRecordingState(bool state) - Unknown pipeline mode!";
            break;
        }

    }

    if( newSampleAdded ){
        emit newTrainingSampleAdded( numTrainingSamples, timeseriesSample_ );
    }

    return true;
}

bool Core::saveTrainingDatasetToFile( const std::string filename ){
    bool result = false;
    {
        std::unique_lock< std::mutex > lock( mutex );

        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                result = classificationTrainingData.save(filename);
            break;
            case REGRESSION_MODE:
                result = regressionTrainingData.save(filename);
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                result = timeseriesClassificationTrainingData.save(filename);
            break;
            case CLUSTER_MODE:
                result = clusterTrainingData.save(filename);
            break;
            default:
                qDebug() << "ERROR: Unknown pipeline mode!";
            break;
        }
    }
    if( result ){
        emit newInfoMessage( "Training data saved to file" );
    }else{
        emit newInfoMessage( "WARNING: Failed to save training data saved to file" );
    }
    emit saveTrainingDataToFileResult( result );
    return result;
}

bool Core::loadTrainingDatasetFromFile( const std::string filename ){

    bool result = false;
    bool inputDimensionsSizeError = false;
    unsigned int numTrainingSamples = 0;
    unsigned int tempDataSize = 0;
    unsigned int tempPipelineMode = 0;
    GRT::ClassificationData tempClassificationData;
    GRT::RegressionData tempRegressionData;
    GRT::TimeSeriesClassificationData tempTimeSeriesData;
    GRT::UnlabelledData tempClusterData;
    std::size_t found;

    {
         std::unique_lock< std::mutex > lock( mutex );
         unsigned int trainingDataSize = 0;
         tempPipelineMode = pipelineMode;
         switch( pipelineMode ){
             case CLASSIFICATION_MODE:
                result = classificationTrainingData.load( filename );

                 if( classificationTrainingData.getNumDimensions() == numInputDimensions ){
                     numTrainingSamples = classificationTrainingData.getNumSamples();
                     tempClassificationData = classificationTrainingData;
                     trainingDataSize = classificationTrainingData.getNumDimensions();
                 }else {
                     inputDimensionsSizeError = true;
                     tempDataSize = classificationTrainingData.getNumDimensions();
                     classificationTrainingData.clear();
                     tempClassificationData = classificationTrainingData;
                     numTrainingSamples = 0;
                 }
             break;
             case REGRESSION_MODE:
                 //Check to see if we should load the data from the default GRT fileformat or as a CSV file
                 found = filename.find( ".csv" );
                 if ( found == std::string::npos ){
                    qDebug() << "loading data from file!";
                     result = regressionTrainingData.loadDatasetFromFile( filename );
                 }
                 else{
                    qDebug() << "loading data from CSV file!";
                     result = regressionTrainingData.loadDatasetFromCSVFile( filename, this->numInputDimensions, this->targetVectorSize );
                 }

                 numTrainingSamples = regressionTrainingData.getNumSamples();
                 tempRegressionData = regressionTrainingData;
                 trainingDataSize = regressionTrainingData.getNumInputDimensions();
             break;
             case TIMESERIES_CLASSIFICATION_MODE:
                 result = timeseriesClassificationTrainingData.load( filename );

                 numTrainingSamples = timeseriesClassificationTrainingData.getNumSamples();
                 tempTimeSeriesData = timeseriesClassificationTrainingData;
                 trainingDataSize = timeseriesClassificationTrainingData.getNumDimensions();
             break;
             case CLUSTER_MODE:
                result = clusterTrainingData.load( filename );

                numTrainingSamples = clusterTrainingData.getNumSamples();
                tempClusterData = clusterTrainingData;
                trainingDataSize = clusterTrainingData.getNumDimensions();
             break;
             default:
                 qDebug() << "ERROR: Unknown pipeline mode!";
                 return false;
             break;
         }
    }

    if( inputDimensionsSizeError ){
        std::string msg = "Training data loaded, however the number of input dimensions in the training data (";
        msg += GRT::Util::toString( tempDataSize ) + ") does not match the number of input dimensions of your project (";
        msg += GRT::Util::toString( numInputDimensions ) + "). You can change the number of input dimensions via the Setup view.";
        emit newErrorMessage( msg );
        emit loadTrainingDataFromFileResult( false );
        return false;
    }

    if( result ){
        emit numTrainingSamplesChanged( numTrainingSamples );

        switch( tempPipelineMode ){
            case CLASSIFICATION_MODE:
                emit trainingDataReset( tempClassificationData );
            break;
            case REGRESSION_MODE:
                emit trainingDataReset( tempRegressionData );
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                emit trainingDataReset( tempTimeSeriesData );
            break;
            case CLUSTER_MODE:
                emit trainingDataReset( tempClusterData );
            break;
            default:
                qDebug() << "ERROR: Unknown pipeline mode!";
                return false;
            break;
        }
    }

    if( result ){
        emit newInfoMessage( "Training data loaded from file" );
    }else emit newWarningMessage( "WARNING: Failed to load training data from file" );

    emit loadTrainingDataFromFileResult( result );

    return result;
}

bool Core::loadTestDatasetFromFile( const std::string filename ){
    bool result = false;
    unsigned int numTestSamples = 0;
    GRT::ClassificationData tempClassificationData;
    GRT::RegressionData tempRegressionData;
    GRT::TimeSeriesClassificationData tempTimeSeriesData;
    GRT::UnlabelledData tempClusterData;

    {
         std::unique_lock< std::mutex > lock( mutex );

         switch( pipelineMode ){
             case CLASSIFICATION_MODE:
                result = classificationTestData.loadDatasetFromFile( filename );
                numTestSamples = classificationTestData.getNumSamples();
                tempClassificationData = classificationTestData;
                if( classificationTestData.getNumDimensions() != classificationTrainingData.getNumDimensions() ){
                    result = false;
                    emit newWarningMessage("WARNING: The number of dimensions in the test data does not match the training data");
                }
             break;
             case REGRESSION_MODE:
                result = regressionTestData.loadDatasetFromFile( filename );
                numTestSamples = regressionTestData.getNumSamples();
                tempRegressionData = regressionTestData;
                if( regressionTestData.getNumInputDimensions() != regressionTrainingData.getNumInputDimensions() ){
                    result = false;
                    emit newWarningMessage("WARNING: The number of input dimensions in the test data does not match the training data");
                }
                if( regressionTestData.getNumTargetDimensions() != regressionTrainingData.getNumTargetDimensions() ){
                    result = false;
                    emit newWarningMessage("WARNING: The number of target dimensions in the test data does not match the training data");
                }
             break;
             case TIMESERIES_CLASSIFICATION_MODE:
                 //TODO
             break;
             case CLUSTER_MODE:
                result = clusterTestData.loadDatasetFromFile( filename );
                numTestSamples = clusterTestData.getNumSamples();
                tempClusterData = clusterTestData;
                if( clusterTestData.getNumDimensions() != clusterTrainingData.getNumDimensions() ){
                    result = false;
                    emit newWarningMessage("WARNING: The number of dimensions in the test data does not match the training data");
                }
             break;
             default:
                 qDebug() << "ERROR: loadTestDatasetFromFile(std::string filename) - Unknown pipeline mode!";
                 return false;
             break;
         }

    }

    if( result ){
        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
               emit testDataReset( tempClassificationData );
            break;
            case REGRESSION_MODE:
                emit testDataReset( tempRegressionData );
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                //TODO
            break;
            case CLUSTER_MODE:
                emit testDataReset( tempClusterData );
            break;
            default:
                qDebug() << "ERROR: loadTestDatasetFromFile(std::string filename) - Unknown pipeline mode!";
                return false;
            break;
        }
    }

    if( result ) emit newInfoMessage( "Test data loaded from file" );
    else emit newWarningMessage( "WARNING: Failed to load test data from file" );

    emit loadTestDataFromFileResult( result );

    return result;
}

void Core::clearTrainingData(){

    GRT::ClassificationData tempClassificationData;
    GRT::RegressionData tempRegressionData;
    GRT::TimeSeriesClassificationData tempTimeSeriesData;
    GRT::UnlabelledData tempClusterData;

    {
        std::unique_lock< std::mutex > lock( mutex );
        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                classificationTrainingData.clear();
                tempClassificationData = classificationTrainingData;
            break;
            case REGRESSION_MODE:
                regressionTrainingData.clear();
                tempRegressionData = regressionTrainingData;
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                timeseriesClassificationTrainingData.clear();
                tempTimeSeriesData = timeseriesClassificationTrainingData;
            break;
            case CLUSTER_MODE:
                clusterTrainingData.clear();
                tempClusterData = clusterTrainingData;
            break;
            default:
                qDebug() << "ERROR: clearTrainingData() - Unknown pipeline mode!";
                return;
            break;
        }
    }

    emit newInfoMessage( "Training data cleared" );
    emit numTrainingSamplesChanged( 0 );

    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            emit trainingDataReset( tempClassificationData );
        break;
        case REGRESSION_MODE:
            emit trainingDataReset( tempRegressionData );
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
            emit trainingDataReset( tempTimeSeriesData );
        break;
        case CLUSTER_MODE:
            emit trainingDataReset( tempClusterData );
        break;
        default:
            qDebug() << "ERROR: clearTrainingData() - Unknown pipeline mode!";
            return;
        break;
    }
}

///////////////////////////////////////////////////////////////////////////
////////////////////////////      GETTERS      ////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool Core::getCoreRunning(){
    std::unique_lock< std::mutex > lock( mutex );
    return coreRunning;
}

bool Core::getTrained(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getTrained();
}

bool Core::getTrainingInProcess(){
    std::unique_lock< std::mutex > lock( mutex );
    return trainingThread.getTrainingInProcess();
}

bool Core::getRecordStatus(){
    std::unique_lock< std::mutex > lock( mutex );
    return recordTrainingData;
}

unsigned int Core::getNumInputDimensions(){
    std::unique_lock< std::mutex > lock( mutex );
    return numInputDimensions;
}

unsigned int Core::getPipelineMode(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipelineMode;
}

unsigned int Core::getTrainingClassLabel(){
    std::unique_lock< std::mutex > lock( mutex );
    return trainingClassLabel;
}

unsigned int Core::getNumTrainingSamples(){
    std::unique_lock< std::mutex > lock( mutex );
    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            return classificationTrainingData.getNumSamples();
        break;
        case REGRESSION_MODE:
            return regressionTrainingData.getNumSamples();
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
            return timeseriesClassificationTrainingData.getNumSamples();
        break;
        case CLUSTER_MODE:
            return clusterTrainingData.getNumSamples();
        break;
        default:
            qDebug() << "ERROR: getNumTrainingSamples() - Unknown pipeline mode!";
            return false;
        break;
    }
}

unsigned int Core::getNumTestSamples(){
    std::unique_lock< std::mutex > lock( mutex );
    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            return classificationTestData.getNumSamples();
        break;
        case REGRESSION_MODE:
            return regressionTestData.getNumSamples();
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
            return timeseriesClassificationTestData.getNumSamples();
        break;
        case CLUSTER_MODE:
            return clusterTestData.getNumSamples();
        break;
        default:
            qDebug() << "ERROR: getNumTestSamples() - Unknown pipeline mode!";
            return false;
        break;
    }
}

unsigned int Core::getNumClasses(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getNumClasses();
}

unsigned int Core::getNumClassesInTrainingData(){
    std::unique_lock< std::mutex > lock( mutex );
    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            return classificationTrainingData.getNumClasses();
        break;
        case REGRESSION_MODE:
            return 0; //There are no classes in regression data
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
            return timeseriesClassificationTrainingData.getNumClasses();
        break;
        case CLUSTER_MODE:
            return 0; //There are no classes in cluster mode
        break;
        default:
            qDebug() << "ERROR: getNumClassesInTrainingData() - Unknown pipeline mode!";
            return false;
        break;
    }
    return 0;
}

vector<unsigned int> Core::getClassLabels(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getClassLabels();
}

GRT::VectorFloat Core::getTargetVector(){
    std::unique_lock< std::mutex > lock( mutex );
    return targetVector;
}

GRT::ClassificationData Core::getClassificationTrainingData(){
    std::unique_lock< std::mutex > lock( mutex );
    return classificationTrainingData;
}

GRT::ClassificationData Core::getClassificationTestData(){
    std::unique_lock< std::mutex > lock( mutex );
    return classificationTestData;
}

GRT::RegressionData Core::getRegressionTrainingData(){
    std::unique_lock< std::mutex > lock( mutex );
    return regressionTrainingData;
}

GRT::RegressionData Core::getRegressionTestData(){
    std::unique_lock< std::mutex > lock( mutex );
    return regressionTestData;
}

GRT::TimeSeriesClassificationData Core::getTimeSeriesClassificationTrainingData(){
    std::unique_lock< std::mutex > lock( mutex );
    return timeseriesClassificationTrainingData;
}

GRT::TimeSeriesClassificationData Core::getTimeSeriesClassificationTestData(){
    std::unique_lock< std::mutex > lock( mutex );
    return timeseriesClassificationTestData;
}

GRT::UnlabelledData Core::getClusterTrainingData(){
    std::unique_lock< std::mutex > lock( mutex );
    return clusterTrainingData;
}

GRT::UnlabelledData Core::getClusterTestData(){
    std::unique_lock< std::mutex > lock( mutex );
    return clusterTestData;
}

double Core::getTestAccuracy(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getTestAccuracy();
}

double Core::getCrossValidationAccuracy(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getCrossValidationAccuracy();
}

double Core::getTrainingRMSError(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getTrainingRMSError();
}

double Core::getTrainingSSError(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getTrainingSSError();
}

GRT::GestureRecognitionPipeline Core::getPipeline(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline;
}

GRT::TestResult Core::getTestResults(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getTestResults();
}

vector< GRT::TestResult > Core::getCrossValidationResults(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getCrossValidationResults();
}

std::string Core::getModelAsString(){
    std::unique_lock< std::mutex > lock( mutex );
    return pipeline.getModelAsString();
}

///////////////////////////////////////////////////////////////////////////
////////////////////////////      SETTERS      ////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool Core::setCoreSleepTime( const unsigned int coreSleepTime ){
    std::unique_lock< std::mutex > lock( mutex );
    this->coreSleepTime = coreSleepTime;
    return true;
}

bool Core::setNumInputDimensions( const int numInputDimensions ){

    bool result = false;
    GRT::ClassificationData tempClassificationData;
    GRT::RegressionData tempRegressionData;
    GRT::TimeSeriesClassificationData tempTimeSeriesData;
    GRT::UnlabelledData tempClusterData;

    {
        std::unique_lock< std::mutex > lock( mutex );
        this->numInputDimensions = (unsigned int)numInputDimensions;
        inputData.clear();
        inputData.resize( numInputDimensions, 0 );

        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                classificationTrainingData.clear();
                classificationTrainingData.setNumDimensions( numInputDimensions );
                tempClassificationData = classificationTrainingData;
            break;
            case REGRESSION_MODE:
                targetVector.clear();
                targetVector.resize( targetVectorSize, 0 );
                regressionTrainingData.clear();
                regressionTrainingData.setInputAndTargetDimensions( numInputDimensions, targetVectorSize );
                tempRegressionData = regressionTrainingData;
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                timeseriesClassificationTrainingData.clear();
                timeseriesClassificationTrainingData.setNumDimensions( numInputDimensions );
                tempTimeSeriesData = timeseriesClassificationTrainingData;
            break;
            case CLUSTER_MODE:
                clusterTrainingData.clear();
                clusterTrainingData.setNumDimensions( numInputDimensions );
                tempClusterData = clusterTrainingData;
            break;
            default:
                qDebug() << "ERROR: setNumInputDimensions() - Unknown pipeline mode!";
                return false;
            break;
        }

        result = true;
    }

    if( result ){
        emit numInputDimensionsChanged( numInputDimensions );

        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                emit trainingDataReset( tempClassificationData );
            break;
            case REGRESSION_MODE:
                emit trainingDataReset( tempRegressionData );
                emit targetDataChanged( getTargetVector() );
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                //TODO
            break;
            case CLUSTER_MODE:
                 emit trainingDataReset( tempClusterData );
            break;
            default:
                qDebug() << "ERROR: setNumInputDimensions() - Unknown pipeline mode!";
                return false;
            break;
        }
    }

    return result;
}

bool Core::setTargetVectorSize( const int targetVectorSize_ ){

    bool result = false;
    GRT::LabelledRegressionData tempRegressionData;

    {
        std::unique_lock< std::mutex > lock( mutex );
        targetVectorSize = (unsigned int)targetVectorSize_;
        targetVector.clear();
        targetVector.resize( targetVectorSize, 0 );
        regressionTrainingData.clear();
        regressionTrainingData.setInputAndTargetDimensions( numInputDimensions, targetVectorSize );
        tempRegressionData = regressionTrainingData;
        result = true;
    }

    if( result ){
        emit numTargetDimensionsChanged( targetVectorSize_ );
        emit trainingDataReset( tempRegressionData );
    }

    return true;
}

bool Core::setMainDataAddress( const std::string address ){
    bool addressUpdated = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        if( this->incomingDataAddress != address ){
            this->incomingDataAddress = address;
            addressUpdated = true;
        }
    }
    if( addressUpdated ){
        emit newInfoMessage( "Data address updated to: " + address );
    }
    return true;
}

bool Core::setDatasetName( const std::string datasetName ){
    std::unique_lock< std::mutex > lock( mutex );
    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            return classificationTrainingData.setDatasetName( datasetName );
        break;
        case REGRESSION_MODE:
            return regressionTrainingData.setDatasetName( datasetName );
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
            //TODO
        break;
        default:
            qDebug() << "ERROR: setDatasetName(std::string datasetName) - Unknown pipeline mode!";
            return false;
        break;
    }
    return false;
}

bool Core::setDatasetInfoText( const std::string infoText ){
    std::unique_lock< std::mutex > lock( mutex );
    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            return classificationTrainingData.setInfoText( infoText );
        break;
        case REGRESSION_MODE:
            return regressionTrainingData.setInfoText( infoText );
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
            return timeseriesClassificationTrainingData.setInfoText( infoText );
        break;
        default:
            qDebug() << "ERROR: setDatasetInfoText(std::string infoText) - Unknown pipeline mode!";
            return false;
        break;
    }
    return false;
}

bool Core::setTrainingClassLabel( const int trainingClassLabel ){
    bool classLabelUpdated = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        if( (unsigned int)trainingClassLabel != this->trainingClassLabel ){
            this->trainingClassLabel = (unsigned int)trainingClassLabel;
            classLabelUpdated = true;
        }
     }
     if( classLabelUpdated ){
        emit trainingClassLabelChanged( trainingClassLabel );
     }
     return true;
}

bool Core::removeAllPreProcessingModules(){
    bool result = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.removeAllPreProcessingModules();
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::removeAllPostProcessingModules(){
    bool result = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.removeAllPostProcessingModules();
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::setPreProcessing( const GRT::PreProcessing &preProcessing ){
    bool result = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.setPreProcessingModule( preProcessing );
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::setFeatureExtraction( const GRT::FeatureExtraction &featureExtraction ){
    bool result = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.setFeatureExtractionModule( featureExtraction );
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::setClassifier( const GRT::Classifier &classifier ){

    bool result = false;
    bool timeseriesCompatible = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.setClassifier( classifier );
        timeseriesCompatible = classifier.getTimeseriesCompatible();

        //Register the training results callback
        GRT::Classifier *ptr = pipeline.getClassifier();
        ptr->removeAllTrainingObservers();
        ptr->removeAllTestObservers();
        ptr->registerTrainingResultsObserver( *this );
        ptr->registerTestResultsObserver( *this );
    }

    if( result ){
        //TODO - Should this setPipelineMode happen here?
        if( timeseriesCompatible ) setPipelineMode( TIMESERIES_CLASSIFICATION_MODE );
        else setPipelineMode( CLASSIFICATION_MODE );
    }

    emit pipelineConfigurationChanged();

    return result;
}

bool Core::setRegressifier( const GRT::Regressifier &regressifier ){

    bool result = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.setRegressifier( regressifier );

        //Register the training results callback
        GRT::Regressifier *ptr = pipeline.getRegressifier();
        ptr->removeAllTrainingObservers();
        ptr->removeAllTestObservers();
        ptr->registerTrainingResultsObserver( *this );
        ptr->registerTestResultsObserver( *this );
    }

    if( result ){
        //Set the pipeline mode into regression mode
        setPipelineMode( REGRESSION_MODE );
    }

    emit pipelineConfigurationChanged();

    return result;
}

bool Core::setClusterer( const GRT::Clusterer &clusterer ){

    bool result = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.setClusterer( clusterer );

        //Register the training results callback
        GRT::Clusterer *ptr = pipeline.getClusterer();
        ptr->removeAllTrainingObservers();
        ptr->removeAllTestObservers();
        ptr->registerTrainingResultsObserver( *this );
        ptr->registerTestResultsObserver( *this );
    }

    if( result ){
        //Set the pipeline mode into cluster mode
        setPipelineMode( CLUSTER_MODE );
    }

    emit pipelineConfigurationChanged();

    return result;
}

bool Core::setPostProcessing( const GRT::PostProcessing &postProcessing ){
    bool result = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.setPostProcessingModule( postProcessing );
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::setPipeline( const GRT::GestureRecognitionPipeline &pipeline ){
    {
        std::unique_lock< std::mutex > lock( mutex );
        this->pipeline = pipeline;
    }
    emit pipelineConfigurationChanged();
    return true;
}

bool Core::setTargetVector( const GRT::VectorFloat &targetVector_ ){

    bool emitTargetDataChanged = false;
    {
        std::unique_lock< std::mutex > lock( mutex );
        if( targetVector_.size() == targetVectorSize ){
            targetVector = targetVector_;
            emitTargetDataChanged = true;
        }
    }

    if( emitTargetDataChanged ){
        emit targetDataChanged( targetVector_ );
        return true;
    }
    return false;
}

bool Core::savePipelineToFile( const std::string filename ){

    bool result = false;

    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.save( filename );
    }

    if( result ){
        emit newInfoMessage( "Pipeline saved to file" );
    }else emit newWarningMessage( "WARNING: Failed to save pipeline to file" );

    return result;
}

bool Core::loadPipelineFromFile( const std::string filename ){

    bool result = false;

    {
        std::unique_lock< std::mutex > lock( mutex );
        result = pipeline.load( filename );
    }

    if( result ){
        emit newInfoMessage( "Pipeline loaded from file" );
        emit pipelineConfigurationChanged();
    }else emit newWarningMessage( "WARNING: Failed to load pipeline from file" );

    return result;
}

bool Core::setInfoMessage( const std::string infoMessage ){
    std::unique_lock< std::mutex > lock( mutex );
    this->infoMessage = infoMessage;
    return true;
}

std::string Core::getInfoMessage(){
    std::unique_lock< std::mutex > lock( mutex );
    return infoMessage;
}

std::string Core::getVersion(){
    std::unique_lock< std::mutex > lock( mutex );
    return version;
}

std::string Core::getIncomingDataAddress(){
    std::unique_lock< std::mutex > lock( mutex );
    return incomingDataAddress;
}

///////////////////////////////////////////////////////////////////////////
//////////////////////////// PROTECTED METHODS ////////////////////////////
///////////////////////////////////////////////////////////////////////////

void Core::notify( const GRT::TrainingResult &data ){
    emit newTrainingResultReceived( data );
}

void Core::notify( const GRT::TestInstanceResult &data ){
    emit newTestInstanceResultReceived( data );
}

void Core::mainThreadFunction(){

    //Try and start the OSC server
    oscServer.setIncomingDataPort( incomingOSCDataPort );
    if( !oscServer.start() ){
        emit newErrorMessage( "ERROR: Failed to start OSC server - the port is in use" );
        return;
    }

    if( !trainingThread.start() ){
        emit newErrorMessage( "ERROR: Failed to start training thread!" );
        return;
    }

    unsigned int coreSleepTime_ = 0;

    //Flag that the core is running
    {
        std::unique_lock< std::mutex > lock( mutex );
        coreRunning = true;
        stopMainThread = false;
        coreSleepTime_ = coreSleepTime;
    }

    //Flag that the core is now running
    emit coreStarted();

    bool keepRunning = true;
    int statusMessageCounter = 0;
    int statusMessageSendRate = 10;
    newDataReceived = false;

    while( keepRunning ){

        //qDebug() << "tick\n";

        tick();

        //Process any new OSC messages
        while( oscServer.getNumMessages() > 0 ){
            processOSCMessage( oscServer.getNextMessage() );
        }

        processNewData();

        if( ++statusMessageCounter >= statusMessageSendRate ){
            statusMessageCounter = 0;
            int pipelineMode = getPipelineMode();
            int pipelineTrained = getTrained() ? 1 : 0;
            int recording = getRecordStatus() ? 1 : 0;
            int numTrainingSamples = getNumTrainingSamples();
            int numClassesInTrainingData = getNumClassesInTrainingData();
            std::string infoMessage = getInfoMessage();
            std::string version = getVersion();

            sendStatusMessage(pipelineMode,pipelineTrained,recording,numTrainingSamples,numClassesInTrainingData,infoMessage,version);
        }

        //Let the thread sleep so we don't kill the CPU
        std::this_thread::sleep_for( std::chrono::milliseconds( coreSleepTime_ ) );

        //Check to see if we should stop the thread
        {
            std::unique_lock< std::mutex > lock( mutex );
            if( stopMainThread ){
                keepRunning = false;
            }else{
                coreSleepTime_ = coreSleepTime;
            }
        }
    }

    //Stop the OSC Server
    oscServer.stop();

    //Stop the training thread
    if( !trainingThread.stop() ){
        emit newErrorMessage( "WARNING: Failed to stop training thread!" );
    }

    //Flag that the core has stopped
    {
        std::unique_lock< std::mutex > lock( mutex );
        coreRunning = false;
        stopMainThread = false;
    }

    //Signal that the core has stopped
    emit coreStopped();
}

bool Core::processOSCMessage( const OSCMessagePtr oscMessage  ){

    bool allowOSCInput = true;
    bool allowOSCControlCommands = true;
    string dataAddress = "";
    const OSCMessage &m = *oscMessage;

    //Safetly get a copy of the varibles we need to check
    {
        std::unique_lock< std::mutex > lock( mutex );
        allowOSCInput = enableOSCInput;
        allowOSCControlCommands = enableOSCControlCommands;
        dataAddress = incomingDataAddress;
    }

    //If we are not allowing any OSC input then there is nothing todo
    if( !allowOSCInput ){
        return true;
    }

    //Check if the training thread is computing a new model
    bool trainingInProcess = trainingThread.getTrainingInProcess();
    if( trainingInProcess ){
        return true;
    }

    //Emit the OSC message
    string msg = "[IN] " + m.getAddressPattern();
    for(unsigned int i=0; i<m.getNumArgs(); i++){
        msg += " [" + GRT::Util::toString( i ) + "]: ";

        if( m[i].getIsInt() ){
            msg += GRT::Util::toString( m[i].getInt() );
        }
        if( m[i].getIsFloat() ){
            msg += GRT::Util::toString( m[i].getFloat() );
        }
        if( m[i].getIsDouble() ){
            msg += GRT::Util::toString( m[i].getDouble() );
        }

        msg += " ";
    }
    emit newOSCMessage( msg );

    if( m.getAddressPattern() == "/Setup" && allowOSCControlCommands ){
        if( m.getNumArgs() == 3 ){
            emit newInfoMessage( "Got OSC Setup Message" );
            unsigned int pipelineMode_ = m[0].getInt();
            unsigned int numInputDimensions_ = m[1].getInt();
            unsigned int targetVectorSize_ = m[2].getInt();
            if( setPipelineMode( pipelineMode_ ) ){
                setNumInputDimensions( numInputDimensions_ );
                setTargetVectorSize( targetVectorSize_ );
            }else newErrorMessage( "Failed to set pipeline mode - invalid OSC /Setup message!" );

            return true;
        }else return false;
    }

    if( m.getAddressPattern() == dataAddress ){
        bool emitInputDataChanged = false;
        bool emitInputDataSizeWarning = false;
        GRT::VectorFloat newInputData_;

        {
            std::unique_lock< std::mutex > lock( mutex );
            if( m.getNumArgs() == numInputDimensions ){

                newDataReceived = true;
                emitInputDataChanged = true;
                newInputData_.resize( numInputDimensions );
                for(unsigned int i=0; i<numInputDimensions; i++){
                    if( m[i].getIsFloat() ){ inputData[i] = m[i].getFloat(); }
                    else if( m[i].getIsDouble() ){ inputData[i] = m[i].getDouble(); }
                    else if( m[i].getIsInt() ){ inputData[i] = m[i].getInt(); }
                    newInputData_[i] = inputData[i];
                }


            }else{
                emitInputDataSizeWarning = true;
            }
        }

        if( emitInputDataChanged ){
            emit dataChanged( newInputData_ );
            return true;
        }

        if( emitInputDataSizeWarning ){
            string warningMessage = "WARNING: The OSC input data vector size (";
            warningMessage += GRT::Util::toString( m.getNumArgs() );
            warningMessage += ") does not match the expected input vector size (";
            warningMessage += GRT::Util::toString( getNumInputDimensions() ) + ")!";
            emit newWarningMessage( warningMessage );
            return false;
        }
    }

    if( m.getAddressPattern() == "/TrainingClassLabel" && allowOSCControlCommands ){
        if( m.getNumArgs() == 1 ){
            {
                std::unique_lock< std::mutex > lock( mutex );
                trainingClassLabel = m[0].getInt();
            }
            emit trainingClassLabelChanged( m[0].getInt() );
            return true;
        }else return false;
    }

    if( m.getAddressPattern() == "/TargetVector" && allowOSCControlCommands ){
        if( m.getNumArgs() == targetVectorSize ){
            {
                std::unique_lock< std::mutex > lock( mutex );
                for(unsigned int i=0; i<targetVectorSize; i++){
                    if( m[i].getIsFloat() ){ targetVector[i] = m[i].getFloat(); }
                    else if( m[i].getIsDouble() ){ targetVector[i] = m[i].getDouble(); }
                    else if( m[i].getIsInt() ){ targetVector[i] = m[i].getInt(); }
                }
            }
            emit targetDataChanged( getTargetVector() );
        }else{
            emit newWarningMessage( "WARNING: The target vector size does not match!" );
            return false;
        }
    }

    if( m.getAddressPattern() == "/Record" && allowOSCControlCommands ){
        if( m.getNumArgs() == 1 ){

            setRecordingState( (m[0].getInt() == 1 ? true : false) );
            bool recordTrainingData_ = false;
            {
                std::unique_lock< std::mutex > lock( mutex );
                recordTrainingData_ = recordTrainingData;
            }
            emit recordStatusChanged( recordTrainingData_ );
        }else return false;
    }

    if( m.getAddressPattern() == "/SaveTrainingDatasetToFile" && allowOSCControlCommands ){
        if( m.getNumArgs() == 1 ){
            //Don't use the mutex here as the saveTrainingDatasetToFile function uses one!
            std::string filename = m[0].getString();
            return saveTrainingDatasetToFile( filename );
        }else return false;
    }

    if( m.getAddressPattern() == "/LoadTrainingDatasetFromFile" && allowOSCControlCommands ){
        if( m.getNumArgs() == 1 ){
            //Don't use the mutex here as the loadTrainingDatasetFromFile function uses one!
            std::string filename = m[0].getString();
            return loadTrainingDatasetFromFile( filename );
        }else return false;
    }

    if( m.getAddressPattern() == "/ClearTrainingDataset" && allowOSCControlCommands ){
        clearTrainingData();
    }

    if( m.getAddressPattern() == "/SetClassifier" && allowOSCControlCommands ){
        if( m.getNumArgs() >= 4 ){
            //Get the first 4 parameters, these are classifier type, useScaling, useNullRejection, and null rejection coeff
            unsigned int classifierType = m[0].getInt();
            bool useScaling = m[1].getInt() == 1 ? true : false;
            bool useNullRejection = m[2].getInt() == 1 ? true : false;
            double nullRejectionCoeff = m[3].getDouble();

            //Get any optional parameters
            double parameter1 = 0;
            if( m.getNumArgs() == 5 ){
                parameter1 = m[4].getDouble();
            }

            //setClassifierMessageReceived(classifierType,useScaling,useNullRejection,nullRejectionCoeff,parameter1);
        }else return false;
    }

    if( m.getAddressPattern() == "/Train" && allowOSCControlCommands ){
        emit trainMessageReceived();
    }

    return true;
}

bool Core::processNewData(){

    bool newSampleAdded = false;
    bool newPreProcessedData = false;
    bool newFeatureData = false;
    bool newClassificationData = false;
    bool newRegressionData = false;
    bool newTimeseriesData = false;
    bool newClusterData = false;
    unsigned int numTrainingSamples = 0;
    unsigned int predictedClassLabel = 0;
    unsigned int predictedClusterLabel = 0;
    double maximumLikelihood = 0;
    double phase = 0;
    GRT::VectorFloat preProcessedData;
    GRT::VectorFloat featureData;
    GRT::VectorFloat classLikelihoods;
    GRT::VectorFloat classDistances;
    GRT::VectorFloat clusterLikelihoods;
    GRT::VectorFloat clusterDistances;
    GRT::VectorFloat regressionData;
    GRT::Vector<unsigned int> classLabels;
    GRT::Vector<unsigned int> clusterLabels;
    GRT::ClassificationSample newSample;
    GRT::RegressionSample newRegressionSample;
    GRT::MatrixFloat newTimeseriesSample;
    GRT::VectorFloat newClusterSample;

    {
        std::unique_lock< std::mutex > lock( mutex );

        if( !newDataReceived ){
            return false;
        }
        newDataReceived = false;

        if( recordTrainingData ){

            switch( pipelineMode ){
                case CLASSIFICATION_MODE:
                    if( classificationTrainingData.addSample(trainingClassLabel,inputData) ){
                        numTrainingSamples = classificationTrainingData.getNumSamples();
                        newSample = classificationTrainingData[classificationTrainingData.getNumSamples()-1];
                        newSampleAdded = true;
                    }
                break;
                case REGRESSION_MODE:
                    if( regressionTrainingData.addSample(inputData,targetVector) ){
                        numTrainingSamples = regressionTrainingData.getNumSamples();
                        newRegressionSample = regressionTrainingData[regressionTrainingData.getNumSamples()-1];
                        newSampleAdded = true;
                    }else newWarningMessage( "Warning: Failed to add regression sample!" );
                break;
                case TIMESERIES_CLASSIFICATION_MODE:
                    if( timeseriesSample.push_back(inputData) ){
                        numTrainingSamples = timeseriesClassificationTrainingData.getNumSamples();
                        newTimeseriesSample = timeseriesSample;
                        newSampleAdded = true;
                    }else newWarningMessage( "Warning: Failed to add timeseries sample!" );
                break;
                case CLUSTER_MODE:
                    if( clusterTrainingData.addSample(inputData) ){
                        numTrainingSamples = clusterTrainingData.getNumSamples();
                        newClusterSample = clusterTrainingData[clusterTrainingData.getNumSamples()-1];
                        newSampleAdded = true;
                    }
                break;
                default:
                    qDebug() << "ERROR: processNewData() - Unknown pipeline mode!";
                    return false;
                break;
            }
        }

        if( predictionModeEnabled ){
            if( pipeline.getTrained() ){
                if( pipeline.predict( inputData ) ){

                    switch( pipelineMode ){
                        case CLASSIFICATION_MODE:
                            predictedClassLabel = pipeline.getPredictedClassLabel();
                            maximumLikelihood = pipeline.getMaximumLikelihood();
                            classLikelihoods = pipeline.getClassLikelihoods();
                            classDistances = pipeline.getClassDistances();
                            classLabels = pipeline.getClassLabels();
                            newClassificationData = true;
                        break;
                        case TIMESERIES_CLASSIFICATION_MODE:
                            predictedClassLabel = pipeline.getPredictedClassLabel();
                            maximumLikelihood = pipeline.getMaximumLikelihood();
                            classLikelihoods = pipeline.getClassLikelihoods();
                            classDistances = pipeline.getClassDistances();
                            classLabels = pipeline.getClassLabels();
                            phase = pipeline.getPhase();
                            newTimeseriesData = true;
                        break;
                        case REGRESSION_MODE:
                            regressionData = pipeline.getRegressionData();
                            newRegressionData = true;
                        break;
                        case CLUSTER_MODE:
                            predictedClusterLabel = pipeline.getPredictedClassLabel();
                            maximumLikelihood = pipeline.getMaximumLikelihood();
                            clusterLikelihoods = pipeline.getClassLikelihoods();
                            clusterDistances = pipeline.getClassDistances();
                            clusterLabels = pipeline.getClassLabels();
                            newClusterData = true;
                        break;
                        default:
                            qDebug() << "ERROR: Unknown pipeline mode!";
                            return false;
                        break;
                    }
                }
            }else{

                if( pipeline.getIsPreProcessingSet() || pipeline.getIsFeatureExtractionSet() ){
                    pipeline.preProcessData( inputData );
                }
            }

            if( pipeline.getIsPreProcessingSet() ){
                newPreProcessedData = true;
                preProcessedData = pipeline.getPreProcessedData();
            }
            if( pipeline.getIsFeatureExtractionSet() ){
                newFeatureData = true;
                featureData = pipeline.getFeatureExtractionData();
            }
        }

    }//End of mutex scope

    if( newSampleAdded ){
        emit numTrainingSamplesChanged( numTrainingSamples );
        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                emit newTrainingSampleAdded( numTrainingSamples, newSample );
            break;
            case REGRESSION_MODE:
                emit newTrainingSampleAdded( numTrainingSamples, newRegressionSample );
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                emit newTrainingSampleAdded( newTimeseriesSample );
            break;
            case CLUSTER_MODE:
                emit newTrainingSampleAdded( newClusterSample );
            break;
            default:
                qDebug() << "ERROR: Unknown pipeline mode!";
                return false;
            break;
        }
    }

    if( newPreProcessedData ){
        //Send the pre processed data out over OSC
        sendPreProcessedData( preProcessedData );
        emit preProcessingDataChanged( preProcessedData );
    }

    if( newFeatureData ){
        //Send the feature extraction data out over OSC
        sendFeatureExtractionData( featureData );
        emit featureExtractionDataChanged( featureData );
    }

    if( newClassificationData ){
        //Send the results out over OSC
        sendPredictionResults( predictedClassLabel, maximumLikelihood );
        sendClassLikelihoods( classLikelihoods );
        sendClassDistances( classDistances );
        sendClassLabels( classLabels );
        emit classificationResultsChanged(predictedClassLabel,maximumLikelihood,classLikelihoods,classDistances,classLabels);
    }

    if( newRegressionData ){
        sendRegressionData( regressionData );
        emit regressionResultsChanged(regressionData);
    }

    if( newTimeseriesData ){
        //Send the results out over OSC
        sendPredictionResults( predictedClassLabel, maximumLikelihood );
        sendClassLikelihoods( classLikelihoods );
        sendClassDistances( classDistances );
        sendClassLabels( classLabels );
        emit timeseriesClassificationResultsChanged(predictedClassLabel,maximumLikelihood,phase,classLikelihoods,classDistances,classLabels);
    }

    if( newClusterData ){
        //sendClusterData( clusterData );
        emit clusterResultsChanged(predictedClusterLabel,maximumLikelihood,clusterLikelihoods,clusterDistances,clusterLabels);
    }

    return true;
}

bool Core::train(){

    //Check to make sure we are not already training something
    if( trainingThread.getTrainingInProcess() ){
        emit newInfoMessage( "Can't start training, training already in process" );
        return false;
    }

    std::unique_lock< std::mutex > lock( mutex );

    //Launch a new training phase
    bool result = false;
    GRT::Trainer trainer;
    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            trainer.setupNoValidationTraining( pipeline, classificationTrainingData );
            result = trainingThread.startNewTraining( trainer );
        break;
        case REGRESSION_MODE:
            trainer.setupNoValidationTraining( pipeline, regressionTrainingData );
            result = trainingThread.startNewTraining( trainer );
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
            trainer.setupNoValidationTraining( pipeline, timeseriesClassificationTrainingData );
            result = trainingThread.startNewTraining( trainer );
        break;
        case CLUSTER_MODE:
            trainer.setupNoValidationTraining( pipeline, clusterTrainingData );
            result = trainingThread.startNewTraining( trainer );
        break;
        default:
            qDebug() << "ERROR: Unknown pipeline mode!";
            return false;
        break;
    }

    return result;
}

bool Core::trainAndTestOnRandomSubset( const unsigned int randomTestSubsetPercentage,const bool useStratifiedSampling ){

    //Check to make sure we are not already training something
    if( getTrainingInProcess() ){
        emit newInfoMessage( "Can't start training, training already in process" );
        return false;
    }

    std::unique_lock< std::mutex > lock( mutex );

    bool result = false;
    GRT::Trainer trainer;
    GRT::ClassificationData tempClassificationTrainingData;
    GRT::ClassificationData tempClassificationTestData;
    GRT::RegressionData tempRegressionTrainingData;
    GRT::RegressionData tempRegressionTestData;

    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            tempClassificationTrainingData = classificationTrainingData;
            tempClassificationTestData = tempClassificationTrainingData.partition( 100-randomTestSubsetPercentage, useStratifiedSampling );
            trainer.setupTrainingAndTesting( pipeline, tempClassificationTrainingData, tempClassificationTestData );
            result = trainingThread.startNewTraining( trainer );
        break;
        case REGRESSION_MODE:
            tempRegressionTrainingData = regressionTrainingData;
            tempRegressionTestData = tempRegressionTrainingData.partition( 100-randomTestSubsetPercentage );
            trainer.setupTrainingAndTesting( pipeline, tempRegressionTrainingData, tempRegressionTestData );
            result = trainingThread.startNewTraining( trainer );
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
        break;
        default:
            qDebug() << "ERROR: Unknown pipeline mode!";
            return false;
        break;
    }

    return result;
}

bool Core::trainAndTestOnTestDataset(){

    //Check to make sure we are not already training something
    if( trainingThread.getTrainingInProcess() ){
        emit newInfoMessage( "Can't start training, training already in process" );
        return false;
    }

    if( getNumTestSamples() > 0 ){

        std::unique_lock< std::mutex > lock( mutex );

        GRT::Trainer trainer;

        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                trainer.setupTrainingAndTesting( pipeline, classificationTrainingData, classificationTestData );
                return trainingThread.startNewTraining( trainer );
            break;
            case REGRESSION_MODE:
                trainer.setupTrainingAndTesting( pipeline, regressionTrainingData, regressionTestData );
                return trainingThread.startNewTraining( trainer );
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                //result = pipeline.train( timeseriesClassificationTrainingData );
            break;
            default:
                qDebug() << "ERROR: Unknown pipeline mode!";
                return false;
            break;
        }
    }
    return false;
}

bool Core::trainWithCrossValidation( const unsigned int numFolds ){

    //Check to make sure we are not already training something
    if( trainingThread.getTrainingInProcess() ){
        emit newInfoMessage( "Can't start training, training already in process" );
        return false;
    }

    std::unique_lock< std::mutex > lock( mutex );

    bool result = false;
    GRT::Trainer trainer;

    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            trainer.setupCVTraining( pipeline, classificationTrainingData, numFolds, false );
            result = trainingThread.startNewTraining( trainer );
        break;
        case REGRESSION_MODE:
            trainer.setupCVTraining( pipeline, regressionTrainingData, numFolds );
            result = trainingThread.startNewTraining( trainer );
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
            //trainer.setupCVTraining( pipeline, timeseriesClassificationTrainingData, numFolds );
            //result = trainingThread.startNewTraining( trainer );
        break;
        default:
            qDebug() << "ERROR: Unknown pipeline mode!";
            return false;
        break;
    }

    return result;
}

bool Core::enablePrediction( const bool state ){
    std::unique_lock< std::mutex > lock( mutex );
    predictionModeEnabled = state;
    return true;
}

void Core::sendPreProcessedData( const GRT::VectorFloat &preProcessedData ){

     const unsigned int msgBufferSize = 1024;
     char msgBuffer[ msgBufferSize ];
     osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    const unsigned int N = (unsigned int)preProcessedData.size();
    msg << osc::BeginMessage( "/PreProcessedData" ) << int( N );
    for(unsigned int i=0; i<N; i++){
        msg << float( preProcessedData[i] );
    }
    msg << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );
}

void Core::sendFeatureExtractionData( const GRT::VectorFloat &featureData ){

    const unsigned int msgBufferSize = 1024;
    char msgBuffer[ msgBufferSize ];
    osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    const unsigned int N = (unsigned int)featureData.size();
    msg << osc::BeginMessage( "/FeatureExtractionData" ) << int( N );
    for(unsigned int i=0; i<N; i++){
        msg << float( featureData[i] );
    }
    msg << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );
}

void Core::sendPredictionResults( const unsigned int predictedClassLabel, const double maximumLikelihood ){

    const unsigned int msgBufferSize = 1024;
    char msgBuffer[ msgBufferSize ];
    osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    msg << osc::BeginMessage( "/Prediction" ) << int( predictedClassLabel ) << float( maximumLikelihood ) << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );

}

void Core::sendClassLikelihoods( const GRT::VectorFloat &classLikelihoods ){

    const unsigned int msgBufferSize = 1024;
    char msgBuffer[ msgBufferSize ];
    osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    const unsigned int N = (unsigned int)classLikelihoods.size();
    msg << osc::BeginMessage( "/ClassLikelihoods" ) << int( N );
    for(unsigned int i=0; i<N; i++){
        msg << float( classLikelihoods[i] );
    }
    msg << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );
}

void Core::sendClassDistances( const GRT::VectorFloat &classDistances ){

    const unsigned int msgBufferSize = 1024;
    char msgBuffer[ msgBufferSize ];
    osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    const unsigned int N = (unsigned int)classDistances.size();
    msg << osc::BeginMessage( "/ClassDistances" ) << int( N );
    for(unsigned int i=0; i<N; i++){
        msg << float( classDistances[i] );
    }
    msg << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );
}

void Core::sendClassLabels( const vector< unsigned int > &classLabels ){

    const unsigned int msgBufferSize = 1024;
    char msgBuffer[ msgBufferSize ];
    osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    const unsigned int N = (unsigned int)classLabels.size();
    msg << osc::BeginMessage( "/ClassLabels" ) << int( N );
    for(unsigned int i=0; i<N; i++){
        msg << int( classLabels[i] );
    }
    msg << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );
}

void Core::sendRegressionData( const GRT::VectorFloat &regressionData ){

    const unsigned int msgBufferSize = 1024;
    char msgBuffer[ msgBufferSize ];
    osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    const unsigned int N = (unsigned int)regressionData.size();
    msg << osc::BeginMessage( "/RegressionData" ) << int( N );
    for(unsigned int i=0; i<N; i++){
        msg << float( regressionData[i] );
    }
    msg << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );
}

void Core::sendStatusMessage( const int pipelineMode,const int trained,const int recording,const int numTrainingSamples,const int numClassesInTrainingData,const std::string infoMessage,const std::string version ){

    const unsigned int msgBufferSize = 1024;
    char msgBuffer[ msgBufferSize ];
    osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    msg << osc::BeginMessage( "/Status" );
    msg << pipelineMode << trained << recording << numTrainingSamples << numClassesInTrainingData << infoMessage.c_str() << version.c_str();
    msg << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );
}
