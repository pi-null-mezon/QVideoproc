#include "qcustomvideosurface.h"


QCustomVideoSurface::QCustomVideoSurface(QObject* parent): QAbstractVideoSurface(parent)
{}

QList<QVideoFrame::PixelFormat> QCustomVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    return QList<QVideoFrame::PixelFormat>()
            << QVideoFrame::Format_RGB32
            /*<< QVideoFrame::Format_ARGB32
            << QVideoFrame::Format_ARGB32_Premultiplied
            << QVideoFrame::Format_RGB32
            << QVideoFrame::Format_RGB24
            << QVideoFrame::Format_RGB565
            << QVideoFrame::Format_RGB555
            << QVideoFrame::Format_ARGB8565_Premultiplied
            << QVideoFrame::Format_BGRA32
            << QVideoFrame::Format_BGRA32_Premultiplied
            << QVideoFrame::Format_BGR32
            << QVideoFrame::Format_BGR24
            << QVideoFrame::Format_BGR565
            << QVideoFrame::Format_BGR555
            << QVideoFrame::Format_BGRA5658_Premultiplied
            << QVideoFrame::Format_AYUV444
            << QVideoFrame::Format_AYUV444_Premultiplied
            << QVideoFrame::Format_YUV444
            << QVideoFrame::Format_YUV420P
            << QVideoFrame::Format_YV12
            << QVideoFrame::Format_UYVY
            << QVideoFrame::Format_YUYV
            << QVideoFrame::Format_NV12
            << QVideoFrame::Format_NV21
            << QVideoFrame::Format_IMC1
            << QVideoFrame::Format_IMC2
            << QVideoFrame::Format_IMC3
            << QVideoFrame::Format_IMC4
            << QVideoFrame::Format_Y8
            << QVideoFrame::Format_Y16
            << QVideoFrame::Format_Jpeg
            << QVideoFrame::Format_CameraRaw
            << QVideoFrame::Format_AdobeDng*/;
}

bool QCustomVideoSurface::present(const QVideoFrame &frame) {

    if(frame.isValid()) {
        QVideoFrame cloneFrame(frame); // makes a shallow copy (since QVideoFrame is explicitly shared), to get the access to the pixel data
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
#ifdef Q_OS_ANDROID
        cv::Mat mat(cloneFrame.height(), cloneFrame.width(), CV_8UC4, (void *)cloneFrame.bits());
        emit frameAvailable(mat, QImage::Format_RGBX8888);
#else
        QImage::Format format = QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat());
        int cvtype = CV_8UC1;
        switch(format) {
            case QImage::Format_RGB32:
                cvtype = CV_8UC4;
                break;
            case QImage::Format_RGB888:
                cvtype = CV_8UC3;
                break;
            case QImage::Format_Invalid:
                qWarning("QCustomVideoSurface Warning: image format is QImage::Format_Invalid");
                return false;
            default:
                // TO DO add the new formats if find
                qWarning("QCustomVideoSurface Warning: image format is not implemented (QImage::Format %d)", format);
                return false;
        }
        cv::Mat mat(cloneFrame.height(), cloneFrame.width(), cvtype, (void *)cloneFrame.bits());
        cv::flip(mat,mat,0);
        emit frameAvailable(mat, format);
#endif
        cloneFrame.unmap();
        return true;
    }
    return false;
}

