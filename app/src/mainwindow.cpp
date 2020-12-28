#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    m_block = false;
    m_resize = false;
    m_rep = 0;
    m_netaccman = new QNetworkAccessManager(this);
    m_shutdown = new Timer(this);
    ui->SongName->setWordWrap(true);
    createPlayer();
    playerButtons();
    connectButtons();
    createRadio();
    radioButtons();
    setVolumeButton();
    setSortButton();
    setDataBase();
    setDBButtons();
    libraryButtons();
    macosArea();
    ui->Screens->setCurrentIndex(0);
    for (auto &i : m_library)
        i->getButton()->setIconSize(QSize(i->getButton()->width(), i->getButton()->width()));
}

MainWindow::~MainWindow() {
    delete ui;
    delete m_dbexec;
    delete m_netaccman;
    delete m_songlist;
    delete m_volume;
    delete m_player;
    delete m_radio;
    delete m_shutdown;
    for (auto &i : m_library)
        delete i;
}

QMediaPlaylist *MainWindow::getPlayList() {
    return m_playlist;
}

QMediaPlayer *MainWindow::getPlayer() {
    return m_player;
}

QToolButton *MainWindow::getVolume() {
    return ui->VolumeSound;
}

QSqlQuery *MainWindow::getQuery() {
    return m_dbexec;
}

QString &MainWindow::getUser() {
    return m_user;
}

QToolButton *MainWindow::getButton() {
    return ui->PlayPause;
}

void MainWindow::moveInPlayList(int from, int to, int current) {
    auto pos = m_player->position();
    m_block = true;
    ui->Stop->click();
    m_playlist->moveMedia(from, to);
    m_playlist->setCurrentIndex(current);
    m_block = false;
    m_player->setPosition(pos);
    ui->PlayPause->click();
}

void MainWindow::setSelect(int index) {
    m_songlist->selectionModel()->select(m_songlist->currentIndex(), QItemSelectionModel::Deselect);
    QModelIndex itNew = m_songlist->model()->index(index, 0);
    m_songlist->selectionModel()->select(itNew, QItemSelectionModel::Select);
    m_songlist->setCurrentIndex(itNew);
    ui->SongName->setText(m_songlist->model()->data(itNew).toString());
}

void MainWindow::deleteRow(int index) {
    auto i = m_playlist->currentIndex();
    auto pos = m_player->position();
    m_playlist->removeMedia(index);
    m_songlist->model()->removeRow(index);
    m_playlist->setCurrentIndex(index > i ? i : i - 1 < 0 ? 0 : i - 1);
    setSelect(index > i ? i : i - 1 < 0 ? 0 : i - 1);
    m_player->setPosition(pos);
    if (m_playlist->isEmpty()) {
        QLayoutItem *item = ui->VideoFr->layout()->takeAt(0);
        if (item != nullptr) {
            delete item->widget();
            delete item;
            m_video = new QVideoWidget(this);
            m_player->setVideoOutput(m_video);
        }
        m_songlist->setCurrent(-1);
        ui->Stop->click();
        buttonsEnable(false);
        ui->SongName->setText("Song name");
        ui->SongImg->setText("Song img");
        ui->SongPos->setValue(0);
    }
}

void MainWindow::changeState(QString user, bool flag) {
    ui->Screens->setCurrentIndex(flag);
    m_user = user;
    ui->actionChangeUser->setEnabled(flag);
    ui->actionUserSettings->setEnabled(flag);
    ui->actionDeleteUser->setEnabled(flag);
    buttonsEnable(false);
}

void MainWindow::buttonsEnable(bool flag) {
    ui->ChooseImg->setEnabled(flag);
    ui->DeleteImg->setEnabled(flag);
    ui->SaveInfoTag->setEnabled(flag);
    ui->PalayerToolBar->setEnabled(flag);
    ui->SongPos->setEnabled(flag);
}

void MainWindow::setDataBase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "uamp");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("uamp.db");
    db.setUserName("root");
    db.setPassword("12345");
    if (!db.open())
        Message("Database error");
    m_dbexec = new QSqlQuery(db);
    m_dbexec->exec("CREATE TABLE IF NOT EXISTS Users("
                   "Id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "Name TEXT NOT NULL UNIQUE,"
                   "Pass TEXT NOT NULL);");
    m_dbexec->exec("CREATE TABLE IF NOT EXISTS Albums("
                   "User TEXT NOT NULL,"
                   "Name TEXT NOT NULL UNIQUE,"
                   "Icon BLOB,"
                   "Model BLOB);");
}

