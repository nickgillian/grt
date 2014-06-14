#include "Core.h"

Core::Core(QObject *parent) : QObject(parent)
{
    coreRunning = false;
    stopMainThread = false;
    verbose = true;
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
    targetVector.resize( targetVectorSize );
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
        if( verbose ){
            qDebug() << "WARNING: Core::start() - The core is already running!" << endl;
            emit newInfoMessage( "WARNING: Failed to start core, it is already running!" );
        }
        return false;
    }

    if( verbose )
        qDebug() << STRING_TO_QSTRING("Core::start() - Starting main thread...");

    try{
        mainThread.reset( new boost::thread( boost::bind( &Core::mainThreadFunction, this) ) );
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
        if( verbose ){
            qDebug() << "WARNING: Core::stop() - The core is not running!" << endl;
            emit newInfoMessage( "WARNING: Failed to stop core, it is not running!" );
        }
        return false;
    }

    if( verbose )
        qDebug() << STRING_TO_QSTRING("Core::stop() - Stopping main thread...");

    //Flag that the core should stop
    {
        boost::mutex::scoped_lock lock( mutex );
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

bool Core::resetOSCClient(std::string clientAddress,int clientPort){

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

bool Core::resetOSCServer(int incomingOSCDataPort){

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

bool Core::setVersion(std::string version){
    boost::mutex::scoped_lock lock( mutex );
    this->version = version;
    return true;
}

bool Core::setEnableOSCInput(bool state){
    boost::mutex::scoped_lock lock( mutex );
    enableOSCInput = state;
    return true;
}

bool Core::setEnableOSCControlCommands(bool state){
    boost::mutex::scoped_lock lock( mutex );
    enableOSCControlCommands = state;
    return true;
}

bool Core::setPipelineMode(unsigned int pipelineMode){
    bool modeChanged = false;
    if( pipelineMode != this->pipelineMode ){
        boost::mutex::scoped_lock lock( mutex );
        this->pipelineMode = pipelineMode;
        modeChanged = true;
    }
    if( modeChanged )
        emit pipelineModeChanged( pipelineMode );
    return true;
}

bool Core::setRecordingState(bool state){
    boost::mutex::scoped_lock lock( mutex );
    recordTrainingData = state;
    return true;
}

bool Core::saveTrainingDatasetToFile(std::string filename){
    bool result = false;

    //Check to see if we should load the data to the default GRT fileformat or as a CSV file
    bool saveToCSV = false;
    std::size_t found = filename.find( ".csv" );
    if ( found!=std::string::npos )
        saveToCSV = true;

    {
        boost::mutex::scoped_lock lock( mutex );

        switch( pipelineMode ){
            case CLASSIFICATION_MODE:
                if( !saveToCSV ) result = classificationTrainingData.saveDatasetToFile(filename);
                else result = classificationTrainingData.saveDatasetToCSVFile(filename);
            break;
            case REGRESSION_MODE:
                if( !saveToCSV ) result = regressionTrainingData.saveDatasetToFile(filename);
                else result = regressionTrainingData.saveDatasetToCSVFile(filename);
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                if( !saveToCSV ) result = timeseriesClassificationTrainingData.saveDatasetToFile(filename);
                //else result = timeseriesClassificationTrainingData.saveDatasetToCSVFile(filename);
            break;
            default:
                qDebug() << "ERROR: Unknown pipeline mode!";
            break;
        }
    }
    if( result ){
        if( !saveToCSV ){ emit newInfoMessage( "Training data saved to file" ); }
        else{ emit newInfoMessage( "Training data saved to CSV file" ); }
    }else{
        if( !saveToCSV ){ emit newInfoMessage( "WARNING: Failed to save training data saved to file" ); }
        else{ emit newInfoMessage( "WARNING: Failed to save training data saved to CSV file" ); }
    }
    emit saveTrainingDataToFileResult( result );
    return result;
}

bool Core::loadTrainingDatasetFromFile(std::string filename){

    bool result = false;
    bool inputDimensionsSizeError = false;
    unsigned int numTrainingSamples = 0;
    unsigned int tempDataSize = 0;
    unsigned int tempPipelineMode = 0;
    GRT::ClassificationData tempClassificationData;
    GRT::RegressionData tempRegressionData;
    GRT::TimeSeriesClassificationData tempTimeSeriesData;

    //Check to see if we should load the data from the default GRT fileformat or as a CSV file
    bool loadFromCSV = false;
    std::size_t found = filename.find( ".csv" );
    if ( found!=std::string::npos )
        loadFromCSV = true;

    {
         boost::mutex::scoped_lock lock( mutex );
         unsigned int trainingDataSize = 0;
         tempPipelineMode = pipelineMode;
         switch( pipelineMode ){
             case CLASSIFICATION_MODE:
                if( !loadFromCSV ) result = classificationTrainingData.loadDatasetFromFile( filename );
                else result = classificationTrainingData.loadDatasetFromCSVFile( filename );

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
                 if( !loadFromCSV ) result = regressionTrainingData.loadDatasetFromFile( filename );
                 else result = regressionTrainingData.loadDatasetFromCSVFile( filename, this->numInputDimensions, this->targetVectorSize );

                 numTrainingSamples = regressionTrainingData.getNumSamples();
                 tempRegressionData = regressionTrainingData;
                 trainingDataSize = regressionTrainingData.getNumInputDimensions();
             break;
             case TIMESERIES_CLASSIFICATION_MODE:
                 if( !loadFromCSV ) result = timeseriesClassificationTrainingData.loadDatasetFromFile( filename );
                 //else result = timeseriesClassificationTrainingData.loadDatasetFromCSVFile( filename );

                 numTrainingSamples = timeseriesClassificationTrainingData.getNumSamples();
                 tempTimeSeriesData = timeseriesClassificationTrainingData;
                 trainingDataSize = timeseriesClassificationTrainingData.getNumDimensions();
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
                //TODO
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

bool Core::loadTestDatasetFromFile(std::string filename){
    bool result = false;
    unsigned int numTestSamples = 0;
    GRT::ClassificationData tempClassificationData;
    GRT::RegressionData tempRegressionData;
    GRT::TimeSeriesClassificationData tempTimeSeriesData;

    {
         boost::mutex::scoped_lock lock( mutex );

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
             default:
                 qDebug() << "ERROR: Unknown pipeline mode!";
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
            default:
                qDebug() << "ERROR: Unknown pipeline mode!";
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

    {
        boost::mutex::scoped_lock lock( mutex );
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
            default:
                qDebug() << "ERROR: Unknown pipeline mode!";
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
            //TODO
        break;
        default:
            qDebug() << "ERROR: Unknown pipeline mode!";
            return;
        break;
    }
}

///////////////////////////////////////////////////////////////////////////
////////////////////////////      GETTERS      ////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool Core::getCoreRunning(){
    boost::mutex::scoped_lock lock( mutex );
    return coreRunning;
}

bool Core::getTrained(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline.getTrained();
}

bool Core::getRecordStatus(){
    boost::mutex::scoped_lock lock( mutex );
    return recordTrainingData;
}

unsigned int Core::getPipelineMode(){
    boost::mutex::scoped_lock lock( mutex );
    return pipelineMode;
}

unsigned int Core::getTrainingClassLabel(){
    boost::mutex::scoped_lock lock( mutex );
    return trainingClassLabel;
}

unsigned int Core::getNumTrainingSamples(){
    boost::mutex::scoped_lock lock( mutex );
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
        default:
            qDebug() << "ERROR: Unknown pipeline mode!";
            return false;
        break;
    }
}

unsigned int Core::getNumTestSamples(){
    boost::mutex::scoped_lock lock( mutex );
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
        default:
            qDebug() << "ERROR: Unknown pipeline mode!";
            return false;
        break;
    }
}

unsigned int Core::getNumClasses(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline.getNumClasses();
}

unsigned int Core::getNumClassesInTrainingData(){
    boost::mutex::scoped_lock lock( mutex );
    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            return classificationTrainingData.getNumClasses();
        break;
        case REGRESSION_MODE:
            return 0;
        break;
        case TIMESERIES_CLASSIFICATION_MODE:
            return timeseriesClassificationTrainingData.getNumClasses();
        break;
        default:
            qDebug() << "ERROR: Unknown pipeline mode!";
            return false;
        break;
    }
    return 0;
}

vector<unsigned int> Core::getClassLabels(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline.getClassLabels();
}

GRT::ClassificationData Core::getClassificationTrainingData(){
    boost::mutex::scoped_lock lock( mutex );
    return classificationTrainingData;
}

GRT::ClassificationData Core::getClassificationTestData(){
    boost::mutex::scoped_lock lock( mutex );
    return classificationTestData;
}

GRT::RegressionData Core::getRegressionTrainingData(){
    boost::mutex::scoped_lock lock( mutex );
    return regressionTrainingData;
}

GRT::RegressionData Core::getRegressionTestData(){
    boost::mutex::scoped_lock lock( mutex );
    return regressionTestData;
}

double Core::getTestAccuracy(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline.getTestAccuracy();
}

double Core::getCrossValidationAccuracy(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline.getCrossValidationAccuracy();
}

double Core::getTrainingRMSError(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline.getTrainingRMSError();
}

double Core::getTrainingSSError(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline.getTrainingSSError();
}

GRT::GestureRecognitionPipeline Core::getPipeline(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline;
}

GRT::TestResult Core::getTestResults(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline.getTestResults();
}

vector< GRT::TestResult > Core::getCrossValidationResults(){
    boost::mutex::scoped_lock lock( mutex );
    return pipeline.getCrossValidationResults();
}

///////////////////////////////////////////////////////////////////////////
////////////////////////////      SETTERS      ////////////////////////////
///////////////////////////////////////////////////////////////////////////

bool Core::setNumInputDimensions(int numInputDimensions){

    bool result = false;
    GRT::ClassificationData tempClassificationData;
    GRT::RegressionData tempRegressionData;
    GRT::TimeSeriesClassificationData tempTimeSeriesData;

    {
        boost::mutex::scoped_lock lock( mutex );
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
                regressionTrainingData.clear();
                regressionTrainingData.setInputAndTargetDimensions( numInputDimensions, targetVectorSize );
                tempRegressionData = regressionTrainingData;
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                timeseriesClassificationTrainingData.clear();
                timeseriesClassificationTrainingData.setNumDimensions( numInputDimensions );
                tempTimeSeriesData = timeseriesClassificationTrainingData;
            break;
            default:
                qDebug() << "ERROR: Unknown pipeline mode!";
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
            break;
            case TIMESERIES_CLASSIFICATION_MODE:
                //TODO
            break;
            default:
                qDebug() << "ERROR: Unknown pipeline mode!";
                return false;
            break;
        }
    }

    return result;
}

bool Core::setTargetVectorSize(int targetVectorSize){

    bool result = false;
    GRT::LabelledRegressionData tempRegressionData;

    {
        boost::mutex::scoped_lock lock( mutex );
        this->targetVectorSize = (unsigned int)targetVectorSize;
        targetVector.clear();
        targetVector.resize( targetVectorSize );
        regressionTrainingData.clear();
        regressionTrainingData.setInputAndTargetDimensions( numInputDimensions, targetVectorSize );
        tempRegressionData = regressionTrainingData;
        result = true;
    }

    if( result ){
        emit numTargetDimensionsChanged( targetVectorSize );
        emit trainingDataReset( tempRegressionData );
    }

    return true;
}

bool Core::setMainDataAddress(std::string address){
    bool addressUpdated = false;
    {
        boost::mutex::scoped_lock lock( mutex );
        if( this->incomingDataAddress != address ){
            this->incomingDataAddress = address;

        }
    }
    if( addressUpdated ){
        emit newInfoMessage( "Data address updated" );
    }
    return true;
}

bool Core::setDatasetName(std::string datasetName){
    boost::mutex::scoped_lock lock( mutex );
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
            qDebug() << "ERROR: Unknown pipeline mode!";
            return false;
        break;
    }
    return false;
}

bool Core::setDatasetInfoText(std::string infoText){
    boost::mutex::scoped_lock lock( mutex );
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
            qDebug() << "ERROR: Unknown pipeline mode!";
            return false;
        break;
    }
    return false;
}

