#include "photoview.h"
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QBoxLayout>
#include <QtGui/QToolButton>
#include <QtGui/QIcon>
#include <QtGui/QLineEdit>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QtDebug>

PhotoView::PhotoView(const QPixmap &pix,  int key, QWidget *parent)
    :QDialog(parent), masterKey(key)
{
    setWindowFlags(Qt::SplashScreen);
    m_pixmap = pix;

    QHBoxLayout *horLayout = new QHBoxLayout();
    QVBoxLayout *vertLayout = new QVBoxLayout();

    m_redEye = new QToolButton(this);
    m_redEye->setIconSize(QSize(64, 64));
    m_redEye->setIcon(QIcon(":/icons/eye.jpg"));
    m_redEye->setCheckable(true);
    connect(m_redEye, SIGNAL(clicked()), this, SLOT(setEye()));

    m_retush = new QToolButton(this);
    m_retush->setIconSize(QSize(64, 64));
    m_retush->setIcon(QIcon(":/icons/Retush.png"));
    m_retush->setCheckable(true);
    connect(m_retush, SIGNAL(clicked()), this, SLOT(setRetush()));

    m_text = new QToolButton(this);
    m_text->setIconSize(QSize(64, 64));
    m_text->setIcon(QIcon(":/icons/text.png"));
    connect(m_text, SIGNAL(clicked()), this, SLOT(setText()));
    m_text->setCheckable(true);

    m_ledit = new QLineEdit(this);
    m_ledit->setVisible(false);

    connect(m_text, SIGNAL(clicked(bool)), m_ledit, SLOT(setVisible(bool)));

    QToolButton *m_rotate = new QToolButton(this);
    m_rotate->setIconSize(QSize(64, 64));
    m_rotate->setIcon(QIcon(":/icons/rotate.png"));
    connect(m_rotate, SIGNAL(clicked()), this, SLOT(rotatePicture()));

    QToolButton *m_close = new QToolButton(this);
    m_close->setIconSize(QSize(64, 64));
    m_close->setIcon(QIcon(":/icons/system-suspend.png"));
    connect(m_close, SIGNAL(clicked()), this, SLOT(close()));


    horLayout->addWidget(m_redEye);
    horLayout->addWidget(m_retush);
    horLayout->addWidget(m_text);
    horLayout->addWidget(m_ledit);
    horLayout->addStretch();
    horLayout->addWidget(m_rotate);
    horLayout->addWidget(m_close);

    m_view = new QGraphicsView();
    m_scene = new QGraphicsScene(m_view);
    m_view->setScene(m_scene);

    m_item = new QGraphicsPixmapItem();
    m_item->setPixmap(m_pixmap);
    m_scene->addItem(m_item);

    vertLayout->addLayout(horLayout);
    vertLayout->addWidget(m_view);

    setWindowState(Qt::WindowMaximized);
    setWindowFlags(Qt::FramelessWindowHint);

    setLayout(vertLayout);

    readSettings();

}

void PhotoView::rotatePicture()
{
    m_view->rotate(90);
}


void PhotoView::setEye()
{
    QSqlQuery query(QSqlDatabase::database("memory"));
    int i = m_redEye->isChecked() ? 1 : 0;
    if ( !query.exec(QString("UPDATE files SET redeye=%1 WHERE id=%2;").arg(i).arg(masterKey)) ){
        qDebug()<< query.lastError().databaseText();
    }
}

void PhotoView::setRetush()
{
    QSqlQuery query(QSqlDatabase::database("memory"));
    int i = m_retush->isChecked() ? 1 : 0;
    if ( !query.exec(QString("UPDATE files SET retush=%1 WHERE id=%2;").arg(i).arg(masterKey)) ){
        qDebug()<< query.lastError().databaseText();
    }
}

void PhotoView::setText()
{
    QSqlQuery query(QSqlDatabase::database("memory"));
    int i = m_text->isChecked() ? 1 : 0;
    QString text = m_ledit->text().isEmpty() ? QString() : m_ledit->text();
    if ( !query.exec(QString("UPDATE files SET hastext=%1 WHERE id=%2;").arg(i).arg(masterKey)) ){
        qDebug()<< query.lastError().databaseText();
    }
}

void PhotoView::readSettings()
{
    QSqlQuery query(QSqlDatabase::database("memory"));
    if ( !query.exec(QString("SELECT redeye, retush, hastext, comment FROM files WHERE id=%1").arg(masterKey)) ){
        qDebug()<< query.lastError().databaseText();
    } else {
        query.next();
        m_redEye->setChecked(query.value(0).toInt());
        m_retush->setChecked(query.value(1).toInt());
        m_text->setChecked(query.value(2).toInt());
        m_ledit->setText(query.value(3).toString());
        m_ledit->setVisible(query.value(2).toInt());
    }

}

void PhotoView::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    if (m_text->isChecked()){
        QSqlQuery query(QSqlDatabase::database("memory"));
        QString text = m_ledit->text().isEmpty() ? QString() : m_ledit->text();
        if ( !query.exec(QString("UPDATE files SET comment='%1' WHERE id=%2;").arg(text).arg(masterKey)) ){
            qDebug()<< query.lastError().databaseText();
        }
    }
}
