#include "qrecognitiontaskposter.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QUrl>
#include <QFile>
#include <QImage>
#include <QBuffer>

QRecognitionTaskPoster::QRecognitionTaskPoster(const QString &_apiurl, const QString &_imgfilename, QObject *parent) : QThread(parent),
    apiurl(_apiurl),
    imgfilename(_imgfilename)
{
}

void QRecognitionTaskPoster::run()
{
    QHttpMultiPart *_fields = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart _photo;
    _photo.setHeader(QNetworkRequest::ContentTypeHeader, "image/jpeg");
    _photo.setHeader(QNetworkRequest::ContentDispositionHeader, "form-data; name=\"file\"; filename=\"face.jpg\"");
    QImage _qimg;
    _qimg.load(imgfilename);
    QByteArray _qba;
    QBuffer _qbuffer(&_qba);
    _qbuffer.open(QIODevice::ReadWrite);
    (_qimg.scaled(QSize(640,360),Qt::KeepAspectRatio)).save(&_qbuffer,"JPEG");

    _photo.setBody(_qba);
    _fields->append(_photo);

    QNetworkRequest _request(QUrl::fromUserInput(apiurl));
    QNetworkAccessManager _netmgr;
    QNetworkReply *_reply = _netmgr.post(_request, _fields);
    QObject::connect(_reply, SIGNAL(finished()), this, SLOT(quit()));
    exec();

    /*if(_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
        QJsonParseError _jperror;
        QByteArray _replydata = _reply->readAll();
        QJsonObject _json = QJsonDocument::fromJson(_replydata,&_jperror).object();
        if(_jperror.error == QJsonParseError::NoError) {
            if(QString::compare(_json.value("status").toString(),"Success",Qt::CaseInsensitive) == 0) { // equal strings
                qInfo("%s", _replydata.constData());
                double _distance = _json.value("distance").toDouble();
                if(_distance < _json.value("distancethresh").toDouble()) {
                    emit labelPredicted(_json.value("label").toInt(),
                                        _distance,
                                        _json.value("labelinfo").toString().toUtf8().constData(),
                                        facerr);
                } else {
                    emit labelPredicted(-1,DBL_MAX,"",facerr);
                }
            } else {
                qWarning("%s", _replydata.constData());
                emit labelPredicted(-1,DBL_MAX,"",facerr);
            }
        } else {
            qWarning("JSON parser error - %s", _jperror.errorString().toUtf8().constData());
        }
    }*/
    emit replyReady(_reply->readAll());

    QFile::remove(imgfilename);
    _fields->setParent(_reply);
    _reply->deleteLater();
}

void QProxyObject::postTask(const QString &_filename)
{
    QRecognitionTaskPoster *thread = new QRecognitionTaskPoster(getApiurl(),_filename);
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    connect(thread,SIGNAL(replyReady(QString)),this,SIGNAL(replyReady(QString)));
    thread->start();
}

QProxyObject::QProxyObject(QObject *_parent) : QObject(_parent)
{
}

QString QProxyObject::getApiurl() const
{
    return apiurl;
}

void QProxyObject::setApiurl(const QString &value)
{
    apiurl = value;
}
