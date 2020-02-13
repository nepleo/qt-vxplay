#include "resample.h"
#include <iostream>
using namespace std;
extern "C" {
#include "libswresample/swresample.h"
#include "libavcodec/avcodec.h"
}

#pragma comment(lib,"swresample.lib")

bool resample::Open(AVCodecParameters *para, bool isClearPara)
{
	if (!para) return false;
	mux.lock();
	//��Ƶ�ز��� �����ĳ�ʼ��
	actx = swr_alloc();
	actx = swr_alloc_set_opts(actx,
		av_get_default_channel_layout(2),	//�����ʽ
		(AVSampleFormat)outFormat,					//���������ʽ
		para->sample_rate,					//���������
		av_get_default_channel_layout(para->channels),//�����ʽ
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, 0
	);
	if(isClearPara)
		avcodec_parameters_free(&para);
	int re = swr_init(actx);
	
	mux.unlock();
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "swr_init  failed! :" << buf << endl;
		return false;
	}
	unsigned char* pcm = NULL;
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

int resample::xresample(AVFrame* indata, unsigned char* d)
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
	uint8_t* data[2] = { 0 };
	data[0] = d;
	int re = swr_convert(actx,
		data, indata->nb_samples,		//���
		(const uint8_t**)indata->data, indata->nb_samples	//����
	);
	int outSize =  re * indata->channels * av_get_bytes_per_sample((AVSampleFormat)outFormat);

	av_frame_free(&indata);

	if (re < 0) return re;
	return outSize;
}

resample::resample()
{
}

resample::~resample()
{
}
