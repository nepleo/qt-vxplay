#include "xdecodethread.h"
#include "xdecode.h"

void xdecodethread::Push(AVPacket* pkt)
{
	if (!pkt)
		return;

	//阻塞
	while (!isExit)
	{
		mux.lock();
		if (packs.size() < maxList)
		{
			packs.push_back(pkt);
			mux.unlock();
			break;
		}
		mux.unlock();
		msleep(1);

	}
}

AVPacket* xdecodethread::Pop()
{
	mux.lock();
	if (packs.empty())
	{
		mux.unlock();
		return NULL;
	}

	AVPacket* pkt = packs.front();
	packs.pop_front();
	mux.unlock();

	return pkt;
}

void xdecodethread::Clear()
{
	mux.lock();
	decode->Clear();
	//清理队列
	while (!packs.empty())
	{
		AVPacket* pkt = packs.front();
		XFreePacket(&pkt);
		packs.pop_front();
	}
	mux.unlock();
}

void xdecodethread::Close()
{
	Clear();
	//等待线程退出
	isExit = true;
	wait();
	decode->Close();

	mux.lock();

	delete decode;
	decode = NULL;
	mux.unlock();

}

xdecodethread::xdecodethread()
{
	if (!decode) decode = new xdecode();
}

xdecodethread::~xdecodethread()
{
	//等待线程退出
	isExit = true;
	wait();
}