void MainWindow::setDBButtons() {
    connect(ui->SignIn, &QPushButton::clicked, [this]() {
        m_dbexec->prepare("SELECT Pass FROM Users WHERE Name = :name;");
        m_dbexec->bindValue(":name", ui->LoginInLe->text());
        m_dbexec->exec();
        if (m_dbexec->first() && m_dbexec->value(0).toString() == ui->PassInLe->text()) {
            changeState(ui->LoginInLe->text(), true);
            loadAll();
            setShourCut(true);
            ui->TreeViewSp->setSizes(QList<int>() << width() * 0.6 << width() * 0.4);
            ui->TagInfoSp->setSizes(QList<int>() << height() * 0.6 << height() * 0.4);
            ui->ImgBassSp->setSizes(QList<int>() << ui->PlayerFr->width() * 0.5 << ui->PlayerFr->width() * 0.5);
        }
        else
            Message("SignIn error");
    });
    connect(ui->SignUp, &QPushButton::clicked, [this]() {
        m_dbexec->prepare("INSERT INTO Users (Name, Pass) VALUES (:name, :pass);");
        m_dbexec->bindValue(":name", ui->LoginUpLe->text());
        m_dbexec->bindValue(":pass", ui->PassUpLe->text());
        if (ui->LoginUpLe->text() == nullptr || ui->PassUpLe->text() == nullptr || !m_dbexec->exec())
            Message("SignUp error");
        else {
            changeState(ui->LoginUpLe->text(), true);
            loadAll();
            setShourCut(true);
            ui->TreeViewSp->setSizes(QList<int>() << width() * 0.6 << width() * 0.4);
            ui->TagInfoSp->setSizes(QList<int>() << height() * 0.6 << height() * 0.4);
            ui->ImgBassSp->setSizes(QList<int>() << ui->PlayerFr->width() * 0.5 << ui->PlayerFr->width() * 0.5);
        }
    });
    connect(ui->actionChangeUser, &QAction::triggered, [this]() {
        deleteAll();
        saveAll();
        setShourCut(false);
        for (auto &i : m_library) {
            Q_UNUSED(i);
            ui->LibraryScroll->layout()->takeAt(0)->widget()->deleteLater();
        }
        for (auto &i : m_library)
            delete i;
        m_library.clear();
        changeState(nullptr, false);
        ui->Stop->click();
        ui->RadioStop->click();
        m_playlist->clear();
        ui->LoginInLe->clear();
        ui->PassInLe->clear();
        ui->LoginUpLe->clear();
        ui->PassUpLe->clear();
        m_volume->setSliderPosition(100);
        ui->SongListName->clear();
    });
    connect(ui->actionUserSettings, &QAction::triggered, [this]() {
        Preferences pref(this);
    });
    connect(ui->actionDeleteUser, &QAction::triggered, [this]() {
        m_dbexec->prepare("DELETE FROM Users WHERE Name = :name;");
        m_dbexec->bindValue(":name", m_user);
        m_dbexec->exec();
        for (auto &i : m_library) {
            Q_UNUSED(i);
            ui->LibraryScroll->layout()->takeAt(0)->widget()->deleteLater();
        }
        for (auto &i : m_library)
            delete i;
        m_library.clear();
        ui->actionChangeUser->trigger();
    });
    connect(ui->actionAutoShutDown, &QAction::triggered, [this]() {
        m_shutdown->showWindow();
    });
}

void MainWindow::setVolumeButton() {
    ui->VolumeSound->setContextMenuPolicy(Qt::CustomContextMenu);
    m_volume = new Volume(this);
    m_volume->setSlider(ui->SongPos);
    connect(ui->VolumeSound, &QPushButton::customContextMenuRequested, [this]() {
        QMenu *menu = new QMenu(this);
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(m_volume);
        menu->setLayout(layout);
        menu->setContentsMargins(0, 0, 0, 0);
        menu->setStyleSheet("QMenu {background-color: #181B2C; border-radius: 5px; border: 1px solid black;}");
        menu->popup(mapToGlobal(QPoint(ui->VolumeSound->pos().x() + ui->VolumeSound->width() + 5,
                                       ui->PalayerToolBar->pos().y())));
    });
    connect(ui->VolumeSound, &QToolButton::clicked, [this]() {
        m_player->setMuted(ui->VolumeSound->isChecked() ? true : false);
        ui->VolumeSound->setStyleSheet(ui->VolumeSound->isChecked()
            ? "image: url(:/img/Volume2.svg);" : "image: url(:/img/Volume1.svg);");
        m_volume->setSliderPosition(m_player->isMuted() ? 0 : m_player->volume());
    });
}

