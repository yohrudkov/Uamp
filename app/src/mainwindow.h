#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QSql>
#include <QUrl>
#include <QSqlDatabase>
#include <QDropEvent>
#include <QMimeData>
#include <QStringListModel>
#include <QToolButton>
#include <QSlider>
#include <QDial>
#include <QFileDialog>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVideoWidget>
#include <QMenu>
#include <QLayout>
#include <QAction>
#include <QBuffer>
#include <QCloseEvent>
#include <QBitmap>
#include <QPainter>
#include <QIcon>
#include <QLabel>
#include <algorithm>

#include "album.h"
#include "preferences.h"
#include "songlist.h"
#include "volume.h"
#include "timer.h"
#include "img.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    QMediaPlaylist *getPlayList();
    QMediaPlayer *getPlayer();
    QToolButton *getVolume();
    QSqlQuery *getQuery();
    QString &getUser();
    QToolButton *getButton();
    void moveInPlayList(int from, int to, int current);
    void setSelect(int index);
    void deleteRow(int index);
    void changeState(QString user, bool flag);
    void buttonsEnable(bool flag);
    void setDataBase();
    void setDBButtons();
    void setVolumeButton();
    void setSortButton();
    void connectButtons();
    void resetSongLenth();
    void createPlayer();
    void playerButtons();
    void createRadio();
    void radioButtons();
    void libraryButtons();
    void libraryEnabled(bool flag);
    void choseAlbum(Album *album);
    void deleteAlbum(Album *album);
    void newAlbum(QString name, Model *model);
    bool changwAlbumName(QString name, Album *album);
    bool checkAlbumName(QString name);
    void loadAll();
    void saveAll();
    void deleteAll();
    void setLyrics(QString lyrics);
    void getImg(QLabel *label);
    QString getLyrics();
    void setSongInfo(TagLib::FileRef song);
    void saveInfo();
    void clearAllIfo();
    void sortModel(QString type);
    void macosArea();
    void setShourCut(bool flag);

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    

private:
    Ui::MainWindow *ui;
    int m_rep;
    bool m_block;
    bool m_resize;
    QSqlQuery *m_dbexec;
    QNetworkAccessManager *m_netaccman;
    Table *m_songlist;
    Volume *m_volume;
    QMediaPlayer *m_player;
    QMediaPlayer *m_radio;
    QMediaPlaylist *m_playlist;
    QVideoWidget *m_video;
    QString m_user;
    QString m_path;
    Timer *m_shutdown;
    QVector<Album *> m_library;

public slots:
    void resizeEvent(QResizeEvent* event) override;
};

#endif // MAINWINDOW_H
