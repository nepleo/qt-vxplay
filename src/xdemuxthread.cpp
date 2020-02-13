#include "xdemuxthread.h"
#include "xvideothread.h"
#include "xaudiothread.h"
#include "xdemux.h"
#include "xdecode.h"
#include <iostream>
extern "C"
{
#include <libavformat/avformat.h>
}
using namespace std;

bool xdemuxthread::Open(const char* url, ivideocall *call)
{
	mux.lock();
	if (!demux) demux = new xdemux();
	if (!at) at = new xaudiothread();
	if (!vt) vt = new xvideothread();
	bool re = demux->Open(url);
	if (!re)
	{
		cout << "demux->Open failed!" << endl;
		re =  false;
	}
	//����Ƶ������
	if (!vt->Open(demux->CopyVPara(), call, demux->width, demux->height))
	{
		re = false;
		cout << "vt->open failed!" << endl;
	}
	//����Ƶ������
	if (!at->Open(demux->CopyAPara(),demux->sampleRate,demux->channels))
	{
		re = false;
		cout << "at->open failed!" << endl;
	}
	totalMs = demux->totalMs;

	mux.unlock();
	return re;
}

void xdemuxthread::start()
{
	mux.lock();

	if (!demux) demux = new xdemux();
	if (!at) at = new xaudiothread();
	if (!vt) vt = new xvideothread();

	QThread::start();
	if (vt) vt->start();
	if (at) at->start();
	mux.unlock();
}

void xdemuxthread::Close()
{
	isExit = true;
	wait();
	if (vt) vt->Close();
	if (at) at->Close();
	mux.lock();
	delete vt;
	delete at;
	vt = NULL;
	at = NULL;
	mux.unlock();
}

void xdemuxthread::Clear()
{
	mux.lock();
	if (demux) demux->Clear();
	if (vt) vt->Clear();
	if (at) at->Clear();
	mux.unlock();
}

void xdemuxthread::Seek(double pos)
{
	//������
	Clear();

	mux.lock();
	bool status = this->isPause;
	mux.unlock();
	//��ͣ
	setPause(true);

	mux.lock();
	if (demux)
		demux->Seek(pos);
	//ʵ��Ҫ��ʾ��λ��pts
	long long seekPts = pos * demux->totalMs;
	while (!isExit)
	{
		AVPacket* pkt = demux->ReadVideo();
		if (!pkt) break;
		//������뵽seekPts
		if (vt->RepaintPts(pkt, seekPts))
		{
			this->pts = seekPts;
			break;
		}
		mux.unlock();

		//seek�Ƿ���ͣ״̬
		if (!status)
			setPause(false);
	}
}

void xdemuxthread::run()
{
	while (!isExit)
	{
		mux.lock();
		if (isPause)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		
		if (!demux)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		//����Ƶͬ��
		if (vt && at)
		{
			pts = at->pts;
			vt->synpts = at->pts;
		}
		AVPacket *pkt = demux->Read();
		if (!pkt)
		{
			mux.unlock();
			msleep(5);
			continue;
		}
		if (demux->IsAudio(pkt))
		{
			if (at) at->Push(pkt);
		}
		else
		{
			if (vt) vt->Push(pkt);
			msleep(1);
		}
		mux.unlock();

	}

}

xdemuxthread::xdemuxthread()
{
}

xdemuxthread::~xdemuxthread()
{
	isExit = true;
	wait();
}

void xdemuxthread::setPause(bool isPause)
{
	mux.lock();
	this->isPause = isPause;
	if (at) at->setPause(isPause);
	if (vt) vt->setPause(isPause);
	mux.unlock();
}
