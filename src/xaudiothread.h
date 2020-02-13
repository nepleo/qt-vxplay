#pragma once
#include <Qthread>
#include <mutex>
#include "xdecodethread.h"
struct AVCodecParameters;
struct AVPacket;
class xdecode;
class xaudioplay;
class resample;
class xaudiothread : public xdecodethread
{
public:
	long long pts = 0;
	virtual bool Open(AVCodecParameters* para,int sampleRate,int channels);
	void run();

	//停止线程，清理资源
	virtual void Close();

	virtual void Clear();
	xaudiothread();
	virtual ~xaudiothread();
	int maxList = 100;
	//线程退出
	bool isExit = false;
	bool isPause = false;
	void setPause(bool isPause);
protected:
	xaudioplay* ap = 0;
	resample* res = 0;
	std::mutex amux;
};

