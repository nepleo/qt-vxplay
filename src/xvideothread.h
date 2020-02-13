#pragma once
//�������ʾ��Ƶ

#include <list>
#include <mutex>
#include <Qthread>
#include "ivideocall.h"
#include "xdecodethread.h"
struct AVPacket;
struct AVCodecParameters;
class xdecode;

class xvideothread : public xdecodethread 
{
public:
	virtual bool Open(AVCodecParameters* para,ivideocall *call,int width,int height);
	void run();

	xvideothread();
	virtual ~xvideothread();
	bool isPause = false;
	void setPause(bool isPause);
	//ͬ��ʱ�����ⲿ����
	long long synpts = 0;
	//����pts,������յ��Ľ�������pts>= seekpts ,return true,������ʾ����
	virtual bool RepaintPts(AVPacket* pkt, long long seekpts);
protected:

	std::mutex vmux;
	ivideocall* call;


};

