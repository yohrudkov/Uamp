#ifndef TABLE_H
#define TABLE_H

#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTableView>
#include <QMouseEvent>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QHeaderView>
#include <QAbstractItemModel>
#include <QLayout>
#include <QDebug>
#include <QStringListModel>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMenu>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include "model.h"

class Table : public QTableView {
    Q_OBJECT
public:
    explicit Table(QWidget *parent = nullptr);
    ~Table();

    void forFiles(QString path);
    void forDirs(QString path);
    void moveLine(int to, int current);
    int getCurrent();
    void setCurrent(int index);
    int getIndex();
    Model *getModel();
    void setMyModel(Model *model);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    int m_index;
    int m_current;
    Model *m_model;
    QWidget *m_parent;

private slots:
    void slotCustomMenuRequested(QPoint position);
};

#endif // TABLE_H
