#ifndef FILEMODEL_H
#define FILEMODEL_H


#include <QVariant>
#include <QModelIndex>
#include <QtGui/QStandardItemModel>

class FileModel : public QStandardItemModel
{
public:
    FileModel(QObject *parent=0);
    QVariant data(const QModelIndex &, int role = Qt::DisplayRole) const;
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};

#endif // FILEMODEL_H
