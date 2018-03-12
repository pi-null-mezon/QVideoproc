#ifndef QDISPLAYWIDGET_H
#define QDISPLAYWIDGET_H

#include <QOpenGLWidget>
#include <QPainter>

#include <opencv2/core.hpp>

class QDisplayWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit QDisplayWidget(QWidget *parent = 0);

signals:

public slots:
    void updateImage(const QImage &_qimg);
    void updateImage(const cv::Mat &_cvmat); // will try to guess QImage::Format, use overloaded function if you now appropriate QImage::Format instead
    void updateImage(const cv::Mat &_cvmat, QImage::Format _format);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QRect __getRectInsideWidget(const QRect &_rect);
    void __updateFPS();
    void __drawFPS(QPainter &painter, const QRect &_rect);

    QImage m_qimg;
    cv::Mat m_cvmat;

    double m_tick;
    double m_fps;
};

#endif // QDISPLAYWIDGET_H
