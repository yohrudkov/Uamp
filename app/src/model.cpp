#include "model.h"
#include "songlist.h"
#include <QDebug>

Model::Model(QWidget *parent) : QStandardItemModel(parent), m_parent(parent) {
    setHorizontalHeaderLabels(QStringList() << tr("Audio Track") << tr("File Path"));
}

Qt::DropActions Model::supportedDropActions() const {
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const {
    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);
    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList Model::mimeTypes() const {
    QStringList types;
    types << "application/vnd.text.list";
    return types;
}

QMimeData *Model::mimeData(const QModelIndexList &indexes) const {
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QString text = data(index, Qt::DisplayRole).toString();
            stream << text;
        }
    }
    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}

bool Model::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    Q_UNUSED(column);
    if (action == Qt::IgnoreAction)
        return true;
    if (!data->hasFormat("application/vnd.text.list"))
        return false;
    int beginRow;
    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());
    QByteArray encodedData = data->data("application/vnd.text.list");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QStringList newItems;
    while (!stream.atEnd()) {
        QString text;
        stream >> text;
        newItems << text;
    }
    auto par = qobject_cast<Table *>(m_parent);
    int current = par->getCurrent();
    if (par->getIndex() < current && current < beginRow)
        current--;
    else if (par->getIndex() > current && current >= beginRow)
        current++;
    else if (par->getIndex() == current)
        current = par->getIndex() < beginRow ? beginRow - 1 : beginRow;
    insertRows(beginRow, 1, QModelIndex());
    for (int i = 0; i < newItems.size(); i++) {
        QModelIndex idx = index(beginRow, i, QModelIndex());
        setData(idx, newItems[i]);
    }
    par->moveLine(par->getIndex() < beginRow ? beginRow - 1 : beginRow, current);
    par->setCurrent(current);
    return true;
}
