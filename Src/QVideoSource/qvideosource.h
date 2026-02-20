#ifndef QVIDEOSOURCE_H
#define QVIDEOSOURCE_H

#include <QObject>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinderSettings>

#include "qcustomvideosurface.h"

class QVideoSource : public QObject
{
    Q_OBJECT
public:
    explicit QVideoSource(QObject *parent=nullptr);

signals:
    void frameReady(const QImage &_qimage);
    void cameraReady();

public slots:
    void open(const QCameraInfo &_qcaminfo);
    void pause();
    void resume();
    void close();
    void setViewfinderSettings(const QCameraViewfinderSettings &_settings);
    QList<QCameraViewfinderSettings> supportedViewfinderSettings() const;

private slots:
    void __onError(QCamera::Error  _error);
    void __onStatusChanged(QCamera::Status _status);

private:
    QCamera *qcamera;
    QCustomVideoSurface *qvideosurface;
    bool cameraLoaded;
};


#endif // QVIDEOSOURCE_H
