#-------------------------------------------------
#
# Project created by QtCreator 2013-09-11T10:29:42
#
#-------------------------------------------------
QT  += core gui
QT  += declarative

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

macx {
 QMAKE_CXXFLAGS = -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc++
} else:unix {
 QMAKE_CXXFLAGS = -std=c++0x
}

CONFIG +=c++11

CONFIG += static
TARGET = GRT
macx: TEMPLATE = app

#Include the boost libraries, the headers and static library files might be in a different location on your machine
#OSX Include
macx{
INCLUDEPATH += /usr/local/include
LIBS += -L../GRT/Resources/OSX/Libs
ICON = Resources/OSX/GRT.icns
}

#Linux Include
unix:!macx{
INCLUDEPATH += /usr/local/include/boost
 LIBS += -L../GRT/Resources/Linux/Libs
}

#Windows Include
win32{
INCLUDEPATH += "C:\SDK\EyesWeb XMI SDK\5.3.0\Include\boost"
LIBS += "C:\SDK\EyesWeb XMI SDK\5.3.0\lib"
}

#Regardless of the OS we need to include the boost libraries
LIBS += -lboost_thread
LIBS += -lboost_date_time
LIBS += -lboost_system
LIBS += -lboost_chrono

SOURCES += main.cpp\
        mainwindow.cpp \
        OSC/oscpack/include/osc/OscTypes.cpp \
        OSC/oscpack/include/osc/OscReceivedElements.cpp \
        OSC/oscpack/include/osc/OscPrintReceivedElements.cpp \
        OSC/oscpack/include/osc/OscOutboundPacketStream.cpp \
        OSC/oscpack/include/ip/IpEndpointName.cpp \
        OSC/oscpack/include/ip/posix/UdpSocket.cpp \
        OSC/oscpack/include/ip/posix/NetworkingUtils.cpp \
        OSC/OSCServer.cpp \
        Core.cpp \
        qcustomplot.cpp \
        TrainingThread.cpp \
        timeseriesgraph.cpp \
        bargraph.cpp \
        versioninfo.cpp

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
        versioninfo.h

#Include the main GRT header
HEADERS += ../../GRT/GRT.h

