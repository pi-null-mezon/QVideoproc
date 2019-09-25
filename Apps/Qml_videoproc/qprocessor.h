#ifndef QPROCESSOR_H
#define QPROCESSOR_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QSettings>

class QProcessor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString apiurl READ apiurl WRITE setApiurl NOTIFY apiurlChanged)

public:
    explicit QProcessor(QQmlApplicationEngine *_qmlengine, QObject *parent = nullptr);

    QString apiurl() const;
    void setApiurl(const QString &_apiurl);

signals:
    void apiurlChanged(const QString& _apiurl);

public slots:
    void postTask(const QString &_filename);

private:
    QQmlApplicationEngine *qmlengine;
    QSettings *settings;

};

#endif // QPROCESSOR_H
