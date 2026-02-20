#include "qprocessor.h"

#include <QVariant>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDir>

#include "qrecognitiontaskposter.h"

QProcessor::QProcessor(QQmlApplicationEngine *_qmlengine, QObject *parent) : QObject(parent),
    qmlengine(_qmlengine)
{  
    QString _appdirpath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation).append("/%1").arg(APP_NAME);
    QDir _dir(_appdirpath);
    if(!_dir.exists())
        _dir.mkpath(_appdirpath);
    settings = new QSettings(_dir.absolutePath().append("/%1.ini").arg(APP_NAME),QSettings::IniFormat,this);

    qmlengine->rootContext()->setContextProperty("settings", this);

    connect(qmlengine->rootObjects().at(0),SIGNAL(frameCaptured(QString)),this,SLOT(postTask(QString)));
}

void QProcessor::postTask(const QString &_filename)
{
    QRecognitionTaskPoster *poster = new QRecognitionTaskPoster(apiurl(),_filename);
    connect(poster,SIGNAL(replyReady(QString)),qmlengine->rootObjects().at(0),SIGNAL(replyReady(QString)));
    connect(poster,SIGNAL(finished()),poster,SLOT(deleteLater()));
    poster->start();
}

QString QProcessor::apiurl() const
{
    return settings->value("apiurl","http://localhost:0").toString();
}

void QProcessor::setApiurl(const QString &_apiurl)
{
    if(_apiurl != apiurl()) {
        settings->setValue("apiurl",_apiurl);
        emit apiurlChanged(apiurl());
    }
}