void MainWindow::setSortButton() {
    connect(ui->Sort, &QPushButton::clicked, [this]() {
        QMenu *menu = new QMenu(this);
        QAction *Name = new QAction(tr("Name"), this);
        QAction *Path = new QAction(tr("Path"), this);
        connect(Name, &QAction::triggered, [this]() {
            sortModel("Name");
        });
        connect(Path, &QAction::triggered, [this]() {
            sortModel("Path");
        });
        menu->addAction(Name);
        menu->addAction(Path);
        menu->setMinimumWidth(ui->Sort->width());
        menu->setMaximumWidth(ui->Sort->width());
        menu->setLayoutDirection(Qt::RightToLeft);
        menu->setContentsMargins(0, 5, 0, 5);
        menu->setStyleSheet("QMenu {background-color: #181B2C; border-radius: 5px; border: 1px solid black;}");
        menu->popup(mapToGlobal(QPoint(ui->TreeViewFr->pos().x() + 5,
                                       ui->TreeViewToolBarFr->pos().y() - 25)));
    });
}

void MainWindow::connectButtons() {
    connect(ui->HideIn, &QPushButton::clicked, [this]() {
        ui->PassInLe->setEchoMode(ui->HideIn->isChecked() ? QLineEdit::Password : QLineEdit::Normal);
        if (ui->HideIn->isChecked())
            ui->HideIn->setIcon(QIcon(QPixmap(":/img/Eye2.png")));
        else
            ui->HideIn->setIcon(QIcon(QPixmap(":/img/Eye1.png")));
        ui->HideIn->setIconSize(QSize(ui->HideIn->width(), ui->HideIn->height()));
    });
    connect(ui->HideUp, &QPushButton::clicked, [this]() {
        ui->PassUpLe->setEchoMode(ui->HideUp->isChecked() ? QLineEdit::Password : QLineEdit::Normal);
        if (ui->HideUp->isChecked())
            ui->HideUp->setIcon(QIcon(QPixmap(":/img/Eye2.png")));
        else
            ui->HideUp->setIcon(QIcon(QPixmap(":/img/Eye1.png")));
        ui->HideUp->setIconSize(QSize(ui->HideUp->width(), ui->HideUp->height()));
    });
    connect(ui->Rep, &QPushButton::clicked, [this]() {
        m_rep = m_rep == 3 ? 0 : m_rep + 1;
        if (m_rep == 0) {
            m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
            ui->Rep->setStyleSheet("image: url(:/img/Rep1.svg);");
        }
        else if (m_rep == 1) {
            m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
            ui->Rep->setStyleSheet("image: url(:/img/Rep2.svg);");
        }
        else if (m_rep == 2) {
            m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
            ui->Rep->setStyleSheet("image: url(:/img/Rep3.svg);");
        }
        else if (m_rep == 3) {
            m_playlist->setPlaybackMode(QMediaPlaylist::Random);
            ui->Rep->setStyleSheet("image: url(:/img/Rep4.svg);");
        }
    });
    connect(m_player, &QMediaPlayer::durationChanged, ui->SongPos, &QSlider::setMaximum);
    connect(m_player, &QMediaPlayer::positionChanged, ui->SongPos, &QSlider::setValue);
    connect(ui->SongPos, &QSlider::sliderMoved, m_player, &QMediaPlayer::setPosition);
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [this](int pos) {
        if (m_block)
            return ;
        if (pos == -1 && m_rep == 0) {
            clearAllIfo();
            if (ui->PlayPause->isChecked())
                ui->PlayPause->click();
            buttonsEnable(false);
            ui->SongName->setText("Song name");
            return ;
        }
        m_path = m_songlist->model()->index(pos, 1).data().toString();
        TagLib::FileRef song(m_path.toStdString().c_str());
        if (!song.isNull() && song.tag())
            setSongInfo(song);
        else {
            Message("TagInfo error");
            clearAllIfo();
        }
        buttonsEnable(true);
        if (!ui->PlayPause->isChecked())
            ui->PlayPause->click();
        setSelect(pos);
    });
    connect(ui->ChooseImg, &QPushButton::clicked, [this]() {
        ui->ChooseImgLe->setText("");
        QString path = QFileDialog::getOpenFileName(this, tr("Open file"), "~", tr("*.jpg"));
        ui->ChooseImgLe->setText(path);
    });
    connect(ui->DeleteImg, &QPushButton::clicked, [this]() {
        ui->ChooseImgLe->setText("");
        ui->SongImg->setPixmap(QPixmap(":/img/Note.svg").scaled(ui->SongImg->width(), ui->SongImg->width(), Qt::KeepAspectRatio));
        TagLib::MPEG::File audioFile(m_path.toStdString().c_str());
        TagLib::ID3v2::Tag *tag = audioFile.ID3v2Tag(true);
        if (!tag->frameListMap()["APIC"].isEmpty())
            tag->removeFrames(tag->frameListMap()["APIC"].front()->frameID());
        audioFile.save();
    });
    connect(m_netaccman, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
        QPixmap img;
        img.loadFromData(reply->readAll());
        QString fileName = QFileDialog::getSaveFileName(this, "Save File", ".", "*.jpg");
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);
        if (img.save(&file)) {
            ImageFile::getImageFile(fileName.toStdString())->setArt(m_path.toStdString());
            getImg(ui->SongImg);
        }
    });
    connect(ui->SaveInfoTag, &QPushButton::clicked, [this]() {
        if (ui->ChooseImgLe->text().size() != 0
            && (m_path.right(4) == ".mp3" || m_path.right(4) == ".mp4")) {
            if (QFileInfo(ui->ChooseImgLe->text()).isFile() && QFileInfo(ui->ChooseImgLe->text()).isReadable()) {
                ui->SongImg->setPixmap(QPixmap(ui->ChooseImgLe->text()));
                ImageFile::getImageFile(ui->ChooseImgLe->text().toStdString())->setArt(m_path.toStdString());
            }
            else
                m_netaccman->get(QNetworkRequest(QUrl(ui->ChooseImgLe->text())));
        }
        saveInfo();
    });
}

