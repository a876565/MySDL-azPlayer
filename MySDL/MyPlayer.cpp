#include "MyPlayer.h"
#include <fstream>
#include <io.h>
#include "tinyxml2.h"


extern int ArgCount;
extern char **Args;
int is_suffix_file(const char*name,const char*suffix[])
{
	const char*p = name;
	const char*dot = nullptr;
	while (*p)
	{
		if (*p == '.')
			dot = p+1;
		p++;
	}
	if (dot == nullptr)
		return 0;

	for (int i=0;suffix[i];i++)
	{
		if (!_stricmp(dot, suffix[i]))
			return i+1;
	}
	return 0;
}

void setupDanmaku(const std::string&name, CDmMgr&dm)
{
	auto p = name.rfind('.');
	dm.OpenFile(name.substr(0, p) + ".xml");
}


void MyPlayer::init()
{
	azApp::init();
	setlocale(LC_ALL, "");

	engine->set_title("azMusic");
	engine->BlendMode(SDL_BLENDMODE_BLEND);
	engine->SetFpsLimit(60.0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

	danmaku.init();
	media.initAudio();
	media.setRenderer(engine->getRenderer());
	if (ArgCount > 1)
	{
		media.OpenFile(Args[1]);
		DBGLOG("Open media:%s", Args[1]);
		if (ArgCount > 2)
		{
			danmaku.OpenFile(Args[2]);
		}
		else
		{;
			setupDanmaku(Args[1], danmaku);
		}
		start_tick = SDL_GetTicks();
		media.play();
	}
	else
		endApp();
	initui();
}

void MyPlayer::destroy()
{
	media.deinitAudio();
	media.CloseFile();
	danmaku.Close();
	azApp::destroy();
}

void MyPlayer::onEvent(SDL_Event & event)
{
	switch (event.type)
	{
	case SDL_WINDOWEVENT:
		if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			danmaku.setArea(&engine->area());
		break;
	case SDL_QUIT:
		endApp();
		break;
	case SDL_KEYUP:
		if(event.key.keysym.sym==SDLK_F5)
			media.pause(media.getPlayState()==MEDIA_PLAYING?1:0);
		if (event.key.keysym.sym == SDLK_RETURN)
			engine->setFullScreen(-1);
		if (event.key.keysym.sym == SDLK_ESCAPE)
			endApp();
		break;
	case MEDIA_EVENT_FINISH:
		media.CloseFile();
		danmaku.Close();
		break;
	default:
		break;
	}
}

void MyPlayer::onDraw()
{
	media.update();
	media.draw();
	danmaku.pass((double)(SDL_GetTicks()-start_tick)/1000);
}


void MyPlayer::mainui()
{

}


void azApp::run()
{

	running = 1;
	init();

	eventloop();

	destroy();
}

void azApp::endApp()
{
	running = 0;
}

void azApp::init()
{
	engine = new azEngine();
	engine->init();

}

void azApp::initui()
{
	ui = new azPage(engine);
	ui->setPos(0, 0);
	ui->setSize(engine->area().w, engine->area().h);
}

void azApp::destroy()
{
	running = 0;
	engine->deinit();
	delete engine;
	delete ui;
}


void azApp::onDraw()
{
}

void azApp::onEvent(SDL_Event & event)
{
}

void azApp::onTick()
{
}

void azApp::eventloop()
{
	SDL_Event event;
	while (running)
	{
		engine->RenderClear();
		onDraw();
		ui->draw();
		engine->RenderFresh();
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					engine->winsized();
				}if (event.window.event == SDL_WINDOWEVENT_MOVED)
				{
					engine->winmoved();
				}
				break;
			default:
				break;
			}
			
			ui->onEvent(event);
			onEvent(event);
		}
		engine->Tick();
		onTick();
	}
}
