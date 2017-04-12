#include "MediaStream.h"
#define SAFE_FREE(p,f) if(p)f(p);p=nullptr;
//!!warning:not prepared for multi-thread




Uint32 AvPixFmtToSDL(AVPixelFormat fmt)
{
	switch (fmt)
	{
	case AV_PIX_FMT_YUV420P:
		return SDL_PIXELFORMAT_IYUV;
	case AV_PIX_FMT_YUYV422:
		return SDL_PIXELFORMAT_YUY2;
	case AV_PIX_FMT_YVYU422:
		return SDL_PIXELFORMAT_YVYU;
	case AV_PIX_FMT_UYVY422:
		return SDL_PIXELFORMAT_UYVY;
	default:
		return 0;
	}
}


const char* av_errstr(int errnum)
{
	static char errs[AV_ERROR_MAX_STRING_SIZE];
	return av_make_error_string(errs, AV_ERROR_MAX_STRING_SIZE, errnum);
}
static int d_print_avdict(AVDictionary *dict)
{

	int dict_num = av_dict_count(dict);
	AVDictionaryEntry*t = NULL;
	DBGLOG("dict_num:%d", dict_num);
	DBGLOG("dict_element:");
	while (t = av_dict_get(dict, "", t, AV_DICT_IGNORE_SUFFIX))
	{
		DBGLOG("\t[key:%10s | value:%s]", t->key, t->value);
	}
	return dict_num;
}
static void d_print_audio(SDL_AudioDeviceID dev)
{
	const char*device_name = SDL_GetAudioDeviceName(dev, 0);
	const char*driver_name = SDL_GetAudioDriver(dev);
	if (NULL == device_name)
		device_name = "Unknown";
	if (NULL == driver_name)
		driver_name = "Unknown";
	/*DBGLOG("Audio:device=%s,driver=%s",
		device_name, driver_name
	);*/
}


bool av_initialized = false;
void MediaStream::initav()
{
	av_register_all();
	avcodec_register_all();
	avformat_network_init();
}


int MediaStream::initAudio(int freq, int channels, int samples)
{
	SDL_AudioSpec want;

	SDL_memset(&want, 0, sizeof(want)); /* or SDL_zero(want) */
	want.silence = 0;
	want.freq = freq;
	want.format = AUDIO_S16;
	want.channels = 2;
	want.samples = 1024;
	want.callback = (SDL_AudioCallback)(&AudioCallBack0);
	want.userdata = this;

	dev = SDL_OpenAudioDevice(NULL, 0, &want, &spec, 0);
	if (dev == 0) {
		DBGLOGEX(Error, "Failed to open audio: %s", SDL_GetError());
		return -1;
	}
	else {
		if (spec.format != want.format)
		{
			/* we let this one thing change. */
			DBGLOGEX(Error, "We didn't get want audio format.");
			SDL_CloseAudioDevice(dev);
			dev = 0;
			return -1;
		}
		d_print_audio(dev);
	}

	return 0;
}
int MediaStream::deinitAudio()
{
	if (dev)
	{
		SDL_CloseAudioDevice(dev);
		dev = 0;
	}
	return 0;
}
void MediaStream::OpenFile(const std::string& url)
{
	int ret;
	CloseFile();

	if (!av_initialized)
		initav();

	play_pos = 0;
	AvFmtCtx = avformat_alloc_context();
	//Open  
	ret =avformat_open_input(&AvFmtCtx, url.c_str(), NULL, NULL);
	if (ret != 0) {
		DBGLOGEX(Error, "Couldn't open input stream.%s\n", av_errstr(ret));
		SDL_SetError(av_errstr(ret));
		avformat_close_input(&AvFmtCtx);
		return;
	}
	// Retrieve stream information  
	ret=avformat_find_stream_info(AvFmtCtx, NULL);
	if (ret < 0) {
		DBGLOGEX(Error, "Couldn't find stream information. %s\n",av_errstr(ret));
		SDL_SetError(av_errstr(ret));
		avformat_close_input(&AvFmtCtx);
		return;
	}
	av_dump_format(AvFmtCtx, 0, url.c_str(), 0);
	// Find the first audio stream  
	audio.index = -1;
	for (unsigned i = 0; i < AvFmtCtx->nb_streams; i++)
	{
		if (AvFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio.index = i;
		}
		if (AvFmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video.index = i;
		}
	}
	SetAudio();
	SetVideo();
	if (audio.index == -1) {
		DBGLOG("Didn't find a audio stream.\n");
		SDL_SetError("Didn't find a audio stream.");
		return;
	}

	StartDecode();
}