void MainWindow::createPlayer() {
    m_songlist = new Table(this);
    m_player = new QMediaPlayer(this);
    m_video = new QVideoWidget(this);
    m_playlist = new QMediaPlaylist(m_player);
    ui->VideoFr->layout()->addWidget(m_video);
    ui->SongListFr->layout()->addWidget(m_songlist);
    m_player->setPlaylist(m_playlist);
    m_player->setVolume(100);
    m_player->setVideoOutput(m_video);
    m_playlist->setPlaybackMode(QMediaPlaylist::Sequential);
}

void MainWindow::playerButtons() {
    connect(ui->PrevSong, &QToolButton::clicked, [this]() {
        m_playlist->previous();
        ui->Stop->click();
        ui->PlayPause->click();
    });
    connect(ui->NextSong, &QToolButton::clicked, [this]() {
        m_playlist->next();
        ui->Stop->click();
        ui->PlayPause->click();
    });
    connect(ui->PlayPause, &QToolButton::clicked, [this]() {
        m_radio->stop();
        ui->PlayPause->isChecked() ? m_player->play() : m_player->pause();
        ui->PlayPause->setStyleSheet(ui->PlayPause->isChecked()
            ? "image: url(:/img/Pause.svg);" : "image: url(:/img/Play.svg);");
    });
    connect(ui->Stop, &QToolButton::clicked, [this]() {
        if (ui->PlayPause->isChecked())
            ui->PlayPause->click();
        m_player->stop();
        ui->SongPos->setValue(0);
    });
    connect(ui->Forward, &QToolButton::clicked, [this]() {
        m_player->setPosition(m_player->position() + 10000);
    });
    connect(ui->Rewind, &QToolButton::clicked, [this]() {
        m_player->setPosition(m_player->position() - 10000);
    });
}

void MainWindow::createRadio() {
    m_radio = new QMediaPlayer(this);
    m_radio->setVolume(100);
    ui->RadioVolume->setValue(100);
}

