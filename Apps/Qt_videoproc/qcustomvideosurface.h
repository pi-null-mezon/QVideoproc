#ifndef QCUSTOMVIDEOSURFACE_H
#define QCUSTOMVIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <opencv2/core.hpp>

class QCustomVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit QCustomVideoSurface(QObject* parent = 0);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const override;

    bool present(const QVideoFrame &frame) override;

signals:
    void frameAvailable(const cv::Mat &frame, QImage::Format format);
};

#endif // QCUSTOMVIDEOSURFACE_H
