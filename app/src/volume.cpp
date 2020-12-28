#include "volume.h"
#include "ui_volume.h"
#include "mainwindow.h"

Volume::Volume(QWidget *parent) : QWidget(parent), ui(new Ui::Volume) {
    ui->setupUi(this);
    m_slider = nullptr;
    setWindowFlag(Qt::Popup);
    connect(ui->VolumeSl, &QSlider::sliderMoved, [this, parent](int value) {
        if (m_slider) {
            qobject_cast<MainWindow *>(parent)->getPlayer()->setVolume(value);
            if (qobject_cast<MainWindow *>(parent)->getVolume()->isChecked())
                qobject_cast<MainWindow *>(parent)->getVolume()->click();
            qobject_cast<MainWindow *>(parent)->getVolume()->setStyleSheet(value == 0
                    ? "image: url(:/img/Volume2.svg);" : "image: url(:/img/Volume1.svg);");;
        }
    });
}

Volume::~Volume() {
    delete ui;
}

void Volume::setSlider(QSlider *slider) {
    m_slider = slider;
}

void Volume::setSliderPosition(int position) {
    ui->VolumeSl->setValue(position);
}