void MainWindow::radioButtons() {
    connect(ui->RadioPlay, &QToolButton::clicked, [this]() {
        m_player->stop();
        m_radio->stop();
        m_radio->play();
    });
    connect(ui->RadioStop, &QToolButton::clicked, m_radio, &QMediaPlayer::stop);
    connect(ui->RadioVolume, &QDial::valueChanged, m_radio, &QMediaPlayer::setVolume);
    connect(ui->KissFm, &QPushButton::clicked, [this]() {
        m_radio->setMedia(QUrl("http://online.kissfm.ua/KissFM_HD"));
        ui->RadioPlay->click();
        ui->RadioWaveName->setText("Now Playing: Kiss FM");
    });
    connect(ui->XitFm, &QPushButton::clicked, [this]() {
        m_radio->setMedia(QUrl("http://online.hitfm.ua/HitFM_HD"));
        ui->RadioPlay->click();
        ui->RadioWaveName->setText("Now Playing: Xit FM");
    });
    connect(ui->RadioRoks, &QPushButton::clicked, [this]() {
        m_radio->setMedia(QUrl("http://online.radioroks.ua/RadioROKS_HD"));
        ui->RadioPlay->click();
        ui->RadioWaveName->setText("Now Playing: Radio Roks");
    });
    connect(ui->NasheRadio, &QPushButton::clicked, [this]() {
        m_radio->setMedia(QUrl("http://online.nasheradio.ua/NasheRadio_HD"));
        ui->RadioPlay->click();
        ui->RadioWaveName->setText("Now Playing: Наше Радіо");
    });
    connect(ui->RadioRelax, &QPushButton::clicked, [this]() {
        m_radio->setMedia(QUrl("http://online.radiorelax.ua/RadioRelax_HD"));
        ui->RadioPlay->click();
        ui->RadioWaveName->setText("Now Playing: Radio Relax");
    });
    connect(ui->MelodiyaFm, &QPushButton::clicked, [this]() {
        m_radio->setMedia(QUrl("http://online.melodiafm.ua/MelodiaFM_HD"));
        ui->RadioPlay->click();
        ui->RadioWaveName->setText("Now Playing: Мелодія FM");
    });
    connect(ui->RussRadio, &QPushButton::clicked, [this]() {
        m_radio->setMedia(QUrl("http://online.rusradio.ua/RusRadio_HD"));
        ui->RadioPlay->click();
        ui->RadioWaveName->setText("Now Playing: Русское Радио");
    });
    connect(ui->RadioGold, &QPushButton::clicked, [this]() {
        m_radio->setMedia(QUrl("http://online.radioplayer.ua/RadioGold_HD"));
        ui->RadioPlay->click();
        ui->RadioWaveName->setText("Now Playing: Radio Gold");
    });
    connect(ui->RadioJazz, &QPushButton::clicked, [this]() {
        m_radio->setMedia(QUrl("http://online.radiojazz.ua/RadioJazz_HD"));
        ui->RadioPlay->click();
        ui->RadioWaveName->setText("Now Playing: Radio Jazz");
    });
}

void MainWindow::libraryButtons() {
    connect(ui->AddNewPlayList, &QPushButton::clicked, [this]() {
        if (ui->SongListName->text().isEmpty() || checkAlbumName(ui->SongListName->text())) {
            Message("Playlist error");
            return ;
        }
        newAlbum(ui->SongListName->text(), m_songlist->getModel());
    });
    connect(ui->BackToDefault, &QPushButton::clicked, [this]() {
        libraryEnabled(true);
        ui->Stop->click();
        ui->SongListName->setText("");
        ui->SongName->setText("Song name");
        m_playlist->clear();
        m_songlist->setModel(nullptr);
        Model *newModel = new Model(m_songlist);
        m_songlist->setModel(newModel);
        m_songlist->setMyModel(newModel);
        m_songlist->setCurrent(0);
        buttonsEnable(false);
    });
    connect(ui->ImportPlayList, &QPushButton::clicked, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Choose Playlist", ".", "*.m3u");
        Model *newModel = new Model;
        QFile f(fileName);
        if (f.open(QFile::ReadOnly)) {
            QTextStream s(&f);
            QString song;
            while (!s.atEnd()) {
                song = s.readLine();
                song = song.right(song.size() - 8);
                QList<QStandardItem *> items;
                items.append(new QStandardItem(QDir(song).dirName()));
                items.append(new QStandardItem(song));
                newModel->appendRow(items);
            }
        }
        else {
            Message("Import error");
            return ;
        }
        newAlbum(QFileInfo(fileName).baseName(), newModel);
    });
}

