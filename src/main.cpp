#include <QApplication>
#include <QFile>
#include <QByteArray>
#include <QResource>
#include <QIcon>
#include <QDir>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QtDebug>
#include "photomanager.h"

bool connect();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (connect()){
    QResource::registerResource(QApplication::applicationDirPath() +QDir::separator()+ "resources.rcc");
    QFile file(":/css/style.css");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        a.setStyleSheet(data);
    } else {
        qDebug() << "Can't open stylesheet";
    }
    a.setWindowIcon(QIcon(":/icons/camera-photo.png"));
    PhotoManager w;

    qDebug()<<QApplication::applicationDirPath() + "/resources.rcc";
    w.show();
    return a.exec();
} else return -1;
}



bool connect()
{
  QDir dir(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
  if (dir.mkpath(".pm")){
        dir.cd(".pm");

        QFile db(dir.path()+QDir::separator()+"pm.db");
        qDebug()<<db.fileName();

    QSqlDatabase base=QSqlDatabase::addDatabase("QSQLITE");
    base.setDatabaseName(db.fileName());

    if(!base.open()){
        QMessageBox::critical(0, QObject::tr("PhotoManager"), base.lastError().databaseText());
        return false;
    }

    QSqlQuery query;
    if ( !query.exec("CREATE TABLE IF NOT EXISTS orders (id INTEGER PRIMARY KEY AUTOINCREMENT, client_name VARCHAR(255) NOT NULL, client_address VARCHAR(255) NOT NULL, comment VARCHAR(255));"))
        qDebug() << "ORDERS ERROR" << query.lastError().databaseText();
    query.clear();
    if ( !query.exec("CREATE TABLE IF NOT EXISTS orderdata (id INTEGER PRIMARY KEY AUTOINCREMENT, orderID INTEGER, name_of_file VARCHAR(255) NOT NULL, photo_count INTEGER DEFAULT 1, format VARCHAR(55) NOT NULL, redeye INTEGER DEFAULT 0, retush INTEGER DEFAULT 0, hastext INTEGER DEFAULT 0, comment VARCHAR(255));"))
        qDebug() << "ORDERSDATA ERROR" <<query.lastError().databaseText();
    query.clear();
    if ( !query.exec("CREATE TABLE IF NOT EXISTS formats (id INTEGER PRIMARY KEY AUTOINCREMENT, name_of_format VARCHAR(55) NOT NULL);"))
        qDebug() << "FORMATS ERROR" <<query.lastError().databaseText();
    //******************
    query.clear();
    query.exec("SELECT COUNT(id) FROM formats");
    query.next();
    if ( query.value(0).toInt() > 0){

    }else {
        query.clear();
        query.exec("INSERT INTO formats(name_of_format) VALUES( '10x15' );");

        query.clear();
        query.exec("INSERT INTO formats(name_of_format) VALUES( '15x20' );");

        query.clear();
        query.exec("INSERT INTO formats(name_of_format) VALUES( '20x30' );");

        query.clear();
        query.exec("INSERT INTO formats(name_of_format) VALUES( '30x40' );");

        query.clear();
        query.exec("INSERT INTO formats(name_of_format) VALUES( 'custom' );");
    }




    //******************

}else {
    qDebug()<<"Can't create application directory!";
    return false;
}
    return true;
}
