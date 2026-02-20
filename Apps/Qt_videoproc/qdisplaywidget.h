#ifndef QDISPLAYWIDGET_H
#define QDISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>

#include <opencv2/core.hpp>

class QDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QDisplayWidget(QWidget *parent = 0);

signals:

public slots:
    void updateImage(const QImage &_qimg);
    void updateImage(const cv::Mat &_cvmat);
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
