#pragma once

#include <QtWidgets/QWidget>
#include "ui_xplay.h"
#include "xdemuxthread.h"

class xplay : public QWidget
{
    Q_OBJECT

  public:
    xplay(QWidget *parent = Q_NULLPTR);

    ~xplay();

    void timerEvent(QTimerEvent *e);

    void resizeEvent(QResizeEvent* e);

    void mouseDoubleClickEvent(QMouseEvent* e);

    void setPause(bool isPause);

  public slots:
    void onOpenFile();
    void onPlayOrPause();
    void onSliderPress();
    void onSliderRelease();

  private:
    bool isSliderPress = false;
    Ui::xplayClass ui;
};
