#pragma once
class xaudioplay
{
public:
    int sampleRate = 44100;
    int sampleSize = 16;
    int channels = 2;
    //����Ƶ����
    virtual bool Open() = 0;
    virtual void Close() = 0;
    virtual void Clear() = 0;
    virtual long long GetNoPlayMs() = 0;
    //������Ƶ
    virtual bool Write(const unsigned char* data, int datasize) = 0;
    virtual int GetFree() = 0;
    static xaudioplay* Get();
    virtual void setPause(bool isPause) = 0;
    xaudioplay();
    virtual ~xaudioplay();
};

