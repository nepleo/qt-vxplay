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
	//��������
	AVDictionary* opts = NULL;
	//����rtsp����tcpЭ���
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);

	//������ʱʱ��
	av_dict_set(&opts, "max_delay", "500", 0);


	mux.lock();
	int re = avformat_open_input(
		&ic,
		url,
		0,  // 0��ʾ�Զ�ѡ������
		&opts //�������ã�����rtsp����ʱʱ��
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


	//��ȡ����Ϣ 
	re = avformat_find_stream_info(ic, 0);

	//��ʱ�� ����
	this->totalMs = ic->duration / (AV_TIME_BASE / 1000);
	cout << "totalMs = " << totalMs << endl;

	//��ӡ��Ƶ����ϸ��Ϣ
	av_dump_format(ic, 0, url, 0);


	//��ȡ��Ƶ��
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	AVStream* as = ic->streams[videoStream];
	cout << "<<===============================================================>>" << endl;
	cout << videoStream << "��Ƶ��Ϣ" << endl;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	width = as->codecpar->width;
	cout << "width=" << as->codecpar->width << endl;
	height = as->codecpar->height;
	cout << "height=" << as->codecpar->height << endl;
	//֡�� fps ����ת��
	cout << "video fps = " << r2d(as->avg_frame_rate) << endl;
	cout << "<<===============================================================>>" << endl;
	
	
	//��ȡ��Ƶ��
	cout << "<<===============================================================>>" << endl;
	cout << audioStream << "��Ƶ��Ϣ" << endl;
	audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	as = ic->streams[audioStream];
	sampleRate = as->codecpar->sample_rate;
	channels = as->codecpar->channels;
	cout << "sample_rate = " << as->codecpar->sample_rate << endl;
	//AVSampleFormat;
	cout << "channels = " << as->codecpar->channels << endl;
	//һ֡���ݣ��� ��ͨ�������� 
	cout << "frame_size = " << as->codecpar->frame_size << endl;
	//1024 * 2 * 2 = 4096  fps = sample_rate/frame_size
	cout << "<<===============================================================>>" << endl;
	mux.unlock();
	return true;
}

AVPacket* xdemux::Read()
{
	mux.lock();
	if (!ic)    //�ݴ�
	{
		mux.unlock();
		return false;
	}

	AVPacket* pkt = av_packet_alloc();
	//��ȡһ֡
	int re = av_read_frame(ic, pkt);
	if (re != 0) 
	{
		mux.unlock();
		av_packet_free(&pkt);
		return 0;
	}
	//pts ת���ɺ���
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


//��Ƶ
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
//��Ƶ
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
	//���� ����
	avformat_flush(ic);
	long long seekPos = 0;
	seekPos = ic->streams[videoStream]->duration * pos;
	//int ms = 3000; //����λ�� ����ʱ�������������ת��
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
	//��ʼ����װ��
	av_register_all();

	//��ʼ������� �����Դ�rtsp rtmp http Э�����ý����Ƶ��
	avformat_network_init();

	isFirst = false;
	}
	dmux.unlock();

}


xdemux::~xdemux()
{
	
}