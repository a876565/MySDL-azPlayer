#pragma once
#include "device.h"
#include "libavcodec/avcodec.h"  
#include "libavformat/avformat.h" 
class AudioStream
{
	SDL_AudioSpec spec;
	SDL_AudioDeviceID dev;
	void AudioCallBack(Uint8*buf, int len);
public:
	int initAudio(int freq = 44100, int channels = 2, int samples = 4608);
	void OpenFile(std::string filename);
	AudioStream();
	~AudioStream();
};

