TEMPLATE=app
TARGET=qmlcamera

QT += quick qml multimedia

SOURCES += qmlcamera.cpp \
    qrecognitiontaskposter.cpp
RESOURCES += declarative-camera.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/declarative-camera
INSTALLS += target
include(../../shared/shared.pri)

HEADERS += \
    qrecognitiontaskposter.h
