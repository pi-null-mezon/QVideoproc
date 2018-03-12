#ifndef QVIDEOSOURCE_H
#define QVIDEOSOURCE_H

#include <QObject>
#include <QCamera>
#include <QTimer>
#include <QFont>
#include <QPalette>

#include <qcustomvideosurface.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#define OPENCV_BACKEND_TIME_DELAY 0

class QVideoSource : public QObject
{
    Q_OBJECT
public:
    explicit QVideoSource(QObject *parent = 0);
    ~QVideoSource();

    enum VideoBackend {Dummy, Qt, OpenCV}; // Dummy is not valid video backend, but it is added to be default value
    enum FrameTransform {NoTransform, RotateClockWise90Degree, RotateClockOpposite90Degree, FlipVertically, FlipHorizontally,};

    VideoBackend    getBackend() const;
    void            setBackend(const VideoBackend &value);

    double          measureActualFPS(unsigned int _howlong_to_measure_ms=1750);

signals:
    void frameUpdated(const cv::Mat &_cvmat, QImage::Format format);
    void preparedForVPG();
    float actualFPS();

public slots:
    void selectDevice(const QFont &_font=QFont(), const QPalette &_palette=QPalette());
    bool open();
    void pause();
    void resume();
    void close();
    void setTransform(FrameTransform transform);
    void setViewfinderSettings(); // works only after QCamera::start() call
    void prepareForVPG(int _w=640, int _h=480, double _fps=30.0); // works only after QCamera::start() call
    void initOpencvBackend();

private slots:
    void __transformFrame(const cv::Mat &_mat, QImage::Format format);
    void __onError(QCamera::Error  _error);
    void __onCvVideoTimerTimeout();

private:
    VideoBackend backend;
    QCamera *pt_qcam = NULL;
    QByteArray qcamdevname;
    QCustomVideoSurface m_qvideosurface;
    FrameTransform m_transform = NoTransform;

    QTimer *cvvideocaptimer;
    cv::VideoCapture cvvideocap;
    int cvvideodevid;
};


#endif // QVIDEOSOURCE_H
