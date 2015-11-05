#include "flippablepad.h"

#include <QtGui/QtGui>



static QRectF boundsFromSize(const QSize &size)
{
    return QRectF((-size.width() / 2.0) * 150, (-size.height() / 2.0) * 150, size.width() * 150, size.height() * 150);
}


static QPointF posForLocation(int column, int row, const QSize &size)
{
    return QPointF(column * 150, row * 150)
            - QPointF((size.width() - 1) * 75,(size.height() - 1) * 75);
}

FlippablePad::FlippablePad(const QSize &size, QGraphicsItem *parent)
    :RoundRectItem(boundsFromSize(size), QColor(266, 255, 92, 64), parent)
{
    int numIcons = size.width() * size.height();
    QList<QPixmap> pixmaps;
    QDirIterator it(":/images", QStringList() << "*.png");
    while (it.hasNext() && pixmaps.size() < numIcons)
        pixmaps << it.next();

    const QRectF iconRect(-54, -54, 108, 108);
    const QColor iconColor(214, 240, 110, 128);
    iconGrid.resize(size.height());
    int n=0;

    for (int y=0; y < size.height(); ++y)
    {
        iconGrid[y].resize(size.width());
        for (int x = 0; x < size.width(); ++x)
        {
            RoundRectItem *rect = new RoundRectItem(iconRect, iconColor, this);
            rect->setZValue(1);
            rect->setPos(posForLocation(x, y, size));
            rect->setPixmap(pixmaps.at(n++ % pixmaps.size()));
            iconGrid[y][x] = rect;
        }
    }
}

RoundRectItem* FlippablePad::iconAt(int column, int row) const
{
    return iconGrid[row][column];
}


