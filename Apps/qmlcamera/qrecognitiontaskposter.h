#ifndef QRECOGNITIONTASKPOSTER_H
#define QRECOGNITIONTASKPOSTER_H

#include <QThread>
#include <QSettings>

class QProxyObject : public QObject
{
    Q_OBJECT
public:
    explicit QProxyObject(QObject *_parent=nullptr);
    QString getApiurl() const;    
    void    setSettings(QSettings *value);

signals:
    void replyReady(const QString &_reply);

public slots:
    void postTask(const QString &_filename);
    void setApiurl(const QString &value);

private:
    QString apiurl;
    QSettings *settings;
};

class QRecognitionTaskPoster : public QThread
{
    Q_OBJECT
public:
    explicit QRecognitionTaskPoster(const QString &_apiurl, const QString &_imgfilename, QObject *parent=nullptr);
signals:
    void replyReady(const QString &_reply);
protected:
    void run() override;
private:
    QString apiurl, imgfilename;
};

#endif // QRECOGNITIONTASKPOSTER_H
