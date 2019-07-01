#-------------------------------------------------
#
# Project created by QtCreator 2013-09-11T10:29:42
#
#-------------------------------------------------

#NOTE: Have a look at the GRT GUI README before attempting to build this project

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

#OSX flags
macx {
 #-std=gnu0x
 QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -stdlib=libc++ -g -Wall
 QMAKE_CXXFLAGS += -DOSC_HOST_LITTLE_ENDIAN
}

#Linux flags
unix:!macx {
 QMAKE_CXXFLAGS += -std=c++11
 QMAKE_CXXFLAGS += -DOSC_HOST_LITTLE_ENDIAN
 QMAKE_CXXFLAGS += -DGRT_CXX11_ENABLED=true
 QMAKE_CXXFLAGS += -Wall
}

#Windows flags
win32 {
 QMAKE_CXXFLAGS += -DOSC_HOST_LITTLE_ENDIAN
}

#Add c++ 11 support
CONFIG += static
CONFIG += c++11

#Set the build target
TARGET = GRT
macx: TEMPLATE = app

#Flag that we want to include the GRT library directly as source code
#comment to use a pre-built version of the GRT library (this should be installed in /user/local/lib
USE_GRT_SOURCE_CODE = "true"


#Windows Include
win32{
 #Link against the windows libraries needed for OSC networking
 LIBS += -lws2_32
 LIBS += -lwinmm
}

