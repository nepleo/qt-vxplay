#include "xvideothread.h"
#include "xdecode.h"


bool xvideothread::Open(AVCodecParameters* para,ivideocall *call, int width, int height)
{
	if (!para)return false;
	Clear();
	vmux.lock();
	synpts = 0;
	//��ʼ����ʾ����
	this->call = call;
	if (call)
	{
		call->Init(width, height);
	}

	vmux.unlock();

	bool re = true;
	if (!decode->Open(para))
	{
		re = false;
	}

	return re;
}


void xvideothread::run()
{
	while (!isExit)
	{
		vmux.lock();
		if (this->isPause)
		{
			vmux.unlock();
			msleep(5);
			continue;
		}

		if (synpts > 0 && synpts < decode->pts)   //����Ƶͬ��
		{
			vmux.unlock();
			msleep(1);
			continue;
		}

		AVPacket* pkt = Pop();
		bool re = decode->Send(pkt);
		if (!re)
		{
			vmux.unlock();
			msleep(1);
			continue;
		}
		while (!isExit)
		{
			AVFrame* frame = decode->Recv();
			if (!frame) break;
			//��ʾ��Ƶ
			if (call)
			{
				call->Repaint(frame);
			}
		}
		vmux.unlock();
	}
}

xvideothread::xvideothread()
{

}

xvideothread::~xvideothread()
{

}

void xvideothread::setPause(bool isPause)
{
	vmux.lock();
	this->isPause = isPause;
	vmux.unlock();
}

bool xvideothread::RepaintPts(AVPacket* pkt, long long seekpts)
{
	vmux.lock();
		bool re = decode->Send(pkt);
		if (!re)
		{
			vmux.unlock();
			return true;  //��������
		}
		AVFrame *frame = decode->Recv();
		if (!frame)
		{
			vmux.unlock();
			return false;
		}
		//����λ��
		if (decode->pts >= seekpts)
		{
			if(call)
			{
				call->Repaint(frame);
			}					
			vmux.unlock();
			return true;
		}
		XFreeFrame(&frame);

		vmux.unlock();
	return false;
}
