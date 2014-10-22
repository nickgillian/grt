#-------------------------------------------------
#
# Project created by QtCreator 2013-09-11T10:29:42
#
#-------------------------------------------------

#NOTE: Have a look at the GRT GUI README before attempting to build this project

QT  += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

#OSX flags
macx {
 QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=gnu0x -stdlib=libc++
 QMAKE_CXXFLAGS += -DOSC_HOST_LITTLE_ENDIAN
}

#Linux flags
unix:!macx {
 QMAKE_CXXFLAGS += -std=c++0x
 QMAKE_CXXFLAGS += -DOSC_HOST_LITTLE_ENDIAN
}

#Add c++ 11 support
CONFIG += c++11

CONFIG += static
TARGET = GRT
macx: TEMPLATE = app

#Include the boost libraries, the headers and static library files might be in a different location on your machine
#OSX Include
macx{
 INCLUDEPATH += /usr/local/include
 LIBS += -L/usr/local/lib
 ICON = Resources/OSX/GRT.icns
}

#Linux pkgconfig
unix:!macx{
 CONFIG += link_pkgconfig
 PKGCONFIG += grt
}

#Windows Include
win32{
 INCLUDEPATH += "INSERT_PATH_TO_BOOST_HEADERS_HERE"
 LIBS += "INSERT_PATH_TO_BOOST_LIBS_HERE"
}

#Regardless of the OS we need to include the boost libraries
LIBS += -lboost_thread
LIBS += -lboost_date_time
LIBS += -lboost_system
LIBS += -lboost_chrono

#Add the main GRT library, you should compile and install this (using the main GRT cmake file before you try and build the GUI)
LIBS += -lgrt

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

FORMS += mainwindow.ui \
    timeseriesgraph.ui \
    bargraph.ui \
    versioninfo.ui

OTHER_FILES += Resources/Images/Button1.png \
               Resources/Images/ClassificationModeImage.png \
               Resources/Images/RegressionModeImage.png \
               Resources/Images/TimeseriesModeImage.png

RESOURCES += Resources/resource.qrc
