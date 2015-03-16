#ifndef PHOTOVIEW_H
#define PHOTOVIEW_H

#include <QtGui/QDialog>
#include <QtGui/QPixmap>

class QGraphicsPixmapItem;
class QGraphicsView;
class QGraphicsScene;
class QToolButton;
class QLineEdit;

class PhotoView : public QDialog
{
    Q_OBJECT
public:
    PhotoView(const QPixmap &pix, int key = -1, QWidget *parent = 0);
protected:
    void closeEvent(QCloseEvent *);
private slots:
    void rotatePicture();
    void setEye();
    void setRetush();
    void setText();
private:
    void readSettings();
    QPixmap m_pixmap;
    QGraphicsPixmapItem *m_item;
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    int masterKey;
    QToolButton *m_redEye;
    QToolButton *m_retush;
    QToolButton *m_text;
    QLineEdit *m_ledit;
};

#endif // PHOTOVIEW_H