void MainWindow::libraryEnabled(bool flag) {
    ui->SongListName->setEnabled(flag);
    ui->BackToDefault->setEnabled(flag ? false : true);
}

void MainWindow::choseAlbum(Album *album) {
    ui->BackToDefault->click();
    libraryEnabled(false);
    ui->SongListName->setText(album->getAlbumName());
    for (int i = 0; i < album->getModel()->rowCount(); i++)
        m_playlist->addMedia(QUrl::fromLocalFile(album->getModel()->index(i, 1).data().toString()));
    m_songlist->setModel(album->getModel());
    m_songlist->setMyModel(album->getModel());
    ui->AMPTw->setCurrentIndex(0);

}

void MainWindow::deleteAlbum(Album *album) {
    int count = 0;
    for (auto &i : m_library) {
        if (i->getAlbumName() == album->getAlbumName()) {
            ui->LibraryScroll->layout()->takeAt(count)->widget()->deleteLater();
            m_library.remove(count);
            break;
        }
        count++;
    }
    if (m_songlist->getModel() == album->getModel())
        ui->BackToDefault->click();
}

void MainWindow::newAlbum(QString name, Model *model) {
    Album *newALbum = new Album(this);
    newALbum->setAlbumName(name);
    newALbum->setModel(model);
    m_library.push_back(newALbum);
    libraryEnabled(false);
    ui->LibraryScroll->layout()->addWidget(newALbum);
}

bool MainWindow::changwAlbumName(QString name, Album *album) {
    if (checkAlbumName(name)) {
        album->setAlbumName(album->getAlbumName());
        return false;
    }
    if (m_songlist->getModel() == album->getModel())
        ui->SongListName->setText(name);
    return true;
}

bool MainWindow::checkAlbumName(QString name) {
    for (auto &i : m_library)
        if (i->getAlbumName() == name)
            return true;
    return false;
}

void MainWindow::loadAll() {
    m_dbexec->prepare("SELECT * FROM Albums WHERE User = :user");
    m_dbexec->bindValue(":user", m_user);
    m_dbexec->exec();
    while (m_dbexec->next() == true) {
        QString name =  m_dbexec->value(1).toString();

        QByteArray arrayIcon = m_dbexec->value(2).toByteArray();
        QPixmap *newIcon = new QPixmap;
        if (arrayIcon.size() > 0)
            newIcon->loadFromData(arrayIcon);

        QByteArray arrayModel = m_dbexec->value(3).toByteArray();
        QDataStream stream(&arrayModel, QIODevice::ReadWrite);
        Model *newModel = new Model;
        Album *newALbum = new Album(this);
        newALbum->setAlbumName(name);
        newALbum->setModel(newModel);
        newALbum->desModel(stream);
        if (arrayIcon.size() != 0)
            newALbum->setPixMap(newIcon);
        m_library.push_back(newALbum);
        ui->LibraryScroll->layout()->addWidget(newALbum);
    }
}

void MainWindow::saveAll() {
    for (auto &i : m_library) {
        m_dbexec->prepare("INSERT INTO Albums (User, Name, Icon, Model) VALUES (:user, :name, :icon, :model);");
        m_dbexec->bindValue(":user", m_user);
        m_dbexec->bindValue(":name", i->getAlbumName());
        QPixmap *inPixmap = i->getPixMap();
        QByteArray arrayIcon;
        if (inPixmap != nullptr) {
            QBuffer inBuffer(&arrayIcon);
            inBuffer.open(QIODevice::WriteOnly);
            inPixmap->save(&inBuffer, "PNG");
        }
        m_dbexec->bindValue(":icon", arrayIcon);

        QByteArray arrayModel;
        QDataStream stream(&arrayModel, QIODevice::ReadWrite);
        i->serModel(stream);
        m_dbexec->bindValue(":model", arrayModel);
        m_dbexec->exec();
    }
}

