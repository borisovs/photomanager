#ifndef ZOOMBUTTON_H
#define ZOOMBUTTON_H

#include <QGraphicsObject>
#include <QtGui/QPixmap>
#include <QtCore/QRectF>

class QGraphicsSceneMouseEvent;

class ZoomButton : public QGraphicsObject
{
    Q_OBJECT
public:
    ZoomButton(QGraphicsObject *parent = 0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void clicked();
private:
    QRectF m_rect;
    QPixmap m_pixmap;
};

#endif // ZOOMBUTTON_H
