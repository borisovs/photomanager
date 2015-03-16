#include <QMessageBox>
#include <QIcon>
#include <QPixmap>
#include <QDir>
#include <QDomDocument>
#include <QtDebug>
#include "photomanager.h"
#include "viewerwidget.h"
#include "messagebox.h"
#include "ui_photomanager.h"

PhotoManager::PhotoManager(QWidget *parent)
        : QDialog(parent), ui(new Ui::PhotoManager)
{

    ui->setupUi(this);
    //connect(ui->pbPrint, SIGNAL(clicked()), this, SLOT(startPrint()));
    ui->lbCentral->setPixmap(QPixmap(":/icons/central.png"));

    ui->pbCf->setIcon(QPixmap(":/icons/CF.png"));
    ui->pbDvd->setIcon(QPixmap(":/icons/DVD_RW.png"));
    ui->pbFlash->setIcon(QPixmap(":/icons/USB-2.0.png"));
    ui->pbMm->setIcon(QPixmap(":/icons/MM.png"));
    ui->pbMs->setIcon(QPixmap(":/icons/MS.png"));
    ui->pbSd->setIcon(QPixmap(":/icons/SD.png"));
    parseXml();

    connect(ui->pbCf, SIGNAL(clicked()), this, SLOT(startPrint()));
    connect(ui->pbDvd, SIGNAL(clicked()), this, SLOT(startPrint()));
    connect(ui->pbFlash, SIGNAL(clicked()), this, SLOT(startPrint()));
    connect(ui->pbMm, SIGNAL(clicked()), this, SLOT(startPrint()));
    connect(ui->pbMs, SIGNAL(clicked()), this, SLOT(startPrint()));
    connect(ui->pbSd, SIGNAL(clicked()), this, SLOT(startPrint()));
//
//    setWindowState(Qt::WindowMaximized);
//    setWindowFlags(Qt::FramelessWindowHint);
}

PhotoManager::~PhotoManager()
{
    delete ui;
}


void PhotoManager::startPrint()
{

    QString path(sender()->property("path").toString());

    MessageBox msgDlg;
    msgDlg.setMsgPixmap(QPixmap(sender()->property("path_icon").toString()));
    msgDlg.setMsgText(tr("PhotoManager"), tr("Did you switch on ") +sender()->property("type").toString()+
                                             tr(" ?\nYou must insert flash before start printing!"));

    if(msgDlg.exec()==QDialog::Accepted){
        ViewerWidget dlg(path);
        if(dlg.exec()){

        }
    }

}


void PhotoManager::parseXml()
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

    QDomElement cf = rootElement.firstChildElement("cf");
    ui->pbCf->setProperty("path", cf.attribute("name"));
    ui->pbCf->setProperty("type", "CF card");
    ui->pbCf->setProperty("path_icon", (":/icons/CF.png"));
    //    m_cf = cf.attribute("name");

    QDomElement dvd = rootElement.firstChildElement("dvd");
    ui->pbDvd->setProperty("path", dvd.attribute("name"));
    ui->pbDvd->setProperty("type", "DVD disk");
    ui->pbDvd->setProperty("path_icon", (":/icons/DVD_RW.png"));
    //m_dvd = dvd.attribute("name");

    QDomElement flash = rootElement.firstChildElement("flash");
    ui->pbFlash->setProperty("path", flash.attribute("name"));
    ui->pbFlash->setProperty("type", "Flash card");
    ui->pbFlash->setProperty("path_icon", (":/icons/USB-2.0.png"));
    //m_flash = flash.attribute("name");

    QDomElement mm = rootElement.firstChildElement("mm");
    ui->pbMm->setProperty("path", mm.attribute("name"));
    ui->pbMm->setProperty("type", "MM  card");
    ui->pbMm->setProperty("path_icon", (":/icons/MM.png"));
    //m_mm = mm.attribute("name");

    QDomElement ms = rootElement.firstChildElement("ms");
    ui->pbMs->setProperty("path", ms.attribute("name"));
    ui->pbMs->setProperty("type", "MS card");
    ui->pbMs->setProperty("path_icon", (":/icons/MS.png"));
    //m_ms = ms.attribute("name");

    QDomElement sd = rootElement.firstChildElement("sd");
    ui->pbSd->setProperty("path", sd.attribute("name"));
    ui->pbSd->setProperty("type", "SD card");
    ui->pbSd->setProperty("path_icon", (":/icons/SD.png"));
    //m_sd = sd.attribute("name");
}
