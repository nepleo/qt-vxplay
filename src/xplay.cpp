#include "xplay.h"
#include <QDebug>
#include <QFileDialog>
#include <QSlider>
#include <QmessageBox>

static xdemuxthread dt;

xplay::xplay(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.playpos, &QSlider::sliderPressed, this, &xplay::onSliderPress);
    connect(ui.playpos, &QSlider::sliderReleased, this, &xplay::onSliderRelease);
    connect(ui.openfile, &QPushButton::clicked, this, &xplay::onOpenFile);
    connect(ui.isplay, &QPushButton::clicked, this, &xplay::onPlayOrPause);
    dt.start();
    startTimer(40);
}

void xplay::onOpenFile()
{
    // 选择文件
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("open video file"));
    if (name.isEmpty())
        return;
    this->setWindowTitle(name);
    if (!dt.Open(name.toLocal8Bit(), ui.video))
    {
        QMessageBox::information(0, "error", "open file failed");
        return;
    }
    setPause(dt.isPause);
}

xplay::~xplay()
{
    dt.Close();
}

// 滑动条
void xplay::timerEvent(QTimerEvent *e)
{
    if (isSliderPress)
        return;
    long long total = dt.totalMs;
    if (total > 0)
    {
        double pos = (double)dt.pts / (double)total;
        int v = ui.playpos->maximum() * pos;
        ui.playpos->setValue(v);
    }
}

void xplay::resizeEvent(QResizeEvent *e)
{
    ui.playpos->move(100, this->height() - 95);
    ui.playpos->resize(this->width() - 200, ui.playpos->height());
    ui.openfile->move(100, this->height() - 60);
    ui.isplay->move(200, this->height() - 60);
    ui.video->resize(this->size());
}

void xplay::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (isFullScreen())
    {
        this->showNormal();
    }
    else
    {
        this->showFullScreen();
    }
}

void xplay::setPause(bool isPause)
{
    if (isPause)
        ui.isplay->setText(QString::fromLocal8Bit("play"));
    else
        ui.isplay->setText(QString::fromLocal8Bit("pause"));
}

void xplay::onPlayOrPause()
{
    bool isPause = !dt.isPause;
    setPause(isPause);
    dt.setPause(isPause);
}

void xplay::onSliderPress()
{
    isSliderPress = true;
}

void xplay::onSliderRelease()
{
    isSliderPress = false;
    double pos = 0.0;
    pos = (double)ui.playpos->value() / (double)ui.playpos->maximum();
    dt.Seek(pos);
}
