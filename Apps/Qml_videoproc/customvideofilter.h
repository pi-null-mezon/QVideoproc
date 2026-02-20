#ifndef CUSTOMVIDEOFILTER_H
#define CUSTOMVIDEOFILTER_H

#include <QAbstractVideoFilter>
#include <QVideoFilterRunnable>

class CustomFilter : public QAbstractVideoFilter {
    Q_OBJECT
public:
    QVideoFilterRunnable *createFilterRunnable() override;
};

class CustomVideoFilterRunnable : public QVideoFilterRunnable
{
public:
    CustomVideoFilterRunnable(QObject *_parent=nullptr);
    ~CustomVideoFilterRunnable();

    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);
};



#endif // CUSTOMVIDEOFILTER_H
