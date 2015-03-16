#ifndef ITEMOBJECT_H
#define ITEMOBJECT_H

#include <QGraphicsObject>

class ZoomButton;

class ItemObject : public QGraphicsObject
{
    Q_OBJECT
public:
    ItemObject(bool m_check = false, bool m_focus = false, QGraphicsItem * parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void setMasterKey(int);
    int masterKey();
    void setFileName(QString);
    void setFullFileName(QString);
    QString ItemObject::fileName(QString str);
    void setCutted(bool);
public slots:
    void setPixmap(QPixmap);
    QPixmap pixmap();
     void setChecked();
     void zoomMe();
     void checkIn();
protected:
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent *);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *);
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
signals:
    void currentKey(int);
private:
    QPixmap m_pixmap;
    int m_masterKey;
    bool m_checked;
    bool focusable;
    bool m_cutted;
    QString m_filename;
    QString m_fullFileName;
    ZoomButton *m_zoom;
};

#endif // ITEMOBJECT_H
