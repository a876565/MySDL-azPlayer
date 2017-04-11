#pragma once
#include "device.h"
#include <deque>
#include <queue>
#include "DmMgr.h"
extern "C"{
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h" 
#include "libswresample/swresample.h"  
#include <libavutil\imgutils.h>
#include <libswscale\swscale.h>
}
enum {
	MEDIA_EVENT_FINISH=SDL_USEREVENT
};
const int  SDL_TIME_BASE=1000;
const  AVRational SDL_TIME_BASE_Q = { 1, SDL_TIME_BASE };

struct TimeSpan
{
	int hour, min, sec,msec;
	unsigned toTick()
	{
		return hour * 3600000 + min * 60000 + sec * 1000+msec;
	}
	void fromTick(unsigned tick)
	{
		hour = tick / 3600000;
		tick -= hour * 3600000;
		min = tick / 60000;
		tick -= min * 60000;
		sec = tick / 1000;
		msec = tick - sec * 1000;
	}
	void fromSecs(double s)
	{
		hour = s / 3600;
		s -= hour * 3600;
		min = s / 60;
		s -= min * 60;
		sec = s ;
		msec = (s - sec) * 1000;
	}
};
struct AudioData
{
	//音频数据
	Uint8*data;
	//数据长度
	unsigned len;
	//读取位置
	unsigned pos;
	//本段长度
	int64_t dur;
};
struct VideoData {
	int64_t pts;
	AVFrame *data;
};
struct AStream{
	int index;
	AVStream*Stream;
	AVCodecParameters*Codecpar;
	AVCodecContext *CodecCtx;//编码格式 
	AVCodec*Codec;
	SwrContext *SwrCtx;
	
};
enum DECODER_STATE {
	DECODER_END,
	DECODER_RUNNING,
	DECODER_STOP
};
enum {
	MEDIA_UNTREADY,
	MEDIA_STOP,
	MEDIA_FINISH,
	MEDIA_PLAYING,
	MEDIA_PAUSE
};
class Decoder {
public:
	int state;
	SDL_Thread* thread;
	SDL_mutex *mutex;
	SDL_cond*cond;
	Decoder() {
		state = DECODER_END;
		thread = NULL;
		mutex = SDL_CreateMutex();
		cond = SDL_CreateCond();
	}
	~Decoder() {
		if (thread)
		{
			state = DECODER_STOP;
			SDL_WaitThread(thread, NULL);
		}
		SDL_DestroyMutex(mutex);
		SDL_DestroyCond(cond);
	}
};
class MediaStream
{
	SDL_AudioSpec spec;
	SDL_AudioDeviceID dev;
	AVFormatContext *AvFmtCtx;//文件格式
	AStream audio;
	AStream video;
	SwsContext*swsCtx; 
	Uint32 tgtFmt;
	SDL_mutex *video_mu;

	SDL_mutex *AvMutex;

	Decoder audioDecoder;

	std::queue<AVPacket*>Pkts;
	std::queue<AudioData> AudioBuffer;
	std::queue<VideoData> VideoBuffer;
	unsigned max_frame_num;
	unsigned low_frame_num;
	int au_size_per_sample;

	unsigned viout_buffer_size;

	SDL_Renderer*render;//渲染器视频播放器用
	SDL_Texture*texture;

	Uint32 start_tick;
	int64_t play_pos;//现在播放的位置
	int play_state;

	int volume;

	SDL_Rect area;
	CDmMgr DmMgr;

	void AudioCallBack(Uint8*stream, int len);
	static void __cdecl AudioCallBack0(void*userdata,Uint8*stream, int len);
	static int __cdecl DecodeThread0(void*data);
public:
	static void initav();
	void init();
	int initAudio(int freq = 44100, int channels = 2, int samples = 1024);
	int deinitAudio();

	void OpenFile(const std::string& filename);
	void CloseFile();
	void SetAudio();
	void SetVideo();
	static void CloseStream(AStream&s);

	void StartDecode();
	void EndDecoder();
	void ReadFrame();
	void demux();
	void decodeAudio(AVPacket*pkt, AVFrame*frame);
	void decodeVideo(AVPacket*pkt);
	void getVideoSize(int *w, int *h);
	void setRenderer(SDL_Renderer*ren) { render = ren; if (!render&&texture)SDL_DestroyTexture(texture); };
	//注意：decode中更改会导致错误，必须先停止
	void update();
	void draw();
	void setDrawArea(SDL_Rect*r);
	void ClearQueue();

	void play();
	void pause(int p = 1);
	int getPlayState() { return play_state; }
	void stop();
	void setPostion(int tick);

	void setVolume(int vol) { if (vol >= 0 && vol <= 128)volume = vol; }
	int getVolume() { return volume; }
	int getState() { return SDL_GetAudioDeviceStatus(dev); }
	unsigned getPosition(){
		if (!audio.Stream)
			return 0;
	double pos = play_pos*av_q2d(audio.Stream->time_base);
	return (unsigned)(pos * 1000);
	}

	bool isReady() { return NULL!=audio.CodecCtx; }
	unsigned GetDuration();
	const char*GetMetaData(const char*key) {
		if (AvFmtCtx)
		{
			AVDictionaryEntry*t = av_dict_get(AvFmtCtx->metadata, key, NULL, 0);
			return t ? t->value : NULL;
		}
		return NULL;
	}
	MediaStream();
	~MediaStream();
};