#Add the GRT Classification module headers
HEADERS += ../../GRT/ClassificationModules/AdaBoost/*.h \
           ../../GRT/ClassificationModules/AdaBoost/WeakClassifiers/*.h \
           ../../GRT/ClassificationModules/ANBC/*.h \
           ../../GRT/ClassificationModules/BAG/*.h \
           ../../GRT/ClassificationModules/DecisionTree/*.h \
           ../../GRT/ClassificationModules/DTW/*.h \
           ../../GRT/ClassificationModules/GMM/*.h \
           ../../GRT/ClassificationModules/HMM/*.h \
           ../../GRT/ClassificationModules/KNN/*.h \
           ../../GRT/ClassificationModules/LDA/*.h \
           ../../GRT/ClassificationModules/MinDist/*.h \
           ../../GRT/ClassificationModules/RandomForests/*.h \
           ../../GRT/ClassificationModules/Softmax/*.h \
           ../../GRT/ClassificationModules/SVM/*.h \
           ../../GRT/ClassificationModules/SVM/LIBSVM/*.h

#Add the GRT Clustering module headers
HEADERS += ../../GRT/ClusteringModules/GaussianMixtureModels/*.h \
           ../../GRT/ClusteringModules/HierarchicalClustering/*.h \
           ../../GRT/ClusteringModules/KMeans/*.h \
           ../../GRT/ClusteringModules/SelfOrganizingMap/*.h

#Add the GRT Context module headers
HEADERS += ../../GRT/ContextModules/*.h

#Add the Core Alogrithm headers
HEADERS += ../../GRT/CoreAlgorithms/BernoulliRBM/*.h \
           ../../GRT/CoreAlgorithms/EvolutionaryAlgorithm/*.h \
           ../../GRT/CoreAlgorithms/ParticleFilter/*.h \
           ../../GRT/CoreAlgorithms/ParticleSwarmOptimization/*.h \
           ../../GRT/CoreAlgorithms/PrincipalComponentAnalysis/*.h

#Add the GRT Pipeline and Core Modules headers
HEADERS += ../../GRT/CoreModules/*.h

#Add the GRT Data Structures headers
HEADERS += ../../GRT/DataStructures/*.h

#Add the GRT Feature Extraction module headers
HEADERS += ../../GRT/FeatureExtractionModules/FFT/*.h \
           ../../GRT/FeatureExtractionModules/KMeansFeatures/*.h \
           ../../GRT/FeatureExtractionModules/KMeansQuantizer/*.h \
           ../../GRT/FeatureExtractionModules/MovementIndex/*.h \
           ../../GRT/FeatureExtractionModules/MovementTrajectoryFeatures/*.h \
           ../../GRT/FeatureExtractionModules/RBMQuantizer/*.h \
           ../../GRT/FeatureExtractionModules/SOMQuantizer/*.h \
           ../../GRT/FeatureExtractionModules/TimeDomainFeatures/*.h \
           ../../GRT/FeatureExtractionModules/TimeseriesBuffer/*.h \
           ../../GRT/FeatureExtractionModules/ZeroCrossingCounter/*.h

#Add the GRT PostProcessing module headers
HEADERS +=  ../../GRT/PostProcessingModules/*.h

#Add the GRT PreProcessing module headers
HEADERS += ../../GRT/PreProcessingModules/*.h

#Add the GRT Regression module headers
HEADERS += ../../GRT/RegressionModules/ArtificialNeuralNetworks/MLP/*.h \
           ../../GRT/RegressionModules/LinearRegression/*.h \
           ../../GRT/RegressionModules/LogisticRegression/*.h \
           ../../GRT/RegressionModules/MultidimensionalRegression/*.h

#Add the GRT Util module headers
HEADERS += ../../GRT/Util/*.h

#Add the GRT Classification modules to the source
SOURCES += ../../GRT/ClassificationModules/AdaBoost/*.cpp \
           ../../GRT/ClassificationModules/AdaBoost/WeakClassifiers/*.cpp \
           ../../GRT/ClassificationModules/ANBC/*.cpp \
           ../../GRT/ClassificationModules/BAG/*.cpp \
           ../../GRT/ClassificationModules/DecisionTree/*.cpp \
           ../../GRT/ClassificationModules/DTW/*.cpp \
           ../../GRT/ClassificationModules/GMM/*.cpp \
           ../../GRT/ClassificationModules/HMM/*.cpp \
           ../../GRT/ClassificationModules/KNN/*.cpp \
           ../../GRT/ClassificationModules/LDA/*.cpp \
           ../../GRT/ClassificationModules/MinDist/*.cpp \
           ../../GRT/ClassificationModules/RandomForests/*.cpp \
           ../../GRT/ClassificationModules/Softmax/*.cpp \
           ../../GRT/ClassificationModules/SVM/*.cpp \
           ../../GRT/ClassificationModules/SVM/LIBSVM/*.cpp

#Add the GRT Clustering modules to the source
SOURCES += ../../GRT/ClusteringModules/GaussianMixtureModels/*.cpp \
           ../../GRT/ClusteringModules/HierarchicalClustering/*.cpp \
           ../../GRT/ClusteringModules/KMeans/*.cpp \
           ../../GRT/ClusteringModules/SelfOrganizingMap/*.cpp

#Add the GRT Context modules to the source
SOURCES += ../../GRT/ContextModules/*.cpp

#Add the Core Alogrithm source
SOURCES += ../../GRT/CoreAlgorithms/BernoulliRBM/*.cpp \
           ../../GRT/CoreAlgorithms/PrincipalComponentAnalysis/*.cpp

#Add the GRT Pipeline and Core Modules source
SOURCES += ../../GRT/CoreModules/*.cpp

#Add the GRT Data Structures to the source
SOURCES += ../../GRT/DataStructures/*.cpp

#Add the GRT Feature Extraction modules to the source
SOURCES += ../../GRT/FeatureExtractionModules/FFT/*.cpp \
           ../../GRT/FeatureExtractionModules/KMeansFeatures/*.cpp \
           ../../GRT/FeatureExtractionModules/KMeansQuantizer/*.cpp \
           ../../GRT/FeatureExtractionModules/MovementIndex/*.cpp \
           ../../GRT/FeatureExtractionModules/MovementTrajectoryFeatures/*.cpp \
           ../../GRT/FeatureExtractionModules/RBMQuantizer/*.cpp \
           ../../GRT/FeatureExtractionModules/SOMQuantizer/*.cpp \
           ../../GRT/FeatureExtractionModules/TimeDomainFeatures/*.cpp \
           ../../GRT/FeatureExtractionModules/TimeseriesBuffer/*.cpp \
           ../../GRT/FeatureExtractionModules/ZeroCrossingCounter/*.cpp

#Add the GRT PostProcessing modules to the source
SOURCES +=  ../../GRT/PostProcessingModules/*.cpp

#Add the GRT PreProcessing modules to the source
SOURCES += ../../GRT/PreProcessingModules/*.cpp

#Add the GRT Regression modules to the source
SOURCES += ../../GRT/RegressionModules/ArtificialNeuralNetworks/MLP/*.cpp \
           ../../GRT/RegressionModules/LinearRegression/*.cpp \
           ../../GRT/RegressionModules/LogisticRegression/*.cpp \
           ../../GRT/RegressionModules/MultidimensionalRegression/*.cpp

#Add the GRT Util modules to the source
SOURCES += ../../GRT/Util/*.cpp

FORMS += mainwindow.ui \
    timeseriesgraph.ui \
    bargraph.ui \
    versioninfo.ui

OTHER_FILES += Resources/Images/Button1.png \
               Resources/Images/ClassificationModeImage.png \
               Resources/Images/RegressionModeImage.png \
               Resources/Images/TimeseriesModeImage.png

RESOURCES += Resources/resource.qrc
