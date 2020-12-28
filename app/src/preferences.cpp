#include "preferences.h"
#include "ui_preferences.h"
#include "mainwindow.h"

Preferences::Preferences(QWidget *parent) : QDialog(parent), ui(new Ui::Preferences) {
    ui->setupUi(this);
    QString &p_name = qobject_cast<MainWindow *>(parent)->getUser();
    QSqlQuery *p_query = qobject_cast<MainWindow *>(parent)->getQuery();
    setPlaceholder(p_query, p_name);
    connect(ui->Save, &QPushButton::clicked, [&p_name, p_query, this]() {
        p_query->prepare("UPDATE Users  SET Name = :newname, Pass = :newpass WHERE Name = :name;");
        p_query->bindValue(":name", p_name);
        p_query->bindValue(":newname", ui->NameLe->text());
        p_query->bindValue(":newpass", ui->PassLe->text());
        if (!p_query->exec()) {
            Message("Save error");
            return;
        }
        p_name = ui->NameLe->text();
        close();
    });
    setWindowTitle("Settings");
    setModal(true);
    exec();
}

Preferences::~Preferences() {
    delete ui;
}

void Preferences::setPlaceholder(QSqlQuery *query, QString &name) {
    ui->NameLe->setText(name);
    query->prepare("SELECT Pass FROM Users WHERE Name = :name;");
    query->bindValue(":name", name);
    query->exec();
    if (query->first())
        ui->PassLe->setText(query->value(0).toString());
}

