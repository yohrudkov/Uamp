#ifndef TIMER_H
#define TIMER_H

#include <QDialog>
#include <QTimer>

#include "message.h"

class MainWindow;

namespace Ui {
    class Timer;
}

class Timer : public QDialog {
    Q_OBJECT

public:
    explicit Timer(QWidget *parent = nullptr);
    ~Timer();

    void showWindow();

private:
    Ui::Timer *ui;
    QTimer *m_count;
};

#endif // TIMER_H