void MediaStream::CloseFile()
{

	SDL_PauseAudioDevice(dev, 1);
	EndDecoder();
	ClearQueue();
	CloseStream(audio);
	CloseStream(video);
	if (AvFmtCtx)
		avformat_close_input(&AvFmtCtx);
}

void MediaStream::SetAudio()
{
	int ret;
	audio.Stream = AvFmtCtx->streams[audio.index];
	// Get a pointer to the codec context for the audio stream  
	audio.Codecpar = audio.Stream->codecpar;

	// Find the decoder for the audio stream  
	audio.Codec = avcodec_find_decoder(audio.Codecpar->codec_id);
	if (audio.Codec == NULL) {
		CloseStream(audio);
		DBGLOGEX(Error, "Codec not found.");
		return;
	}
	// Open codec  
	audio.CodecCtx = avcodec_alloc_context3(audio.Codec);
	avcodec_parameters_to_context(audio.CodecCtx, audio.Codecpar);
	ret = avcodec_open2(audio.CodecCtx, audio.Codec, NULL);
	if (ret < 0) {
		CloseStream(audio);
		DBGLOGEX(Error, "Could not open codec.");
		SDL_SetError(av_errstr(ret));
		return;
	}

	//Out Audio Param  
	int64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
	AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
	int out_channels = 2;
	int out_sample_rate = spec.freq;
	int out_samples = spec.samples;

	int in_samples = audio.CodecCtx->frame_size;
	int in_channels = av_get_channel_layout_nb_channels(out_channel_layout);
	int64_t in_channel_layout = av_get_default_channel_layout(audio.CodecCtx->channels);
	au_size_per_sample = av_get_bytes_per_sample(out_sample_fmt);

	// 检查是否需要转换
	if (audio.CodecCtx->sample_rate == out_sample_rate&&
		in_channels == out_channels&&
		audio.CodecCtx->sample_fmt == out_sample_fmt)
	{
		audio.SwrCtx = NULL;
		return;
	}
	

	//初始化转换
	audio.SwrCtx = swr_alloc();
	audio.SwrCtx = swr_alloc_set_opts(
		audio.SwrCtx,
		out_channel_layout,
		out_sample_fmt,
		out_sample_rate,
		in_channel_layout,
		audio.CodecCtx->sample_fmt,
		audio.CodecCtx->sample_rate,
		0, NULL);
	swr_init(audio.SwrCtx);
}

void MediaStream::SetVideo()
{
	int ret;

	video.Stream = AvFmtCtx->streams[video.index];
	// Get a pointer to the codec context for the video stream  
	video.Codecpar = video.Stream->codecpar;

	// Find the decoder for the video stream  
	video.Codec = avcodec_find_decoder(video.Codecpar->codec_id);
	if (video.Codec == NULL) {
		CloseStream(video);
		DBGLOGEX(Error, "Codec not found.");
		return;
	}
	// Open codec  
	video.CodecCtx = avcodec_alloc_context3(video.Codec);
	avcodec_parameters_to_context(video.CodecCtx, video.Codecpar);
	ret = avcodec_open2(video.CodecCtx, video.Codec, NULL);
	if (ret < 0) {
		CloseStream(video);
		DBGLOGEX(Error, "Could not open codec.");
		SDL_SetError(av_errstr(ret));
		return;
	}

	tgtFmt = AvPixFmtToSDL(video.CodecCtx->pix_fmt);
	if(tgtFmt)
		texture = SDL_CreateTexture(render, 
				tgtFmt,
				SDL_TEXTUREACCESS_STREAMING, 
				video.CodecCtx->width, video.CodecCtx->height);
	if (texture == nullptr)
	{
		DBGLOG("VIDEO:%s", SDL_GetError());
		swsCtx = sws_getCachedContext(swsCtx, video.CodecCtx->width, video.CodecCtx->height, video.CodecCtx->pix_fmt,
			video.CodecCtx->width, video.CodecCtx->height, AV_PIX_FMT_YUV420P,
			0, NULL, NULL, NULL);
		tgtFmt = SDL_PIXELFORMAT_YV12;
	}
	else
	{
		return;
	}
	texture = SDL_CreateTexture(render,
		tgtFmt,
		SDL_TEXTUREACCESS_STREAMING,
		video.CodecCtx->width, video.CodecCtx->height);
}

