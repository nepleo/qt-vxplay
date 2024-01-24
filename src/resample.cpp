#include "resample.h"
#include <iostream>

using namespace std;

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"

}

#pragma comment(lib, "swresample.lib")

bool resample::Open(AVCodecParameters *para, bool isClearPara)
{
    if (!para)
        return false;
    mux.lock();
    // 音频重采样 上下文初始化
    actx = swr_alloc();
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(2),              // 输出格式
                              (AVSampleFormat)outFormat,                     // 输出样本格式
                              para->sample_rate,                             // 输出采样率
                              av_get_default_channel_layout(para->channels), // 输入格式
                              (AVSampleFormat)para->format, para->sample_rate, 0, 0);
    if (isClearPara)
        avcodec_parameters_free(&para);
    int re = swr_init(actx);

    mux.unlock();
    if (re != 0)
    {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        cout << "swr_init  failed! :" << buf << endl;
        return false;
    }
    unsigned char *pcm = NULL;
    return true;
}

void resample::Close()
{
    mux.lock();
    if (actx)
    {
        swr_free(&actx);
    }
    mux.unlock();
}

int resample::xresample(AVFrame *indata, unsigned char *d)
{
    if (!indata)
    {
        return 0;
    }
    if (!d)
    {
        av_frame_free(&indata);
        return 0;
    }
    uint8_t *data[2] = {0};
    data[0] = d;
    int re = swr_convert(actx, data, indata->nb_samples,                    // 输出
                         (const uint8_t **)indata->data, indata->nb_samples // 输入
    );
    int outSize = re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);

    av_frame_free(&indata);

    if (re < 0)
        return re;
    return outSize;
}

resample::resample()
{
}

resample::~resample()
{
}
