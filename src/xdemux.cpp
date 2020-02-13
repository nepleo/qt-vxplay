#include "xdemux.h"
#include <iostream>

extern "C" {
#include "libavformat/avformat.h"
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")


using namespace std;

static double r2d(AVRational r)
{
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

bool xdemux::Open(const char* url)
{
	Close();
	//参数设置
	AVDictionary* opts = NULL;
	//设置rtsp流已tcp协议打开
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);

	//网络延时时间
	av_dict_set(&opts, "max_delay", "500", 0);


	mux.lock();
	int re = avformat_open_input(
		&ic,
		url,
		0,  // 0表示自动选择解封器
		&opts //参数设置，比如rtsp的延时时间
	);
	if (re != 0)
	{
		mux.unlock();
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! :" << buf << endl;
		return false;
	}
	cout << "open " << url << " success! " << endl;


	//获取流信息 
	re = avformat_find_stream_info(ic, 0);

	//总时长 毫秒
	this->totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMs = " << totalMs << endl;

	//打印视频流详细信息
	av_dump_format(ic, 0, url, 0);


	//获取视频流
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	AVStream* as = ic->streams[videoStream];
	cout << "<<===============================================================>>" << endl;
	cout << videoStream << "视频信息" << endl;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	width = as->codecpar->width;
	cout << "width=" << as->codecpar->width << endl;
	height = as->codecpar->height;
	cout << "height=" << as->codecpar->height << endl;
	//帧率 fps 分数转换
	cout << "video fps = " << r2d(as->avg_frame_rate) << endl;
	cout << "<<===============================================================>>" << endl;
	
	
	//获取音频流
	cout << "<<===============================================================>>" << endl;
	cout << audioStream << "音频信息" << endl;
	audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	as = ic->streams[audioStream];
	sampleRate = as->codecpar->sample_rate;
	channels = as->codecpar->channels;
	cout << "sample_rate = " << as->codecpar->sample_rate << endl;
	//AVSampleFormat;
	cout << "channels = " << as->codecpar->channels << endl;
	//一帧数据？？ 单通道样本数 
	cout << "frame_size = " << as->codecpar->frame_size << endl;
	//1024 * 2 * 2 = 4096  fps = sample_rate/frame_size
	cout << "<<===============================================================>>" << endl;
	mux.unlock();
	return true;
}

AVPacket* xdemux::Read()
{
	mux.lock();
	if (!ic)    //容错
	{
		mux.unlock();
		return false;
	}

	AVPacket* pkt = av_packet_alloc();
	//读取一帧
	int re = av_read_frame(ic, pkt);
	if (re != 0) 
	{
		mux.unlock();
		av_packet_free(&pkt);
		return 0;
	}
	//pts 转换成毫秒
	pkt->pts = pkt->pts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
	pkt->dts = pkt->dts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));

	mux.unlock();
	cout << pkt->pts << " " << flush;
	return pkt;
}

AVPacket* xdemux::ReadVideo()
{
	AVPacket* pkt = NULL;
	for (int i = 0; i < 20; i++)
	{
		pkt = Read();
		if (pkt->stream_index == videoStream)
		{
			break;
		}
		av_packet_free(&pkt);
	}
	return pkt;
}

bool xdemux::IsAudio(AVPacket* pkt)
{
	if(!pkt) return false;
	if (pkt->stream_index == videoStream)
		return false;
	return true;
}


//视频
AVCodecParameters *xdemux::CopyVPara()
{
	mux.lock();
	if (!ic) 
	{
		mux.unlock();
		return NULL;
	}

	AVCodecParameters* pa = avcodec_parameters_alloc();
	avcodec_parameters_copy(pa, ic->streams[videoStream]->codecpar);
	mux.unlock();
	return pa;

}
//音频
AVCodecParameters* xdemux::CopyAPara()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return NULL;
	}

	AVCodecParameters* pa = avcodec_parameters_alloc();
	avcodec_parameters_copy(pa, ic->streams[audioStream]->codecpar);
	mux.unlock();
	return pa;
}

//pos 0.0~1.0
bool xdemux::Seek(double pos)
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return false;
	}
	//清理 缓冲
	avformat_flush(ic);
	long long seekPos = 0;
	seekPos = ic->streams[videoStream]->duration * pos;
	//int ms = 3000; //三秒位置 根据时间基数（分数）转换
	//long long pos = (double)ms / (double)1000 * r2d(ic->streams[pkt->stream_index]->time_base);
	int re = av_seek_frame(ic, videoStream, seekPos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	mux.unlock();
	if (re < 0) return false;
	return true;
}

void xdemux::Clear()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return ;
	}
	avformat_flush(ic);
	mux.unlock();
}

void xdemux::Close()
{
	mux.lock();
	if (!ic)
	{
		mux.unlock();
		return;
	}
	avformat_close_input(&ic);
	totalMs = 0;

	mux.unlock();
	
}

xdemux::xdemux()
{
	static bool isFirst = true;
	static std::mutex dmux;
	dmux.lock();
	if (isFirst)
	{
	//初始化封装库
	av_register_all();

	//初始化网络库 （可以打开rtsp rtmp http 协议的流媒体视频）
	avformat_network_init();

	isFirst = false;
	}
	dmux.unlock();

}


xdemux::~xdemux()
{
	
}