#ifndef QRECOGNITIONTASKPOSTER_H
#define QRECOGNITIONTASKPOSTER_H

#include <QThread>

class QProxyObject : public QObject
{
    Q_OBJECT
public:
    explicit QProxyObject(QObject *_parent=nullptr);
    QString getApiurl() const;
    void setApiurl(const QString &value);

signals:
    void replyReady(const QString &_reply);
public slots:
    void postTask(const QString &_filename);

private:
    QString apiurl;
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
