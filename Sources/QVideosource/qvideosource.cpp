#include "qvideosource.h"

#include <QDialog>
#include <QCameraViewfinderSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QVariant>
#include <QByteArray>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QCameraInfo>

#include <opencv2/imgproc.hpp>

QVideoSource::QVideoSource(QObject *parent) : QObject(parent),
    backend(QVideoSource::Dummy),
    cvvideodevid(0)
{    
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<QImage::Format>("QImage::Format");    
}

QVideoSource::~QVideoSource()
{   
    close();
}

void QVideoSource::selectDevice(const QFont &_font, const QPalette &_palette)
{   
    QDialog dialog;
    dialog.setFont(_font);
    dialog.setPalette(_palette);
    dialog.setWindowTitle(tr("Выберите устройство для захвата видео"));

    QVBoxLayout dialogL;

    QComboBox devicesCB;
    QList<QCameraInfo> devlist = QCameraInfo::availableCameras();
    for(int i = 0; i < devlist.length(); i++)
        devicesCB.addItem( devlist[i].description() );

    QHBoxLayout buttonsL;
    QPushButton acceptB(tr("Выбрать"));
    connect(&acceptB, SIGNAL(clicked()), &dialog, SLOT(accept()));
    QPushButton cancelB(tr("Отмена"));
    connect(&cancelB, SIGNAL(clicked()), &dialog, SLOT(reject()));       
    buttonsL.addWidget(&acceptB);
    buttonsL.addWidget(&cancelB);

    dialogL.addWidget(&devicesCB);
    dialogL.addLayout(&buttonsL);

    dialog.setLayout(&dialogL);  

    if(dialog.exec() == QDialog::Accepted) {        
        if(devlist.size() > 0) {
            qcamdevname = devlist[devicesCB.currentIndex()].deviceName().toUtf8();
            cvvideodevid = devicesCB.currentIndex();
        } else {
            qcamdevname = "-1";
            cvvideodevid = -1;
        }
    } else {
        qcamdevname = "-1";
        cvvideodevid = -1;
    }
}

