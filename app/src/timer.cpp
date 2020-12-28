#include "timer.h"
#include "ui_timer.h"
#include "mainwindow.h"

Timer::Timer(QWidget *parent) : QDialog(parent), ui(new Ui::Timer) {
    ui->setupUi(this);
    m_count = new QTimer(this);
    m_count->setInterval(60000);
    ui->Time->setAttribute(Qt::WA_MacShowFocusRect, 0);
    connect(m_count, &QTimer::timeout, [this, parent]() {
        ui->Time->setValue(ui->Time->value() - 1);
        if (ui->Time->value() == 0)
            qobject_cast<MainWindow *>(parent)->close();
    });
    connect(ui->Disable, &QPushButton::clicked, [this]() {
        ui->Time->setValue(0);
        m_count->stop();
        ui->Status->setText("Timer disabled");
    });
    connect(ui->Activate, &QPushButton::clicked, [this]() {
        if (ui->Time->value() == 0) {
            Message("Timer error");
            return ;
        }
        m_count->stop();
        ui->Status->setText("Timer activated");
        m_count->start();
    });
}

Timer::~Timer() {
    delete ui;
    delete m_count;
}

void Timer::showWindow() {
    setWindowTitle("Timer");
    setModal(true);
    exec();
}