#If USE_GRT_SOURCE_CODE is defined, then we add the GRT source code to the project
defined(USE_GRT_SOURCE_CODE,var){
 message( "Build grt lib from source." )

 #Add the custom GRT paths
 INCLUDEPATH += ../..

 #Include the main GRT header
 HEADERS += ../../GRT/GRT.h \
           ../../GRT/ClassificationModules/AdaBoost/AdaBoostClassModel.h \
           ../../GRT/ClassificationModules/AdaBoost/AdaBoost.h \
           ../../GRT/ClassificationModules/AdaBoost/WeakClassifiers/DecisionStump.h \
           ../../GRT/ClassificationModules/AdaBoost/WeakClassifiers/RadialBasisFunction.h \
           ../../GRT/ClassificationModules/AdaBoost/WeakClassifiers/WeakClassifier.h \
           ../../GRT/ClassificationModules/ANBC/ANBC.h \
           ../../GRT/ClassificationModules/ANBC/ANBC_Model.h \
           ../../GRT/ClassificationModules/BAG/BAG.h \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTreeClusterNode.h \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTree.h \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTreeNode.h \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTreeThresholdNode.h \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTreeTripleFeatureNode.h \
           ../../GRT/ClassificationModules/DTW/DTW.h \
           ../../GRT/ClassificationModules/FiniteStateMachine/FiniteStateMachine.h \
           ../../GRT/ClassificationModules/FiniteStateMachine/FSMParticleFilter.h \
           ../../GRT/ClassificationModules/FiniteStateMachine/FSMParticle.h \
           ../../GRT/ClassificationModules/GMM/GMM.h \
           ../../GRT/ClassificationModules/GMM/MixtureModel.h \
           ../../GRT/ClassificationModules/HMM/ContinuousHiddenMarkovModel.h \
           ../../GRT/ClassificationModules/HMM/DiscreteHiddenMarkovModel.h \
           ../../GRT/ClassificationModules/HMM/HMMEnums.h \
           ../../GRT/ClassificationModules/HMM/HMM.h \
           ../../GRT/ClassificationModules/KNN/KNN.h \
           ../../GRT/ClassificationModules/MinDist/MinDist.h \
           ../../GRT/ClassificationModules/MinDist/MinDistModel.h \
           ../../GRT/ClassificationModules/ParticleClassifier/ParticleClassifier.h \
           ../../GRT/ClassificationModules/ParticleClassifier/ParticleClassifierParticleFilter.h \
           ../../GRT/ClassificationModules/RandomForests/RandomForests.h \
           ../../GRT/ClassificationModules/Softmax/Softmax.h \
           ../../GRT/ClassificationModules/Softmax/SoftmaxModel.h \
           ../../GRT/ClassificationModules/SVM/LIBSVM/libsvm.h \
           ../../GRT/ClassificationModules/SVM/SVM.h \
           ../../GRT/ClassificationModules/SwipeDetector/SwipeDetector.h \
           ../../GRT/ClusteringModules/ClusterTree/ClusterTree.h \
           ../../GRT/ClusteringModules/ClusterTree/ClusterTreeNode.h \
           ../../GRT/ClusteringModules/GaussianMixtureModels/GaussianMixtureModels.h \
           ../../GRT/ClusteringModules/HierarchicalClustering/HierarchicalClustering.h \
           ../../GRT/ClusteringModules/KMeans/KMeans.h \
           ../../GRT/ClusteringModules/SelfOrganizingMap/SelfOrganizingMap.h \
           ../../GRT/ContextModules/Gate.h \
           ../../GRT/CoreAlgorithms/BernoulliRBM/BernoulliRBM.h \
           ../../GRT/CoreAlgorithms/EvolutionaryAlgorithm/EvolutionaryAlgorithm.h \
           ../../GRT/CoreAlgorithms/EvolutionaryAlgorithm/Individual.h \
           ../../GRT/CoreAlgorithms/GridSearch/GridSearch.h \
           ../../GRT/CoreAlgorithms/LeastSquares/LinearLeastSquares.h \
           ../../GRT/CoreAlgorithms/MeanShift/MeanShift.h \
           ../../GRT/CoreAlgorithms/MovementDetector/MovementDetector.h \
           ../../GRT/CoreAlgorithms/ParticleFilter/ParticleFilter.h \
           ../../GRT/CoreAlgorithms/ParticleFilter/Particle.h \
           ../../GRT/CoreAlgorithms/ParticleSwarmOptimization/ParticleSwarmOptimization.h \
           ../../GRT/CoreAlgorithms/ParticleSwarmOptimization/PSOParticle.h \
           ../../GRT/CoreAlgorithms/PrincipalComponentAnalysis/PrincipalComponentAnalysis.h \
           ../../GRT/CoreAlgorithms/Tree/Node.h \
           ../../GRT/CoreAlgorithms/Tree/Tree.h \
           ../../GRT/CoreModules/Classifier.h \
           ../../GRT/CoreModules/Clusterer.h \
           ../../GRT/CoreModules/Context.h \
           ../../GRT/CoreModules/FeatureExtraction.h \
           ../../GRT/CoreModules/GestureRecognitionPipeline.h \
           ../../GRT/CoreModules/GRTBase.h \
           ../../GRT/CoreModules/MLBase.h \
           ../../GRT/CoreModules/PostProcessing.h \
           ../../GRT/CoreModules/PreProcessing.h \
           ../../GRT/CoreModules/Regressifier.h \
           ../../GRT/DataStructures/ClassificationData.h \
           ../../GRT/DataStructures/ClassificationDataStream.h \
           ../../GRT/DataStructures/ClassificationSample.h \
           ../../GRT/DataStructures/MatrixFloat.h \
           ../../GRT/DataStructures/Matrix.h \
           ../../GRT/DataStructures/RegressionData.h \
           ../../GRT/DataStructures/RegressionSample.h \
           ../../GRT/DataStructures/TimeSeriesClassificationData.h \
           ../../GRT/DataStructures/TimeSeriesClassificationSample.h \
           ../../GRT/DataStructures/TimeSeriesPositionTracker.h \
           ../../GRT/DataStructures/UnlabelledData.h \
           ../../GRT/DataStructures/VectorFloat.h \
           ../../GRT/DataStructures/Vector.h \
           ../../GRT/FeatureExtractionModules/EnvelopeExtractor/EnvelopeExtractor.h \
           ../../GRT/FeatureExtractionModules/FFT/FastFourierTransform.h \
           ../../GRT/FeatureExtractionModules/FFT/FFTFeatures.h \
           ../../GRT/FeatureExtractionModules/FFT/FFT.h \
           ../../GRT/FeatureExtractionModules/KMeansFeatures/KMeansFeatures.h \
           ../../GRT/FeatureExtractionModules/KMeansQuantizer/KMeansQuantizer.h \
           ../../GRT/FeatureExtractionModules/MovementIndex/MovementIndex.h \
           ../../GRT/FeatureExtractionModules/MovementTrajectoryFeatures/MovementTrajectoryFeatures.h \
           ../../GRT/FeatureExtractionModules/PCA/PCA.h \
           ../../GRT/FeatureExtractionModules/RBMQuantizer/RBMQuantizer.h \
           ../../GRT/FeatureExtractionModules/SOMQuantizer/SOMQuantizer.h \
           ../../GRT/FeatureExtractionModules/TimeDomainFeatures/TimeDomainFeatures.h \
           ../../GRT/FeatureExtractionModules/TimeseriesBuffer/TimeseriesBuffer.h \
           ../../GRT/FeatureExtractionModules/ZeroCrossingCounter/ZeroCrossingCounter.h \
           ../../GRT/PostProcessingModules/ClassLabelChangeFilter.h \
           ../../GRT/PostProcessingModules/ClassLabelFilter.h \
           ../../GRT/PostProcessingModules/ClassLabelTimeoutFilter.h \
           ../../GRT/PreProcessingModules/DeadZone.h \
           ../../GRT/PreProcessingModules/Derivative.h \
           ../../GRT/PreProcessingModules/DoubleMovingAverageFilter.h \
           ../../GRT/PreProcessingModules/FIRFilter.h \
           ../../GRT/PreProcessingModules/HighPassFilter.h \
           ../../GRT/PreProcessingModules/LeakyIntegrator.h \
           ../../GRT/PreProcessingModules/LowPassFilter.h \
           ../../GRT/PreProcessingModules/MedianFilter.h \
           ../../GRT/PreProcessingModules/MovingAverageFilter.h \
           ../../GRT/PreProcessingModules/RMSFilter.h \
           ../../GRT/PreProcessingModules/SavitzkyGolayFilter.h \
           ../../GRT/PreProcessingModules/WeightedAverageFilter.h \
           ../../GRT/RegressionModules/ArtificialNeuralNetworks/MLP/MLP.h \
           ../../GRT/RegressionModules/ArtificialNeuralNetworks/MLP/Neuron.h \
           ../../GRT/RegressionModules/LinearRegression/LinearRegression.h \
           ../../GRT/RegressionModules/LogisticRegression/LogisticRegression.h \
           ../../GRT/RegressionModules/MultidimensionalRegression/MultidimensionalRegression.h \
           ../../GRT/RegressionModules/RegressionTree/RegressionTree.h \
           ../../GRT/RegressionModules/RegressionTree/RegressionTreeNode.h \
           ../../GRT/Util/Cholesky.h \
           ../../GRT/Util/CircularBuffer.h \
           ../../GRT/Util/ClassificationResult.h \
           ../../GRT/Util/ClassTracker.h \
           ../../GRT/Util/CommandLineParser.h \
           ../../GRT/Util/DataType.h \
           ../../GRT/Util/DebugLog.h \
           ../../GRT/Util/Dict.h \
           ../../GRT/Util/DynamicType.h \
           ../../GRT/Util/EigenvalueDecomposition.h \
           ../../GRT/Util/ErrorLog.h \
           ../../GRT/Util/FileParser.h \
           ../../GRT/Util/GRTCommon.h \
           ../../GRT/Util/GRTException.h \
           ../../GRT/Util/GRTTypedefs.h \
           ../../GRT/Util/GRTVersionInfo.h \
           ../../GRT/Util/IndexedDouble.h \
           ../../GRT/Util/InfoLog.h \
           ../../GRT/Util/Log.h \
           ../../GRT/Util/LUDecomposition.h \
           ../../GRT/Util/Metrics.h \
           ../../GRT/Util/MinMax.h \
           ../../GRT/Util/Observer.h \
           ../../GRT/Util/ObserverManager.h \
           ../../GRT/Util/PeakDetection.h \
           ../../GRT/Util/Random.h \
           ../../GRT/Util/RangeTracker.h \
           ../../GRT/Util/SVD.h \
           ../../GRT/Util/TestingLog.h \
           ../../GRT/Util/TestInstanceResult.h \
           ../../GRT/Util/TestResult.h \
           ../../GRT/Util/ThreadPool.h \
           ../../GRT/Util/ThresholdCrossingDetector.h \
           ../../GRT/Util/Timer.h \
           ../../GRT/Util/TimeSeriesClassificationSampleTrimmer.h \
           ../../GRT/Util/TimeStamp.h \
           ../../GRT/Util/TrainingDataRecordingTimer.h \
           ../../GRT/Util/TrainingLog.h \
           ../../GRT/Util/TrainingResult.h \
           ../../GRT/Util/Util.h \
           ../../GRT/Util/WarningLog.h


 #Add the GRT Classification modules to the source
 SOURCES += ../../GRT/ClassificationModules/AdaBoost/AdaBoost.cpp \
           ../../GRT/ClassificationModules/AdaBoost/WeakClassifiers/DecisionStump.cpp \
           ../../GRT/ClassificationModules/AdaBoost/WeakClassifiers/RadialBasisFunction.cpp \
           ../../GRT/ClassificationModules/AdaBoost/WeakClassifiers/WeakClassifier.cpp \
           ../../GRT/ClassificationModules/ANBC/ANBC.cpp \
           ../../GRT/ClassificationModules/ANBC/ANBC_Model.cpp \
           ../../GRT/ClassificationModules/BAG/BAG.cpp \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTree.cpp \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTreeClusterNode.cpp \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTreeNode.cpp \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTreeThresholdNode.cpp \
           ../../GRT/ClassificationModules/DecisionTree/DecisionTreeTripleFeatureNode.cpp \
           ../../GRT/ClassificationModules/DTW/DTW.cpp \
           ../../GRT/ClassificationModules/FiniteStateMachine/FiniteStateMachine.cpp \
           ../../GRT/ClassificationModules/GMM/GMM.cpp \
           ../../GRT/ClassificationModules/HMM/ContinuousHiddenMarkovModel.cpp \
           ../../GRT/ClassificationModules/HMM/DiscreteHiddenMarkovModel.cpp \
           ../../GRT/ClassificationModules/HMM/HMM.cpp \
           ../../GRT/ClassificationModules/KNN/KNN.cpp \
           ../../GRT/ClassificationModules/MinDist/MinDist.cpp \
           ../../GRT/ClassificationModules/MinDist/MinDistModel.cpp \
           ../../GRT/ClassificationModules/ParticleClassifier/ParticleClassifier.cpp \
           ../../GRT/ClassificationModules/RandomForests/RandomForests.cpp \
           ../../GRT/ClassificationModules/Softmax/Softmax.cpp \
           ../../GRT/ClassificationModules/SVM/SVM.cpp \
           ../../GRT/ClassificationModules/SVM/LIBSVM/libsvm.cpp \
           ../../GRT/ClassificationModules/SwipeDetector/SwipeDetector.cpp

 #Add the GRT Clustering modules to the source
 SOURCES += ../../GRT/ClusteringModules/ClusterTree/ClusterTree.cpp \
            ../../GRT/ClusteringModules/GaussianMixtureModels/GaussianMixtureModels.cpp \
           ../../GRT/ClusteringModules/HierarchicalClustering/HierarchicalClustering.cpp \
           ../../GRT/ClusteringModules/KMeans/KMeans.cpp \
           ../../GRT/ClusteringModules/SelfOrganizingMap/SelfOrganizingMap.cpp

 #Add the GRT Context modules to the source
 SOURCES += ../../GRT/ContextModules/Gate.cpp

 #Add the Core Alogrithm source
 SOURCES += ../../GRT/CoreAlgorithms/BernoulliRBM/BernoulliRBM.cpp \
           ../../GRT/CoreAlgorithms/PrincipalComponentAnalysis/PrincipalComponentAnalysis.cpp \
           ../../GRT/CoreAlgorithms/Tree/Node.cpp \
           ../../GRT/CoreAlgorithms/Tree/Tree.cpp

 #Add the GRT Pipeline and Core Modules source
 SOURCES += ../../GRT/CoreModules/Classifier.cpp \
           ../../GRT/CoreModules/Clusterer.cpp \
           ../../GRT/CoreModules/Context.cpp \
           ../../GRT/CoreModules/FeatureExtraction.cpp \
           ../../GRT/CoreModules/GestureRecognitionPipeline.cpp \
           ../../GRT/CoreModules/GRTBase.cpp \
           ../../GRT/CoreModules/MLBase.cpp \
           ../../GRT/CoreModules/PostProcessing.cpp \
           ../../GRT/CoreModules/PreProcessing.cpp \
           ../../GRT/CoreModules/Regressifier.cpp

 #Add the GRT Data Structures to the source
 SOURCES += ../../GRT/DataStructures/ClassificationData.cpp \
           ../../GRT/DataStructures/ClassificationDataStream.cpp \
           ../../GRT/DataStructures/ClassificationSample.cpp \
           ../../GRT/DataStructures/MatrixFloat.cpp \
           ../../GRT/DataStructures/RegressionData.cpp \
           ../../GRT/DataStructures/RegressionSample.cpp \
           ../../GRT/DataStructures/TimeSeriesClassificationData.cpp \
           ../../GRT/DataStructures/TimeSeriesClassificationSample.cpp \
           ../../GRT/DataStructures/UnlabelledData.cpp \
           ../../GRT/DataStructures/VectorFloat.cpp

 #Add the GRT Feature Extraction modules to the source
 SOURCES += ../../GRT/FeatureExtractionModules/FFT/FastFourierTransform.cpp \
           ../../GRT/FeatureExtractionModules/FFT/FFT.cpp \
           ../../GRT/FeatureExtractionModules/FFT/FFTFeatures.cpp \
           ../../GRT/FeatureExtractionModules/KMeansFeatures/KMeansFeatures.cpp \
           ../../GRT/FeatureExtractionModules/KMeansQuantizer/KMeansQuantizer.cpp \
           ../../GRT/FeatureExtractionModules/MovementIndex/MovementIndex.cpp \
           ../../GRT/FeatureExtractionModules/MovementTrajectoryFeatures/MovementTrajectoryFeatures.cpp \
           ../../GRT/FeatureExtractionModules/RBMQuantizer/RBMQuantizer.cpp \
           ../../GRT/FeatureExtractionModules/SOMQuantizer/SOMQuantizer.cpp \
           ../../GRT/FeatureExtractionModules/TimeDomainFeatures/TimeDomainFeatures.cpp \
           ../../GRT/FeatureExtractionModules/TimeseriesBuffer/TimeseriesBuffer.cpp \
           ../../GRT/FeatureExtractionModules/ZeroCrossingCounter/ZeroCrossingCounter.cpp

 #Add the GRT PostProcessing modules to the source
 SOURCES += ../../GRT/PostProcessingModules/ClassLabelChangeFilter.cpp \
           ../../GRT/PostProcessingModules/ClassLabelFilter.cpp \
           ../../GRT/PostProcessingModules/ClassLabelTimeoutFilter.cpp

 #Add the GRT PreProcessing modules to the source
 SOURCES += ../../GRT/PreProcessingModules/DeadZone.cpp \
           ../../GRT/PreProcessingModules/Derivative.cpp \
           ../../GRT/PreProcessingModules/DoubleMovingAverageFilter.cpp \
           ../../GRT/PreProcessingModules/FIRFilter.cpp \
           ../../GRT/PreProcessingModules/HighPassFilter.cpp \
           ../../GRT/PreProcessingModules/LeakyIntegrator.cpp \
           ../../GRT/PreProcessingModules/LowPassFilter.cpp \
           ../../GRT/PreProcessingModules/MedianFilter.cpp \
           ../../GRT/PreProcessingModules/MovingAverageFilter.cpp \
           ../../GRT/PreProcessingModules/RMSFilter.cpp \
           ../../GRT/PreProcessingModules/SavitzkyGolayFilter.cpp \
           ../../GRT/PreProcessingModules/WeightedAverageFilter.cpp

 #Add the GRT Regression modules to the source
 SOURCES += ../../GRT/RegressionModules/ArtificialNeuralNetworks/MLP/MLP.cpp \
           ../../GRT/RegressionModules/ArtificialNeuralNetworks/MLP/Neuron.cpp \
           ../../GRT/RegressionModules/LinearRegression/LinearRegression.cpp \
           ../../GRT/RegressionModules/LogisticRegression/LogisticRegression.cpp \
           ../../GRT/RegressionModules/MultidimensionalRegression/MultidimensionalRegression.cpp

 #Add the GRT Util modules to the source
 SOURCES += ../../GRT/Util/Cholesky.cpp \
           ../../GRT/Util/DebugLog.cpp \
           ../../GRT/Util/EigenvalueDecomposition.cpp \
           ../../GRT/Util/ErrorLog.cpp \
           ../../GRT/Util/InfoLog.cpp \
           ../../GRT/Util/LUDecomposition.cpp \
           ../../GRT/Util/Metrics.cpp \
           ../../GRT/Util/PeakDetection.cpp \
           ../../GRT/Util/Random.cpp \
           ../../GRT/Util/RangeTracker.cpp \
           ../../GRT/Util/SVD.cpp \
           ../../GRT/Util/TestingLog.cpp \
           ../../GRT/Util/ThreadPool.cpp \
           ../../GRT/Util/ThresholdCrossingDetector.cpp \
           ../../GRT/Util/TimeSeriesClassificationSampleTrimmer.cpp \
           ../../GRT/Util/TrainingDataRecordingTimer.cpp \
           ../../GRT/Util/TrainingLog.cpp \
           ../../GRT/Util/Util.cpp \
           ../../GRT/Util/WarningLog.cpp

} #end of USE_GRT_SOURCE_CODE
else {
 message( "Use prebuilt grt lib." )

 #Include the custom and library and header paths
 #Note that these paths might be different for your specific machine

 # All uNIX
 unix{
  #Add the default include and lib directories (we assume boost and GRT are installed here)
  INCLUDEPATH += /usr/local/include
  LIBS += -L/usr/local/lib
 }

 #OSX Include/Link
 macx{
  #Add the custom resources file
  ICON = Resources/OSX/GRT.icns
 }

 #Linux pkgconfig
 unix:!macx:!android{
  #Add the custom grt pkgconfig
  CONFIG += link_pkgconfig
  PKGCONFIG += grt
 }


 #Add the main GRT library, you should have already compiled and install this (using the main GRT cmake file)
 LIBS += -lgrt
}

