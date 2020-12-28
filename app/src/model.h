#ifndef MODEL_H
#define MODEL_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMimeData>

class Table;

class Model : public QStandardItemModel {
    Q_OBJECT
public:
    explicit Model(QWidget *parent = nullptr);

    Qt::DropActions supportedDropActions() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data,
        Qt::DropAction action, int row, int column, const QModelIndex &parent);

private:
    QWidget *m_parent;
};

#endif // MODEL_H
