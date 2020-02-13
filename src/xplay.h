#pragma once

#include <QtWidgets/QWidget>
#include "ui_xplay.h"


class xplay : public QWidget
{
	Q_OBJECT

public:
	xplay(QWidget *parent = Q_NULLPTR);
	~xplay();

	//定时器  滑动条显示
	void timerEvent(QTimerEvent *e);

	//窗口尺寸变化
	void resizeEvent(QResizeEvent* e);

	//双击尺寸变化
	void mouseDoubleClickEvent(QMouseEvent* e);

	//播放函数
	void setPause(bool isPause);


public slots:
	void openFile();
	void PlayOrPause();
	void SliderPress();
	void SliderRelease();

private:
	bool isSliderPress = false;
	Ui::xplayClass ui;
};
