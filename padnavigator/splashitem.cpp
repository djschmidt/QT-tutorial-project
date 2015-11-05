#include "splashitem.h"

#include<QtGui/QtGui>

SplashItem::SplashItem(QGraphicsItem *parent)
    :QGraphicsObject(parent)
{
    text = tr("Welcome to the Pad Navigator Example. You can use the"
              " keyboard arrows to navigate the icons, and press enter"
              " to activate an item. Press any key to begin."
              );
}

QRectF SplashItem::boundingRect() const
{
    return QRectF(0, 0, 400, 175);
}

void SplashItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QColor(245, 245, 255, 220));
    painter->setClipRect(boundingRect());
    painter->drawRoundRect(3, -100 + 3, 400 - 6, 250 - 6);

    QRectF textRect = boundingRect().adjusted(10, 10, -10, -10);
    int flags = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap;

    QFont font;
    font.setPixelSize(18);
    painter->setPen(Qt::black);
    painter->setFont(font);
    painter->drawText(textRect, flags, text);

}
