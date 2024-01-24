#include "xaudiothread.h"
#include "xdecode.h"
#include "xaudioplay.h"
#include "resample.h"

extern "C" {
#include"libavcodec/avcodec.h"
}

bool xaudiothread::Open(AVCodecParameters* para, int sampleRate, int channels)
{
    if (!para)return false;
    Clear();
    amux.lock();
    pts = 0;

    bool re = true;
    if (!res->Open(para, false))
    {
        re = false;
        //mux.unlock();
        //return false;
    }
    ap->sampleRate =  sampleRate;
    ap->channels = channels;
    if (!ap->Open())
    {
        re = false;
        //mux.unlock();
        //return false;
    }
    if (!decode->Open(para))
    {
        re = false;
    }

    amux.unlock();
    return re;
}

void xaudiothread::run()
{
    unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
    while (!isExit)
    {
        amux.lock();

        if (isPause)
        {
            amux.unlock();
            msleep(5);
            continue;
        }

               //没有数据
               //if (packs.empty() || !decode ||!res || !ap)
               //{
               //	amux.unlock();
               //	msleep(1);
               //	continue;
               //}
               //AVPacket* pkt = packs.front();
               //packs.pop_front();

        AVPacket* pkt = Pop();
        bool re = decode->Send(pkt);
        if (!re)
        {
            amux.unlock();
            msleep(1);
            continue;
        }
        while (!isExit)
        {
            AVFrame* frame = decode->Recv();
            if (!frame) break;
            pts = decode->pts - ap->GetNoPlayMs();
            //重采样
            int size = res->xresample(frame, pcm);
            //播放音频
            while (!isExit)
            {
                if (size <= 0) break;
                //缓冲未播放完
                if (ap->GetFree() < size || isPause)
                {
                    msleep(1);
                    continue;
                }
                ap->Write(pcm, size);
                break;
            }
        }
        amux.unlock();
    }
    delete pcm;
}

void xaudiothread::Close()
{
    xdecodethread::Close();
    if (res)
    {
        res->Close();
        amux.lock();
        delete res;
        res = NULL;
        amux.unlock();
    }
    if (ap)
    {
        ap->Close();
        amux.lock();
        ap = NULL;
        amux.unlock();

    }
}

void xaudiothread::Clear()
{
    xdecodethread::Clear();
    mux.lock();
    if (ap) ap->Clear();
    mux.unlock();
}

xaudiothread::xaudiothread()
{
    if (!res) res = new resample();
    if (!ap) ap = xaudioplay::Get();
}

xaudiothread::~xaudiothread()
{
    //等待线程退出
    isExit = true;
    wait();
}

void xaudiothread::setPause(bool isPause)
{
    //amux.lock();
    this->isPause = isPause;
    if (ap)
        ap->setPause(isPause);
    //amux.unlock();
}
