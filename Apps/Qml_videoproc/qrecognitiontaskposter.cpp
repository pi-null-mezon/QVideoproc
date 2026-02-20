#include "qrecognitiontaskposter.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonParseError>
#include <QJsonObject>
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
    (_qimg.scaled(QSize(640,480),Qt::KeepAspectRatio,Qt::SmoothTransformation)).save(&_qbuffer,"JPEG",97);

    _photo.setBody(_qba);
    _fields->append(_photo);

    QNetworkRequest _request(QUrl::fromUserInput(apiurl));
    QNetworkAccessManager _netmgr;
    QNetworkReply *_reply = _netmgr.post(_request, _fields);
    QObject::connect(_reply, SIGNAL(finished()), this, SLOT(quit()));
    exec();

    QString _result;
    QString _resultcolorname = "red";
    if(_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200) {
        QJsonParseError _jperror;
        QByteArray _replydata = _reply->readAll();
        QJsonObject _json = QJsonDocument::fromJson(_replydata,&_jperror).object();
        if(_jperror.error == QJsonParseError::NoError) {
            if(QString::compare(_json.value("status").toString(),"Success",Qt::CaseInsensitive) == 0) {
                qInfo("%s", _replydata.constData());
                double _distance = _json.value("distance").toDouble();
                if(_distance < _json.value("distancethresh").toDouble()) {
                    _result = _json.value("labelinfo").toString();
                    //_result = _replydata;
                    _resultcolorname = "#88EE00";
                } else {
                    _result = _replydata;
                }
            } else {
                qWarning("%s", _replydata.constData());
                _result = tr("Error on identification");
            }
        } else {
            qWarning("JSON parser error - %s", _jperror.errorString().toUtf8().constData());
            _result = tr("Can not parse JSON from server");
        }        
    } else {
        if(_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid())
            _result = tr("Reply status code: %1").arg(_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toString());
        else
            _result = tr("Can not connect to the server");
    }

    _result = QString("<font color='%2'>%1</font>").arg(_result,_resultcolorname);
    emit replyReady(_result.toUtf8());

    QFile::remove(imgfilename);
    _fields->setParent(_reply);
    _reply->deleteLater();
}
