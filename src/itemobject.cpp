#include "itemobject.h"
#include <QtGui/QPainter>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QMessageBox>
#include <QtCore/QRectF>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtGui/QAction>
#include <QtGui/QGraphicsBlurEffect>
#include <QApplication>
#include <QtGui/QPixmap>
#include "zoombutton.h"
#include "photoview.h"

#include <QtDebug>

ItemObject::ItemObject( bool m_check, bool m_focus, QGraphicsItem * parent):
        QGraphicsObject(parent), m_checked(m_check), focusable(m_focus), m_cutted( false )
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    setFlags(QGraphicsItem::ItemIsFocusable);
    m_zoom = new ZoomButton(this);
    connect(m_zoom, SIGNAL(clicked()), this, SLOT(zoomMe()));
}

void ItemObject::setPixmap(QPixmap pix)
{
    m_pixmap = pix;
}

QPixmap ItemObject::pixmap()
{
    return m_pixmap;
}

void ItemObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if( !m_checked ){
        painter->drawPixmap(boundingRect(), m_pixmap, boundingRect());
    } else {
        painter->drawPixmap(boundingRect(), m_pixmap, boundingRect());
        painter->setPen(QPen(Qt::yellow, 2));
        painter->drawRoundedRect(boundingRect(), 0.5, 0.5);
        if ( m_cutted ){
            QPixmap temPix(":/icons/kadrir.png");
            painter->drawPixmap(boundingRect().topRight().x() - 32, boundingRect().topRight().y(), 32, 32,  temPix.scaledToWidth(32, Qt::SmoothTransformation));
        }
    }
    painter->drawText(boundingRect().adjusted(0, boundingRect().height() - painter->fontMetrics().height() ,10,10), m_filename);
}

QRectF ItemObject::boundingRect() const
{
    return QRectF(m_pixmap.rect());
}

void ItemObject::setMasterKey(int key)
{
    m_masterKey = key;
}

int ItemObject::masterKey()
{
    return m_masterKey;
}

void ItemObject::setChecked()
{
    QSqlQuery query(QSqlDatabase::database("memory"));
    QString sql = "UPDATE files SET flag = %1 WHERE id = %2;";
    
    if ( !m_checked ) {
        if (query.exec(sql.arg(1).arg(m_masterKey))){
            m_checked = true;
        }  else  {
            query.lastError().databaseText();
        }
    } else {
        if (query.exec(sql.arg(0).arg(m_masterKey))){
            m_checked = false;
        } else {
            query.lastError().databaseText();
        }
    }
    update();
    
}

void  ItemObject::checkIn()
{
    QSqlQuery query(QSqlDatabase::database("memory"));
    QString sql = "UPDATE files SET flag=%1 WHERE id=%2;";
    if ( query.exec(sql.arg(1).arg(m_masterKey))){
        if ( !m_checked ) {
            m_checked = true;
            update();
        }

    }
}

void ItemObject::setFileName(QString str)
{
    m_filename = str;
}

QString ItemObject::fileName(QString str)
{
    return m_filename;
}

void ItemObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //    emit(currentKey(m_masterKey));
    
}

void ItemObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    setChecked();
}

void ItemObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit(currentKey(m_masterKey));
}

void  ItemObject::zoomMe()
{
    setGraphicsEffect(new QGraphicsBlurEffect(this));
    QPixmap tmp(m_fullFileName);
    PhotoView dlg(tmp.scaledToWidth(1024, Qt::SmoothTransformation), m_masterKey);
    if ( dlg.exec() ){
        
    }
    graphicsEffect()->setEnabled(false);
    
}

void ItemObject::setFullFileName(QString tmp)
{
    m_fullFileName = tmp;
}

void ItemObject::setCutted(bool tmp)
{
    m_cutted = tmp;
    update();
}


