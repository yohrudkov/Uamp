#include "message.h"
#include "ui_message.h"

Message::Message(QString message, QWidget *parent) : QDialog(parent), ui(new Ui::Message) {
    ui->setupUi(this);
    ui->Text->setText(message);
    ui->Icon->setIcon(QIcon(QPixmap(":/img/Error.svg")));
    ui->Icon->setIconSize(QSize(ui->Icon->width() - 20, ui->Icon->width() - 20));
    connect(ui->Ok, &QPushButton::clicked, this, &QDialog::close);
    this->setWindowTitle("");
    this->setModal(true);
    this->exec();
}

Message::~Message() {
    delete ui;
}
