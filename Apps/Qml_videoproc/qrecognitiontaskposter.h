#ifndef QRECOGNITIONTASKPOSTER_H
#define QRECOGNITIONTASKPOSTER_H

#include <QThread>

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
