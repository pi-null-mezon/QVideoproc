#ifndef QVIDEOSOURCE_H
#define QVIDEOSOURCE_H

#include <QObject>
#include <QCamera>

#include <qcustomvideosurface.h>
#include <opencv2/core.hpp>

class QVideoSource : public QObject
{
    Q_OBJECT
public:
    enum FrameTransform {NoTransform, RotateClockWise90Degree, RotateClockOpposite90Degree, FlipVertically, FlipHorizontally};
    explicit QVideoSource(QObject *parent = 0);

signals:
    void frameUpdated(const cv::Mat &_cvmat, QImage::Format format);

public slots:
    void selectDevice();
    bool open();
    void pause();
    void resume();
    void close();
    void nextTransform();
    void setTransform(FrameTransform transform);
    void setViewfinderSettings(); // works only after QCamera::start() call

private slots:
    void __transformFrame(const cv::Mat &_mat, QImage::Format format);
    void __onError(QCamera::Error  _error);

private:
    QCamera *pt_qcam;
    QCustomVideoSurface m_qvideosurface;
    FrameTransform m_transform;
};


#endif // QVIDEOSOURCE_H
