#ifndef QCUSTOMVIDEOSURFACE_H
#define QCUSTOMVIDEOSURFACE_H

#include <QAbstractVideoSurface>

class QCustomVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit QCustomVideoSurface(QObject* parent=nullptr);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const override;

    bool present(const QVideoFrame &frame) override;

signals:
    void frameReady(const QImage &_qimage);
};

#endif // QCUSTOMVIDEOSURFACE_H
