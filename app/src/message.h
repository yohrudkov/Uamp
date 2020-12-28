#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDialog>

namespace Ui {
    class Message;
}

class Message : public QDialog {
    Q_OBJECT

public:
    explicit Message(QString message, QWidget *parent = nullptr);
    ~Message();

private:
    Ui::Message *ui;
};

#endif // MESSAGE_H
