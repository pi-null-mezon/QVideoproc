#include "qvideosource.h"

#include <QDialog>
#include <QCameraViewfinderSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QVariant>
#include <QByteArray>

#include <opencv2/imgproc.hpp>

QVideoSource::QVideoSource(QObject *parent) : QObject(parent),
    pt_qcam(nullptr),
    m_transform(QVideoSource::NoTransform)
{
    connect(&m_qvideosurface, SIGNAL(frameAvailable(cv::Mat,QImage::Format)), this, SLOT(__transformFrame(cv::Mat,QImage::Format)));
}

void QVideoSource::selectDevice()
{
    QDialog dialog;
    int pS = dialog.font().pointSize();
    dialog.resize(40*pS, 10*pS);

    QVBoxLayout dialogL;

    QComboBox devicesCB;
    QList<QByteArray> devlist = QCamera::availableDevices();
    for(int i = 0; i < devlist.length(); i++)
        devicesCB.addItem(QCamera::deviceDescription(devlist[i]));

    QHBoxLayout buttonsL;
    QPushButton cancelB(tr("Cancel"));
    connect(&cancelB, SIGNAL(clicked()), &dialog, SLOT(reject()));
    QPushButton acceptB(tr("Accept"));
    connect(&acceptB, SIGNAL(clicked()), &dialog, SLOT(accept()));
    buttonsL.addWidget(&cancelB);
    buttonsL.addWidget(&acceptB);

    dialogL.addWidget(&devicesCB);
    dialogL.addLayout(&buttonsL);

    dialog.setLayout(&dialogL);

    if(dialog.exec() == QDialog::Accepted) {
        delete pt_qcam;
        pt_qcam = nullptr;
        pt_qcam = new QCamera(devlist[devicesCB.currentIndex()]);
    }
}

bool QVideoSource::open()
{
    if(pt_qcam != nullptr) {
        connect(pt_qcam, SIGNAL(error(QCamera::Error)), this, SLOT(__onError(QCamera::Error)));
        pt_qcam->setViewfinder(&m_qvideosurface);
        pt_qcam->start();
        return true;
    } else {
        return false;
    }
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
    if(pt_qcam != nullptr) {
        pt_qcam->stop();
    }
}

void QVideoSource::pause()
{
    if(pt_qcam != nullptr) {
        pt_qcam->stop();
    }
}

void QVideoSource::resume()
{
    if(pt_qcam != nullptr) {
        pt_qcam->start();
    }
}

void QVideoSource::__transformFrame(const cv::Mat &_mat, QImage::Format format)
{
    cv::Mat tmpmat;
    switch(m_transform) {
        case NoTransform:
            tmpmat = _mat;
            break;
        case FlipVertically:
            cv::flip(_mat, tmpmat, 0);
            break;
        case FlipHorizontally:
            cv::flip(_mat, tmpmat, 1);
            break;
        case RotateClockWise90Degree:
            cv::flip(_mat, tmpmat, 0);
            cv::transpose(tmpmat,tmpmat);
            break;
        case RotateClockOpposite90Degree:
            cv::flip(_mat, tmpmat, 1);
            cv::transpose(tmpmat,tmpmat);
            break;
    }
    emit frameUpdated(tmpmat, format);
}

void QVideoSource::nextTransform()
{
    m_transform = static_cast<FrameTransform>((static_cast<int>(m_transform) + 1) % 5); // 5 is a number of FrameTransform instances
    qWarning("Transform selected %d", m_transform);
}

void QVideoSource::setTransform(FrameTransform _transform)
{
    m_transform = _transform;
}

void QVideoSource::setViewfinderSettings()
{
    if(pt_qcam) {

        QDialog dialog;
        int pS = dialog.font().pointSize();
        dialog.resize(40*pS, 10*pS);

        QVBoxLayout dialogL;

        QComboBox settingsCB;
        QList<QCameraViewfinderSettings> settingsList = pt_qcam->supportedViewfinderSettings();
        for(int i = 0; i < settingsList.length(); i++) {
            QString description = QString("%1x%2 - %3 fps").arg(QString::number(settingsList[i].resolution().width()),
                                                                QString::number(settingsList[i].resolution().height()),
                                                                QString::number(settingsList[i].minimumFrameRate(),'f',1));
            settingsCB.addItem(description);
        }

        QHBoxLayout buttonsL;
        QPushButton cancelB(tr("Cancel"));
        connect(&cancelB, SIGNAL(clicked()), &dialog, SLOT(reject()));
        QPushButton acceptB(tr("Accept"));
        connect(&acceptB, SIGNAL(clicked()), &dialog, SLOT(accept()));
        buttonsL.addWidget(&cancelB);
        buttonsL.addWidget(&acceptB);

        dialogL.addWidget(&settingsCB);
        dialogL.addLayout(&buttonsL);

        dialog.setLayout(&dialogL);

        if(dialog.exec() == QDialog::Accepted)
            pt_qcam->setViewfinderSettings(settingsList[settingsCB.currentIndex()]);
    }
}
