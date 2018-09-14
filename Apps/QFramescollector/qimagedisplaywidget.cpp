#include "qimagedisplaywidget.h"

#include <QPaintEvent>
#include <QPainter>

QImageDisplayWidget::QImageDisplayWidget(QWidget *parent) : QWidget(parent)
{

}

void QImageDisplayWidget::paintEvent(QPaintEvent *_event)
{
    QPainter _painter(this);

    if(!qimage.isNull()) {
        QRect _qimagerect = __resizeRect(qimage.rect());
        _painter.drawImage(_qimagerect,qimage);
    }
    _event->accept();
}

void QImageDisplayWidget::updateImage(const QImage &_qimage)
{
    qimage = _qimage;
    update();
}

QRect QImageDisplayWidget::__resizeRect(const QRect &_rect)
{
    QRect _outrect = rect();
    float w2h = static_cast<float>(_rect.width())/static_cast<float>(_rect.height());
    if( w2h > static_cast<float>(rect().width())/static_cast<float>(rect().height()) ) {
       _outrect.setWidth( rect().width() );
       _outrect.setHeight( static_cast<int>(rect().width() / w2h) );
       _outrect.moveTop( static_cast<int>((rect().height() - _outrect.height()) / 2.0f));
    } else {
        _outrect.setHeight( rect().height() );
        _outrect.setWidth( static_cast<int>(rect().height() * w2h));
        _outrect.moveLeft( static_cast<int>((rect().width() - _outrect.width()) / 2.0f));
    }
    return _outrect;
}
