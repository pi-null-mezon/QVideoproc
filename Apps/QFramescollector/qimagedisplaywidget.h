#ifndef QIMAGEDISPLAYWIDGET_H
#define QIMAGEDISPLAYWIDGET_H

#include <QWidget>

class QImageDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QImageDisplayWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *_event) override;

public slots:
    void updateImage(const QImage &_qimage);

private:
    QRect __resizeRect(const QRect &_rect);

private:
    QImage qimage;
};

#endif // QIMAGEDISPLAYWIDGET_H