bool Core::setTrainingClassLabel(int trainingClassLabel){
    bool classLabelUpdated = false;
    {
        boost::mutex::scoped_lock lock( mutex );
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
        boost::mutex::scoped_lock lock( mutex );
        result = pipeline.removeAllPreProcessingModules();
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::removeAllPostProcessingModules(){
    bool result = false;
    {
        boost::mutex::scoped_lock lock( mutex );
        result = pipeline.removeAllPostProcessingModules();
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::setPreProcessing( const GRT::PreProcessing &preProcessing ){
    bool result = false;
    {
        boost::mutex::scoped_lock lock( mutex );
        result = pipeline.setPreProcessingModule( preProcessing );
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::setFeatureExtraction( const GRT::FeatureExtraction &featureExtraction ){
    bool result = false;
    {
        boost::mutex::scoped_lock lock( mutex );
        result = pipeline.setFeatureExtractionModule( featureExtraction );
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::setClassifier( const GRT::Classifier &classifier ){

    bool result = false;
    bool timeseriesCompatible = false;
    {
        boost::mutex::scoped_lock lock( mutex );
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
        if( timeseriesCompatible ) setPipelineMode( TIMESERIES_CLASSIFICATION_MODE );
        else setPipelineMode( CLASSIFICATION_MODE );
    }

    emit pipelineConfigurationChanged();

    return result;
}

bool Core::setRegressifier( const GRT::Regressifier &regressifier ){

    bool result = false;
    {
        boost::mutex::scoped_lock lock( mutex );
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

bool Core::setPostProcessing( const GRT::PostProcessing &postProcessing ){
    bool result = false;
    {
        boost::mutex::scoped_lock lock( mutex );
        result = pipeline.setPostProcessingModule( postProcessing );
    }
    emit pipelineConfigurationChanged();
    return result;
}

bool Core::setPipeline( const GRT::GestureRecognitionPipeline &pipeline ){
    {
        boost::mutex::scoped_lock lock( mutex );
        this->pipeline = pipeline;
    }
    emit pipelineConfigurationChanged();
    return true;
}

bool Core::savePipelineToFile(std::string filename){

    bool result = false;

    {
        boost::mutex::scoped_lock lock( mutex );
        result = pipeline.savePipelineToFile( filename );
    }

    if( result ){
        emit newInfoMessage( "Pipeline saved to file" );
    }else emit newWarningMessage( "WARNING: Failed to save pipeline to file" );

    return result;
}

bool Core::loadPipelineFromFile(std::string filename){

    bool result = false;

    {
        boost::mutex::scoped_lock lock( mutex );
        result = pipeline.loadPipelineFromFile( filename );
    }

    if( result ){
        emit newInfoMessage( "Pipeline loaded from file" );
        emit pipelineConfigurationChanged();
    }else emit newWarningMessage( "WARNING: Failed to load pipeline from file" );

    return result;
}

bool Core::setInfoMessage(std::string infoMessage){
    boost::mutex::scoped_lock lock( mutex );
    this->infoMessage = infoMessage;
    return true;
}

std::string Core::getInfoMessage(){
    boost::mutex::scoped_lock lock( mutex );
    return infoMessage;
}

std::string Core::getVersion(){
    boost::mutex::scoped_lock lock( mutex );
    return version;
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

    //Flag that the core is running
    {
        boost::mutex::scoped_lock lock( mutex );
        coreRunning = true;
        stopMainThread = false;
    }

    //Flag that the core is now running
    emit coreStarted();

    bool keepRunning = true;
    int statusMessageCounter = 0;
    int statusMessageSendRate = 10;
    newDataReceived = false;

    while( keepRunning ){

        //qDebug() << "tick\n";

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
        boost::this_thread::sleep( boost::posix_time::milliseconds( DEFAULT_CORE_THREAD_SLEEP_TIME ) );

        //Check to see if we should stop the thread
        {
            boost::mutex::scoped_lock lock( mutex );
            if( stopMainThread ){
                keepRunning = false;
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
        boost::mutex::scoped_lock lock( mutex );
        coreRunning = false;
        stopMainThread = false;
    }

    //Signal that the core has stopped
    emit coreStopped();
}

bool Core::processOSCMessage( const OSCMessage &m ){

    bool allowOSCInput = true;
    bool allowOSCControlCommands = true;
    string dataAddress = "";

    {
        boost::mutex::scoped_lock lock( mutex );
        allowOSCInput = enableOSCInput;
        allowOSCControlCommands = enableOSCControlCommands;
        dataAddress = incomingDataAddress;
    }

    //If we are not allowing any OSC input then there is nothing todo
    if( !allowOSCInput ){
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
            unsigned int tempPipelineMode = m[0].getInt();
            unsigned int tempNumInputDimensions = m[1].getInt();
            unsigned int tempTargetVectorSize = m[2].getInt();
            setPipelineMode( tempPipelineMode );
            setNumInputDimensions( tempNumInputDimensions );
            setTargetVectorSize( tempTargetVectorSize );
            return true;
        }else return false;
    }

    if( m.getAddressPattern() == dataAddress ){
        boost::mutex::scoped_lock lock( mutex );
        if( m.getNumArgs() == numInputDimensions ){

            newDataReceived = true;
            for(unsigned int i=0; i<numInputDimensions; i++){
                if( m[i].getIsFloat() ){ inputData[i] = m[i].getFloat(); }
                else if( m[i].getIsDouble() ){ inputData[i] = m[i].getDouble(); }
                else if( m[i].getIsInt() ){ inputData[i] = m[i].getInt(); }
            }

            emit dataChanged( inputData );
        }else{
            //emit newWarningMessage( "WARNING: The data vector size does not match!" );
            return false;
        }
    }

    if( m.getAddressPattern() == "/TrainingClassLabel" && allowOSCControlCommands ){
        if( m.getNumArgs() == 1 ){
            {
                boost::mutex::scoped_lock lock( mutex );
                trainingClassLabel = m[0].getInt();
            }
            emit trainingClassLabelChanged( trainingClassLabel );
        }else return false;
    }

    if( m.getAddressPattern() == "/TargetVector" && allowOSCControlCommands ){
        if( m.getNumArgs() == targetVectorSize ){
            {
                boost::mutex::scoped_lock lock( mutex );
                for(unsigned int i=0; i<targetVectorSize; i++){
                    if( m[i].getIsFloat() ){ targetVector[i] = m[i].getFloat(); }
                    else if( m[i].getIsDouble() ){ targetVector[i] = m[i].getDouble(); }
                    else if( m[i].getIsInt() ){ targetVector[i] = m[i].getInt(); }
                }
            }
            emit targetDataChanged( targetVector );
        }else{
            emit newWarningMessage( "WARNING: The target vector size does not match!" );
            return false;
        }
    }

    if( m.getAddressPattern() == "/Record" && allowOSCControlCommands ){
        if( m.getNumArgs() == 1 ){
            {
                boost::mutex::scoped_lock lock( mutex );
                recordTrainingData = (m[0].getInt() == 1 ? true : false);
            }
            emit recordStatusChanged( recordTrainingData );
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

            setClassifierMessageReceived(classifierType,useScaling,useNullRejection,nullRejectionCoeff,parameter1);
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
    bool newPredictionData = false;
    bool newRegressionData = false;
    unsigned int numTrainingSamples = 0;
    unsigned int predictedClassLabel = 0;
    double maximumLikelihood = 0;
    GRT::VectorDouble preProcessedData;
    GRT::VectorDouble featureData;
    GRT::VectorDouble classLikelihoods;
    GRT::VectorDouble classDistances;
    GRT::VectorDouble regressionData;
    GRT::vector<GRT::UINT> classLabels;
    GRT::ClassificationSample newSample;
    GRT::RegressionSample newRegressionSample;

    {
        boost::mutex::scoped_lock lock( mutex );

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
                break;
                default:
                    qDebug() << "ERROR: Unknown pipeline mode!";
                    return false;
                break;
            }
        }

        if( predictionModeEnabled ){
            if( pipeline.getTrained() ){
                if( pipeline.predict( inputData ) ){

                    switch( pipelineMode ){
                        case CLASSIFICATION_MODE:
                        case TIMESERIES_CLASSIFICATION_MODE:
                            predictedClassLabel = pipeline.getPredictedClassLabel();
                            maximumLikelihood = pipeline.getMaximumLikelihood();
                            classLikelihoods = pipeline.getClassLikelihoods();
                            classDistances = pipeline.getClassDistances();
                            classLabels = pipeline.getClassLabels();
                            newPredictionData = true;
                        break;
                        case REGRESSION_MODE:
                            regressionData = pipeline.getRegressionData();
                            newRegressionData = true;
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

    if( newPredictionData ){
        //Send the results out over OSC
        sendPredictionResults( predictedClassLabel, maximumLikelihood );
        sendClassLikelihoods( classLikelihoods );
        sendClassDistances( classDistances );
        sendClassLabels( classLabels );
        emit predictionResultsChanged(predictedClassLabel,maximumLikelihood,classLikelihoods,classDistances,classLabels);
    }

    if( newRegressionData ){
        sendRegressionData( regressionData );
        emit regressionResultsChanged(regressionData);
    }

    return true;
}

bool Core::train(){

    //Check to make sure we are not already training something
    if( trainingThread.getTrainingInProcess() ){
        emit newInfoMessage( "Can't start training, training already in process" );
        return false;
    }

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
        break;
        default:
            qDebug() << "ERROR: Unknown pipeline mode!";
            return false;
        break;
    }

    return result;
}

bool Core::trainAndTestOnRandomSubset(unsigned int randomTestSubsetPercentage){

    bool result = false;
    GRT::Trainer trainer;
    GRT::ClassificationData tempClassificationTrainingData;
    GRT::ClassificationData tempClassificationTestData;
    GRT::RegressionData tempRegressionTrainingData;
    GRT::RegressionData tempRegressionTestData;

    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            tempClassificationTrainingData = classificationTrainingData;
            tempClassificationTestData = tempClassificationTrainingData.partition( 100-randomTestSubsetPercentage );
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

    if( getNumTestSamples() > 0 ){
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

bool Core::trainWithCrossValidation(unsigned int numFolds){

    bool result = false;
    GRT::Trainer trainer;

    switch( pipelineMode ){
        case CLASSIFICATION_MODE:
            trainer.setupCVTraining( pipeline, classificationTrainingData, numFolds );
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

bool Core::enablePrediction(bool state){
    boost::mutex::scoped_lock lock( mutex );
    predictionModeEnabled = state;
    return true;
}

void Core::sendPreProcessedData( const GRT::VectorDouble &preProcessedData ){

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

void Core::sendFeatureExtractionData( const GRT::VectorDouble &featureData ){

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

void Core::sendPredictionResults( unsigned int predictedClassLabel, double maximumLikelihood ){

    const unsigned int msgBufferSize = 1024;
    char msgBuffer[ msgBufferSize ];
    osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    msg << osc::BeginMessage( "/Prediction" ) << int( predictedClassLabel ) << float( maximumLikelihood ) << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );

}

void Core::sendClassLikelihoods( const GRT::VectorDouble &classLikelihoods ){

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

void Core::sendClassDistances( const GRT::VectorDouble &classDistances ){

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

void Core::sendRegressionData( const GRT::VectorDouble &regressionData ){

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

void Core::sendStatusMessage(int pipelineMode,int trained,int recording,int numTrainingSamples,int numClassesInTrainingData,std::string infoMessage,std::string version){

    const unsigned int msgBufferSize = 1024;
    char msgBuffer[ msgBufferSize ];
    osc::OutboundPacketStream msg( msgBuffer, msgBufferSize );

    msg << osc::BeginMessage( "/Status" );
    msg << pipelineMode << trained << recording << numTrainingSamples << numClassesInTrainingData << infoMessage.c_str() << version.c_str();
    msg << osc::EndMessage;

    //Send the message
    socket->Send( msg.Data(), msg.Size() );
}
