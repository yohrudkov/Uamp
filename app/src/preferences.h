#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDialog>

#include "message.h"

class MainWindow;

namespace Ui {
    class Preferences;
}

class Preferences : public QDialog {
    Q_OBJECT

public:
    Preferences(QWidget *parent = nullptr);
    ~Preferences();

    void setPlaceholder(QSqlQuery *query, QString &name);

private:
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H