void MediaStream::CloseStream(AStream & s)
{
	s.index = -1;
	if (s.SwrCtx)
	{
		swr_close(s.SwrCtx);
		s.SwrCtx = NULL;
	}
	if (s.CodecCtx)
	{
		avcodec_free_context(&s.CodecCtx);
	}
	s.Codec = NULL;
	s.Stream = NULL;
	s.Codecpar = NULL;
}

void MediaStream::StartDecode()
{
	audioDecoder.state = DECODER_RUNNING;
	audioDecoder.thread = SDL_CreateThread(&DecodeThread0, "decoder", this);
	//ReadFrame();
}

void MediaStream::EndDecoder()
{
	if (audioDecoder.thread)
	{
		audioDecoder.state = DECODER_STOP;
		SDL_CondSignal(audioDecoder.cond);
		SDL_WaitThread(audioDecoder.thread, NULL);
		audioDecoder.thread = NULL;
	}
}

void MediaStream::ReadFrame()
{
	AVPacket pkt;
	AVFrame *audioFrame=NULL;
	uint32_t len = 0;
	int ret;
	//初始化frame outbuffer
	av_init_packet(&pkt);
	if(audio.index>=0)
		audioFrame = av_frame_alloc();
	while (1)
	{
		if (audioDecoder.state != DECODER_RUNNING)
			break;
		SDL_LockMutex(AvMutex);
		ret = av_read_frame(AvFmtCtx, &pkt);
		SDL_UnlockMutex(AvMutex);

		if (ret == AVERROR_EOF)
		{
			DBGLOG("Decode end");
			break;
		}
		else if (ret < 0)
		{
			DBGLOGEX(Error, "Error in av_read_frame.");
			break;
		}
		if (pkt.stream_index == audio.index)
		{
			decodeAudio(&pkt,audioFrame);
		}
		else if (pkt.stream_index == video.index)
		{
			decodeVideo(&pkt);
		}
		av_packet_unref(&pkt);
		if (AudioBuffer.size() >= max_frame_num)
		{
			SDL_CondWait(audioDecoder.cond, NULL);
		}
	}
	if(audioFrame)
		av_frame_free(&audioFrame);
	//DBGLOG("Frames:%d", AudioBuffer.size());
	audioDecoder.state = DECODER_END;
	if (audio.SwrCtx)
		swr_free(&audio.SwrCtx);
	if (swsCtx)
	{
		sws_freeContext(swsCtx);
		swsCtx = NULL;
	}
}

void MediaStream::demux()
{
}

void MediaStream::decodeAudio(AVPacket*pkt,AVFrame*frame)
{
	int ret;

	ret = avcodec_send_packet(audio.CodecCtx, pkt);
	if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
	{
		DBGLOGEX(Error, "Error in decoding audio frame.");
		return;
	}
	while (1)
	{

		ret = avcodec_receive_frame(audio.CodecCtx, frame);
		if (ret == AVERROR(EAGAIN))
		{
			break;
		}
		else if (ret < 0)
		{

			DBGLOGEX(Error, "Error in decoding audio frame.");
			break;
		}
		Uint8*outbuffer = NULL; 
		if (audio.SwrCtx)
		{
			int au_out_samples = swr_get_out_samples(audio.SwrCtx, frame->nb_samples);
			outbuffer=(uint8_t *)av_malloc(spec.channels*au_size_per_sample*au_out_samples);
			ret = swr_convert(audio.SwrCtx, &outbuffer, au_out_samples,
				(const uint8_t **)frame->data, frame->nb_samples);
		}
		else
		{
			outbuffer = (Uint8*)av_malloc(frame->linesize[0]);
			memcpy(outbuffer, frame->data[0], frame->linesize[0]);
			ret = frame->nb_samples;
		}

		AudioData au;
		au.data = outbuffer;
		au.len = ret*spec.channels*au_size_per_sample;
		au.pos = 0;
		au.dur = av_frame_get_pkt_duration(frame);


		SDL_LockMutex(audioDecoder.mutex);
		AudioBuffer.push(au);
		SDL_UnlockMutex(audioDecoder.mutex);
	}
}

