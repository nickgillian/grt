#include "TrainingThread.h"

TrainingThread::TrainingThread(QObject *parent) : QObject(parent)
{
    threadRunning = false;
    stopMainThread = false;
    verbose = true;
    debug = false;
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
        if( debug ){
            qDebug() << "WARNING: TrainingThread::start() - The thread is already running!" << endl;
        }

        if( verbose ){
            emit newInfoMessage( "WARNING: Failed to start training thread, it is already running!" );
        }
        return false;
    }

    if( debug ){
        qDebug() << STRING_TO_QSTRING("TrainingThread::start() - Starting training thread...");
    }

    try{
        mainThread.reset( new std::thread( std::bind( &TrainingThread::mainThreadFunction, this) ) );
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
        if( debug ){
            qDebug() << "WARNING: TrainingThread::stop() - The thread is not running!" << endl;
        }

        if( verbose ){
            emit newInfoMessage( "WARNING: Failed to stop thread, it is not running!" );
        }
        return false;
    }

    if( debug ){
        qDebug() << STRING_TO_QSTRING("TrainingThread::stop() - Stopping training thread...");
    }

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

bool TrainingThread::getThreadRunning(){
    std::unique_lock< std::mutex > lock( mutex );
    return threadRunning;
}

bool TrainingThread::getTrainingInProcess(){
    std::unique_lock< std::mutex > lock( mutex );
    return trainingInProcess;
}

bool TrainingThread::startNewTraining(const GRT::Trainer &trainer ){

    //Check to make sure another training task is not in process
    if( getTrainingInProcess() ) return false;

    //Flag that a new training task should be started using the new trainer
    std::unique_lock< std::mutex > lock( mutex );
    this->startTraining = true;
    this->trainer = trainer;

    return true;
}

void TrainingThread::mainThreadFunction(){

    //Flag that the core is running
    {
        std::unique_lock< std::mutex > lock( mutex );
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
            std::unique_lock< std::mutex > lock( mutex );
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
                std::unique_lock< std::mutex > lock( mutex );
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
                std::unique_lock< std::mutex > lock( mutex );
                tempPipeline = trainer.getPipeline();
            }
            emit pipelineUpdated( tempPipeline );
            emit pipelineTrainingFinished( result );
        }

        //Let the thread sleep so we don't kill the CPU
        std::this_thread::sleep_for( std::chrono::milliseconds( DEFAULT_TRAINING_THREAD_SLEEP_TIME ) );

        //Check to see if we should stop the thread
        {
            std::unique_lock< std::mutex > lock( mutex );
            if( stopMainThread ){
                keepRunning = false;
            }
        }
    }

    //Flag that the core has stopped
    {
        std::unique_lock< std::mutex > lock( mutex );
        threadRunning = false;
        stopMainThread = false;
        trainingInProcess = false;
        startTraining = false;
    }

    //Signal that the core has stopped
    emit threadStopped();
}

