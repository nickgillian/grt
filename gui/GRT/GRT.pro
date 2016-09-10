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
 QMAKE_CXXFLAGS += -std=c++0x
 QMAKE_CXXFLAGS += -DOSC_HOST_LITTLE_ENDIAN
}

#Windows flags
unix:!macx {
 QMAKE_CXXFLAGS += -DOSC_HOST_LITTLE_ENDIAN
}

#Add c++ 11 support
CONFIG += static
CONFIG += c++11

#Set the build target
TARGET = GRT
macx: TEMPLATE = app

#Include the custom and library and header paths
#Note that these paths might be different for your specific machine

#OSX Include/Link
macx{
 #Add the default include and lib directories (we assume boost and GRT are installed here)
 INCLUDEPATH += /usr/local/include
 LIBS += -L/usr/local/lib

 #Add the base oscpack directory
 INCLUDEPATH += OSC/oscpack/include

 #Add the custom resources file
 ICON = Resources/OSX/GRT.icns

 #Flag that we want to use a pre-built version of the GRT library (this should be installed in /user/local/lib
 USE_GRT_LIB = "true"

 #Flag that we want to include the GRT library directly as source code
 #USE_GRT_SOURCE = "true"
}

android {
    #Add the base oscpack directory
    INCLUDEPATH += OSC/oscpack/include
    USE_GRT_SOURCE_CODE = "true"
}

#Linux pkgconfig
unix:!macx:!android{
 #Add the defauly include and lib directories (we assume boost and GRT are installed here)
 INCLUDEPATH += /usr/local/include
 LIBS += -L/usr/local/lib

 #Add the base oscpack directory
 INCLUDEPATH += OSC/oscpack/include

 #Add the custom grt pkgconfig
 CONFIG += link_pkgconfig
 PKGCONFIG += grt

 #Flag that we want to use a pre-built version of the GRT library (this should be installed in /user/local/lib
 #USE_GRT_LIB = "true"

 #Flag that we want to include the GRT library directly as source code
 USE_GRT_SOURCE_CODE = "true"
}

#Windows Include
win32{
 #Add the custom GRT paths
 INCLUDEPATH += ../..

 #Add the base oscpack directory
 INCLUDEPATH += OSC/oscpack/include

 #Flag that we want to use the GRT source (instead of the GRT precomplied library - this is much easier on Windows!)
 USE_GRT_SOURCE_CODE = "true"

 #Link against the windows libraries needed for OSC networking
 LIBS += -lws2_32
 LIBS += -lwinmm
}

#If USE_GRT_LIB is defined, then we add the prebuilt GRT lib
defined(USE_GRT_LIB,var){
 #Add the main GRT library, you should have already compiled and install this (using the main GRT cmake file)
 LIBS += -lgrt
}

#If USE_GRT_SOURCE_CODE is defined, then we add the GRT source code to the project
defined(USE_GRT_SOURCE_CODE,var){
 #Include the main GRT header
 HEADERS += ../../GRT/GRT.h

 #Add the GRT Classification module headers
 HEADERS += ../../GRT/ClassificationModules/AdaBoost/*.h \
           ../../GRT/ClassificationModules/AdaBoost/WeakClassifiers/*.h \
           ../../GRT/ClassificationModules/ANBC/*.h \
           ../../GRT/ClassificationModules/BAG/*.h \
           ../../GRT/ClassificationModules/DecisionTree/*.h \
           ../../GRT/ClassificationModules/DTW/*.h \
           ../../GRT/ClassificationModules/FiniteStateMachine/*.h \
           ../../GRT/ClassificationModules/GMM/*.h \
           ../../GRT/ClassificationModules/HMM/*.h \
           ../../GRT/ClassificationModules/KNN/*.h \
           ../../GRT/ClassificationModules/LDA/*.h \
           ../../GRT/ClassificationModules/MinDist/*.h \
           ../../GRT/ClassificationModules/ParticleClassifier/*.h \
           ../../GRT/ClassificationModules/RandomForests/*.h \
           ../../GRT/ClassificationModules/Softmax/*.h \
           ../../GRT/ClassificationModules/SVM/*.h \
           ../../GRT/ClassificationModules/SVM/LIBSVM/*.h \
           ../../GRT/ClassificationModules/SwipeDetector/*.h

 #Add the GRT Clustering module headers
 HEADERS += ../../GRT/ClusteringModules/ClusterTree/*.h \
            ../../GRT/ClusteringModules/GaussianMixtureModels/*.h \
           ../../GRT/ClusteringModules/HierarchicalClustering/*.h \
           ../../GRT/ClusteringModules/KMeans/*.h \
           ../../GRT/ClusteringModules/SelfOrganizingMap/*.h

 #Add the GRT Context module headers
 HEADERS += ../../GRT/ContextModules/*.h

 #Add the Core Alogrithm headers
 HEADERS += ../../GRT/CoreAlgorithms/BernoulliRBM/*.h \
           ../../GRT/CoreAlgorithms/EvolutionaryAlgorithm/*.h \
           ../../GRT/CoreAlgorithms/LeastSquares/LinearLeastSquares.h \
           ../../GRT/CoreAlgorithms/ParticleFilter/*.h \
           ../../GRT/CoreAlgorithms/ParticleSwarmOptimization/*.h \
           ../../GRT/CoreAlgorithms/PrincipalComponentAnalysis/*.h  \
           ../../GRT/CoreAlgorithms/Tree/*.h

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
           ../../GRT/ClassificationModules/FiniteStateMachine/*.cpp \
           ../../GRT/ClassificationModules/GMM/*.cpp \
           ../../GRT/ClassificationModules/HMM/*.cpp \
           ../../GRT/ClassificationModules/KNN/*.cpp \
           ../../GRT/ClassificationModules/LDA/*.cpp \
           ../../GRT/ClassificationModules/MinDist/*.cpp \
           ../../GRT/ClassificationModules/ParticleClassifier/*.cpp \
           ../../GRT/ClassificationModules/RandomForests/*.cpp \
           ../../GRT/ClassificationModules/Softmax/*.cpp \
           ../../GRT/ClassificationModules/SVM/*.cpp \
           ../../GRT/ClassificationModules/SVM/LIBSVM/*.cpp \
           ../../GRT/ClassificationModules/SwipeDetector/*.cpp

 #Add the GRT Clustering modules to the source
 SOURCES += ../../GRT/ClusteringModules/ClusterTree/*.cpp \
            ../../GRT/ClusteringModules/GaussianMixtureModels/*.cpp \
           ../../GRT/ClusteringModules/HierarchicalClustering/*.cpp \
           ../../GRT/ClusteringModules/KMeans/*.cpp \
           ../../GRT/ClusteringModules/SelfOrganizingMap/*.cpp

 #Add the GRT Context modules to the source
 SOURCES += ../../GRT/ContextModules/*.cpp

 #Add the Core Alogrithm source
 SOURCES += ../../GRT/CoreAlgorithms/BernoulliRBM/*.cpp \
           ../../GRT/CoreAlgorithms/PrincipalComponentAnalysis/*.cpp    \
           ../../GRT/CoreAlgorithms/Tree/*.cpp

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

} #end of USE_GRT_SOURCE_CODE

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
