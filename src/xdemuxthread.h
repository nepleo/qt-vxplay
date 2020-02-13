#pragma once
#include <QThread>
#include <mutex>
class ivideocall;
class xvideothread;
class xaudiothread;
class xdemux;
class xdemuxthread:public QThread
{
public:
	virtual bool Open(const char* url, ivideocall* call);
	//启动线程
	virtual void start();

	//关闭线程清理资源
	virtual void Close();
	virtual void Clear();
	//Seek
	virtual void Seek(double pos);

	void run();
	xdemuxthread();
	virtual ~xdemuxthread();
	//线程退出
	bool isExit = false;

	void setPause(bool isPause);
	long long pts = 0;
	long long totalMs = 0;
	bool isPause = false;

protected:
	std::mutex mux;
	xdemux* demux = 0;
	xvideothread* vt = 0;
	xaudiothread* at = 0;

};

