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

include($${PWD}/../../Src/opencv.pri)
include(opengl.pri)

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat


ANDROID_PACKAGE_SOURCE_DIR = $${PWD}/android