bool QVideoSource::open()
{
    bool _result = false;
    switch(getBackend()) {
        case QVideoSource::Qt:
            pt_qcam = new QCamera(qcamdevname);
            if(pt_qcam->isAvailable()) {
                connect(pt_qcam, SIGNAL(error(QCamera::Error)), this, SLOT(__onError(QCamera::Error)));
                pt_qcam->setViewfinder(&m_qvideosurface);
                pt_qcam->start();
                _result = true;
            }
            break;
         case QVideoSource::OpenCV:
            if(cvvideodevid > -1) {
                if(cvvideocap.open(cvvideodevid)) {
                    _result = true;
                    QTimer::singleShot(OPENCV_BACKEND_TIME_DELAY,cvvideocaptimer,SLOT(start()));
                }
            }
            break;
        default:
            qWarning("QVideoSource: invalid backend!");
            break;
    }
    return _result;
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

void QVideoSource::__onCvVideoTimerTimeout()
{
    if(cvvideocap.isOpened()) {
        cv::Mat _frame;
        if(cvvideocap.read(_frame)) {
            QImage::Format _qimgfmt;
            switch(_frame.channels()) {
                case 4:
                    _qimgfmt = QImage::Format_ARGB32;
                    break;
                case 3:
                    _qimgfmt = QImage::Format_RGB888;
                    break;
                case 1:
                    _qimgfmt = QImage::Format_Grayscale8;
                    break;
                default:
                    _qimgfmt = QImage::Format_Invalid;
                    break;
            }
            emit frameUpdated(_frame,_qimgfmt);
        }
    }
}

QVideoSource::VideoBackend QVideoSource::getBackend() const
{
    return backend;
}

void QVideoSource::setBackend(const VideoBackend &value)
{
    if(value == getBackend()) {
        qInfo("You already use this backend...");
        return;
    }
    // We came here only if the backend is actually should be changed
    backend = value;
    close();

    switch(getBackend()) {
        case QVideoSource::Qt:
            qInfo("QVideoSource: Qt's backend selected");
            // Disconnect from the Opencv backend
            disconnect(cvvideocaptimer, SIGNAL(timeout()), this, SLOT(__onCvVideoTimerTimeout()));
            // Connect to Qt backend
            connect(&m_qvideosurface, SIGNAL(frameAvailable(cv::Mat,QImage::Format)), this, SLOT(__transformFrame(cv::Mat,QImage::Format)));
            break;
        case QVideoSource::OpenCV:
         qInfo("QVideoSource: Opencv's backend selected");
            // Disconnect from the Qt backend
            disconnect(&m_qvideosurface, SIGNAL(frameAvailable(cv::Mat,QImage::Format)), this, SLOT(__transformFrame(cv::Mat,QImage::Format)));
            // Connect to Opencv backend
            connect(cvvideocaptimer, SIGNAL(timeout()), this, SLOT(__onCvVideoTimerTimeout()));
            break;
        default:
            qWarning("QVideoSource: invalid backend!");
            break;
    }
}


void QVideoSource::close()
{
    switch(getBackend()) {
        case QVideoSource::Qt:
            if(pt_qcam != NULL) {
                pt_qcam->stop();
                pt_qcam->deleteLater();
                pt_qcam = NULL;
            }
            break;
        case QVideoSource::OpenCV:
            if(cvvideocap.isOpened()) {
                QTimer::singleShot(OPENCV_BACKEND_TIME_DELAY,cvvideocaptimer,SLOT(stop()));
                QTimer::singleShot(3*cvvideocaptimer->interval(),[&]() {
                    cvvideocap.release();
                });
            }
            break;
        default:
            qWarning("QVideoSource: invalid backend!");
            break;
    }
}

void QVideoSource::pause()
{
    switch(getBackend()) {
        case QVideoSource::Qt:
            if(pt_qcam != NULL) {
                pt_qcam->stop();
            }
            break;
        case QVideoSource::OpenCV:
            if(cvvideocap.isOpened()) {
                QTimer::singleShot(OPENCV_BACKEND_TIME_DELAY,cvvideocaptimer,SLOT(stop()));
            }
            break;
        default:
            qWarning("QVideoSource: invalid backend!");
            break;
    }
}

void QVideoSource::resume()
{
    switch(getBackend()) {
        case QVideoSource::Qt:
            if(pt_qcam != NULL) {
                pt_qcam->start();
            }
            break;
        case QVideoSource::OpenCV:
            if(cvvideocap.isOpened()) {
                QTimer::singleShot(OPENCV_BACKEND_TIME_DELAY,cvvideocaptimer,SLOT(start()));
            }
            break;
        default:
            qWarning("QVideoSource: invalid backend!");
            break;
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

void QVideoSource::setTransform(FrameTransform _transform)
{
    m_transform = _transform;
}

void QVideoSource::setViewfinderSettings()
{
    switch(getBackend()) {
        case QVideoSource::Qt:
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
            break;
        case QVideoSource::OpenCV:
            if(cvvideocap.isOpened()) {
                cvvideocap.set(CV_CAP_PROP_SETTINGS,0.0);
            }
            break;
        default:
            qWarning("QVideoSource: invalid backend!");
            break;
    }
}

double QVideoSource::measureActualFPS(unsigned int _howlong_to_measure_ms)
{
    // Backend independent function
    QElapsedTimer _elapsedtimer;
    double _timeelapsed_ms = 0;
    unsigned int  _frames = 0;
    unsigned int  _startshift = 5;
    connect(this, &QVideoSource::frameUpdated, [&]() {
        if(_frames == _startshift) {
            _elapsedtimer.start();
        } else if(_frames > _startshift) {
            _timeelapsed_ms = _elapsedtimer.elapsed();
            qInfo("%d) %f ms", _frames, _timeelapsed_ms);
        }
        _frames++;        
    });
    QEventLoop _el;
    QTimer::singleShot(_howlong_to_measure_ms,&_el,SLOT(quit()));
    _el.exec();
    disconnect();
    double _actualfps = 1000.0*(_frames-_startshift-1)/_timeelapsed_ms;
    qInfo("QVideoSource: actual FPS measured %f", _actualfps);
    return _actualfps;
}

void QVideoSource::prepareForVPG(int _w, int _h, double _fps)
{
    switch(getBackend()) {
        case QVideoSource::Qt:
            if(pt_qcam) {
                QList<QCameraViewfinderSettings> settingsList = pt_qcam->supportedViewfinderSettings();
                for(int i = 0; i < settingsList.length(); i++) {
                    QSize res = settingsList[i].resolution();
                    qreal fps = settingsList[i].minimumFrameRate();
                    if(res.width() == _w && res.height() == _h && fps > _fps-1.0 && fps < _fps+1.0) {
                        pt_qcam->setViewfinderSettings(settingsList[i]);
                        qInfo("Video mode %d x %d x %.1f has been applied", res.width(), res.height(), fps);
                        return;
                    }
                }
                qWarning("Appropriate view finder settings for PPG measurement has not been found!");
            }
            break;
        case QVideoSource::OpenCV:
            if(cvvideocap.isOpened()) {
                pause();
                QTimer::singleShot(cvvideocaptimer->interval()*3,[&](){
                    cvvideocap.set(CV_CAP_PROP_FRAME_WIDTH, _w);
                    cvvideocap.set(CV_CAP_PROP_FRAME_HEIGHT, _h);
                    cvvideocap.set(CV_CAP_PROP_FPS, _fps);
                    resume();
                });
            }
            break;
        default:
            qWarning("QVideoSource: invalid backend!");
            break;
    }
    emit prepareForVPG();
}

void QVideoSource::initOpencvBackend()
{
    cvvideocaptimer = new QTimer();
    cvvideocaptimer->setTimerType(Qt::PreciseTimer);
    cvvideocaptimer->setInterval(66);
    setBackend(QVideoSource::OpenCV);
}
