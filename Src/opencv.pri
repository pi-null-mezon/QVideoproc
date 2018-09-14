#--------------------------------------------------------OPENCV----------------------------------------------------
#Specify a path to the build directory of opencv library and library version
win32 {
    OPENCV_VERSION = 330
    OPENCV_DIR = C:/Programming/3rdParties/opencv$${OPENCV_VERSION}/build
    DEFINES += OPENCV_DIRECTORY=\\\"$${OPENCV_DIR}/..\\\"
    INCLUDEPATH += $${OPENCV_DIR}/include

    #Specify the part of OpenCV path corresponding to compiler version
    win32-msvc2010: OPENCV_COMPILER_DIR = vc10
    win32-msvc2012: OPENCV_COMPILER_DIR = vc11
    win32-msvc2013: OPENCV_COMPILER_DIR = vc12
    win32-msvc2015: OPENCV_COMPILER_DIR = vc14
    win32-g++:      OPENCV_COMPILER_DIR = mingw

    #Specify the part of OpenCV path corresponding to target architecture
    win32:contains(QMAKE_TARGET.arch, x86_64){
            OPENCV_ARCHITECTURE_DIR = x64
    } else {
            OPENCV_ARCHITECTURE_DIR = x86
    }

    #A tricky way to resolve debug and release library versions
    defineReplace(qtLibraryName) {
       unset(LIBRARY_NAME)
       LIBRARY_NAME = $$1
       CONFIG(debug, debug|release): RET = $$member(LIBRARY_NAME, 0)d
       isEmpty(RET):RET = $$LIBRARY_NAME
       return($$RET)
    }

    #Specify path to *.lib files
    win32-msvc*:LIBS += -L$${OPENCV_DIR}/$${OPENCV_ARCHITECTURE_DIR}/$${OPENCV_COMPILER_DIR}/lib/
    win32-msvc*:LIBS += -L$${OPENCV_DIR}/$${OPENCV_ARCHITECTURE_DIR}/$${OPENCV_COMPILER_DIR}/bin/
    win32-g++:  LIBS += -L$${OPENCV_DIR}/$${OPENCV_ARCHITECTURE_DIR}/$${OPENCV_COMPILER_DIR}/bin/

    #Specify names of *.lib files
    LIBS += -l$$qtLibraryName(opencv_core$${OPENCV_VERSION}) \
            -l$$qtLibraryName(opencv_highgui$${OPENCV_VERSION}) \
            -l$$qtLibraryName(opencv_imgproc$${OPENCV_VERSION}) \
            -l$$qtLibraryName(opencv_videoio$${OPENCV_VERSION}) \
            -l$$qtLibraryName(opencv_objdetect$${OPENCV_VERSION}) \
            -l$$qtLibraryName(opencv_imgcodecs$${OPENCV_VERSION})

    message(OpenCV library version $${OPENCV_DIR}/$${OPENCV_ARCHITECTURE_DIR}/$${OPENCV_COMPILER_DIR} will be used)
}
#------------------------------------------------------------------------
unix {
    OPENCV_DIR = /home/pi/Programming/3rdParties/opencv/build
    LIBS += -L/usr/local/lib
    LIBS += -L/usr/local/bin
    LIBS += -lopencv_core \
            -lopencv_objdetect \
            -lopencv_highgui \
            -lopencv_imgproc \
            -lopencv_videoio \
            -lopencv_imgcodecs
}
#------------------------------------------------------------------------
android {

# TO DO: add architecture parsing in this file

ANDROID_LIBS_PATH = C:/Android/OpenCV-android-sdk/sdk/native
# Original instruction has been found here:
# http://stackoverflow.com/questions/27562311/how-to-integrate-opencv-into-qt-creator-android-project

INCLUDEPATH += $${ANDROID_LIBS_PATH}/jni/include

    LIBS += -L$${ANDROID_LIBS_PATH}/3rdparty/libs/armeabi-v7a\
            -L$${ANDROID_LIBS_PATH}/libs/armeabi-v7a

    LIBS += -llibtiff \
            -llibjpeg \
            -llibjasper \
            -llibpng \
            -lIlmImf \
            -ltbb \
            -llibwebp \
            -lopencv_core \
            -lopencv_flann \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lopencv_features2d \
            -lopencv_calib3d \
            -lopencv_ml \
            -lopencv_objdetect \
            -lopencv_video \
            -lopencv_videoio \
            -lopencv_photo \
            -lopencv_java3 \
            -lopencv_ts \
            -lopencv_imgcodecs

    ANDROID_PACKAGE_SOURCE_DIR = $${PWD}/android

    # !READ THIS! Manual step required to run the app:
    # copy libopencv_java3.so from "OpenCV-android-sdk/sdk/native/3rdparty/libs/armeabi-v7a"
    # to $${PWD}/android/libs/armeabi-v7a
}
