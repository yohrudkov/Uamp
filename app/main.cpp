#include <QApplication>
#include <QCoreApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon(":/img/exe.svg"));
    w.setWindowTitle("Music Player");
    w.show();
    int res = a.exec();
//    system("leaks -q uamp");
    return res;
}
