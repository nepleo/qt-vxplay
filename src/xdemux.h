#pragma once
#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

// 分解复用,视频流和音频流
class xdemux
{
  public:
    xdemux();
    virtual ~xdemux();

    // 打开媒体文件 或者流媒体 rtmp http rstp
    virtual bool Open(const char *url);

    // 空间需要调用者释放,释放AVPacket 对象空间，和数据空间 av_packet_free
    virtual AVPacket *Read();

    // 只读视频
    virtual AVPacket *ReadVideo();

    virtual bool IsAudio(AVPacket *pkt);

    // 获取视频参数
    AVCodecParameters *CopyVPara();

    // 获取音频参数
    AVCodecParameters *CopyAPara();

    // seek 位置  0.0~1.0
    virtual bool Seek(double pos);

    virtual void Clear();

    virtual void Close();

    // 总时长,先设置为共有
    int totalMs = 0;
    int width = 0;
    int height = 0;
    int sampleRate = 0;
    int channels = 0;

  protected:
    std::mutex mux;
    // 解封装上下文
    AVFormatContext *ic = NULL;
    // 音视频索引，读取时区分音视频
    int videoStream = 0;
    int audioStream = 1;
};
