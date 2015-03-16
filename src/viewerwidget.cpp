#include <QIcon>
#include <QMessageBox>
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlError>
#include <QDirIterator>
#include <QDomDocument>
#include <QDomElement>
#include <QDir>
#include <QFileInfoList>
#include <QImageReader>
#include <QStringList>
#include <QTableView>
#include <QItemSelectionModel>
#include <QScrollBar>
#include <QSignalMapper>
#include <QList>
#include <QToolButton>
#include <QFontMetrics>
#include <QHeaderView>
#include <QGraphicsScene>
#include <QtCore/QList>
#include <QtGui/QGraphicsItem>
#include <QtGui/QStandardItem>
#include <QtGui/QPaintEngine>
#include <QtCore/QDateTime>
#include <cmath>
#include <QtDebug>
#include "viewerwidget.h"
#include "messagebox.h"
#include "application.h"
#include "filemodel.h"
#include "itemobject.h"

ViewerWidget::ViewerWidget(const QString &flash, QWidget *parent) :
        m_flash(flash), QDialog(parent), m_between(10), m_width(192), m_height(150),
        m_sceneWidth(1024), max_count(20), m_curPos (0), m_forDoc(false), m_currentFormat(1)
{
    setupUi(this);
    //    m_scene=new QGraphicsScene(this);
    setIcons();
    parseXml();
    connect(pbLogout, SIGNAL(clicked()), this, SLOT(confirmLogout()));
    createFormats();
    createMemoryDb();
    //    connect(tvPreview, SIGNAL(activated(QModelIndex)), this, SLOT(setPrintStatus(const QModelIndex &)));
    connect(tbAdd, SIGNAL(clicked()), this, SLOT(addPhoto()));
    connect(tbRemove, SIGNAL(clicked()), this, SLOT(removePhoto()));
    connect(pbPrint, SIGNAL(clicked()), this, SLOT(showNextPage()));
    connect(pbBack, SIGNAL(clicked()), this, SLOT(showPreviousPage()));
    connect(stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(setOprationsVisible(int)));
    connect(pbSelect, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(pbDownPage, SIGNAL(clicked()), this, SLOT(previousPhotosPage()));
    connect(pbUpPage, SIGNAL(clicked()), this, SLOT(nextPhotosPage()));
    connect(pbCopy, SIGNAL(clicked()), this, SLOT(addPhotoForSelected()));
    connect(pbDocum, SIGNAL(toggled(bool)), this, SLOT(setForDoc(bool)));
    connect(pbCut,  SIGNAL(toggled(bool)), this, SLOT(setCuttAll(bool)));
    connect(pbFormat, SIGNAL(clicked()), this, SLOT (changeFormat()));
    createSignalMapper();
    setPageInfo();
    buildTreeModel();
    setWindowState(Qt::WindowMaximized);
    setWindowFlags(Qt::FramelessWindowHint);
}

void ViewerWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void ViewerWidget::setIcons()
{
    pbPrint->setIcon(QIcon(":/icons/go-next.png"));
    pbLogout->setIcon(QIcon(":/icons/system-suspend.png"));
    tbAdd->setIcon(QIcon(":/icons/contact-new.png"));
    tbRemove->setIcon(QIcon(":/icons/list-remove.png"));
    pbBack->setIcon(QIcon(":/icons/go-previous.png"));
    pbSelect->setIcon(QIcon(":/icons/select.png"));
    pbCopy->setIcon(QIcon(":/icons/addphoto.png"));
    pbDocum->setIcon(QIcon(":/icons/docum.png"));
    pbDownPage->setIcon(QIcon(":/icons/go-previous.png"));
    pbUpPage->setIcon(QIcon(":/icons/go-next.png"));
    pbCut->setIcon(QIcon(":/icons/kadrir.png"));
//    pbFormat->setIcon(QIcon(":/icons/photo.png"));
}

void ViewerWidget::confirmLogout()
{
    MessageBox msgDlg;
    msgDlg.setMsgPixmap(QPixmap(":/icons/system-suspend.png"));
    msgDlg.setMsgText(tr("PhotoManager"), tr("Do you want to cancel order?"));

    if(msgDlg.exec()==QDialog::Accepted){
        reject();
    }
}

void ViewerWidget::createMemoryDb()
{
    memory=QSqlDatabase::addDatabase("QSQLITE", "memory");
    memory.setDatabaseName(":memory:");
    if(!memory.open())
        qDebug()<<memory.lastError().databaseText();
    QSqlQuery query(memory);
    if(!query.exec("CREATE TABLE files(id INTEGER PRIMARY KEY AUTOINCREMENT, file_name VARCHAR(255) NOT NULL, photo_count INTEGER DEFAULT 1, flag INTEGER, cut INTEGER DEFAULT 0, format VARCHAR(55) NOT NULL, redeye INTEGER DEFAULT 0, retush INTEGER DEFAULT 0, hastext INTEGER DEFAULT 0, comment VARCHAR(255));"))
        qDebug()<<query.lastError().databaseText();

    loadFiles(m_flash);
}

void ViewerWidget::loadFiles(QString m_temppath)
{
    QDir dir(m_temppath);

    QStringList filters;
    filters.append("*.jpg");

    QSqlQuery query(memory);
    foreach (QString file, dir.entryList(filters, QDir::Files)){
        qDebug()<<QFileInfo(dir, file).canonicalFilePath();
        query.prepare("INSERT INTO files(file_name, flag, format) VALUES(:fn, 0, :fm);");
        query.bindValue(":fn", QFileInfo(dir, file).canonicalFilePath());
        query.bindValue(":fm", m_format[m_currentFormat]);

        if(!query.exec()){
            qDebug()<<query.executedQuery();
            qDebug()<< query.lastError().databaseText();
        }
    }

    foreach (QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        loadFiles(m_temppath + QDir::separator() + subDir);

    QSqlQuery m_count(memory);
    if (m_count.exec("SELECT COUNT(id) FROM files;")){
        m_count.next();
        if (m_count.value(0).toInt() < max_count){
            setData(0, m_count.value(0).toInt() );
        } else  setData(0, max_count);
    }


}


void ViewerWidget::parseXml()
{
    QDir appDir(qApp->applicationDirPath());
    appDir.cd("xml");

    QString xmlPath = appDir.path();
    QString fileName = xmlPath + QDir::separator()+"settings.xml";
    QFile xmlFile(fileName);
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Can't open XML for menu from file" << fileName;
        xmlFile.close();
    }

    QDomDocument doc;
    QString errorMsg;
    int errorLine = 0, errorColumn = 0;
    if (!doc.setContent(&xmlFile, &errorMsg, &errorLine, &errorColumn)) {
        qDebug() << "Parse error" << errorMsg << errorLine << errorColumn;
        xmlFile.close();
    }


    QDomElement rootElement=doc.documentElement();

    QDomElement path=rootElement.firstChildElement("path");
    m_path=path.attribute("name");

    QDomElement path_doc=rootElement.firstChildElement("doc");
    m_doc=path_doc.attribute("name");
    //    QDomElement flash=rootElement.firstChildElement("flash");
    //    m_flash=flash.attribute("name");

    QDomElement prefix=rootElement.firstChildElement("prefix");
    m_prefix=prefix.attribute("name");

    QDomElement network=rootElement.firstChildElement("network");
    m_netmode=network.attribute("name").toInt();

    QDomElement host=rootElement.firstChildElement("host");
    m_host=host.attribute("name");

}


void ViewerWidget::setData(int index, int records)
{
    QSqlQuery m_data(memory);

    if ( m_data.exec("SELECT id, file_name, photo_count, flag, cut FROM files;") ){
        qreal curX = m_between;
        qreal curY = m_between;
        //        delete m_scene;
        m_scene=new QGraphicsScene(this);
        qDebug()<<"HEAR"<<index << records;
        m_data.seek(index);
        for (int i = 0; i < records; ++i){

            ItemObject *item = new ItemObject(m_data.value(3).toBool());
            item->setPixmap(QPixmap(m_data.value(1).toString()).scaled(QSize(m_width, m_height), Qt::KeepAspectRatio , Qt::SmoothTransformation));
            item->setMasterKey(m_data.value(0).toInt());
            item->setFileName(QFileInfo(m_data.value(1).toString()).fileName());
            item->setFullFileName(m_data.value(1).toString());
            item->setCutted(m_data.value(4).toBool());
            m_scene->addItem(item);

            connect(item, SIGNAL(currentKey(int)), this, SLOT(countPhotos(int)));

            if ((curX + item->boundingRect().width() <= m_sceneWidth )){

                if( item->boundingRect().width() < m_width ){
                    curX = curX + (m_width - item->boundingRect().width())/2;
                    item->setPos(curX, curY);
                    curX = curX + item->boundingRect().width() + m_between + (m_width - item->boundingRect().width())/2;
                } else {
                    item->setPos(curX, curY);
                    curX = curX + item->boundingRect().width() + m_between;

                }

            } else {

                curX = m_between;
                curY = curY +  m_between + item->boundingRect().height();

                if( item->boundingRect().width() < m_width ){
                    curX = curX + (m_width - item->boundingRect().width())/2;
                    item->setPos(curX, curY);
                    curX = curX + item->boundingRect().width() + m_between + (m_width - item->boundingRect().width())/2;
                } else {
                    item->setPos(curX, curY);
                    curX = curX + item->boundingRect().width() + m_between;

                }

            }

            m_data.next();

        }



        graphicsView->setScene(m_scene);
        m_curPos = index + records;
    }

}


void ViewerWidget::addPhoto()
{
    ItemObject *tmp = static_cast<ItemObject *> (m_scene->focusItem());
    int key = tmp->masterKey();
    //    QModelIndex index = lastIndex;
    QSqlQuery query(memory), status(memory);

    status.exec(QString("SELECT photo_count FROM files WHERE id = %1;").arg(key));
    if (status.next()){
        int m_count=status.value(0).toInt();
        if(!query.exec(QString("UPDATE files SET photo_count = %1 WHERE id = %2;").arg(++m_count).arg(key)))
            qDebug() << query.lastError().databaseText();
        countPhotos(key);

    }
}


void  ViewerWidget::removePhoto()
{

    ItemObject *tmp = static_cast<ItemObject *> (m_scene->focusItem());
    int key = tmp->masterKey();

    QSqlQuery query(memory), status(memory);
    status.exec(QString("SELECT photo_count FROM files WHERE id = %1;").arg(key));
    if (status.next()){
        int m_count=status.value(0).toInt();
        if(m_count > 1){
            if(!query.exec(QString("UPDATE files SET photo_count = %1 WHERE id = %2;").arg(--m_count).arg(key)))
                qDebug() << query.lastError().databaseText();

            countPhotos(key);

        }
    }
}


void ViewerWidget::showNextPage()
{
    int m_next = stackedWidget->currentIndex();
    if(m_next == 1){
        MessageBox msgDlg;
        msgDlg.setMsgPixmap(QPixmap(":/icons/media-flash-sd-mmc.png"));
        msgDlg.setMsgText(tr("PhotoManager"), tr("Do you want to make order?"));

        if(msgDlg.exec()==QDialog::Accepted){
            createOrder();
        }

    } else {
        stackedWidget->setCurrentIndex(++m_next);
    }
}

void ViewerWidget::createSignalMapper()
{
    m_list = findChildren<QToolButton *>();

    signalMapper=new QSignalMapper(this);
    QList<QToolButton *>::iterator i = m_list.begin();

    while (i != m_list.end()) {
        if((*i)->objectName()==pbLogout->objectName() || (*i)->objectName()==pbPrint->objectName()){
            //  connect((*i), SIGNAL(clicked()), this, SIGNAL(enterPressed()));
        } else if((*i)->objectName()== tbBackspace->objectName()){
            connect((*i), SIGNAL(clicked()), this, SLOT(doBackspace()));
        } else if((*i)->objectName() == tbEnter->objectName()){
            connect((*i), SIGNAL(clicked()), this, SLOT(doEnter()));

        }else {
            connect((*i), SIGNAL(clicked()), signalMapper, SLOT(map()));
            signalMapper->setMapping((*i), (*i)->text());
        }
        qDebug()<< "button name"<<  (*i)->text();
        ++i;
    }
    connect(signalMapper, SIGNAL(mapped(const QString &)),
            this, SIGNAL(clicked(const QString &)));
    connect(this, SIGNAL(clicked(const QString &)), this, SLOT(keyPress(QString)));

}

void ViewerWidget::keyPress(const QString &str)
{
    if (leName->hasFocus()){
        leName->setText(leName->text() + str);
    } else if (leAddress->hasFocus()){
        leAddress->setText(leAddress->text() + str);
    } else if (leNumber->hasFocus()){
        leNumber->setText(leNumber->text() + str);
    }
}


void ViewerWidget::doEnter()
{
    qDebug() << "Enter";
}


void ViewerWidget::doBackspace()
{
    if (leName->hasFocus()){
        leName->backspace();
    } else if (leAddress->hasFocus()){
        leAddress->backspace();
    } else if (leNumber->hasFocus()){
        leNumber->backspace();
    }
}

void ViewerWidget::showPreviousPage()
{
    int m_next = stackedWidget->currentIndex();
    stackedWidget->setCurrentIndex(--m_next);
}

void ViewerWidget::createOrder()
{
    QSqlQuery m_testOrder(memory);
    if (m_testOrder.exec("SELECT COUNT(id) FROM files WHERE flag=1;")){
        m_testOrder.next();
        if ( m_testOrder.value(0).toInt() > 0 ){
            QDir dir;

            if(m_forDoc)
                m_path = m_doc;

            if (!dir.exists(m_path)){
                MessageBox msgDlg;
                msgDlg.setMsgPixmap(QPixmap(":/icons/system-suspend.png"));
                msgDlg.setMsgText(tr("PhotoManager"), tr("Destination folder doesn't exists!"));
                msgDlg.exec();
                reject();
            }

            dir.cd(m_path);

            QSqlQuery query(memory), insert, last;
            insert.prepare("INSERT INTO orders(client_name, client_address, comment) VALUES (:cn, :ca, :co);");
            insert.bindValue(":cn", leName->text().isEmpty() ? QString("default") : leName->text());
            insert.bindValue(":ca", leAddress->text().isEmpty() ? QString("default") : leAddress->text());
            insert.bindValue(":co", leNumber->text().isEmpty() ? QString("default") : leNumber->text());
            if ( insert.exec() ){
                last.exec("SELECT id, comment FROM orders");
                last.last();

                dir.mkdir(last.value(0).toString()+QString("_")+last.value(1).toString());
                dir.cd(dir.canonicalPath()+dir.separator()+last.value(0).toString()+QString("_")+last.value(1).toString());

                if ( !query.exec( "SELECT file_name, photo_count, format, redeye, retush, hastext, comment FROM files WHERE flag = 1;"))
                    qDebug() << query.lastError().databaseText();

                while( query.next() ){
                    QSqlQuery data;
                    data.prepare("INSERT INTO orderdata(orderID,  name_of_file, photo_count, format, redeye, retush, hastext, comment) VALUES(:oid, :fn, :pc, :fm, :reye, :ret, :ht, :comm);");
                    data.bindValue(":oid", last.value(0).toInt());
                    data.bindValue(":fn", QFileInfo(query.value(0).toString()).fileName());
                    data.bindValue(":pc", query.value(1).toInt());
                    data.bindValue(":fm", query.value(2).toString());
                    data.bindValue(":reye", query.value(3).toString());
                    data.bindValue(":ret", query.value(4).toString());
                    data.bindValue(":ht", query.value(5).toString());
                    data.bindValue(":comm", query.value(6).toString());

                    if ( !data.exec() )
                        qDebug()<<data.lastError().databaseText();

                    QFile::copy(query.value(0).toString(), dir.canonicalPath()+dir.separator()+query.value(1).toString()+"_copies_" + query.value(2).toString()+"_" + QFileInfo(query.value(0).toString()).fileName());
                }
                if(m_netmode == 1) netCopy(dir.canonicalPath());
            }



            accept();
        } else {
            MessageBox msgDlg;
            msgDlg.setMsgPixmap( QPixmap(":/icons/system-suspend.png") );
            msgDlg.setMsgText( tr("PhotoManager") , tr("You must select photos!") );

            if(msgDlg.exec()){
                int m_tmp = stackedWidget->currentIndex();
                stackedWidget->setCurrentIndex( --m_tmp );
            }
        }
    }
}

void ViewerWidget::netCopy(const QString &from)
{
    QFileInfo fileinfo(from);

    QDir dest(m_host);
    dest.mkdir(QString(m_prefix+"_"+fileinfo.baseName()));
    qDebug()<<dest.canonicalPath();

    dest.cd(m_prefix+"_"+fileinfo.baseName());
    qDebug()<< dest.canonicalPath();

    QDir source(from);


    QStringList filters;
    filters.append("*.jpg");

    foreach (QString file, source.entryList(filters, QDir::Files)){
        QFile::copy(QFileInfo(source, file).canonicalFilePath(), dest.canonicalPath()+QDir::separator()+QFileInfo(source, file).fileName());
    }
}

void ViewerWidget::itemSelected()
{
    //
}

void ViewerWidget::setOprationsVisible(int page)
{
    if (page == 0){
        pbSelect->setVisible(true);
        pbCopy->setVisible(true);
        pbDocum->setVisible(true);
        pbCut->setVisible(true);
        pbFormat->setVisible(true);
        pb3->setVisible(true);
    }  else {

        pbSelect->setVisible(false);
        pbCopy->setVisible(false);
        pbDocum->setVisible(false);
        pbCut->setVisible(false);
        pbFormat->setVisible(false);
        pb3->setVisible(false);
    }

}

void ViewerWidget::selectAll()
{
    QSqlQuery query(memory);
    if (!query.exec( "UPDATE files SET flag = 1;"))
        qDebug()<< query.lastError().databaseText();

    QList<QGraphicsItem *> list(m_scene->items());

    QList<QGraphicsItem *>::iterator it = list.begin();
    QList<QGraphicsItem *>::iterator end = list.end();
    while ( it != end ){
        ItemObject *tmp = static_cast<ItemObject *> (*(it));
        tmp->checkIn();
        ++it;
    }
}

void ViewerWidget::nextPhotosPage()
{
    if (m_curPage < lbPages->text().toInt()){
        QSqlQuery query(memory);
        if (query.exec("SELECT COUNT(id) FROM files;")){
            query.next();

            qDebug() <<"ON NEXT PAGE GUNCTION"<< query.value(0).toInt() << m_curPos ;

            if ((query.value(0).toInt() - m_curPos) < max_count)
                setData(m_curPos, (query.value(0).toInt() - m_curPos));
            else setData(m_curPos, max_count);
        } else qDebug()<< query.lastError().databaseText();
        lbCurPage->setText(QString::number(++m_curPage));
    }


}

void ViewerWidget::previousPhotosPage()
{
    if (m_curPage > 1){
        QSqlQuery query(memory);
        if (query.exec("SELECT COUNT(id) FROM files;")){
            m_scene->clear();

            query.next();
            if((m_curPos - 2 * max_count) >= 0)
                setData(m_curPos - 2 * max_count, max_count);
            else setData( 0 , max_count);
        } else qDebug()<< query.lastError().databaseText();
        lbCurPage->setText(QString::number(--m_curPage));
    }
}

void ViewerWidget::setPageInfo()
{
    QSqlQuery query(memory);
    if (query.exec("SELECT COUNT(id) FROM files;")){
        query.next();
        m_curPage = 1;
        lbCurPage->setText(QString::number(m_curPage));
        lbPages->setText(QString::number( ceil(query.value(0).toInt() / 20.0)));
    }

}

void ViewerWidget::addPhotoForSelected()
{
    QSqlQuery query(memory);
    if (!query.exec("UPDATE files SET photo_count=photo_count+1 WHERE flag=1;")){
        qDebug()<< query.lastError().databaseText();
    }
    countPhotos(m_masterKey);

}

void ViewerWidget::countPhotos(int key)
{
    m_masterKey = key;

    QSqlQuery status(memory);

    status.exec(QString("SELECT photo_count, file_name FROM files WHERE id = %1;").arg(m_masterKey));
    if (status.next()){
        int m_count=status.value(0).toInt();
        lbCount->setText(QString::number(m_count));

        QPixmap m_pixmap(status.value(1).toString());
        lbWidth->setText(QString::number(m_pixmap.width()));
        lbHeight->setText(QString::number(m_pixmap.height()));
        lbDepht->setText(QString::number(m_pixmap.defaultDepth()));
        lbHResolution->setText(QString::number(m_pixmap.logicalDpiX()));
        lbVResolution->setText(QString::number(m_pixmap.logicalDpiY()));

        QFileInfo info(status.value(1).toString());
        lbLastModified->setText(info.lastModified().toLocalTime().toString());
    }

}

void ViewerWidget::setForDoc(bool ok)
{
    m_forDoc = ok;
}

void ViewerWidget::buildTreeModel()
{
    model = new FileModel(this);
    QStandardItem *parentItem = model->invisibleRootItem();

    QSqlQuery m_data(memory), m_count(memory);

    if (m_count.exec("SELECT COUNT(id) FROM files;") ){

        m_count.next();
        int m_folders = ceil(m_count.value(0).toInt() / 20.0);

        if(m_data.exec("SELECT file_name FROM files;") ){
            for(int i = 0 ; i < m_folders; ++i){
                int m_page = i;
                QStandardItem *item = new QStandardItem(QString(tr("Page %0")).arg(++m_page));
                parentItem->appendRow(item);
                for (int j = 0; j < 20; j++){
                    if(m_data.next()){
                        QStandardItem *fileitem = new QStandardItem(m_data.value(0).toString());
                        item->appendRow(fileitem);
                    }
                }
            }

        }
    }

    treeView->setModel(model);
}

void ViewerWidget::setCuttAll(bool is_cut)
{
    QSqlQuery query(memory);
    if (!query.exec( QString("UPDATE files SET cut=%1").arg(is_cut)))
        qDebug()<< query.lastError().databaseText();

    QList<QGraphicsItem *> list(m_scene->items());

    QList<QGraphicsItem *>::iterator it = list.begin();
    QList<QGraphicsItem *>::iterator end = list.end();
    while ( it != end ){
        ItemObject *tmp = static_cast<ItemObject *> (*(it));
        tmp->setCutted(is_cut);
        ++it;
    }
}

void ViewerWidget::changeFormat()
{
    if( m_currentFormat < m_format.count() )
        pbFormat->setText(m_format[++m_currentFormat]);
    else {
        m_currentFormat = 1;
        pbFormat->setText(m_format[m_currentFormat]);
    }

    QSqlQuery query(memory);
    if (!query.exec(QString("UPDATE files SET format='%1';").arg(m_format[m_currentFormat]))){
        qDebug()<< query.lastError().databaseText();
    }

}

void ViewerWidget::createFormats()
{
    QSqlQuery format;
    if (format.exec( "SELECT name_of_format FROM formats;" )){
        int m_tmp = 0;
        while ( format.next() ){
            m_format.insert( ++m_tmp, format.value(0).toString() );
            qDebug() << m_tmp <<format.value(0).toString() ;
        }
        pbFormat->setText(m_format[m_currentFormat]);


    } else {
        qDebug()<<format.lastError().databaseText();
    }
}

