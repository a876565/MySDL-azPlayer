#include "azEngine.h"
#include "azView.h"
#include "azText.h"
#include <string>
#include <stdio.h>
#include <sstream>
#include "MyMusic.h"
#include "AudioStream.h"

int ArgCount; 
char **Args;

void printrenflag(Uint32 flag)
{
	printf("RendererFlags:");
	if (flag & SDL_RENDERER_ACCELERATED)
		printf("ACCELERATED ");
	if (flag & SDL_RENDERER_SOFTWARE)
		printf("SOFTWARE ");
	if (flag & SDL_RENDERER_PRESENTVSYNC)
		printf("PRESENTVSYNC ");
	if (flag & SDL_RENDERER_TARGETTEXTURE)
		printf("TARGETTEXTURE ");
	printf("\n");
}


#ifdef _DEBUG
#undef main
#endif

#define TestAudioStream 0
void TestAudio()
{
	MediaStream a;
	SDL_Window*win = SDL_CreateWindow("TestAudioStream", 32, 32, 640, 640, 0);
	SDL_Event event;
	a.initAudio();
	a.OpenFile("test.mp3");
	//a.OpenFile("C:\\Users\\swa\\Videos\\Parakit & Alden Jacob - Save Me.mp4");
	a.play();
	while (SDL_WaitEvent(&event)>0)
	{
		if (event.type == SDL_QUIT)
			break;
	}
	a.deinitAudio();
	a.CloseFile();
	SDL_DestroyWindow(win);

}
int main(int argc, char **argv)
{
	azEngine::InitLibs();
	MediaStream::initav();
	ArgCount = argc;
	Args = argv;
#if TestAudioStream
	TestAudio();
	return 0;
#endif
	MyMusic mApp;
	mApp.run();
	azEngine::DeinitLibs();
	return 0;
}
