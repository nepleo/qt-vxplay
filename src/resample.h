#pragma once
#include <mutex>
struct AVCodecParameters;
struct AVFrame;
struct SwrContext;

class resample
{
public:
	virtual bool Open(AVCodecParameters* para,bool isClearPara = false);
	virtual void Close();
	//返回重采样后的大小
	virtual int xresample(AVFrame* indata, unsigned char* d);
	resample();
	~resample();

	//AV_SAMPLE_FMT_S16
	int outFormat = 1;

protected:
	std::mutex mux;
	SwrContext* actx = 0;
};

