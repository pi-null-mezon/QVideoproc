QT += quick qml multimedia widgets

TEMPLATE=app

VERSION=1.0.0.0

DEFINES += APP_NAME=\\\"$${TARGET}\\\" \
           APP_VERSION=\\\"$${VERSION}\\\" \
           APP_DESIGNER=\\\"Alex_A._Taranov\\\"

SOURCES += \
           customvideofilter.cpp \
           main.cpp \
           qprocessor.cpp \
           qrecognitiontaskposter.cpp

RESOURCES += declarative-camera.qrc

HEADERS += \
    customvideofilter.h \
    qprocessor.h \
    qrecognitiontaskposter.h

include($${PWD}/../../Src/opencv.pri)

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
