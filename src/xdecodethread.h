#pragma once

#include <QThread>
#include <mutex>
#include <list>

struct AVPacket;
class xdecode;

class xdecodethread:public QThread
{
  public:
    virtual void Push(AVPacket* pkt);
    //取出一帧数据，并出栈
    virtual AVPacket* Pop();

           //清理队列
    virtual void Clear();

           //清理资源，停止线程
    virtual void Close();

    xdecodethread();
    virtual ~xdecodethread();
    //最大队列
    int maxList = 100;
    bool isExit = false;

  protected:

    std::list<AVPacket*> packs;
    std::mutex mux;
    xdecode* decode = 0;
};

