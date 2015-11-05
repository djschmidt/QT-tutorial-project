#ifndef SPLASHITEM_H
#define SPLASHITEM_H
#include <QGraphicsObject>


class SplashItem: public QGraphicsObject
{
    Q_OBJECT
public:
    explicit SplashItem(QGraphicsItem *parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    QString text;
};

#endif // SPLASHITEM_H
