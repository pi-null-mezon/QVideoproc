#include "qvideosource.h"

QVideoSource::QVideoSource(QObject *parent) : QObject(parent),
    qcamera(nullptr)
{
    qvideosurface = new QCustomVideoSurface(this);
    connect(qvideosurface, SIGNAL(frameReady(QImage)), this, SIGNAL(frameReady(QImage)));
}

void QVideoSource::open(const QCameraInfo &_qcaminfo)
{
    delete qcamera;
    qcamera = new QCamera(_qcaminfo);
    connect(qcamera, SIGNAL(error(QCamera::Error)), this, SLOT(__onError(QCamera::Error)));
    qcamera->setViewfinder(qvideosurface);
}

void QVideoSource::__onError(QCamera::Error _error)
{
    switch(_error) {
        case QCamera::NoError:
            qWarning("QCamera: no errors have occurred");
            break;
        case QCamera::CameraError:
            qWarning("QCamera: an error has occurred");
            break;
        case QCamera::InvalidRequestError:
            qWarning("QCamera: system resource doesn't support requested functionality");
            break;
        case QCamera::ServiceMissingError:
            qWarning("QCamera: no camera service available");
            break;
        case QCamera::NotSupportedFeatureError:
            qWarning("QCamera: the feature is not supported");
            break;
    }
}

void QVideoSource::close()
{
    if(qcamera != nullptr) {
        qcamera->stop();
    }
}

void QVideoSource::pause()
{
    if(qcamera != nullptr) {
        qcamera->stop();
    }
}

void QVideoSource::resume()
{
    if(qcamera != nullptr) {
        qcamera->start();
    }
}

void QVideoSource::setViewfinderSettings(const QCameraViewfinderSettings &_settings)
{
    if(qcamera != nullptr) {
        qcamera->setViewfinderSettings(_settings);
    }
}

QList<QCameraViewfinderSettings> QVideoSource::supportedViewfinderSettings() const
{
    if(qcamera != nullptr) {
        return qcamera->supportedViewfinderSettings();
    }
    return QList<QCameraViewfinderSettings>();
}