void MainWindow::deleteAll() {
    m_dbexec->prepare("DELETE FROM Albums WHERE User = :name;");
    m_dbexec->bindValue(":name", m_user);
    m_dbexec->exec();
    ui->BackToDefault->click();
    ui->AMPTw->setCurrentIndex(0);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (m_user != nullptr) {
        m_player->stop();
        m_radio->stop();
        ui->actionChangeUser->trigger();
    }
    event->accept();
}

void MainWindow::setLyrics(QString lyrics) {
    TagLib::MPEG::File file(m_path.toStdString().c_str());
    TagLib::ID3v2::FrameList frames = file.ID3v2Tag()->frameListMap()["USLT"];
    TagLib::ID3v2::UnsynchronizedLyricsFrame *frame =
        new TagLib::ID3v2::UnsynchronizedLyricsFrame;
    if (!file.ID3v2Tag()->frameListMap()["USLT"].isEmpty())
        file.ID3v2Tag()->removeFrames(file.ID3v2Tag()->frameListMap()["USLT"].front()->frameID());
    frame->setText(lyrics.toStdString().c_str());
    file.ID3v2Tag()->addFrame(frame);
    file.save();
}

void MainWindow::getImg(QLabel *label) {
    TagLib::MPEG::File file(m_path.toStdString().c_str());
    TagLib::ID3v2::Tag *m_tag = file.ID3v2Tag(true);
    TagLib::ID3v2::FrameList frameList = m_tag->frameList("APIC");
    if(frameList.isEmpty()) {
        label->setPixmap(QPixmap(":/img/Note.svg").scaled(200, 200, Qt::KeepAspectRatio));
        return ;
    }
    TagLib::ID3v2::AttachedPictureFrame *coverImg = 
        static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());
    QImage coverQImg;
    coverQImg.loadFromData((const uchar *)coverImg->picture().data(),
                            coverImg->picture().size());
    QPixmap pix = QPixmap::fromImage(coverQImg).scaled(
        label->width(),
        label->height(),
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
    );
    QBitmap map(pix.width(), pix.height());
    map.fill(Qt::color0);
    QPainter painter(&map);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(0, 0, pix.width(), pix.height(), 50, 50);
    pix.setMask(map);
    label->setPixmap(pix.scaled(200, 200, Qt::KeepAspectRatio));
}

QString MainWindow::getLyrics() {
    TagLib::String lyrics;
    TagLib::MPEG::File file(m_path.toStdString().c_str());
    TagLib::ID3v2::FrameList frames = file.ID3v2Tag()->frameListMap()["USLT"];
    TagLib::ID3v2::UnsynchronizedLyricsFrame *frame = NULL;
    if (!frames.isEmpty()) {
        frame = dynamic_cast<TagLib::ID3v2::UnsynchronizedLyricsFrame *>(frames.front());
        if (frame)
            lyrics = frame->text();
    }
    return lyrics.toCString(true);
}

void MainWindow::setSongInfo(TagLib::FileRef song) {
    clearAllIfo();
    ui->AlbumLe->setText(song.tag()->album().toCString());
    ui->ArtistLe->setText(song.tag()->artist().toCString());
    ui->CommentLe->setText(song.tag()->comment().toCString());
    ui->GenreLe->setText(song.tag()->genre().toCString());
    ui->TitleLe->setText(song.tag()->title().toCString());
    ui->YearLe->setText(TagLib::String::number(song.tag()->year()).toCString());
    ui->TrackLe->setText(TagLib::String::number(song.tag()->track()).toCString());
    ui->LyricsTe->setText(getLyrics());
    getImg(ui->SongImg);
}

void MainWindow::saveInfo() {
    TagLib::FileRef song(m_path.toStdString().c_str());
    if (!song.isNull() && song.tag()) {
        song.tag()->setAlbum(ui->AlbumLe->text().toStdString().empty() ? "" : ui->AlbumLe->text().toStdString());
        song.tag()->setArtist(ui->ArtistLe->text().toStdString().empty() ? "" : ui->ArtistLe->text().toStdString());
        song.tag()->setComment(ui->CommentLe->text().toStdString().empty() ? "" : ui->CommentLe->text().toStdString());
        song.tag()->setGenre(ui->GenreLe->text().toStdString().empty() ? "" : ui->GenreLe->text().toStdString());
        song.tag()->setTitle(ui->TitleLe->text().toStdString().empty() ? "" : ui->TitleLe->text().toStdString());
        song.tag()->setYear(ui->YearLe->text().toInt());
        song.tag()->setTrack(ui->TrackLe->text().toInt());
        song.save();
        if (m_path.right(4) == ".mp3" || m_path.right(4) == ".mp4") {
            setLyrics(ui->LyricsTe->toPlainText());
            getImg(ui->SongImg);
        }
    }
    else {
        Message("Save error");
        clearAllIfo();
    }
}

