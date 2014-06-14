#include "TrainingThread.h"

TrainingThread::TrainingThread(QObject *parent) : QObject(parent)
{
    threadRunning = false;
    stopMainThread = false;
    verbose = true;
    trainingInProcess = false;
    startTraining = false;
}

TrainingThread::~TrainingThread(){
    if( getThreadRunning() ){
        stop();
    }
}

bool TrainingThread::start(){

    if( getThreadRunning() ){
        if( verbose ){
            qDebug() << "WARNING: TrainingThread::start() - The thread is already running!" << endl;
            emit newInfoMessage( "WARNING: Failed to start training thread, it is already running!" );
        }
        return false;
    }

    if( verbose )
        qDebug() << STRING_TO_QSTRING("TrainingThread::start() - Starting training thread...");

    try{
        mainThread.reset( new boost::thread( boost::bind( &TrainingThread::mainThreadFunction, this) ) );
    }catch( std::exception const &error ){
        QString qstr = "ERROR: Core::start() - Failed to start training thread! Exception: ";
        qstr += error.what();
        qstr += "\n";
        qDebug() << qstr;
        return false;
    }

    return true;
}

bool TrainingThread::stop(){

    if( !getThreadRunning() ){
        if( verbose ){
            qDebug() << "WARNING: TrainingThread::stop() - The thread is not running!" << endl;
            emit newInfoMessage( "WARNING: Failed to stop thread, it is not running!" );
        }
        return false;
    }

    if( verbose )
        qDebug() << STRING_TO_QSTRING("TrainingThread::stop() - Stopping training thread...");

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

bool TrainingThread::getThreadRunning(){
    boost::mutex::scoped_lock lock( mutex );
    return threadRunning;
}

bool TrainingThread::getTrainingInProcess(){
    boost::mutex::scoped_lock lock( mutex );
    return trainingInProcess;
}

bool TrainingThread::startNewTraining(const GRT::Trainer &trainer ){

    //Check to make sure another training task is not in process
    if( getTrainingInProcess() ) return false;

    //Flag that a new training task should be started using the new trainer
    boost::mutex::scoped_lock lock( mutex );
    this->startTraining = true;
    this->trainer = trainer;

    return true;
}

void TrainingThread::mainThreadFunction(){

    //Flag that the core is running
    {
        boost::mutex::scoped_lock lock( mutex );
        threadRunning = true;
        stopMainThread = false;
        trainingInProcess = false;
        startTraining = false;
    }

    //Flag that the core is now running
    emit threadStarted();

    bool keepRunning = true;

    while( keepRunning ){

        //Check to see if we should start a new training batch
        bool runTraining = false;

        {
            boost::mutex::scoped_lock lock( mutex );
            runTraining = startTraining;
            if( startTraining ){
                startTraining = false;
                trainingInProcess = true;
            }
        }

        if( runTraining ){
            emit pipelineTrainingStarted();

            //Start the training, the thread will pause here until the training has finished
            bool result = trainer.train();

            //Flag that the training has stopped
            {
                boost::mutex::scoped_lock lock( mutex );
                trainingInProcess = false;
            }

            //Emit the results
            if( result ){
                emit newInfoMessage( "Pipeline trained" );
            }else{
                emit newHelpMessage( trainer.getLastErrorMessage() );
            }

            GRT::GestureRecognitionPipeline tempPipeline;
            {
                boost::mutex::scoped_lock lock( mutex );
                tempPipeline = trainer.getPipeline();
            }
            emit pipelineUpdated( tempPipeline );
            emit pipelineTrainingFinished( result );
        }

        //Let the thread sleep so we don't kill the CPU
        boost::this_thread::sleep( boost::posix_time::milliseconds( DEFAULT_TRAINING_THREAD_SLEEP_TIME ) );

        //Check to see if we should stop the thread
        {
            boost::mutex::scoped_lock lock( mutex );
            if( stopMainThread ){
                keepRunning = false;
            }
        }
    }

    //Flag that the core has stopped
    {
        boost::mutex::scoped_lock lock( mutex );
        threadRunning = false;
        stopMainThread = false;
        trainingInProcess = false;
        startTraining = false;
    }

    //Signal that the core has stopped
    emit threadStopped();
}

