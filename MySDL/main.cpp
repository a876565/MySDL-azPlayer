#include "azEngine.h"
#include "azView.h"
#include "azText.h"
#include <string>
#include <stdio.h>
#include <sstream>
#include "MyPlayer.h"
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

extern std::string MyPath;
void setupPath(const char*self)
{
	MyPath = self;
	size_t pos = MyPath.rfind(DIR_SEP);
	if (pos != std::string::npos)
	{
		MyPath.erase(pos);
		MyPath.append(DIR_SEPS);
	}
}
int main(int argc, char **argv)
{
	azEngine::InitLibs();
	ArgCount = argc;
	Args = argv;
	setupPath(argv[0]);
	MyPlayer mApp;
	mApp.run();
	azEngine::DeinitLibs();
	return 0;
}
