#include <QtGui/QPixmap>
#include <QtCore/QFileInfo>
#include "filemodel.h"

FileModel::FileModel(QObject *parent): QStandardItemModel(parent)
{
}


QVariant FileModel::data(const QModelIndex & index, int role) const
{
    QVariant result = QStandardItemModel::data(index, role);

    if (role == Qt::DecorationRole){
        if (!index.parent().isValid()){
            return QPixmap(":/icons/folder.png").scaledToWidth(32);
        } else {
            return QPixmap(":/icons/photo.png").scaledToWidth(16);
        }

    }
    if(role == Qt::DisplayRole )
        return QFileInfo(result.toString()).baseName();

//        if(role == Qt::DecorationRole){
//            if(index.column()==0)
//                return QPixmap(index.model()->data(index.model()->index(index.row(), 1)).toString()).scaledToWidth(64);
//        }

    //    if(role == Qt::DecorationRole){
//        if(index.column()==0){
//                if(QStandardItemModel::data(QStandardItemModel::index(index.row(), 3)).toInt() > 0)
//                return QPixmap(":/icons/flag.png");
//            }
//        }

//    if (role == Qt::CheckStateRole)
//        if (index.column() == 3)
//            return (QStandardItemModel::data(index).toInt() != 0) ?
//                    Qt::Checked : Qt::Unchecked;
//    else
//        return result;

    if (role == Qt::TextAlignmentRole)
        return int(Qt::AlignHCenter | Qt::AlignVCenter);

    return result;
}


//QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    if (orientation != Qt::Horizontal || role != Qt::DisplayRole )
//        return QVariant();
//
//
//    switch (section){
//    case 0:
//        return QString(tr("Num"));
//
//    case 1:
//        return QString(tr("File name"));
//    default:
//        return QVariant();
//    }
//
//    return QVariant();
//
//}




