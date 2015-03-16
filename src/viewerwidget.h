#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include "ui_viewerwidget.h"
#include <QSqlDatabase>
#include <QModelIndex>
class FileModel;
class QSignalMapper;
class QGraphicsScene;

struct PhotoInfo{
    PhotoInfo(int m_id =0 , QString m_fn = QString(), int m_pc = 0, bool m_fl = false)
        :id(m_id), file_name(m_fn), photo_count(m_pc), flag(m_fl)
    {}
    int id;
    QString file_name;
    int photo_count;
    bool flag;
};

class ViewerWidget : public QDialog, private Ui::ViewerWidget {
    Q_OBJECT
public:
    ViewerWidget(const QString &, QWidget *parent = 0);

protected:
    void changeEvent(QEvent *e);
private slots:
    void confirmLogout();
    void addPhoto();
    void addPhotoForSelected();
    void removePhoto();
    void showNextPage();
    void createSignalMapper();
    void keyPress(const QString &);
    void doEnter();
    void doBackspace();
    void showPreviousPage();
    void itemSelected();
    void setOprationsVisible(int);
    void selectAll();
    void nextPhotosPage();
    void previousPhotosPage();
    void setData(int index, int records);
    void countPhotos(int);
    void setForDoc(bool);
    void setCuttAll(bool);
    void changeFormat();
signals:
    void clicked(const QString &);
private:
    void createFormats();
    void netCopy(const QString &);
    void createOrder();
    void setIcons();
    void createMemoryDb();
    void loadFiles(QString);
    void parseXml();
    void drawItems(int, int);
    void setPageInfo();
    void buildTreeModel();
    QString m_path;
    QString m_host;
    QString m_prefix;
    QString m_doc;
    int m_netmode;
    QSqlDatabase memory;
    FileModel *model;
    QModelIndex lastIndex;
    QSignalMapper *signalMapper;
    QList<QToolButton *> m_list;
    const QString m_flash;
    QGraphicsScene *m_scene;
    const int m_between;
    const int m_width;
    const int m_height;
    const int m_sceneWidth;
    QVector <PhotoInfo> photosVector;
    int max_count;
    int m_curPos;
    int m_curPage;
    int m_masterKey;
    bool m_forDoc;
    QMap<int, QString> m_format;
    int m_currentFormat;
    int m_countFormats;
};

#endif // VIEWERWIDGET_H
