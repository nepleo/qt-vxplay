#pragma once

#include <QtWidgets/QWidget>
#include "ui_xplay.h"


class xplay : public QWidget
{
	Q_OBJECT

public:
	xplay(QWidget *parent = Q_NULLPTR);
	~xplay();

	//��ʱ��  ��������ʾ
	void timerEvent(QTimerEvent *e);

	//���ڳߴ�仯
	void resizeEvent(QResizeEvent* e);

	//˫���ߴ�仯
	void mouseDoubleClickEvent(QMouseEvent* e);

	//���ź���
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
