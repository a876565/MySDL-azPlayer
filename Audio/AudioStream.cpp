#include "AudioStream.h"


int AudioStream::initAudio(int freq,int channels,int samples)
{
	av_register_all();

	SDL_AudioSpec want;

	SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	want.freq = freq;
	want.format = AUDIO_F32;
	want.channels = 2;
	want.samples = 4096;
	want.callback = nullptr; /* you wrote this function elsewhere. */

	dev = SDL_OpenAudioDevice(NULL, 0, &want, &spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	if (dev == 0) {
		DBGLOGEX(Error,"Failed to open audio: %s", SDL_GetError());
		return -1;
	}
	else {
		if (spec.format != want.format) { /* we let this one thing change. */
			DBGLOGEX(Error, "We didn't get want audio format.");
			SDL_CloseAudioDevice(dev);
			dev = 0;
			return -1;
		}
	}

	return 0;
}

void AudioStream::OpenFile(std::string filename)
{
	AVFormatContext *pInFmtCtx = NULL;//文件格式
	AVCodecContext *pInCodecCtx = NULL;//编码格式 

	if (avformat_open_input(&pInFmtCtx, filename.c_str(), NULL, NULL) != 0)//获取文件格式
		printf("av_open_input_file error\n");
	if (avformat_find_stream_info(pInFmtCtx,NULL) < 0)//获取文件内音视频流的信息
		printf("av_find_stream_info error\n");

}

void AudioStream::AudioCallBack(Uint8 * buf, int len)
{
}

AudioStream::AudioStream()
{
}


AudioStream::~AudioStream()
{
}