#Add the base oscpack directory
INCLUDEPATH += OSC/oscpack/include

#Add the custom networking code, based on the OS
unix{
 SOURCES += OSC/oscpack/include/ip/posix/UdpSocket.cpp
 SOURCES += OSC/oscpack/include/ip/posix/NetworkingUtils.cpp
}
win32{
 SOURCES += OSC/oscpack/include/ip/win32/UdpSocket.cpp
 SOURCES += OSC/oscpack/include/ip/win32/NetworkingUtils.cpp
}

#Add the common project source code
SOURCES += main.cpp\
        mainwindow.cpp \
        OSC/oscpack/include/osc/OscTypes.cpp \
        OSC/oscpack/include/osc/OscReceivedElements.cpp \
        OSC/oscpack/include/osc/OscPrintReceivedElements.cpp \
        OSC/oscpack/include/osc/OscOutboundPacketStream.cpp \
        OSC/oscpack/include/ip/IpEndpointName.cpp \
        OSC/OSCServer.cpp \
        Core.cpp \
        qcustomplot.cpp \
        TrainingThread.cpp \
        timeseriesgraph.cpp \
        bargraph.cpp \
        versioninfo.cpp \
        featureplot.cpp

HEADERS  += mainwindow.h \
        OSC/OSCMessage.h \
        OSC/oscpack/include/osc/OscTypes.h \
        OSC/oscpack/include/osc/OscReceivedElements.h \
        OSC/oscpack/include/osc/OscPrintReceivedElements.h \
        OSC/oscpack/include/osc/OscPacketListener.h \
        OSC/oscpack/include/osc/OscOutboundPacketStream.h \
        OSC/oscpack/include/osc/OscHostEndianness.h \
        OSC/oscpack/include/osc/OscException.h \
        OSC/oscpack/include/osc/MessageMappingOscPacketListener.h \
        OSC/oscpack/include/ip/UdpSocket.h \
        OSC/oscpack/include/ip/TimerListener.h \
        OSC/oscpack/include/ip/PacketListener.h \
        OSC/oscpack/include/ip/NetworkingUtils.h \
        OSC/oscpack/include/ip/IpEndpointName.h \
        OSC/OSCServer.h \
        Core.h \
        qcustomplot.h \
        TrainingThread.h \
        Trainer.h \
        timeseriesgraph.h \
        bargraph.h \
        versioninfo.h \
        featureplot.h

FORMS += mainwindow.ui \
    timeseriesgraph.ui \
    bargraph.ui \
    versioninfo.ui \
    featureplot.ui

OTHER_FILES += Resources/Images/Button1.png \
               Resources/Images/ClassificationModeImage.png \
               Resources/Images/RegressionModeImage.png \
               Resources/Images/TimeseriesModeImage.png

RESOURCES += Resources/resource.qrc
