#--------------------------------------------------------OPENCV----------------------------------------------------
#Specify a path to the build directory of opencv library and library version
win32 {
    OPENCV_VERSION = 412
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
linux {
    INCLUDEPATH += /usr/local/include/opencv4
    LIBS += -lopencv_core \
            -lopencv_objdetect \
            -lopencv_highgui \
            -lopencv_imgproc \
            -lopencv_videoio \
            -lopencv_imgcodecs
}
#------------------------------------------------------------------------
android { # builds successfully on Windows with android-ndk-r19c, Qt5.13.1 and opencv411
    contains(ANDROID_TARGET_ARCH,$${ANDROID_TARGET_ARCH}) {
        INCLUDEPATH += $${PWD}/../../../Android/Opencv-sdk/sdk/native/jni/include

        LIBS += -L"$${PWD}/../../../Android/Opencv-sdk/sdk/native/staticlibs/$${ANDROID_TARGET_ARCH}" \
                -lopencv_videoio \
                -lopencv_dnn \
                -lopencv_features2d \
                -lopencv_flann \
                -lopencv_highgui \
                -lopencv_imgcodecs \
                -lopencv_imgproc \
                -lopencv_ml \
                -lopencv_objdetect \
                -lopencv_photo \
                -lopencv_stitching \
                -lopencv_video \
                -lopencv_core

        LIBS += \
            -L"$${PWD}/../../../Android/Opencv-sdk/sdk/native/3rdparty/libs/$${ANDROID_TARGET_ARCH}" \
            -ltegra_hal \
            -lcpufeatures \
            -lIlmImf \
            -littnotify \
            -llibjasper \
            -llibjpeg-turbo \
            -llibpng \
            -llibprotobuf \
            -llibtiff \
            -llibwebp \
            -lquirc \
            -ltbb
    } else {
        error("Unsupported architecture: $${ANDROID_TARGET_ARCH}")
    }
}
