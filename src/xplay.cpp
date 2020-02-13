#include "xplay.h"
#include <QFileDialog>
#include "xdemuxthread.h"
#include <QDebug>
#include <QmessageBox>

static xdemuxthread dt;

xplay::xplay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	dt.start();
	startTimer(40);
}

void xplay::openFile()
{
	//选择文件
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
	//qDebug() << name << endl;
	if (name.isEmpty()) return;
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

//滑动条
void xplay::timerEvent(QTimerEvent *e)
{
	if (isSliderPress) return;
	long long total = dt.totalMs;
	if (total > 0)
	{
		double pos = (double)dt.pts / (double)total;
		int v = ui.playpos->maximum() * pos;
		ui.playpos->setValue(v);
	}
}

void xplay::resizeEvent(QResizeEvent* e)
{
	ui.playpos->move(100, this->height() - 95);
	ui.playpos->resize(this->width() - 200, ui.playpos->height());
	ui.openFile->move(100 , this->height() - 60);
	ui.isplay->move(200, this->height() - 60);
	ui.video->resize(this->size());
}

void xplay::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (isFullScreen())
	{
		this -> showNormal();
	}
	else
	{
		this->showFullScreen();
	}
}

void xplay::setPause(bool isPause)
{
	if (isPause)
		ui.isplay->setText(QString::fromLocal8Bit("播放"));
	else
		ui.isplay->setText(QString::fromLocal8Bit("暂停"));
}

void xplay::PlayOrPause()
{
	bool isPause = !dt.isPause;
	setPause(isPause);
	dt.setPause(isPause);
}

void xplay::SliderPress()
{
	isSliderPress = true;
}

void xplay::SliderRelease()
{
	isSliderPress = false;
	double pos = 0.0;
	pos = (double)ui.playpos->value() / (double)ui.playpos->maximum();
	dt.Seek(pos);
}
