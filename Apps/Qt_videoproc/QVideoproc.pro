#-------------------------------------------------
#
# Project created by QtCreator 2016-09-25T11:52:14
#
#-------------------------------------------------

QT       += core gui multimedia
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
VERSION = 1.0.0.0

SOURCES += main.cpp\
        mainwindow.cpp \
    qdisplaywidget.cpp \
    qvideosource.cpp \
    qcustomvideosurface.cpp

HEADERS  += mainwindow.h \
    qdisplaywidget.h \
    qvideosource.h \
    qcustomvideosurface.h

FORMS    += mainwindow.ui

include($${PWD}/../../Src/opencv.pri)

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat






