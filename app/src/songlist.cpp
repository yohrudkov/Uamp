#include "songlist.h"
#include "mainwindow.h"

Table::Table(QWidget *parent) : QTableView(parent), m_parent(parent) {
    m_current = -1;
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDragDropOverwriteMode(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
    setDefaultDropAction(Qt::MoveAction);
    m_model = new Model(this);
    setModel(m_model);
    connect(this, &QTableView::doubleClicked, [this](const QModelIndex &index){
        m_current = index.row();
        QMediaPlaylist *list = qobject_cast<MainWindow *>(m_parent)->getPlayList();
        if (list->currentIndex() == index.row() && list->currentIndex() >= 0)
            qobject_cast<MainWindow *>(m_parent)->getButton()->click();
        else
            list->setCurrentIndex(index.row());
    });
    connect(this, &QTableView::customContextMenuRequested, this, &Table::slotCustomMenuRequested);
}

Table::~Table() {
    delete m_model;
}

void Table::forFiles(QString path) {
    if (QFileInfo(path).exists()
        && QFileInfo(path).isFile()
        && QFileInfo(path).isWritable()
        && QFileInfo(path).isReadable()
        && (path.right(4) == ".wav" || path.right(4) == ".mp3" || path.right(4) == ".mp4" || path.right(5) == ".flac")) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QDir(path).dirName()));
        items.append(new QStandardItem(path));
        m_model->appendRow(items);
        qobject_cast<MainWindow *>(m_parent)->getPlayList()->addMedia(QUrl::fromLocalFile(path));
    }
}

void Table::forDirs(QString path) {
    if (QFileInfo(path).exists()
        && QFileInfo(path).isDir()
        && QFileInfo(path).isReadable()) {
        QDirIterator iterator(path, QStringList() << "*.wav" << "*.mp3" << "*.flac" << "*.mp4", QDir::Files, QDirIterator::Subdirectories);
        while (iterator.hasNext())
            forFiles(QFile(iterator.next()).fileName());
    }
}

void Table::moveLine(int to, int current) {
    qobject_cast<MainWindow *>(m_parent)->moveInPlayList(m_index, to, current);
}

int Table::getCurrent() {
    return m_current;
}

void Table::setCurrent(int index) {
    m_current = index;
}

int Table::getIndex() {
    return m_index;
}

Model *Table::getModel() {
    return m_model;
}

void Table::setMyModel(Model *model) {
    m_model = model;
}

void Table::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton))
        return ;
    m_index = indexAt(event->pos()).row();
    QTableView::mouseMoveEvent(event);
}

void Table::dragEnterEvent(QDragEnterEvent *event) {
    event->acceptProposedAction();
}

void Table::dropEvent(QDropEvent *event) {
    if (event->proposedAction() == Qt::CopyAction && event->mimeData()->hasUrls()) {
        QList<QUrl> urlList = event->mimeData()->urls();
        foreach (QUrl url, urlList) {
            forFiles(url.toLocalFile());
            forDirs(url.toLocalFile());
        }
    }
    else if (event->proposedAction() == Qt::MoveAction
             && indexAt(event->pos()).row() - 1 != m_index
             && indexAt(event->pos()).row() != -1)
        QTableView::dropEvent(event);
}

void Table::slotCustomMenuRequested(QPoint position) {
    QMenu *menu = new QMenu(this);
    QAction *hideDevice = new QAction(tr(this->isColumnHidden(1) ? "Show path column" : "Hide path column"), this);
    QAction *deleteDevice = new QAction(tr("Delete"), this);
    QAction *clearDevice = new QAction(tr("Clear"), this);
    connect(hideDevice, &QAction::triggered, [this]() {
        setColumnHidden(1, this->isColumnHidden(1) ? false : true);
    });
    connect(deleteDevice, &QAction::triggered, [this]() {
        QModelIndexList selection = selectionModel()->selectedRows();
        while (!selection.isEmpty()) {
            qobject_cast<MainWindow *>(m_parent)->deleteRow(selection.last().row());
            selection.removeLast();
        }
    });
    connect(clearDevice, &QAction::triggered, [this]() {
        for (int i = m_model->rowCount() - 1; i >= 0; i--)
            qobject_cast<MainWindow *>(m_parent)->deleteRow(i);
    });
    menu->addAction(hideDevice);
    menu->addAction(deleteDevice);
    menu->addAction(clearDevice);
    menu->setContentsMargins(0, 0, 0, 0);
    menu->setStyleSheet("QMenu {background-color: #181B2C; border-radius: 5px; border: 1px solid black;}");
    menu->popup(viewport()->mapToGlobal(position));
}