void MediaStream::decodeVideo(AVPacket*pkt)
{
	int ret;
	ret = avcodec_send_packet(video.CodecCtx, pkt);
	if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
	{
		DBGLOGEX(Error, "Error in decoding video frame.");
		return;
	}
	while (1)
	{
		AVFrame*frame =av_frame_alloc();
		AVFrame*frameYUV = NULL;

		ret = avcodec_receive_frame(video.CodecCtx, frame);
		if (ret == AVERROR(EAGAIN))
		{
			av_frame_free(&frame);
			break;
		}
		else if (ret < 0)
		{

			DBGLOGEX(Error, "Error in decoding audio frame.%s",av_errstr(ret));
			break;
		}

		VideoData vd;
		if (swsCtx)
		{
			frameYUV = av_frame_alloc();
			sws_scale(swsCtx, frame->data, frame->linesize, 0, video.Codecpar->height,
				frameYUV->data, frameYUV->linesize);
			vd.data = frameYUV;
			av_frame_free(&frame);
		}
		else
		{
			vd.data = frame;
		}
		vd.pts=av_rescale_q(vd.data->pts, video.Stream->time_base, audio.Stream->time_base);
		SDL_LockMutex(video_mu);
		VideoBuffer.push(vd);
		SDL_UnlockMutex(video_mu);
	}

}

void MediaStream::getVideoSize(int * w, int * h)
{
	if (video.CodecCtx)
	{
		if (w)
			*w = video.CodecCtx->width;
		if (h)
			*h = video.CodecCtx->height;
	}
}

void MediaStream::update()
{
	SDL_LockMutex(video_mu);
	if (!VideoBuffer.empty())
	{
		auto&vd = VideoBuffer.front();
		if (vd.pts <= play_pos)
		{
			int a=SDL_UpdateYUVTexture(texture, NULL,
				vd.data->data[0], vd.data->linesize[0],
				vd.data->data[1], vd.data->linesize[1],
				vd.data->data[2], vd.data->linesize[2]);
			av_frame_free(&vd.data);
			VideoBuffer.pop();
		}
	}
	SDL_UnlockMutex(video_mu);

}

void MediaStream::draw()
{
	if(texture&&render)
		SDL_RenderCopy(render, texture, NULL, NULL);
}

void MediaStream::setDrawArea(SDL_Rect * r)
{
	area = *r;
}

void MediaStream::ClearQueue()
{
	while (AudioBuffer.size() > 0)
	{
		auto&t = AudioBuffer.front();
		av_free(t.data);
		AudioBuffer.pop();
	}
	while (VideoBuffer.size() > 0)
	{
		auto&t = VideoBuffer.front();
		av_frame_free(&t.data);
		VideoBuffer.pop();
	}
}

void MediaStream::play()
{
	if (audioDecoder.state != DECODER_RUNNING)
		StartDecode();
	SDL_PauseAudioDevice(dev, 0);
	play_state = MEDIA_PLAYING;
}

void MediaStream::pause(int p) 
{ 
	SDL_PauseAudioDevice(dev, p);
	play_state = p ? MEDIA_PAUSE : MEDIA_PLAYING; 
}

void MediaStream::stop() 
{
	play_state = MEDIA_STOP;
	SDL_PauseAudioDevice(dev, 1);
	EndDecoder();
}

void MediaStream::setPostion(int tick)
{
	SDL_LockMutex(AvMutex);
	SDL_LockMutex(video_mu);
	SDL_LockMutex(audioDecoder.mutex);
	ClearQueue();
	int64_t t = av_rescale_q(tick, SDL_TIME_BASE_Q, audio.Stream->time_base);
	av_seek_frame(AvFmtCtx, audio.index, t, AVSEEK_FLAG_BACKWARD);
	SDL_UnlockMutex(AvMutex);
	SDL_UnlockMutex(video_mu);
	SDL_UnlockMutex(audioDecoder.mutex);
}

unsigned MediaStream::getPosTicks() 
{
	
	return (unsigned)(getPosition() * 1000);
}

unsigned MediaStream::getDurTicks()
{
	
	return (unsigned)(getDuration() * 1000);
}

