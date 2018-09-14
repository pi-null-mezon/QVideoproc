#include "qcustomvideosurface.h"

#include <QDebug>


QCustomVideoSurface::QCustomVideoSurface(QObject* parent): QAbstractVideoSurface(parent)
{
}

QList<QVideoFrame::PixelFormat> QCustomVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    qDebug() << handleType;
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_RGB24;
    }
    return QList<QVideoFrame::PixelFormat>();
}

bool QCustomVideoSurface::present(const QVideoFrame &frame)
{
    if(frame.isValid()) {
        QVideoFrame cloneFrame(frame); // makes a shallow copy (since QVideoFrame is explicitly shared), to get the access to the pixel data
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        qDebug() << cloneFrame.pixelFormat();
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
        cloneFrame.unmap();
        return true;
    }
    return false;
}

