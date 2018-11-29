TEMPLATE=app

TARGET=OIRTClient
VERSION=1.0.0.0

DEFINES += APP_NAME=\\\"$${TARGET}\\\" \
           APP_VERSION=\\\"$${VERSION}\\\" \
           APP_DESIGNER=\\\"Alex_A._Taranov\\\"

QT += quick qml multimedia

SOURCES += qmlcamera.cpp \
    qrecognitiontaskposter.cpp
RESOURCES += declarative-camera.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/declarative-camera
INSTALLS += target
include(../../shared/shared.pri)

HEADERS += \
    qrecognitiontaskposter.h
