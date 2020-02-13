#pragma once
#include <mutex>

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
class xdemux
{
public:

	//��ý���ļ� ������ý�� rtmp http rstp 
	virtual bool Open(const char* url);

	//�ռ���Ҫ�������ͷ�,�ͷ�AVPacket ����ռ䣬�����ݿռ� av_packet_free
	virtual AVPacket *Read();

	//ֻ����Ƶ
	virtual AVPacket* ReadVideo();

	virtual bool IsAudio(AVPacket* pkt);

	//��ȡ��Ƶ����
	AVCodecParameters *CopyVPara();

	//��ȡ��Ƶ����
	AVCodecParameters *CopyAPara();

	//seek λ��  0.0~1.0
	virtual bool Seek(double pos);

	virtual void Clear();

	virtual void Close();

	xdemux();
	virtual ~xdemux();
	//��ʱ��
	int totalMs = 0;
	int width = 0;
	int height = 0;
	int sampleRate = 0;
	int channels = 0;

protected:
	std::mutex mux;
	//���װ������
	AVFormatContext *ic = NULL;
	//����Ƶ��������ȡʱ��������Ƶ
	int videoStream = 0;
	int audioStream = 1;

};

