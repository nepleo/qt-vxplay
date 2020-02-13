#pragma once
struct AVPacket;
class xdecode;
#include <QThread>
#include <mutex>
#include <list>
class xdecodethread:public QThread
{
public:
	virtual void Push(AVPacket* pkt);
	//ȡ��һ֡���ݣ�����ջ
	virtual AVPacket* Pop();

	//�������
	virtual void Clear();

	//������Դ��ֹͣ�߳�
	virtual void Close();

	xdecodethread();
	virtual ~xdecodethread();
	//������
	int maxList = 100;
	bool isExit = false;

protected:

	std::list<AVPacket*> packs;
	std::mutex mux;
	xdecode* decode = 0;
};