double MediaStream::getPosition()
{
	if (!audio.Stream)
		return 0;
	return play_pos*av_q2d(audio.Stream->time_base);
}

double MediaStream::getDuration()
{
	if (!audio.Stream)
		return 0;
	return audio.Stream->duration*av_q2d(audio.Stream->time_base);
}

void MediaStream::AudioCallBack(Uint8 * stream, int size)
{
	SDL_memset(stream, 0, size);
	if (AudioBuffer.size() <= low_frame_num)
	{
		if (AudioBuffer.size() == 0 && audioDecoder.state == DECODER_END)
		{
			play_state = MEDIA_FINISH;
			SDL_Event au_event;
			au_event.type = MEDIA_EVENT_FINISH;
			SDL_PushEvent(&au_event);
			SDL_PauseAudioDevice(dev, 1);
		}
		SDL_CondSignal(audioDecoder.cond);
	}
	while (size > 0 && AudioBuffer.empty() == false)
	{
		SDL_LockMutex(audioDecoder.mutex);
		auto&audata = AudioBuffer.front();
		int len = (size > (audata.len - audata.pos) ? audata.len - audata.pos : size);
		/*  Mix  as  much  data  as  possible*/
		SDL_MixAudioFormat(stream, audata.data + audata.pos, AUDIO_S16LSB, len, volume);
		play_pos += audata.dur*len / audata.len;
		audata.pos += len;
		size -= len;
		stream += len;
		if (audata.pos == audata.len)
		{
			av_free(audata.data);
			AudioBuffer.pop();

			if (AudioBuffer.size() <= low_frame_num)
			{
				SDL_CondSignal(audioDecoder.cond);
			}
		}
		SDL_UnlockMutex(audioDecoder.mutex);
	}
}

void MediaStream::AudioCallBack0(void * userdata, Uint8 * stream, int len)
{
	((MediaStream*)userdata)->AudioCallBack(stream, len);
}

int MediaStream::DecodeThread0(void * data)
{
	((MediaStream*)data)->ReadFrame();
	return 0;
}

const char * MediaStream::GetMetaData(const char * key) {
	if (AvFmtCtx)
	{
		AVDictionaryEntry*t = av_dict_get(AvFmtCtx->metadata, key, NULL, 0);
		return t ? t->value : NULL;
	}
	return NULL;
}

MediaStream::MediaStream() :AvFmtCtx(NULL), low_frame_num(8),
max_frame_num(32), dev(0), volume(SDL_MIX_MAXVOLUME), swsCtx(nullptr)
{
	SDL_zero(spec);
	SDL_zero(audio);
	SDL_zero(video);
	audio.index = video.index = -1;
	AvMutex = SDL_CreateMutex();
	video_mu = SDL_CreateMutex();
}


MediaStream::~MediaStream()
{
	CloseFile();
}

/*

double dur = AvFmtCtx->streams[i]->duration*av_q2d(AvFmtCtx->streams[i]->time_base);
int hor = (int)(dur / 3600);
int min = (int)(dur / 60 - hor * 60);
int sec = (int)(dur - min * 60 - hor * 3600);
DBGLOG("Duration of %s is %lf,%02d:%02d:%02d", url.c_str(), dur,
hor, min, sec
);
d_print_avdict(AvFmtCtx->metadata);

void MediaStream::AudioCallBack(Uint8 * stream, int size)
{
SDL_memset(stream, 0, size);
if (AudioBuffer.size() <= low_frame_num)
{
SDL_CondSignal(audioDecoder.cond);
}
while (size > 0 && AudioBuffer.empty() == false)
{
SDL_LockMutex(audioDecoder.mutex);
auto&audata = AudioBuffer.front();
int len = (size > (audata.len - audata.pos) ? audata.len - audata.pos : size);
/*  Mix  as  much  data  as  possible
SDL_MixAudioFormat(stream, audata.data, AUDIO_S16LSB, len, volume);
play_pos += audata.dur*len / audata.len;
audata.pos += len;
size -= len;
stream += len;
if (audata.pos == audata.len)
{
	av_free(audata.data);
	AudioBuffer.pop();

	if (AudioBuffer.size() <= low_frame_num)
	{
		SDL_CondSignal(audioDecoder.cond);
	}
}
SDL_UnlockMutex(audioDecoder.mutex);
	}
}
*/