#ifndef VOLUME_H
#define VOLUME_H

#include <QWidget>
#include <QSlider>

class MainWindow;

namespace Ui {
    class Volume;
}

class Volume : public QWidget {
    Q_OBJECT

public:
    explicit Volume(QWidget *parent = nullptr);
    ~Volume();

    void setSlider(QSlider *slider);
    void setSliderPosition(int position);

private:
    Ui::Volume *ui;
    QSlider *m_slider;
};

#endif // VOLUME_H
