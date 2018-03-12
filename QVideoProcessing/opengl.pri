#CONFIG += opengl_widgets
opengl_widgets {
    message("OpenGl will be used for build")
    DEFINES +=  USE_OPENGL_WIDGETS \
                WIDGET_CLASS=QOpenGLWidget
} else {
    DEFINES +=  WIDGET_CLASS=QWidget
}

