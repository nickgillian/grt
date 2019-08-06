#ifndef GRT_TRAINING_THREAD_H
#define GRT_TRAINING_THREAD_H

//#include <GRT/GRT.h>
//Include the OSC server to get the boost headers and QDebug headers
#include "OSC/OSCServer.h"
#include "Trainer.h"

#include <QObject>
#include <mutex>
#include <condition_variable>

class TrainingThread : public QObject
{
    Q_OBJECT
public:
    explicit TrainingThread(QObject *parent = 0);

    ~TrainingThread();

    bool start();
    bool stop();
    bool getThreadRunning() const { return threadRunning; }
    bool getTrainingInProcess() const { return trainingInProcess; }
    bool startNewTraining( const GRT::Trainer &trainingContainer );

signals:
    void threadStarted();
    void threadStopped();
    void newInfoMessage(std::string msg);
    void newWarningMessage(std::string msg);
    void newErrorMessage(std::string msg);
    void newHelpMessage(std::string msg);

    void pipelineTrainingStarted();
    void pipelineTrainingFinished(bool result);
    void pipelineTestingFinished(bool result);
    void pipelineUpdated(const GRT::GestureRecognitionPipeline &pipeline);

protected:
    void mainThreadFunction();
    bool train();

    std::mutex mutex;
    std::shared_ptr< std::thread > mainThread;
    bool threadRunning = false;
    bool stopMainThread = false;
    bool verbose = true;
    bool debug = false;
    bool trainingInProcess = false;
    std::condition_variable startTraining;

    GRT::Trainer trainer;
    
};

#endif // GRT_TRAINING_THREAD_H