void MainWindow::clearAllIfo() {
    ui->AlbumLe->clear();
    ui->ArtistLe->clear();
    ui->CommentLe->clear();
    ui->GenreLe->clear();
    ui->TitleLe->clear();
    ui->YearLe->clear();
    ui->TrackLe->clear();
    ui->LyricsTe->clear();
}

void MainWindow::sortModel(QString type) {
    ui->Stop->click();
    m_playlist->clear();
    m_songlist->setSortingEnabled(true);
    if (type == "Name")
        m_songlist->sortByColumn(0, Qt::SortOrder::DescendingOrder);
    else
        m_songlist->sortByColumn(1, Qt::SortOrder::DescendingOrder);
    for (int i = 0; i < m_songlist->model()->rowCount() - 1; i++)
        m_playlist->addMedia(QUrl::fromLocalFile(m_songlist->model()->index(i, 1).data().toString()));
    m_songlist->setSortingEnabled(false);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (!m_resize) {
        m_resize = true;
        return ;
    }
    for (auto &i : m_library)
        i->getButton()->setIconSize(QSize(i->getButton()->width() / 2, i->getButton()->width() / 2));
}

void MainWindow::macosArea() {
    ui->LoginUpLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->PassUpLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->LoginInLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->PassInLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->SongListName->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->AlbumLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->ArtistLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->CommentLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->GenreLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->TitleLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->YearLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->TrackLe->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->LyricsTe->setAttribute(Qt::WA_MacShowFocusRect, 0);

    ui->HideIn->setIcon(QIcon(QPixmap(":/img/Eye1.png")));
    ui->HideIn->setIconSize(QSize(ui->HideIn->width(), ui->HideIn->height()));
    ui->HideUp->setIcon(QIcon(QPixmap(":/img/Eye1.png")));
    ui->HideUp->setIconSize(QSize(ui->HideUp->width(), ui->HideUp->height()));

    ui->YearLe->setValidator(new QIntValidator(0, 100000000, this));
    ui->TrackLe->setValidator(new QIntValidator(0, 100000000, this));
}

void MainWindow::setShourCut(bool flag) {
    ui->NextSong->setShortcut(flag ? QKeySequence(Qt::CTRL + Qt::Key_N) : QKeySequence::NoMatch);
    ui->PrevSong->setShortcut(flag ? QKeySequence(Qt::CTRL + Qt::Key_B) : QKeySequence::NoMatch);
    ui->Stop->setShortcut(flag ? QKeySequence(Qt::CTRL + Qt::Key_S) : QKeySequence::NoMatch);
    ui->PlayPause->setShortcut(flag ? QKeySequence(Qt::CTRL + Qt::Key_P) : QKeySequence::NoMatch);
    ui->Rewind->setShortcut(flag ? QKeySequence(Qt::CTRL + Qt::Key_R) : QKeySequence::NoMatch);
    ui->Forward->setShortcut(flag ? QKeySequence(Qt::CTRL + Qt::Key_F) : QKeySequence::NoMatch);
    ui->VolumeSound->setShortcut(flag ? QKeySequence(Qt::CTRL + Qt::Key_V) : QKeySequence::NoMatch);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Return) {
        if (ui->SongListName->hasFocus()) {
            ui->AddNewPlayList->click();
            ui->AMPTw->setCurrentIndex(1);
        }
        else if (ui->LoginInLe->hasFocus())
            ui->SignIn->click();
        else if (ui->PassInLe->hasFocus())
            ui->SignIn->click();
        else if (ui->LoginUpLe->hasFocus())
            ui->SignUp->click();
        else if (ui->PassUpLe->hasFocus())
            ui->SignUp->click();
        else
            QMainWindow::keyPressEvent(event);
    }
    else
        QMainWindow::keyPressEvent(event);
}
