#ifndef PHOTOMANAGER_H
#define PHOTOMANAGER_H

#include <QtGui/QDialog>

namespace Ui
{
    class PhotoManager;
}

class PhotoManager : public QDialog
{
    Q_OBJECT

public:
    PhotoManager(QWidget *parent = 0);
    ~PhotoManager();
private slots:
    void startPrint();
private:
    void parseXml();
    Ui::PhotoManager *ui;
//    QString m_cf;
//    QString m_dvd;
//    QString m_flash;
//    QString m_mm;
//    QString m_ms;
//    QString m_sd;

};

#endif // PHOTOMANAGER_H
