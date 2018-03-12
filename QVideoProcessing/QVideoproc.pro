#-------------------------------------------------
#
# Project created by QtCreator 2014-02-08T18:00:16
#
#-------------------------------------------------

QT +=   core gui

greaterThan(QT_MAJOR_VERSION, 4) {
    QT +=   widgets multimedia
}

TEMPLATE =  app
TARGET  =   QVideoproc
VERSION =   1.0.0.0

DEFINES +=  APP_NAME=\\\"$${TARGET}\\\" \
            APP_VERS=\\\"$${VERSION}\\\"

SOURCES +=  main.cpp\
            mainwindow.cpp \
            qimagewidget.cpp \
            qopencvprocessor.cpp \
            qvideocapture.cpp

HEADERS  += mainwindow.h \
            qimagewidget.h \
            qopencvprocessor.h \
            qvideocapture.h

include(opencv.pri)
include(opengl.pri)

CONFIG += designbuild

designbuild {
    message("Design build configuration was selected, design paths will be used")
    DEFINES += HAARCASCADES_PATH=\\\"$${OPENCV_DIR}/../sources/data/haarcascades/\\\" \
               LBPCASCADES_PATH=\\\"$${OPENCV_DIR}/../sources/data/lbpcascades/\\\"
} else {
    CONFIG(release, debug|release): DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT
    message("Release build configuration was selected, deployment paths will be used")
}





