#include "album.h"
#include "mainwindow.h"
#include "ui_album.h"

Album::Album(QWidget *parent) : QWidget(parent), ui(new Ui::Album) {
    ui->setupUi(this);
    m_icon = nullptr;
    ui->ContextMenu->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->ContextMenu, &QPushButton::customContextMenuRequested, [this, parent](QPoint pos) {
        QMenu *menu = new QMenu(this);
        QAction *Export = new QAction(tr("Export"), this);
        QAction *ChangeImg = new QAction(tr("ChangeImg"), this);
        QAction *Delete = new QAction(tr("Delete"), this);
        setContextButtons(Export, ChangeImg, Delete, parent);
        menu->addAction(Export);
        menu->addAction(ChangeImg);
        menu->addAction(Delete);
        menu->setStyleSheet("QMenu {background-color: #181B2C; border-radius: 5px; border: 1px solid black;}");
        menu->popup(mapToGlobal(pos));
    });
    connect(ui->ContextMenu, &QPushButton::clicked, [this, parent]() {
        qobject_cast<MainWindow *>(parent)->choseAlbum(this);
    });
    connect(ui->AlbumName, &QLineEdit::editingFinished, [this, parent]() {
        if (qobject_cast<MainWindow *>(parent)->changwAlbumName(ui->AlbumName->text(), this))
            m_name = ui->AlbumName->text();
    });
    ui->ContextMenu->setIcon(QIcon(QPixmap(":/img/Album.svg")));
    ui->ContextMenu->setIconSize(QPixmap(":/img/Album.svg").rect().size());
    ui->AlbumName->setAttribute(Qt::WA_MacShowFocusRect, 0);
}

Album::~Album() {
    delete ui;
    delete m_model;
    delete m_icon;
}

void Album::setContextButtons(QAction *Export, QAction *ChangeImg,
                              QAction *Delete, QWidget *parent) {
    connect(Export, &QAction::triggered, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Open Directory", ".",
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        QMediaPlaylist list;
        for (int i = 0; i < m_model->rowCount(); i++)
            list.addMedia(QUrl::fromLocalFile(m_model->index(i, 1).data().toString()));
        list.save(QUrl::fromLocalFile(dir + "/" + getAlbumName() + ".m3u"), "m3u");
    });
    connect(ChangeImg, &QAction::triggered, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Choose Image",
                                                        ".", "Image Files (*.png *.jpg *.jpeg)");
        if (QFileInfo(fileName).exists() && QFileInfo(fileName).isReadable()) {
            m_icon = new QPixmap(fileName);
            QBitmap map(m_icon->width(), m_icon->height());
            map.fill(Qt::color0);
            QPainter painter(&map);
            painter.setBrush(Qt::color1);
            painter.drawRoundedRect(0, 0, m_icon->width(), m_icon->height(), 50, 50);
            m_icon->setMask(map);
            ui->ContextMenu->setIcon(QIcon(*m_icon));
            ui->ContextMenu->setIconSize(m_icon->rect().size());
        }
    });
    connect(Delete, &QAction::triggered, [this, parent]() {
        qobject_cast<MainWindow *>(parent)->deleteAlbum(this);
    });
}

void Album::setAlbumName(QString name) {
    ui->AlbumName->setText(name);
    m_name = name;
}

void Album::setModel(Model *model) {
    m_model = model;
}

void Album::setPixMap(QPixmap *pixmap) {
    m_icon = pixmap;
    QIcon ButtonIcon(*m_icon);
    ui->ContextMenu->setIcon(ButtonIcon);
    ui->ContextMenu->setIconSize(m_icon->rect().size());
}

QString Album::getAlbumName() {
    return ui->AlbumName->text();
}

Model *Album::getModel() {
    return m_model;
}

QPixmap *Album::getPixMap() {
    return m_icon;
}

QPushButton *Album::getButton() {
    return ui->ContextMenu;
}

QDataStream &Album::serModel(QDataStream &out) {
    for (int i = 0; i < m_model->rowCount(); i++)
        out << m_model->index(i, 0).data().toString()
            << m_model->index(i, 1).data().toString();
    return out;
}

QDataStream &Album::desModel(QDataStream &in) {
    QString name;
    QString path;
    while (!in.atEnd()) {
        in >> name;
        in >> path;
        QList<QStandardItem *> items;
        items.append(new QStandardItem(name));
        items.append(new QStandardItem(path));
        m_model->appendRow(items);
    }
    return in;
}
