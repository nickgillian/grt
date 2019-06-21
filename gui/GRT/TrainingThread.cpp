#include "TrainingThread.h"

TrainingThread::TrainingThread(QObject *parent) : QObject(parent)
{
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
    std::unique_lock< std::mutex > lock( mutex );
    stopMainThread = true;
    startTraining.notify_all();

    //Wait for it to stop
    mainThread->join();
    mainThread.reset();

    return true;
}

bool TrainingThread::getThreadRunning(){
    return threadRunning;
}

bool TrainingThread::getTrainingInProcess(){
    return trainingInProcess;
}

bool TrainingThread::startNewTraining(const GRT::Trainer &my_trainer ){

    //Check to make sure another training task is not in process
    if( getTrainingInProcess() ) return false;

    //Flag that a new training task should be started using the new trainer
    std::unique_lock< std::mutex > lock( mutex );
    trainer = my_trainer;
    startTraining.notify_all();

    return true;
}

void TrainingThread::mainThreadFunction(){

    //Flag that the core is running
    {
        std::unique_lock< std::mutex > lock( mutex );
        threadRunning = true;
        stopMainThread = false;
        trainingInProcess = false;
    }

    //Flag that the core is now running
    emit threadStarted();

    while( ! stopMainThread ){

        //Wait until main loop starts this
        std::unique_lock<std::mutex> lock(mutex);
        startTraining.wait(lock);

        //Check to see if we should exit
        if ( stopMainThread ) break;

        trainingInProcess = true;

        emit pipelineTrainingStarted();

        //Start the training, the thread will pause here until the training has finished
        bool result = trainer.train();

        //Flag that the training has stopped
        trainingInProcess = false;

        //Emit the results
        if( result ){
            emit newInfoMessage( "Pipeline trained" );
        }else{
            emit newHelpMessage( trainer.getLastErrorMessage() );
        }

        GRT::GestureRecognitionPipeline tempPipeline;
        tempPipeline = trainer.getPipeline();

        emit pipelineUpdated( tempPipeline );
        emit pipelineTrainingFinished( result );
    }

    //Flag that the core has stopped
    threadRunning = false;

    //Signal that the core has stopped
    emit threadStopped();
}

