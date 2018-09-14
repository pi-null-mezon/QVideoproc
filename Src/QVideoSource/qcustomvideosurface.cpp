#include "qcustomvideosurface.h"

#include <QVideoFrame>

QCustomVideoSurface::QCustomVideoSurface(QObject* parent): QAbstractVideoSurface(parent)
{
}

QList<QVideoFrame::PixelFormat> QCustomVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
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
        QVideoFrame _videoframe(frame); // makes a shallow copy (since QVideoFrame is explicitly shared), to get the access to the pixel data
        _videoframe.map(QAbstractVideoBuffer::ReadOnly);
        QImage _qimage(_videoframe.bits(),
                       _videoframe.width(),
                       _videoframe.height(),
                       QVideoFrame::imageFormatFromPixelFormat(_videoframe.pixelFormat()));
        emit frameReady(_qimage.mirrored().copy());
        _videoframe.unmap();
        return true;
    }
    return false;
}

