#pragma once
// 解码和显示视频

#include "ivideocall.h"
#include "xdecodethread.h"
#include <Qthread>
#include <list>
#include <mutex>

struct AVPacket;
struct AVCodecParameters;
class xdecode;

class xvideothread : public xdecodethread
{
  public:
    virtual bool Open(AVCodecParameters *para, ivideocall *call, int width, int height);
    void run();

    xvideothread();
    virtual ~xvideothread();
    bool isPause = false;
    void setPause(bool isPause);
    // 同步时间由外部传入
    long long synpts = 0;
    // 解码pts,如果接收到的解码数据pts>= seekpts ,return true,并且显示画面
    virtual bool RepaintPts(AVPacket *pkt, long long seekpts);

  protected:
    std::mutex vmux;
    ivideocall *call;
};
