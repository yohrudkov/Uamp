#ifndef ALBUM_H
#define ALBUM_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMenu>
#include <QAction>
#include <QPushButton>

#include "model.h"

namespace Ui {
    class Album;
}

class Album : public QWidget {
    Q_OBJECT

public:
    explicit Album(QWidget *parent = nullptr);
    ~Album();

    void setContextButtons(QAction *Export,
                           QAction *ChangeImg,
                           QAction *Delete,
                           QWidget *parent);
    void setAlbumName(QString name);
    void setModel(Model *model);
    void setPixMap(QPixmap *pixmap);
    QString getAlbumName();
    Model *getModel();
    QPixmap *getPixMap();
    QPushButton *getButton();
    QDataStream &serModel(QDataStream &out);
    QDataStream &desModel(QDataStream &in);

private:
    Ui::Album *ui;
    Model *m_model;
    QPixmap *m_icon;
    QString m_name;
};

#endif // ALBUM_H
