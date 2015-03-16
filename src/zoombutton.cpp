#include "zoombutton.h"
#include <QtGui/QPainter>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <QtDebug>

ZoomButton::ZoomButton(QGraphicsObject *parent)
    :QGraphicsObject(parent), m_rect(QRectF(0 , 0 , 32, 32))
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    m_pixmap = QPixmap(":/icons/zoom.png");
}


QRectF ZoomButton::boundingRect() const
{
    return m_rect;
}

void ZoomButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(boundingRect(), m_pixmap, boundingRect());
}


void ZoomButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    emit clicked();
}
