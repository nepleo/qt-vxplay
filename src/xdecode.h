#pragma once
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
#include <mutex>
extern void XFreePacket(AVPacket** pkt);
extern void XFreeFrame(AVFrame** frame);
class xdecode
{
public:
	long long pts = 0;
	bool isAudio = false;
	//�򿪽�����,���Ƿ�ɹ����ͷſռ�
	virtual bool Open(AVCodecParameters *para);
	virtual void Close();
	virtual void Clear();

	virtual bool Send(AVPacket* pkt);
	virtual AVFrame *Recv();
	xdecode();
	virtual ~xdecode();

protected:
	AVCodecContext* codec = 0;
	std::mutex mux;
};

